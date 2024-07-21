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
// damper_status.cpp
//

#include "damper_status.h"

DamperStatus::DamperStatus() {}

void DamperStatus::Setup(TestpointLed *testp, int debug_level) {

  debug_level_ = debug_level;

  testp_ = testp;

  lower_r_led_interval_when_all_notes_calibrated_ = 250;
  lower_r_led_last_change_ = millis();
  lower_r_led_state_ = false;
  lower_r_led_in_nonvol_mode_ = false;

  sca_led_interval_ = 1000;
  sca_led_last_change_ = millis();
  sca_led_state_ = false;

  ethernet_led_interval_on_ = 1000;
  ethernet_led_interval_off_ = 9000;
  ethernet_led_last_change_ = millis();
  ethernet_led_state_ = false;

  serial_interval_ = 2000;
  serial_last_change_ = millis();
  filter0_.Setup();
  filter1_.Setup();

  statistics_interval_ = 15000;
  statistics_last_change_ = millis();
  for (int k = 0; k < NUM_NOTES; k++) {
    min_[k] = 1.0;
    max_[k] = 0.0;
    played_count_[k] = 0;
  }

}

// Control LED on front of board next to Teensy.
void DamperStatus::FrontLed(const float *calibrated_floats,
float damper_threshold_low, float damper_threshold_med,
float damper_threshold_high, int test_index) {

  bool found;

  // Turn on LED if any key is above the low damper threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > damper_threshold_low) {
      testp_->SetTp9(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp9(false);

  // Turn on LED if any key is above the medium damper threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > damper_threshold_med) {
      testp_->SetTp10(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp10(false);

  // Turn on LED if any key is above the high damper threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > damper_threshold_high) {
      testp_->SetTp11(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp11(false);

}

// Flash lower right LED (under TFT) based on calibration status.
// Also indicate if nonvolatile was written.
void DamperStatus::LowerRightLed(bool all_notes_using_cal, 
bool nonvolatile_was_written) {
  // If Nonvolatile memory was written, turn on LED for a long duration.
  if (nonvolatile_was_written == true) {
    lower_r_led_in_nonvol_mode_ = true;
    lower_r_led_last_change_ = millis();
    testp_->SetLowerRightLED(true);
  }
  else if (lower_r_led_in_nonvol_mode_ == true) {
    if (millis() - lower_r_led_last_change_ > 24 *
    lower_r_led_interval_when_all_notes_calibrated_) {
      lower_r_led_in_nonvol_mode_ = false;
    }
  }
  // Flash slowly if all keys calibrated else flash quickly if some are not.
  else {
    unsigned long interval;
    if (all_notes_using_cal == true)
      interval = 6 * lower_r_led_interval_when_all_notes_calibrated_;
    else
      interval = lower_r_led_interval_when_all_notes_calibrated_;
  
    if (millis() - lower_r_led_last_change_ > interval) {
      lower_r_led_state_ = !lower_r_led_state_;
      lower_r_led_last_change_ = millis();
    }
    testp_->SetLowerRightLED(lower_r_led_state_);
  }
}

// Control the two LEDs under the Six Channel Analog (SCA) board.
void DamperStatus::SCALed() {
  if (millis() - sca_led_last_change_ > sca_led_interval_) {
    sca_led_state_ = !sca_led_state_;
    sca_led_last_change_ = millis();
  }

  testp_->SetScaLedL(sca_led_state_);
  testp_->SetScaLedR(sca_led_state_);
}

// Ethernet LED control.
void DamperStatus::EthernetLed() {
  unsigned int delta = millis() - ethernet_led_last_change_;
  if (ethernet_led_state_ == false) {
    if (delta > ethernet_led_interval_off_) {
      ethernet_led_state_ = true;
      ethernet_led_last_change_ = millis();
    }
  }
  else {
    if (delta > ethernet_led_interval_on_) {
      ethernet_led_state_ = false;
      ethernet_led_last_change_ = millis();
    }
  }
  testp_->SetEthernetLED(ethernet_led_state_);
}

// General information sent to the serial monitor.
void DamperStatus::SerialMonitor(const int *adc, const float *position,
float damper_threshold) {

  if (debug_level_ >= DEBUG_MINOR) {
    // Filter displayed data.
    unsigned int rs0, rs1;
    rs0 = filter0_.boxcarFilterUInts(adc[39]);  // Middle C.
    rs1 = filter1_.boxcarFilterUInts(adc[53]);  // D5.
    // Display the raw ADC counts and the fully calibrated floats for two notes.
    if (millis() - serial_last_change_ > serial_interval_) {
      Serial.print("Damper Board");
      Serial.print("    position=(");
      Serial.print(rs0);
      Serial.print(")(");
      Serial.print(position[39]);
      Serial.print(")    position=(");
      Serial.print(rs1);
      Serial.print(")(");
      Serial.print(position[53]);
      Serial.println(")");
      serial_last_change_ = millis();
    }
  }

  if (debug_level_ >= DEBUG_STATS) {
    if (millis() - statistics_last_change_ > statistics_interval_) {

      statistics_last_change_ = millis();

      // Find largest and smallest values, of all the values,
      // and highlight when display with all capital letters.
      int smallest_ind = 0;
      int largest_ind = 0;
      float smallest_value = 1.0;
      float largest_value = 0.0;
      for (int k = 0; k < NUM_NOTES; k++) {
        if (min_[k] < smallest_value) {
          smallest_value = min_[k];
          smallest_ind = k;
        }
        if (max_[k] > largest_value) {
          largest_value = max_[k];
          largest_ind = k;
        }
      }

      // For each note, display statistics since last time.

      for (int k = 0; k < NUM_NOTES; k++) {
        if (k < 10)
          Serial.print("index=0");  // Insert 0 to keep vertical alignment.
        else
          Serial.print("index=");
        Serial.print(k);
        if (played_count_[k] < 10)
          Serial.print(" count=0");  // Insert 0 to keep vertical alignment.
        else
          Serial.print(" count=");
        Serial.print(played_count_[k]);
        if (k == smallest_ind)
          Serial.print("  v  ");
        else
          Serial.print(" min=");
        Serial.print(min_[k]);
        if (k == largest_ind)
          Serial.print("  ^  ");
        else
          Serial.print(" max=");
        Serial.print(max_[k]);
        if (k%4 == 3)
          Serial.println();
        else
          Serial.print(" ");
        
        // Start over for the next display.
        played_count_[k] = 0;
        min_[k] = 1.0;
        max_[k] = 0.0;
      }
      Serial.println();

    }
    else {
      // In between display updates, keep track of statistics.
      for (int k = 0; k < NUM_NOTES; k++) {
        if (position[k] < min_[k])
          min_[k] = position[k];
        else if (position[k] > max_[k])
          max_[k] = position[k];

        // TODO -
        // This increments while damper is above
        // threshold so it is not very useful.
        if (position[k] > damper_threshold)
          played_count_[k]++;
      }
    }
  }
}