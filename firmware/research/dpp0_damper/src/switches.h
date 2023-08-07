// gcz 2023

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "dpp.h"

class Switches
{
  public:
    Switches();
    void Setup(int);
    void updatePuDoState();
    bool read_switch_11();
    bool read_switch_12();
    bool read_switch_21();
    bool read_switch_22();
    bool read_switch_31();
    bool read_switch_32();
    bool read_switch_41();
    bool read_switch_42();

  private:

    void update_state(int, int *, int *);

    int pin_sw21_;
    int pin_sw22_;
    int pin_sw11_;
    int pin_sw12_;
    int pin_sw31_;
    int pin_sw32_;
    int pin_sw41_;
    int pin_sw42_;

    int switch_pu_do_setting_;

    int sw11_state_;
    int sw11_counter_;
    int sw12_state_;
    int sw12_counter_;
    int sw21_state_;
    int sw21_counter_;
    int sw22_state_;
    int sw22_counter_;
    int sw31_state_;
    int sw31_counter_;
    int sw32_state_;
    int sw32_counter_;
    int sw41_state_;
    int sw41_counter_;
    int sw42_state_;
    int sw42_counter_;

};

#endif