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
// TODO - Is this needed?

#include "gain_control.h"

GainControl::GainControl() {}

void GainControl::Setup(float velocity_scale, int debug_level) {
  velocity_scale_ = velocity_scale;
  debug_level_ = debug_level;
}

// Automatic gain control.
void GainControl::AutomaticGainControl(float *velocity_data,
const bool *velocity_event) {

  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= velocity_scale_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= 2) {
          Serial.print("velocity of ");
          Serial.print(key);
          Serial.print(" hit limit, orig velocity was ");
          Serial.println(velocity_data[key] / velocity_scale_);
        }
        velocity_data[key] = 1.0;
      }
    }
  }
}