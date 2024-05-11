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
// src_hammer.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Main code for hammer board of stem piano.
//
// stem piano is an open source hybrid digital piano.
//

#include "stem_piano_ips2.h"

#include "hammer_settings.h"
#include "six_channel_analog_00.h"
#include "auto_mute.h"
#include "board2board.h"
#include "calibration_sensor.h"
#include "dsp_damper.h"
#include "dsp_hammer.h"
#include "dsp_pedal.h"
#include "gain_control.h"
#include "hammer_status.h"
#include "midiout.h"
#include "network.h"
#include "nonvolatile.h"
#include "switches.h"
#include "testpoint_led.h"
#include "timing.h"
#include "tft_display.h"

HammerSettings Set;
SixChannelAnalog00 Adc;
AutoMute Mute;
Board2Board B2B;
CalibrationSensor Cal;
DspDamper DspD;
DspHammer DspH;
DspPedal DspP;
GainControl Gain;
HammerStatus HStat;
MidiOut Midi;
Network Eth;
Nonvolatile Nonv;
Switches SwIPS1;
Switches SwIPS2;
Switches SwSCA1;
Switches SwSCA2;
TestpointLed Tpl;
Timing Tmg;
TftDisplay Tft;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, mi);

void setup(void) {

  // Serial port setup.
  Serial.begin(9600);
  Serial.println(".");
  Serial.println(".");
  Serial.println("welcome to stem piano by gcz");
  Serial.println(".");
  Serial.println(".");

  // Important information.
  Serial.println("Hardware required:");
  Serial.println("  IPS2.X hammer board.");
  Serial.println("  SCA0.0 analog board.");
  Serial.println("  HPS 0.8, 0.7, or 0.4 sensor board (any board meeting spec is also ok).");
  Serial.println("License: GNU GPLv3 - for documentation, code, and design see:");
  Serial.println("  https://github.com/gzweigle/DIY-Grand-Digital-Piano");
  Serial.println(".");
  Serial.println(".");

  // Load the settings. Must be first in the setup() function.
  Set.SetAllSettingValues();

  if (Set.debug_level >= DEBUG_STATS) {
    Serial.println("Beginning hammer board initialization.");
  }

  // Initialize the nonvolatile memory.
  // Initialize early in case any setup() uses storage.
  Nonv.Setup(Set.debug_level);

  // Setup reading the switches.
  SwIPS1.Setup(Set.switch_debounce_micro,
  Set.switch11_ips_pin, Set.switch12_ips_pin, Set.debug_level);
  SwIPS2.Setup(Set.switch_debounce_micro,
  Set.switch21_ips_pin, Set.switch22_ips_pin, Set.debug_level);
  SwSCA1.Setup(Set.switch_debounce_micro,
  Set.switch11_sca_pin, Set.switch12_sca_pin, Set.debug_level);
  SwSCA2.Setup(Set.switch_debounce_micro,
  Set.switch21_sca_pin, Set.switch22_sca_pin, Set.debug_level);

  Mute.Setup();

  // Setup the analog front-end and the board-to-board communication.
  // Physically connecting the board-to-board link is optional and
  // only required if using a separate set of sensors for the dampers.
  // These two classes are common to hammer and pedal boards.
  Adc.Setup(Set.adc_spi_clock_frequency, Set.adc_is_differential,
  Set.using18bitadc, Set.sensor_v_max, Set.adc_reference, &Tpl);
  B2B.Setup(Set.canbus_enable);

  // Diagnostics and status
  HStat.Setup(&DspP, &Tpl, Set.debug_level, Set.damper_threshold_low,
  Set.damper_threshold_high, Set.strike_threshold);

  // Setup the dampers, hammers, and pedals on hammer board.
  DspD.Setup( Set.damper_threshold_low, Set.damper_threshold_high,
  Set.damper_velocity_scaling, Set.adc_sample_period_microseconds, Set.debug_level);
  DspH.Setup(Set.adc_sample_period_microseconds, Set.strike_threshold,
  Set.release_threshold, Set.min_repetition_seconds, Set.min_strike_velocity,
  Set.hammer_travel_meters, Set.debug_level);
  DspP.Setup(Set.pedal_sample_interval_microseconds, Set.pedal_threshold,
  Set.sustain_pin, Set.sustain_connected_pin, Set.sostenuto_pin,
  Set.sostenuto_connected_pin, Set.una_corda_pin, Set.una_corda_connected_pin,
  Set.debug_level);

  // Dynamically adjust signal levels. Maybe removed in future.
  Gain.Setup(Set.velocity_scale, Set.debug_level);

  // Setup sending damper, hammer, and pedal data over MIDI.
  Midi.Setup(Set.midi_channel, &mi, Set.debug_level);

  // Common on hammer and pedal board: Ethernet, test points, TFT display, etc.
  Cal.Setup(Set.calibration_threshold, Set.calibration_match_gain,
  Set.calibration_match_offset, Set.debug_level, &Nonv);
  Eth.Setup(Set.computer_ip, Set.teensy_ip, Set.upd_port,
  Set.ethernet_start_ind, Set.ethernet_end_ind,
  Set.switch_debounce_micro, Set.debug_level);
  Tpl.Setup();
  Tmg.Setup(Set.adc_sample_period_microseconds);
  Tft.Setup(Set.using_display, Set.debug_level);
  Tft.HelloWorld();

  // Ready to be a piano.
  if (Set.debug_level >= DEBUG_STATS) {
    Serial.println("Finished hammer board initialization.");
  }

}

// After startup, wait before sending anything to MIDI
// in order to avoid any potential startup transients.
int startup_counter = 0;

// Switches.
bool switch_high_damper_threshold;
bool switch_external_damper_board;
bool switch_enable_ethernet;
bool switch_tft_display;
bool switch_set_peak_velocity;
bool switch_freeze_cal_values;
bool switch_disable_and_reset_calibration;

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];
int position_adc_counts[NUM_CHANNELS];

// Damper, hammer, and pedal data.
bool damper_event[NUM_CHANNELS], hammer_event[NUM_CHANNELS];
float damper_position[NUM_CHANNELS], calibrated_floats[NUM_CHANNELS],
position_floats[NUM_CHANNELS];
float damper_velocity[NUM_CHANNELS], hammer_velocity[NUM_CHANNELS];

// Data to send over Ethernet.
// TODO - This needs some work.
float raw_to_send[12];

void loop() {

  // Measure every processing interval so the pickup and
  // dropout timers update. Later, when a switch is read,
  // what is actually read is the internal state of timers.
  SwIPS1.updatePuDoState("IPS11", "IPS12");
  SwIPS2.updatePuDoState("IPS21", "IPS22");
  SwSCA2.updatePuDoState("SCA21", "SCA22");
  SwSCA1.updatePuDoState("SCA11", "SCA12");

  // Read all switch inputs.
  switch_high_damper_threshold = SwIPS1.read_switch_2();
  switch_external_damper_board = SwIPS1.read_switch_1();
  switch_enable_ethernet = SwIPS2.read_switch_2();
  switch_tft_display = SwIPS2.read_switch_1();
  switch_set_peak_velocity = SwSCA2.read_switch_1();
  switch_freeze_cal_values = SwSCA1.read_switch_2();
  switch_disable_and_reset_calibration = SwSCA1.read_switch_1();

  // When the TFT is operational, turn off the alorithms that
  // could generate MIDI output and slow down the sampling.
  // Slow down sampling because the TFT takes a long time for
  // processing. But, do keep the sampling going so that the TFT
  // can display things like maximum and minimum hammer positions.
  if (switch_tft_display == true) {
    DspD.Enable(false);
    DspH.Enable(false);
    DspP.Enable(false);
    switch_freeze_cal_values = true; // Ignore switch value.
    Tmg.ResetInterval(Set.adc_sample_period_microseconds_during_tft);
  }
  else {
    DspD.Enable(true);
    DspH.Enable(true);
    DspP.Enable(true);
    Tmg.ResetInterval(Set.adc_sample_period_microseconds);
  }

  // This if statement determines the sample rate.
  // Everything below in this file runs at the sample rate.
  if (Tmg.AllowProcessing() == true) {

    Tpl.SetTp8(true); // Front left test point asserts during processing.
    
    // Get hammer and pedal data from ADC.
    Adc.GetNewAdcValues(raw_samples);

    // Reorder the back row.
    Adc.ReorderAdcValues(raw_samples);

    // Normalize the ADC values.
    Adc.NormalizeAdcValues(position_adc_counts, position_floats, raw_samples);

    // Undo the sensor nonlinearity.
    bool all_notes_using_cal = Cal.Calibration(switch_freeze_cal_values,
    switch_disable_and_reset_calibration, calibrated_floats, position_floats);

    // Zero out the signal from any unconnected pins to avoid noise
    // or anything else causing an unwanted piano note to play.
    for (int k = 0; k < NUM_CHANNELS; k++) {
      if (Set.connected_channel[k] == false) {
        position_adc_counts[k] = 0;
        calibrated_floats[k] = 0.0;
      }
    }

    // Select source for damper signals.
    if (switch_external_damper_board == true) {
      // There exists a damper board so use remote data from the damper board.
      B2B.GetDamperData(damper_position);
    }
    else {
      // No external damper board.
      // Use the hammer position as an estimate of the damper position.
      for (int k = 0; k < NUM_CHANNELS; k++)
        damper_position[k] = calibrated_floats[k];
    }

    // Process hammer, damper, and pedal data.
    // For hammer and damper get an event boolean flag and velocity.
    // For pedal get the state of the pedal.
    DspD.GetDamperEventData(damper_event, damper_velocity, damper_position,
    switch_high_damper_threshold);
    DspH.GetHammerEventData(hammer_event, hammer_velocity, calibrated_floats);
    DspP.UpdatePedalState(calibrated_floats);

    // Adjust velocity. Probably not needed in the future.
    Gain.DamperVelocityScale(damper_velocity, damper_event);
    Gain.HammerVelocityScale(hammer_velocity, switch_set_peak_velocity, hammer_event);

    // Sending data over MIDI.
    if (startup_counter < Set.startup_counter_value) {
      startup_counter++;
    }
    else {
      Midi.SendNoteOn(&Mute, hammer_event, hammer_velocity);
      Midi.SendNoteOff(&Mute, damper_event, damper_velocity);
      Midi.SendPedal(&DspP);
    }

    // Send data over Ethernet. This needs work.
    //int offset = 5 * 12;
    raw_to_send[0] = calibrated_floats[27]; //offset + 0];
    raw_to_send[1] = calibrated_floats[29]; //offset + 1];
    raw_to_send[2] = calibrated_floats[31]; //offset + 2];
    raw_to_send[3] = calibrated_floats[32]; //offset + 3];
    raw_to_send[4] = calibrated_floats[34]; //offset + 4];
    raw_to_send[5] = calibrated_floats[36]; //offset + 5];
    raw_to_send[6] = calibrated_floats[38]; //offset + 6];
    raw_to_send[7] = calibrated_floats[39]; //offset + 7];
    raw_to_send[8] = calibrated_floats[41]; //offset + 8];
    raw_to_send[9] = calibrated_floats[43]; //offset + 9];
    raw_to_send[10] = calibrated_floats[44]; //offset + 10];
    raw_to_send[11] = calibrated_floats[46]; //offset + 11];
    Eth.SendPianoPacket(raw_to_send, switch_enable_ethernet);

    // Run the TFT display.
    Tft.Display(switch_tft_display, calibrated_floats, damper_position);

    // Debug and display information.
    HStat.FrontLed(calibrated_floats, switch_high_damper_threshold);
    HStat.LowerRightLed(all_notes_using_cal);
    HStat.SCALed();
    HStat.EthernetLed();
    HStat.SerialMonitor(position_adc_counts, calibrated_floats, hammer_event,
    Set.canbus_enable, switch_external_damper_board);
    Tpl.SetTp8(false);
  }
}