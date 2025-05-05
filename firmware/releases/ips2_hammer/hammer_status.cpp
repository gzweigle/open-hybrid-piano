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
// hammer_status.cpp
//

#include "hammer_status.h"

HammerStatus::HammerStatus() {}

void HammerStatus::Setup(DspPedal *dspp, TestpointLed *testp, int debug_level) {

  debug_level_ = debug_level;

  dspp_ = dspp;
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

  serial_interval_ = 10000;
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
  print_stats_count_ = -1;

  interval_interval_ = 30000;
  interval_max_ = 0;
  interval_start_millis_ = millis();

}

// Control LED on front of board next to Teensy.
void HammerStatus::FrontLed(const float *calibrated_floats,
float damper_threshold, float strike_threshold, int test_index) {

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
    if (calibrated_floats[k] > strike_threshold) {
      testp_->SetTp10(true);
      found = true;
      break;
    }
  }
  if (found == false)
    testp_->SetTp10(false);

  if (test_index < 0) {
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
  else {
    // If in high-speed test mode, leave on.
    testp_->SetTp11(true);
  }
}

// Flash lower right LED (under TFT) based on calibration status.
// Also indicate if nonvolatile was written.
void HammerStatus::LowerRightLed(bool all_notes_using_cal, 
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

  if (debug_level_ >= DEBUG_NOTES) {
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

  if (debug_level_ >= DEBUG_NOTES) {

    bool print_now;

    if (millis() - statistics_last_change_ > statistics_interval_) {
      statistics_last_change_ = millis();

      print_now = true;

      if (canbus_enable == false && switch_external_damper_board == true) {
        Serial.print("Warning - ");
        Serial.println("  Trying to use remote board without Can bus enabled.");
      }

    }
    else {
      print_now = false;
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
    IncrementalPrint(print_now);
  }

}

// Display the maximum processing interval
void HammerStatus::DisplayProcessingIntervalStart() {
  interval_start_micros_ = micros();
}
void HammerStatus::DisplayProcessingIntervalEnd() {
  if (debug_level_ >= DEBUG_INFO) {
    unsigned long interval = micros() - interval_start_micros_;
    if (interval > interval_max_) {
      interval_max_ = interval;
    }
    if (millis() - interval_start_millis_ > interval_interval_) {
      interval_start_millis_ = millis();
      Serial.printf("Max processing interval = %d microseconds.\n", interval_max_);
      interval_max_ = 0;
    }
  }
}

void HammerStatus::IncrementalPrint(bool print_now) {
  if (print_now == true) {
    print_stats_count_ = 0;
  }

  if (print_stats_count_ == 0) {
    // Find largest and smallest values, of all the values,
    // and highlight when display with all capital letters.
    smallest_ind_ = 0;
    largest_ind_ = 0;
    float smallest_value = 1.0;
    float largest_value = 0.0;
    for (int k = 0; k < NUM_NOTES; k++) {
      if (min_[k] < smallest_value) {
        smallest_value = min_[k];
        smallest_ind_ = k;
      }
      if (max_[k] > largest_value) {
        largest_value = max_[k];
        largest_ind_ = k;
      }
    }
  }

  // For each note, display statistics since last time.

  // Print one note each pass at the sample rate.
  // This keeps the serial print from taking too much of processor.
  // The check against < NUM_NOTES is not necessary but is an
  // extra safety to avoid overrunning arrays in case of a bug.
  if (print_stats_count_ >= 0 && print_stats_count_ < NUM_NOTES) {
    int k = print_stats_count_;  // Make code easier to read.
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
    if (k == smallest_ind_)
      Serial.print("  v  ");
    else
      Serial.print(" min=");
    if (min_[k] >= 0)
      Serial.print(" ");
    else
      Serial.print("-");
    Serial.print(abs(min_[k]));
    if (k == largest_ind_)
      Serial.print("  ^  ");
    else
      Serial.print(" max=");
    Serial.print(max_[k]);
    if (k%4 == 3)
      Serial.println();
    else
      Serial.print(" ");
    min_[k] = 1.0;
    max_[k] = 0.0;
    played_count_[k] = 0;
  }

  if (print_stats_count_ == NUM_NOTES - 1) {
    print_stats_count_ = -1;
    Serial.println();
  }
  else if (print_stats_count_ >= 0) {
    print_stats_count_++;
  }
}