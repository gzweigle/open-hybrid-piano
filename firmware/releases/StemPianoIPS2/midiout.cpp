// gcz 2023
//
// Output data over MIDI

#include "midiout.h"

MidiOut::MidiOut() {}

void MidiOut::Setup(int midi_channel, MY_MIDI_INTERFACE *MidiInstance) {
  midi_channel_ = midi_channel;
  mi_ = MidiInstance;
  midi_value_for_A0_ = 21;  // MIDI standard.
  mi_->begin();
}

void MidiOut::SendNoteOn(const bool *event, const float *velocity) {
  SendNote(event, velocity, true);
}

void MidiOut::SendNoteOff(const bool *event, const float *velocity) {
  SendNote(event, velocity, false);
}

void MidiOut::SendPedal(DspPedal *DspP) {
  if (DspP->GetSustainCrossedDownThreshold() == true) {
    mi_->sendControlChange(64, 127, midi_channel_);
    Serial.println("MIDI sustain is now ON.");
  }
  else if (DspP->GetSustainCrossedUpThreshold() == true) {
    mi_->sendControlChange(64, 0, midi_channel_);
    Serial.println("MIDI sustain is now OFF.");
  }
  if (DspP->GetSostenutoCrossedDownThreshold() == true) {
    mi_->sendControlChange(66, 127, midi_channel_);
    Serial.println("MIDI sostenuto is now ON.");
  }
  else if (DspP->GetSostenutoCrossedUpThreshold() == true) {
    mi_->sendControlChange(66, 0, midi_channel_);
    Serial.println("MIDI sostenuto is now OFF.");
  }
  if (DspP->GetUnaCordaCrossedDownThreshold() == true) {
    mi_->sendControlChange(67, 127, midi_channel_);
    Serial.println("MIDI una corda is now ON.");
  }
  else if (DspP->GetUnaCordaCrossedUpThreshold() == true) {
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