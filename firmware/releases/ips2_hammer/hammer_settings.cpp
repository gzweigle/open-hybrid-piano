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
// hammer_settings.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Common location for all settings related to the system.
//
// WARNING - Board consistency.
// The following values must be identical on hammer and damper boards:
//    - adc_sample_period_microseconds.

#include "hammer_settings.h"

HammerSettings::HammerSettings() {}

void HammerSettings::SetAllSettingValues() {

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
  adc_global_scale = 0.5;

  ////////
  // Gain control.
  // The combination of piano action plus mechanical structure
  // built for a stem piano could result in a different velocity
  // minimum and maximum values per piano. Use this scaling factor
  // when converting velocity in meters per second to MIDI values.
  velocity_scale = 0.35;

  // Any MIDI velocity equal to this value or larger is muted.
  // Typically set to 127. But is user configurable. Assumption
  // is that normal piano playing won't use a volume of 127 but a
  // problem could. For example, a firmware or mechanical issue.
  // To disable, set >= max MIDI velocity (128 most likely).
  maximum_midi_velocity = 127;

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
  calibration_threshold = 0.5;

  ////////
  // Damper Settings.

  // When the damper position crosses this percentage of max-min
  // damper position, declare a damper event.
  damper_threshold = 0.5;

  // Magic value to get velocity in range [-1,0].
  damper_velocity_scaling = 0.025;

  ////////
  // Hammer Settings.

  // Select the algorithm for playing a piano note.
  // Algorithm 0:
  //    Detect hammer shank hitting hammer stop bar by velocity = 0.
  //    MIDI velocity = maximum velocity during hammer shank travel.
  //    Advantage: No thresholds. It is reliable even if mechanical
  //      system tolerances are large.
  //    Disadvantages: Longer delay, less accurate (due to shank oscillations).
  // Algorithm 1:
  //    Detect hammer shank hitting hammer stop bar by a position threshold.
  //    MIDI velocity = velocity at position threshold.
  //    Advantage: Less delay, more accurate (immune to shank oscillations).
  //    Disadvantage: Use of threshold makes it less reliable.
  //    Use this algorithm only after mechanical system is precisely setup.
  hammer_strike_algorithm = 0;

  if (hammer_strike_algorithm == 0) {
    // At this fraction of the position, start checking for a string hit.
    // Value does not need to be precise.
    // Just needs to be large enough to avoid noisy data when hammer
    // is near its rest or check position.
    strike_threshold = 0.8;
  }
  else {
    // When the hammer shank position crosses this threshold
    // declare that the hammer shank hit the hammer stop bar.
    // Ideally this position is at the letoff point.
    strike_threshold = 0.96;
  }

  // After a strike, do not allow another until the hammer has dropped
  // below this threshold. Set higher if signal is less noisy.
  // Set lower if signal is very noisy. Practically - set as high as
  // possible until get false piano sounds when holding the piano key down.
  // If set too low, lose repetition capability with hammer near string.
  // Disable functionality by setting >= 1.0.
  release_threshold = 0.90;

  // Not allowing a strike sooner than this many seconds since the last.
  // Needed because when the hammer shank hits the stop bar, the stop bar
  // can deform slightly. This can lead to a false multiple strike.
  min_repetition_seconds = 0.05;

  // Hammer must be at least this fast in order to generate a strike.
  // For a piano, a super slow motion hammer won't fly off the jack.
  // This setting creates the "undesirable" silent strike when
  // a pianist strikes a key for ppppp... too slowly.
  // Set to zero disables the functionality.
  min_strike_velocity = 0.15;

  // Swing distance at hammer. Measurement is in inches so
  // includes a conversion to meters (0.0254 meters / inch).
  hammer_travel_meters = .0254 * 1.75;

  ////////
  // Pedal Settings.

  // Run pedal sampling slower otherwise get noise around the threshold crossing.
  // Also, this reduces unnecessary processor burden because pedal processing
  // does not need to happen as quickly as for hammers.
  pedal_sample_interval_microseconds = 1000;
  pedal_threshold = 0.4;
  sustain_connected_pin = 95;
  sustain_pin = 94;
  sostenuto_connected_pin = 93;
  sostenuto_pin = 92;
  una_corda_connected_pin = 91;
  una_corda_pin = 90;

  ////////
  // MIDI
  midi_channel = 2;
  
  ////////
  // Ethernet data.
  //
  true_for_tcp_else_udp = true;
  MUST EDIT TO ADD VALUES BEFORE RUNNING.
  snprintf(teensy_ip, 16, "X.X.X.X");   // Arbitrary assigned Teensy IP
  snprintf(computer_ip,16, "X.X.X.X");  // Get from ipconfig command on local computer
  network_port = X;  // Must match UDP port in receiver code
  //

  ////////
  // Canbus.

  // Must be true if using separate remote board for measuring the
  // damper position.
  canbus_enable = false;

  ////////
  // TFT display.
  using_display = true;

  ////////
  // Set used inputs to true and unused inputs to false.
  // This is to avoid any loud notes due to noise on
  // unconnected inputs.
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    connected_channel[channel] = true;
  }

  // Turn off unused pedal inputs.
  for (int channel = 88; channel < 90; channel++) {
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