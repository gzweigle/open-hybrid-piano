# Hybrid MIDI Digital Piano

The warnings below may be repeated or described in more detail elsewhere in the repo. It cannot be guaranteed, but it is the intent that this *WARNINGS.md* file contains a high-level superset of all warnings.

Check this page regularly for updates.

## Project is Active

New pianos, features, capabilities, and enhancements could be added in the future. These changes could impact the hardware, firmware, software, or the mechanical structure.

See [STATUS.md](./STATUS.md) for the project state.

## Experience

It is recommended to have some experience with software and electronics projects. Or learn using internet and other resources.

## Known Issues

Take a look at any pending hardware changes or known problems in [Issues](https://github.com/gzweigle/DIY-Grand-Digital-Piano/issues) before building boards or playing the piano.

## Headphone and Audio System Safety

An external light source can trigger the sensors and cause one or many loud notes to play. Shield the sensors from light.

Also, although unlikely, problems or bugs with the piano could cause multiple (even all 88) notes to play at the maximum MIDI volume level unexpectedly. This could happen even if the piano is not in use. See example from a *stem piano* video at approximately the 3:00 minute mark: https://www.youtube.com/watch?v=gNeLMGaxmG0.

Before playing, powering up, or using the piano in any way, please verify with another MIDI program as source that volume and all other parameters are acceptable in the event of any piano problem, including as described above. Also, check for external light triggers before increasing volume.

## Teensy USB and External Power

The Teensy 4.1 shorts its USB power connection and its +5 volt power pin. Some *stem piano* printed circuit boards route the Teensy +5 volt power pin to an external power input connection.

The PJRC Teensy 4.1 website gives details on how to modify a USB cable to remove the power wires: https://www.pjrc.com.

If using an unmodified USB cable, do not connect it to the Teensy 4.1 at the same time as a +5 power cable is connected to the circuit board. This could damage a computer connected to the USB cable. Some *stem piano* circuit boards include a dedicated jumper as an extra reminder. Disconnect the jumper when powering the board with USB.

It is possible to power *stem piano* circuit boards through the USB and without a separate +5 power connection. This setup is great for debugging.

However, when *stem piano* has all 88 key sensors connected, the total current draw will likely exceed the capability of an external computer that is trying to power the board through a USB cable connected to the Teensy 4.1. Therefore, the external +5 volt power source on the circuit board is needed in these cases.

Please see the PJRC Teensy 4.1 website for more details: https://www.pjrc.com.

## Power Supply Protection

Some boards include places to install fuses and some may not. Fuses require design and coordination with external power sources. See each circuit board documentation.

Use power supplies that include short-circuit (overcurrent), overvoltage, thermal, and all other protection for safety. This includes the USB power source.

Some boards include low dropout voltage regulators (LDO). The LDO should include short-circuit protection. This means that if the LDO output is accidentally shorted to ground, the LDO will current limit to a specified value. This current limit can be used when calculating the worst-case total current of the board. See the *bill_of_materials* files in hardware directories for recommended LDO devices.

## Exposed Connections

Instructions are not provided for circuit board or piano enclosures.

Shield the circuit boards from physical access.

Some pins on the circuit boards are at power level and some are at ground level. Be careful not to accidentally connect them together. For example, be careful with anything conductive near the circuit boards.

Be careful with placing anything near or around the circuit boards, or with anything related to the circuit boards that are accessible to others.

The boards are low-voltage (five volts and below) but portions could under unexpected or unforseen circumstances get hot. For example a fuse. Keep the circuit boards clear of any materials.

## EEPROM
The nonvolatile EEPROM Teensy memory wears out after approximately 100,000 writes. Under normal use this limit is sufficient. A firmware or hardware problem could result in excessive writes. Use caution when modifying firmware or hardware related to the EEPROM. At startup some firmware (depending on debug setting) displays total lifetime writes.

## Static Sensitive Parts

Be careful when soldering, assembling and using the electronic parts. Electrostatic Discharge (ESD) can damage components. An ESD damaged component could cause an intermittent failure that is very difficult to debug.

Use a nonconductive instrument for moving the configuration switches.

Many resources on the internet provide information about ESD.

For a low level of ESD prevention, work in a very dry environment, with carpet, shuffle feet across ground, and frequently touch as many circuit components as possible (actually, no, don't do this!).

## Lead Free

Recommend lead free for everything.

## it is all: As Is

Read the licenses, disclaimers of warranty, no liabilities, the "as is" sections, etc.