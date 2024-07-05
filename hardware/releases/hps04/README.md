# Hammer Position Sensor (HPS) Version 0.4

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

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
* Fixed resistors.
* All resistors are through hole.

### Pin Order:
* Power
* Signal
* Ground

## Resistors

R2 sets the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano works with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value. If the R1 value is unavailable, choose a larger value.