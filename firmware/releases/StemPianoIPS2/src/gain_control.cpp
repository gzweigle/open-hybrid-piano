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
// gain_control.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//

#include "gain_control.h"

GainControl::GainControl() {}

void GainControl::Setup(float velocity_scale, int debug_level) {
  velocity_scale_ = velocity_scale;
  last_switch_value_ = false;
  found_at_least_one_new_max_ = false;
  reciprocal_max_hammer_velocity_ = 1.0;
  debug_level_ = debug_level;
}

// Scale the hammer velocities.
// Velocity range = [0.0, 1.0].
void GainControl::HammerVelocityScale(float *velocity_data,
bool switch_set_peak_velocity, const bool *velocity_event) {

  // Multiply the velocity by a fixed value.
  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= velocity_scale_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= DEBUG_MINOR) {
          Serial.print("velocity_scale_: velocity of key ");
          Serial.print(key);
          Serial.print(" hit limit, orig velocity was ");
          Serial.print(velocity_data[key] / velocity_scale_);
          Serial.println(".");
        }
        velocity_data[key] = 1.0;
      }
    }
  }

  // Switch transitioned to on: load a working max value.
  if (switch_set_peak_velocity == true && last_switch_value_ == false) {
    found_at_least_one_new_max_ = false;
    max_hammer_velocity_working_ = 0.0;
  }
  // Switch transitioned to off: save the working max value.
  else if (switch_set_peak_velocity == false && last_switch_value_ == true) {
    if (found_at_least_one_new_max_ == true) {
      // Don't let it be zero. And, any small value is probably wrong.
      if (max_hammer_velocity_working_ < 0.25) {
        max_hammer_velocity_working_ = 0.25;
        if (debug_level_ > DEBUG_NONE) {
          Serial.println("Warning - max hammer velocity set to 0.25");
        }
      }
      if (debug_level_ > DEBUG_NONE) {
        Serial.print("Found a new max hammer velocity = ");
        Serial.print(max_hammer_velocity_working_);
        Serial.println(".");
      }
      reciprocal_max_hammer_velocity_ = 1.0 / max_hammer_velocity_working_;
    }
    else {
      if (debug_level_ > DEBUG_NONE) {
        Serial.println("Warning - did not find a new max hammer velocity");
        Serial.print("          keeping the previous max hammer velocity of ");
        Serial.print(1.0/reciprocal_max_hammer_velocity_);
        Serial.println(".");
      }
    }
  }
  // Switch is fully on: find a new max value for all keys played.
  else if (switch_set_peak_velocity == true && last_switch_value_ == true) {
    for (int key = 0; key < NUM_NOTES; key++) {
      if (velocity_event[key] == true) {
        if (velocity_data[key] > max_hammer_velocity_working_) {
          max_hammer_velocity_working_ = velocity_data[key];
          found_at_least_one_new_max_ = true;
        }
      }
    }
  }
  last_switch_value_ = switch_set_peak_velocity;

  // Multiply the velocity by the dynamically scaled value.
  // Optimization opportunity - combine with scaling above.
  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= reciprocal_max_hammer_velocity_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= DEBUG_MINOR) {
          Serial.print("max_hammer_velocity_: velocity of key ");
          Serial.print(key);
          Serial.print(" hit limit, orig velocity was ");
          Serial.print(velocity_data[key] / velocity_scale_);
          Serial.println(".");
        }
        velocity_data[key] = 1.0;
      }
    }
  }

}

// Scale the damper velocities.
void GainControl::DamperVelocityScale(float *velocity_data,
const bool *velocity_event) {

  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= velocity_scale_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= DEBUG_ALL) {
          Serial.print("velocity of key ");
          Serial.print(key);
          Serial.print(" hit limit, orig velocity was ");
          Serial.print(velocity_data[key] / velocity_scale_);
          Serial.println(".");
        }
        velocity_data[key] = 1.0;
      }
    }
  }
}