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
// tft_text.cpp
//
// This class is not hardware dependent.
//
// Text drawing on TFT display.

#include "tft_text.h"

#ifdef TFT_INSTALLED

TftText::TftText() {}

void TftText::Setup(Adafruit_ILI9341 *tft, int display_height) {
  TftPointer_ = tft;
  display_height_ = display_height;
}

// mode: 0=wrap print, 1=no wrap print, 2=no wrap println.
// use x<0 and y<0 to avoid specifying.
void TftText::Print(char const *text, int x, int y, int color, int size, int mode) {
  if (mode == 0) {
    TftPointer_->setTextWrap(true);
  }
  else {
    TftPointer_->setTextWrap(false);
  }
  TftPointer_->setTextColor(color);
  TftPointer_->setTextSize(size);
  if (x >= 0 && y >= 0) {
    TftPointer_->setCursor(x, display_height_ - y);
  }
  if (mode == 0 || mode == 1) {
    TftPointer_->print(text);
  }
  else {
    TftPointer_->println(text);
  }
}

#endif