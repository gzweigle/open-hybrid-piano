// gcz 2023
// For dpp pcb version 0.0

#include "dpp.h"
#include "converter_muxed.h"

// Test points and LEDs.
int pin_ethernet_ledya = 23;
int pin_tp11 = 5;
int pin_tp10 = 4;
int pin_tp9 = 3;
int pin_tp8 = 2;

// Two-input switches.
int pin_sw1_23 = 22;
int pin_sw1_14 = 21;
int pin_sw2_23 = 20;
int pin_sw2_14 = 19;
int pin_sw3_23 = 18;
int pin_sw3_14 = 17;
int pin_sw4_23 = 16;
int pin_sw4_14 = 15;

ConverterMuxed Adc;

void setup() {

  Serial.begin(9600);

  // This gets the interval to just over 4Ksps.
  Adc.Setup(20000000);

  // Setup test points, LEDs, and two-input switches.
  pinMode(pin_ethernet_ledya, OUTPUT); digitalWrite(pin_ethernet_ledya, LOW);
  pinMode(pin_sw1_23, INPUT); pinMode(pin_sw1_23, INPUT_PULLUP);
  pinMode(pin_sw1_14, INPUT); pinMode(pin_sw1_14, INPUT_PULLUP);
  pinMode(pin_sw2_23, INPUT); pinMode(pin_sw2_23, INPUT_PULLUP);
  pinMode(pin_sw2_14, INPUT); pinMode(pin_sw2_14, INPUT_PULLUP);
  pinMode(pin_sw3_23, INPUT); pinMode(pin_sw3_23, INPUT_PULLUP);
  pinMode(pin_sw3_14, INPUT); pinMode(pin_sw3_14, INPUT_PULLUP);
  pinMode(pin_sw4_23, INPUT); pinMode(pin_sw4_23, INPUT_PULLUP);
  pinMode(pin_sw4_14, INPUT); pinMode(pin_sw4_14, INPUT_PULLUP);
  pinMode(pin_tp11, OUTPUT); digitalWrite(pin_tp11, LOW);
  pinMode(pin_tp10, OUTPUT); digitalWrite(pin_tp10, LOW);
  pinMode(pin_tp9, OUTPUT); digitalWrite(pin_tp9, LOW);
  pinMode(pin_tp8, OUTPUT); digitalWrite(pin_tp8, LOW);

  Serial.println("Starting the test!");

}

int led_count = 0;

void loop() {

  unsigned int adc_array[NUM_CHANNELS];
  float normalized_array[NUM_CHANNELS];

  // Assert for oscilloscope to monitor during data acquisition cycle.
  digitalWrite(pin_tp8, HIGH);
  Adc.GetNewAdcValues(adc_array);
  digitalWrite(pin_tp8, LOW);

  Adc.NormalizeAdcValues(normalized_array, adc_array);

  led_count++;
  if (led_count == 1000) {
    // Make the LEDs flash so I know the board is ok.
    digitalWrite(pin_tp11, HIGH);
    digitalWrite(pin_tp10, HIGH);
    digitalWrite(pin_tp9, HIGH);
  }
  else if (led_count == 2000) {
    led_count = 0;
    digitalWrite(pin_tp11, LOW);
    digitalWrite(pin_tp10, LOW);
    digitalWrite(pin_tp9, LOW);
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

  // Testing using the following analog 16:1 multiplexer inputs:
  //    analog 0 = CNY70 output (using R1=50, R2=5600, Rvar=200)
  //    analog 1 = tied to ground
  //    analog 2 = tied to Vdd
  //    analog 3 = floating.
  //
  // Results:
  // CNY70 d = 0
  //    CNY70 output = 3832  low analog = 0  high analog = 4095  floating analog = 3951
  // CNY70 d = max
  //    CNY70 output = 261  low analog = 0  high analog = 4095  floating analog = 3950
  //
  // TODO - R2=5600 maybe is too large as CNY70 out of linear range as d->0?

}
