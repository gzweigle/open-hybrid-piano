# DIY Hybrid Digital Grand Piano - WARNINGS

Please read all warnings prior to starting any work.

It is recommended to have some experience with software and electronics projects. Or learn using internet and other resources.

Some of the warnings below may be repeated or described in more detail elsewhere in the repo. It cannot be guaranteed, but it is the intent that this *WARNINGS.md* file contains a high-level superset of all warnings. The information in this file may not be complete.

Please read disclaimers in licenses.

## Teensy USB and External Power

Internally the Teensy 4.1 shorts the its USB connection and the +5V power pin.

Do not connect an external +5V supply to a circuit board at the same time as the Teensy 4.1 USB programming cable is connected and when that USB cable is also suppling power. This could short the computer connected to the USB with the +5V supply connected to the circuit board, and damage the computer. Please see the PJRC Teensy 4.1 website for more details on this issue: [PJRC](http://prjc.com). Some circuit boards have a dedicated jumper as an extra reminder.

For boards connected to all 88 piano key sensors. The total current draw of a circuit board will likely exceed the capability of an external computer that is trying to power the board through a USB cable connected to the Teensy 4.1. Therefore, the external +5V power source on the circuit board is needed in these cases. The PJRC website has information on how to use a USB cable under this scenario.

Please see the [PJRC website](https://www.pjrc.com) for more information.

## Fuses and Short Circuit Protection

Some boards include places to install fuses and some may not. Use fuses.

Use an external power supply that includes short-circuit protection.

Some boards include low dropout voltage regulators (LDO). The LDO should include short-circuit protection. This means that if the LDO output is accidentally shorted to ground, the LDO will current limit to a specified value. This current limit can be used when calculating the worst-case total current of the board. See the bill_of_materials files in hardware directories for recommended LDO devices.

## Power and Ground Pins

Some pins on the circuit boards are at power level and some are at ground level. Be careful not to accidentally connect them together.

## Static sensitive parts

Be carefully when soldering, assembling and using the electronic parts. Electrostatic Discharge (ESD) can damage components. An ESD damaged component could cause an intermittent failure that is very difficult to debug.

Many resources on the internet provide information on ESD.

For a low level of ESD prevention, work in a very dry environment, with carpet, shuffle feet across ground, and frequently touch as many circuit components as possible (actually, no, don't do this!).

## Lead Free

Recommend lead free for everything.