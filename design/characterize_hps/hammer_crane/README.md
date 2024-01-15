# Hammer Crane

## Purpose

Lift a hammer shank in a controlled manner with automation.

## Components

### Circuit Board

Place components on a 0.1" lead pitch breadboard.

* Arduino Nano.
* L298N motor drive controller.
* Breadboard style push button switches (6).
* Power connection for +12V input, mounted on the breadboard.

### Power Supplies
#### For L298N - Requirements on the power source:
* +12V output.
* 10A (amperage depends on linear actuator requirements).
#### For Arduino - How powered:
&nbsp;&nbsp; The Arduino Nano receives +5V power through a USB connection to computer.

### 12V Linear Actuator Requirements
* Must have at least 4 inches of stroke travel.
* Must be tall enough to clear the height of the hammers.
* Must be able to lift the weight of a piano shank and hammer.
* Includes power and ground wires.
* Optionally a position sensor output (typically 3 wires).

## Connections

### Linear Actuator Resistance Wires (optional)
* Orange = Arduino ground on breadboard.
* Yellow = Arduino +5 output on breadboard.
* Green = Arduino analog A7 input on breadboard.

### Linear Actuator Power Wires
&nbsp;&nbsp; See connections to L298N below.

### L298N Linear Actuator Controller
  * Connect IN3 of L298N to D9 of Arduino Nano.
  * Connect IN4 of L298N to D8 of Arduino Nano.
  * Connect OUT3 of L298N to linear actuator Red wire.
  * Connect OUT4 of L298N to linear actuator Brown wire.
  * Connect +12V of L298N to the +12V, 10A power supply.
  * Connect GND of L298N to power supply and Arduino ground.
  * Do not use +5 connection on L298N.

### Push Buttons
One side of each push button is connected through a resistor to ground.
The resistor should be at least 10K ohms.

The other side of each push button is connected to the following Arduino Nano pins:
* D2 - While pushed, the actuator pulls the shank up (via thread) in a very slow and controlled manner - use this push button to move the linear actuator during the tests.

* D3 - While pushed, the actuator releases the shank in a very slow and controlled manner.

* D4 - While pushed, the actuator quickly moves up.

* D5 - While pushed, the actuator quickly moves down.

* D6 - Future.

## Linear Actuator Mechanical
* Position behind *stem piano* next to damper rail.
* Mount on something sturdy. For example, a block of wood.
* Connect a rod at a 90-degree angle from the top of the linear actuator. One end of the rod is connected to the linear actuator. One end of the rod is directly above a hammer shank.
* Attach a thread from actuator rod to one of the hammer shanks.
* At the rod, use a light piece of tape so that if the actuator tries to pull the shank past the hammer stop, it will not break the shank.
* At the shank, use a lightweight metal hook.