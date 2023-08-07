// gcz 2023
// Common location for all settings related to the system.

#include "settings.h"

Settings::Settings() {

  // At adc_spi_clock_frequency of 20e6, sampling
  // all 88 inputs takes approximately 240us.
  // So, adc_sample_period_microseconds must be longer.
  adc_spi_clock_frequency = 20000000;     // Max 20e6 per ADC datasheet.
  adc_sample_period_microseconds = 1000;  // 1Ksps.

  // Noncritical values for display and LED.
  serial_baud_rate = 9600;
  switch_pickup_dropout_samples = 10;
  serial_display_interval_samples = 1000;

}