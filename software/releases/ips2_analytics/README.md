# Ethernet Data Acquisition

## To get data from hammer board

Edit get_hammer_data.py and add IP address and port number.

These must match the values in the board's settings .cpp file.

From a command line type: python get_hammer_data.py.

## To plot the data

After the data is acquired.

Run Octave or Matlab run.

Type in command line:

clear; x = load("hammer_position.txt"); plot(x); grid;