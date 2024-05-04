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
// calibration_sensor.h
//
// Undo the CNY-70 nonlinearity.
//
// TODO - Eliminate using a switch, run always, delete temporary values.
//
// For ips pcb version 2.X
// For sca pcb version 0.X

#include "calibration_sensor.h"

CalibrationSensor::CalibrationSensor() {}

void CalibrationSensor::Setup(float threshold, float match_gain,
float match_offset, int debug_level) {

  debug_level_ = debug_level;

  for (int note = 0; note < NUM_NOTES; note++) {
    gain_staged_[note] = 1.0;
    offset_staged_[note] = 0.0;
    gain_[note] = 1.0;
    offset_[note] = 0.0;
    max_[note] = 0.0;
    min_[note] = 1.0;
    max_updated_staged_[note] = false;
    max_updated_[note] = false;
    for (int sample = 0; sample < CALIBRATION_FILTER_SAMPLES; sample++) {
      filter_buffer_[note][sample] = 0.0;
    }
  }

  // These are temporary while evaluating performance.
  // Allows playing with both calibrated and uncalibrated keys.
  // Eventually, when calibration is automated and
  // does not need a switch to run, delete these.
  orig_gain_ = match_gain;
  orig_offset_ = match_offset;

  // Temporary to help while evaluating performance.
  // Set to zero to disable.
  update_threshold_ = 0;

  // An input must exceed this value otherwise no
  // calibration values are computed.
  // Warning - any key that starts out with extremely
  // out of tolerance value (less than threshold_ when
  // a key is pressed) is not calibrated.
  threshold_ = threshold;
}

// Create and apply calibration values.
// Return true if all NUM_NOTES notes are using a calibrated value.
bool CalibrationSensor::Calibration(bool switch_freeze_cal_values,
bool switch_disable_and_reset_calibration, float *out, const float *in) {

  /////////////////////////////////
  // APPLYING CALIBRATION VALUES //
  /////////////////////////////////
  for (int note = 0; note < NUM_NOTES; note++) {
    if (switch_disable_and_reset_calibration == false) {
      if (max_updated_[note] == true) {
        // See design document for details of the algorithm.
        out[note] = (log(in[note]) - offset_[note]) * gain_[note];
        out[note] = out[note] * orig_gain_ + orig_offset_;
      }
      else {
        out[note] = in[note];
      }
    }
    else {
      out[note] = in[note];
    }
  }
  // Copy pedals over without modification.
  for (int note = NUM_NOTES; note < NUM_CHANNELS; note++) {
      out[note] = in[note];
  }

  /////////////////////////////////
  // BUILDING CALIBRATION VALUES //
  /////////////////////////////////

  // If turn off calibration, force rebuilding next time turn on.
  if (switch_disable_and_reset_calibration == true) {
    for (int note = 0; note < NUM_NOTES; note++) {
      gain_staged_[note] = 1.0;
      offset_staged_[note] = 0.0;
      gain_[note] = 1.0;
      offset_[note] = 0.0;
      max_[note] = 0.0;
      min_[note] = 1.0;
      max_updated_staged_[note] = false;
      max_updated_[note] = false;
    }
  }
  // If calibration values not frozen, update them.
  else if (switch_freeze_cal_values == false) {

    bool max_updated;

    for (int note = 0; note < NUM_NOTES; note++) {

      // Create a buffer of recent values.
      filter_buffer_[note][buffer_index_] = in[note];

      // Keep track of largest and smallest.
      if (in[note] > max_[note] + update_threshold_) {
        max_[note] = in[note];
        max_updated = true;
      }
      else if (in[note] < min_[note] - update_threshold_) {
        min_[note] = in[note];
        max_updated = false;

        // Show the values preceding the selected value.
        if (debug_level_ > DEBUG_NONE) {
          Serial.print("NEW MINIMUM FOR ");
          Serial.print(note);
          Serial.print(" ");
          int index = buffer_index_;
          for (int sample = 0; sample < CALIBRATION_FILTER_SAMPLES; sample++) {
            Serial.print(filter_buffer_[note][index]);
            Serial.print(" ");
            index--;
            if (index < 0) {
              index = CALIBRATION_FILTER_SAMPLES;
            }
          }
          Serial.println();
        }
      }
      else {
        max_updated = false;
      }

      // Check against a threshold to avoid a small incremental
      // max causing a bad gain and offset value. The min values
      // don't have this problem since hammers are normally near min.
      if (max_updated == true && max_[note] > threshold_) {

        // By the power of math, hereby nonlinear becomes linear
        gain_staged_[note] = 1.0 / (log(max_[note]) - log(min_[note]));
        offset_staged_[note] = log(min_[note]);

        // This note is ready to go.
        max_updated_staged_[note] = true;

        if (debug_level_ >= DEBUG_STATS) {
          Serial.print("==>Index: ");
          Serial.print(note);
          Serial.print(" Max: ");
          Serial.print(max_[note]);
          Serial.print(" Gain: ");
          Serial.print(gain_[note]);
          Serial.print(" Min: ");
          Serial.print(min_[note]);
          Serial.print(" Offset: ");
          Serial.println(offset_[note]);
        }
      }

      // Don't change anything until the hammer/damper settled back to its
      // resting position. Otherwise, could get one bad note when gain and
      // offset change while the hammer/damper is moving toward the sensor.
      if (in[note] < 1.5 * min_[note]) {
        if (debug_level_ >= DEBUG_STATS) {
          if (max_updated_[note] == false && max_updated_staged_[note] == true) {
            Serial.print("Now using a calibrated value. In=");
            Serial.print(in[note]);
            Serial.print(" min_=");
            Serial.println(min_[note]);
          }
        }
        max_updated_[note] = max_updated_staged_[note];
        gain_[note] = gain_staged_[note];
        offset_[note] = offset_staged_[note];
      }
    }

    buffer_index_++;
    if (buffer_index_ >= CALIBRATION_FILTER_SAMPLES) {
      buffer_index_ = 0;
    }

  }

  // Its a big AND gate.
  bool all_notes_are_using_calibration_values = true;
  for (int note = 0; note < NUM_NOTES; note++) {
    if (max_updated_[note] == false) {
      all_notes_are_using_calibration_values = false;
      break;
    }
  }
  return all_notes_are_using_calibration_values;

}