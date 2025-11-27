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
// dsp_pedal.h
//
// This class is not hardware dependent.
//
// Pedal input processing

#ifndef DSP_PEDAL_H_
#define DSP_PEDAL_H_

#include "stem_piano_ips2.h"
#include "timing.h"

// Use #define since sizes memory.
#define NUM_PEDALS 3

class DspPedal
{
  public:
    DspPedal();
    void Setup(int, float, int, int, int, int, int, int, int);
    void UpdatePedalState(const float *);
    bool GetSustainCrossedDownThreshold();
    bool GetSustainCrossedUpThreshold();
    bool GetSostenutoCrossedDownThreshold();
    bool GetSostenutoCrossedUpThreshold();
    bool GetUnaCordaCrossedDownThreshold();
    bool GetUnaCordaCrossedUpThreshold();
    void Enable(bool);

  private:

    bool enable_;
    int debug_level_;

    bool IsSustainConnected(float);
    bool IsSostenutoConnected(float);
    bool IsUnaCordaConnected(float);
    int ComputeState(float, float, float);
    void UpdatePedalThresholds();
    void UpdateSustainMaxValue(float);
    void UpdateSostenutoMaxValue(float);
    void UpdateUnaCordaMaxValue(float);
    void DetectSustainFirstPress(float);
    void DetectSostenutoFirstPress(float);
    void DetectUnaCordaFirstPress(float);

    Timing Timing_;

    float pedal_threshold_;
    float initial_threshold_;
    float pedal_last_[NUM_PEDALS];
    bool connected_[NUM_PEDALS];
    int state_[NUM_PEDALS];

    // For dynamically learning the pedal thresholds.
    float max_position_[NUM_PEDALS];
    bool max_position_valid_[NUM_PEDALS];
    float threshold_[NUM_PEDALS];

    int sustain_pin_;
    int sostenuto_pin_;
    int una_corda_pin_;

    int sustain_connected_pin_;
    int sostenuto_connected_pin_;
    int una_corda_connected_pin_;

    enum Ind {
      sustain = 0,
      sostenuto = 1,
      una_corda = 2
    };

    enum State {
      no_change = 0,
      on_to_off = 1,
      off_to_on = 2
    };

};

#endif