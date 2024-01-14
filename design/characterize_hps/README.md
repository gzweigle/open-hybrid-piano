# Hammer Position Sensor (HPS) System Characterization

## Purpose

Construct the *stem piano* measurement system response from experimental data. The measurement system response consists of:
* CNY70
* Hammer shank reflection properties
* Physical position and orientation of CNY70

Data is useful for understanding nonlinearities, noise, distortion, and tolerances of the hammer position measurement system.

## Setup

### stem piano
Code running on piano must include streaming raw analog-to-digital (ADC) counts over Ethernet. The *Stem Piano G* release supports this.

Edit the data placed in Ethernet frame by *stem piano* so that the raw ADC counts for the hammer sensor under test are in the first 24-bits of Ethernet data.

### Hammer Crane
Linear actuator and controller with an Arduino Nano. Actuator is attached by a thread to a hammer shank. Pulls shank up in a controlled manner.

### Analysis Code
Construct the measurement system response from *stem piano* measurements received over Ethernet while the *Hammer Crane* is moving the shank

## Running It!

### Getting Data

Attach a thread from the *Hammer Crane* to a hammer shank. Use a lightly attached piece of tape so that if the *Hammer Crane* accidentally tries to pull the shank past the hammer stop, it does not break the shank.

Turn on *stem piano*. Make sure *stem piano* is streaming raw ADC samples for the shank connected to *Hammer Crane*.

Run [analog_analysis/get_hammer_cal_data.py](analog_analysis/get_hammer_cal_data.py).

Start the *Hammer Crane* code. This lifts the hammer shank from resting position until contact with the hammer stop.

The measurement values are stored in *data/hammer_cal_data.txt*.

### Analyzing Data

Rename *hammer_cal_data.txt* to something describing the particular test setup. Record the test setup information in [data/README.md](data/README.md).

Run [analog_analysis/generate_analog_response.m](analog_analysis/generate_analog_response.m) from within Octave or Matlab. First edit the .m file so the data file name matches the file name selected in previous step.

The result is a curve of measurement system response vs. hammer shank position.

Optional: Run [analog_analysis/combine_analog_responses.m](analog_analysis/combine_analog_responses.m) from within Octave or Matlab. First edit the .m file so the data file names all matches the file names from previous tests.

The result is a set of measurement system responses vs. hammer shank position curves, resampled and normalized for comparison.