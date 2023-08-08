// gcz 2023
//
// Output data over MIDI
//
// TODO - Fix SendPedal() so do not need to know the index pedal meaning.

#include "midiout.h"

MidiOut::MidiOut() {}

void MidiOut::Setup(int midi_channel, MY_MIDI_INTERFACE *MidiInstance) {
  midi_channel_ = midi_channel;
  mi_ = MidiInstance;
  midi_value_for_A0_ = 21;  // Standardized by MIDI.
  mi_->begin();
}

void MidiOut::SendNoteOn(const bool *event, const float *velocity) {
  SendNote(event, velocity, true);
}

void MidiOut::SendNoteOff(const bool *event, const float *velocity) {
  SendNote(event, velocity, false);
}

// From pedals.cpp:
//  0 = on to off
//  1 = off to on
// -1 = no change
// Index 0 is sustain.
// Index 1 is sostenuto.
// Index 2 is una corda.
void MidiOut::SendPedal(const int *pedal_state) {
  if (pedal_state[0] == 1) {
    mi_->sendControlChange(64, 127, midi_channel_);
    Serial.println("MIDI sustain is now ON.");
  }
  else if (pedal_state[0] == 0) {
    mi_->sendControlChange(64, 0, midi_channel_);
    Serial.println("MIDI sustain is now OFF.");
  }
  if (pedal_state[1] == 1) {
    mi_->sendControlChange(66, 127, midi_channel_);
    Serial.println("MIDI sostenuto is now ON.");
  }
  else if (pedal_state[1] == 0) {
    mi_->sendControlChange(66, 0, midi_channel_);
    Serial.println("MIDI sostenuto is now OFF.");
  }
  if (pedal_state[2] == 1) {
    mi_->sendControlChange(67, 127, midi_channel_);
    Serial.println("MIDI una corda is now ON.");
  }
  else if (pedal_state[2] == 0) {
    mi_->sendControlChange(67, 0, midi_channel_);
    Serial.println("MIDI una corda is now OFF.");
  }
}

void MidiOut::SendNote(const bool *event, const float *velocity, bool send_on) {
  int velocity_int;
  int midi_note;
  for (int key = 0; key < NUM_NOTES; key++) {
    midi_note = key + midi_value_for_A0_;
    if (event[key] == true) {
      velocity_int = static_cast<int>(128.0 * velocity[key]);
      if (velocity_int < 0) {
        velocity_int = -velocity_int;
      }
      if (velocity_int > 127) {
        velocity_int = 127;
      }
      Serial.print("MIDI note (");
      Serial.print(midi_note);
      Serial.print(") velocity (");
      Serial.print(velocity_int);
      Serial.print(")");
      if (send_on == true) {
        Serial.println(" is now ON.");
        mi_->sendNoteOn(midi_note, velocity_int, midi_channel_);
      }
      else {
        Serial.println(" is now OFF.");
        mi_->sendNoteOff(midi_note, velocity_int, midi_channel_);
      }
    }
  }
}