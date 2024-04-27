# Hammer Position Sensor (HPS) Version 0.8

## Introduction

Originally, the HPS PCB was for hammers.  However, the sensor also works for dampers.  Not changing the name because many circuit boards have already been built.

Version 0.8 is the recommended sensor for surface mount.

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

### Manufacturing

To manufacture a board, send the files in (*coming soon*) to a PCB manufacturer.

Use the bill of materials file that matches the mainboard connected to the HPS.

R1 and R2 set the maximum output voltage. If the output voltage is too large, the analog-to-digital converter will clip. If the output voltage is smaller, it decreases the dynamic range. The piano will work with a lower dynamic range but may not work if the signal is clipping. Therefore, if the R2 value in bill of materials is unavailable, choose a smaller value. If the R1 value is unavailable, choose a larger value.

### KiCad vs Gerbers

Boards for Gerber files were fabricated and tested.

KiCad files should be identical, but there is a small probability they do not match what was fabricated and tested.

See [../../kicad_general.md](../../kicad_general.md) for information on KiCad files.