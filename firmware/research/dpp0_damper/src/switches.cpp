// gcz 2023
//
// TODO - Convert to list or array or something besides the copy/paste approach.

#include "switches.h"

Switches::Switches() {}

void Switches::Setup(int switch_pu_do_setting) { 

  pin_sw12_ = 22;
  pin_sw11_ = 21;
  pin_sw22_ = 20;
  pin_sw21_ = 19;
  pin_sw32_ = 18;
  pin_sw31_ = 17;
  pin_sw42_ = 16;
  pin_sw41_ = 15;

  pinMode(pin_sw11_, INPUT); pinMode(pin_sw11_, INPUT_PULLUP);
  pinMode(pin_sw12_, INPUT); pinMode(pin_sw12_, INPUT_PULLUP);
  pinMode(pin_sw21_, INPUT); pinMode(pin_sw21_, INPUT_PULLUP);
  pinMode(pin_sw22_, INPUT); pinMode(pin_sw22_, INPUT_PULLUP);
  pinMode(pin_sw31_, INPUT); pinMode(pin_sw31_, INPUT_PULLUP);
  pinMode(pin_sw32_, INPUT); pinMode(pin_sw32_, INPUT_PULLUP);
  pinMode(pin_sw41_, INPUT); pinMode(pin_sw41_, INPUT_PULLUP);
  pinMode(pin_sw42_, INPUT); pinMode(pin_sw42_, INPUT_PULLUP);

  switch_pu_do_setting_ = switch_pu_do_setting;

  sw11_state_ = digitalRead(pin_sw11_); sw11_counter_ = 0;
  sw12_state_ = digitalRead(pin_sw12_); sw12_counter_ = 0;
  sw21_state_ = digitalRead(pin_sw21_); sw21_counter_ = 0;
  sw22_state_ = digitalRead(pin_sw22_); sw22_counter_ = 0;
  sw31_state_ = digitalRead(pin_sw31_); sw31_counter_ = 0;
  sw32_state_ = digitalRead(pin_sw32_); sw32_counter_ = 0;
  sw41_state_ = digitalRead(pin_sw41_); sw41_counter_ = 0;
  sw42_state_ = digitalRead(pin_sw42_); sw42_counter_ = 0;

}

void Switches::updatePuDoState() {
  int sw_value;
  sw_value = digitalRead(pin_sw11_);
  update_state(sw_value, &sw11_state_, &sw11_counter_);
  sw_value = digitalRead(pin_sw12_);
  update_state(sw_value, &sw12_state_, &sw12_counter_);
  sw_value = digitalRead(pin_sw21_);
  update_state(sw_value, &sw21_state_, &sw21_counter_);
  sw_value = digitalRead(pin_sw22_);
  update_state(sw_value, &sw22_state_, &sw22_counter_);
  sw_value = digitalRead(pin_sw31_);
  update_state(sw_value, &sw31_state_, &sw31_counter_);
  sw_value = digitalRead(pin_sw32_);
  update_state(sw_value, &sw32_state_, &sw32_counter_);
  sw_value = digitalRead(pin_sw41_);
  update_state(sw_value, &sw41_state_, &sw41_counter_);
  sw_value = digitalRead(pin_sw42_);
  update_state(sw_value, &sw42_state_, &sw42_counter_);
}

// When 'on', switches get pulled to ground, so return opposite state.

bool Switches::read_switch_11() {
  if (sw11_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_12() {
  if (sw12_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_21() {
  if (sw21_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_22() {
  if (sw22_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_31() {
  if (sw31_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_32() {
  if (sw32_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_41() {
  if (sw41_state_ == 0)
    return true;
  else
    return false;
}
bool Switches::read_switch_42() {
  if (sw42_state_ == 0)
    return true;
  else
    return false;
}


// Pick-up / Drop-out logic to eliminate transients.
// Must run every time in loop().
void Switches::update_state(int dip_value, int *dip_state, int *dip_counter) {
  if (*dip_state == 0) {
    if (dip_value == HIGH) {
      (*dip_counter)++;
    }
    else {
      *dip_counter = 0;
    }
    if (*dip_counter == switch_pu_do_setting_) {
      *dip_state = 1;
      *dip_counter = 0;
    }
  }
  else {
    if (dip_value == LOW) {
      (*dip_counter)++;
    }
    else {
      *dip_counter = 0;
    }
    if (*dip_counter == switch_pu_do_setting_) {
      *dip_state = 0;
      *dip_counter = 0;
    }
  }
}