// gcz 2023
// For dpp pcb version 0.0

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "dpp.h"
#include <SPI.h>

class ConverterMuxed
{
  public:
    ConverterMuxed();
    void Setup(const int);
    void GetNewAdcValues(unsigned int *);
    void NormalizeAdcValues(float *, const unsigned int *);
 
  private:
    int sclk_frequency_;
    int pin_mux8_a_;
    int pin_mux8_b_;
    int pin_mux8_c_;
    int pin_mux16_s0_;
    int pin_mux16_s1_;
    int pin_mux16_s2_;
    int pin_mux16_s3_;
    int pin_adc_cs_;

};

#endif
