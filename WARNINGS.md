# DIY Hybrid Digital Grand Piano - WARNINGS

Please read all warnings prior to starting any work.

It is recommended to have experience with (or do research/learning on) areas listed below, and subjects related to software and electronics projects, before starting. Many resources are available on the internet.

The following warnings are only related to the piano itself.

Please read disclaimers in licenses.

Some of the warnings below may be repeated or described in more detail elsewhere in the repo. It cannot be guaranteed, but it is the intent that this *WARNINGS.md* file contains a high-level superset of all warnings. The information in this file may not be complete.

## Teensy USB and External Power

Internally the Teensy 4.1 shorts the its USB connection and the +5V power pin.

Be careful to **NEVER** connect the Teensy 4.1 USB cable and the external +5V power input at the same time. This could damage the computer connected to the USB.

When all 88 sensors are connected, the total current draw of the system will likely exceed the capability of an external computer that is trying to power the system through a USB cable connected to the Teensy 4.1. Therefore, the external +5V power source on the circuit board is needed. The PJRC website has information on how to use a USB cable under this scenario.

Please see the [PJRC website](https://www.pjrc.com) for more information.

## Fuses and Short Circuit Protection

Some boards include places to install fuses and some may not. Use fuses.

Use an external power supply that includes short-circuit protection.

Many of the boards include Low Dropout Regulators (LDO). When selecting the LDO to install onto the circuit board, select one that includes short-circuit protection. This means that if the LDO output is accidentally shorted to ground, the LDO will current limit to a specified value. This current limit can be used when calculating the worst-case total current of the board.

## Power and Ground Pins

Some pins on the circuit boards are at power level and some are at ground level. Be careful not to accidentally connect them together.

## Static sensitive parts

Be carefully when soldering, assembling and using the electronic parts. Electrostatic Discharge (ESD) can damage components. An ESD damaged component could cause an intermittent failure that is very difficult to debug.

Many resources on the internet provide information on ESD.

For a low level of ESD prevention, work in a very dry environment, with carpet, shuffle feet across ground, and frequently touch as many circuit components as possible (actually, no, don't do this!).

## Lead Free

Recommend lead free for everything.

## Touch Displays

Use the 2.8" TFT unmodified. For example, the TFT IM1, IM2, and IM3 pins are connected to +3.3V on the IPS 2.X PCB. Therefore, these jumpers internal to the TFT should *not* be soldered together (the manufacturers website may describe soldering them together).