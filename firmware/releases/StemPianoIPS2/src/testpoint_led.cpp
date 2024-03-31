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
// testpoint_led.cpp
//
// For ips pcb version 2.X
//
// Control test points and LEDs

#include "testpoint_led.h"

TestpointLed::TestpointLed() {}

void TestpointLed::Setup() {

  pin_d1_ = 4;
  pin_d2_ = 5;
  pin_d3_ = 8;
  pin_d4_ = 26;
  pin_tft_led_ = 32;
  pin_eth_led_ = 23;
  pin_sca_l_ = 21;
  pin_sca_r_ = 20;

  // These are set based on the board layout.

  // The four test points / LEDs.
  pinMode(pin_d4_, OUTPUT); digitalWrite(pin_d4_, LOW);
  pinMode(pin_d3_, OUTPUT); digitalWrite(pin_d3_, LOW);
  pinMode(pin_d2_, OUTPUT); digitalWrite(pin_d2_, LOW);
  pinMode(pin_d1_, OUTPUT); digitalWrite(pin_d1_, LOW);

  // Lower right LED, by TFT.
  pinMode(pin_tft_led_, OUTPUT); digitalWrite(pin_tft_led_, LOW);

  // Under SCA board.
  pinMode(pin_sca_l_, OUTPUT); digitalWrite(pin_sca_l_, LOW);
  pinMode(pin_sca_r_, OUTPUT); digitalWrite(pin_sca_r_, LOW);

  // Extra Ethernet LED.
  pinMode(pin_eth_led_, OUTPUT); digitalWrite(pin_eth_led_, LOW);

}

void TestpointLed::SetScaLedL(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_sca_l_, LOW);
  }
  else {
    digitalWrite(pin_sca_l_, HIGH);
  }
}
void TestpointLed::SetScaLedR(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_sca_r_, LOW);
  }
  else {
    digitalWrite(pin_sca_r_, HIGH);
  }
}

void TestpointLed::SetEthernetLED(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_eth_led_, LOW);
  }
  else {
    digitalWrite(pin_eth_led_, HIGH);
  }
}

void TestpointLed::SetLowerRightLED(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_tft_led_, LOW);
  }
  else {
    digitalWrite(pin_tft_led_, HIGH);
  }
}

void TestpointLed::SetTp11(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_d4_, LOW);
  }
  else {
    digitalWrite(pin_d4_, HIGH);
  }
}
void TestpointLed::SetTp10(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_d3_, LOW);
  }
  else {
    digitalWrite(pin_d3_, HIGH);
  }
}
void TestpointLed::SetTp9(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_d2_, LOW);
  }
  else {
    digitalWrite(pin_d2_, HIGH);
  }
}
void TestpointLed::SetTp8(bool set_value) {
  if (set_value == false) {
    digitalWrite(pin_d1_, LOW);
  }
  else {
    digitalWrite(pin_d1_, HIGH);
  }
}