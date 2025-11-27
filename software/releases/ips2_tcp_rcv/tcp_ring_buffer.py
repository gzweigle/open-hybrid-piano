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
# tcp_ring_buffer.py
#
# This file is super simple example of reading Ethernet 
# data sent by IPS 2.X board via TCP.
# Data is stored in rotating directories.
#
# To run this code:
# Install Python.
# Set stem piano (IPS 2.X) to initiate a client connection
# to the computer running tcp_ring_buffer.py.
# Open a command window.
# Type: python tcp_ring_buffer.py
# Data for all 96 sensors is saved continuously into rotating directories.
# Hit enter to end the program.

import socket
import os
import threading

# Longer files are easier for analyzing data.
# Shorter files reduce the file load time when analyzing data.
sample_rate = 3333
seconds_of_data_per_file = 5
file_length = seconds_of_data_per_file*sample_rate

num_notes = 96
max_packet_size = 2*num_notes

local_ip = socket.gethostbyname(socket.gethostname())
port = FILL IN BEFORE RUNNING # Match this to value on stem piano.

finish_processing = False
def listen_for_stop():
    global finish_processing
    input("Press Enter to finish writing end program.\n")
    finish_processing = True

try:

    # Build directories for data storage.
    directories = ["data0", "data1", "data2", "data3"]
    for directory in directories:
        try:
            os.makedirs(directory, exist_ok=True)
        except OSError as e:
            print(f"Error creating directory {directory}: {e}.")
            exit(1)

    # Establish connection.
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.settimeout(5)
    server.bind((local_ip, port))
    server.listen(1)
    print(f"Server is listening on {local_ip}:{port}.")
    try:
        client_socket, addr = server.accept()
        print(f"Connection is established with {addr}.")
        client_socket.settimeout(5)
    except socket.timeout:
        print("No client connected within the timeout period. Exiting.")
        server.close()
        exit(1)

    # Listen for signal to end the program.
    stop_thread = threading.Thread(target=listen_for_stop)
    stop_thread.start()

    # Get data from stem piano over TCP Ethernet and write to files continuously.
    buffer = b""
    current_directory_index = 0
    while not finish_processing:

        # Move to next directory.
        current_directory = directories[current_directory_index]
        print(f"Writing to directory: {current_directory}")
        current_directory_index = (current_directory_index + 1) % len(directories)

        file_pointer_list = []
        try:
            
            # Open all num_notes files in the active storage directory.
            for i in range(num_notes):
                file_name = f"{current_directory}/data_{i}.txt"
                try:
                    file_pointer = open(file_name, "w")
                    file_pointer_list.append(file_pointer)
                except OSError as e:
                    print(f"Error opening file {file_name}: {e}.")
                    for fp in file_pointer_list:
                        fp.close()
                    exit(1)

            # Receive a set of num_notes samples and store as an entry
            # in each file. End when the number if samples hits the
            # target number for each file (set by file_length).
            sample_count = 0
            while sample_count < file_length:
                try:
                    chunk = client_socket.recv(2*num_notes)
                    if not chunk:
                        print("TCP connection closed by stem piano?")
                        break
        
                    # Use chunks to handle receiving partial packets.
                    buffer += chunk
                    while len(buffer) >= 2*num_notes:
                        datal = buffer[:2*num_notes]
                        buffer = buffer[2*num_notes:]
        
                        for i, fp in enumerate(file_pointer_list):
                            try:
                                # Each sample is two bytes, signed.
                                data = (int(datal[2*i])&255) | (int(datal[2*i+1]&255)<<8)
                                if data & (1 << 15):
                                    data -= (1 << 16)
                                fp.write(str(data) + '\n')
                            except:
                                print(f"Error writing to file {fp.name}: {e}.")
                                for fp in file_pointer_list:
                                    fp.close()
                                exit(1)
                            
                        # Handle corner case of hitting file length due to chunk.
                        sample_count += 1
                        if sample_count >= file_length:
                            break
    
                except socket.timeout:
                    print("Socket timeout.")
                    print("Is stem piano setup to initiate a client connection?")
                    break
    
                except Exception as e:
                    print(f"Error receiving data: {e}.")
                    break
        
        finally:
            for fp in file_pointer_list:
                try:
                    fp.close()
                except OSError as e:
                    print("Error closing file {fp.name}: {e}.")

    print("Stop signal received. Finishing current directory and exiting.")

finally:
    try:
        server.close()
        print("Server socket closed.")
    except Exception as e:
        print(f"Error closing server socket: {e}.")