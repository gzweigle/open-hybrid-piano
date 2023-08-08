// gcz 2023
//
// Output data over MIDI

#ifndef MIDIOUT_H_
#define MIDIOUT_H_

#include <MIDI.h>
#include "stem_piano_ips2.h"

#define MY_SERIAL_MIDI MIDI_NAMESPACE::SerialMIDI<HardwareSerial>
#define MY_MIDI_INTERFACE MIDI_NAMESPACE::MidiInterface<MY_SERIAL_MIDI>

class MidiOut
{
    public:
        MidiOut();
        void Setup(int, MY_MIDI_INTERFACE *);
        void SendNoteOn(const bool *, const float *);
        void SendNoteOff(const bool *, const float *);
        void SendPedal(const int *);

    private:
        int midi_channel_;
        int midi_value_for_A0_;
        MY_MIDI_INTERFACE *mi_;
        void SendNote(const bool *, const float *, bool);

};

#endif