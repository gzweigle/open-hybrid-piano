# UDP Ethernet Data Acquisition and Display

## To get data from hammer board

Edit *get_hammer_data.py* and add IP address and port number.

These must match the values in the board's settings .cpp file.

From a command line type: *python get_hammer_data.py*.

## Settings

In get_hammer_data.py set the following values:

* file_length: number of samples to receive - this is also the hammer_position.txt file length.
* note_number_min: (0 = A0),..., (87 = C8),..., (95 = last of analog muxes).
* note_number_max: (0 = A0),..., (87 = C8),..., (95 = last of analog muxes).

## To plot the data

After the data is acquired.

Run Octave or Matlab run.

Type in command line:

*clear; x = load("hammer_position.txt"); plot(x); grid;*

In this directory is an example hammer_position.txt output. The example output was generated using the get_hammer_data.py with settings in the file.