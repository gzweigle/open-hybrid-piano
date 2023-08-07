# Gregary C. Zweigle, 2022
#
# Manage the UDP network interface.

import socket
import struct
import time

class PianoNetwork:

    def __init__(self, timeout_seconds, max_packet_size):

        self.timeout_seconds = timeout_seconds
        self.max_packet_size = max_packet_size

        self.MCAST_GRP = 
        self.MCAST_PORT =

    def setup_network(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.client.bind(('', self.MCAST_PORT))
        mreq = struct.pack("4sl", socket.inet_aton(self.MCAST_GRP), socket.INADDR_ANY)
        self.client.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
        self.client.settimeout(self.timeout_seconds)
        print("Client socket is setup.")

    # Receiving packets at different rates from various xPS boards.
    # If do not get any packets after a certain time, return that no packets
    # were received. This is a temporary approach that allows the received
    # hammer samples to have a streaming effect via the plotting code.
    # Eventually this is going to be rewritten when I build the web app.
    def blocking_get_packet_of_data(self):

        start_time = time.perf_counter_ns()
        try:
            data = self.client.recv(self.max_packet_size)
        except socket.timeout as e:
            packet_received = False
            data = []
        else:
            packet_received = True
        finally:
            diff_time = time.perf_counter_ns() - start_time

        return packet_received, diff_time, data