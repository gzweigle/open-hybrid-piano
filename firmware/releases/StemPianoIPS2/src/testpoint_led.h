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
// testpoint_led.h
//
// For ips pcb version 2.X
//
// Control test points and LEDs

#ifndef TESTPOINT_LED_H_
#define TESTPOINT_LED_H_

#include "stem_piano_ips2.h"

class TestpointLed
{
  public:
    TestpointLed();
    void Setup();
    void SetEthernetLED(bool);
    void SetLowerRightLED(bool);
    void SetScaLedL(bool);
    void SetScaLedR(bool);
    void SetTp11(bool);
    void SetTp10(bool);
    void SetTp9(bool);
    void SetTp8(bool);

  private:
    int pin_d1_;
    int pin_d2_;
    int pin_d3_;
    int pin_d4_;
    int pin_tft_led_;
    int pin_eth_led_;
    int pin_sca_l_;
    int pin_sca_r_;
 
};

#endif