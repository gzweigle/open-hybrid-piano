# Hammer Position Sensor (HPS) Version 0.7

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

Version 0.7 changes the header pin order. This provides a better physical match to some mainboards.

Version 0.7 is the recommended sensor, if using the trimmer resistor.

## Features

### I/O
* Power input.
* Ground.
* Analog output signal, with range set by resistors.

### Size
* 1.5 inches long.
* 0.4 inches wide.

### Capabilties
* CNY70 sensor.
* Trimming resistor to fine tune output voltage maximum value.
* All resistors are surface mount packages.

### Manufacturing

To manufacture a board, send the files in [hps07_gerber/](hps07_gerber/) to a PCB manufacturer.

Use the bill of materials file that matches the mainboard connected to the HPS.

R2 sets the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano will work with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value.