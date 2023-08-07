# Integrated Processing System 2 (IPS2) specification

Requires a Six Channel Analog (SCA) card.

## Requirements

### I/O
* Either:
  * Analog interfaces to 88 hammer sensor boards, or
  * Analog interfaces to 88 damper sensor boards.
* 3 pedal inputs: 1/4 inch.
* MIDI output.
* Ethernet interface (optional).
* 5 volt, 2.5 amp power input.
* CAN bus.

### Capabilities
* Process hammer samples and generate MIDI outputs.
* Process damper samples and send to hammer processing.
* Process pedal samples and generate MIDI outputs.
* Dual in-line package switches for configuration.
* Optional advanced monitoring, setup, and analytics over Ethernet and web brower.
* Optional hammer and pedal data over Ethernet using the piano network protocol (PNP).
* Optional TFT display for setup.

### Components
* Highly available, not likely supply chain constrained.
* Relatively large pitch, easy to solder parts.

## Form
* Single board.
* Low profile.