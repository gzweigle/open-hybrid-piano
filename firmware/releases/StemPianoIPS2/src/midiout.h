// Copyright (C) 2023 Greg C. Zweigle
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//
// Location of documentation, code, and design:
// https://github.com/gzweigle/DIY-Grand-Digital-Piano
//
// midiout.h
//
// This class is not hardware dependent.
//
// Output data over MIDI

#ifndef MIDIOUT_H_
#define MIDIOUT_H_

#include "stem_piano_ips2.h"

#include <MIDI.h>
#include "dsp_pedal.h"

#define MY_SERIAL_MIDI MIDI_NAMESPACE::SerialMIDI<HardwareSerial>
#define MY_MIDI_INTERFACE MIDI_NAMESPACE::MidiInterface<MY_SERIAL_MIDI>

class MidiOut
{
  public:
    MidiOut();
    void Setup(int, MY_MIDI_INTERFACE *, int);
    void SendNoteOn(const bool *, const float *);
    void SendNoteOff(const bool *, const float *);
    void SendPedal(DspPedal *);

  private:
    int debug_level_;
    int midi_channel_;
    int midi_value_for_A0_;
    MY_MIDI_INTERFACE *mi_;
    void SendNote(const bool *, const float *, bool);

};

#endif