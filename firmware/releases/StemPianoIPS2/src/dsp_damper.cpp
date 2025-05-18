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
// dsp_damper.cpp
//
// This class is not hardware dependent.
//
// Damper digital signal processing

#include "dsp_damper.h"

DspDamper::DspDamper() {}

void DspDamper::Setup(float damper_threshold, float velocity_scaling,
int adc_sample_period_microseconds, int debug_level) {

  debug_level_ = debug_level;

  // Hysteresis to force one event around a threshold crossing.
  // Initialize to a large value to avoid startup transients.
  for (int key = 0; key < NUM_CHANNELS; key++) {
    event_block_counter_up_[key] = 2*(NUM_DELAY_ELEMENTS);
    event_block_counter_down_[key] = 2*(NUM_DELAY_ELEMENTS);
  }

  // Delay buffer, for lowpass filtering the velocity.
  buffer_index_ = 0;
  for (int key = 0; key < NUM_CHANNELS; key++) {
    for (int ind = 0; ind < NUM_DELAY_ELEMENTS; ind++) {
      damper_buffer_[key][ind] = 0.0;
    }
  }

  damper_threshold_ = damper_threshold;
  velocity_scaling_ = velocity_scaling;
  samples_per_second_ = 1000000.0 /static_cast<float>(adc_sample_period_microseconds);
  enable_ = true;

  // For CheckHammerDamperSync() need a threshold for when to
  // issue a damper event. Since this won't be due to a damper
  // threshold crossing, unable to compute a velocity.
  // In this case it will be small so select a default small value.
  velocity_if_force_event_ = 0.2;
  damper_low_threshold_ = 0.8 * damper_threshold_;

}

// When position[key] crosses a threshold, set event[key] true and store the 
// associated damper velocity in velocity[key].
// The velocity[key] value will be > 0 on damper rising and < 0 on damper falling.
// Using threshold crossings vs thresholds levels because need to generate
// a single damper event and the velocity of that event.
void DspDamper::GetDamperEventData(bool *event, float *velocity, const float *position) {

  if (enable_ == true) {

    float position_now, position_then;  // Use to keep code easier to read.

    for (int key = 0; key < NUM_CHANNELS; key++) {

      // Present and oldest position values for each key.
      position_now = position[key];
      position_then = damper_buffer_[key][(buffer_index_+1)%(NUM_DELAY_ELEMENTS)];
      damper_buffer_[key][buffer_index_] = position[key];

      if (event_block_counter_up_[key] == 0) {
        // If crossed a threshold going up, velocity will be positive.
        // If crossed a threshold going down, velocity will be negative.
        // In all cases, velocity is in range [-1, ..., 1].
        if (position_now >= damper_threshold_ && position_then < damper_threshold_) {
          event[key] = false;
          velocity[key] = (position_now - position_then) *
          samples_per_second_ / static_cast<float>(NUM_DELAY_ELEMENTS);
          velocity[key] *= velocity_scaling_;
          event_block_counter_up_[key] = 2*(NUM_DELAY_ELEMENTS);
          if (debug_level_ >= DEBUG_ALG) {
            Serial.println("GetDamperEventData() - damper up");
            Serial.print("  key=");
            Serial.print(key);
            Serial.print(" pos_now=");
            Serial.print(position_now);
            Serial.print(" pos_then=");
            Serial.print(position_then);
            Serial.print(" velocity=");
            Serial.print(velocity[key]);
            Serial.print(" threshold=");
            Serial.print(damper_threshold_);
            Serial.println("");
          }
        }
      }
      else if (event_block_counter_up_[key] > 0) {
        // Using this counter to block back-to-back events,
        // in case there is any jitter in signal around
        // a threshold crossing.
        event_block_counter_up_[key]--;
        event[key] = false;
      }

      if (event_block_counter_down_[key] == 0) {
        if (position_now <= damper_threshold_ && position_then > damper_threshold_) {
          event[key] = true;  // Damp the sound.
          velocity[key] = (position_now - position_then) *
          samples_per_second_ / static_cast<float>(NUM_DELAY_ELEMENTS);
          velocity[key] *= velocity_scaling_;
          event_block_counter_down_[key] = 2*(NUM_DELAY_ELEMENTS);
          if (debug_level_ >= DEBUG_ALG) {
            Serial.println("GetDamperEventData() - damper down");
            Serial.print("  key=");
            Serial.print(key);
            Serial.print(" pos_now=");
            Serial.print(position_now);
            Serial.print(" pos_then=");
            Serial.print(position_then);
            Serial.print(" velocity=");
            Serial.print(velocity[key]);
            Serial.print(" threshold=");
            Serial.print(damper_threshold_);
            Serial.println("");
          }
        }
        else {
          event[key] = false;
          velocity[key] = 0.0;
        }
      }
      else if (event_block_counter_down_[key] > 0) {
        // Using this counter to block back-to-back events,
        // in case there is any jitter in signal around
        // a threshold crossing.
        event_block_counter_down_[key]--;
        event[key] = false;
      }
    }
    buffer_index_ = (buffer_index_+1)%(NUM_DELAY_ELEMENTS);

  }
  else {
    for (int key = 0; key < NUM_CHANNELS; key++) {
      event[key] = false;
    }
  }
}

// It is possible to slightly impulse a key, give the hammer enough
// momentum to cross the strike threshold, but the key not pressed
// deep enough to cross the damper threshold. This causes a MIDI ON
// event but not a MIDI OFF event and so the note sustains incorrectly.
// This function covers that corner case of a shallow high velocity
// key press that accelerates the hammer enough to hit a virtual string
// but the key does not depress enough to generate a damper release event.
void DspDamper::CheckHammerDamperSync(bool *damper_event, float *velocity,
  const float *position, const bool *hammer_event) {

  for (int key = 0; key < NUM_CHANNELS; key++) {
    switch (hammer_previous_event_[key]) {
      case 0: // Got a note sound!
        if (hammer_event[key] == true)
          hammer_previous_event_[key] = true;
        break;
      case 1:
        // Damper released in the normal case.
        if (damper_event[key] == true)
          hammer_previous_event_[key] = false;
        // Cover the corner case of hammer event but without
        // enough key movement to generate a damper event.
        else if (position[key] < damper_low_threshold_) {
          hammer_previous_event_[key] = false;
          damper_event[key] = true;
          velocity[key] = velocity_if_force_event_;
          if (debug_level_ >= DEBUG_ALG) {
            Serial.printf("Forced damper release for note %d\n", key);
          }
        }
        break;
    }
  }
}

void DspDamper::Enable(bool enable) {
  enable_ = enable;
}