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
// hammer_settings.h
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Common location for all settings related to the system.

#ifndef HAMMER_SETTINGS_H_
#define HAMMER_SETTINGS_H_

#include "stem_piano_ips2.h"

#define IP_STRING_LENGTH 128

class HammerSettings
{
  
  public:

    HammerSettings();
    void SetAllSettingValues();
    int debug_level;
    int startup_counter_value;
    int adc_spi_clock_frequency;
    int adc_sample_period_microseconds;
    int adc_sample_period_microseconds_during_tft;
    bool adc_is_differential;
    bool using18bitadc;
    float sensor_v_max;
    float adc_reference;
    float velocity_scale;
    int switch_debounce_micro;
    int switch11_ips_pin;
    int switch12_ips_pin;
    int switch21_ips_pin;
    int switch22_ips_pin;
    int switch11_sca_pin;
    int switch12_sca_pin;
    int switch21_sca_pin;
    int switch22_sca_pin;
    float calibration_threshold;
    float calibration_match_gain;
    float calibration_match_offset;
    float damper_threshold_low;
    float damper_threshold_high;
    float damper_velocity_scaling;
    float strike_threshold;
    float release_threshold;
    float min_repetition_seconds;
    float min_strike_velocity;
    float hammer_travel_meters;
    int pedal_sample_interval_microseconds;
    float pedal_threshold;
    int sustain_pin;
    int sustain_connected_pin;
    int sostenuto_pin;
    int sostenuto_connected_pin;
    int una_corda_pin;
    int una_corda_connected_pin;
    int midi_channel;
    char teensy_ip[IP_STRING_LENGTH];
    char computer_ip[IP_STRING_LENGTH];
    int upd_port;
    int ethernet_start_ind;
    int ethernet_end_ind;
    bool canbus_enable;
    bool using_display;
    bool connected_channel[NUM_CHANNELS];
};

#endif