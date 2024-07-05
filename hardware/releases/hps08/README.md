# Hammer Position Sensor (HPS) Version 0.8

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

Like version 0.4, version 0.8 has fixed resistor values. Version 0.8 is a different header pin order compared to version 0.4. This provides a better physical match to some mainboards.

Version 0.4 is through hole resistors, version 0.8 is surface mount resistors.

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
* All resistors are surface mount packages.

### Pin Order:
* Power
* Ground
* Signal

## Specifications

Current is approximately 11 milliamps.

Because of CNY-70 tolerances each HPS 0.8 board has a different maximum output voltage. The maximum output voltage is when the distance from the CNY-70 on the HPS 0.8, to a white reflective surface, is at approximately 1 millimeter.

Test results:
* The board to board maximum output voltage ranges from 1.5 volts to 2.5 volts.
* The board to board average maximum output voltage is approximately 2 volts.

The maximum output voltage for each HPS 0.8 board individually does not vary. So, for example, if an HPS 0.8 board maximum output voltage is 2.1 volts when distance is smallest, the maximum output voltage is consistently 2.1 volts each time the distance is smallest.

## Resistors

R1 and R2 set the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano will work with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value. If the R1 value is unavailable, choose a larger value.