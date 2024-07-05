# Hammer Position Sensor (HPS) Version 0.7

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

Version 0.7 is the same as version 0.6 except with a different header pin order. This provides a better physical match to some mainboards.

This circuit board includes a trimming resistor. Unless needed for a specific use case, the circuit board with trimming resistor is not recommended.

## Features

### I/O
* Power input.
* Ground.
* Analog output signal, with range set by resistors.

### Size
* 1.5 inches long.
* 0.4 inches wide.

### Capabilities
* CNY70 sensor.
* Trimming resistor to fine tune output voltage maximum value.
* All resistors are surface mount packages.

### Pin Order:
* Power
* Ground
* Signal

## Calibrating

### Overview

Adjust the RV0 trimming resistor to get the output voltage in the correct range. A 1.0mm flathead screwdriver is required. Turn the screw slowly and gently. Information according to the datasheet [Bourns 3224 - 4mm SMD Trimpot Trimming Potentiometer, C1753 5/17/18]:
* Adjustment angle = 12 turns.
* Rotational cycling max = 200 cycles.

Based on testing, typical adjustment is no more than approximately 2 turns clockwise or counterclockwise. This results in a resistance range of approximately 70 to 130 ohms.

Turning the RV0 trimming screw clockwise decreases the resistance between pins 2 and 3 of the trimming resistor RV0 ==> this decreases the combined resistance R1 + RV0 ==> this increases the forward current of the CNY-70 emitter ==> this increases the output voltage.

Turning the RV0 trimming screw counterclockwise increases the resistance between pins 2 and 3 of the trimming resistor RV0 ==> this increases the combined resistance R1 + RV0 ==> this decreases the forward current of the CNY-70 emitter ==> this decreases the output voltage.

### Experimental Data

Each CNY-70 is different. The following data is for randomly selected CNY-70.

Tests are with the following resistance values.
* R1 = 50 ohms
* R2 = 4300 ohms

In the following table, *screw turns* definition:
* (+) is clockwise
* (-) is counterclockwise
* 1 = a full, 360 degree turn
* 0.5 = a half, 180 degree turn

The trimmer RV0 is 200 ohms. The nominal value is 100 ohms. The nominal value is most likely the initial value when the trimmer is purchased. All *screw turns* are referenced to the nominal. For accurate results, check with an ohmmeter.

| screw turns | RV0 | current (mA) | maximum output voltage |
|-------------|-----|--------------|------------------------|
| + 2.5       |  53 | 18.5         | 3.10                   | 
| + 2.0       |  62 | 17           | 3.05                   | 
| + 1.5       |  71 | 16           | 2.85                   | 
| + 1.0       |  81 | 14.5         | 2.60                   | 
| + 0.5       |  93 | 13           | 2.35                   | 
| 0           | 105 | 12           | 2.18                   | 
| - 0.5       | 116 | 11           | 2.05                   | 
| - 1.0       | 126 | 10           | 1.90                   | 
| - 1.5       | 135 | 10           | 1.50                   | 


R2 sets the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano will work with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value.