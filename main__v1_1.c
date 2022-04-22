/*
 * main.c
 *
 *  Created on: Mar 23, 2021
 *      Author: Raquel Ibáñez Alcalá
 *      Version: v1.1 (March 21, 2022)
 */

/****************************************************************************************************
 * MSP430-FR2355 R.E.C.O.R.D. (Reward-Cost in Rodent Decision-making) System Embedded Electronics
 *
 * Description: Proof of concept rodent arena controller for Friedman Lab rodent decision-making tasks.
 *  Communicates microcontroller with serial COM client running Windows 10 and connects to Noldus' TTL
 *  IO-Box. Microcontroller will receive commands from client called by Ethovision XT 16 and
 *  communicate back with a TTL pulse (ACK) after command finishes executing.
 *
 *                                                -------->| Inscopix TTL2_IN        |
 *                                               |   ----->| Inscopix TTL1_IN        |
 *                                               |  |       -------------------------
 *                                               |  |
 *                      MSP430FR2355             |  |            Noldus IO Box
 *             -------------------------------   |  |       -------------------------
 *            |                               |  |  |      |                         |
 *           <| P1.0 (LED1)      P3.0 (ACK)   |--|--o----->| TTL1_IN                 |
 *   PuTTY--->| P1.6 (UART RX)   P3.5 (TTL_IN)|<[]o--------| TTL1_OUT                |
 *   PuTTY<---| P1.7 (UART TX)   P3.6(TTL_OUT)|            |                         |
 *           >| P4.1 (SWITCH1)                |            |                         |
 *           >| P2.3 (SWITCH2)                |            |                         |
 *            |                               |            -------------------------
 *            |                               |                Open-Field Rodent Arena
 *            |                               |             -----------------------------
 *            |                               |            |                             |
 *            |                  P6.0 (TB3_1) |----------->| 1 Feeder1 LED ring          |
 *            |                  P6.1 (TB3_2) |----------->| 2 Feeder2 LED ring          |
 *            |                  P6.2 (TB3_3) |----------->| 3 Feeder3 LED ring          |
 *            |                  P6.3 (TB3_4) |----------->| 4 Feeder4 LED ring          |
 *            |                  P6.4 (TB3_5) |----------->| 5 Cue LED                   |
 *            |                  P3.1 (GPIO)  |---[Rly1]-->| 6 Valve 1                   |
 *            |                  P3.2 (GPIO)  |---[Rly2]-->| 7 Valve 2                   |
 *            |                  P3.7 (GPIO)  |---[Rly3]-->| 8 Valve 3                   |
 *            |                  P3.4 (GPIO)  |---[Rly4]-->| 9 Valve 4                   |
 *            |                               |            |                         GND |----< GND
 *            |                               |             -----------------------------
 *
 * Written by: Raquel Ibáñez Alcalá in March 2021 for Dr. Alexander Friedman
 * Version 1.1: 21 March 2022
 *
 ***************************************************************************************************/

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>

#include "RECORD.h"

void main(void)
{
    WDTCTL = WDTHOLD + WDTPW;   // Stop Watchdog timer

    /** ------ Calibrate clock ------ **/
    // Calibrate CPU clock to 8 MHz
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode
                                // to activate 1previously configured port settings

    __bis_SR_register(SCG0);    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;  // Set REFO as FLL reference source
    CSCTL1 = DCOFTRIMEN_1 |
             DCOFTRIM0    |
             DCOFTRIM1    |
             DCORSEL_3;         // DCOFTRIM=3, DCO Range = 8MHz
    CSCTL2 = FLLD_0 + 243;      // DCODIV = 8MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);    // enable FLL
    Software_Trim();            // Software Trim to get the best DCOFTRIM value

    CSCTL4 = SELMS__DCOCLKDIV |
             SELA__REFOCLK;     // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                // default DCODIV as MCLK and SMCLK source

    /** ------ Setup GPIO ------ **/
    // Port 1
    P1DIR |= 0xFF;          // Set all pins on this port as output to start.
    P1OUT = 0x00;           // Turn all outputs off.

    P1SEL0 |= RXD | TXD;    // Set the UART pins to second function.
    P1SEL1 &= ~(RXD | TXD); // Make sure these bits are not set.

    // Port 2
    P2DIR |= 0xFF;          // Set all pins as output.
    P2OUT = 0x00;           // Set all outputs to 0

    // Port 3
    P3DIR |= 0xFF;          // Set all pins to output.
    P3OUT = 0x00;
    P3OUT |= RELAY1 | RELAY2    // Set these high for the relay array that toggles only when signal is low.
             | RELAY3 | RELAY4;
    P3DIR &= ~(TTL_IN);     // Input signal from Noldus IO box.

    // Port 5
    P5DIR |= 0xFF;
    P5OUT = 0x00;

    // Port 6
    P6DIR |= 0xFF;
    P6OUT = 0x00;

    P6SEL0 |= ( TB3_1 |
                TB3_2 |
                TB3_3 |
                TB3_4 |
                TB3_5 );    // Select second function for these pins, Timer B3 for PWM.
    P6SEL1 &= ~( TB3_1 |
                 TB3_2 |
                 TB3_3 |
                 TB3_4 |
                 TB3_5 );   // Unset these bits.

    ButtonSetup();          // Optional internal button setup

    PM5CTL0 &= ~LOCKLPM5;   // Disable the GPIO power-on default high-impedance mode
                            // to activate previously configured port settings

    /** ------ Setup Timer B3 ------ **/
    // This is for PWM. Timer B3 will drive the LED rings to control brightness, and the Cue LED to make it glow.
    TB3CTL &= ~MC_3;        // Start with timer stopped. Mode referenced has no relevance to functionality, I'm just clearing the bits.
    TB3CTL |= TBSSEL_2;     // Select SMCLK source CONSIDER CHANGING THIS

    TB3CCTL1 |= OUTMOD_6;   // Set OUTMOD to Toggle/Set to output a pulse width modulated signal.
    TB3CCTL2 |= OUTMOD_6;
    TB3CCTL3 |= OUTMOD_6;
    TB3CCTL4 |= OUTMOD_6;
    TB3CCTL5 |= OUTMOD_6;

    TB3CCR0 = 8000;         // f(out) = SMCLK/TB1CCR0 = period of 1kHz, consider this for calculating duty cycle

    TB3CCR1 = 8000;         // This determines duty cycle: DC = (1-(TBCCR1/TBCCR0))*100%, if TB1CCR1 = TB1CCR0, duty cycle is 0%.
    TB3CCR2 = 8000;
    TB3CCR3 = 8000;
    TB3CCR4 = 8000;
    TB3CCR5 = 8000;

    /** ------ Setup UART ------ **/
    UCA0CTLW0 |= UCSWRST;                      // Disable UART module and reset its registers for configuration.
    UCA0CTLW0 |= UCSSEL__SMCLK;                // Use SMCLK as source.

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 22-4: UCBRSx = 0x49
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI

    //** ------ Setup Interrupts ------ **//

    UCA0IE |= UCRXIE;       // Enable USCI_A0 RX interrupt
    __bis_SR_register(GIE); // Enable global interrupts

    //** ------ Finish Initialization ------ **//
    TB3CTL |= MC_3;               // Start timer and start PWM, saving this for later
    //P3OUT |= TTL_OUT;           // Let Noldus know initialization is done.
    //while(!(P3IN & TTL_IN));    // Wait for Noldus to respond...
    //P3OUT &= ~TTL_OUT;          // TTL low and wait for commands!

    int ccr_value = 8000;
    SetBrightness('1', '0');
    SetBrightness('2', '0');
    SetBrightness('3', '0');
    SetBrightness('4', '0');
    while(1)
    {
        if(_BLINK)
        {
            while (ccr_value > LOWER_LIMIT)
            {
                delay_us(5000);
                ccr_value -= STEP;
               // TB3CCR1 = value;
               // TB3CCR2 = value;
               // TB3CCR3 = value;
               // TB3CCR4 = value;
                TB3CCR5 = ccr_value;
            }
            while (ccr_value < UPPER_LIMIT)
            {
                delay_us(5000);
                ccr_value += STEP;
               // TB3CCR1 = value;
               // TB3CCR2 = value;
               // TB3CCR3 = value;
               // TB3CCR4 = value;
                TB3CCR5 = ccr_value;
            }
        }
    }

}



#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    int ERROR = 0;
    unsigned int i = 0;
//    volatile unsigned int temp = 0;
    unsigned short int _RXSTR = 0;
    char c, INPUT, INPUT_lvl, INPUT_fdr;
    char CCRVAL[] = "";
    char* string = "";
    struct LightLevel *lvl_ptr;
//    volatile unsigned int *reg_ptr;

    P3OUT &= ~(ACK | TTL_OUT);

    while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
        //...interrupt flag goes high...
    c = UCA0RXBUF;              // Store received character
    while(UCA0STATW & UCBUSY);   //May not be needed but gives a good excuse to use a software delay.

    switch (c)
    {
        case '#':	// WHAT WILL ACTIVATE LED RINGS
            // Hang here while receiving characters one by one.
            // Acknowledgement won't be sent until ISR is pretty much done in order to parse stream.
			// In other words, ACK won't be sent until all characters are received and appropriate actions have been executed.
            // Instructions will look like this: #FxLy, where x is a feeder ID number, and y is a brightness level between 0 and 3.
            _RXSTR = 1;
            while (i <= length -1)
            {
                while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
                UCA0IFG &= ~(UCRXIFG);         // Clear the RX interrupt flag
                rxstring[i] = UCA0RXBUF;
                i++;
                while(UCA0STATW & UCBUSY);   //May not be needed but gives a good excuse to use a software delay.
            }
            string = ": Feeder configured successfully!\r\n\n";
            break;
        case '$': // CONFIG MODE FOR BRIGHTNESS LEVELS. Asks for user input to change the CCR value for a brightness level. Changes will not be committed to memory (yet). Future update must write to Flash.
            // Command indication
            string =  "Entering configuration mode...\r\n Keep in mind any configuration done here will be lost when the system is powered off. Commit these changes as default by changing the system's code.\r\n\n";
            UARTsendMsg(string);
            // ----- Give instructions (1/2)... -----
            string =  "Please enter the level to reconfigure (1, 2, or 3).\r\n> ";
            UARTsendMsg(string);
            // Wait for input...
            while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
            UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
            INPUT_lvl = UCA0RXBUF;
            // Display input...
            while(UCA0STATW & UCBUSY);        // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
            UCA0TXBUF = INPUT_lvl;                   // Echo character back
            string = "\r\n";
            UARTsendMsg(string);

            string =  "Please enter the feeder to reconfigure (1, 2, 3, or 4).\r\n> ";
            UARTsendMsg(string);
            // Wait for input...
            while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
            UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
            INPUT_fdr = UCA0RXBUF;
            // Display input...
            while(UCA0STATW & UCBUSY);        // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
            UCA0TXBUF = INPUT_fdr;                   // Echo character back
            string = "\r\n";
            UARTsendMsg(string);

            // ----- Give instructions (2/2)... -----
            string = "Please enter the new integer CCR value for this level and feeder (0 through 8000, whole numbers only).\r\nEnter a 4-character number or press enter if you're entering less than 4 characters.\r\n> ";
            UARTsendMsg(string);
            // Wait for input and display it...
            length = 4;
            while (i <= length -1)
            {
                while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
                UCA0IFG &= ~(UCRXIFG);         // Clear the RX interrupt flag
                if (UCA0RXBUF == '\r')         // User can press ENTER to finish inputting a value if their input contains less than 4 characters.
                {
                    CCRVAL[i] = '\0';
                    break;
                }
                else
                {
                    CCRVAL[i] = UCA0RXBUF;
                    UCA0TXBUF = CCRVAL[i];
                    i++;
                }
                while(UCA0STATW & UCBUSY);   //May not be needed but gives a good excuse to use a software delay.
            }
            string = "\r\n";
            UARTsendMsg(string);

            // Parse CCRVAL
            if (atoi((const char *)CCRVAL) > 8000 || atoi((const char *)INPUT_lvl) > 3 || atoi((const char *)INPUT_fdr) > 4)
            {
                // Indicate an error if inputs are not valid.
                ERROR = 1;
            }
            else
            {
                // Check what level to modify and commit the new value to variable.
                switch (INPUT_lvl)
                {
                case '1':
                    // Variable for level 1
                    lvl_ptr = &L1;
                    break;
                case '2':
                    // Variable for level 2
                    lvl_ptr = &L2;
                    break;
                case '3':
                    // Variable for level 3
                    lvl_ptr = &L3;
                    break;
                default:
                    string = "Error: New CCR value should not exceed 8000 or LEVEL should not exceed 3. Configuration not set.\r\n";
                    UARTsendMsg(string);
                    ERROR = 1;
                    string = ": Configuration aborted due to error, try again. Resuming previous operations...\r\n\n";
                    break;
                }
            }

            if (ERROR != 1)
            {
                ModifyCCR(lvl_ptr, INPUT_fdr, atoi(CCRVAL));
                // Test new value?
                string = "Would you like to test the new value? [y/n]\r\n> ";
                UARTsendMsg(string);
                // Wait for input...
                while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
                UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
                INPUT = UCA0RXBUF;
                // Display input...
                while(UCA0STATW & UCBUSY);        // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
                UCA0TXBUF = INPUT;                   // Echo character back
                string = "\r\n";
                UARTsendMsg(string);
                if (INPUT == 'y')
                {
                    SetBrightness('1', INPUT_lvl);
                    SetBrightness('2', INPUT_lvl);
                    SetBrightness('3', INPUT_lvl);
                    SetBrightness('4', INPUT_lvl);
                    string = ": New settings applied! Resuming previous operations...\r\n\n";
                }
                else
                    string = ": Configuration applied! Restart feeders to see changes. Resuming previous operations...\r\n\n";
            }
            else
            {
                string = ": Configuration aborted due to error, try again. Resuming previous operations...\r\n\n";
            }
            break;
//        case '%':
//            // Command indication
//            string =  "Entering calibration mode...\r\n Keep in mind any configuration done here will be lost when the system is powered off. Commit these changes as default by changing the system's code.\r\n\n";
//            UARTsendMsg(string);
//            // ----- Give instructions (1/3)... -----
//            string =  "Please enter the level to reconfigure (1, 2, or 3).\r\n> ";
//            UARTsendMsg(string);
//            // Wait for input...
//            while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
//            UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
//            INPUT_lvl = UCA0RXBUF;
//            // Display input...
//            while(UCA0STATW & UCBUSY);        // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
//            UCA0TXBUF = INPUT_lvl;                   // Echo character back
//            string = "\r\n";
//            UARTsendMsg(string);
//            if (atoi((const char *)INPUT_lvl) > 3 || atoi((const char *)INPUT_lvl) < 1)
//            {
//                // Indicate an error if inputs are not valid.
//                ERROR = 1;
//            }
//            else
//            {
//                switch (INPUT_lvl)
//                {
//                case '1':
//                    // Variable for level 1
//                    lvl_ptr = &L1;
//                    break;
//                case '2':
//                    // Variable for level 2
//                    lvl_ptr = &L2;
//                    break;
//                case '3':
//                    // Variable for level 3
//                    lvl_ptr = &L3;
//                    break;
//                default:
//                    break;
//                }
//            }
//            // ----- Give instructions (2/3)... ----
//            string =  "Please enter the feeder to reconfigure (1, 2, 3, or 4).\r\n> ";
//            UARTsendMsg(string);
//            // Wait for input...
//            while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
//            UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
//            INPUT_fdr = UCA0RXBUF;
//            // Display input...
//            while(UCA0STATW & UCBUSY);        // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
//            UCA0TXBUF = INPUT_fdr;                   // Echo character back
//            string = "\r\n";
//            UARTsendMsg(string);
//            switch (INPUT_fdr)
//            {
//            case '1':
//                reg_ptr = &TB3CCR1;
//                break;
//            case '2':
//                reg_ptr = &TB3CCR2;
//                break;
//            case '3':
//                reg_ptr = &TB3CCR3;
//                break;
//            case '4':
//                reg_ptr = &TB3CCR4;
//                break;
//            default:
//                ERROR = 1;
//                break;
//            }
//            if (!ERROR)
//            {
//                i = 0; // Will contain the temporary CCR value.
//                // ----- Give instructions (3/3)... -----
//                string =  "Use '[' and ']' to decrease or increase the CCR value for the selected feeder. Press 'Enter' when done.\r\n> ";
//                UARTsendMsg(string);
//
//                while(INPUT != '\r')
//                {
//                    string = "CCR value is currently: ";
//                    UARTsendMsg(string);
//                    itoa(reg_ptr, string, 10);
//                    UARTsendMsg(string);
//                    string = "\r\n";
//                    UARTsendMsg(string);
//                    // Wait for input
//                    while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
//                    UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
//                    INPUT = UCA0RXBUF;
//
//                    if(INPUT == '[') // Decrease CCR value
//                    {
//                        temp = temp - 50;
//                        reg_ptr = temp;
//                    }
//                    else if(INPUT == ']')
//                    {
//                        temp = temp + 50;
//                        if(temp > 8000)
//                            temp = 8000;
//                        reg_ptr = temp;
//                    }
//                    else
//                    {
//                        string = "Please only enter '[' or ']'. Press 'Enter' to exit calibration mode.";
//                        UARTsendMSG(string);
//                    }
//                }
//                string = "Would you like to apply these changes? [y/n]";
//                UARTsendMsg(string);
//                // Wait for input...
//                while(!(UCA0IFG & UCRXIFG)); //Make the CPU wait here until the receive...
//                UCA0IFG &= ~(UCRXIFG);       // Clear the RX interrupt flag
//                INPUT = UCA0RXBUF;
//                if(INPUT == 'y')    // Modify the level structure
//                {
//                    ModifyCCR(lvl_ptr, INPUT_fdr, i);
//                    SetBrightness('1', INPUT_lvl);
//                    SetBrightness('2', INPUT_lvl);
//                    SetBrightness('3', INPUT_lvl);
//                    SetBrightness('4', INPUT_lvl);
//                    string = ": New settings applied! Resuming previous operations...\r\n\n";
//                }
//                else    // Do not modify level structure and revert.
//                {
//                    SetBrightness('1', INPUT_lvl);
//                    SetBrightness('2', INPUT_lvl);
//                    SetBrightness('3', INPUT_lvl);
//                    SetBrightness('4', INPUT_lvl);
//                    string = ": Rolled back to previous configuration. Resuming previous operations...\r\n\n";
//                }
//            }
//            else
//                string = ": Cannot continue due to error. Check that both level and feeder values are between their respective ranges. Calibration aborted.";
//            break;
        case 'A':	// NOT USED
            P1OUT |= REDLED;
            P5OUT |= (LED1 | LED2 | LED3 | LED4);
            P6OUT |= GRNLED;
            string = ": all on\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'g':	// NOT USED
            P6OUT |= GRNLED;
            string = ": green on\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'r': 	// NOT USED
            P1OUT |= REDLED;
            string = ": red on\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'k':	// ACTIVATE (TOGGLE) RED LIGHT (TRIAL START LIGHT) & WILL SEND A MESSAGE TO CONSOLE (CMD PROMPT)
            if (_BLINK)
                { _BLINK = 0; string = ": blinking off\r\n\n"; }
            else
                { _BLINK = 1; string = ": blinking on\r\n\n"; }
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'R':	// RESETS ALL LIGHT TO OFF & SENDS MESSAGE TO CONSOLE
            P1OUT &= ~(REDLED);
            P3OUT &= ~(ACK | TTL_OUT);
            P3OUT |= ( RELAY1 | RELAY2 | RELAY3 | RELAY4 );
            P5OUT &= ~(LED1 | LED2 | LED3 | LED4);
            P6OUT &= ~(GRNLED);
            TB3CCR1 = 8000;
            TB3CCR2 = 8000;
            TB3CCR3 = 8000;
            TB3CCR4 = 8000;
            TB3CCR5 = 8000;
            _BLINK = 0;
            string = ": all off\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'F':	// ACTIVATES RELAY (VALVE) #1 & SENDS MESSAGE
            P3OUT &= ~RELAY1;
            __delay_cycles(RELAY_ONTIME);
            P3OUT |= RELAY1;
            string = ": relay1 toggled\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'G':	// ACTIVATES RELAY (VALVE) #2 & SENDS MESSAGE
            P3OUT &= ~RELAY2;
            __delay_cycles(RELAY_ONTIME);
            P3OUT |= RELAY2;
            string = ": relay2 toggled\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'H':	// ACTIVATES RELAY (VALVE) #3 & SENDS MESSAGE
            P3OUT &= ~RELAY3;
            __delay_cycles(RELAY_ONTIME);
            P3OUT |= RELAY3;
            string = ": relay3 toggled\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case 'J':	// ACTIVATES RELAY (VALVE) #4 & SENDS MESSAGE
            P3OUT &= ~RELAY4;
            __delay_cycles(RELAY_ONTIME);
            P3OUT |= RELAY4;
            string = ": relay4 toggled\r\n\n";
            P3OUT |= ACK;               // Send command acknowledgement
            __delay_cycles(4000000);
            P3OUT &= ~(ACK);
            break;
        case '?': // Help section. Lists available commands and gives a short explanation of what they do.
            string = "Welcome to the about section! I will list all the commands I have available for you to use and will give you a short description of what each of them does.\r\n\n";
            UARTsendMsg(string);
            string = "1. '#': Turn on a specific feeder LED ring at a specified level.\r\nI will quietly wait for your input and only execute it once you're done.\r\nPlease format your input like this: FxLy. X is the feeder you want to activate, and Y is the brightness level you want to set it to.\r\n\n";
            UARTsendMsg(string);
            string = "2. '$': Starts configuration mode. Instructions will pop up as soon as you input this command.\r\nThis will allow you to reconfigure how bright feeder LEDs should be.\r\n\n";
            UARTsendMsg(string);
            string = "3. 'k': Toggles the 'trial in progress' light.\r\n\n";
            UARTsendMsg(string);
            string = "4. 'R': Resets everything. All LEDs are turned off and relays are opened.\r\n\n";
            UARTsendMsg(string);
            string = "5. 'F': Toggles relay 1, which will open and close valve 1.\r\nThe amount of time that the relay will be closed can be configured at the beginning of the code I'm executing.\r\nYou'll need to edit that yourself.\r\n\n";
            UARTsendMsg(string);
            string = "6. 'G': Toggles relay 2, which will open and close valve 2.\r\nThe amount of time that the relay will be closed can be configured at the beginning of the code I'm executing.\r\nYou'll need to edit that yourself.\r\n\n";
            UARTsendMsg(string);
            string = "7. 'H': Toggles relay 3, which will open and close valve 3.\r\nThe amount of time that the relay will be closed can be configured at the beginning of the code I'm executing.\r\nYou'll need to edit that yourself.\r\n\n";
            UARTsendMsg(string);
            string = "8. 'J': Toggles relay 4, which will open and close valve 4.\r\nThe amount of time that the relay will be closed can be configured at the beginning of the code I'm executing.\r\nYou'll need to edit that yourself.\r\n\n";
            UARTsendMsg(string);
            string = ": Information.\r\n\n";
            break;
        default:	// RESPONSE MESSAGE WHEN MCU RECIEVES AN UNKNOWN COMMAND
            string = ": I cannot recognize that command. Send me a '?' for a list of commands.\r\n\n";
            break;
    }

    UCA0TXBUF = c;                   // Echo character back
    UARTsendMsg(string);
    if (_RXSTR)
    {
        // Parse the received command:
        char FEEDER = rxstring[1];
        char BRIGHT = rxstring[3];
        SetBrightness(FEEDER, BRIGHT);
        // Echo recevied string back...
        i = 0;
        while(i <= length -1)
        {
            UCA0TXBUF = rxstring[i];
            i++;
            while(UCA0STATW & UCBUSY);
        }
        while(UCA0STATW & UCBUSY);
        UCA0TXBUF = '\r';
        UCA0TXBUF = '\n';
        UCA0TXBUF = '\n';

        P3OUT |= ACK;               // Acknowledge string receive and execution.
        __delay_cycles(4000000);
        P3OUT &= ~(ACK);
    }

    UCA0IFG &= ~(UCRXIFG);         // Clear the RX interrupt flag and exit

}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{
    P6OUT |= GRNLED;
    char* msg = "test\r\n\n";
    while(*msg)
        {
            UCA0TXBUF = *msg++;
            while(UCA0STATW & UCBUSY);   // May not be needed. Checks if the UCA0 module is busy sending or receiving a message.
        }

    P3OUT |= TTL_OUT;               // Send TTL pulse to Noldus and turn LED4 on as indication
    P5OUT |= LED4;
    __delay_cycles(8000000);        // 1 s delay because idk what the TTL pulse width should be for Noldus
    P3OUT &= ~(TTL_OUT);            // End TTL pulse, LED4 stays on
    while(!(P2IN & TTL_IN)){};      // Wait for Noldus to respond with a TTL
    P5OUT |= LED3;                  // Turn LED3 on as indication that TTL was successfully received

    P6OUT &= ~GRNLED;
    P4IFG &= ~BTN1;                  // Clear port interrupt flag
}

/*
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{

}
*/
