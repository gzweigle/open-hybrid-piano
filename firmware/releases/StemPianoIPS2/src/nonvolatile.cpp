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
// Design approach is that Nonvolatile class handles addresses.
// Therefore, the function names are specific to each source.
// Also, the nonvolatile functions meet each source requirements.

#include "nonvolatile.h"

Nonvolatile::Nonvolatile() {}

void Nonvolatile::Setup(int debug_level) {

  calibration_scale_value_ = 4194304.0;  // 2^22.

  // If change this address, total EEPROM writes is lost.
  // Do not not changes this address.
  // Uses 3 addresses. Increment total_writes_address_ by 3 for next address.
  total_writes_address_ = 0;

  /////////////////////////////////////////////
  // Nonvolatile memory for calibration values.
  // Stores values in range [-2.0, ..., 2.0].
  calibration_min_start_address_ = total_writes_address_ + 3;
  calibration_max_start_address_ = calibration_min_start_address_ + 3*(NUM_NOTES);
  calibration_flag_address_ = calibration_max_start_address_ + 3*(NUM_NOTES);
  /////////////////////////////////////////////

  ////////////////////////////////////////////////
  // Nonvolatile memory for velocity scale values.
  velocity_scale_flag_address_ = calibration_flag_address_ + 1;
  velocity_scale_address_ = velocity_scale_flag_address_ + 1;
  ////////////////////////////////////////////////

  // Set to true in code below anytime nonvolatile memory was written.
  // Cleared by ReturnTrueIfNonvolatileWasWritten().
  nonvolatile_was_written_ = false;

  if (debug_level >= DEBUG_STATS) {
    Serial.print("Total EEPROM writes = ");
    Serial.print(ReadTotalWrites());
    Serial.print(". ");
    Serial.println("EEPROM is ok up to 100,000 writes.");
  }

}

bool Nonvolatile::NonvolatileWasWritten() {
  bool return_value = nonvolatile_was_written_;
  nonvolatile_was_written_ = false;
  return return_value;
}


//////////////////////////////////////////////////////
// Nonvolatile memory for position calibration values.
float Nonvolatile::ReadCalibrationPositionMin(int index) {
  int base_address = 3 * index + calibration_min_start_address_;
  return ReadNormalizedFloat(base_address);
}

void Nonvolatile::WriteCalibrationPositionMin(int index, float data) {
  int base_address = 3 * index + calibration_min_start_address_;
  WriteNormalizedFloat(base_address, data);
}

float Nonvolatile::ReadCalibrationPositionMax(int index) {
  int base_address = 3 * index + calibration_max_start_address_;
  return ReadNormalizedFloat(base_address);
}

void Nonvolatile::WriteCalibrationPositionMax(int index, float data) {
  int base_address = 3 * index + calibration_max_start_address_;
  WriteNormalizedFloat(base_address, data);
}

// The EEPPROM defaults to 0xFF. Therefore, 0xFF is
// calibration is not done. This ensures a new Teensy
// reads back the calibration never done before.
bool Nonvolatile::ReadCalibrationDoneFlag() {
  int data = EEPROM.read(calibration_flag_address_);
  if (data == 0xFF)
    return false;
  else
    return true;
}
void Nonvolatile::WriteCalibrationDoneFlag(bool data) {
  bool last_value = ReadCalibrationDoneFlag();
  if (last_value != data) {
    int write_data;
    if (data == false)
      write_data = 0xFF;
    else
      write_data = 0;
    EEPROM.write(calibration_flag_address_, write_data);
    nonvolatile_was_written_ = true;
  }
}
/////////////////////////////////////////////


//////////////////////////////////////////////////////
// Nonvolatile memory for velocity calibration values.
float Nonvolatile::ReadMaxVelocity() {
  return ReadNormalizedFloat(velocity_scale_address_);
}
void Nonvolatile::WriteMaxVelocity(float data) {
  WriteNormalizedFloat(velocity_scale_address_, data);
}
bool Nonvolatile::ReadMaxVelocityStoredFlag() {
  int data = EEPROM.read(velocity_scale_flag_address_);
  if (data == 0xFF)
    return false;
  else
    return true;
}
void Nonvolatile::WriteMaxVelocityStoredFlag(bool data) {
  bool last_value = ReadMaxVelocityStoredFlag();
  if (last_value != data) {
    int write_data;
    if (data == false)
      write_data = 0xFF;
    else
      write_data = 0;
    EEPROM.write(velocity_scale_flag_address_, write_data);
    nonvolatile_was_written_ = true;
  }
}
/////////////////////////////////////////////


/////////////////////////////////////////////
// Read / write floating point values.
float Nonvolatile::ReadNormalizedFloat(int address) {
  int data_int = 0;
  for (int shift = 0; shift < 3; shift++) {
    data_int |= (EEPROM.read(address + shift) << (8*shift));
  }
  return static_cast<float>(data_int) / calibration_scale_value_;
}
void Nonvolatile::WriteNormalizedFloat(int address, float data) {
  int data_int = static_cast<int>(data * calibration_scale_value_);
  for (int shift = 0; shift < 3; shift++) {
    EEPROM.write(address + shift, (data_int >> (8*shift)) & 0x0FF);
  }
  nonvolatile_was_written_ = true;
}
/////////////////////////////////////////////


///////////////////////////////////////////////////////////
// Nonvolatile memory for counting total writes
// This is writes to any address.
// So, for knowing when total writes is close
// to exceeding maximum tolerable writes, the
// count value is pessimistic.
// This is not called by write functions in nonvolatile.cpp
// because sometimes this is called in loops by other
// class instances and don't want so many EEPROM writes.
///////////////////////////////////////////////////////////
void Nonvolatile::UpdateAndWriteTotalWrites() {
  int x = ReadTotalWrites() + 1;
  for (int shift = 0; shift < 3; shift++) {
    EEPROM.write(total_writes_address_ + shift, (x >> (8*shift)) & 0x0FF);
  }
  nonvolatile_was_written_ = true;
}
int Nonvolatile::ReadTotalWrites() {
  int x = 0;
  for (int shift = 0; shift < 3; shift++) {
    x |= (EEPROM.read(total_writes_address_ + shift) << (8*shift));
  }
  // Rollover early because on startup each cell returns 0xFF.
  if (x == 0x00FFFFFF) {
    x = 0;
  }
  return x;
}