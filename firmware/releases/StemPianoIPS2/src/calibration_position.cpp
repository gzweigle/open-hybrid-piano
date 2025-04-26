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
// calibration_position.h
//
// Undo the position related errors due to mechanical tolerances.

#include "calibration_position.h"

CalibrationPosition::CalibrationPosition() {}

void CalibrationPosition::Setup(float threshold,
int debug_level, Nonvolatile *Nv) {

  // Setup the calibration state.
  InitializeState(Nv, debug_level);
  
  // Only allow an EEPROM write every three seconds.
  // This is a safety feature in case of a software bug
  // or in case of extreme configuration switch noise.
  // EEPROM is good for approximately 100,000 writes.
  // Three seconds is assumed fastest time someone
  // would configuration switch off then back on.
  min_write_interval_millis_ = 3000;
  last_write_time_ = millis();

  // Due to various nonideal conditions could
  // get clipping. Use this value to give some margin.
  gain_correction_ = 0.99;

  // Threshold scaling for when to apply staged_values.
  staged_scaling_value_ = 1.5;
  // A note must exceed this value to get a new max_[].
  // Avoids low max_ and bad cal values when key at rest.
  threshold_ = threshold;

  // Write EEPROM is only on a switch change.
  // Initialize freeze true to avoid a false write on startup.
  switch_freeze_cal_values_last_ = true;
  switch_disable_and_reset_calibration_last_ = false;

}

// Return true if all NUM_NOTES notes have a calibrated value.
bool CalibrationPosition::Calibration(bool switch_freeze_cal_values,
bool switch_disable_and_reset_calibration, float *out, const float *in) {

  ApplyCalibrationValues(switch_disable_and_reset_calibration, out, in);

  bool all_notes_calibrated = BuildCalibrationValues(switch_freeze_cal_values,
  switch_disable_and_reset_calibration, in);

  // Use millis() to limit number of writes per second.
  // This is to prevent a bug or other problem causing
  // too many writes to the EEPROM memory.
  if (millis() - last_write_time_ > min_write_interval_millis_) {
    WriteEeprom(switch_freeze_cal_values, switch_disable_and_reset_calibration,
    all_notes_calibrated);
    last_write_time_ = millis();
  }

  return all_notes_calibrated;
}

// Applying the calibration correction to each received input.
void CalibrationPosition::ApplyCalibrationValues(bool
switch_disable_and_reset_calibration, float *out, const float *in) {

  for (int note = 0; note < NUM_NOTES; note++) {
    if (switch_disable_and_reset_calibration == false) {
      if (gain_[note] > 0.0) {
        // See design document for algorithm details.
        out[note] = static_cast<float>((static_cast<double>(log(in[note])) - offset_[note]) * gain_[note]);
      }
      else {
        out[note] = in[note];
      }
    }
    else {
      out[note] = in[note];
    }
  }
  // Copy pedals without modification.
  // If use these extra inputs for more piano keys, include in above.
  for (int note = NUM_NOTES; note < NUM_CHANNELS; note++) {
      out[note] = in[note];
  }
}

// Construct the calibration correction values.
bool CalibrationPosition::BuildCalibrationValues(bool switch_freeze_cal_values,
bool switch_disable_and_reset_calibration, const float *in) {

  // If turn off calibration, force rebuilding next time turn on.
  if (switch_disable_and_reset_calibration == true) {
    for (int note = 0; note < NUM_NOTES; note++) {
      max_[note] = 0.0;
      min_[note] = 1.0;
      gain_[note] = gain_default_;
      offset_[note] = 0.0;
      gain_staged_[note] = gain_[note];
      offset_staged_[note] = offset_[note];
      min_at_least_one_[note] = false;
      max_at_least_one_[note] = false;
    }
  }
  // If calibration values not frozen, update them.
  else if (switch_freeze_cal_values == false) {

    bool found_new_min_or_max;
    double in_double;

    for (int note = 0; note < NUM_NOTES; note++) {

      in_double = static_cast<double>(in[note]);

      // Create a buffer of recent values.
      filter_buffer_[note][buffer_index_] = in[note];

      // Keep track of largest and smallest.
      if (in_double > max_[note] && in_double > threshold_) {
        max_[note] = in_double;
        max_at_least_one_[note] = true;
        found_new_min_or_max = true;
      }
      else if (in_double < min_[note]) {
        min_[note] = in_double;
        min_at_least_one_[note] = true;
        found_new_min_or_max = true;
      }
      else {
        found_new_min_or_max = false;
      }

      // Got a new min or max and also the min and
      // max both have at least one good value.
      if (found_new_min_or_max == true &&
        max_at_least_one_[note] == true &&
        min_at_least_one_[note] == true) {

        gain_staged_[note] = GetGain(min_[note], max_[note]);
        offset_staged_[note] = GetOffset(min_[note]);

        if (debug_level_ >= DEBUG_ALG) {
          Serial.print("==>Index: ");
          Serial.print(note);
          Serial.print(" Max: ");
          Serial.print(max_[note]);
          Serial.print(" Min: ");
          Serial.print(min_[note]);
          Serial.print(" Gain: ");
          Serial.print(gain_[note]);
          Serial.print(" Offset: ");
          Serial.println(offset_[note]);
        }
      }

      // Don't change anything until the hammer/damper settled back to its
      // resting position. Otherwise, could get a bad note when gain and
      // offset change while the hammer/damper is moving toward the sensor.
      if (in[note] < static_cast<float>(staged_scaling_value_ * min_[note])) {
        gain_[note] = gain_correction_ * gain_staged_[note];
        gain_[note] = gain_staged_[note];
        offset_[note] = offset_staged_[note];
      }
    }

    // For future.
    buffer_index_++;
    if (buffer_index_ >= CALIBRATION_FILTER_SAMPLES) {
      buffer_index_ = 0;
    }

  }

  // Its a big AND gate.
  bool all_notes_are_using_calibration_values = true;
  for (int note = 0; note < NUM_NOTES; note++) {
    if (gain_[note] <= 0.0) {
      all_notes_are_using_calibration_values = false;
      break;
    }
  }

  return all_notes_are_using_calibration_values;

}

// By the power of math, hereby nonlinear becomes linear
double CalibrationPosition::GetGain(double min, double max) {
  return (1.0 / (log(max) - log(min)));
}
double CalibrationPosition::GetOffset(double min) {
  return log(min);
}

// Large initialization loop.
void CalibrationPosition::InitializeState(Nonvolatile *Nv, int debug_level) {

  // In function because function depends on them.
  Nv_ = Nv;
  debug_level_ = debug_level;
  gain_default_ = -1.0; // A gain < 0 indicates not yet set.

  // Display if reading initial values from EEPROM.
  bool all_notes_calibrated = Nv_->ReadCalibrationDoneFlag();
  unsigned long start_read_time = micros();
  if (debug_level_ >= DEBUG_INFO) {
    if (all_notes_calibrated == true) {
      Serial.print("Reading initial position calibration values from EEPROM, time = ");
    }
    else {
      Serial.println("No initial position calibration were found in EEPROM.");
    }
  }

  for (int note = 0; note < NUM_NOTES; note++) {

    if (all_notes_calibrated == true) {
      // If all notes have a calibration value in memory, apply all.
      max_[note] = Nv_->ReadCalibrationPositionMax(note);
      min_[note] = Nv_->ReadCalibrationPositionMin(note);
      gain_[note] = GetGain(min_[note], max_[note]);
      offset_[note] = GetOffset(min_[note]);
    }
    else {
      // Not all notes had calibration values, therefore,
      // these default values will force recalibration.
      max_[note] = 0.0;
      min_[note] = 1.0;
      // When gain_ less than gain_default_, calibration values not applied.
      gain_[note] = gain_default_;
      offset_[note] = 0.0;
    }
    // Stage the gain and offset because only apply new values when
    // a key is at rest. If apply when playing a key, the new values
    // could cause the wrong volume, and potentially a loud volume.
    gain_staged_[note] = gain_[note];
    offset_staged_[note] = offset_[note];

    // Don't update calibration until both max and min have new values.
    min_at_least_one_[note] = false;
    max_at_least_one_[note] = false;

    // Future... for skipping large outliers.
    for (int sample = 0; sample < CALIBRATION_FILTER_SAMPLES; sample++) {
      filter_buffer_[note][sample] = 0.5;  // Default to middle.
    }
  }
  if (debug_level_ >= DEBUG_INFO) {
    if (all_notes_calibrated == true) {
      Serial.print(micros() - start_read_time);
      Serial.println(" microseconds.");
    }
  }
}

// hic sunt dracones

//////////////////////////////////////////
// WRITING CALIBRATION VALUES TO EEPROM //
//////////////////////////////////////////
// The EEPROM write endurance is approximately 100,000 cycles.
// This is why only write on the transition of a configuration switch.
// A bug in this code that accidentally causes multiple writes
// repeatedly could permanently damage the EEPROM.
// So, be careful when editing code below.
void CalibrationPosition::WriteEeprom(bool switch_freeze_cal_values,
bool switch_disable_and_reset_calibration,
bool all_notes_are_using_calibration_values) {

  if (switch_freeze_cal_values == true &&
  switch_freeze_cal_values_last_ == false) {
    if (all_notes_are_using_calibration_values == true) {
      unsigned long start_write_time = micros();
      if (debug_level_ >= DEBUG_INFO) {
        Serial.println("Writing position max/min to EEPROM.");
      }
      for (int note = 0; note < NUM_NOTES; note++) {
        Nv_->WriteCalibrationPositionMax(note, max_[note]);
        Nv_->WriteCalibrationPositionMin(note, min_[note]);
      }
      Nv_->WriteCalibrationDoneFlag(true);
      Nv_->UpdateAndWriteTotalWrites();
      if (debug_level_ >= DEBUG_INFO) {
        Serial.print("Finished writing EEPROM, write time = ");
        Serial.print(micros() - start_write_time);
        Serial.println(" microseconds.");
      }
    }
    else if (debug_level_ >= DEBUG_INFO) {
      Serial.println("All notes do not have calibration values.");
      Serial.println("Therefore, calibration values not written to EEPROM.");
    }
  }

  // To clear calibration values, just clear the flag.
  // Do not need to overwrite the calibration values.
  // This avoids extra unnecessary EEPROM writes.
  if (switch_disable_and_reset_calibration == true &&
  switch_disable_and_reset_calibration_last_ == false) {
    bool cal_done_flag = Nv_->ReadCalibrationDoneFlag();
    // To avoid unnecessary writes, only write a false if previous was true.
    if (cal_done_flag == true) {
      unsigned long start_write_time = micros();
      if (debug_level_ >= DEBUG_INFO) {
        Serial.println("Clearing position max/min from EEPROM.");
      }
      Nv_->WriteCalibrationDoneFlag(false);
      Nv_->UpdateAndWriteTotalWrites();
      if (debug_level_ >= DEBUG_INFO) {
        Serial.print("Finished writing EEPROM, write time = ");
        Serial.print(micros() - start_write_time);
        Serial.println(" microseconds.");
      }
    }
    else {
      if (debug_level_ >= DEBUG_INFO) {
        Serial.println("Position max/min values in EEPROM were already clear.");
      }
    }
  }
  switch_freeze_cal_values_last_ = switch_freeze_cal_values;
  switch_disable_and_reset_calibration_last_ = switch_disable_and_reset_calibration;
}