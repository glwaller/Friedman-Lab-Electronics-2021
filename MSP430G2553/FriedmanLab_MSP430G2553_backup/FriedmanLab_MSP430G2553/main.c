/*
 * main.c
 *
 *  Created on: Mar 23, 2021
 *      Author: Raylene
 */

/****************************************************************************************************
 * MSP430G2553 Rodent Arena Test Sub-controller (RATS)
 *
 * Description: Proof of concept rodent arena controller for Friedman Lab rodent decision-making tasks.
 *  Communicates microcontroller with serial COM client running Windows 10 and connects to Noldus' TTL
 *  IO-Box. Microcontroller will receive commands from client called by Ethovision XT 15 and
 *  communicate back with TTL pulses if command fails to execute (?).
 *
 *												  -------->| Inscopix TTL2_IN        |
 *												 |	 ----->| Inscopix TTL1_IN		 |
 *												 |  |		-------------------------
 *												 |  |
 *                      MSP430G2553              |  |            Noldus IO Box
 *             -------------------------------   |  |       -------------------------
 *            |                               |  |  |      |                         |
 *           <| P1.0 (LED1)      P2.2 (GPIO)  |--|--o----->| TTL1_IN                 |
 *   KiTTY--->| P1.1 (UART RX)   P2.1 (GPIO)  |<-o---------| TTL1_OUT                |
 *   KiTTY<---| P1.2 (UART TX)                |            |                         |
 *           >| P1.3 (SWITCH1)                |            |                         |
 *            |                               |            |                         |
 *            |                  P1.6 (LED2)  |>            -------------------------
 *            |                               |                Open-Field Rodent Arena
 *            |                               |             -----------------------------
 *            |                               |            |                             |
 *            |                  P2.0 (TA1_0) |-- 1k Ohm ->| 1 Feeder1 LED ring          |
 *            |                               |            | 2 Feeder2 LED ring          |
 *            |                               |            | 3 Feeder3 LED ring          |
 *            |                               |            | 4 Feeder4 LED ring          |
 *            |                               |            | 5 Feeder5 LED ring          |
 *            |                               |            | 6 Feeder6 LED ring      GND |----< GND
 *            |                               |             -----------------------------
 *
 * Written by: Rodrigo Ibáñez Alcalá, March 2021 for Dr. Alexander Friedman
 * Version 0.0: 26 March 2021
 *
 ***************************************************************************************************/

#include <msp430.h>

/* ------ Definitions ------ */
// Port 1
#define RXD     BIT1    // UART RX
#define TXD     BIT2    // UART TX
#define REDLED  BIT0    // LED1 output
#define GRNLED  BIT6    // LED2 output
#define ACK		BIT7	// Acknowledge command signal
#define ERR		BIT5	// Error detected signal
#define RELAY   BIT4    // Relay to drive larger circuit
#define BTN     BIT3    // On-board button SW1
// Port 2
#define TA1_0   BIT0    // Timer A1, output for capture/compare 0
#define LED3    BIT3    // External LED for indicating received TTL from Noldus
#define LED4    BIT4    // External LED
#define TTL_IN  BIT1    // TTL input from Noldus, stepped down to ~ 3.3V using a voltage divider
#define TTL_OUT BIT2    // TTL output to Noldus
/* ------ Functions ------ */
void ButtonSetup(void);         // Sets up button on P1.3 for interrupts to be used as a tester.
void UARTsendMsg(char* string); // Sends a string of characters through UCA0 in the configures UART mode.
/* ------ Global Variables ------ */


void main(void)
{
    WDTCTL = WDTHOLD + WDTPW;           // Stop Watchdog timer

    /** ------ Calibrate clock ------ **/
    // System clock is calibrated to run at 1 MHz, which is the default I think lol
    if (CALBC1_1MHZ==0xFF)  // If calibration constant erased
    {
        while(1);           // Trap CPU!!
    }
    DCOCTL = 0;             // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;  // Set range
    DCOCTL = CALDCO_1MHZ;   // Set DCO step + modulation

    /** ------ Setup GPIO ------ **/
    // Port 1
    P1DIR |= (REDLED
                | GRNLED
                | RELAY );  // Set to output for LEDs, relay, and UART TX
    P1OUT &= ~(REDLED
                | GRNLED
                | RELAY );  // Make sure LEDs and relay are off

    P1SEL |= (RXD | TXD);   // Set P1.1 and P1.2 to UART mode
    P1SEL2 |= (RXD | TXD);  // Step 2 of the same

    // Port 2
    P2SEL |= TA1_0;         // Set P2.0 to TA1.0 output
    P2SEL2 &= ~TA1_0;       // Make sure this bit is not set
    P2DIR |= (TA1_0         // Set TA1.0 external LEDs and TTL to Noldus as outputs
                | TTL_OUT
                | LED3
                | LED4);
    P2DIR &= ~TTL_IN;       // Set TTL from Noldus as input
    P2OUT = 0;              // Set all outputs to 0

    ButtonSetup();          // Optional internal button setup

    /** ------ Setup Timers ------ **/
    // This is for PWM lol
    TA1CTL &= ~MC_3;        // Start with timer stopped. Mode referenced has no relevance to functionality, I'm just clearing the bits.
    TA1CTL |= TASSEL_2;     // Select SMCLK source CONSIDER CHANGING THIS
    TA1CCTL1 |= OUTMOD_6;   // Set OUTMOD to Toggle/Set
    TA1CCR0 = 1000;         // f(out) = SMCLK/TA1CCR0 = period of 1kHz, consider this for calculating duty cycle
    TA1CCR1 = 100;          // This determines duty cycle: DC = (1-(TACCR1/TACCR0))*100%

    /** ------ Setup UART ------ **/
    UCA0CTL1 |= UCSWRST;    // Disable UART module and reset its registers for configuration

    // Initialize UART control and timing registers //
    UCA0CTL0 = 0;                       // ****** No parity, Least Sig. Bit first, 8-bit data, 1 stop bit, UART, Asynchronous p. 429 User's Guide ****** //
    UCA0CTL1 |= (UCSSEL_2 | UCRXEIE);   // ****** SMCLK source, erroneous characters will trigger UCA0RX interrupt flag, keep UCA0 off (in reset state) p. 430 User's Guide ****** //
    /** Note for low power modes: The UCA0 module will activate the source clock associated to it when in use, this can affect other peripherals that use the same clock source (timer modules will increment) **/
    UCA0BR0 = 104;                      // ****** 9600 Baud rate - Assumes 1 MHz clock p. 421 User's Guide: N = f_clk / desired_baud_rate ****** //
    UCA0BR1 = 0;
    UCA0MCTL = (UCBRS0 | UCBRF0);       // ****** 2nd Stage modulation = 1, Oversampling off p. 431 User's Guide ****** //
    UCA0CTL1 &= ~UCSWRST;   // Enable UART module
    //** ------ Setup Interrupts for serial communication ------ **//
    IFG2 &= ~(UCA0RXIFG);   // clear the Receive interrupt flag
    IE2 |= UCA0RXIE;        // Enable UART interrupt
    __bis_SR_register(GIE); // Enable global interrupts

    //** ------ Finish Initialization ------ **//
    //TA1CTL |= MC_2;         // Start timer and start PWM, saving this for later

    while(1){}

}

void ButtonSetup(void)
{
    //P1.3 Setup in a Pull-up Configuration with Falling-Edge Triggered Interrupt
    P1DIR &= ~BTN;
    P1REN |= BTN;
    P1OUT |= BTN;
    P1IE  |= BTN;
    P1IES |= BTN;
    P1IFG &= ~BTN;

    return;
}

void UARTsendMsg(char* string)
{
    while(*string)
        {
            UCA0TXBUF = *string++;
            while(UCA0STAT & UCBUSY);   // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
        }

    return;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void RX_Function(void)
{
    /*switch(UCA0STAT)
    {
        case UCFE:
            // Framing error flag.
            break;
        case UCOE:
            // Overrun error flag.
            break;
        case UCPE:
            // Parity error flag.
            break;
        case UCBRK:
            // Break detect flag.
            break;
        case UCRXERR:
            // Receive error flag.
            break;
        default:
            // If there was no error, continue.
            break;
    }*/

    char c;
    char* string = " test\r\n";

    P1OUT &= ~(GRNLED | REDLED | RELAY);
    P2OUT &= ~(LED3 | LED4 | TTL_OUT);

    while(!(IFG2 & UCA0RXIFG)); //Make the CPU wait here until the receive...
        //...interrupt flag goes high...
    c = UCA0RXBUF;              // Store received character
    while(UCA0STAT & UCBUSY);   //May not be needed but gives a good excuse to use a software delay.

    switch (c)
    {
        case 'A':
            P1OUT |= (GRNLED | REDLED);
            string = ": all on\r\n";
            break;
        case 'g':
            P1OUT |= GRNLED;
            string = ": green on\r\n";
            break;
        case 'r':
            P1OUT |= REDLED;
            string = ": red on\r\n";
            break;
        case 'R':
            P1OUT &= ~(GRNLED | REDLED | RELAY);
            P2OUT &= ~(LED3 | LED4 | TTL_OUT);
            string = ": all off\r\n";
            break;
        case 'F':
            P1OUT |= RELAY;
            string = ": relay on\r\n";
            break;
        case 'T':
            P2OUT |= (TTL_OUT | LED4);      // Send TTL pulse to Noldus and turn LED4 on as indication
            __delay_cycles(1000000);        // 1 s delay because idk what the TTL pulse width should be for Noldus
            P2OUT &= ~(TTL_OUT);            // End TTL pulse, LED4 stays on
            while(!(P2IN & TTL_IN)){};      // Wait for Noldus to respond with a TTL
            P2OUT |= LED3;                  // Turn LED3 on as indication that TTL was successfully received
            string = ": TTL done\r\n";       // Acknowledge TTL on terminal
            break;
        default:
            P1OUT &= ~(GRNLED | REDLED | RELAY);
            P2OUT &= ~(LED3 | LED4 | TTL_OUT);
            string = ": what?\r\n";
            break;
    }


    UCA0TXBUF = c;  // Echo character back
    while(UCA0STAT & UCBUSY);   // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
    while(*string)
    {
        UCA0TXBUF = *string++;
        while(UCA0STAT & UCBUSY);   // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
    }

    IFG2 &= ~(UCA0RXIFG);   // Clear the RX interrupt flag

}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1OUT |= GRNLED;
    char* msg = "test\r\n";
    while(*msg)
        {
            UCA0TXBUF = *msg++;
            while(UCA0STAT & UCBUSY);   // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
        }

    P2OUT |= (TTL_OUT | LED4);      // Send TTL pulse to Noldus and turn LED4 on as indication
    __delay_cycles(1000000);        // 1 s delay because idk what the TTL pulse width should be for Noldus
    P2OUT &= ~(TTL_OUT);            // End TTL pulse, LED4 stays on
    while(!(P2IN & TTL_IN)){};      // Wait for Noldus to respond with a TTL
    P2OUT |= LED3;                  // Turn LED3 on as indication that TTL was successfully received

    P1OUT &= ~GRNLED;
    P1IFG &= ~BIT3;     // Clear port interrupt flag
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{

}
