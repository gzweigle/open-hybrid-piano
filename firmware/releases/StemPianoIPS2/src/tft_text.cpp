// gcz 2023
//
// This class is not hardware dependent.
//
// Text drawing on TFT display.

#include "tft_text.h"

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