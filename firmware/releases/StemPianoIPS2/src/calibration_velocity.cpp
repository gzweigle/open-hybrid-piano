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
// calibration_velocity.cpp
//
// All calibration is done at the hammer board.
// The damper board does not use this code.
//
// The purpose of this code is to receive velocities in meters / second
// and scale them to range [0.0, ..., 1.0].
//
// Two methods:
//
// (1) Coarse velocity calibration by applying fixed_velocity_scale.
//     The simplest velocity calibration approach is to adjust
//     fixed_velocity_scale in hammer_settings.cpp so that the
//     loudest velocity is scaled at 1.0. Set fixed_velocity_scale
//     after building the complete physical piano action setup.
//
// (2) Optionally apply a dynamically measured fine tuning velocity
//     scaling. This optional scaling is enabled by a switch on
//     the hardware. Code does not update the velocity scaling unless
//     all notes are position calibrated otherwise get incorrect
//     velocity values due to positon calibration errors.
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//

#include "calibration_velocity.h"

CalibrationVelocity::CalibrationVelocity() {}

void CalibrationVelocity::Setup(float fixed_velocity_scale, int debug_level,
Nonvolatile *Nv) {
  fixed_velocity_scale_ = fixed_velocity_scale;
  debug_level_ = debug_level;
  
  // Only allow an EEPROM write every three seconds.
  // This is a safety feature in case of a software bug
  // or in case of extreme configuration switch noise.
  // EEPROM is good for approximately 100,000 writes.
  // Three seconds is assumed fastest time someone
  // would configuration switch off then back on.
  min_write_interval_millis_ = 3000;
  last_write_time_ = millis();

  // Restrict the hammer velocity scaling to the following range.
  // If need a wider range, change velocity_scale in the settings
  // file. The reason for this narrow range is to avoid very loud
  // notes on user corner cases that are difficult to eliminate
  // without making things complicated for the user.
  min_max_hammer_velocity_ = 0.75;
  max_max_hammer_velocity_ = 1.25;

  // Write EEPROM is only on a switch change.
  // Initialize freeze true to avoid a false write on startup.
  switch_freeze_cal_values_last_ = true;
  switch_disable_and_reset_calibration_last_ = false;

  // Read initial calibration value from EEPROM.
  InitializeState(Nv);
}

void CalibrationVelocity::HammerVelocityScale(float *velocity_data,
const bool *velocity_event, bool switch_enable_dynamic_velocity,
bool switch_freeze_cal_values, bool switch_disable_and_reset_calibration,
bool all_notes_using_cal) {

  // This is the settings-based coarse adjustment.
  // In many cases it is sufficient and the subsequent
  // optional (enabled with a hardware switch)
  // adjustment is not needed.
  FixedScale(velocity_data, velocity_event);

  // The code below is for the optional adaptive velocity adjustment.

  // Until all positions are calibrated, do not update
  // the optional velocity calibration.
  if (all_notes_using_cal == false) {
    new_max_velocity_ = false;
    max_hammer_velocity_ = min_max_hammer_velocity_;
  }
  // If turn off calibration, force rebuilding next time turn on.
  if (switch_disable_and_reset_calibration == true) {
    new_max_velocity_ = false;
    max_hammer_velocity_ = min_max_hammer_velocity_;
  }
  // Require fully calibrated positions otherwise the velocity
  // will have errors due to mechanical tolerances.
  else if (switch_freeze_cal_values == false &&
  all_notes_using_cal == true) {
    BuildVelocityScale(velocity_data, velocity_event);
  }

  // Use millis() to limit number of writes per second.
  // This is to prevent a bug or other problem causing
  // too many writes to the EEPROM memory.
  if (millis() - last_write_time_ > min_write_interval_millis_) {
    WriteEeprom(switch_freeze_cal_values, switch_disable_and_reset_calibration);
    last_write_time_ = millis();
  }

  // An optional dynamic algorithm, if the coarse value is not sufficient.
  // Run this last because ApplyVelocityScale() modifies velocity_data.
  if (switch_enable_dynamic_velocity == true) {
    if (switch_disable_and_reset_calibration == false) {
      if (new_max_velocity_ == true) {
        ApplyVelocityScale(velocity_data, velocity_event);
      }
    }
  }
}

// For damper velocity, only use the coarse settings-based adjustment.
void CalibrationVelocity::DamperVelocityScale(float *velocity_data,
const bool *velocity_event) {
  FixedScale(velocity_data, velocity_event);
}

// Use the setting to adjust velocity.
void CalibrationVelocity::FixedScale(float *velocity_data,
const bool *velocity_event) {
  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= fixed_velocity_scale_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= DEBUG_MINOR) {
          Serial.print("FixedScale(): velocity of key ");
          Serial.print(key);
          Serial.print(" is set at limit, orig velocity was ");
          Serial.print(velocity_data[key] / fixed_velocity_scale_);
          Serial.println(".");
        }
        velocity_data[key] = 1.0;
      }
    }
  }
}

// Save the largest velocity from any piano key.
void CalibrationVelocity::BuildVelocityScale(const float *velocity_data,
const bool *velocity_event) {
  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      if (velocity_data[key] > max_hammer_velocity_) {
        max_hammer_velocity_ = velocity_data[key];
        reciprocal_max_hammer_velocity_ = 1.0 / max_hammer_velocity_;
        new_max_velocity_ = true;
        if (debug_level_ >= DEBUG_STATS) {
          Serial.print("BuildVelocityScale() - new max velocity = ");
          Serial.print(max_hammer_velocity_);
          Serial.print(", key = ");
          Serial.print(key);
          Serial.println(".");
        }
      }
    }
  }
}
  
// Apply dynamic scaling to put velocity in range [0.0,...,1.0].
void CalibrationVelocity::ApplyVelocityScale(float *velocity_data,
const bool *velocity_event) {
  for (int key = 0; key < NUM_NOTES; key++) {
    if (velocity_event[key] == true) {
      velocity_data[key] *= reciprocal_max_hammer_velocity_;
      if (velocity_data[key] > 1.0) {
        if (debug_level_ >= DEBUG_MINOR) {
          Serial.print("ApplyVelocityScale(): velocity of key ");
          Serial.print(key);
          Serial.print(" hit limit, orig velocity was ");
          Serial.print(velocity_data[key] / reciprocal_max_hammer_velocity_);
          Serial.println(".");
        }
        velocity_data[key] = 1.0;
      }
    }
  }
}

// Read initial scaling from EEPROM.
void CalibrationVelocity::InitializeState(Nonvolatile *Nv) {
  Nv_ = Nv;
  new_max_velocity_ = Nv_->ReadMaxVelocityStoredFlag();
  if (new_max_velocity_ == true) {
    max_hammer_velocity_ = static_cast<float>(Nv_->ReadMaxVelocity());
    if (debug_level_ >= DEBUG_STATS) {
      Serial.print("Read initial velocity scale of ");
      Serial.print(max_hammer_velocity_);
      Serial.println(" from EEPROM.");
    }
    if (max_hammer_velocity_ < min_max_hammer_velocity_ ||
        max_hammer_velocity_ > max_max_hammer_velocity_) {
      // In case of an unexpected EEPROM failure, set to 1.0
      // which effectively turns off the scaling.
      max_hammer_velocity_ = 1.0;
      new_max_velocity_ = false;
      if (debug_level_ >= DEBUG_STATS) {
        Serial.println("EEPROM issue. Velocity scale forced to 1.0.");
      }
    }
  }
  else {
    if (debug_level_ >= DEBUG_STATS) {
      Serial.println("No initial velocity scale was found in EEPROM.");
    }
    max_hammer_velocity_ = 1.0;
  }
  reciprocal_max_hammer_velocity_ = 1.0 / max_hammer_velocity_;
}

////////////////////////////////////////////
// WRITING VELOCITY SCALE VALUE TO EEPROM //
////////////////////////////////////////////
// The EEPROM write endurance is approximately 100,000 cycles.
// This is why only write on the transition of a configuration
// switch. A bug in this code that accidentally causes multiple
// writes repeatedly could permanently damage the EEPROM.
// So, be careful when editing code below.
void CalibrationVelocity::WriteEeprom(bool switch_freeze_cal_values,
bool switch_disable_and_reset_calibration) {

  if (switch_freeze_cal_values == true &&
  switch_freeze_cal_values_last_ == false) {
    if (new_max_velocity_ == true) {
      unsigned long start_write_time = micros();
      if (debug_level_ >= DEBUG_STATS) {
        Serial.print("Writing max hammer velocity = ");
        Serial.print(max_hammer_velocity_);
        Serial.println(" to EEPROM.");
      }
      Nv_->WriteMaxVelocity(static_cast<float>(max_hammer_velocity_));
      Nv_->WriteMaxVelocityStoredFlag(true);
      Nv_->UpdateAndWriteTotalWrites();
      if (debug_level_ >= DEBUG_STATS) {
        Serial.print("Finished writing EEPROM, write time = ");
        Serial.print(micros() - start_write_time);
        Serial.println(" microseconds.");
      }
    }
    else if (debug_level_ >= DEBUG_STATS) {
      Serial.println("No new velocity scale value.");
      Serial.println("Therefore, calibration values not written to EEPROM.");
    }
  }

  // To clear calibration values, just clear the flag.
  // Do not need to overwrite the calibration values.
  // This avoids extra unnecessary EEPROM writes.
  if (switch_disable_and_reset_calibration == true &&
  switch_disable_and_reset_calibration_last_ == false) {
    new_max_velocity_ = Nv_->ReadMaxVelocityStoredFlag();
    // To avoid unnecessary writes, only write a false if previous was true.
    if (new_max_velocity_ == true) {
      unsigned long start_write_time = micros();
      if (debug_level_ >= DEBUG_STATS) {
        Serial.println("Clearing velocity scale from EEPROM.");
      }
      Nv_->WriteMaxVelocityStoredFlag(false);
      Nv_->UpdateAndWriteTotalWrites();
      if (debug_level_ >= DEBUG_STATS) {
        Serial.print("Finished writing EEPROM, write time = ");
        Serial.print(micros() - start_write_time);
        Serial.println(" microseconds.");
      }
    }
    else {
      if (debug_level_ >= DEBUG_STATS) {
        Serial.println("Velocity scale in EEPROM was already clear.");
      }
    }
  }
  switch_freeze_cal_values_last_ = switch_freeze_cal_values;
  switch_disable_and_reset_calibration_last_ = switch_disable_and_reset_calibration;
}