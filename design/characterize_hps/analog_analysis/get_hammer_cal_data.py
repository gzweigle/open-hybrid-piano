# Copyright (C) 2024 Greg C. Zweigle
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
# get_hammer_cal_data.py
#
# To run this code, install Python.
# Open a command prompt or terminal (depending on OS).k
# Type: python get_hammer_cal_data.py

import socket
import time

# Make long enough that the linear actuator finishes
# stepping the hammer from rest position to the hammer
# stop position before this code exits.
file_length = 200000

# Number of bytes sent by Teensy 4.1.
max_packet_size = 12

# This computer's IP address. Must match value in Teensy.
UDP_IP = Insert computer IP address here.
# Port number must match value in Teensy.
UDP_PORT = Insert Teensy port number here.

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.bind((UDP_IP, UDP_PORT))
print("Client socket is setup to take measurements.")
file = open('../data/hammer_cal_data.txt', 'w')
last_time = time.perf_counter()

while file_length >= 0:

    try:
        data, addr = client.recvfrom(max_packet_size)
    except socket.timeout as e:
        data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    # The hammer position raw ADC count data.
    dat0  =  int(data[0])&255
    dat0 |= (int(data[1])&255)<<8
    dat0 |= (int(data[2])&255)<<16

    # None of these are used yet.
    dat1  =  int(data[3])&255
    dat1 |= (int(data[4])&255)<<8
    dat1 |= (int(data[5])&255)<<16
    dat2  =  int(data[6])&255
    dat2 |= (int(data[7])&255)<<8
    dat2 |= (int(data[8])&255)<<16
    dat3  =  int(data[9])&255
    dat3 |= (int(data[10])&255)<<8
    dat3 |= (int(data[11])&255)<<16
    dat2 /= 16777216
    dat3 /= 16777216

    file.write(str(dat0)+' '+str(dat1)+' '+str(dat2)+' '+str(dat3)+'\n')
    file_length = file_length - 1

    if time.perf_counter() - last_time > 0.5:
        last_time = time.perf_counter()
        print("num={0}  dat = {1} {2} {3} {4}".format(file_length, dat0, dat1, dat2, dat3))