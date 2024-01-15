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
// push_buttons.h
//

#ifndef PUSH_BUTTONS_H_
#define PUSH_BUTTONS_H_

#include <Arduino.h>

class PushButtons
{
  public:
    PushButtons();
    void Setup();
    bool GetD2();
    bool GetD3();
    bool GetD4();
    bool GetD5();
    bool GetD6();
 
};

#endif