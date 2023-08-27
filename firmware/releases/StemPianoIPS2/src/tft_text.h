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
// tft_text.h
//
// This class is not hardware dependent.
//
// Text drawing on TFT display.

#ifndef TFT_TEXT_H_
#define TFT_TEXT_H_

#include "stem_piano_ips2.h"

#ifdef TFT_INSTALLED

#include <Adafruit_GFX.h>
#include "Adafruit_ILI9341.h"
#include <SPI.h>

class TftText
{

  public:

    TftText();
    void Setup(Adafruit_ILI9341 *, int);
    void Print(char const *, int, int, int, int, int);

  private:
    Adafruit_ILI9341 *TftPointer_;
    int display_height_;

};

#endif

#endif