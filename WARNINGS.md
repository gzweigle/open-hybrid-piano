# Hybrid MIDI Digital Grand Piano

The warnings below may be repeated or described in more detail elsewhere in the repo. It cannot be guaranteed, but it is the intent that this *WARNINGS.md* file contains a high-level superset of all warnings.

## Project Is Not Done

This is an active project.

Please expect changes.

The hardware could require changes for the piano to be useful. The design could change. The firmware and software could have bugs. New pianos, features, capabilities, and enhancements could be added in the future.

## Experience

It is recommended to have some experience with software and electronics projects. Or learn using internet and other resources.

## Known Issues

Take a look at any pending hardware changes or known problems in [Issues](https://github.com/gzweigle/DIY-Grand-Digital-Piano/issues) before building boards.

## Teensy USB and External Power

The Teensy 4.1 shorts its USB power connection and its +5 volt power pin. Some *stem piano* printed circuit boards route the Teensy +5 volt power pin to an external power input connection.

The PJRC Teensy 4.1 website gives details on how to modify a USB cable to remove the power wires: https://www.pjrc.com.

If using an unmodified USB cable, do not connect it to the Teensy 4.1 at the same time as a +5 power cable is connected to the circuit board. This could damage a computer connected to the USB cable. Some *stem piano* circuit boards include a dedicated jumper as an extra reminder. Disconnect the jumper when powering the board with USB.

It is possible to power *stem piano* circuit boards through the USB and without a separate +5 power connection. This setup is great for debugging.

However, when *stem piano* has all 88 key sensors connected, the total current draw will likely exceed the capability of an external computer that is trying to power the board through a USB cable connected to the Teensy 4.1. Therefore, the external +5 volt power source on the circuit board is needed in these cases.

Please see the PJRC Teensy 4.1 website for more details: https://www.pjrc.com.

## Fuses and Short Circuit Protection

Some boards include places to install fuses and some may not. Use fuses.

Use an external power supply that includes short-circuit (overcurrent) protection.

Some boards include low dropout voltage regulators (LDO). The LDO should include short-circuit protection. This means that if the LDO output is accidentally shorted to ground, the LDO will current limit to a specified value. This current limit can be used when calculating the worst-case total current of the board. See the *bill_of_materials* files in hardware directories for recommended LDO devices.

## Power and Ground Pins

Some pins on the circuit boards are at power level and some are at ground level. Be careful not to accidentally connect them together.

## Static sensitive parts

Be careful when soldering, assembling and using the electronic parts. Electrostatic Discharge (ESD) can damage components. An ESD damaged component could cause an intermittent failure that is very difficult to debug.

Many resources on the internet provide information about ESD.

For a low level of ESD prevention, work in a very dry environment, with carpet, shuffle feet across ground, and frequently touch as many circuit components as possible (actually, no, don't do this!).

## Lead Free

Recommend lead free for everything.