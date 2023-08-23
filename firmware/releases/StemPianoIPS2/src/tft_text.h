// gcz 2023
//
// This class is not hardware dependent.
//
// Text drawing on TFT display.

#ifndef TFT_TEXT_H_
#define TFT_TEXT_H_

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