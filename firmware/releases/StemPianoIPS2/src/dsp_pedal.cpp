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
// dsp_pedal.cpp
//
// This class is not hardware dependent.
//
// Pedal input processing
//
// Pedal (for polarity of the triple pedal I tested with):
//   When connected:
//     Pedal not pressed = small resistance, leading to a small ADC value.
//     Pedal is pressed = large resistance, leading to a larger ADC value.
//   When not connected:
//     Pedal is open circuited, leading to a larger ADC value.
// Connected signal:
//     Pedal is connected - tied to ground.
//     Pedal is not connected - tied to pedal signal.
//
// TODO - Does not handle the following pedal corner cases:
//        1. Resetting max value if a different pedal is plugged in.
//        2. Detecting that a pedal was removed.
//        3. Different pedal polarities.
// TODO - The names and MIDI functionality of everything is hardcoded
//        to sustain, sostenuto, and una corda. What if someone wants
//        a different pedal type with a different MIDI code?
//        Since pedals are independent could refactor into a single class
//        and iterate over for pedals of different types and positions.
// TODO - Does not use the quarter inch jack connected inputs.
// TODO - Compile time check that pin numbers do not exceed NUM_CHANNELS.

#include "dsp_pedal.h"

// Public methods. ////////

DspPedal::DspPedal() {}

void DspPedal::Setup(int pedal_sample_interval_microseconds,
float pedal_threshold, int sustain_pin, int sustain_connected_pin,
int sostenuto_pin, int sostenuto_connected_pin, int una_corda_pin,
int una_corda_connected_pin, int debug_level) {

  debug_level_ = debug_level;
 
  Timing_.Setup(pedal_sample_interval_microseconds, debug_level);

  // Want a small value, just enough to detect a pedal is connected.
  initial_threshold_ = 0.2;
  
  // Threshold when running.
  pedal_threshold_ = pedal_threshold;

  // To which ADC pin is the pedal signal connected?
  // These pins are selected in the settings.cpp file
  // associated with the board.
  sustain_pin_ = sustain_pin;
  sostenuto_pin_ = sostenuto_pin;
  una_corda_pin_ = una_corda_pin;

  // Not used yet.
  sustain_connected_pin_ = sustain_connected_pin;
  sostenuto_connected_pin_ = sostenuto_connected_pin;
  una_corda_connected_pin_ = una_corda_connected_pin;

  for (int ind = 0; ind < NUM_PEDALS; ind++) {
    pedal_last_[ind] = 0.0;
    connected_[ind] = false;
    state_[ind] = State::no_change;
    max_position_[ind] = 0.0;
    max_position_valid_[ind] = false;
    threshold_[ind] = initial_threshold_;
  }
  enable_ = true;
}

// Position measurement is [0.0 to 1.0], where 1.0 is maximum ADC value.
void DspPedal::UpdatePedalState(const float *position) {

  if (Timing_.AllowProcessing() == true) {

    state_[Ind::sustain] = ComputeState(position[sustain_pin_],
    pedal_last_[Ind::sustain], threshold_[Ind::sustain]);

    state_[Ind::sostenuto] = ComputeState(position[sostenuto_pin_],
    pedal_last_[Ind::sostenuto], threshold_[Ind::sostenuto]);

    state_[Ind::una_corda] = ComputeState(position[una_corda_pin_],
    pedal_last_[Ind::una_corda], threshold_[Ind::una_corda]);

    pedal_last_[Ind::sustain] = position[sustain_pin_];
    pedal_last_[Ind::sostenuto] = position[sostenuto_pin_];
    pedal_last_[Ind::una_corda] = position[una_corda_pin_];

    if (IsSustainConnected(position[sustain_pin_]) == false) {
      state_[Ind::sustain] = State::no_change;
    }
    if (IsSostenutoConnected(position[sostenuto_pin_]) == false) {
      state_[Ind::sostenuto] = State::no_change;
    }
    if (IsUnaCordaConnected(position[una_corda_pin_]) == false) {
      state_[Ind::una_corda] = State::no_change;
    }

    DetectSustainFirstPress(position[sustain_pin_]);
    DetectSostenutoFirstPress(position[sostenuto_pin_]);
    DetectUnaCordaFirstPress(position[una_corda_pin_]);
    UpdateSustainMaxValue(position[sustain_pin_]);
    UpdateSostenutoMaxValue(position[sostenuto_pin_]);
    UpdateUnaCordaMaxValue(position[una_corda_pin_]);
    UpdatePedalThresholds();

  }
  else {
      // Want the state change to pulse otherwise will send
      // repeated state change signals over MIDI.
      state_[Ind::sustain] = State::no_change;
      state_[Ind::sostenuto] = State::no_change;
      state_[Ind::una_corda] = State::no_change;
  }

}

int DspPedal::ComputeState(float position, float last_position, float threshold) {
  int pedal_state_out;
  if (position > threshold) {
    if (last_position < threshold)
      pedal_state_out = State::off_to_on;  // Pedal is now pressed.
    else
      pedal_state_out = State::no_change;  // No change to pedal state.
  }
  else {
    if (last_position > threshold)
      pedal_state_out = State::on_to_off;  // Pedal is no longer pressed.
    else
      pedal_state_out = State::no_change;  // No change to pedal state.
  }
  return pedal_state_out;
}

// Set the pedals up/down threshold based on the max
// position as measured by the sensor. If the max position
// has yet to be determined, then use the threshold directly.
void DspPedal::UpdatePedalThresholds() {
  for (int ind = 0; ind < NUM_PEDALS; ind++) {
    if (max_position_valid_[ind] == true) {
      threshold_[ind] = max_position_[ind] * pedal_threshold_;
    }
    else {
      threshold_[ind] = initial_threshold_;
    }
  }
}

// Keep track of min and max hammer position.
// However, when the pedal is disconnected, the max will be at ADC max.
// Therefore, only check this after confirming a pedal is connected.
void DspPedal::UpdateSustainMaxValue(float position) {
  if (max_position_valid_[Ind::sustain] == true) {
    if (position > max_position_[Ind::sustain]) {
      max_position_[Ind::sustain] = position;
      if (debug_level_ >= DEBUG_ALG) {
        Serial.print("UpdateSustainMaxValue() new max = ");
        Serial.println(position);
      }
    }
  }
}
void DspPedal::UpdateSostenutoMaxValue(float position) {
  if (max_position_valid_[Ind::sostenuto] == true) {
    if (position > max_position_[Ind::sostenuto]) {
      max_position_[Ind::sostenuto] = position;
      if (debug_level_ >= DEBUG_ALG) {
        Serial.print("UpdateSostenutoMaxValue() new max = ");
        Serial.println(position);
      }
    }
  }
}
void DspPedal::UpdateUnaCordaMaxValue(float position) {
  if (max_position_valid_[Ind::una_corda] == true) {
    if (position > max_position_[Ind::una_corda]) {
      max_position_[Ind::una_corda] = position;
      if (debug_level_ >= DEBUG_ALG) {
        Serial.print("UpdateUnaCordaMaxValue() new max = ");
        Serial.println(position);
      }
    }
  }
}

// Until a pedal is pressed once, the max positions will be incorrect.
// Therefore, check and set a flag that the max positions are valid.
void DspPedal::DetectSustainFirstPress(float position) {
  if (GetSustainCrossedDownThreshold() && IsSustainConnected(position))
    max_position_valid_[Ind::sustain] = true;
}
void DspPedal::DetectSostenutoFirstPress(float position) {
  if (GetSostenutoCrossedDownThreshold() && IsSostenutoConnected(position))
    max_position_valid_[Ind::sostenuto] = true;
}
void DspPedal::DetectUnaCordaFirstPress(float position) {
  if (GetUnaCordaCrossedDownThreshold() && IsUnaCordaConnected(position))
    max_position_valid_[Ind::una_corda] = true;
}

// Both unconnected and pedal being pressed down results in a large
// ADC value. Therefore, need to detect if the pedal is connected
// otherwise an unconnected pedal will result in sustain/sostenuto/unacorda
// active being sent out MIDI. The solution is decalare the pedal connected
// the first time it is < threshold.
bool DspPedal::IsSustainConnected(float position) {
  if (position < initial_threshold_) {
    connected_[Ind::sustain] = true;
  }
  return connected_[Ind::sustain];
}
bool DspPedal::IsSostenutoConnected(float position) {
  if (position < initial_threshold_) {
    connected_[Ind::sostenuto] = true;
  }
  return connected_[Ind::sostenuto];
}
bool DspPedal::IsUnaCordaConnected(float position) {
  if (position < initial_threshold_) {
    connected_[Ind::una_corda] = true;
  }
  return connected_[Ind::una_corda];
}

bool DspPedal::GetSustainCrossedDownThreshold() {
  if (state_[Ind::sustain] == State::off_to_on)
    return true;
  else
    return false;
}
bool DspPedal::GetSustainCrossedUpThreshold() {
  if (state_[Ind::sustain] == State::on_to_off)
    return true;
  else
    return false;
}
bool DspPedal::GetSostenutoCrossedDownThreshold() {
  if (state_[Ind::sostenuto] == State::off_to_on)
    return true;
  else
    return false;
}
bool DspPedal::GetSostenutoCrossedUpThreshold() {
  if (state_[Ind::sostenuto] == State::on_to_off)
    return true;
  else
    return false;
}
bool DspPedal::GetUnaCordaCrossedDownThreshold() {
  if (state_[Ind::una_corda] == State::off_to_on)
    return true;
  else
    return false;
}
bool DspPedal::GetUnaCordaCrossedUpThreshold() {
  if (state_[Ind::una_corda] == State::on_to_off)
    return true;
  else
    return false;
}

void DspPedal::Enable(bool enable) {
  enable_ = enable;
}