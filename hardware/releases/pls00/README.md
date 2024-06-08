# Pedal Level Shift (PLS) specification

## Purpose

Pedal output voltage must be in range 0V to the ADC Vref volts.

The PLS board adds one pulldown resistor to each pedal T output. There are three pedals, so that means there are 3 T outputs and 3 resistors on the PLS circuit board: R1, R2, and R3.

Select resistance such that output voltage does not exceed ADC Vref. See recommendations below.

## Recommended Values

### (1) For Stem Piano G and IPS 2.0.

Use R1 = R2 = R3 = 7500 ohms.

This sets maximum pedal output at 2.0 volts. The ADC Vref for this piano is 2.5 volts.