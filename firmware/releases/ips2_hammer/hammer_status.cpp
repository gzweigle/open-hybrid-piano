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
// hammer_status.cpp
//

#include "hammer_status.h"

HammerStatus::HammerStatus() {}

void HammerStatus::Setup(DspPedal *dspp, TestpointLed *testp,
int debug_level, float damper_threshold_low, float damper_threshold_high,
float strike_threshold) {

  debug_level_ = debug_level;

  dspp_ = dspp;
  testp_ = testp;
  damper_threshold_low_ = damper_threshold_low;
  damper_threshold_high_ = damper_threshold_high;
  strike_threshold_ = strike_threshold;

  lower_r_led_interval_when_all_notes_calibrated_ = 250;
  lower_r_led_last_change_ = millis();
  lower_r_led_state_ = false;

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

  statistics_interval_ = 5000;
  statistics_last_change_ = millis();
  for (int k = 0; k < NUM_NOTES; k++) {
    min_[k] = 1.0;
    max_[k] = 0.0;
    played_count_[k] = 0;
  }

}

// Control LED on front of board next to Teensy.
void HammerStatus::FrontLed(const float *calibrated_floats,
bool switch_high_damper_threshold) {

  float damper_threshold;
  if (switch_high_damper_threshold == true) {
    damper_threshold = damper_threshold_high_;
  }
  else {
    damper_threshold = damper_threshold_low_;
  }

  bool found;

  // Turn on LED if any key is above damper threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > damper_threshold) {
      testp_->SetTp9(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp9(false);

  // Turn on LED if any key is above strike thresold threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > strike_threshold_) {
      testp_->SetTp10(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp10(false);

  // Turn on LED if any pedal input is above its threshold.
  if (dspp_->GetSustainCrossedUpThreshold() || 
  dspp_->GetSostenutoCrossedUpThreshold() ||
  dspp_->GetUnaCordaCrossedUpThreshold()) {
    testp_->SetTp11(false);
  }
  else if (dspp_->GetSustainCrossedDownThreshold() || 
  dspp_->GetSostenutoCrossedDownThreshold() ||
  dspp_->GetUnaCordaCrossedDownThreshold()) {
    testp_->SetTp11(true);
  }
}

// Flash lower right LED (under TFT) based on calibration status.
void HammerStatus::LowerRightLed(bool all_notes_using_cal) {
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

// Control the two LEDs under the Six Channel Analog (SCA) board.
void HammerStatus::SCALed() {
  if (millis() - sca_led_last_change_ > sca_led_interval_) {
    sca_led_state_ = !sca_led_state_;
    sca_led_last_change_ = millis();
  }

  testp_->SetScaLedL(sca_led_state_);
  testp_->SetScaLedR(sca_led_state_);
}

// Ethernet LED control.
void HammerStatus::EthernetLed() {
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
void HammerStatus::SerialMonitor(const int *adc, const float *position,
const bool *event, bool canbus_enable, bool switch_external_damper_board) {

  if (debug_level_ >= DEBUG_MINOR) {
    // Filter displayed data.
    unsigned int rs0, rs1;
    rs0 = filter0_.boxcarFilterUInts(adc[39]);  // Middle C.
    rs1 = filter1_.boxcarFilterUInts(adc[53]);  // D5.
    // Display the raw ADC counts and the fully calibrated floats for two notes.
    if (millis() - serial_last_change_ > serial_interval_) {
      Serial.print("Hammer Board");
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
          Serial.print("   N ");
        else
          Serial.print(" min=");
        Serial.print(min_[k]);
        if (k == largest_ind)
          Serial.print("   X ");
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

      if (canbus_enable == false && switch_external_damper_board == true) {
        Serial.print("Warning - ");
        Serial.println("  Trying to use remote board without Can bus enabled.");
      }
    }
    else {
      // In between display updates, keep track of statistics.
      for (int k = 0; k < NUM_NOTES; k++) {
        if (position[k] < min_[k])
          min_[k] = position[k];
        else if (position[k] > max_[k])
          max_[k] = position[k];
        if (event[k] == true)
          played_count_[k]++;
      }
    }
  }
}