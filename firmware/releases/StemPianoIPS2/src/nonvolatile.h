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

// All memory from 0 to 127 is reserved.
// If change then may need to write a utility
// program that clears state from EEPROM.
#define NONVOLATILE_USER_START_ADDRESS 128

// Use a fixed value instead of sizeof(double) because need to
// enfore EEPROM size limits. A check is run at compile time.
#define SIZE_DOUBLE 8

class Nonvolatile
{
  public:
    Nonvolatile();
    void Setup(int);
 
    // Index is from 0 to NUM_NOTES - 1
    double ReadCalibrationPositionMin(int);
    void WriteCalibrationPositionMin(int, double);
    double ReadCalibrationPositionMax(int);
    void WriteCalibrationPositionMax(int, double);
    bool ReadCalibrationDoneFlag();
    void WriteCalibrationDoneFlag(bool);

    bool ReadMaxVelocityStoredFlag();
    void WriteMaxVelocityStoredFlag(bool);
    double ReadMaxVelocity();
    void WriteMaxVelocity(double);

    void UpdateAndWriteTotalWrites();
    int ReadTotalWrites();

    bool NonvolatileWasWritten();
    
  private:

    int debug_level_;

    double ReadDouble(int);
    void WriteDouble(int, double);

    bool enable_memory_;
    
    // Each is based on the previous.
    // Min and max mean the min and max hammer position
    // values, not the min and max memory addresses.
    int nonvolatile_user_start_address_;
    int calibration_min_start_address_;
    int calibration_max_start_address_;
    int calibration_flag_address_;
    int velocity_scale_flag_address_;
    int velocity_scale_address_;
    int total_writes_address_;

    bool nonvolatile_was_written_;

};

#endif