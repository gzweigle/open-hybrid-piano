// gcz 2023
// For ips pcb version 2.0

#include "damper_settings.h"

#include "stem_piano_ips.h"

#include "six_channel_analog_00.h"
#include "board2board.h"
#include "network.h"
#include "switches.h"
#include "testpoint_led.h"
#include "timing.h"

DamperSettings Set;

SixChannelAnalog00 Adc;
Board2Board B2B;
Network Eth;
Switches Swi;
TestpointLed Tpl;
Timing Tmg;

void setup(void) {

  // This must be placed first and in the setup() function.
  Set.SetAllSettingValues();

  Serial.begin(Set.serial_baud_rate);
  Serial.println("Beginning damper board initialization.");

  // Setup everything.
  Swi.Setup(Set.switch_pickup_dropout_samples);
  Adc.Setup(Set.adc_spi_clock_frequency, Set.adc_settling_time_nanoseconds,
  Set.adc_input_invert, &Tpl);
  B2B.Setup(Set.canbus_baud_rate);
  Eth.Setup(Set.teensy_ip, Set.upd_port);
  Tpl.Setup(Set.serial_display_interval_samples);
  Tmg.Setup(Set.adc_sample_period_microseconds);

  Serial.println("Hello world of piano dampers.");

}

int led_count = 0;

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];   // NOT ordered by piano note order.
int damper_position_adc_counts[NUM_CHANNELS];

// Same as damper_position_adc_counts except floating point values.
float damper_position[NUM_CHANNELS];

void loop() {

  // This if statement determines how often the code inside loop() runs.
  if (Tmg.AllowProcessing() == true) {
    Tpl.SetTp8(true);

    // Every processing interval, update the setup / hold
    // for measuring the DIP switch state.
    Swi.updatePuDoState();

    // Get ADC data and send over the board-to-board interface.
    Adc.GetNewAdcValues(raw_samples);
    Adc.NormalizeAdcValues(damper_position_adc_counts, damper_position, raw_samples);
    for (int k = 0; k < NUM_CHANNELS; k++) {
      if (Set.connected_channel[k] == false) {
        damper_position_adc_counts[k] = 0;
        damper_position[k] = 0.0;
      }
    }
    B2B.SendDamperData(damper_position);

    // Sending data over Ethernet.
    //Eth.SendPianoPacket(damper_position_adc_counts[41], damper_position_adc_counts[43]);

    /////////////////
    // Temporary debug and diagnostic stuff.
    led_count++;
    if (led_count == Set.serial_display_interval_samples/2) {
      Tpl.SetTp10(false);
      Tpl.SetTp11(true);
      Tpl.SetLowerRightLED(false);
      Tpl.SetEthernetLED(false);
    }
    else if (led_count == Set.serial_display_interval_samples) {
      led_count = 0;
      // Display ADC output for two of the analog inputs.
      Serial.print("Damper Board ");
      Serial.print(" 40="); Serial.print(damper_position[40]);
      Serial.print(" 41="); Serial.print(damper_position[41]);
      Serial.println("");
      Tpl.SetTp10(true);
      Tpl.SetTp11(false);
      Tpl.SetLowerRightLED(true);
      Tpl.SetEthernetLED(true);
    }
    Tpl.SetTp8(false);
  }
}