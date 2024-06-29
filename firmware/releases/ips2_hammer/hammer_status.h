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
// hammer_status.h
//
// For ips pcb version 2.X
// For sca pcb version 0.0

#ifndef HAMMER_STATUS_H_
#define HAMMER_STATUS_H_

#include "stem_piano_ips2.h"

#include "testpoint_led.h"
#include "dsp_pedal.h"
#include "utilities.h"

class HammerStatus
{
  public:
    HammerStatus();
    void Setup(DspPedal *, TestpointLed *, int, float, float, float);
    void FrontLed(const float *, bool, int);
    void LowerRightLed(bool, bool);
    void SCALed();
    void EthernetLed();
    void SerialMonitor(const int *, const float *, const bool *, bool, bool);
 
  private:

    int debug_level_;

    DspPedal *dspp_;
    TestpointLed *testp_;
    float damper_threshold_low_;
    float damper_threshold_high_;
    float strike_threshold_;

    unsigned long lower_r_led_interval_when_all_notes_calibrated_;
    unsigned long lower_r_led_last_change_;
    bool lower_r_led_state_;
    bool lower_r_led_in_nonvol_mode_;

    unsigned long sca_led_interval_;
    unsigned long sca_led_last_change_;
    bool sca_led_state_;

    unsigned long ethernet_led_interval_on_;
    unsigned long ethernet_led_interval_off_;
    unsigned long ethernet_led_last_change_;
    bool ethernet_led_state_;

    unsigned long serial_interval_;
    unsigned long serial_last_change_;
    Utilities filter0_;
    Utilities filter1_;

    unsigned long statistics_interval_;
    unsigned long statistics_last_change_;
    float min_[NUM_NOTES];
    float max_[NUM_NOTES];
    int played_count_[NUM_NOTES];

};

#endif
