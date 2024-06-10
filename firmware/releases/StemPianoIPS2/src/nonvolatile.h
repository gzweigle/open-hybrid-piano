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
// nonvolatile.h
//
// Manage access to the EEPROM storage.

#ifndef NONVOLATILE_H_
#define NONVOLATILE_H_

#include "stem_piano_ips2.h"
#include <EEPROM.h>

// Per the Teensy 4.1 documentation.
#define MAX_EEPROM_ADDRESS 4283

class Nonvolatile
{
  public:
    Nonvolatile();
    void Setup(int);
 
    // Index is from 0 to NUM_NOTES - 1
    float ReadCalibrationSensorMin(int);
    void WriteCalibrationSensorMin(int, float);
    float ReadCalibrationSensorMax(int);
    void WriteCalibrationSensorMax(int, float);
    bool ReadCalibrationDoneFlag();
    void WriteCalibrationDoneFlag(bool);

    void UpdateAndWriteTotalWrites();
    int ReadTotalWrites();

    bool NonvolatileWasWritten();
    
  private:

    // For values in range [-2.0, ..., 2.0].
    // Store as 24-bit integers.
    void WriteNormalizedFloat(int, float);
    float ReadNormalizedFloat(int);
    
    int calibration_min_start_address_;
    int calibration_max_start_address_;
    int calibration_flag_address_;
    float calibration_scale_value_;

    int total_writes_address_;

    bool nonvolatile_was_written_;

};

#endif