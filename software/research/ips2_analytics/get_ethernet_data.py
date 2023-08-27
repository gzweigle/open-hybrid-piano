# Copyright (C) 2023 Greg C. Zweigle
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
# https://github.com/gzweigle/DIY-Grand-Digital-Piano
#
# get_ethernet_data.py
#
# This file is super simple example of reading from Ethernet 
# data sent by IPS 2.X board.
#
# To run this code, install Python.
# Open a command window.
# Type: python get_ethernet_data.py

import socket
import time

sample_rate = 2000
file_length = 10*sample_rate
max_packet_size = 8

#####################

# THE FOLLOWING MUST BE FILLED IN BEFORE USE

# This computer's IP address. Must match value in Teensy.
UDP_IP =
# Port number must match value in Teensy.
UDP_PORT =

#####################

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.bind((UDP_IP, UDP_PORT))
print("Client socket is setup")
file = open('ethernet_data.txt', 'w')
last_time = time.perf_counter()

while file_length >= 0:

    try:
        data, addr = client.recvfrom(max_packet_size)
    except socket.timeout as e:
        data = [0, 0, 0, 0, 0, 0, 0, 0]

    dat0  =  int(data[0])&255
    dat0 |= (int(data[1])&255)<<8
    dat1  =  int(data[2])&255
    dat1 |= (int(data[3])&255)<<8
    dat2  =  int(data[4])&255
    dat2 |= (int(data[5])&255)<<8
    dat3  =  int(data[6])&255
    dat3 |= (int(data[7])&255)<<8

    dat0 /= 65536
    dat1 /= 65536
    dat2 /= 65536
    dat3 /= 65536

    file.write(str(dat0)+' '+str(dat1)+' '+str(dat2)+' '+str(dat3)+'\n')
    file_length = file_length - 1

    if time.perf_counter() - last_time > 0.5:
        last_time = time.perf_counter()
        print("num={0}  dat = {1} {2} {3} {4}".format(file_length, dat0, dat1, dat2, dat3))