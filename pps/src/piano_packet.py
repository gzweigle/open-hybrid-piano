# Gregary C. Zweigle, 2022
#
# Decode the packet according to the PNP protocol.
# PNP = Piano Network Protocol.
# See the PNP specification for protocol details.

class PianoPacket:

    def __init__(self):

        self.valid_packet = False
        self.board_number = 0
        self.packet_type = 0

        self.sample = [0] * 8
        self.state = [0] * 8
        self.velocity = [0] * 8

        # to do - Must match the xPS code.
        # Need to pull from one location instead of copy/paste.
        self.NETWORK_NO_EVENT = 0
        self.NETWORK_KEY_RELEASE = 1
        self.NETWORK_KEY_STRIKE = 2
        self.EVENT_PACKET = 2
        self.DATA_PACKET = 3

    def decode_packet(self, packet_received, data):

        if packet_received == False:
            self.valid_packet = False
        else:
            self.board_number = (data[1]>>2)&15
            self.packet_type = data[1]&3
            self.valid_packet = True
            if self.is_data_packet() == True:
                self.decode_data_packet(data)
            elif self.is_event_packet() == True:
                self.decode_event_packet(data)
        
    def get_packet_samples(self):
        if self.is_data_packet() == True:
            return self.sample
        else:
            return 0

    def get_sample(self, index):
        if self.is_data_packet() == True:
            return self.sample[index]
        else:
            return 0

    def get_velocity(self, index):
        if self.is_event_packet() == True:
            return self.velocity[index]
        else:
            return 0

    def is_note_off(self, index):
        if self.is_event_packet() == True:
            if self.state[index] == self.NETWORK_KEY_RELEASE:
                return True
            else:
                return False
        else:
            return False

    def is_note_on(self, index):
        if self.is_event_packet() == True:
            if self.state[index] == self.NETWORK_KEY_STRIKE:
                return True
            else:
                return False
        else:
            return False

    def is_data_packet(self):
        if self.valid_packet == True and self.packet_type == self.DATA_PACKET:
            return True
        else:
            return False

    def is_event_packet(self):
        if self.valid_packet == True and self.packet_type == self.EVENT_PACKET:
            return True
        else:
            return False

    def get_board_number(self):
        return self.board_number

    def decode_data_packet(self, data):
        self.sample = []
        # Build a 24-bit value out of the thee 8-bit values received.
        for k in range(8):
            sample_value  = (data[2 + 3*k] << 16)
            sample_value |= (data[2 + 3*k+1] << 8)
            sample_value |= (data[2 + 3*k+2])
            self.sample.append(sample_value)

    def decode_event_packet(self, data):
        self.state = []
        self.velocity = []
        for k in range(8):
            # Format of received data if according to PNP (protocol).
            self.state.append(data[2+2*k]&3)
            self.velocity.append(data[2+2*k+1])