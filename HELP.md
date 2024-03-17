# DIY Hybrid Digital Grand Piano - HELP!

I put quite a bit of effort into making the design as foolproof as possible. However, when things do not work, it can be frustrating. The list below covers possible issues and how to resolve.

Before building boards take a look at any pending hardware changes or known problems in [Issues](https://github.com/gzweigle/DIY-Grand-Digital-Piano/issues). If there is a known hardware problem, it is better to understand it before spending money on making a piano.

Similarly, if the piano is not working properly, check if the symptoms are covered in the [Issues](https://github.com/gzweigle/DIY-Grand-Digital-Piano/issues) list. This could provide help in fixing.

In addition to the information below, since starting the project I have been documenting progress in YouTube videos. Please see [documentation/video_documentation.md](documentation/video_documentation.md) for links to the videos. These videos are a historical source of information that could be helpful when trying to debug problems.

There are a couple of piano forums and also the https://forum.pjrc.com/ that could be good sources for information and questions.

If hand-soldering the boards, a great approach is to get things working in stages. Building an entire piano to completion and then powering up everything together for the first time can make finding small problems very difficult. The incremental approach is covered in some of the build instructions and videos.

The board includes test points and LEDs that are programmable with user-defined functionality in firmware to help with debugging. Also, the Arduino serial window.

Now for the help list...

## Compiling

Check the https://www.pjrc.com website for instructions on setting up to compile and download for the Teensy 4.1.

Make sure the latest Arduino software is installed and with updated libraries.

## Touch Screen and Compiling

For most pianos the touch screen TFT display is optional. It is added to the project with a #define in one of the source files.

The touch screen TFT display requires installing special libraries. See the manufacturer's website for information. If the project fails to compile, try to #define out the TFT code

Some touch screen TFT display SD libraries conflict with the Teensy SD library. Details are found in the build instructions elsewhere in this repo. If the project fails to compile, try to #define out the TFT code.

## Booting

Check the https://www.pjrc.com website for instructions on setting up to compile and download for the Teensy 4.1.

If the project fails to download to the Teensy, try pushing the download button on the Teensy  (see https://www.pjrc.com website for details).

If there is a buffer overflow bug in the firmware, this can cause the Teensy to appear to be locked up, or, to continuously initialize.

If using multiple boards (hammer board and damper board), with two instances of Arduino software running and a connection to each board, it is possible to accidentally download code to the wrong board (for example, hammer firmware downloaded to the damper board). One way to check that the correct code is going to the correct board is to have both serial output windows open in the Arduino software. Verify that the board type (hammer or damper), which is printed on the serial output, matches the firmware type (hammer or damper) intending to be downloaded.

## The Piano Does Not Work At All

Use a multimeter or oscilloscope to verify +5V and +3.3V locations on the circuit board are getting power.

Check that all integrated circuits are installed with pin 1 matching the * printed on schematic.

If the Teensy is socketed, check if it was accidentally installed rotated 180 degrees.

Some header files include code that #define out functionality. Check the #defines in header files (for example stem_piano_ips2.h) to see if this happening.

A buffer overflow in the firmware can lock-up the board. Check any changes to firmware that memory access is staying in proper ranges.

If the Ethernet is not physically connected, the firmware may hang while trying to initialize the Ethernet. Try commenting out the Ethernet initialization code or try connecting an Ethernet cable to the board.

If a circuit component was Electrostatic Discharge (ESD) damaged during handling or soldering then a wide range of unusual or intermittent failures are possible. If all other debugging avenues are exhausted, ESD could be the problem.

Some circuit boards use the Teensy internal pullup or pulldown resistors instead of an external resistor. Search the firmware for which pins are set in these modes. Then, check that the Teensy pullup or pulldown is working correctly by debug options such as:
* Try a different Teensy processor.
* Put a volt meter on the pin and see if the value matches the value expected in the firmware.

Never let the smoke out of any integrated circuit.

## Sound

No sound:

* The firmware includes code that disables unused hammer / damper inputs. Check that these inputs are not disabled in the firmware.
* Set the debug level (for example see stem_piano_ips2.h) to its maximum value. Open a serial port in Arduino software while the piano is running. Compare messages to what is expected.
* If the distance from the hammer shank or damper to the sensor is too large, the signal will not cross detection thresholds in the firmware.
* Try disconnecting the MIDI cable from *stem piano* and then connecting the MIDI cable to another keyboard, see if that works.
* Put an oscilloscope on the circuit board test point that is at the Analog to Digital Converter (ADC) input. Verify getting a signal when move a piano key.
* Check soldering of signals in sensor data path.
* Check that integrated circuits are installed with pin 1 matching the * printed on schematic.
* Turn on speakers.

Very quiet sound:
* Check the physical distance from the hammer shank or damper to its sensor.
* Set the debug level (for example see stem_piano_ips2.h) to its maximum value. Open a serial port in Arduino software while the piano is running. Look at the volume level printed for MIDI when press a key. If it is small then the problem is with the piano. If it is large then the problem is with equipment connected to the piano.

Very loud sounds:
* If the firmware includes an automated gain control function, check if a bug is causing too much gain.
* Turn volume on speakers down.

## Playability
 
Uneven piano dynamics
* Any variation between the distance from the sensor to the hammer, from key-to-key, may cause a variation in the volume between the keys. It is important to get the sensor to hammer distance as consistent as possible between the keys.
* Variation in the piano action can cause volume variation between keys. For example, if one hammer shank is slightly higher than another, this can reflect into the piano dynamics.
* For best dynamics, the piano action regulation should be to the hammer stop bar and sensors.

Random dropped or sounding notes
* Look at the comments around settings in the settings file and consider making changes.
* External light sources on the sensors can cause the sensors to get false signals. Try shading the sensors from any external light sources.

Note repetition is erratic
* Look at the comments around settings in the settings file and consider making changes.
* Check the spacing between the sensors and the hammer shanks or dampers.

Some notes work, some do not
* Check connections at the sensor at piano key not working.
* The firmware includes code that disables unused hammer / damper inputs. Check that these inputs are not disabled in the firmware.
* Look at the comments around settings in the settings file and consider making changes.
* Check that the sensors are getting power.
* Check the sensor output with a multimeter or oscilloscope. When a hammer shank or damper is close to the sensor the output voltage should be close to the ADC voltage reference value. The voltage reference value is found either in a hardware bill of materials file (in this repository) or, if the reference is internal then the analog-to-digital converter datasheet.
* Check for proper soldering of signals in the sensor data path.

One or more notes play repeatedly
* The threshold for note could be too low.  Look at the comments around settings in the settings file and consider making changes.
* The analog circuitry is very noisy. This is probably due to one of the following:
    * bad electrical connections or solder joints.
    * wrong component installed on board: for example, a capacitor has the wrong value.

Some notes are quieter or louder than others -
* Check the sensor spacing to the hammer shanks or dampers.
* Check the alignment of sensors over hammer shanks or dampers.

## Pedals

Pedals do not work
* Check that the pedal wiring matches the pin numbers in the settings file.
* Some pedal firmware could include functionality to check if the pedal is connected. Check for an incorrect pedal wire connection or a problem with the firmware.
* Look at the comments around settings in the settings file and consider making changes.
* Check soldering of pedal connections and resistors.

Pressing a pedal causes a note to play
* The pedal is probably connected to one of the inputs that are configured to be for hammers.

Pressing a piano key causes a pedal function to activate
* The hammer sensor is probably connected to one of the inputs that are configured to be for pedals.

Sustain, sostenuto, or una corda do not release
* Look at the comments around settings in the settings file and consider making changes.

## Ethernet

Ethernet does not work
* Do the addresses in settings file match the computer address?
* Does the code on computer receiving Ethernet data have the same addresses as in settings file?
* Is the piano or computer connected to the correct subnet? Look at the IP addresses.
* If using a VPN on the computer, try turning it off temporarily.
* Try Wireshark to debug.
* Make sure the Ethernet cable is connected.
* If going through a switch or router, make sure they are plugged in and getting power.
* Check soldering.

## MIDI

MIDI command overload
* Restart everything with MIDI disconnected, then connect.
* Try disconnecting MIDI from *stem piano* and connecting it to another device with MIDI output to the MIDI receiver.
* Check circuit board for signs of a bad solder of MIDI resistors.

No MIDI messages received by MIDI receiver
* Check connections.
* Restart everything with MIDI disconnected, then connect.
* Try disconnecting MIDI from *stem piano* and connecting it to another device with MIDI output to the MIDI receiver.
* Are the MIDI resistors soldered into place?

## TFT Touch Screen

TFT display is blank
* Is the display configured to be on? (details for turning display on and off are found in the build instructions elsewhere in this repo).
* Make sure the display is properly connected to the board.
* Check soldering.

TFT works but touch screen features do not work
* The touch screen feature may be an option for the display. Make sure the display was purchased with the touch screen.

TFT SD card does not work
* Try a different SD card
* Make sure the pullup resistors on the circuit board are soldered properly into place.

## Board 2 Board Communication

If the Can bus is not working, check the soldering.

Check Can bus wire connection match between hammer and damper boards.

Make sure both circuit boards are powered up and operating correctly.

Check that the Can bus integrated circuit is installed with pin 1 matching the * printed on schematic.