# Hammer Position Sensor (HPS) Version 0.6

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

This circuit board includes a trimming resistor. Unless needed for a specific use case, the circuit board with trimming resistor is not recommended.

In repository for historical reasons.

## Features

### I/O
* Power input.
* Analog output signal, with range set by resistors.
* Ground.

### Size
* 1.5 inches long.
* 0.4 inches wide.

### Capabilities
* CNY70 sensor.
* Trimming resistor to fine tune output voltage maximum value.
* All resistors are surface mount packages.

### Pin Order:
* Power
* Signal
* Ground

## Calibrating

Adjust the RV0 trimming resistor to get the output voltage in the correct range. A 1.0mm flathead screwdriver is required. Turn the screw slowly and gently. Information according to the datasheet [Bourns 3224 - 4mm SMD Trimpot Trimming Potentiometer, C1753 5/17/18]:
* Adjustment angle = 12 turns.
* Rotational cycling max = 200 cycles.

Based on testing, typical adjustment is no more than approximately 2 turns clockwise or counterclockwise. This results in a resistance range of approximately 75 to 125 ohms.

Turning the RV0 trimming screw clockwise decreases the resistance between pins 2 and 3 of the trimming resistor RV0 ==> this decreases the combined resistance R1 + RV0 ==> this increases the forward current of the CNY-70 emitter ==> this increases the output voltage.

Turning the RV0 trimming screw counterclockwise increases the resistance between pins 2 and 3 of the trimming resistor RV0 ==> this increases the combined resistance R1 + RV0 ==> this decreases the forward current of the CNY-70 emitter ==> this decreases the output voltage.

R2 sets the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano will work with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value.