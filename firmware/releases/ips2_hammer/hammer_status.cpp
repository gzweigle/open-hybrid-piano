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
int debug_level, float damper_threshold, float strike_threshold,
unsigned long serial_display_interval_micros) {

  debug_level_ = debug_level;

  dspp_ = dspp;
  testp_ = testp;
  damper_threshold_ = damper_threshold;
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

  serial_interval_micros_ = serial_display_interval_micros;
  serial_last_change_micros_ = micros();
  filter0_.Setup();
  filter1_.Setup();

}

// Control LED on front of board next to Teensy.
void HammerStatus::FrontLed(const float *calibrated_floats) {

  bool found;

  // Turn on LED if any key is above damper threshold.
  found = false;
  for (int k = 0; k < NUM_NOTES; k++) {
    if (calibrated_floats[k] > damper_threshold_) {
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
void HammerStatus::SerialMonitor(const int *adc, const float *position) {

  // Filter displayed data.
  unsigned int rs0, rs1;
  rs0 = filter0_.boxcarFilterUInts(adc[39]);
  rs1 = filter1_.boxcarFilterUInts(adc[53]);

  if (micros() - serial_last_change_micros_ > serial_interval_micros_) {
    if (debug_level_ >= 1) {
      Serial.print("Hammer Board ");
      Serial.print(" position=");
      Serial.print(rs0);
      Serial.print(" position=");
      Serial.println(rs1);
    }
    serial_last_change_micros_ = micros();
  }
}