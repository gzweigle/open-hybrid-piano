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
// damper_settings.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Common location for all settings related to the system.
//
// WARNING - Board consistency.
// The following values must be identical on hammer and damper boards:
//    - adc_sample_period_microseconds.

#include "damper_settings.h"

DamperSettings::DamperSettings() {}

void DamperSettings::SetAllSettingValues() {

  // Debug. See stem_piano_ips2.h for information.
  // DEBUG_NONE = Nothing displayed except startup info.
  // DEBUG_INFO = Occasional code state information.
  // DEBUG_NOTES = Above plus info about note changes.
  // DEBUG_ALG = Above plus algorithm details.
  // DEBUG_ALL = Above plus useless stuff.
  debug_level = DEBUG_INFO;
  Serial.print("Debug level is set to ");
  Serial.println(debug_level);

  // Avoid risk of anything bad happening on startup. Probably not needed.
  // Wait this number of ADC samples before allow piano sounds.
  startup_counter_value = 100;

  ////////
  // ADC settings.

  // Max is 70e6 per the ADC datasheet.
  // Set at the lowest value needed to get all data clocked
  // out in time to meet the adc_sample_period_microseconds value.
  adc_spi_clock_frequency = 60000000;

  // High sample rate data acquisition mode.
  // If >= 0, all piano functions are disabled except test_index channel. 
  // Because all functions are disabled, it is possible to set the sample
  // rate approximately 10x faster than during normal operation.
  test_index = -1;
  
  // Must be longer than the time to sample and collect all NUM_CHANNELS
  // data from the ADC plus the time for processing all of the data.
  // WARNING - For normal operation, this value must match damper board value.
  adc_sample_period_microseconds = 250;
  
  if (debug_level >= DEBUG_INFO) {
    Serial.print("The sample period is set to ");
    Serial.print(adc_sample_period_microseconds);
    Serial.println(" microseconds.");
    Serial.println("The sample period value must match on hammer and damper boards.");
  }

  // When the TFT is running, slow everything down because the TFT
  // processing takes a long time.
  adc_sample_period_microseconds_during_tft = 100000;

  // The analog circuitry in front of ADC is not differential.
  // Therefore, if using a differential ADC, lose a bit.
  adc_is_differential = true;

  // Presently support a 16-bit or 18-bit ADC.
  using18bitadc = false;

  // Historically, ADC changes during development resulted in
  // 1.2V, 2.5V, 3.0V, and 3.3V for adc_reference. 
  // Also, ADC changes during development resulted in using
  // 1.2V and 3.3V for sensor_v_max. Therefore, the sensor_v_max
  // and adc_reference setttings are useful during development to
  // support connecting older and newer ADC systems together.

  // The maximum voltage output by the sensor when hammer is at highest position.
  sensor_v_max = 2.5;

  // Reference value on ADC. When the output of the sensor is at this voltage,
  // the ADC outputs its maximum value.
  adc_reference = 2.5;

  // A global scaling applied to all ADC inputs.
  // Normally set to 1.0.
  adc_global_scale = 1.0;

  ////////
  // Switch settings.
  switch_debounce_micro = 500000; // Read DIP switches at this interval, microseconds.
  switch11_ips_pin = 3;
  switch12_ips_pin = 22;
  switch21_ips_pin = 0;
  switch22_ips_pin = 2;
  switch11_sca_pin = 19;
  switch12_sca_pin = 18;
  switch21_sca_pin = 17;
  switch22_sca_pin = 16;

  ////////
  // Calibration Settings.
  calibration_threshold = 0.6;

  
  ////////
  // Ethernet data.
  //
  MUST EDIT TO ADD VALUES BEFORE RUNNING.
  true_for_tcp_else_udp = true;
  snprintf(teensy_ip, 16, "X.X.X.X");   // Arbitrary assigned Teensy IP
  snprintf(computer_ip,16, "X.X.X.X");  // Get from ipconfig command on local computer
  // Recommend different UDP port for hammer and damper boards.
  network_port = X;  // Must match UDP port in receiver code
  //
  ////////

  ////////
  // Canbus.

  // Must be true if using separate remote board for measuring the
  // damper position.
  canbus_enable = true;

  ////////
  // TFT display.
  using_display = true;

  ////////
  // Set used inputs to true and unused inputs to false.
  // This is to avoid any loud notes due to noise on
  // unconnected inputs.
  for (int channel = 0; channel < NUM_NOTES; channel++) {
    connected_channel[channel] = true;
  }

  // Turn off unused pedal inputs.
  for (int channel = NUM_NOTES; channel < NUM_CHANNELS; channel++) {
    connected_channel[channel] = false;
  }

  ////////
  // Optional custom reordering.
  // To customize reordering, place the new source index into reorder_list.
  //
  // For example, assume that a sensor at C4 is connected to the lowest
  // input on IPS 2.0. When starting to number with index 0, C4 is index 39.
  // So, set reorder_list[39] = 0.
  // Without reordering, the lowest input on IPS 2.0 is for B0.
  //
  // If the sensor at C4 is connected to the 34th input on IPS 2.0, then set
  // reorder_list[39] = 34.
  // Without reordering, the 34th input on IPS 2.0 is for G3.
  //
  // Use the assembly_manual.md document for IPS 2.0 numbers.
  //
  // The default is no custom reordering.
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    reorder_list[channel] = channel;
  }
}