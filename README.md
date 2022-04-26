# Common Issues and Their Solutions

Harware Troubleshooting

Anything concerning the physical components of the RECORD electronics and arenas will be contained here. When in doubt, turn everything off and on → unplug and plug. Make sure everything, from the wires and cables, to the tubes that deliver sucrose, is connected.
__________________________________

Valves and Relays

Problem: Relay clicking, but valves not clicking…
Solutions: Before proceeding, check that the valve cable is connected to a valve on one side, and to the corresponding slot on the RECORD box or circuit board on the other side.Use PuTTY to activate each valve individually by sending “F” for valve 1, “G” for 2, “H” for 3, then “J” for 4, and identify which valve clicks and which does not. See Protocol for using PuTTY on slack in #important-documents.

If you have at least one valve that does click, swap the connectors for a working valve and a non-working valve then use PuTTY to activate both valves.
If the non-working valve clicks and the working valve does not, the problem may be at the valve cable. Proceed to step (1) below.
If the working valve clicks and the non-working valve does not, the valves may be the problem. Jump to step (3) below.

If no valves click, make sure that the valves are being supplied the correct voltage for them to operate (15 V - 24 V). Check that the power supply cable is connected to the RECORD box and the valve power supply is turned on, then test again. If the problem persists, proceed to step (1) below.

Using the Valve Cables Maintenance guide as a reference (link), check connections and cables for short circuits. Make sure all exposed copper wire is shielded; wires should not be touching neighbouring wires at the head of connections to the PCB or the valves. If needed, tape each individual section of exposed wire with electrical tape to shield connections from one another.
Check the connector at the valve side of the valve cable, it is possible that the wire may have come loose from its housing and might not be making contact with the connector on the valve. Alternatively, the contacts on the connectors may be dirty. Disconnect the valve from the cable and clean the contacts using a damp paper towel, then connect them again.
If the problem persists, it may be necessary to either clean or replace the non-working valve(s). To clean the valve, dismount it from the valve stand, remove the solenoid from it, and submerge the valve plunger in water for at least 1 hour, then test it again.

Problem: Relays not clicking…
Solution: Likely caused by a connection issue between the microcontroller and relays. Carefully open the RECORD box and look for the wires that connect the microcontroller (in red) and the relays (long blue board featuring several blue blocks with white writing). If these wires are disconnected, the relays will not click. Refer to the PCB, Relay, and MCU connection tables (link) to repair and ask a lab supervisor for help, do not connect or disconnect anything on the PCB or microcontroller without supervision.

Problem: Valves and relays are clicking, but no sucrose is being dispensed…
Solution: This is likely caused by air being trapped in the system. Before proceeding, check that all tubing is connected to its corresponding feeders. Check for air bubbles in the tubing between the syringes and the valves, and between the valves and the arena feeders. If there are bubbles present, activate the valves a few times using PuTTY, or run the system_cleanup batch script routine. You may also force sucrose solution into the system by using the syringe plunger to push down the solution while the valves are open. To do this, take a syringe plunger and while activating the valve with PuTTY, push the plunger down the syringe to flush liquid through and clear the blockage. When finished, check for leaks in the system and fix them accordingly.

__________________________________

Lights

Problem: Light does not turn on for one or more feeders...
Solutions:
Make sure that the feeder lights are active at any (visible) brightness. Use PuTTY to turn the lights on. See Protocol for using PuTTY on slack in #important-documents.
Check that the non-working feeder lights are connected to the LED cable, this should be a two-wire connector that attaches to the cable. If the feeder is connected, make sure the lights are not connected backwards. Some feeders will have a blue and black wire pair, others will have a yellow and green wire pair, and (though uncommon) others may have a red and black wire pair. Make sure that the black wire on the LED cable (representing ground) is connected either to the black or green wire on the feeder LEDs, and the red wire on the LED cable (representing live wire) is connected to the yellow, red, or blue wire on the feeder LEDs. The light should turn on after this.
If the light does not turn on, or if one some LEDs turn on on that feeder, lift the feeder up by pushing the “tail” of the feeder up (where the tubing attaches) and pull it up and off the feeder base. Flip the feeder over and look at the underside. Carefully separate any exposed metal contacts on the LED ring bundle, making sure there are no short-circuits in the loop. Handle these wires with care, making sure not to break any of the connections. The lights should turn on after separating all the wires.
If the lights still do not turn on, check connections and cables for short circuits. Make sure all exposed copper wire is shielded; wires should not be touching neighbouring wires at the head of connections to the PCB or the valves. If needed, tape each individual section of exposed wire with electrical tape to shield connections from one another.

Problem: Two lights turning on at once…
Solution: The given cable’s copper wires are likely touching at the white connector on the end of the cable which connects to the RECORD box. Use electrical tape to shield each individual exposed copper wire to prevent the wires from touching one another.

__________________________________

Arena Pieces

Pieces will wear down and come apart over time, this process can happen a lot sooner if the rats gnaw on the arena. Simply replace faulty pieces with new parts and/or glue them back together. 

__________________________________

EthoVision Troubleshooting

Good rule of thumb to avoid issues in EthoVision is to set the program priority for EthoVision to “Realtime” via TaskManager (ctrl → shift → esc) and having other programs (especially big ones like MATlab and Google Chrome) closed while running trials.

Problem: Trials getting stuck at the beginning stage...
Alternative problem 1: Trial never ends…
Alternative problem 2: Lights stay on after a trial ends…
Solution: Caused by desynchronization between the RECORD system and the animal tracking software. Stop the trials. Close all command windows that may have been left open, then turn the microcontrollers off, wait up to 10 seconds, and turn them back on. Do this only after stopping the trials, doing so during a trial will desynchronize both systems again!

If the problem persists, check that the microcontroller is connected to the I/O interface via one of the data lines on the hardware synchronization cable. It is possible that the connection might need to be flipped around so that the output from the microcontroller goes to the TTL input line, and the input for the microcontroller goes to the TTL output line.

Changes to the trial control may be needed if the problem persists as the trial may not be allowing enough time for the microcontroller to resynchronize with the trial after running a command, or the serial communication channel may be closing prematurely.

Problem: EthoVision crashed and data was not recovered… 
Solution: This is more of a workaround than a solution. Create a backup of the experiment, then create a new experiment and delete any other behavioral task trial control structures from the one which you wish to run. A good practice is to keep different task versions in separate experiments, and always back up your experiments. This is to keep bugs in the program isolated to only one experiment.

Problem: A feeder has a significant delay before delivering sucrose in one or more arenas…
Solution: Check configuration for zones under the trial control settings, make sure that options for each decision-making condition variable (box x accepts, box y rejects, etc.) are set to the appropriate settings. Also increase the time for which accept/reject conditions must be met (for example 0.5s might be too low, and should be increased to 1.0s).
Example: We incurred this error when the condition check in “box 4 rejects” was set to “when center-point is in all zones simultaneously” instead of “in any of the zones” in the trial control settings. This made the trial hang because that condition can never be met as the rat cannot exist in more than one zone simultaneously, and the program was waiting until the “box 4 accepts” condition was met.

Problem: The Windows “USB disconnect” sound plays when more than one valve/relay clicks, then the current trial never ends…
Solution: This may be due to a power supply overload in the system, which makes the microcontrollers reset and de-synchronizes them from EthoVision.

If this happens to you in the middle of running trials, manually stop the trial, close all command windows, and continue running trials. 

To prevent this problem from happening again, modify the trial control in EthoVision so that the delivery stage at each arena is delayed 0.5 seconds after the previous one, this will ensure that valves don’t click at the same time and that the power supply for the relays is not overloaded. 

Problem: Ethovision gives an error saying calibration lines are significantly different, when editing arena settings… 
Solution: A new calibration line that is significantly smaller  or bigger than the others may have been accidentally added. This causes a conflict between the existing calibration lines which measure the arenas at 64.5cm. To solve, delete the new calibration line that was accidentally created.

__________________________________

Firmware Troubleshooting

It is rare for the firmware running on the microcontroller to act up. Firmware is debugged before uploading it to the microcontroller. Fixes to the firmware will require C language programming skills, but a good solution that will work most of the time is to turn the microcontroller(s) off, wait 10 seconds, then turn it back on.
