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
// six_channel_analog_00.h
//
// For ips pcb version 2.X
// For sca pcb version 0.0

#ifndef SIX_CHANNEL_ANALOG_00_H_
#define SIX_CHANNEL_ANALOG_00_H_

#include <SPI.h>

#include "stem_piano_ips2.h"
#include "testpoint_led.h"

// Hardware dependent. Do not change.
// This is a #define to make clear
// the meaning of magic numbers in code.
#define NUM_16_CHANNEL_INPUTS 16
#define NUM_8_CHANNEL_INPUTS 6  // Only use 6 of the 8 8:1 mux channels.

#if (NUM_16_CHANNEL_INPUTS) * (NUM_8_CHANNEL_INPUTS) != NUM_CHANNELS
#error "ERROR - six_channel_analog_00.h number of channels is wrong".
#endif

// Hardware dependent. Do not change.
// Set per ADC t_conv value in datasheet.
// This is a #define to to make clear
// the meaning of magic numbers in code.
// Per ADC datasheet this must be at least 710ns.
#define ADC_CONVERSION_NANOSECONDS 710

class SixChannelAnalog00
{
  public:
    SixChannelAnalog00();
    void Setup(int, bool, bool, float, float, float, TestpointLed *);
    void GetNewAdcValues(unsigned int *, int);
    void NormalizeAdcValues(int *, float *, const unsigned int *);
    void ReorderAdcValues(unsigned int *);
 
  private:
    TestpointLed *Tpl_;
    int sclk_frequency_;
    bool adc_is_differential_;
    float adc_max_value_;
    int using18bitadc_;
    float sensor_v_max_;
    float adc_reference_;
    float adc_global_;
    // These arrays were an attempt to speed up the
    // code and make the overall analog-to-digital conversion
    // of all channels faster.  It didn't help the speed much.
    // But am keeping because it is easier to read the code.
    // Arrays match the mux connections in IPS 2.0 PCB schematic.
    const int mux16_0_[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    const int mux16_1_[16] = {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1};
    const int mux16_2_[16] = {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1};
    const int mux16_3_[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
    const int mux8_a_[6] = {0,1,0,1,1,1};
    const int mux8_b_[6] = {1,0,0,1,1,0};
    const int mux8_c_[6] = {0,0,0,0,1,1};

    // Keep track of pin names.
    int convst_;
    int din_;
    int mux8_1_pin_a_;
    int mux8_1_pin_b_;
    int mux8_1_pin_c_;
    int mux16_1_pin_s0_;
    int mux16_1_pin_s1_;
    int mux16_1_pin_s2_;
    int mux16_1_pin_s3_;

};

#endif
