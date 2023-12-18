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
//
// TODO - Make selecting damper_velocity_scaling value based on analysis.

#include "damper_settings.h"

DamperSettings::DamperSettings() {}

void DamperSettings::SetAllSettingValues() {


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
    Serial.println(" in damper_settings.cpp");
    Serial.println("Extra debug information will be printed to the serial port");
  }
  else if (debug_level == 1) {
    Serial.println("Warning - debug_level is set at 1 in damper_settings.cpp");
    Serial.println("Some debug information will be printed to the serial port");
  }

  if (board_bringup == true) {
    Serial.println("Warning - board_bringup in damper_settings.cpp is defined.");
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
  adc_sample_period_microseconds = 400;

  if (debug_level >= 1) {
    Serial.print("The sample period is set to ");
    Serial.print(adc_sample_period_microseconds);
    Serial.println(" microseconds.");
    Serial.println("The sample period value must match on hammer and damper boards.");
  }

  // When the TFT is running, slow things down alot!
  adc_sample_period_microseconds_during_tft = 100000;

  // Must be larger than the settling time of the 16:1 mux, 8:1 mux plus
  // the ADC SAR input settling time.
  // The value is based on analysis plus experimentally checked.
  // Make shorter will decrease accuracy as the ADC input has less time to settle.
  // However, making shorter allows for a faster sample rate.
  adc_extra_settling_time_nanoseconds = 700;

  // The analog circuitry in front of ADC is not differential.
  // Therefore, if using a differential ADC, lose a bit.
  adc_is_differential = true;

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
  damper_threshold = 0.6;

  // Magic value to get velocity in range [0,1].
  damper_velocity_scaling = 0.025;

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
    canbus_enable = false;
  }
  else {
    canbus_enable = true;
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
      if (channel >= 88)
        connected_channel[channel] = false;  // No pedals for damper board.
      else
        connected_channel[channel] = true;
    }
  }

  // The ADC sample library runs left to right across the board.
  // The damper keys are connected right to left.
  // Instead of modifying the library, swap after sampling.
  // This also re-orders the front row.

  // n = 80 + n, n=0-15
  note_order[0] = 80;
  note_order[1] = 81;
  note_order[2] = 82;
  note_order[3] = 83;
  note_order[4] = 84;
  note_order[5] = 85;
  note_order[6] = 86;
  note_order[7] = 87;

  // 80 + (7 - n), n=0-15
  note_order[8] = 79;
  note_order[9] = 78;
  note_order[10] = 77;
  note_order[11] = 76;
  note_order[12] = 75;
  note_order[13] = 74;
  note_order[14] = 73;
  note_order[15] = 72;

  note_order[16] = 64;
  note_order[17] = 65;
  note_order[18] = 66;
  note_order[19] = 67;
  note_order[20] = 68;
  note_order[21] = 69;
  note_order[22] = 70;
  note_order[23] = 71;

  note_order[24] = 63;
  note_order[25] = 62;
  note_order[26] = 61;
  note_order[27] = 60;
  note_order[28] = 59;
  note_order[29] = 58;
  note_order[30] = 57;
  note_order[31] = 56;

  note_order[32] = 48;
  note_order[33] = 49;
  note_order[34] = 50;
  note_order[35] = 51;
  note_order[36] = 52;
  note_order[37] = 53;
  note_order[38] = 54;
  note_order[39] = 55;

  note_order[40] = 47;
  note_order[41] = 46;
  note_order[42] = 45;
  note_order[43] = 44;
  note_order[44] = 43;
  note_order[45] = 42;
  note_order[46] = 41;
  note_order[47] = 40;

  note_order[48] = 32;
  note_order[49] = 33;
  note_order[50] = 34;
  note_order[51] = 35;
  note_order[52] = 36;
  note_order[53] = 37;
  note_order[54] = 38;
  note_order[55] = 39;

  note_order[56] = 31;
  note_order[57] = 30;
  note_order[58] = 29;
  note_order[59] = 28;
  note_order[60] = 27;
  note_order[61] = 26;
  note_order[62] = 25;
  note_order[63] = 24;

  note_order[64] = 16;
  note_order[65] = 17;
  note_order[66] = 18;
  note_order[67] = 19;
  note_order[68] = 20;
  note_order[69] = 21;
  note_order[70] = 22;
  note_order[71] = 23;

  note_order[72] = 15;
  note_order[73] = 14;
  note_order[74] = 13;
  note_order[75] = 12;
  note_order[76] = 11;
  note_order[77] = 10;
  note_order[78] = 9;
  note_order[79] = 8;

  note_order[80] = 7;
  note_order[81] = 6;
  note_order[82] = 5;
  note_order[83] = 4;
  note_order[84] = 3;
  note_order[85] = 2;
  note_order[86] = 1;
  note_order[87] = 0;

  note_order[88] = 88;
  note_order[89] = 89;
  note_order[90] = 90;
  note_order[91] = 91;
  note_order[92] = 92;
  note_order[93] = 93;
  note_order[94] = 94;
  note_order[95] = 95;

}