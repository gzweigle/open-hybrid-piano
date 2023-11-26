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
// dsp_hammer.h
//
// This class is not hardware dependent.
//
// Hammer digital signal processing

#ifndef DSP_HAMMER_H_
#define DSP_HAMMER_H_

#include "stem_piano_ips2.h"

// Using a #define because this is used for statically allocated arrays.
#define DERIVATIVE_AVERAGE_SAMPLES 8

class DspHammer
{
  public:
    DspHammer();
    void Setup(int, float, float, float, float, float, int, int);
    void GetHammerEventData(bool *, float *, const float *);
    void Enable(bool);

  private:

    void ComputeDerivative(const float *);
    void UpdateHammerRange(const float *);
    void UpdateMaxHammerVelocity();
    void UpdateHammerThresholds();
    void DetectHammerStrike(bool *, float *, const float *);
    void DetectFirstHammer(const bool *);
    void DetectReleased(const float *);

    bool enable_;
    int debug_level_;

    // Variables related to determining when the hammer hit the string.
    int samples_per_second_;
    float strike_threshold_;
    float release_threshold_;
    int min_repetition_samples_;
    int repetition_counter_[NUM_CHANNELS];
    bool released_[NUM_CHANNELS];

    // Let everything settle out before running and assigning internal state.
    // Without this, for example, the min value can get stuck at a false value
    // of zero and then hammer never release.
    int initialize_counter_;
    int initialize_wait_count_;

    // This will become the velocity reported via MIDI.
    float max_velocity_[NUM_CHANNELS];
    float min_strike_velocity_;

    // For dynamically learning the position where hammer hits strings.
    float max_position_[NUM_CHANNELS];
    bool min_max_position_valid_[NUM_CHANNELS];
    float hammer_threshold_[NUM_CHANNELS];

    // Derivative of hammer position, x.
    float velocity_[NUM_CHANNELS];
    float velocity_buffer_[NUM_CHANNELS][DERIVATIVE_AVERAGE_SAMPLES];
    unsigned int buffer_pointer_;

    // Measure this on the physical action.
    float hammer_travel_meters_;
  
};

#endif
