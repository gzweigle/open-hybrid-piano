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
// dsp_damper.h
//
// This class is not hardware dependent.
//
// Damper digital signal processing

#ifndef DSP_DAMPER_H_
#define DSP_DAMPER_H_

#include "stem_piano_ips2.h"

// Using a #define because this is for statically allocated arrays.
#define NUM_DELAY_ELEMENTS 4

class DspDamper
{
  
  public:

    DspDamper();
    void Setup(float, float, int, int);
    void GetDamperEventData(bool *, float *, const float *);
    void Enable(bool);

  private:

    bool enable_;
    int debug_level_;

    float damper_threshold_;

    int event_block_counter_[NUM_CHANNELS];

    // Look back previous samples for computing velocity.
    float damper_buffer_[NUM_CHANNELS][NUM_DELAY_ELEMENTS];
    int buffer_index_;

    float velocity_scaling_;

    float samples_per_second_;

};

#endif