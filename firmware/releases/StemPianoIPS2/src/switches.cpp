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
// switches.cpp
//
// This class is not hardware dependent.
//
// Read the input from a 2-input DIP switch.
//
// TODO - Investigate need for set_pinmode_hardware_workaround().

#include "switches.h"

Switches::Switches() {}

void Switches::Setup(int switch_pu_do_setting, int pin_1, int pin_2,
int debug_level) { 

  debug_level_ = debug_level;

  pin_1_ = pin_1;
  pin_2_ = pin_2;

  startup_ = true;

  switch_pu_do_interval_ = static_cast<unsigned long>(switch_pu_do_setting);

  // Fix an intermittent failure discovered during hardware testing.
  set_pinmode_hardware_workaround(pin_1_);
  set_pinmode_hardware_workaround(pin_2_);

  // See comment below - must invert signal received.
  sw1_state_ = 1 - digitalRead(pin_1_);
  sw2_state_ = 1 - digitalRead(pin_2_);
  sw1_last_micros_ = micros();
  sw2_last_micros_ = micros();

}

// Switch is connected to ground, and input is pulled high.
// Turning 'on' shorts to ground.
// Turning 'off' uses pullup and so is high.
// Therefore, convert a 0 into 1 and a 1 into 0. Hence the y = 1-x code.
void Switches::updatePuDoState(const char *sw_1_name, const char *sw_2_name) {
  int sw_1_value;
  int sw_2_value;
  if (startup_ == true) {
    // Set mode again because one of pins is overlapping with MIDI
    // read pin of Teensy. The board does not use MIDI read and so must
    // undo anything that MIDI does to the pin r/w mode. For the other
    // pins this extra code is not needed but is ok to have.
    set_pinmode_hardware_workaround(pin_1_);
    set_pinmode_hardware_workaround(pin_2_);
    startup_ = false;
  }
  sw_1_value = 1 - digitalRead(pin_1_);
  sw_2_value = 1 - digitalRead(pin_2_);
  update_state(sw_1_value, &sw1_state_, &sw1_last_micros_, sw_1_name);
  update_state(sw_2_value, &sw2_state_, &sw2_last_micros_, sw_2_name);
}

bool Switches::read_switch_1() {
  if (sw1_state_ == 0)
    return false;
  else
    return true;
}
bool Switches::read_switch_2() {
  if (sw2_state_ == 0)
    return false;
  else
    return true;
}

// Pick-up / Drop-out logic to eliminate transients.
void Switches::update_state(int dip_value, int *dip_state,
unsigned long *dip_last_micros, const char *switch_name) {
  if (*dip_state == 0) {
    if (dip_value == 0) {
      *dip_last_micros = micros();
    }
    else if (*dip_last_micros >= switch_pu_do_interval_) {
      *dip_state = 1;
      *dip_last_micros = micros();
      if (debug_level_ >= DEBUG_STATS) {
        char display_string[MAX_SWITCH_NAME_LENGTH];
        sprintf(display_string,"DIP %s is now ON", switch_name);
        Serial.println(display_string);
      }
    }
  }
  else {
    if (dip_value == 1) {
      *dip_last_micros = micros();
    }
    else if (*dip_last_micros >= switch_pu_do_interval_) {
      *dip_state = 0;
      *dip_last_micros = micros();
      if (debug_level_ >= DEBUG_STATS) {
        char display_string[MAX_SWITCH_NAME_LENGTH];
        sprintf(display_string,"DIP %s is now OFF", switch_name);
        Serial.println(display_string);
      }
    }
  }
}

// Observed an intermittent failure mode as follows:
//   Switch is set to be open (high resistance, not on).
//   Teensy is set to have a pullup.
//   Power up Teensy.
//   Teensy read of pin returns a 0. Should be a 1.
//   Connect voltage meter to Teensy pin to check it.
//   As soon as voltage meter touches pin, the switch
//   immediately reads 1, as expected.
// When check resistance of switch on/off, it is as expected.
// So, something is not working properly with the pullup.
// The following workaround code fixed the problem and is harmless
// to use generally (in case the problem seen was due to something
// unique to the hardware being tested).
// Have not root-caused why this happened.
void Switches::set_pinmode_hardware_workaround(int pin_number) {
  pinMode(pin_number, INPUT_PULLUP);
  delay(20);
  pinMode(pin_number, INPUT_PULLDOWN);
  delay(20);
  pinMode(pin_number, INPUT_PULLUP);
  delay(20);
}