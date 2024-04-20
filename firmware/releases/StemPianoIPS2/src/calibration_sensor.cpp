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
float match_offset, int debug) {

  debug_ = debug;

  for (int k = 0; k < NUM_NOTES; k++) {
    gain_staged_[k] = 1.0;
    offset_staged_[k] = 0.0;
    gain_[k] = 1.0;
    offset_[k] = 0.0;
    max_[k] = 0.0;
    min_[k] = 1.0;
    max_updated_staged_[k] = false;
    max_updated_[k] = false;
  }

  // These are temporary while evaluating performance.
  // Allows playing with both calibrated and uncalibrated keys.
  // Eventually, when calibration is automated and
  // does not need a switch to run, delete these.
  orig_gain_ = match_gain;
  orig_offset_ = match_offset;

  // Temporary to help while evaluating performance.
  // Set to zero to disable.
  update_threshold_ = 1e-3;

  // An input must exceed this value otherwise no
  // calibration values are computed.
  // Warning - any key that starts out with extremely
  // out of tolerance value (less than threshold_ when
  // a key is pressed) is not calibrated.
  threshold_ = threshold;
}

// Create and apply calibration values.
void CalibrationSensor::Calibration(bool use_calibration,
bool disable_calibration, float *out, const float *in) {

  /////////////////////////////////
  // APPLYING CALIBRATION VALUES //
  /////////////////////////////////

  // During runtime, undo the CNY-70 nonlinearity.
  for (int k = 0; k < NUM_NOTES; k++) {
    // If calibration disabled, don't use it but don't force rebuilding.
    if (use_calibration == true && disable_calibration == false) {
      if (max_updated_[k] == true) {
        // See design document for details of the algorithm.
        out[k] = (log(in[k]) - offset_[k]) * gain_[k];
        // Put in same approx range as originals.
        out[k] = out[k] * orig_gain_ + orig_offset_;
      }
      else {
        out[k] = in[k];
      }
    }
    else {
      out[k] = in[k];
    }
  }
  // Except, copy pedals over without modification.
  for (int k = NUM_NOTES; k < NUM_CHANNELS; k++) {
      out[k] = in[k];
  }

  /////////////////////////////////
  // BUILDING CALIBRATION VALUES //
  /////////////////////////////////

  // If turn off calibration, force rebuilding next time turn on.
  if (use_calibration == false) {
    for (int k = 0; k < NUM_NOTES; k++) {
      max_updated_staged_[k] = false;
      max_[k] = 0.0;
      min_[k] = 1.0;
    }
  }
  else {
    bool value_updated;
    for (int k = 0; k < NUM_NOTES; k++) {
      if (in[k] > max_[k] + update_threshold_) {
        max_[k] = in[k];
        value_updated = true;
      }
      else if (in[k] < min_[k] - update_threshold_) {
        min_[k] = in[k];
        value_updated = true;
      }
      else {
        value_updated = false;
      }
      if (value_updated == true && max_[k] > threshold_) {

        // By the power of math, hereby nonlinear becomes linear
        gain_staged_[k] = 1.0 / (log(max_[k]) - log(min_[k]));
        offset_staged_[k] = log(min_[k]);

        // This note is ready to go.
        max_updated_staged_[k] = true;

        if (debug_ > 0) {
          Serial.print("Note: ");
          Serial.print(k);
          Serial.print(" Max: ");
          Serial.print(max_[k]);
          Serial.print(" Gain: ");
          Serial.print(gain_[k]);
          Serial.print(" Min: ");
          Serial.print(min_[k]);
          Serial.print(" Offset: ");
          Serial.println(offset_[k]);
        }
      }

      // Don't change anything until the hammer/damper settled back to its
      // resting position. Otherwise, get artifacts when the gain and
      // offset change while the hammer/damper is moving toward the sensor.
      if (in[k] < 1.5 * min_[k]) {
        if (debug_ > 0) {
          if (max_updated_[k] == false && max_updated_staged_[k] == true) {
            Serial.println("Now using calibrated value.");
          }
        }
        max_updated_[k] = max_updated_staged_[k];
        gain_[k] = gain_staged_[k];
        offset_[k] = offset_staged_[k];
      }
    }
  }
}