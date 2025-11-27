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
// timing.cpp
//
// This class is not hardware dependent.
//
// Control how often something runs

#include "timing.h"

Timing::Timing() {}

void Timing::Setup(int processing_interval, int debug_level) {
  last_micros_ = micros();
  processing_interval_ = (unsigned long) processing_interval;
  debug_level_ = debug_level;

  // Checking for errors in processing interval.
  start_micros_ = micros();
  last_long_micros_ = micros();
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

void Timing::WarnOnProcessingInterval() {
  if (debug_level_ >= DEBUG_INFO) {
    unsigned long now_micros, delta_micros;
    now_micros = micros();
    delta_micros = now_micros - start_micros_;
    start_micros_ = now_micros;
    if (delta_micros > processing_interval_)
    {
      Serial.printf("Warning. Interval too long: %d us > %d us.",
      delta_micros, processing_interval_);
      Serial.printf(" Time since last: %d us.\n", micros() - last_long_micros_);
      last_long_micros_ = micros();
    }
  }
}