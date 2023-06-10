// gcz 2023
// For dpp pcb version 0.0

#include "dpp.h"
#include "converter_muxed.h"
#include "diagnostics.h"
#include "settings.h"
#include "switches.h"

ConverterMuxed Adc;
Diagnostics Diag;
Settings Set;
Switches Switch;

void setup() {

  Serial.begin(Set.serial_baud_rate);
  Adc.Setup(Set.adc_spi_clock_frequency);
  Switch.Setup(Set.switch_pickup_dropout_samples);
  Diag.Setup(Set.serial_display_interval_samples, &Switch);

  Serial.println("Hello world of piano dampers.");

}

int led_count = 0;
unsigned long last_micros = micros();

void loop() {

  // Run every millisecond.
  if (micros() - last_micros > (unsigned long) Set.adc_sample_period_microseconds) {
    last_micros = micros();

    unsigned int adc_array[NUM_CHANNELS];
    float normalized_array[NUM_CHANNELS];

    Switch.updatePuDoState();
    Diag.DisplayState();
    Diag.ControlEthernetLed();

    // Assert for oscilloscope to monitor during data acquisition cycle.
    Diag.SetTp8(true);
    Adc.GetNewAdcValues(adc_array);
    Diag.SetTp8(false);

    Adc.NormalizeAdcValues(normalized_array, adc_array);

    led_count++;
    if (led_count == Set.serial_display_interval_samples) {
      // Make the LEDs flash so I know the board is ok.
      Diag.SetTp11(true);
      Diag.SetTp10(true);
      Diag.SetTp9(true);
    }
    else if (led_count == 2*Set.serial_display_interval_samples) {
      led_count = 0;
      Diag.SetTp11(false);
      Diag.SetTp10(false);
      Diag.SetTp9(false);
      // Display ADC output for two of the analog inputs.
      Serial.print("  CNY70 = ");
      Serial.print(adc_array[0]);
      Serial.print("  low = ");
      Serial.print(adc_array[1]);
      Serial.print("  high = ");
      Serial.print(adc_array[2]);
      Serial.print("  float = ");
      Serial.println(adc_array[3]);
    }
  }
}
