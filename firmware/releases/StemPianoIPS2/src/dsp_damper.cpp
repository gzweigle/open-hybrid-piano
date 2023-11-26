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
// dsp_damper.cpp
//
// This class is not hardware dependent.
//
// Damper digital signal processing

#include "dsp_damper.h"

DspDamper::DspDamper() {}

void DspDamper::Setup(const bool *using_hammer_to_estimate_damper,
float threshold0, float threshold1, float velocity_scaling,
int adc_sample_period_microseconds, int debug_level) {
  debug_level_ = debug_level;
  for (int key = 0; key < NUM_CHANNELS; key++) {
    using_hammer_to_estimate_damper_[key] = using_hammer_to_estimate_damper[key];
    damper_threshold_[key] = 0.0;
    // Initialize out of range to ensure
    // gets set by UpdateDamperRange();
    min_damper_position_[key] = 1.0;
    max_damper_position_[key] = 0.0;
    // Until this is set, use estimtes of min/max damper positions.
    min_max_damper_position_valid_[key] = false;
    // Hysteresis to force one event around a threshold crossing.
    // Initialize to a large value to avoid startup transients.
    event_block_counter_[key] = 2*(NUM_DELAY_ELEMENTS);
  }

  // Delay buffer, for lowpass filtering the velocity.
  buffer_index_ = 0;
  for (int key = 0; key < NUM_CHANNELS; key++) {
    for (int ind = 0; ind < NUM_DELAY_ELEMENTS; ind++) {
      damper_buffer_[key][ind] = 0.0;
    }
  }

  threshold0_ = threshold0;
  threshold1_ = threshold1;
  velocity_scaling_ = velocity_scaling;
  samples_per_second_ = 1000000.0 /static_cast<float>(adc_sample_period_microseconds);
  enable_ = true;
}

// When position[key] crosses a threshold, set event[key] true and store the 
// associated damper velocity in velocity[key].
// The velocity[key] value will be > 0 on damper rising and < 0 on damper falling.
void DspDamper::GetDamperEventData(bool *event, float *velocity, const float *position) {

  if (enable_ == true) {

    float position_now, position_then;  // Use to keep code easier to read.
    UpdateDamperRange(position);
    UpdateDamperThresholds();
    for (int key = 0; key < NUM_CHANNELS; key++) {

      // Present and oldest position values for each key.
      position_now = position[key];
      position_then = damper_buffer_[key][(buffer_index_+1)%(NUM_DELAY_ELEMENTS)];
      damper_buffer_[key][buffer_index_] = position[key];

      if (event_block_counter_[key] == 0) {
        // If crossed a threshold going up, velocity will be positive.
        // If crossed a threshold going down, velocity will be negative.
        // In all cases, velocity is in range [-1, ..., 1].
        if (position_now >= damper_threshold_[key] &&
        position_then < damper_threshold_[key])
        {
          event[key] = false;
          velocity[key] = (position_now - position_then) *
          samples_per_second_ / static_cast<float>(NUM_DELAY_ELEMENTS);
          velocity[key] *= velocity_scaling_;
          event_block_counter_[key] = 2*(NUM_DELAY_ELEMENTS);
          if (debug_level_ >= 2) {
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
            Serial.print(damper_threshold_[key]);
            Serial.println("");
          }
        }
        else if (position_now <= damper_threshold_[key] &&
        position_then > damper_threshold_[key])
        {
          event[key] = true;  // Damp the sound.
          velocity[key] = (position_now - position_then) *
          samples_per_second_ / static_cast<float>(NUM_DELAY_ELEMENTS);
          velocity[key] *= velocity_scaling_;
          event_block_counter_[key] = 2*(NUM_DELAY_ELEMENTS);
          if (debug_level_ >= 2) {
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
            Serial.print(damper_threshold_[key]);
            Serial.println("");
          }
        }
        else {
          event[key] = false;
          velocity[key] = 0.0;
        }
      }
      else if (event_block_counter_[key] > 0) {
        // Using this counter to block back-to-back events,
        // in case there is any jitter in signal around
        // a threshold crossing.
        event_block_counter_[key]--;
        event[key] = false;
      }
    }
    buffer_index_ = (buffer_index_+1)%(NUM_DELAY_ELEMENTS);
    DetectFirstDamper(event);
  }
  else {
    for (int key = 0; key < NUM_CHANNELS; key++) {
      event[key] = false;
    }
  }
}

// Keep track of minimum and maximum damper position.
void DspDamper::UpdateDamperRange(const float *damper_position) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (damper_position[key] < min_damper_position_[key]) {
      min_damper_position_[key] = damper_position[key];
      if (debug_level_ >= 2) {
        Serial.print("UpdateDamperRange() ");
        Serial.print(" key=");
        Serial.print(key);
        Serial.print(" min_damper_position=");
        Serial.print(min_damper_position_[key]);
        Serial.println("");
      }
    }
    if (damper_position[key] > max_damper_position_[key]) {
      max_damper_position_[key] = damper_position[key];
      if (debug_level_ >= 2) {
        Serial.print("UpdateDamperRange() ");
        Serial.print(" key=");
        Serial.print(key);
        Serial.print(" max_damper_position=");
        Serial.print(max_damper_position_[key]);
        Serial.println("");
      }
    }
  }
}

// Until a key is pressed once, the min/max positions will be incorrect.
// Therefore, check and set a flag that the min/max positions are valid.
void DspDamper::DetectFirstDamper(const bool *event) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (event[key] == true) {
      if (debug_level_ >= 2) {
        if (min_max_damper_position_valid_[key] == false) {
          Serial.print("DetectFirstDamper() ");
          Serial.print(" key=");
          Serial.print(key);
          Serial.print(" min_max_position_valid is now true");
          Serial.println("");
        }
      }
      min_max_damper_position_valid_[key] = true;
    }
  }
}

// Set the damper up/down threshold based on the min/max
// position as measured by the sensor. If the min/max position
// has yet to be determined, then use the threshold directly.
void DspDamper::UpdateDamperThresholds() {
  float threshold;
  for (int key = 0; key < NUM_CHANNELS; key++) {

    // If hammer is used to estimate the damper position,
    // then use a different_threshold.
    if (using_hammer_to_estimate_damper_[key] == false)
      threshold = threshold0_;  // For using damper to measure damper.
    else
      threshold = threshold1_;  // For using hammer to estimate damper.

    if (min_max_damper_position_valid_[key] == true) {
      damper_threshold_[key] =
      (max_damper_position_[key] - min_damper_position_[key]) *
      threshold;
    }
    else {
      damper_threshold_[key] = threshold;
    }
  }
}

void DspDamper::Enable(bool enable) {
  enable_ = enable;
}