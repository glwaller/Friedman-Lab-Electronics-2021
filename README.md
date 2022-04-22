# Microcontroller-Firmware Design Files

Contained here is the Code Composer Studio project which contains the firmware which runs on the Texas Instruments MSP430-EXPFR2355 microcontroller launchpad for our RECORD system. All code is written in C.

6 files are of great imporantce for the microcontroller to run effectively, one C file, and 5 header files:

1. "main__v[Firmware Version].c": 
  Contains the main code executed by the microcontroller. This code includes all other header files described in this list.
  The code makes use of 5 Timer B outputs, one for each LED array on the arena and an additional indicating LED. The Timer B module is used to create a pulse width
  modulated signal in order to dim and brighten the LEDs connected to the system so that varying levels of cost can be produced for our cost/benefit tasks.
  Additionally, 7 standard general purpose input/output (GPIO) pins are defined, 4 for controlling an external relay which powers valves for reward delivery,
  two for outgoing 3.3 V TTL signals, and finally one for incoming TTL signals. It is important to note that incoming TTL signals should be stepped down to 3.3 V
  as a higher voltage may burn the pin.
  The microcontroller is configured to run at 8 MHz using its internal clock and baud rate for serial communication via UART is configured to be 8600.
  The system uses a hardware interrupt for the serial communications module which, as mentioned before, is configured for UART. The interrupt is called only when
  the recevie buffer is populated with data. Data may be received from a PC or another UART-enabled device.
  
2. "RECORD.h":
  Is a library of definitions, functions, and global variables critical for the RECORD system. All functions and variables are described within.
  
3. "arenacfg[1 through 4].h":
  Defines structures containing a value which is populated into each Timer B Capture Compare register for pulse width modulation for varying LED brightness in order
  to produce varying levels of cost in our cost/benefit tasks. Four structures are defined, one for each level, with "Light Level 0" corresponding to a duty cycle of 0%
  (LEDs are turned off) and "Light Level 3" corresponding to the highest cost level defined for our validated setup.
  The values currently defined in these files are tailored to our setup, thus it is recommended that they are changed. For our purposes, we set "Light Level 1" to
  15 Lux, "Light Level 2" to 40 Lux, and "Light Level 3" to 260 Lux, measured at 3mm from each light source.
  Only one "arenacfg" header files is loaded into the microcontroller. It is recommended that each arena have a separate set of "Light Level" structures as there will
  likely be some varability among feeder LEDs. To do this, simply change the "#include" like on line 16 of the "RECORD.h" header file to match each one of your RECORD
  arenas before flashing the firmware to each arena's microcontroller.
