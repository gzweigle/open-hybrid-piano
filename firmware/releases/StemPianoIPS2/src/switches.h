// Copyright (C) 2024 Greg C. Zweigle
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
// switches.h
//
// This class is not hardware dependent.
//
// Read the input from a 2-input DIP switch.

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "stem_piano_ips2.h"

// Warning - there is no check that the name of a switch
// is smaller than this value. However, it seems extremely
// unlikely that... (famous last words).
#define MAX_SWITCH_NAME_LENGTH 200

class Switches
{
  public:
    Switches();
    void Setup(int, int, int, int);
    void updatePuDoState(const char *, const char *);
    bool read_switch_1();
    bool read_switch_2();
    bool direct_read_switch_1();
    bool direct_read_switch_2();

  private:

    void update_state(int, int *, unsigned long *, const char *);
    void set_pinmode_hardware_workaround(int);
    unsigned long switch_pu_do_interval_;
    unsigned long sw1_last_micros_;
    unsigned long sw2_last_micros_;
    bool startup_;
    int debug_level_;

    int pin_1_;
    int pin_2_;

    int sw1_state_;
    int sw2_state_;

};

#endif