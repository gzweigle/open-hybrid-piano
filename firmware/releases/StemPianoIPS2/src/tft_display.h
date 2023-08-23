// gcz, 2023
//
// For ips pcb version 2.X
//
// Control of 2.8" TFT display

#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_FT6206_Wire2.h"

#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ImageReader.h>

#include "stem_piano_ips2.h"
#include "tft_text.h"

class TftDisplay
{
  public:
    TftDisplay();
    void Setup(bool);
    void StartupDisplay();
    void Display(bool, const float *, const float *);

  private:
    void Clear();
    void GetTouchPosition(int *, int *);
    void Picture();
    int ReturnColor(unsigned int, unsigned int, unsigned int);
    void PrintTouch(int, int, int, int, int, int);
    void PrintType(bool, int, int, int, int);
    void PrintKey(int, int, int, int, int);
    void PrintValue(float, int, int, int, int);

    TftText Text_;

    Adafruit_ILI9341 *Tft_;
    Adafruit_FT6206_Wire2 *Ts_;
    Adafruit_ImageReader Reader_;
    SdFat SD_;

    bool using_display_;
    bool startup_;
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

};

#endif