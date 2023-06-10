// gcz 2023
// For dpp pcb version 0.0

#include "dpp.h"
#include "converter_muxed.h"
#include "diagnostics.h"
#include "switches.h"

ConverterMuxed Adc;
Diagnostics Diag;
Switches Switch;

void setup() {

  Serial.begin(9600);
  Adc.Setup(20000000);
  Switch.Setup(10);
  Diag.Setup(1000, &Switch);

  Serial.println("Hello world of piano dampers.");

}

int led_count = 0;

void loop() {

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
  if (led_count == 1000) {
    // Make the LEDs flash so I know the board is ok.
    Diag.SetTp11(true);
    Diag.SetTp10(true);
    Diag.SetTp9(true);
  }
  else if (led_count == 2000) {
    led_count = 0;
    Diag.SetTp11(false);
    Diag.SetTp10(false);
    Diag.SetTp9(false);
    // Display ADC output for two of the analog inputs.
    Serial.print("CNY70 output = ");
    Serial.print(adc_array[0]);
    Serial.print("  low analog = ");
    Serial.print(adc_array[1]);
    Serial.print("  high analog = ");
    Serial.print(adc_array[2]);
    Serial.print("  floating analog = ");
    Serial.println(adc_array[3]);
  }

}
