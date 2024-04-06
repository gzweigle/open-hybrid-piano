# Build Instructions for Stem Piano G

Step-by-step instructions to build a complete piano.

No separate damper board.

## (0) Prepare

* Read this and other documentation (including help and warning files).
* Get familiar with components such as the Teensy, TFT display, MIDI, Ethernet, etc.
* Have some knowledge of Arduino project concepts.
* Have some knowledge of electronics projects.
* Could be helpful to watch relevant historical YouTube videos [https://www.youtube.com/@gzpiano88](https://www.youtube.com/@gzpiano88)
* Attend Juilliard, Curtis Institute of Music, or Berklee College of Music for eight years and get a PhD in piano.

## (1) Manufacture The Circuit Boards

### (1.A) Select a circuit board manufacturer

### (1.B) Send the mainboard (IPS) Gerber files to the PCB manufacturer and order one board

* File location: [../../hardware/releases/ips20/ips20_gerber/](../../hardware/releases/ips20/ips20_gerber/)

### (1.C) Send the analog board (SCA) Gerber files to the PCB manufacturer and order one board

* File location: [../../hardware/releases/sca00/sca00_gerber/](../../hardware/releases/sca00/sca00_gerber/)

### (1.D) Send the sensor board (HPS) Gerber files to the PCB manufacturer and order 88 boards

* Option #0 - The board with a trimming resistor. This board is more expensive but gives greater control over note accuracy. File location: [../../hardware/releases/hps07/hps07_gerber/](../../hardware/releases/hps07/hps07_gerber/)

* Option #1 - The board without a trimming resistor. This board is less expensive but runs the risk of firmware calibration not adequately able to account for sensor nonlinearity or mechanical tolerances. One can always try the less expensive option first and then upgrade to the more expensive option at a later time. File location: [../../hardware/releases/hps04/hps04_gerber/](../../hardware/releases/hps04/hps04_gerber/)

* Option #2 - Any sensor board that meets the interface specification will work. However, the instructions in this file are for an HPS board.

## (2) Assemble The Circuit Boards

* Parts are listed with part numbers in the parts files below. Parts are listed but without part numbers for LEDs, 0.1-inch standard headers, +5 volt barrel jack inputs, discrete resistors, and MIDI connectors. For the LED, select a type/color and add the part number. For the other parts, work with the board assembly manufacturer or purchase and send to the manufacturer.

* For resistors, capacitors, and fuses it is ok to substitute reasonably close, or better, equivalents.

* If hand soldering the boards, see *HAND SOLDER* at the end of this file.

### (2.A) Select a manufacturer to install the components

* Work with the manufacturer on parts acquisition.
* The manufacturer may supply some parts such as integrated circuits, resistors, and capacitors.
* Some parts may need ordering separately such as the MIDI connector or pedal quarter-inch jack inputs.

### (2.B) Assemble the IPS board

* Parts File: [../../hardware/releases/ips20/ips20_bill_of_materials.txt/](../../hardware/releases/ips20/ips20_bill_of_materials.txt/)

* If using Ethernet:
  * The 2x3 pin 2mm header is hand soldered onto the Teensy. See video on my YouTube channel for an example at approximately the 3:00 time: [https://www.youtube.com/watch?v=M1_228-ClXM](https://www.youtube.com/watch?v=M1_228-ClXM).
  * One of the two 2x3 pin 2mm socket headers is not soldered in place. It is stacked on the other. See same video for example.
  * If unsure about these steps, it is ok to skip Ethernet. Can add later anytime.

### (2.C) Assemble the SCA board

* Parts File: [../../hardware/releases/sca00/sca00_bill_of_materials.txt/](../../hardware/releases/sca00/sca00_bill_of_materials.txt/)

### (2.D) Assemble 88 HPS boards

* 88 are required.

* Parts File: [../../hardware/releases/hps04/hps04_ips2x_bill_of_materials.txt/](../../hardware/releases/hps04/hps04_ips2x_bill_of_materials.txt/)

* Parts File: [../../hardware/releases/hps07/hps07_ips2x_bill_of_materials.txt/](../../hardware/releases/hps07/hps07_ips2x_bill_of_materials.txt/)

### (2.E) Connect IPS and SCA

* After boards arrive from assembly.
* Connect the SCA board to the IPS board. It should snap into place.
* See this video on my YouTube channel for an example: [https://www.youtube.com/watch?v=NmziaIYKS1g](https://www.youtube.com/watch?v=NmziaIYKS1g).

### (2.F) Install the Teensy 4.1

* Purchase a Teensy that includes pins or hand solder the pins onto the Teensy.
* The Teensy is socketed. It should snap into place.

### (2.G) Install the TFT display

* This is an optional step. The piano works without a TFT installed.
* May need to solder pins onto the TFT.
* The TFT is socketed. It should snap into place.

## (3) Get the Firmware to Compile

### (3.A) Install Arduino software

### (3.B) Add libraries

* See [https://www.pjrc.com/](https://www.pjrc.com/) for instructions related to the Teensy 4.1.
* If using Ethernet, install anything else needed per [https://www.pjrc.com/](https://www.pjrc.com/).
* If using the 2.8" TFT, install libraries per the manufacturer. See the parts file listed above for the manufacturer name and part number. Then, visit their web page for instructions.
* For simplest initial board bring up, it may be easiest to skip Ethernet and TFT. Can add later.

### (3.C) Download and edit source files

* Since these instructions are for a single board that includes both damper and hammer functionality, it is not necessary to place common code in a separate Arduino library.
* Get the code from the following locations and save into the Arduino project folder on computer:
    * [../../firmware/releases/StemPianoIPS2/](../../firmware/releases/StemPianoIPS2/)
    * [../../firmware/releases/ips2_hammer/](../../firmware/releases/ips2_hammer/)
* Rename *src_hammer.cpp* to *src_hammer.ino*.
* Edit *hammer_settings.cpp* and put values into the Teensy and computer IP addresses. If not using Ethernet, any numbers are ok. Comment out the line about needing to edit the Ethernet values.
* Edit *hammer_settings.cpp* and set *debug_level = 1*.
* Adjust these settings in *hammer_settings.cpp* based on analog-to-digital (ADC) converter decisions.
    * *using18bitadc* - Set to false if 16-bit ADC. Set to true if 18-bit ADC.
    * *sensor_v_max* - Set to 3.3.
    * *adc_reference* - Set to 3.3.
* Open *stem_piano_ips2.h*. If not using the TFT, comment out the line *#define TFT_INSTALLED*. If not using Ethernet, comment out the line *#define ETHERNET_INSTALLED*. For simplest initial board bring up, it's best to comment out both lines. Can add later.

### (3.D) Run the compiler

* Use the Arduino software.
* Select Teensy 4.1 as the board.
* Compile the project without selecting the download step.
* If it does not compile, see [../../../HELP.md](.././../HELP.md) file and the repository Issues list. Also, check Arduino and Teensy web pages.
* If it does compile, the boards are now ready to install firmware.

## (4) Test The Firmware and Hardware Together

### (4.A) Setup
* Do Not Connect the +5V power for this step. The board is powered through the Teensy 4.1 USB cable.
* Do not install jumper J12.
* Make sure all switches are in their off position. If switches were not installed, this step is not necessary.
* There is no need to connect a MIDI or Ethernet cable for this step. However, if *#define ETHERNET_INSTALLED* is not commented out in *hammer_settings.cpp* then connect an Ethernet cable or else the board may hang while waiting for Ethernet to initialize.

### (4.B) Connect a single HPS board

* For simple initial test, connect a single HPS board to the IPS board.
* Each HPS board is marked with a plus (+) symbol, a minus (-) symbol, and an exclamation point (!) symbol.
* The pins on the IPS board are market with a "3.3V" symbol, a "gnd" symbol, and a "in" symbol.
* Select the triplet of pins on the IPS board that are closest to the +5V power input and connect (+) to "3.3V", connect (-) to "gnd", and connect (!) to "in". These pins are for the lowest note on a piano, which is A0. It is data for index 0 in the firmware.

### (4.C) Connect USB to Teensy and power up

* Connect a USB cable to the Teensy. Because the board is powered through the USB cable, use a normal USB cable without modification (later steps may require modifying a cable to remove the power connections).
* Connect the opposite side of the USB cable to a computer.
* The IPS, SCA, and a single HPS board are now powered!
* Optionally measure the voltage at the 3.3V pin on lower right of board with a multimeter to check that it reads 3.3 volts. If it does not, disconnect USB cable from computer and see [../../../HELP.md](.././../HELP.md) file and the repository Issues list for help.

### (4.D) Download

* Compile/download the firmware using Arduino software.
* May need to select a serial port in Arduino software.
* May need to push the download button on the Teensy the first time firmware is downloaded.

### (4.E) Test it works?

* Open a serial monitor in Arduino. A set of startup messages should display from the firmware running on the Teensy. If not, the problem could be with the Arduino serial connection. Or, the problem could be with the board. See [../../../HELP.md](.././../HELP.md) file and the repository Issues list.

* Use a rectangle of white cardboard and try to "play a note" by quickly moving the cardboard toward the CNY-70 sensor. See my YouTube video at approximately the 2:00 time: [https://www.youtube.com/watch?v=NmziaIYKS1g](https://www.youtube.com/watch?v=NmziaIYKS1g).

* A message should appear in the Arduio serial monitor that a note was sent to MIDI.

* Try connecting a pedal to one of the quarter-inch pedal inputs. The pedal inputs require a wire connection from the pedal header to one of the input headers. See the [README.md](README.md) file in this directory for details.
  
* When press a pedal the Arduino serial monitor should display a message that that the pedal was pressed and a value was sent over MIDI.

### (4.F) Test with a MIDI cable

* Power down.
* Connect a MIDI cable from IPS to a computer.
* Run software on an external computer that converts MIDI to sound. Probably should first check that this is working with an existing MIDI keyboard.
* Power up.
* "Play the note" again. Using the cardboard. Should hear a sound.
* Try it with a sustain pedal. Should sustain.

### (4.G) Celebrate, just a little

* If at this step, congratulations, most of the project risk is eliminated.
* To eliminate even more risk, test other HPS boards and other IPS input pins. Test several simultaneously.
* As more HPS sensor boards are connected to the IPS board, the total current draw of the system increases. The computer probably cannot deliver sufficient current through the USB cable when the number of connected HPS boards passes a certain threshold. That threshold is computer-dependent. See below for adding the +5V cable.

## (5) Build The Piano Action Frame

* The steps below are for a Grand Piano action.
* If using an upright action or another musical idea (perhaps a new invention?) then the Grand Piano instructions can give general guidance.
* Information on the frame: [../../mechanical/README.md](../../mechanical/README.md)

### (5.A) Acquire a piano action.

### (5.B) Build a frame that holds two bars
* See the following videos on my YouTube channel for examples:
    * [https://www.youtube.com/watch?v=xohaQG593e4](https://www.youtube.com/watch?v=xohaQG593e4)
    * [https://www.youtube.com/watch?v=NmziaIYKS1g](https://www.youtube.com/watch?v=NmziaIYKS1g)
* Both bars must be adjustable vertically and horizontally.

### (5.C) Install a heavy bar for the hammer stop

* Use a strip of firm material to absorb the force of shank against the bar. For example, weatherstripping works ok.
* Position the hammer stop bar horizontally as close to each hammer as possible.
* Adjust the hammer stop bar vertically until it is approximately 1/16" above the shank drop point. So, when pressing a piano key, move the key down slowly and adjust the bar so the hammer shank does not quite hit the stop bar. Try to get an even distance for all 88 hammer shanks.
* See this video at approximately the 1:30 time: [https://www.youtube.com/watch?v=ANJI4KAolEw](https://www.youtube.com/watch?v=ANJI4KAolEw)

### (5.D) Install a bar for the HPS sensors

* Drill 88 holes in the sensor rail. Each hole is located vertically above a hammer shank.
* Horizontally, the hole is such that the CNY-70 on the HPS is close to the place where the shank rotates on the pin adjacent to the drop screw. The shank may get wider at this location.
* Install all 88 HPS sensors.
* Adjust the sensor rail up and down until the hammer shank is approximately 1 millimeter away from the sensor when at closest. If the distance is smaller than 1 millimeter, the CNY-70 response loses sensitivity.
* Good note-to-note consistency depends on the consistency of shank-to-sensor distances for each sensor. It is a good idea to spend extra time making these distances as consistent as possible. One approach is using thin paper shims when connecting the sensors to the sensor rail.
* If the shank wood is not very reflective, place white stickers on each shank under the CNY-70 location.

### (5.E) For best results, get the piano action regulated while in the configuration described above

## (6) Connect the Hardware and the Action

### (6.A) Connect all HPS sensors

* Wire all 88 sensors to the pin headers on the IPS mainboard.
* See the [README.md](README.md) file in this directory for connection instructions.

### (6.B) Connect a sustain pedal

* Wire all pedal pin headers to the input pin headers (both are on the IPS mainboard).
* See the [README.md](README.md) file in this directory for connection instructions.

### (6.C) Change USB cable

* With all 88 HPS boards connected, the external computer connected to the USB cable probably cannot supply the required current. The board must be powered by an external +5V power supply. Two options:
    * Remove USB cable. Install J12. Connect a +5V power supply to the +5V input jack. Advantage: simplicity. Disadvantage: cannot re-program Teensy.
    * Modify the USB cable to disconnect the power line. See [https://www.pjrc.com/](https://www.pjrc.com/) for details. Install J12. Connect a +5V power supply to the +5V input jack. Advantage: can program Teensy. Disadvantage: must keep track of the cable and always use it.
* No need to re-download software, as this was done in Step #4 above.

### (6.D) Keep the MIDI cable connected

### (6.E) Optionally connect Ethernet

### (6.F) Apply power to the +5V power input

## (7) Play The Piano

* Press a piano key. If it works, stop, eat pizza, celebrate.
* Press all 88 piano keys. If they all work, stop, eat even more pizza, celebrate.
* If it does not work. See the [../../../HELP.md](.././../HELP.md) file and the repository Issues list. Order lots of pizza, it's going to be a long weekend.
* Because every action is a little different, here are the most likely settings in *hammer_settings.cpp* that could require adjustment:
    * *velocity_scale* - Make larger if volume is very quiet. Make smaller if volume is too loud.
    * *damper_threshold_using_hammer* - Make larger if notes sustain after key release. Make smaller if notes do not sustain while a piano key is pushed. If some notes sustain after release and some notes do not sustain at all, this means the sensor physical placement has too much variation from note-to-note.
    * *strike_threshold* - If no notes work, try decreasing.

## (8) External Software on an External Computer (optional)

... will add these instructions in the future ...


## (HAND SOLDER) Instructions for if hand soldering all parts

* Alternate instructions for Step #2.
* Will add these instructions in the future.