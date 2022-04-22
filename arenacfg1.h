/*
 * RECORD.h
 *
 *  Created on: Mar 24, 2022
 *      Author: Raquel Ibáñez Alcalá
 *      Version: v0.0 (March 24, 2022) for MCU firmware version v1.1 and RECORD library v0.0
 *
 *  Duty cycle configurations for individual feeder LED rings.
 *  Configurations for Arena: 1
 *
 */

#ifndef ARENACFG1_H_
#define ARENACFG1_H_

/* -------------------- Structures --------------------------------------------------------------------------------- */
/* Declare structures for light levels 1, 2, and 3. These structures will contain the CCR value for each LED ring for
 * all four feeders at each level, allowing fine-tuned control of the duty cycle for each LED ring. This is to account
 * for natural variations due to cable length, soldering, wires, etc.
 */
struct LightLevel
{
    unsigned int fdr1;
    unsigned int fdr2;
    unsigned int fdr3;
    unsigned int fdr4;
};

/* -------- Independent Variables ---------------------------------------------------------------------------------- */

/*** LED BRIGHTNESS CONTROL:
* These three values control the brightness of the feeder LEDs.
* The number has an inverse relationship with brightness, meaning the lower the number, the brighter the
* LEDs will glow. A value of 0 is the minimum value you can set these to, which will correspond to the
* LED's maximum brightness capped by the LED itself. The maximum value is 8000, which will turn the LEDs off.
*
* - Set a value between 0 and 8000. Higher numbers will increase LED brightness. MODIFY ONLY THE NUMBER, NOT
*   THE DEFINITION NAME:
*/
/* Light Level 0 (0 Lux) */
struct LightLevel L0 = {.fdr1 = 8000,
                        .fdr2 = 8000,
                        .fdr3 = 8000,
                        .fdr4 = 8000};

/* Light Level 1 (15 Lux) */
struct LightLevel L1 = {.fdr1 = 7742,
                        .fdr2 = 7800,
                        .fdr3 = 7700,
                        .fdr4 = 7388};

/* Light Level 2 (170 Lux) */
struct LightLevel L2 = {.fdr1 = 5500,
                        .fdr2 = 5200,
                        .fdr3 = 5200,
                        .fdr4 = 5000};

/* Light Level 3 (260 Lux) */
struct LightLevel L3 = {.fdr1 = 4200,
                        .fdr2 = 3715,
                        .fdr3 = 3880,
                        .fdr4 = 4200};

// Last Modified: Apr 2, 2022

/*** RELAY CLOSE TIME:
* The following controls the amount of time a relay, which supplies its associated valve with power, will
* remain closed. A closed relay will supply power to its associated valve, which opens the valve and drops
* liquid reward into the feeder on the arena floor. The number corresponds to the amount of *CPU cycles* the
* microcontroller must wait with the relay closed before opening it up again. This code sets the MCU clock to
* 8 MHz, this means that 1 second corresponds to 8 million (8,000,000) CPU cycles.
*
* To clarify: This number is *NOT* in regular time units (seconds, milliseconds, microseconds), it is in CPU
* cycles, and the internal clock is calibrated to 8 MHz. This may be changed in a future update. This change is
* relatively simple and is mostly set up, I just need to check that it works.
*
* - Set a value that specifies how long the relay should be closed every time it is toggled. Remember: 8000000
*   corresponds to 1 second, 4000000 corresponds to half a second, and 2000000 corresponds to a quarter of a second.
*   MODIFY ONLY THE NUMBER AND AID YOUSELF WITH THE FORMULA BELOW, DO NOT CHANGE THE DEFINITION NAME:
*
*   Formula:
*   x = (8,000,000 * A)/1000
*   x: CPU cycles to delay some desired amount of milliseconds. This is what you write below.
*   A: Desired delay in milliseconds (1000 milliseconds = 1 second).
*/
//const int long RELAY_ONTIME = 8000000;
#define RELAY_ONTIME 4000000    // CPU cycles to delay when toggling relay 1, which connects to valve 1. Default: 8000000

#endif /* ARENACFG1_H_ */
