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
// hammer_crane.cpp
//
// This code runs on an Arduino Nano and controls a linear actuator.

#include <Arduino.h>

#include "linear_actuator.h"
#include "push_buttons.h"

LinearActuator LinAct;
PushButtons PushB;

// How long to hold at each level in milliseconds.
// This value must match t_hold in generate_analog_response.m.
int t_hold = 500;

// How long to transition between levels in milliseconds.
// This value must match t_tr in generate_analog_response.m.
int t_tr = 10;

// Time between button sampling, unless waiting t_hold or t_tr.
int t_wait = 10;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  LinAct.Setup(true);
  PushB.Setup();
}
void loop() {

  // Get linear actutor position. Not used.
  float ratio = LinAct.GetPositionRatio();

  // Get all pushbutton values.
  bool d2, d3, d4, d5, d6;
  d2 = PushB.GetD2();
  d3 = PushB.GetD3();
  d4 = PushB.GetD4();
  d5 = PushB.GetD5();
  d6 = PushB.GetD6();
  Serial.print(d2);
  Serial.print(" ");
  Serial.print(d3);
  Serial.print(" ");
  Serial.print(d4);
  Serial.print(" ");
  Serial.print(d5);
  Serial.print(" ");
  Serial.print(d6);
  Serial.print(" ");
  Serial.println(ratio);

  if (d2 == 0) {
    LinAct.Extend();
    delay(t_tr);
    digitalWrite(LED_BUILTIN, HIGH);
    LinAct.Stop();
    delay(t_hold);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (d3 == 0) {
    LinAct.Retract();
    delay(t_tr);
    digitalWrite(LED_BUILTIN, HIGH);
    LinAct.Stop();
    delay(t_hold);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (d4 == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    LinAct.Extend();
  }
  else if (d5 == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    LinAct.Retract();
  }
  else if (d6 == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    LinAct.Stop();
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    LinAct.Stop();
  }
  delay(t_wait);

}