# Hammer Position Sensor System Data Analysis

Directory contains three source files.

## get_hammer_cal_data.py

Get the Ethernet data from *stem piano* and store in a file.

The format of data streamed from *stem piano* must match the format expected in *get_hammer_cal_data.py*.

Before running, edit code to set:
* Local computer Ethernet address
* Teensy 4.1 port number.

The checked-in code will error until these are set.

## generate_analog_response.m

Generate a combined HPS, CNY70, and analog subsystem response curve vs. hammer shank position.

Code functionality:

Load the raw ADC values that resulted from the following.
* The linear actuator starts with hammer at rest position.
* The linear actuator pulls the hammer shank higher for *t_tr* seconds.
* The linear actuator pauses for *t_hold* seconds.
* Repeat until the hammer shank hits the hammer stop bar.

These ADC values from *stem piano* were received by get_hammer_cal_data.py and stored in a file that *generate_analog_response.m* reads.

With that data, the code finds all the hold intervals then averages the raw ADC values recorded during the hold intervals.  Collect those results into the CNY response vs. position curve. Save the result to a file.

Note that an alternate approach could have been for the Hammer Crane Arduino Nano to signal *stem piano* hardware (for example a test point converted to an input) on each transition. Then, *stem piano* could have encoded that signal into the Ethernet stream. Makes the physical system more complex and the *generate_analog_response.m* code simpler. The approach selected here is to keep the hardware simple and parse the hold intervals with somewhat complex *generate_analog_response.m*.

## combine_analog_responses.m

The number of sample points of the linear actuator is not consistent between runs. Resample to the one with the least number of samples. Then display.