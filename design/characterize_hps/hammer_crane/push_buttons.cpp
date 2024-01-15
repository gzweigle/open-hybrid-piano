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
// push_buttons.cpp
//
// Push buttons on breadboard.
//

#include "push_buttons.h"

PushButtons::PushButtons() {}

void PushButtons::Setup() {
  // One side of push button is connected through a resistor to ground.
  // The other side is connected to Arduino Nano with an internal pullup.
  pinMode(PD2, INPUT_PULLUP);
  pinMode(PD3, INPUT_PULLUP);
  pinMode(PD4, INPUT_PULLUP);
  pinMode(PD5, INPUT_PULLUP);
  pinMode(PD6, INPUT_PULLUP);
}

bool PushButtons::GetD2() {
  return static_cast<bool>(digitalRead(PD2));
}

bool PushButtons::GetD3() {
  return static_cast<bool>(digitalRead(PD3));
}

bool PushButtons::GetD4() {
  return static_cast<bool>(digitalRead(PD4));
}

bool PushButtons::GetD5() {
  return static_cast<bool>(digitalRead(PD5));
}

bool PushButtons::GetD6() {
  return static_cast<bool>(digitalRead(PD6));
}