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
//
// TODO - Make selecting damper_velocity_scaling value based on analysis.
// TODO - Change initialize_hammer_wait_count to based on seconds, not counts.

#include "hammer_settings.h"

HammerSettings::HammerSettings() {}

void HammerSettings::SetAllSettingValues() {

  ////////
  // Debug
  // Set this to zero during normal operation.
  // debug_level 0 = No debug information on serial port.
  //                 Will print whether a hammer or damper board.
  // debug_level 1 = Small amount of debug information.
  //                 Errors and warnings.
  //                 MIDI send notification.
  //                 Change of switch state.
  // debug_level 2 = Large amount of debug information.
  //                 Details internal to the algorithms.
  debug_level = 1;

  // Set to true following documentation for bringing up the board.
  board_bringup = false;

  if (debug_level >= 2) {
    Serial.print("Warning - debug_level is ");
    Serial.print(debug_level);
    Serial.println(" in hammer_settings.cpp");
    Serial.println("Extra debug information will be printed to the serial port");
  }
  else if (debug_level == 1) {
    Serial.println("Warning - debug_level is set at 1 in hammer_settings.cpp");
    Serial.println("Some debug information will be printed to the serial port");
  }

  if (board_bringup == true) {
    Serial.println("Warning - board_bringup in hammer_settings.cpp is defined.");
    Serial.println("Using settings for testing minimal board functionality.");
    Serial.println("See online documentation for more details.");
  }

  ////////
  // ADC settings.

  // Max 70e6 per ADC datasheet.
  adc_spi_clock_frequency = 60000000;
  
  // Must be longer than the time to sample and collect all NUM_CHANNELS
  // data from the ADC plus the time for processing all of the data.
  // WARNING - This value must match the value on damper board.
  adc_sample_period_microseconds = 300;
  
  if (debug_level >= 1) {
    Serial.print("The sample period is set to ");
    Serial.print(adc_sample_period_microseconds);
    Serial.println(" microseconds.");
    Serial.println("The sample period value must match on hammer and damper boards.");
  }

  // When the TFT is running, slow things down alot!
  adc_sample_period_microseconds_during_tft = 100000;

  // The analog circuitry in front of ADC is not differential.
  // Therefore, if using a differential ADC, lose a bit.
  adc_is_differential = true;

  // If a hammer value exceeds this threshold, then the value is used to track the
  // maximum hammer position for scaling. Use this threshold to simplify the scaling
  // algorithm so that it does not scale the values when hammer is at rest
  // Must be larger than 0.0.
  adjust_gain = true;
  adc_scale_threshold = 0.2;

  ////////
  // Noncritical values for display and LED.
  serial_baud_rate = 9600;
  serial_display_interval = 1000000;  // Number of microseconds.

  ////////
  // DIP switch settings.
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
  // Damper Settings.

  // When the damper position crosses this percentage of max-min
  // damper position, declare a damper event.
  damper_threshold_using_damper = 0.35;
  damper_threshold_using_hammer = 0.6;

  // Magic value to get velocity in range [0,1].
  damper_velocity_scaling = 0.025;

  // Set to true if using hammer as an estimate of damper.
  // For keys with this set as true, using the damper_threshold_using_hammer.
  // For keys with this set as false, using the damper_threshold_using_damper.
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    // For the present setup, only keys in the middle use the damper sensors.
    if (channel >= 32 && channel < 54)
      using_hammer_to_estimate_damper[channel] = false;
    else
      using_hammer_to_estimate_damper[channel] = true;
  }

  ////////
  // Hammer Settings.

  // At this fraction of the position, start checking for a string hit.
  // Value does not need to be precise.
  // Just needs to be large enough to avoid checking when the hammer
  // is near its rest position and could have noisy data.
  strike_threshold = 0.7;

  // After a strike, do not allow another until the hammer has dropped
  // below this threshold.
  // Set higher if signal is less noisy.
  // Set lower if signal is very noisy.
  // Practically - set as high as possible until get false piano sounds
  // when holding the piano key down.
  // If set too low, lose repetition capability with hammer near string.
  // Disable functionality by setting >= 1.0.
  release_threshold = 0.65;

  // Not allowing a strike sooner than this many seconds since the last.
  // Needed because when the hammer shank hits the stop bar, the stop bar
  // can deform slightly. This can lead to a false multiple strike.
  min_repetition_seconds = 0.05;

  // Hammer must be at least this fast in order to generate a strike.
  // For a piano, a super slow motion hammer won't fly off the jack.
  // This setting creates the "undesirable" silent strike when
  // a pianist strikes a key for ppppp... too slowly.
  // Set to zero disables the functionality.
  min_strike_velocity = 0.05;

  // Swing distance at CNY70 location in meters.
  // Measure this after building the mechanical system and structure
  // that holds the sensors. The measurement is in inches so includes
  // a conversion to meters (0.0254 meters / inch)
  hammer_travel_meters = .0254 * 0.3125; 

  // Avoid risk of anything bad happening on startup.
  // Probably not needed.
  initialize_hammer_wait_count = 1000;

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
  MUST EDIT TO ADD VALUES BEFORE RUNNING.
  snprintf(teensy_ip, 16, "X.X.X.X");   // Arbitrary assigned Teensy IP
  snprintf(computer_ip,16, "X.X.X.X");  // Get from ipconfig command on local computer
  upd_port = X;  // Must match UDP port in receiver code
  //

  ////////
  // Canbus.

  // Set to true if using separate remote board for measuring the
  // damper position. If this is set to false, then the damper
  // position will be estimated based on the hammer position.
  if (board_bringup == true) {
    // Bringing up the board mode is for a single hammer board. No damper.
    connected_to_remote_damper_board = false;
    canbus_enable = false;
  }
  else {
    connected_to_remote_damper_board = true;
    canbus_enable = true;
  }

  if (debug_level >= 1) {
    if (canbus_enable == false && connected_to_remote_damper_board == true) {
      Serial.println("Warning - trying to use remote board without Can bus enabled.");
    }
  }

  ////////
  // TFT display.
  using_display = true;

  ////////
  // Set used and unused channels
  // This is to avoid any loud notes due to noise on
  // unconnected inputs.
  if (board_bringup == true) {
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
      connected_channel[channel] = false;
    }
    connected_channel[24] = true;
    connected_channel[25] = true;
  }
  else {
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
      connected_channel[channel] = true;
    }
  }

  // Turn off unused pedal inputs.
  for (int channel = 88; channel < 90; channel++) {
    connected_channel[channel] = false;
  }

}