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
// tft_display.cpp
//
// For ips pcb version 2.X
//
// Control of 2.8" TFT display
//
// TODO - Lots of cool possibilities for the display.
// TODO - Could use during real-time by writing just one value at a time?
// TODO - Instead of -8, have a setting for the pedal inputs.

#include "tft_display.h"

#ifdef TFT_INSTALLED

TftDisplay::TftDisplay() : Reader_(SD_) {}

void TftDisplay::Setup(bool using_display, int debug_level) {

  debug_level_ = debug_level;

  using_display_ = using_display;
  startup_ = true;
  tft_switch_last_ = false;

  // Hardcoded based on pcb layout.
  cs_ = 27;        // Chip select for graphics display.
  sd_cs_ = 33;     // Chip select for images on SD card.
  dc_ = 10;        // Control signal for graphics display.
  reset_ = 9;      // To reset the board, assert low.
  backlight_ = 7;  // PWM for backlight intensity.
  sd_detect_ = 34; // Pulled high if a SD card is detected.

  // Do not change - these are based on the 2.8" hardware.
  screen_rotation_ = 1;
  width_ = 320;
  height_ = 240;

  if (using_display_ == true) {

    // TFT backlight.  High = maximum brightness.
    pinMode(backlight_, OUTPUT);
    digitalWrite(backlight_, HIGH);

    // TFT reset pin and do not reset.
    pinMode(reset_, OUTPUT);
    digitalWrite(reset_, HIGH);

    // SD card detect.
    pinMode(sd_detect_, INPUT);

    // This sets up the SPI interface for display as well as SD card.
    Tft_ = new Adafruit_ILI9341(cs_, dc_);
    Ts_ = new Adafruit_FT6206();

    Tft_->begin();
    Tft_->fillScreen(ILI9341_BLACK);
    Tft_->setRotation(screen_rotation_);
    Tft_->setTextWrap(false);
    Tft_->setTextSize(3);

    // Internally instantiated classes.
    Text_.Setup(Tft_, height_);

    // Touch screen.
    bool ts_status = Ts_->begin(FT62XX_DEFAULT_THRESHOLD, &Wire2);
    if (ts_status == false) {
      using_display_ = false;
      if (debug_level_ >= 1) {
        Serial.println("TFT display failed, do not know why.");
        Serial.println("TFT display will not be used.");
      }
    }
    else {
      if (digitalRead(sd_detect_) == HIGH) {
        sd_card_detected_ = true;
        if (!SD_.begin(sd_cs_, SD_SCK_MHZ(10))) {
          sd_card_started_ = false;
          if (debug_level_ >= 1) {
            Serial.println("Could not start SD card.");
          }
        }
        else {
          sd_card_started_ = true;
          if (debug_level_ >= 1) {
            Serial.println("Successfully started SD card.");
          }
        }
      }
      else {
        sd_card_detected_ = false;
        sd_card_started_ = false;
        if (debug_level_ >= 1) {
          Serial.println("No card detected for displaying images.");
        }
      }
    }
  }
}

void TftDisplay::HelloWorld() {
  if (using_display_ == true) {
    Clear();
    Tft_->setTextColor(ILI9341_WHITE);
    Tft_->setCursor(15, 64);
    Tft_->print("hello world of");
    Tft_->setCursor(15, 64+28);
    Tft_->print("stem piano");
    Tft_->setCursor(15, 64+56);
    Tft_->print("by greg zweigle");
    //delay(1500);
    //Clear();
  }
}

void TftDisplay::Clear() {
  if (using_display_ == true) {
    Tft_->fillScreen(ILI9341_BLACK);
  }
}

// Display the following when in TFT mode:
//
// (x,y) pair of where touching the screen.
//
// The maximum value over all piano keys:
//   Whether a hammer or damper was largest.
//   Which key was the max value.
//   The maximum value.
// Why is this useful?
//   Because then can push one key at a time and
//   get a display of the largest value read out
//   from the sensor for that key.
//   Use to check sensor physical position and / or calibrate sensor.
void TftDisplay::Display(bool tft_switch, const float *hp, const float *dp) {
  static float max_value_last = 0.0;
  static int max_key_last = 0;
  static bool was_hammer_last = true;
  static int tx_last = 0;
  static int ty_last = 0;
  if (using_display_ == true) {
    if (tft_switch == true) {

      if (tft_switch_last_ == false) {
        HelloWorld();
      }

      int color;
      color = ReturnColor(50, 82, 123);  // Blue is for the sky.
      Text_.Print("diy hybrid grand piano gcz", 0, height_, color, 2, 2);

      float max_value = 0.0;
      int max_key = 0;
      bool was_hammer = true;
      int tx = 0;
      int ty = 0;
      // Instead of -8 should track which are pedal.
      for (int key = 0; key < NUM_CHANNELS - 8; key++) {
        if (hp[key] > max_value) {
          max_value = hp[key];
          max_key = key;
          was_hammer = true;
        }
        if (dp[key] > max_value) {
          max_value = dp[key];
          max_key = key;
          was_hammer = false;
        }
      }

      // Write black first to erase the previous value from the screen.

      // Red is for sunsets and sunrises.
      GetTouchPosition(&tx, &ty);
      color = ReturnColor(0, 0, 0);      PrintTouch(tx_last, ty_last, 0, 175, color, 2);
      color = ReturnColor(153, 30, 53);  PrintTouch(tx,      ty,      0, 175, color, 2);

      // Purple because the proto boards were purple.
      color = ReturnColor(0, 0, 0);      PrintType(was_hammer_last, 0, 100, color, 2);
      color = ReturnColor(186, 85, 255); PrintType(was_hammer,      0, 100, color, 2);

      // Green because the Teensy is green.
      color = ReturnColor(0, 0, 0);     PrintKey(max_key_last, 0, 75, color, 2);
      color = ReturnColor(79, 121, 66); PrintKey(max_key,      0, 75, color, 2);

      // Yellow is for sunshine.
      color = ReturnColor(0, 0, 0);      PrintValue(max_value_last, 0, 50, color, 3);
      color = ReturnColor(218, 165, 32); PrintValue(max_value,      0, 50, color, 3);

      tx_last = tx;
      ty_last = ty;
      max_value_last = max_value;
      max_key_last = max_key;
      was_hammer_last = was_hammer;

      // If touch the lower right corner, display an image for 0.5 seconds!
      if (tx > 250 && ty < 50) {
        Picture();
        delay(500);
        Clear();
      }

    }
    else {
      // Done with TFT so clear the screen to avoid burn-in.
      // Clearing takes quite a bit of time, so only do this once.
      /// TEMPORARY!!!
      ///
      //if (tft_switch_last_ == true || startup_ == true) {
        //Clear();
      //}
    }
    startup_ = false;
  }
  tft_switch_last_ = tft_switch;
}

// Small helper functions to keep code above cleaner.

void TftDisplay::PrintTouch(int tx, int ty, int x, int y, int color, int size) {
  char print_string[100];
  snprintf(print_string, 100, "touch(x,y) = (%d,%d)", tx, ty);
  Text_.Print(print_string, x, y, color, size, 2);
}

void TftDisplay::PrintType(bool was_hammer, int x, int y, int color, int size) {
  if (was_hammer == true)
    Text_.Print("value is for a hammer", x, y, color, size, 2);
  else
    Text_.Print("value is for a damper", x, y, color, size, 2);
}

void TftDisplay::PrintKey(int key, int x, int y, int color, int size) {
  char print_string[100];
  snprintf(print_string, 100, "Key = %d", key);
  Text_.Print(print_string, x, y, color, size, 2);
}

void TftDisplay::PrintValue(float max, int x, int y, int color, int size) {
  char print_string[100];
  snprintf(print_string, 100, "Value = %2.3f", max);
  Text_.Print(print_string, x, y, color, size, 2);
}

int TftDisplay::ReturnColor(unsigned int red, unsigned int green, unsigned int blue)
{
  return (((red&0x0FF)>>3)<<11) | (((green&0x0FF)>>2)<<5) | ((blue&0x0FF)>>3);
}

// Get touch position. Notice x and y read back are swapped so that
// (0,0) is the lower left according to how screen is installed on pcb.
void TftDisplay::GetTouchPosition(int *x, int *y) {
  if (using_display_ == true) {
    TS_Point touch_point;
    if (Ts_->touched()) {
      touch_point = Ts_->getPoint();
      *x = width_ - touch_point.y;
      *y = height_ - touch_point.x;
      if (debug_level_ >= 2) {
        Serial.print("Touchscreen position (x=");
        Serial.print(*x);
        Serial.print(",y=");
        Serial.print(*y);
        Serial.println(")");
      }
    }
  }
}

// Display pictures used in one of the YouTube videos.
// Each file must be 240x320 pixels in 24-bit BMP format.
// Images smaller than 240x320 are allowed too.
// The .bmp in code below is in the repository.
// These images need to be copied onto a microSD card and
// inserted into the 2.8" TFT display.
void TftDisplay::Picture() {
  static int picture_number = 0;
  if (using_display_ == true && sd_card_detected_ == true) {
    if (sd_card_started_ == false) {
      // Depending on power sequencing, sometimes the card will
      // not be detected at startup. If that happens, try again.
      // The following code usually fixes things.
      SD_.end();
      if (!SD_.begin(sd_cs_, SD_SCK_MHZ(10))) {
        sd_card_started_ = false;
        if (debug_level_ >= 1) {
          Serial.println("Continue to cannot start SD card.");
        }
      }
      else {
        sd_card_started_ = true;
        if (debug_level_ >= 1) {
          Serial.println("Started SD card after multiple tries.");
        }
      }
    }
    if (sd_card_started_ == true) {
      ImageReturnCode stat;
      if (debug_level_ >= 2) {
        Serial.println("New TFT picture.");
      }
      if (picture_number == 0) {
        stat = Reader_.drawBMP("/diy_16_thumbnail.bmp", *Tft_, 0, 0);
        picture_number++;
      }
      else if (picture_number == 1) {
        stat = Reader_.drawBMP("/diy_19_thumbnail.bmp", *Tft_, 0, 0);
        picture_number++;
      }
      else if (picture_number == 2) {
        stat = Reader_.drawBMP("/diy_20_thumbnail.bmp", *Tft_, 0, 0);
        picture_number++;
      }
      else if (picture_number == 3) {
        stat = Reader_.drawBMP("/diy_23_thumbnail.bmp", *Tft_, 0, 0);
        picture_number++;
      }
      else {
        stat = Reader_.drawBMP("/diy_24_thumbnail.bmp", *Tft_, 0, 0);
        picture_number = 0;
      }
      if (debug_level_ >= 2) {
        Serial.println("TftDisplay::Picture()");
        Serial.println(stat);
      }
    }
  }
}

#else

TftDisplay::TftDisplay() {}

void TftDisplay::Setup(bool not_used, int debug_level) {
  if (debug_level >= 1) {
    Serial.println("TFT Display is not in build and is not used.");
  }
}

void TftDisplay::HelloWorld() {}

void TftDisplay::Display(bool a, const float *b, const float *c) {}

#endif