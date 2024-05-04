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
// gain_control.h
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
    void Setup(float, int);
    void HammerVelocityScale(float *, bool, const bool *);
    void DamperVelocityScale(float *, const bool *);
 
  private:
    float velocity_scale_;
    bool last_switch_value_;
    float reciprocal_max_hammer_velocity_;
    float max_hammer_velocity_working_;
    bool found_at_least_one_new_max_;
    int debug_level_;

};

#endif
