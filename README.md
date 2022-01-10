# DIY-Grand-Digital-Piano (GDP)

## Goal

Build a full-custom hybrid digital piano using a complete grand piano action. Ultimately I want to create and build every component. This includes both sampled and modelled piano sounds.

In the spirit of "everything worth doing is worth overdoing", the design will be intentionally overdesigned and include many marginally useful features.

This is a hobby and so the approach may be driven primarily by curiosity on a subject, and not by efficiently getting to completion.

## Why?

It is fun.

It is a great way to learn so many new things.

## History

I started this project December 5, 2020.

It was inspired, in part, by https://github.com/ekumanov/cybrid

## Block Diagram
![alt text](block_diagram.png)

## Documentation on YouTube

Along the way I have been documenting progress in YouTube:

Step 1 - Architecture https://www.youtube.com/watch?v=P3PVIuGSpIk

1. High-performance digital architecture approach.
2. Selecting the Teensy 4.1 processor.
2. Testing the CNY70 sensor, temporarily using the Teensy internal ADC.
3. Building a single-key action for testing.
4. Writing C++ on the Teensy and Python on my computer to get something working for the first time (for example, preliminary code to generate hammer strike events and velocities).
5. Testing.

Step 2 - Analog Subsystem https://www.youtube.com/watch?v=p1Ko1ZMaAus  (note the audio was accidentally recorded mono channel)

1. Selecting the Ti ADS131M08 32Ksps 24-bit 8-channel analog-to-digital converter (ADC).
2. Learning the ADC.
3. Building a test system by modifying a Ti Evaluation Module.
4. Connecting the ADC to the Teensy via SPI.
5. Writing ADC SPI driver C++ code on the Teensy.
6. Testing.

Step 3 - Digital Signal Processing https://www.youtube.com/watch?v=5eaIhX29dF4  (note the audio was accidentally recorded mono channel)

1. Designing an algorithm that converts ADC samples of the hammer position into key strike events and velocities.
2. Implement the algorithm in C++ on the Teensy.
3. Testing the algorithm with the single-key action and ADC subsystem.

Step 4 - Piano Action Frame https://www.youtube.com/watch?v=5U7LvOMth28  (note the audio was accidentally recorded mono channel)

1. Designing the physical layout of future PCB boards with respect to a grand piano action.
2. Designing the layout of circuit boards for the analog subsystem and microprocessors.
3. Building a wood frame for a piano action that includes hammer stops and sensor mounts.

Step 5 - Printed Circuit Boards https://www.youtube.com/watch?v=6EG-CYtTOFs  (note the audio was accidentally recorded mono channel)

1. Partitioning the design into two circuit boards.
2. Hammer Position Sensor (HPS 0.0) PCB: CNY70, biasing resistors, connection to microprocessor.
3. Hammer ENcoder (HEN 0.0) PCB: ADS131M08, power supply, Teensy 4.1.
4. Circuit design for both boards.
5. PCB layout for both boards.
6. Fabrication at OSH Park.

Step 6 - Learning to Solder https://www.youtube.com/watch?v=wlrAXrDVhN0  (note the audio was accidentally recorded mono channel)

1. After receiving the circuit boards, I had difficulty hand soldering small surface mount parts.
2. So, I took time to learn the science of soldering.
3. Practicing soldering.

Step 7 - HPS PCB (CNY70) https://www.youtube.com/watch?v=usbgk9fsZ5I  (note the audio was accidentally recorded mono channel)

1. Finalize selection of biasing resistors.
2. Document design process for selecting biasing resistors.
3. Solder CNY70 and resistors on the board.
4. Testing.

Step 8 - HEN PCB https://www.youtube.com/watch?v=tCwKCgKxA0A

1. Design new (HEN 1.0) circuit board to add a clock and change physical orientation.
2. Circuit design and layout.
3. Soldering parts on the new HEN printed circuit board.
4. Continue C++ development for the Teensy.
5. Bringing up the board and testing.
6. Full system test with HPS 0.0 and HEN 1.0
8. Write Python code on my computer to generate MIDI signals for sound generation through an external digital keyboard.

Step 9 - Raspberry Pi 4B https://www.youtube.com/watch?v=q-dFU-f_Ju8

1. Investigate the possibility of using a Raspberry Pi 4B in place of the Teensy 4.1.
2. Modify the Teensy C++ code to include a lower layer that abstracts the hardware.
3. Test with Ti Evaluation Module from Step 2.
4. Get system running.
5. Investigate but do not solve challenges with obtaining sufficiently fast real-time performance from Linux.
6. Although I made no physical changes to the Pi, for reference I am calling this the Raspberry Processing System (RPS).

Step 10 - Testing with Piano Action https://www.youtube.com/watch?v=ANJI4KAolEw

1. Layout three new HPS variations: HPS 0.1 changed the header spacing, HPS 0.2 increased the length slightly to get better alignment over the hammer shanks, HPS 0.2D is with the After Dark OSH Park fabrication process.
2. Investigate slight changes to bias resistors. Based on discoveries when tested with piano action.
3. Connect 8 Hammer Position Sensor (HPS, various versions) boards to 1 Hammer ENcoder (HEN 1.0) board.
4. Mount on the frame from Step 4 and place on a physical grand piano action.
5. Precisely locate each hammer stop position to exactly match hammer position when action is in the piano.
6. Continue improving the Teensy (HSP) C++ code and computer Python code.
7. Test.

Step 11 - Latency https://www.youtube.com/watch?v=k5ro7su6gYo

1. Investigate a problem with latency discovered during Step 10.
2. Quickly learn this was due to very large Ethernet packets containing hammer samples for visualization.
3. Remove that code (it was temporary for testing).
4. Measure the system latency.

Step 12 - Board Split https://www.youtube.com/watch?v=3AoEzj19FGQ

1. During the course of this project I am learning that soldering is very time consuming.
2. Want to avoid soldering 88 HPS boards and 11 HEN boards, then needing a board spin.
3. Decide to split the HEN into two boards.
4. New Board #1: Teensy Processing System (TPS) - contains the Teensy, Ethernet, and a new LCD (marginally useful features!).
5. New Board #2: Eight Channel Analog (ECA) - contains the Ti ADS131M08, associated support circuitry, and connections to the HPS.
6. Advantage of the split: TPS is easy to solder and likely to change. ECA is difficult to solder and unlikely to change.
7. Design and layout of TPS 0.0.
8. Soldering and bringing up the TPS.
9. Testing the TPS, discover a problem with the LCD write times.
10. Design a solution to long LCD write times.
11. Changes to Teensy C++ code to workaround the write time issue.

Step 13 - Eight Channel Analog https://www.youtube.com/watch?v=e2ARDHr45-I
1. Soldering and bringing up the ECA (version 0.0).
2. Testing the ECA with the TPS and associated Teensy 4.1
3. Testing the ECA with a Raspberry Pi 4B (named the RPS).

The following are in progress or planned, and the order may change:

Step 14 - Network and PPS (in progress, 80% finished)
1. Design a network protocol for sending hammer strike events (basically, MIDI information) and high-speed streaming hammer samples (for data analysis and visualization).
2. Design the protocol such that it avoids the latency issues seen in Step 11.
3. Writing protocol C++ code for TPS/RPS.
4. Writing Python for the Piano Processing System (PPS) portion of the design.
5. The PPS is running on another Raspberry Pi 4B and is responsible for converting hammer strike events into piano sounds, the MIDI interface, and other similar features.  The PPS is intended to implement features found on a modern digital keyboard.
6. Presently the PPS only generates MIDI. Other features are for the future.
7. Writing Python and Matlab / Octave code for my desktop computer to receive streaming samples, store them in a temporary cache, and plot the data in real-time. This is all temporary code for tesing.
8. Redo the ECA layout so it works better with a Raspberry Pi 4B.
9. Use ECA 0.0 with TPS 0.0 and ECA 0.1 with RPS.
10. Testing.

Step 14 - TPS 0.1 and Case (in progress, 10% finished)

1. I plan to design a different LCD approach, that avoids the timing issues discovered in Step 12.
2. Test LCD options.
3. New firmware to support LCD.
4. Layout a new TPS that works with ECA 0.2 (modification planned for hand soldering considerations) and simultaneously design the physical case.
5. Circuit design, layout and fabrication.
6. Test.

Step 15 - Action #2 (pending)
1. I am getting close to acquiring a grand piano action dedicated to this project (previously was.... very rarely (twice?) ... and super carefully!! ....removing my existing piano action for checking fit).
2. Build a new physical frame, hammer stop, and sensor mounts for this action.
3. Make any PCB or case changes as required.

Step 16 - At some point I need to document the firmware and software.
1. C++ on TPS (Teensy 4.1) - real-time data processing.
2. C++ on RPS (Raspberry Pi 4B) - real-time data processing.
3. Python on PPS (Raspberry Pi 4B) - digital keyboard features.
4. Matlab / Octave on desktop computer - analytics and visualization.
5. Python / Javascript on desktop computer - analytics and visualization.

Step 17 - Jigs for soldering (future)
1. Faster and easier soldering.
2. Better control the CNY70 orientation for board-to-board consistency (my auto-calibration code can only solve so many problems).

Step 18 - Pedal Unit (future)

Step 19 - Solder and assemble one complete system (future)

Steps 20 through ?? - PPS code to remove need for external MIDI connection and generate the piano sounds internally (future)

Steps ?++ - I have a long list.... or.... maybe.... just play the piano and start a new EE/CS project.