// gcz 2023

#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#include "dpp.h"
#include "switches.h"

class Diagnostics
{
  public:
    Diagnostics();
    void Setup(int, Switches *);
    void UpdateMaxValue(float *normalized_array);
    void ControlEthernetLed();
    void DisplayState();
    void SetTp11(bool);
    void SetTp10(bool);
    void SetTp9(bool);
    void SetTp8(bool);

  private:
    int display_interval_;
    int led_flash_count_;
    int serial_display_count_;
    Switches *Dip_;
 
};

#endif