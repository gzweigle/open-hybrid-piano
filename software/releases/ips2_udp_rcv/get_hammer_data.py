# Copyright (C) 2025 Greg C. Zweigle
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
#
# Location of documentation, code, and design:
# https://github.com/gzweigle/open-hybrid-piano
# https://github.com/stem-piano
#
# To run this code, install Python.
# Open a command prompt or terminal (depending on OS).
# Type: python get_hammer_data.py
#
# The data is stored in a text file, one column per key.
#
# To view data, install Octave.
# Type: x=load("hammer_position.txt");plot(x);

# Convert encoded integers into original value.
def convert_to_float(a, b):
    x  =  int(a)&255
    x |= (int(b)&255)<<8
    if x & (1 << 15):
        x -= (1 << 16)
    y = float(x) / 32768.0
    return y

import socket
import time

# Settings values.
file_length = 10000     # Total number of values to receive.
note_number_min = 2     # First note is B0 (index 2).
note_number_max = 3     # Last note is C1 (index 3).

# Fixed values.
num_received_values = note_number_max - note_number_min + 1
max_packet_size = 2*96

# This computer's IP address. Must match value in Teensy.
UDP_IP = FILL IN BEFORE RUNNING
# Port number must match value in Teensy.
UDP_PORT = FILL IN BEFORE RUNNING

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.bind((UDP_IP, UDP_PORT))
print("Client socket is setup to take measurements.")

file = open('hammer_position.txt', 'w')
last_time = time.perf_counter()

for ind in range(0,file_length):

    try:
        data, addr = client.recvfrom(max_packet_size)
    except socket.timeout as e:
        data = [0] * max_packet_size

    # Data is sent as two 8-bit integers. Convert to [-1, ..., 1].
    data_float = []
    for k in range(note_number_min,note_number_max+1):
      data_float.append(convert_to_float(data[2*k+0], data[2*k+1]))

    # Write to file as a row of floats.
    # Write only the single note tested.
    write_string = ''
    for k in range(0,num_received_values):
        write_string += str(data_float[k]) + ' '
    write_string += '\n'
    file.write(write_string)

    # Put something on screen to track progress.
    if time.perf_counter() - last_time > 0.5:
        last_time = time.perf_counter()
        print("num={0} dat = {1}".format(file_length, data_float[0]))