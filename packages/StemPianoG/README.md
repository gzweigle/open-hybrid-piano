# Stem Piano Model G

The first release of Stem Piano is the Model G.

## Features

* Works on grand and upright pianos.
* Works with the action separate from the piano or as a silent system in a full piano.
* Works with a digital keyboard, using sensors on the digital keys.
* Can use as part of creating innovative new musical instruments.
* Sends data via 5-pin serial MIDI.
* Measures and sends hammer velocity and, optionally, damper velocity over MIDI.
* Includes three jacks, quarter-inch, for pedals.
* Accepts up to 96 inputs. Therefore, will work for pianos with more than 88 keys.
* Optional 10/100 Mbit Ethernet port for configuration, status, and debug.
* Optional 2.8" TFT display for configuration and debug.
* Four DIP switches for simple configuration.

## Required Components

Until the Stem Piano Model G is fully released, and the overall repo is in a
finished state, some code may be in releases/ directories and some in
research/ directories.
This will all be cleaned up at time of full (official) release.

#### [StemPianoIPS2 Library link](../../firmware/releases/StemPianoIPS2/)
Arduino library firmware that runs on the Integrated Processing System (IPS) hardware. Copy this code to the Arduino library location or to the same directory as the hammer firmware.

#### [Hammer Firmware link](../../firmware/releases/ips2_hammer/)
Main Arduino code for the Teensy 4.1 processor on the IPS hardware connected to hammer sensors.
Use this code as the Arduino sketch code. May need to rename the top level file from .cpp to .ino.

The settings file is located in this directory.

#### [Six Channel Analog link](../../hardware/releases/sca00/)
The six channel analog card with ADC that removably connects into the IPS mainboard.
In this directory are the files needed for PCB fabrication and a list of all components for the board.

#### [Integrated Processing System link](../../hardware/releases/ips20/)
The Integrated Processing System 2 mainboard.  In this directory are the files needed for PCB fabrication and a list of all components for the board.

## Optional Components

#### [Damper Firmware link](../../firmware/releases/ips2_damper/)
Main Arduino code for the Teensy 4.1 processor on the IPS mainboard connected to damper sensors. When using a damper measurement system, a second IPS mainboard and second SCA0 card are required.

If the damper measurement system is not installed, the hammer firmware will estimate the damper location based on the hammer sensor inputs.

#### [Hammer Position Sensor 0.6 link](../../hardware/releases/hps06/)
#### [Hammer Position Sensor 0.4 link](../../hardware/releases/hps04/)
Either hammer position sensor (HPS) board at these two links will work.
Eighty-eight (or more) sensor boards are required, one for each hammer.
When using a damper board, 88 HPS (or more) sensor boards are required, one for each damper.

It is also possible to use other sensors, which is why these boards are listed as optional. However, some form of sensor is required.

The sensor boards must output an analog value in range [0, ..., 3.3] volts when the hammer or damper is at minimum and maximum position respectively. It is ok to send a value less than 3.3 V, such as 3.0 V or even lower. Lower voltages could require adjusting the settings in settings file.

The maximum allowable current draw of each sensor board is 15 mA.

#### [IPS2Analytics Library link](../../software/releases/ips2_analytics/)
Optional Python software for acquiring and displaying data from the Ethernet connection.

#### [Mechanical link](../../mechanical)
Examples of mechanical configurations.

## Purchasing Parts and PCB Fabrication

The bill of materials file in each *hardware/* subdirectory lists all parts required to build the boards.

For PCB fabrication, use the gerber files in each *hardware/* subdirectory. Often a manufacturer will request putting the gerber files in a single .zip directory.

## Hardware Build and Assembly

Please see the following YouTube video (*NOT DONE YET*) for soldering, build, and assembly instructions in video form.

All components on IPS were selected for ease of soldering. They are also mature parts that should have good availability or with good substitute options.

The analog-to-digital conversion (ADC) subsystem is on a separate, removable, card (SCA) that connects to the IP printed circuit board. This simplifies and reduces the cost of testing an ADC and enables experimenting with different hammer or damper ADC resolution and sampling rates. Also, ADC parts can often be in packages that are difficult to solder. A separate circuit board lowers the cost and impact of soldering mistakes.

The IPS board comes with an optional external +5V supply connection. The external supply must be able to deliver at least five amps.

**WARNING** --- Do not connect an external +5V supply at the same time as the Teensy 4.1 USB programming cable is connected. This could short the computer connected to the USB, with the +5V supply connected to the IPS, and damage the computer. Please see the PJRC Teensy 4.1 website for more details on this issue [PJRC](http://prjc.com). The IPS circuit board has a dedicated jumper as an extra reminder. --- **WARNING**

**WARNING** --- When all 88 sensors are connected, the total current draw of the IPS will likely exceed the capability of an external computer that is trying to power the IPS through a USB cable connected to the Teensy 4.1. Therefore, the external +5V power source on the IPS is needed. The PJRC website has information on how to use a USB cable under this scenario. -- **WARNING**

## Hardware Connections
### Hammer Connections
Hammers are numbered starting at A0 (0), through C8 (87).
![spg_hammer_connections](hammer_connections.png)
### Damper Connections
Damper keys are numbered starting at A0 (0), through C8 (87).
![spg_damper_connections](damper_connections.png)
### Pedal Connections
Connect jumper wires as shown in figure.
Three of the connection wires are optional (sustain connected, sostenuto connected, and una corda connected). Also, in many cases a sostenuto and una corda pedal are not used. Therefore, up to seven keys beyond the traditional 88 keys can be connected (total of 95 keys).
![spg_pedal_connections](pedal_connections.png)

## Piano Action and Frame

Please see the following YouTube video (*NOT DONE YET*) for soldering, building, and assembly instructions.

See the [mechanical/](../../mechanical/) directory for mechanical design and instructions.

Three metal bars are needed:
* A bar to stop the hammers. This bar must be very strong and mounted to a solid surface.
* A bar for mounting the sensor (for example HPS) circuit boards to measure hammer location.
* An optional bar for mounting the sensor (for example HPS) circuit boards to measure damper location.

When mounting Stem Piano Model G inside a piano, for a silent piano system, it is possible to break each bar into smaller sub-bars to fit within the structure.

## Firmware Compiling and Running
See Teensy 4.1 and Adafruit 2.8" TFT web pages for required Arduino software setup and required libraries.

The Adafruit 2.8" TFT is using an AdaFruit library for SD card that conflicts with the library for SD card on Teensy.  As a temporary fix, delete the SdFat/ directory from Teensy library (instead of deleting it is probably better to move the directory to another location in case the directory is needed for another project in future). Deleting SdFat/ will cause warnings when building the project.

If comment out the TFT #define in stem_piano_ip2.h, then the Adafruit 2.8" TFT is not required and deleting the Teensy SD library is not required.

## Firmware Settings
See hammer_settings.cpp in [Hammer Firmware link](../../firmware/releases/ips2_hammer/). This file contains detailed comments around each setting value.

## Using MIDI
Connect a 5-pin DIN to the MIDI output port on the IPS 2.X board.

Use external software to receive MIDI, convert to musical sounds, and that sends to speakers or headphones.

## Using Ethernet
See [Analytics link](../../software/releases/ips2_analytics/) for example software that receives Ethernet data from the board.

## Touch Screen TFT Screens and Features
Change DIP switch SW2 #1 to the ON position to enable the Touch Screen TFT mode. Change the DIP switch to the not-ON position to disable the Touch Screen TFT mode. When in Touch Screen TFT mode, MIDI output from the piano is disabled.

During Touch Screen TFT mode the Analog to Digital Converter (ADC) sampling rate slows to approximately 100 samples per second. The display shows the maximum value of all hammer and damper signals, in range [0.0, ..., 1.0]. Use this capability to raise one key at a time and verify good sensor height, position, and/or calibration.

## Warnings
Please see the [WARNINGS.md](../../WARNINGS.md) file in root directory.

## Troubleshooting
Please see the [HELP.md](../../HELP.md) file in the root directory.