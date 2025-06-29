// Copyright (C) 2025 Greg C. Zweigle
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
// https://github.com/gzweigle/open-hybrid-piano
// https://github.com/stem-piano
//
// midiout.cpp
//
// This class is not hardware dependent.
//
// Output data over MIDI
//
// TODO - Easily support other pedal functions.

#include "midiout.h"
#include "auto_mute.h"

MidiOut::MidiOut() {}

void MidiOut::Setup(int midi_channel, MY_MIDI_INTERFACE *MidiInstance,
int maximum_midi_value, int debug_level) {
  debug_level_ = debug_level;
  midi_channel_ = midi_channel;
  mi_ = MidiInstance;
  midi_value_for_A0_ = 21;  // MIDI standard.
  pedal_midi_value_ = 127;  // Send this when a threshold pedal is activated.
  maximum_midi_value_ = maximum_midi_value;
  mi_->begin();
}

void MidiOut::SendNoteOn(AutoMute *mute, const bool *event, const float *velocity) {
  SendNote(mute, event, velocity, true, false);
}

void MidiOut::SendNoteOff(AutoMute *mute, const bool *event, const float *velocity,
bool source) {
  SendNote(mute, event, velocity, false, source);
}

void MidiOut::SendPedal(DspPedal *DspP) {
  if (DspP->GetSustainCrossedDownThreshold() == true) {
    mi_->sendControlChange(64, pedal_midi_value_, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(64, pedal_midi_value_, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI sustain ON.");
    }
  }
  else if (DspP->GetSustainCrossedUpThreshold() == true) {
    mi_->sendControlChange(64, 0, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(64, 0, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI sustain OFF.");
    }
  }
  if (DspP->GetSostenutoCrossedDownThreshold() == true) {
    mi_->sendControlChange(66, pedal_midi_value_, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(66, pedal_midi_value_, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI sostenuto ON.");
    }
  }
  else if (DspP->GetSostenutoCrossedUpThreshold() == true) {
    mi_->sendControlChange(66, 0, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(66, 0, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI sostenuto OFF.");
    }
  }
  if (DspP->GetUnaCordaCrossedDownThreshold() == true) {
    mi_->sendControlChange(67, pedal_midi_value_, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(67, pedal_midi_value_, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI una corda ON.");
    }
  }
  else if (DspP->GetUnaCordaCrossedUpThreshold() == true) {
    mi_->sendControlChange(67, 0, midi_channel_);
    #ifdef ENABLE_USB_MIDI
    usbMIDI.sendControlChange(67, 0, midi_channel_);
    #endif
    if (debug_level_ >= DEBUG_NOTES) {
      Serial.println("MIDI una corda OFF.");
    }
  }
}

void MidiOut::SendNote(AutoMute *mute,
const bool *event, const float *velocity, bool send_on, bool source) {
  int velocity_int;
  int midi_note;
  int velocity_potentially_muted;
  for (int key = 0; key < NUM_NOTES; key++) {
    if (event[key] == true) {
      midi_note = key + midi_value_for_A0_;
      velocity_int = static_cast<int>(128.0 * velocity[key]);
      if (velocity_int < 0) {
        velocity_int = -velocity_int;
      }
      if (velocity_int > maximum_midi_value_) {
        velocity_int = maximum_midi_value_;
      }
      if (debug_level_ >= DEBUG_NOTES) {
        Serial.printf("MIDI note (%2d) index(%2d) velocity(%2d)",
        midi_note, key, velocity_int);
        if (send_on == true) {
          Serial.println(" ON.");
        }
        else {
          Serial.print(" OFF ");
          if (source == false)
            Serial.println("(int)");
          else
            Serial.println("(ext)");
        }
      }
      // Immediately before sending MIDI command, check for certain values and if
      // detected, automatically reduce the volume level.
      if (send_on == true) {
        velocity_potentially_muted = 
        mute->AutomaticallyDecreaseVolume(velocity_int, debug_level_);
      }
      else {
        // Do not mute for the note off (damper) velocity.
        velocity_potentially_muted = velocity_int;
      }
      if (send_on == true) {
        mi_->sendNoteOn(midi_note, velocity_potentially_muted, midi_channel_);
        #ifdef ENABLE_USB_MIDI
	      usbMIDI.sendNoteOn(midi_note, velocity_potentially_muted, midi_channel_);
        #endif
      }
      else {
        mi_->sendNoteOff(midi_note, velocity_potentially_muted, midi_channel_);
        #ifdef ENABLE_USB_MIDI
	      usbMIDI.sendNoteOff(midi_note, velocity_potentially_muted, midi_channel_);
        #endif
      }
    }
  }
}
