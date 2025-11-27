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
// six_channel_analog_00.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Converters:
// ADS8860 (16-bit, 1Msps, single-ended input), Datasheet February 2019
// ADS8861 (16-bit, 1Msps, differential input), Datasheet December 2013
//
// Will also work with:
// ADS8881 (18-bit, 1Msps, differential input), Datasheet August 2015
//
// If using ADS8861 or ADS8881, then adc_is_differential must be true.

#include "six_channel_analog_00.h"

SixChannelAnalog00::SixChannelAnalog00() {}

void SixChannelAnalog00::Setup(int sclk_frequency, bool adc_is_differential,
bool using18bitadc, float sensor_v_max, float adc_reference, 
float adc_global, const int *reorder_list, TestpointLed *Tpl) {

  sclk_frequency_ = sclk_frequency;
  adc_is_differential_ = adc_is_differential;
  sensor_v_max_ = sensor_v_max;
  adc_reference_ = adc_reference;
  adc_global_ = adc_global;
  Tpl_ = Tpl;
  using18bitadc_ = using18bitadc;

  if (using18bitadc_ == true) {
    adc_max_value_ = 262143.0;
  }
  else {
    adc_max_value_ = 65535.0;
  }

  // Custom reordering.
  for (int ind = 0; ind < NUM_CHANNELS; ind++) {
    reorder_list_[ind] = reorder_list[ind];
  }

  // Pin numbers are set by board layout.
  convst_ = 14;
  din_ = 11;
  mux8_1_pin_a_ = 39;
  mux8_1_pin_b_ = 40;
  mux8_1_pin_c_ = 41;
  mux16_1_pin_s0_ = 38;
  mux16_1_pin_s1_ = 37;
  mux16_1_pin_s2_ = 36;
  mux16_1_pin_s3_ = 35;

  // Convert start signal.
  pinMode(convst_, OUTPUT);
  digitalWrite(convst_, LOW);
  // DIN pin, hold permanently high.
  pinMode(din_, OUTPUT);
  digitalWrite(din_, HIGH);

  // 8:1 analog multiplexer.
  pinMode(mux8_1_pin_c_, OUTPUT);
  pinMode(mux8_1_pin_b_, OUTPUT);
  pinMode(mux8_1_pin_a_, OUTPUT);
  // 16:1 analog multiplexer.
  pinMode(mux16_1_pin_s0_, OUTPUT);
  pinMode(mux16_1_pin_s1_, OUTPUT);
  pinMode(mux16_1_pin_s2_, OUTPUT);
  pinMode(mux16_1_pin_s3_, OUTPUT);

  // Initialize to 0 input as when hit loop in GetNewAdcValues()
  // it will immediately begin conversion on the muxed input.
  // So mux must initialize to pass the channel 0 analog through.
  digitalWrite(mux8_1_pin_c_, mux8_c_[0]);
  digitalWrite(mux8_1_pin_b_, mux8_b_[0]);
  digitalWrite(mux8_1_pin_a_, mux8_a_[0]);
  digitalWrite(mux16_1_pin_s0_, mux16_0_[0]);
  digitalWrite(mux16_1_pin_s1_, mux16_1_[0]);
  digitalWrite(mux16_1_pin_s2_, mux16_2_[0]);
  digitalWrite(mux16_1_pin_s3_, mux16_3_[0]);

  SPI.begin();

}

// Get data from ADC and put in adc_array[].
// The code is piplined, which gives extra settling time for
// ADC input while the ADC is clocking out the previous value.
// This pipelining speeds up the overall conversion time.
void SixChannelAnalog00::GetNewAdcValues(unsigned int *adc_array, int test_index) {
  // ADC clocks data out on the falling edge. Therefore, using SPI Mode 1.
  SPI.beginTransaction(SPISettings(sclk_frequency_, MSBFIRST, SPI_MODE1));

  if (test_index < 0) {
    int adc_ind = 0;
    int mux8_next, mux16_next;
    for (int mux8 = 0; mux8 < NUM_8_CHANNEL_INPUTS; mux8++) {
      for (int mux16 = 0; mux16 < NUM_16_CHANNEL_INPUTS; mux16++) {

        // Sample the analog inputs at ADC.
        // First ADC sampling is for [mux8,mux16] = [0,0].
        // They were left at [0,0] the last time SixChannelAnalog00 was run.
        digitalWriteFast(convst_, HIGH);

        // Pipelined, so calculate next mux channel.
        mux16_next = mux16 + 1;
        if (mux16_next == NUM_16_CHANNEL_INPUTS) {
          mux16_next = 0;
          mux8_next = mux8 + 1;
          if (mux8_next == NUM_8_CHANNEL_INPUTS) {
            mux8_next = 0;
          }
        }
        else {
          mux8_next = mux8;
        }

        // Test point so can trigger oscilloscope exactly when muxes switch.
        // The specific mux value it triggers on is arbitrary.
        // Use this test point to help validate signal settling time.
        //if ( (mux16_next == 13 && mux8_next == 3)) {
        //  Tpl_->SetTp11(true);
        //}
        //else {
        //  Tpl_->SetTp11(false);
        //}

        // Switch all muxes to next analog input channel.
        // Switch 16:1 first to give juuuust a little extra
        // settling time, as ADC has to wait for 8:1 in any case.
        // From the time of these mux changes until convst_ goes high
        // must be long enough to allow the signal to settle under
        // worst case conditions. Oscilloscope testing was used to
        // verify that it is ok.
        digitalWriteFast(mux16_1_pin_s0_, mux16_0_[mux16_next]);
        digitalWriteFast(mux16_1_pin_s1_, mux16_1_[mux16_next]);
        digitalWriteFast(mux16_1_pin_s2_, mux16_2_[mux16_next]);
        digitalWriteFast(mux16_1_pin_s3_, mux16_3_[mux16_next]);
        digitalWriteFast(mux8_1_pin_a_, mux8_a_[mux8_next]);
        digitalWriteFast(mux8_1_pin_b_, mux8_b_[mux8_next]);
        digitalWriteFast(mux8_1_pin_c_, mux8_c_[mux8_next]);

        // Total time from convst_ HIGH to LOW must be at least
        // ADC_CONVERSION_NANOSECONDS. The switching above takes some
        // time so waiting the full ADC_CONVERSION_NANOSECONDS here
        // is very conservative, which is good.
        delayNanoseconds(ADC_CONVERSION_NANOSECONDS);
        digitalWriteFast(convst_, LOW);   // End a conversion.

        // Now, get the data from the conversion initiated above.
        // Conversion is for [mux16,mux8] values not
        // [mux16_next, mux8_next].
        // Write 0xFF so that DIN stays high.
        if (using18bitadc_ == true) {
          // Not getting two lower bits. Force them to zero.
          adc_array[adc_ind++] = (SPI.transfer16(0xFF) << 2);
        }
        else {
          adc_array[adc_ind++] = SPI.transfer16(0xFF);
        }

        // Make sure DIN stays high. Probably not necessary.
        digitalWrite(din_, HIGH);
      }
    }
  }
  else {
    // Sample one channel. Use for very high speed sampling testing.
    // Code replicates normal code because don't want extra if()
    // checks in high-speed, normal operation, code above.
    int mux8 = test_index >> 4;
    int mux16 = test_index & 0x0F;
    digitalWriteFast(convst_, HIGH);
    digitalWriteFast(mux16_1_pin_s0_, mux16_0_[mux16]);
    digitalWriteFast(mux16_1_pin_s1_, mux16_1_[mux16]);
    digitalWriteFast(mux16_1_pin_s2_, mux16_2_[mux16]);
    digitalWriteFast(mux16_1_pin_s3_, mux16_3_[mux16]);
    digitalWriteFast(mux8_1_pin_a_, mux8_a_[mux8]);
    digitalWriteFast(mux8_1_pin_b_, mux8_b_[mux8]);
    digitalWriteFast(mux8_1_pin_c_, mux8_c_[mux8]);
    delayNanoseconds(ADC_CONVERSION_NANOSECONDS);
    digitalWriteFast(convst_, LOW);
    if (using18bitadc_ == true) {
      adc_array[test_index] = (SPI.transfer16(0xFF) << 2);
    }
    else {
      adc_array[test_index] = SPI.transfer16(0xFF);
    }
    digitalWrite(din_, HIGH);
  }
  SPI.endTransaction();
}

// diff_scale_raw: adjust if differential but otherwise no changes.
// normalized_float: scale to [0.0,1.0], where 1.0 is the max ADC value.
void SixChannelAnalog00::NormalizeAdcValues(int *diff_scale_raw,
float *normalized_float, const unsigned int *adc_values) {

  float scale = adc_reference_ / sensor_v_max_;

  for (int ind = 0; ind < NUM_CHANNELS; ind++) {

    normalized_float[ind] = static_cast<float>(adc_values[ind]) / adc_max_value_;

    // Normally adc_reference_ == sensor_v_max_.
    // Allow other values in case mixing and matching front ends.
    normalized_float[ind] *= scale;

    // Global scaling.
    normalized_float[ind] *= adc_global_;

    diff_scale_raw[ind] = adc_values[ind];

    // Multiply by 2 because the SCA version 0.0 is single-ended and so if a
    // differential ADC is on the board, half of the dynamic range is lost.
    if (adc_is_differential_ == true) {
      normalized_float[ind] *= 2.0;
      diff_scale_raw[ind] *= 2;
    }

  }
}

// Connections to back row of 16:1 multiplexers is reversed
// on PCB compared to the piano key order. Fix that here.
void SixChannelAnalog00::ReorderAdcValues(unsigned int *data,
const unsigned int *data_in) {
  // Swap the back row.
  for (int grp = 0; grp < NUM_CHANNELS; grp += 16) {
    for (int ind = 0; ind < 4; ind++) {
      data[ind + grp] = data_in[7-ind + grp];
      data[7-ind + grp] = data_in[ind + grp];
    }
  }
  // Keep the front row as-is.
  for (int grp = 8; grp < NUM_CHANNELS; grp += 16) {
    for (int ind = 0; ind < 8; ind++) {
      data[ind + grp] = data_in[ind + grp];
    }
  }
  // Custom reordering.
  unsigned int data_tmp[NUM_CHANNELS];
  for (int ind = 0; ind < NUM_CHANNELS; ind++) {
    data_tmp[ind] = data[ind];
  }
  for (int ind = 0; ind < NUM_CHANNELS; ind++) {
    data[ind] = data_tmp[reorder_list_[ind]];
  }
}