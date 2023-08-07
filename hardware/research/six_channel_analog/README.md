# Six Channel Analog (SCA) specification

Connects to Integrated Processing System (IPS) 2.0 board.

## Introduction
Data conversion board for IPS 2.0.

Low cost PCB due to small size.

Users can experiment with different ADC front-ends without having to make a new (large, expensive) IPS 2.0 board.

Compared to IPS 2.0, the more difficult to solder parts may be on the SCA pcb. Mistakes are localized to a small, low-cost board.

## Requirements

### I/O
* Six analog inputs, for conversion to digital.
* Eleven digital inputs/outputs, for any purpose. Typical use is controlling the ADC or providing a fast word-wide interface to the Teensy.
* SPI interface.

## Power
* 5 volt input. Not power constrained.
* 3.3 voltage input - for digital components.
* 3.3 voltage input - for analog components.
* Ground connection at SPI/digital interface.
* Optional ground connection at power interface.

## Size
3.3 inches by 1.3 inches.

# Hold Down
One options 2.2mm screw.

### Capabilities
* Analog-to-digital conversion from the six 16:1 muxes on IPS 2.0 to the Teensy 4.1 on IPS 2.0.

