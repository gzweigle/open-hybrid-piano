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
// tft_display.h
//
// For ips pcb version 2.X
//
// Control of 2.8" TFT display

#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_

#include "stem_piano_ips2.h"

#ifdef TFT_INSTALLED

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_FT6206.h"

#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ImageReader.h>

#include "tft_text.h"

class TftDisplay
{
  public:
    TftDisplay();
    void Setup(bool, int);
    void HelloWorld();
    void Clear();
    void Display(bool, const float *, const float *);

  private:
    void GetTouchPosition(int *, int *);
    void Picture();
    int ReturnColor(unsigned int, unsigned int, unsigned int);
    void PrintTouch(int, int, int, int, int, int);
    void PrintType(bool, int, int, int, int);
    void PrintKey(int, int, int, int, int);
    void PrintValue(float, int, int, int, int);
    void LiveDraw();

    TftText Text_;

    Adafruit_ILI9341 *Tft_;
    Adafruit_FT6206 *Ts_;
    Adafruit_ImageReader Reader_;
    SdFat SD_;

    int debug_level_;
    bool using_display_;
    bool tft_switch_last_;

    int cs_;
    int dc_;
    int backlight_;
    int reset_;
    int sd_cs_;
    int sd_detect_;

    unsigned int screen_rotation_;
    int width_;
    int height_;

    bool sd_card_started_;
    bool sd_card_detected_;

    int live_draw_x_, live_draw_y_, live_draw_l_;
    unsigned long live_last_time_, live_wait_time_;
    bool live_draw_state_;

};

#else

class TftDisplay
{
  public:
    TftDisplay();
    void Setup(bool, int);
    void HelloWorld();
    void Display(bool, const float *, const float *);
    void Clear();

};

#endif

#endif