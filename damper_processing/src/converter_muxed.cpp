// gcz 2023
// For dpp pcb version 0.0
//
// See the Texas Instruments ADCS7476 12-bit, 1Msps, successive
// approximation, with internal track-and-hold.
// Code is to the May 2016 version of the datasheet.
//
// ADC clocks data out on the falling edge.
// Using SPI Mode 0 (CPOL=0 and CPHA=0)

#include "converter_muxed.h"

ConverterMuxed::ConverterMuxed() {}

void ConverterMuxed::Setup(const int sclk_frequency) {

  // 8:1 analog multiplexer.
  pin_mux8_a_ = 41;
  pin_mux8_b_ = 40;
  pin_mux8_c_ = 39;

  // 16:1 analog multiplexer.
  pin_mux16_s0_ = 38;
  pin_mux16_s1_ = 37;
  pin_mux16_s2_ = 36;
  pin_mux16_s3_ = 35;
  pin_adc_cs_ = 14;

  sclk_frequency_ = sclk_frequency;

  pinMode(pin_mux8_a_, OUTPUT); digitalWrite(pin_mux8_a_, LOW);
  pinMode(pin_mux8_b_, OUTPUT); digitalWrite(pin_mux8_b_, LOW);
  pinMode(pin_mux8_c_, OUTPUT); digitalWrite(pin_mux8_c_, LOW);
  pinMode(pin_mux16_s0_, OUTPUT); digitalWrite(pin_mux16_s0_, LOW);
  pinMode(pin_mux16_s1_, OUTPUT); digitalWrite(pin_mux16_s1_, LOW);
  pinMode(pin_mux16_s2_, OUTPUT); digitalWrite(pin_mux16_s2_, LOW);
  pinMode(pin_mux16_s3_, OUTPUT); digitalWrite(pin_mux16_s3_, LOW);
  pinMode(pin_adc_cs_, OUTPUT); digitalWrite(pin_adc_cs_, HIGH);

  SPI.begin();

}

// Mux positions are based on PCB wiring.
void ConverterMuxed::GetNewAdcValues(unsigned int *adc_array) {

  int adc_ind = 0;

  // Select one output from the 8:1 mux at a time.
  for (int mux8 = 0; mux8 < 6; mux8++) {

    switch(mux8) {
      case 0:
        digitalWrite(pin_mux8_a_, LOW);
        digitalWrite(pin_mux8_b_, HIGH);
        digitalWrite(pin_mux8_c_, LOW);
        break;
      case 1:
        digitalWrite(pin_mux8_a_, HIGH);
        digitalWrite(pin_mux8_b_, LOW);
        digitalWrite(pin_mux8_c_, LOW);
        break;
      case 2:
        digitalWrite(pin_mux8_a_, LOW);
        digitalWrite(pin_mux8_b_, LOW);
        digitalWrite(pin_mux8_c_, LOW);
        break;
      case 3:
        digitalWrite(pin_mux8_a_, HIGH);
        digitalWrite(pin_mux8_b_, HIGH);
        digitalWrite(pin_mux8_c_, LOW);
        break;
      case 4:
        digitalWrite(pin_mux8_a_, HIGH);
        digitalWrite(pin_mux8_b_, LOW);
        digitalWrite(pin_mux8_c_, HIGH);
        break;
      case 5:
        digitalWrite(pin_mux8_a_, HIGH);
        digitalWrite(pin_mux8_b_, HIGH);
        digitalWrite(pin_mux8_c_, HIGH);
        break;
    }

    // Cycle through all 16 inputs for all 6 16:1 mux.
    for (int mux16 = 0; mux16 < 16; mux16++) {

      if ((mux16&1) == 0) digitalWrite(pin_mux16_s0_, LOW);
      else                digitalWrite(pin_mux16_s0_, HIGH);
      if ((mux16&2) == 0) digitalWrite(pin_mux16_s1_, LOW);
      else                digitalWrite(pin_mux16_s1_, HIGH);
      if ((mux16&4) == 0) digitalWrite(pin_mux16_s2_, LOW);
      else                digitalWrite(pin_mux16_s2_, HIGH);
      if ((mux16&8) == 0) digitalWrite(pin_mux16_s3_, LOW);
      else                digitalWrite(pin_mux16_s3_, HIGH);

      // Saftey check while code is in development.
      if (adc_ind >= NUM_CHANNELS) {
        Serial.println("adc_ind out of range.");
        adc_ind = NUM_CHANNELS - 1;
      }

      // Get one ADC value, save it.
      delayMicroseconds(1);  // Settle time after the analog mux changes.
      SPI.beginTransaction(SPISettings(sclk_frequency_, MSBFIRST, SPI_MODE0));
      digitalWrite(pin_adc_cs_, LOW);
      adc_array[adc_ind++] = SPI.transfer16(0);
      digitalWrite(pin_adc_cs_, HIGH);
      SPI.endTransaction();

    }
  }
}

// Normalize to [0.0,1.0], where 1.0 is the max ADC value.
// Normalization is for a 12-bit ADC.
void ConverterMuxed::NormalizeAdcValues(float *normalized_values,
const unsigned int *adc_values) {
  for (unsigned int i = 0; i < NUM_CHANNELS; i++) {
    normalized_values[i] = ((float) adc_values[i]) / 4095.0;
  }
}