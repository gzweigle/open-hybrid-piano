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
// calibration_velocity.h
//
// For ips pcb version 2.X
// For sca pcb version 0.0

#ifndef CALIBRATION_VELOCITY_H_
#define CALIBRATION_VELOCITY_H_

#include "stem_piano_ips2.h"
#include "nonvolatile.h"

class CalibrationVelocity
{
  public:
    CalibrationVelocity();
    void Setup(float, int, Nonvolatile *);
    void HammerVelocityScale(float *, const bool *, bool, bool, bool, bool);
    void DamperVelocityScale(float *, const bool *);
 
  private:
    float velocity_scale_;
    float fixed_velocity_scale_;
    float reciprocal_max_hammer_velocity_;
    float max_hammer_velocity_;
    float min_max_hammer_velocity_;
    float max_max_hammer_velocity_;
    bool new_max_velocity_;
    int debug_level_;

    Nonvolatile *Nv_;
    unsigned long last_write_time_;
    unsigned long min_write_interval_millis_;

    bool switch_freeze_cal_values_last_;
    bool switch_disable_and_reset_calibration_last_;

    void FixedScale(float *, const bool *);
    void BuildVelocityScale(const float *, const bool *);
    void ApplyVelocityScale(float *, const bool *);
    void InitializeState(Nonvolatile *);
    void WriteEeprom(bool, bool);

};

#endif
