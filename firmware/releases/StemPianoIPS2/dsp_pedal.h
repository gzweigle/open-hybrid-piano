// gcz 2023
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
    void Setup(int, float, int, int, int, int, int, int);
    void UpdatePedalState(const float *);
    bool GetSustainCrossedDownThreshold();
    bool GetSustainCrossedUpThreshold();
    bool GetSostenutoCrossedDownThreshold();
    bool GetSostenutoCrossedUpThreshold();
    bool GetUnaCordaCrossedDownThreshold();
    bool GetUnaCordaCrossedUpThreshold();

  private:

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