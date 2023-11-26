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
// gain_control.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//

#include "gain_control.h"

GainControl::GainControl() {}

void GainControl::Setup(bool adjust_gain, float adc_scale_threshold) {

  adjust_gain_ = adjust_gain;
  adc_scale_threshold_ = adc_scale_threshold;

  for (int ind = 0; ind < NUM_CHANNELS; ind++) {
    max_value_[ind] = 0.0;
    latched_max_value_[ind] = false;
  }

}

// Automatic gain control.
void GainControl::AutomaticGainControl(float *out_array, const float *in_array) {

  for (int ind = 0; ind < NUM_CHANNELS; ind++) {

    // Once input exceeds a threshold, assume a key is being pressed and the
    // hammer is traveling upwards. The max_value will continue to increase as
    // it tracks the upward motion of the hammer. Once the max value no longer
    // is increasing, assume we have the high point of the travel and that
    // becomes the maximum value. This prevents transient large scaling on
    // first note press. Also, prevents a divide-by-zero.
    if (in_array[ind] > adc_scale_threshold_) {
      if (in_array[ind] > max_value_[ind]) {
        max_value_[ind] = in_array[ind];
      }
      if (in_array[ind] < max_value_[ind]) {
        latched_max_value_[ind] = true;
      }
    }
    if (latched_max_value_[ind] == true && adjust_gain_ == true) {
      out_array[ind] = in_array[ind] / max_value_[ind];
      if (out_array[ind] > 1.0) {
        out_array[ind] = 1.0;
      }
    }
    else {
      out_array[ind] = in_array[ind];
    }

  }
}