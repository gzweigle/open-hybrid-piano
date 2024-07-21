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
// calibration_position.h
//

#ifndef CALIBRATION_POSITION_H_
#define CALIBRATION_POSITION_H_

#define CALIBRATION_FILTER_SAMPLES 4

#include "stem_piano_ips2.h"
#include "nonvolatile.h"

class CalibrationPosition
{
  public:
    CalibrationPosition();
    void Setup(float, int, Nonvolatile *);
    bool Calibration(bool, bool, float *, const float *);
 
  private:

    int debug_level_;

    double gain_staged_[NUM_NOTES], gain_[NUM_NOTES];
    double offset_staged_[NUM_NOTES], offset_[NUM_NOTES];
    double max_[NUM_NOTES];
    double min_[NUM_NOTES];

    double gain_default_;
    double gain_correction_;

    bool min_at_least_one_[NUM_NOTES], max_at_least_one_[NUM_NOTES];

    double threshold_;
    double staged_scaling_value_;

    bool switch_freeze_cal_values_last_;
    bool switch_disable_and_reset_calibration_last_;

    float filter_buffer_[NUM_NOTES][CALIBRATION_FILTER_SAMPLES];
    int buffer_index_;

    Nonvolatile *Nv_;
    unsigned long last_write_time_;
    unsigned long min_write_interval_millis_;

    double GetGain(double, double);
    double GetOffset(double);
    void InitializeState(Nonvolatile *, int);
    float ClipLimit(float);
    void ApplyCalibrationValues(bool, float *, const float *);
    bool BuildCalibrationValues(bool, bool, const float *);
    void WriteEeprom(bool, bool, bool);

};

#endif
