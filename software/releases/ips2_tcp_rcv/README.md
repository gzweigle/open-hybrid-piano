# TCP Ethernet Data Acquisition and Display

stem piano streams calibrated hammer position measurements over Ethernet for all 88 keys plus the positions of the extra 8 upper sensors. The extra 8 upper sensors can be pedal inputs, additional piano keys, or other inventive sources.

The streaming measurements are useful to study hammer motion as a function of time, to debug piano algorithms, for visualization, or for many other options.

## Setting the port

Edit *tcp_ring_buffer.py* and add a port number.

The port number match the values in the piano settings .cpp file.

## Running

Set the open hybrid piano to attempt establishing a TCP client connection to the local computer.

See documentation https://github.com/stem-piano/stem-piano-g-main.

Type *python tcp_ring_buffer.py* into a command window on the local computer.

Play piano.

Data for all 96 sensors are stored in four rotating directories.

When finished, hit enter in the command window to stop *tcp_ring_buffer.py*. The program will stop after it finishes writing to the present directory. This ensures no partial files.

## To plot the data

After the data is acquired.

Run Octave or Matlab.

Type in command line:

*plot_data(directory_number, scaling);*

Function arguments:

* directory_number = 0, 1, 2, or 3.
* scale = 1 to display raw ADC counts.
* scale = 32768 to display values normalized to +/- 1.