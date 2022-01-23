# Gregary C. Zweigle, 2022
#
# Send received velocity value to MIDI output.

import rtmidi

class PianoMidi:

    def __init__(self):
        self.MIDI_CHANNEL = 0
        self.MIDI_PORT = 1
        self.on_code = 0x90 + self.MIDI_CHANNEL
        self.off_code = 0x80 + self.MIDI_CHANNEL
        self.midi_driver = rtmidi.MidiOut()
        self.midi_driver.open_port(self.MIDI_PORT)
        print("MIDI is setup.")

    def note_off(self, note, velocity):
        print(f'Received a Note-Off packet. Pitch={note}, Velocity={velocity}')
        self.midi_driver.send_message([self.off_code, note, velocity])

    def note_on(self, note, velocity):
        print(f'Received a Note-On  packet. Pitch={note}, Velocity={velocity}')
        self.midi_driver.send_message([self.on_code, note, velocity])