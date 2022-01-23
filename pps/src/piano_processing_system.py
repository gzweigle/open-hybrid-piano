# Gregary C. Zweigle, 2022
#
# Receive hammer strike and hammer sample packets from the xPS.
# x = T for Arduino Teensy Processing System
# x = R for Raspberry Pi Processing System
#
# Generate MIDI output from hammer strike packets.
# Store samples in rotating files from hammer sample packets.

import piano_database
import piano_midi
import piano_network
import piano_packet

# A desktop computer likely will be for database + analytics
# and a Raspberry Pi will likely be MIDI + sound generation.
# For now, use same code for both and these variables determine
# which computing system has which functionality. In the future these
# codebases will likely diverge too much to keep common.
do_midi = False
do_database = True

rows_per_file = [4000, 8000]  # The value in plot_as_acquire.m must match.
sample_rate = [500, 1000]
max_network_wait_time = 0.05  # Quick testing shows make > 20 milliseconds.
max_packet_length = 32  # Make larger than maximum packet sent from tps/bps

if do_database == True:
    packet_data = []
    for k in range(len(rows_per_file)):
        packet_data.append(
            piano_database.PianoDatabase(k, sample_rate[k], rows_per_file[k]))

if do_midi == True:
    mi = piano_midi.PianoMidi()

pn = piano_network.PianoNetwork(max_network_wait_time, max_packet_length)

pp = piano_packet.PianoPacket()

pn.setup_network()
while True:

    packet_received, diff_time, data = pn.blocking_get_packet_of_data()
    pp.decode_packet(packet_received, data)

    if do_midi == True:
        for note in range(8):
            # The value 60 puts lowest ADC input at middle C on keyboard.
            # to do - Make "60" a function of board number.
            if pp.is_note_off(note):
                mi.note_off(60 + note, pp.get_velocity(note))
            elif pp.is_note_on(note):
                mi.note_on(60 + note, pp.get_velocity(note))

    if do_database == True:
        for k in range(len(rows_per_file)):
            packet_data[k].write_samples(
                pp.is_data_packet(), pp.get_packet_samples(),
                pp.get_board_number(), diff_time)