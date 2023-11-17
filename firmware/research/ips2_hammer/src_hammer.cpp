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
// src_hammer.cpp
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Main code for hammer board of Stem Piano.
//
// TODO - For more than 88 keys, could use local hammer for extra
//        dampers, and remote for the normal 88.

#include "stem_piano_ips2.h"

#include "hammer_settings.h"
#include "six_channel_analog_00.h"
#include "board2board.h"
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

HammerSettings Set;

SixChannelAnalog00 Adc;
Board2Board B2B;
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
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, mi);

void setup(void) {

  // This must be placed first and in the setup() function.
  Set.SetAllSettingValues();

  // These must be called second and in the setup() function.
  SwIPS1.Setup(Set.switch_debounce_micro,
  Set.switch11_ips_pin, Set.switch12_ips_pin, Set.debug_level);
  SwIPS2.Setup(Set.switch_debounce_micro,
  Set.switch21_ips_pin, Set.switch22_ips_pin, Set.debug_level);
  SwSCA1.Setup(Set.switch_debounce_micro,
  Set.switch11_sca_pin, Set.switch12_sca_pin, Set.debug_level);
  SwSCA2.Setup(Set.switch_debounce_micro,
  Set.switch21_sca_pin, Set.switch22_sca_pin, Set.debug_level);

  // Serial port setup.
  Serial.begin(Set.serial_baud_rate);

  if (Set.debug_level >= 1) {
    Serial.println("Beginning hammer board initialization.");
  }

  // Common to hammer and pedal board.
  Adc.Setup(Set.adc_spi_clock_frequency, Set.adc_is_differential,
  Set.adc_extra_settling_time_nanoseconds, &Tpl);
  B2B.Setup(Set.canbus_enable);

  // Optional to dynamically adjust gain of values from ADC.
  Gain.Setup(Set.adjust_gain, Set.adc_scale_threshold);

  // Processing dampers, hammers, and pedals on hammer board.
  DspD.Setup(Set.using_hammer_to_estimate_damper,
  Set.damper_threshold_using_damper, Set.damper_threshold_using_hammer,
  Set.damper_velocity_scaling, Set.adc_sample_period_microseconds, Set.debug_level);
  DspH.Setup(Set.adc_sample_period_microseconds, Set.strike_threshold,
  Set.release_threshold, Set.min_repetition_seconds, Set.min_strike_velocity,
  Set.hammer_travel_meters, Set.initialize_hammer_wait_count, Set.debug_level);
  DspP.Setup(Set.pedal_sample_interval_microseconds, Set.pedal_threshold,
  Set.sustain_pin, Set.sustain_connected_pin, Set.sostenuto_pin,
  Set.sostenuto_connected_pin, Set.una_corda_pin, Set.una_corda_connected_pin,
  Set.debug_level);

  // Sending damper, hammer, and pedal data over MIDI.
  Midi.Setup(Set.midi_channel, &mi, Set.debug_level);

  // Common on hammer and pedal board.
  Eth.Setup(Set.computer_ip, Set.teensy_ip, Set.upd_port, Set.debug_level);
  Tpl.Setup();
  Tmg.Setup(Set.adc_sample_period_microseconds);
  Tft.Setup(Set.using_display, Set.debug_level);
  Tft.HelloWorld();

  if (Set.debug_level >= 1) {
    Serial.println("Finished hammer board initialization.");
  }

  Serial.println("GNU GPLv3 - for documentation, code, and design see:");
  Serial.println("https://github.com/gzweigle/DIY-Grand-Digital-Piano");

  Serial.println(".");
  Serial.println("Welcome to Stem Piano, IPS2.X/SCA0.0 Hammer Board, by GCZ.");
  Serial.println(".");

}

// Temporary debug and diagnostic stuff.
unsigned long last_micros_ = micros();
bool serial_display_toggle = false;

// Place declaration of all variables for DIP switches here.
bool switch_tft;

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];
int hammer_position_adc_counts[NUM_CHANNELS];

// Damper, hammer, and pedal data.
bool damper_event[NUM_CHANNELS], hammer_event[NUM_CHANNELS];
float damper_position[NUM_CHANNELS], hammer_position[NUM_CHANNELS], hammer_scaled[NUM_CHANNELS];
float damper_velocity[NUM_CHANNELS], hammer_velocity[NUM_CHANNELS];

// Velocity scaling.
float vscale = 1.0;

void loop() {

  // Measure every processing interval so the pickup and
  // dropout timers update. In other places, when a switch
  // is read, what is actually read is the internal state
  // of timers.
  SwIPS1.updatePuDoState("IPS11", "IPS12");
  SwIPS2.updatePuDoState("IPS21", "IPS22");
  SwSCA1.updatePuDoState("SCA11", "SCA12");
  SwSCA2.updatePuDoState("SCA21", "SCA22");

  // Read all switch values in one place so can keep track of them.
  switch_tft = SwIPS2.read_switch_1();  // TFT mode.

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

  // This if statement determines the sample rate.
  if (Tmg.AllowProcessing() == true) {
    Tpl.SetTp8(true);
    
    // Get all data: local hammer (ADC), remote damper (board-to-board) data,
    // and local pedals (also from ADC data).
    Adc.GetNewAdcValues(raw_samples);

    // Reorder the back row.
    int tmp;
    for (int ind = 0; ind < 4; ind++) {
      for (int grp = 0; grp < NUM_CHANNELS; grp += 16) {
        tmp = raw_samples[ind + grp];
        raw_samples[ind + grp] = raw_samples[7-ind + grp];
        raw_samples[7-ind + grp] = tmp;
      }
    }

    Adc.NormalizeAdcValues(hammer_position_adc_counts, hammer_position, raw_samples);
    Gain.AutomaticGainControl(hammer_scaled, hammer_position);
    for (int k = 0; k < NUM_CHANNELS; k++) {
      if (Set.connected_channel[k] == false) {
        hammer_position_adc_counts[k] = 0;
        hammer_scaled[k] = 0.0;
      }
    }

    if (Set.connected_to_remote_damper_board == true) {
      // There exists a damper board so use remote data from the damper board.
      B2B.GetDamperData(damper_position);
      for (int k = 0; k < NUM_CHANNELS; k++) {
        if (Set.using_hammer_to_estimate_damper[k] == true) {
          damper_position[k] = hammer_scaled[k];
        }
      }
    }
    else {
      // No external damper board.
      // Use the hammer position as an estimate of the damper position.
      for (int i = 0; i < NUM_CHANNELS; i++)
        damper_position[i] = hammer_scaled[i];
    }

    // From data, process hammer, damper, and pedal data.
    // For hammer and damper get an event boolean flag and velocity.
    // For pedal get the state of the pedal.
    DspD.GetDamperEventData(damper_event, damper_velocity, damper_position);
    DspH.GetHammerEventData(hammer_event, hammer_velocity, hammer_scaled);
    DspP.UpdatePedalState(hammer_scaled);

    if (SwSCA1.read_switch_1() == true) {
      vscale = 0.01;
    }
    else if (SwSCA1.read_switch_2() == true) {
      vscale = 3.0;
    }
    else if (SwSCA2.read_switch_1() == true) {
      vscale = 4.0;
    }
    else if (SwSCA2.read_switch_2() == true) {
      vscale = 5.0;
    }
    else {
      vscale = 1.0;
    }
    for (int key = 0; key < 88; key++) {
      if (damper_event[key] == true) {
        damper_velocity[key] *= vscale;
        if (damper_velocity[key] > 1.0) {
          if (Set.debug_level >= 1) {
            Serial.print("damper ");
            Serial.print(key);
            Serial.print(" hit limit, orig velocity was ");
            Serial.println(damper_velocity[key] / vscale);
          }
          damper_velocity[key] = 1.0;
        }
      }
      if (hammer_event[key] == true) {
        hammer_velocity[key] *= vscale;
        if (hammer_velocity[key] > 1.0) {
          if (Set.debug_level >= 1) {
            Serial.print("hammer ");
            Serial.print(key);
            Serial.print(" hit limit, orig velocity was ");
            Serial.println(hammer_velocity[key] / vscale);
          }
          hammer_velocity[key] = 1.0;
        }
      }
    }

    // Sending data over MIDI and Ethernet.
    Midi.SendNoteOn(hammer_event, hammer_velocity);
    Midi.SendNoteOff(damper_event, damper_velocity);
    Midi.SendPedal(&DspP);
    Eth.SendPianoPacket(hammer_scaled[32],
                        hammer_scaled[33],
                        hammer_scaled[34],
                        hammer_scaled[35],
                        hammer_scaled[36],
                        hammer_scaled[37],
                        hammer_scaled[38],
                        hammer_scaled[39]);

    // Run the TFT display.
    Tft.Display(switch_tft, hammer_scaled, damper_position);

    /////////////////
    // Temporary debug and diagnostic stuff.
    // Flash all the LED to make sure they are working.
    // Except tp8 and tp9 as they are used elsewhere.
    if (micros() - last_micros_ > Set.serial_display_interval) {
      last_micros_ = micros();
      if (serial_display_toggle == true) {
        serial_display_toggle = false;
        Tpl.SetTp10(false);
        Tpl.SetTp11(true);
        Tpl.SetLowerRightLED(false);
        Tpl.SetScaLedL(false);
        Tpl.SetScaLedR(false);
        Tpl.SetEthernetLED(false);
      }
      else {
        serial_display_toggle = true;
        if (Set.debug_level >= 1) {
          Serial.print("Hammer Board ");
          if (Set.board_bringup == true) {
            Serial.print(" h24="); Serial.print(hammer_scaled[24]);
            Serial.print(" h25="); Serial.print(hammer_scaled[25]);
            Serial.print(" d24="); Serial.print(damper_position[24]);
            Serial.print(" d25="); Serial.print(damper_position[25]);
          }
          else
          {
            Serial.print(" d32="); Serial.print(damper_position[32]);
            Serial.print(" d33="); Serial.print(damper_position[33]);
            Serial.print(" d34="); Serial.print(damper_position[34]);
            Serial.print(" d35="); Serial.print(damper_position[35]);
            Serial.print(" d36="); Serial.print(damper_position[36]);
            Serial.print(" d37="); Serial.print(damper_position[37]);
            Serial.print(" d38="); Serial.print(damper_position[38]);
            Serial.print(" d39="); Serial.print(damper_position[39]);
          }
          Serial.println("");
        }
        Tpl.SetTp10(true);
        Tpl.SetTp11(false);
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