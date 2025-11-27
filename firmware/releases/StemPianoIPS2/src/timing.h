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
// timing.h
//
// This class is not hardware dependent.
//
// Control how often something runs

#ifndef TIMING_H_
#define TIMING_H_

#include "stem_piano_ips2.h"

class Timing
{
  public:
    Timing();
    void Setup(int, int);
    bool AllowProcessing();
    void ResetInterval(int);
    void WarnOnProcessingInterval();
 
  private:
    int debug_level_;
    unsigned long last_micros_;
    unsigned long processing_interval_;
    unsigned long start_micros_;
    unsigned long last_long_micros_;

};

#endif
