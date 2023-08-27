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
// timing.cpp
//
// This class is not hardware dependent.
//
// Control how often something runs

#include "timing.h"

Timing::Timing() {}

void Timing::Setup(int processing_interval) {
  last_micros_ = micros();
  processing_interval_ = (unsigned long) processing_interval;
}

// Use a separate function for changing the processing interval
// during runtime because Setup() functions are reserved for init.
void Timing::ResetInterval(int processing_interval) {
  processing_interval_ = (unsigned long) processing_interval;
}

bool Timing::AllowProcessing() {
  if (micros() - last_micros_ > processing_interval_) {
    last_micros_ = micros();
    return true;
  }
  else {
    return false;
  }
}