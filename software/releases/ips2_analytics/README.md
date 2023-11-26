# Ethernet Data Acquisition

## To get data from hammer board
Edit get_ethernet_data_hammer.py to add IP address and port number.

These must match the values in the board's settings .cpp file.

From a command line type: python get_ethernet_data_hammer.py.

## To get data from damper board
Edit get_ethernet_data_damper.py to add IP address and port number.

These must match the values in the board's settings .cpp file.

From a command line type: python get_ethernet_data_damper.py.

## To plot the data

After the data is acquired, from Octave or Matlab run: plot_ethernet_data.m.

If only one board is used, edit plot_ethernet_data.m to comment out the unused data file.