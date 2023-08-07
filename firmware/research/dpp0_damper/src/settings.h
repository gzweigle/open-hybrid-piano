// gcz 2023
// Configuration parameters.

#ifndef SETTINGS_H
#define SETTINGS_H

#include "dpp.h"

class Settings
{
  
  public:

    Settings();
    int serial_baud_rate;
    int adc_spi_clock_frequency;
    int adc_sample_period_microseconds;
    int switch_pickup_dropout_samples;
    int serial_display_interval_samples;

};

#endif