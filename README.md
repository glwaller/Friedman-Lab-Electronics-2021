# List of Common Issues and Their Solutions

Lights

1. Problem: one or more of the feeder lights won't turn on. 
1a. Solution: Lift feeder up and check Led bundle. You may need to resolder, but usually the light isn’t turning on b/c the black and blue wires are touching (thus shorting the circuit) and need to be readjusted. Readjust wires and secure them with tape. If this doesn’t resolve the issue then check LED cable and MCU’s/Breadboard/Relay’s as you would for the valve cables, and make sure that everything is plugging in and connected correctly.


Valves

1.Problem: Relay clicking, but valves not clicking. 
1a. Solution: Check connections and cables, start at the physical valves and check the cable all the way to MCU/Breadboards/Relays and then usb connections to the desktop.

2. Problem: Relays for arenas not clicking. 
2a. Solution: Most likely a connection issue at the breadboards/relays (blue and white blocks near the red MCUs (microcontrollers). 
___________________________________


Software
1. No known issues with the software, but it is a good rule of thumb to set the priority for EthoVision to "realtime" using task manager.
