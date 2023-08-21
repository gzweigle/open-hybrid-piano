// gcz 2023
//
// This class is not hardware dependent.
//
// Output data over MIDI
//
// TODO - Easily support other pedal functions.

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
    #if DEBUG_LEVEL >= 1
      Serial.println("MIDI sustain is now ON.");
    #endif
  }
  else if (DspP->GetSustainCrossedUpThreshold() == true) {
    mi_->sendControlChange(64, 0, midi_channel_);
    #if DEBUG_LEVEL >= 1
      Serial.println("MIDI sustain is now OFF.");
    #endif
  }
  if (DspP->GetSostenutoCrossedDownThreshold() == true) {
    #if DEBUG_LEVEL >= 1
    mi_->sendControlChange(66, 127, midi_channel_);
      Serial.println("MIDI sostenuto is now ON.");
    #endif
  }
  else if (DspP->GetSostenutoCrossedUpThreshold() == true) {
    #if DEBUG_LEVEL >= 1
    mi_->sendControlChange(66, 0, midi_channel_);
      Serial.println("MIDI sostenuto is now OFF.");
    #endif
  }
  if (DspP->GetUnaCordaCrossedDownThreshold() == true) {
    #if DEBUG_LEVEL >= 1
    mi_->sendControlChange(67, 127, midi_channel_);
      Serial.println("MIDI una corda is now ON.");
    #endif
  }
  else if (DspP->GetUnaCordaCrossedUpThreshold() == true) {
    #if DEBUG_LEVEL >= 1
    mi_->sendControlChange(67, 0, midi_channel_);
      Serial.println("MIDI una corda is now OFF.");
    #endif
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
      #if DEBUG_LEVEL >= 1
        Serial.print("MIDI note (");
        Serial.print(midi_note);
        Serial.print(") velocity (");
        Serial.print(velocity_int);
        Serial.print(")");
        if (send_on == true) {
          Serial.println(" is now ON.");
        }
        else {
        Serial.println(" is now OFF.");
        }
      #endif
      if (send_on == true) {
        mi_->sendNoteOn(midi_note, velocity_int, midi_channel_);
      }
      else {
        mi_->sendNoteOff(midi_note, velocity_int, midi_channel_);
      }
    }
  }
}