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
// linear_actuator.h
//

#ifndef LINEAR_ACTUATOR_H_
#define LINEAR_ACTUATOR_H_

#include <Arduino.h>

class LinearActuator
{
  public:
    LinearActuator();
    void Setup(bool);
    float GetPositionRatio();
    void Extend();
    void Retract();
    void Stop();

  private:
    bool arduino_workaround_;
 
};

#endif