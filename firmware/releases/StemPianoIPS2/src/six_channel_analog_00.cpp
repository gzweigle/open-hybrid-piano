// Copyright (C) 2023 Greg C. Zweigle
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
// six_channel_analog_00.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// See Texas Instruments ADS886X 16-bit, 1Msps, successive
// approximation, with internal track-and-hold datasheet.
//   X = 0 is single-ended ADC.
//   X = 1 is differential ADC.
//
// If using ADS8861, then adc_is_differential must be true.

#include "six_channel_analog_00.h"

SixChannelAnalog00::SixChannelAnalog00() {}

void SixChannelAnalog00::Setup(int sclk_frequency, bool adc_is_differential,
int adc_extra_settling_time_nanoseconds, TestpointLed *Tpl) {

  sclk_frequency_ = sclk_frequency;
  adc_is_differential_ = adc_is_differential;
  adc_extra_settling_time_nanoseconds_ = adc_extra_settling_time_nanoseconds;
  Tpl_ = Tpl;

  // 16-bit ADC.
  adc_max_value_ = 65535;

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
void SixChannelAnalog00::GetNewAdcValues(unsigned int *adc_array) {
  // ADC clocks data out on the falling edge. Therefore, using SPI Mode 1.
  SPI.beginTransaction(SPISettings(sclk_frequency_, MSBFIRST, SPI_MODE1));
  int adc_ind = 0;
  int mux8_next, mux16_next;

  for (int mux8 = 0; mux8 < NUM_8_CHANNEL_INPUTS; mux8++) {
    for (int mux16 = 0; mux16 < NUM_16_CHANNEL_INPUTS; mux16++) {

      delayNanoseconds(adc_extra_settling_time_nanoseconds_);
      digitalWriteFast(convst_, HIGH);  // Sample the analog inputs at ADC.
      delayNanoseconds(ADC_CONVERSION_NANOSECONDS);
      digitalWriteFast(convst_, LOW);   // End a conversion.

      // Pipelined, so calculate next mux channel.
      mux16_next = mux16+1;
      if (mux16_next == NUM_16_CHANNEL_INPUTS) {
        mux16_next = 0;
        mux8_next = mux8+1;
        if (mux8_next == NUM_8_CHANNEL_INPUTS) {
          mux8_next = 0;
        }
      }
      else {
        mux8_next = mux8;
      }

      // Test point so can trigger oscilloscope exactly when muxes switch.
      // The specific mux value it triggers on is arbitrary.
      if ( (mux16_next == 8 && mux8_next == 1) ||
      (mux16_next == 9 && mux8_next == 1)) {
        Tpl_->SetTp9(true);
      }

      // Switch all muxes to next analog input channel.
      // Switch 16:1 first to give juuuust a little extra
      // settling time, as ADC has to wait for 8:1 in any case.
      digitalWriteFast(mux16_1_pin_s0_, mux16_0_[mux16_next]);
      digitalWriteFast(mux16_1_pin_s1_, mux16_1_[mux16_next]);
      digitalWriteFast(mux16_1_pin_s2_, mux16_2_[mux16_next]);
      digitalWriteFast(mux16_1_pin_s3_, mux16_3_[mux16_next]);
      digitalWriteFast(mux8_1_pin_a_, mux8_a_[mux8_next]);
      digitalWriteFast(mux8_1_pin_b_, mux8_b_[mux8_next]);
      digitalWriteFast(mux8_1_pin_c_, mux8_c_[mux8_next]);
      Tpl_->SetTp9(false);

      // Now, get the data from the conversion initiated above.
      // Conversion is for the analog channel prior to the one just switched to.
      // Write 0xFF so that DIN stays high.
      adc_array[adc_ind++] = SPI.transfer16(0xFF);

      // Make sure DIN stays high. Probably this is not necessary.
      digitalWrite(din_, HIGH);
    }
  }
  SPI.endTransaction();
}

// Normalize to [0.0,1.0], where 1.0 is the max ADC value.
void SixChannelAnalog00::NormalizeAdcValues(int *normalized_raw,
float *normalized_float, const unsigned int *adc_values) {

  for (int ind = 0; ind < NUM_CHANNELS; ind++) {

    normalized_float[ind] = static_cast<float>(adc_values[ind]) /
    static_cast<float>(adc_max_value_);

    normalized_raw[ind] = adc_values[ind];

    // Multiply by 2 because the hardware is single-ended and so if a
    // differential ADC is on the board, half of the dynamic range is lost.
    if (adc_is_differential_ == true) {
      normalized_float[ind] *= 2.0;
      normalized_raw[ind] *= 2;
    }
  }
}