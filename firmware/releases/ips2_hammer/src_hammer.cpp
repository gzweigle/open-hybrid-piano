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
//
//
//
// stem piano is an open source hybrid digital piano.
//
//
//
//
// TODO - For more than 88 keys, could use local hammer for extra
//        dampers, and remote for the normal 88.
//      - Max velocity should be 127. Need to automate vscale.
// TODO - Include calibration once testing and documentation is finished.

//#define INCLUDE_CALIBRATION

#include "stem_piano_ips2.h"

#include "hammer_settings.h"
#include "six_channel_analog_00.h"
#include "auto_mute.h"
#include "board2board.h"
#if INCLUDE_CALIBRATION
#include "calibration_build.h"
#endif
#include "dsp_damper.h"
#include "dsp_hammer.h"
#include "dsp_pedal.h"
#include "gain_control.h"
#include "midiout.h"
#include "network.h"
#include "switches.h"
#include "testpoint_led.h"
#include "timing.h"
#include "tft_display.h"
#include "utilities.h"

HammerSettings Set;
SixChannelAnalog00 Adc;
AutoMute Mute;
Board2Board B2B;
#if INCLUDE_CALIBRATION
CalibrationBuild CalB;
//CalibrationRuntime CalR;
#endif
DspDamper DspD;
DspHammer DspH;
DspPedal DspP;
GainControl Gain;
MidiOut Midi;
Network Eth;
Switches SwIPS1;
Switches SwIPS2;
Switches SwSCA1;
Switches SwSCA2;
TestpointLed Tpl;
Timing Tmg;
TftDisplay Tft;
Utilities Utl0;
Utilities Utl1;
Utilities Utl2;
Utilities Utl3;
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
  Serial.println("  HPS0.7 or HPS0.4 sensor board (any board meeting spec is also ok).");
  Serial.println("License: GNU GPLv3 - for documentation, code, and design see:");
  Serial.println("  https://github.com/gzweigle/DIY-Grand-Digital-Piano");
  Serial.println(".");
  Serial.println(".");

  // Load the settings. Must be first in the setup() function.
  Set.SetAllSettingValues();

  // Setup reading the switches.
  SwIPS1.Setup(Set.switch_debounce_micro,
  Set.switch11_ips_pin, Set.switch12_ips_pin, Set.debug_level);
  SwIPS2.Setup(Set.switch_debounce_micro,
  Set.switch21_ips_pin, Set.switch22_ips_pin, Set.debug_level);
  SwSCA1.Setup(Set.switch_debounce_micro,
  Set.switch11_sca_pin, Set.switch12_sca_pin, Set.debug_level);
  SwSCA2.Setup(Set.switch_debounce_micro,
  Set.switch21_sca_pin, Set.switch22_sca_pin, Set.debug_level);

  if (Set.debug_level >= 1) {
    Serial.println("Beginning hammer board initialization.");
  }

  Mute.Setup();

  // Setup the analog front-end and the board-to-board communication.
  // Physically connecting the board-to-board link is optional and
  // only required if using a separate set of sensors for the dampers.
  // These two classes are common to hammer and pedal boards.
  Adc.Setup(Set.adc_spi_clock_frequency, Set.adc_is_differential,
  Set.using18bitadc, Set.sensor_v_max, Set.adc_reference, &Tpl);
  B2B.Setup(Set.canbus_enable);

  // Build the calibration values.
  #if INCLUDE_CALIBRATION
  CalB.Setup(&Tpl);
  //CalR.Setup();
  #endif

  // Setup the dampers, hammers, and pedals on hammer board.
  DspD.Setup(Set.using_hammer_to_estimate_damper,
  Set.damper_threshold_using_damper, Set.damper_threshold_using_hammer,
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
  Eth.Setup(Set.computer_ip, Set.teensy_ip, Set.upd_port,
  Set.ethernet_start_ind, Set.ethernet_end_ind, Set.debug_level);
  Tpl.Setup();
  Tmg.Setup(Set.adc_sample_period_microseconds);
  Tft.Setup(Set.using_display, Set.debug_level);
  Tft.HelloWorld();

  // Utility functionality.
  Utl0.Setup();
  Utl1.Setup();
  Utl2.Setup();
  Utl3.Setup();

  // Getting ready to run!....
  if (Set.debug_level >= 1) {
    Serial.println("Finished hammer board initialization.");
  }

}

// Temporary debug and diagnostic stuff.
unsigned long display_last_micros_ = micros();
bool serial_display_toggle = false;

// After startup, wait before sending anything to MIDI
// in order to avoid any potential startup transients.
int startup_counter = 0;

// Place declaration of all variables for DIP switches here.
bool switch_tft;
#if INCLUDE_CALIBRATION
bool switch_cal_motor_enable;
bool switch_cal_motor_direction;
bool switch_cal_build_cal_values;
#endif

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];
int position_adc_counts[NUM_CHANNELS];

// Damper, hammer, and pedal data.
bool damper_event[NUM_CHANNELS], hammer_event[NUM_CHANNELS];
float damper_position[NUM_CHANNELS], position_floats[NUM_CHANNELS];
float damper_velocity[NUM_CHANNELS], hammer_velocity[NUM_CHANNELS];

// Data for display and also used to build the calibration tables.
unsigned int rs0, rs1, rs2, rs3;

// Data to send over Ethernet.
// TODO - This needs some work.
float raw_to_send[12];

void loop() {

  // Measure every processing interval so the pickup and
  // dropout timers update. Later, when a switch is read,
  // what is actually read is the internal state of timers.
  SwIPS1.updatePuDoState("IPS11", "IPS12");
  SwIPS2.updatePuDoState("IPS21", "IPS22");
  SwSCA1.updatePuDoState("SCA11", "SCA12");
  SwSCA2.updatePuDoState("SCA21", "SCA22");

  // Read all switch inputs.
  switch_tft = SwIPS2.read_switch_1();
  #if INCLUDE_CALIBRATION
  switch_cal_motor_enable = SwIPS1.read_switch_1();
  switch_cal_motor_direction = SwIPS1.read_switch_2(); 
  switch_cal_build_cal_values = SwIPS2.read_switch_2();
  #endif

  // When the TFT is operational, turn off the alorithms that
  // could generate MIDI output and slow down the sampling.
  // Slow down sampling because the TFT takes a long time for
  // processing. But, do keep the sampling going so that the TFT
  // can display things like maximum and minimum hammer positions.
  if (switch_tft == true) {
    DspD.Enable(false);
    DspH.Enable(false);
    DspP.Enable(false);
    Tmg.ResetInterval(Set.adc_sample_period_microseconds_during_tft);
  }
  else {
    DspD.Enable(true);
    DspH.Enable(true);
    DspP.Enable(true);
    Tmg.ResetInterval(Set.adc_sample_period_microseconds);
  }

  // For now, using the same board for creating the calibration values,
  // as use for runtime. The advantage is being able to include the full
  // front end analog processing, all the way to the Teensy for each
  // channel. However, his probably is not necessary and so may create
  // a separate software program for the creation of calibration values.
  if (Set.use_board_to_generate_calibration_values_ == true) {
    #if INCLUDE_CALIBRATION
    CalB.BuildCalibrationTables(switch_cal_motor_enable, switch_cal_motor_direction,
    switch_cal_build_cal_values, rs0, rs1);
    #endif
  }

  // This if statement determines the sample rate.
  // Everything below in this file runs at the sample rate.
  if (Tmg.AllowProcessing() == true) {

    Tpl.SetTp8(true); // Front left test point asserts during processing.
    
    // Get all data: local hammer (ADC), remote damper (board-to-board) data,
    // and local pedals (also from ADC data). Place in raw_samples[NUM_CHANNELS].
    Adc.GetNewAdcValues(raw_samples);

    // Reorder the back row.
    Adc.ReorderAdcValues(raw_samples);

    // Undo sensor nonlinearity and, optionally, mechanical tolerances.
    #if INCLUDE_CALIBRATION
    //CalR.Calibrate(raw_samples);
    #endif

    // Normalize the ADC values.
    Adc.NormalizeAdcValues(position_adc_counts, position_floats, raw_samples);

    // Zero out the signal from any unconnected pins to avoid noise
    // or anything else causing an unwanted piano note to play.
    for (int k = 0; k < NUM_CHANNELS; k++) {
      if (Set.connected_channel[k] == false) {
        position_adc_counts[k] = 0;
        position_floats[k] = 0.0;
      }
    }

    // Select source for damper signals.
    if (Set.connected_to_remote_damper_board == true) {
      // There exists a damper board so use remote data from the damper board.
      B2B.GetDamperData(damper_position);
      for (int k = 0; k < NUM_CHANNELS; k++) {
        if (Set.using_hammer_to_estimate_damper[k] == true) {
          damper_position[k] = position_floats[k];
        }
      }
    }
    else {
      // No external damper board.
      // Use the hammer position as an estimate of the damper position.
      for (int k = 0; k < NUM_CHANNELS; k++)
        damper_position[k] = position_floats[k];
    }

    // Process hammer, damper, and pedal data.
    // For hammer and damper get an event boolean flag and velocity.
    // For pedal get the state of the pedal.
    DspD.GetDamperEventData(damper_event, damper_velocity, damper_position);
    DspH.GetHammerEventData(hammer_event, hammer_velocity, position_floats);
    DspP.UpdatePedalState(position_floats);

    // Adjust velocity. Probably not needed in the future.
    Gain.AutomaticGainControl(damper_velocity, damper_event);
    Gain.AutomaticGainControl(hammer_velocity, hammer_event);

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
    #ifdef CALIBRATION_SETUP
    raw_to_send[0] = position_floats[0];
    raw_to_send[1] = position_floats[1];
    #else
    //int offset = 5 * 12;
    raw_to_send[0] = position_floats[24]; //offset + 0];
    raw_to_send[1] = position_floats[26]; //offset + 1];
    raw_to_send[2] = position_floats[27]; //offset + 2];
    raw_to_send[3] = position_floats[29]; //offset + 3];
    raw_to_send[4] = position_floats[31]; //offset + 4];
    raw_to_send[5] = position_floats[32]; //offset + 5];
    raw_to_send[6] = position_floats[34]; //offset + 6];
    raw_to_send[7] = position_floats[36]; //offset + 7];
    raw_to_send[8] = position_floats[38]; //offset + 8];
    raw_to_send[9] = position_floats[39]; //offset + 9];
    raw_to_send[10] = position_floats[41]; //offset + 10];
    raw_to_send[11] = position_floats[43]; //offset + 11];
    #endif

    Eth.SendPianoPacket(raw_to_send);

    // Run the TFT display.
    Tft.Display(switch_tft, position_floats, damper_position);

    // Filter data to be displayed and also that is used for
    // generating the calibration tables. Note that rs* is
    // not used until next processing interval (above) and
    // that is ok since latency does not matter for
    // generating the calibration tables.
    #ifdef CALIBRATION_SETUP
    rs0 = Utl0.boxcarFilterUInts(position_adc_counts[0]);
    rs1 = Utl1.boxcarFilterUInts(position_adc_counts[1]);
    rs2 = Utl2.boxcarFilterUInts(position_adc_counts[2]);
    rs3 = Utl3.boxcarFilterUInts(position_adc_counts[3]);
    #else
    rs0 = Utl0.boxcarFilterUInts(position_adc_counts[26]);
    rs1 = Utl1.boxcarFilterUInts(position_adc_counts[27]);
    rs2 = Utl2.boxcarFilterUInts(position_adc_counts[28]);
    rs3 = Utl3.boxcarFilterUInts(position_adc_counts[29]);
    #endif



    /////////////////////////////////////////////////
    // Debug and display information.
    // Not critical for the piano.
    /////////////////////////////////////////////////

    // Only set Tp* if not calibrating because these pins
    // are used by the calibration to control the motors.
    if (Set.use_board_to_generate_calibration_values_ == false) {

      bool found;

      // Turn on LED if any key is above damper threshold.
      found = false;
      for (int k = 0; k < NUM_NOTES; k++) {
        if (position_floats[k] > Set.damper_threshold_using_hammer) {
          Tpl.SetTp9(true);
          found = true;
          break;
        }
      }
      if (found == false)
        Tpl.SetTp9(false);

      // Turn on LED if any key is above strike thresold threshold.
      found = false;
      for (int k = 0; k < NUM_NOTES; k++) {
        if (position_floats[k] > Set.strike_threshold) {
          Tpl.SetTp10(true);
          found = true;
          break;
        }
      }
      if (found == false)
        Tpl.SetTp10(false);

      if (DspP.GetSustainCrossedUpThreshold() || 
      DspP.GetSostenutoCrossedUpThreshold() ||
      DspP.GetUnaCordaCrossedUpThreshold()) {
        Tpl.SetTp11(false);
      }
      else if (DspP.GetSustainCrossedDownThreshold() || 
      DspP.GetSostenutoCrossedDownThreshold() ||
      DspP.GetUnaCordaCrossedDownThreshold()) {
        Tpl.SetTp11(true);
      }
    }

    if (micros() - display_last_micros_ > Set.serial_display_interval_micro) {
      display_last_micros_ = micros();
      if (serial_display_toggle == true) {
        serial_display_toggle = false;
        Tpl.SetLowerRightLED(false);
        Tpl.SetScaLedL(false);
        Tpl.SetScaLedR(false);
        Tpl.SetEthernetLED(false);
      }
      else {
        serial_display_toggle = true;
        //if (Set.debug_level >= 1) {
          Serial.print("Hammer Board ");
          Serial.print(" r0a="); Serial.print(rs0);
          Serial.print(" r1a="); Serial.print(rs1);
          Serial.print(" r0f="); Serial.print(position_floats[0]);//*3.3/2.5);
          Serial.print(" r1f="); Serial.print(position_floats[1]);//*3.3/2.5);
          Serial.println("");
        //}
        Tpl.SetLowerRightLED(true);
        Tpl.SetScaLedL(true);
        Tpl.SetScaLedR(true);
        Tpl.SetEthernetLED(true);
      }
    }
    /////////////////

    Tpl.SetTp8(false);
  }
}