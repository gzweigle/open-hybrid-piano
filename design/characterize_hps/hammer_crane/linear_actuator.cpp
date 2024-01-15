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
// linear_actuator.cpp
//
// Control linear actuator through L298N board.

#include "linear_actuator.h"

LinearActuator::LinearActuator() {}

void LinearActuator::Setup(bool arduino_workaround) {
  arduino_workaround_ = arduino_workaround;

  if (arduino_workaround_ == true) {
    // For some Arduino Nano the following does not work:
    //   DigitalWrite(PBX, <value>);
    //   X = 0, 1, 2, ...
    // Therefore, write registers directly.
    DDRB |= _BV(0);   // Set PB0 as output.
    DDRB |= _BV(1);   // Set PB1 as output.
  }
  else {
    pinMode(PB0, OUTPUT);
    pinMode(PB1, OUTPUT);
  }
}

// Position input from linear actuator to 10-bit internal ADC.
// Tests show resistance for the linear actuator ranges from:
//   45 = fully retracted.
//   473 = fully extended.
float LinearActuator::GetPositionRatio() {
  float resistance, ratio;
  resistance = static_cast<float>(analogRead(A7));
  ratio = resistance - 45.0;
  ratio /= (473.0 - 45.0);
  if (ratio < 0) {
    ratio = 0;
  }
  else if (ratio > 1.0) {
    ratio = 1.0;
  }
  return ratio;
}

void LinearActuator::Extend() {
  if (arduino_workaround_ == true) {
    PORTB |= _BV(0);
    PORTB &= ~(_BV(1));
  }
  else {
    digitalWrite(PB0, HIGH);
    digitalWrite(PB1, LOW);
  }
}

void LinearActuator::Retract() {
  if (arduino_workaround_ == true) {
    PORTB &= ~(_BV(0));
    PORTB |= _BV(1);
  }
  else {
    digitalWrite(PB0, LOW);
    digitalWrite(PB1, HIGH);
  }
}

void LinearActuator::Stop() {
  if (arduino_workaround_ == true) {
    PORTB &= ~(_BV(0));
    PORTB &= ~(_BV(1));
  }
  else {
    digitalWrite(PB0, LOW);
    digitalWrite(PB1, LOW);
  }
}