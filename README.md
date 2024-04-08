# Hybrid MIDI Digital Piano

Also known as **stem piano**

![alt text](documentation/ips00_stem_piano_small.jpg)

## How to Build Your Own Piano
Go straight to the [pianos/](pianos/) directory!

But first, please read the [WARNINGS](WARNINGS.md) and [HELP](HELP.md) files that are located in this root directory.

See the [STATUS](STATUS.md) file for status of the project and repository.

If you would like to contribute hardware, software, mechanical, or firmware to the project, see the [CONTRIBUTING](CONTRIBUTING.md) file.

**If you build a piano, please share about it in the Show & Tell Discussions of this repository.**

## Overview

A hybrid piano combines the physical action of an acoustic piano with MIDI, sound generation, and other features of a digital keyboard. Some piano manufacturers sell hybrid pianos.

This project ("*stem piano*") is an open-source hybrid digital piano.

The qualifier digital is important. It means that signals are sampled with an analog-to-digital converter (ADC) and then all processing is with a microprocessor. Therefore, once the hardware is installed all algorithms, improvements, and new features are with software or firmware.

The repository includes schematics, printed circuit board layouts, firmware source files for the real-time processing, optional software source files for Ethernet connected features, and lists of required hardware components.

A physical piano action, or other instrument, is required. The electronics and code (*stem piano*) will work with a grand or upright piano action. Mechanical suggestions are included but not detailed mechanical instructions.

The platform and code could also adapt to building a range of other unique and innovative musical instruments.

## Directory Organization

The components required to build a piano are located in the [firmware/](firmware/), [hardware/](hardware/), [mechanical/](mechanical/) and [software/](software/) directories. In each directory is a *research* subdirectory and a *releases* subdirectory.

Each *research* subdirectory contains code and hardware for work in progress. It could have major bugs. It could be incomplete.

Each *releases* subdirectory contains code and hardware ready for use. The material in *releases* could partially duplicate material in one or more *research* subdirectories. However, the requirement is that anything in *releases* is well-tested, complete, and ready-to-use.

More details on each directory:

#### [design/](design/)

Designs and engineering analysis. To keep the repository small, associated data may be located in the separate Piano-Data repository.

#### [documentation/](documentation/)

High level documentation.

#### [pianos/](pianos/)

Building a complete piano requires firmware, hardware, mechanical, and sometimes software. The *pianos/* directory provides information on everything needed to build a specific version of a musical instrument.

#### [firmware/](firmware/)

Real-time code running on custom hardware from *hardware/* directory.

#### [hardware/](hardware/)

All printed circuit boards and related parts and design. A separate open-source license for the hardware is in this directory and applies to all hardware.

#### [mechanical/](mechanical/)

Anything to physically build a piano. A separate open-source license for mechanical is in this directory and applies to anything mechanical.

#### [software/](software/)

Code running on a standard computer or Raspberry Pi connected to the piano over Ethernet or MIDI.

## History

Started December 2020.

Please see [documentation/video_documentation.md](documentation/video_documentation.md) for historical, how-to, and other documentation in video form.

## Related Projects:

Work was inspired, in part, by https://github.com/ekumanov/cybrid

See also:

* https://github.com/jkominek/piano-conversion/
* https://github.com/davidedelvento/Mybrid
* https://www.youtube.com/watch?v=nRBsudMpwwc
* https://www.youtube.com/watch?v=-kpf0OSJxkE
* https://www.youtube.com/watch?v=b-PRGymqECM

## "stem piano" because?

* science technology engineering math
* like a plant stem, everything here is one component of the overall instrument
* like a note stem, everything here is one component of...
* 12 leaves on the logo, matches 12 piano notes
* logo looks like a sine wave
* sounds cool
* does not sound cool