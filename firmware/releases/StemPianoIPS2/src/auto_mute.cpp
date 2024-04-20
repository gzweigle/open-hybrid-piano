// Copyright (C) 2024 Greg C. Zweigle
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
// auto_mute.cpp
//
// Check for loud notes and automatically mute them.
//
// Why?
//
// Notes should only play when a piano key is pressed.
// Unwanted loud notes could possibly happen under these conditions:
//   External light sources on the sensors.
//   Bugs in code.
//   Problems with hardware or mechanical.
//   Something else not expected.
//
// Warning - This code is not perfect. Do not rely on it to catch
// and mitigate any of the problems listed above. Before using piano
// please ensure that volume level is acceptable under all conditions,
// including problems listed above.
//
// Algorithm:
//   (1) Any note at MIDI volume 127 is muted.
//   (2) More than max_simultaneous_notes_ notes above MIDI volume
//       max_simultaneous_volume_ are muted.
//
#include "auto_mute.h"

AutoMute::AutoMute() {}

void AutoMute::Setup() {
  // Set mute velocity to a non-zero value so that the condition is audible
  // which alerts to user that something did try to play.
  mute_velocity_ = 16;

  // Increase this value if more than one person is playing the piano.
  // For example, a duet performance.
  max_simultaneous_notes_ = 12;
  simultaneous_note_count_ = 0;

  // Any MIDI volume above this is muted when more than maximum
  // number of notes are played simultaneously.
  max_simultaneous_volume_ = 64;

  // All notes played within this number of milliseconds are
  // considered "simultaneous".
  loud_note_interval_ = 5;
  last_loud_note_time_ = millis();

}

int AutoMute::AutomaticallyDecreaseVolume(int velocity, int debug_level) {
  int return_velocity;

  // Count number of simultaneous loud notes.
  if (velocity > max_simultaneous_volume_) {
    simultaneous_note_count_++;
  }
  if (millis() - last_loud_note_time_ > loud_note_interval_) {
    simultaneous_note_count_ = 0;
  }

  // All max MIDI volume are muted.
  // Assumption: piano playing won't use volume of 127 but a
  // problem could. For example, a bug causing numerical max value.
  if (velocity >= 127) {
    return_velocity = mute_velocity_;
    if (debug_level > 0) {
      Serial.println("AutoMute Activated - piano volume reduced for this note.");
    }
  }

  // Assumption: piano playing won't strike more than a certain number
  // of notes "simultaneously". If many notes are played simultaneously,
  // it could be due to a problem. So, mute the volume.
  if (velocity > max_simultaneous_volume_) {

    if (debug_level > 1) {
      Serial.print("AutoMute: simultaneous = ");
      Serial.print(simultaneous_note_count_);
      Serial.print(" interval = ");
      Serial.println(millis() - last_loud_note_time_);
    }

    if (millis() - last_loud_note_time_ < loud_note_interval_) {
      if (simultaneous_note_count_ >= max_simultaneous_notes_) {
        return_velocity = mute_velocity_;
        if (debug_level > 0) {
          Serial.println("AutoMute Activated - piano volume reduced for this note.");
        }
      }
      else {
        return_velocity = velocity;
      }
    }
    else {
      return_velocity = velocity;
    }

    last_loud_note_time_ = millis();
  }

  // Nothing happened, so return the original velocity.
  // This is the normal case.
  else {
    return_velocity = velocity;
  }

  return return_velocity;
}