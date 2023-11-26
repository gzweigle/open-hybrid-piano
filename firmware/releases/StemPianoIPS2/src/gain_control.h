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
// six_channel_analog_00.h
//
// For ips pcb version 2.X
// For sca pcb version 0.0

#ifndef GAIN_CONTROL_H_
#define GAIN_CONTROL_H_

#include "stem_piano_ips2.h"

class GainControl
{
  public:
    GainControl();
    void Setup(bool, float);
    void AutomaticGainControl(float *, const float *);
 
  private:
    bool adjust_gain_;
    float adc_scale_threshold_;
    float max_value_[NUM_CHANNELS];
    bool latched_max_value_[NUM_CHANNELS];

};

#endif
