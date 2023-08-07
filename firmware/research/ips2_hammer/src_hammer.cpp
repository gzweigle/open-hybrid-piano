// gcz 2023
// For ips pcb version 2.0

#include "hammer_settings.h"

#include "stem_piano_ips.h"

#include "six_channel_analog_00.h"
#include "board2board.h"
#include "dsp_damper.h"
#include "dsp_hammer.h"
#include "dsp_pedal.h"
#include "midiout.h"
#include "network.h"
#include "switches.h"
#include "testpoint_led.h"
#include "timing.h"
#include "tft_display.h"

HammerSettings Set;

SixChannelAnalog00 Adc;
Board2Board B2B;
DspDamper DspD;
DspHammer DspH;
DspPedal DspP;
MidiOut Midi;
Network Eth;
Switches Swi;
TestpointLed Tpl;
Timing Tmg;
TftDisplay Tft;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, mi);

void setup(void) {

  // This must be placed first and in the setup() function.
  Set.SetAllSettingValues();

  Serial.begin(Set.serial_baud_rate);
  Serial.println("Beginning hammer board initialization.");

  // Common to hammer and pedal board.
  Swi.Setup(Set.switch_pickup_dropout_samples);
  Adc.Setup(Set.adc_spi_clock_frequency, Set.adc_settling_time_nanoseconds,
  Set.adc_input_invert, &Tpl);
  B2B.Setup(Set.canbus_baud_rate);

  // Processing dampers, hammers, and pedals on hammer board.
  DspD.Setup(Set.damper_threshold, Set.velocity_scaling,
  Set.adc_sample_period_microseconds);
  DspH.Setup(Set.adc_sample_period_microseconds, Set.highest_rest_fraction,
  Set.min_repetition_seconds, Set.min_strike_velocity, Set.hammer_travel_meters,
  Set.initialize_hammer_wait_count);
  DspP.Setup(Set.pedal_sample_interval_microseconds, Set.pedal_threshold,
  Set.sustain_pin, Set.sustain_connected_pin, Set.sostenuto_pin,
  Set.sostenuto_connected_pin, Set.una_corda_pin, Set.una_corda_connected_pin);

  // Sending damper, hammer, and pedal data over Ethernet.
  Midi.Setup(Set.midi_channel, &mi);

  // Common on hammer and pedal board.
  Eth.Setup(Set.teensy_ip, Set.upd_port);
  Tpl.Setup(Set.serial_display_interval_samples);
  Tmg.Setup(Set.adc_sample_period_microseconds);
  Tft.Setup();

  Tft.StartupDisplay();

  Serial.println("Hello world of piano hammers.");

}

int led_count = 0;

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];   // NOT ordered by piano note order.
int hammer_position_adc_counts[NUM_CHANNELS];

// Damper, hammer, and pedal data.
bool damper_event[NUM_CHANNELS], hammer_event[NUM_CHANNELS];
float damper_position[NUM_CHANNELS], hammer_position[NUM_CHANNELS];
float damper_velocity[NUM_CHANNELS], hammer_velocity[NUM_CHANNELS];
int pedal_state[NUM_PEDALS];

void loop() {

  // This if statement determines how often the code inside loop() runs.
  if (Tmg.AllowProcessing() == true) {
    Tpl.SetTp8(true);

    // Every processing interval, update the setup / hold
    // for measuring the DIP switch state.
    Swi.updatePuDoState();
    
    // Get all data: local hammer (ADC), remote damper (board-to-board) data,
    // and local pedals (also from ADC data).
    Adc.GetNewAdcValues(raw_samples);
    Adc.NormalizeAdcValues(hammer_position_adc_counts, hammer_position, raw_samples);
    for (int k = 0; k < NUM_CHANNELS; k++) {
      if (Set.connected_channel[k] == false) {
        hammer_position_adc_counts[k] = 0;
        hammer_position[k] = 0.0;
      }
    }
    if (Set.connected_to_remote_damper_board == true) {
      // There exists a damper board so use remote data from the damper board.
      B2B.GetDamperData(damper_position);
    }
    else {
      // No external damper board.
      // Use the hammer position as an estimate of the damper position.
      for (int i = 0; i < NUM_CHANNELS; i++)
        damper_position[i] = hammer_position[i];
    }

    // From data, process hammer, damper, and pedal data.
    // For hammer and damper get an event boolean flag and velocity.
    // For pedal get the state of the pedal.
    DspD.GetDamperEventData(damper_event, damper_velocity, damper_position);
    DspH.GetHammerEventData(hammer_event, hammer_velocity, hammer_position);
    DspP.PedalState(pedal_state, hammer_position);

    // Sending data over MIDI and Ethernet.
    Midi.SendNoteOn(hammer_event, hammer_velocity);
    Midi.SendNoteOff(damper_event, damper_velocity);
    Midi.SendPedal(pedal_state);
    Eth.SendPianoPacket(hammer_position_adc_counts[40], hammer_position_adc_counts[41]);

    // Touchscreen display.
    Tft.GetTouchPosition();
    Tft.Picture();
    Tft.UpdateDisplay();

    /////////////////
    // Temporary debug and diagnostic stuff.
    led_count++;
    if (led_count == Set.serial_display_interval_samples/4) {
      Tpl.SetTp10(false);
      Tpl.SetTp11(true);
      Tpl.SetLowerRightLED(false);
      Tpl.SetEthernetLED(false);
    }
    else if (led_count == Set.serial_display_interval_samples) {
      led_count = 0;
      // Display ADC output for two of the analog inputs.
      Serial.print("Hammer Board ");
      Serial.print(" 40="); Serial.print(damper_position[40]);
      Serial.print(" 41="); Serial.print(hammer_position[41]);
      Serial.print(" 90="); Serial.print(hammer_position[90]);
      Serial.print(" 91="); Serial.print(hammer_position[91]);
      Serial.print(" 92="); Serial.print(hammer_position[92]);
      Serial.print(" 93="); Serial.print(hammer_position[93]);
      Serial.print(" 94="); Serial.print(hammer_position[94]);
      Serial.print(" 95="); Serial.print(hammer_position[95]);
      Serial.println("");
      Tpl.SetTp10(true);
      Tpl.SetTp11(false);
      Tpl.SetLowerRightLED(true);
      Tpl.SetEthernetLED(true);
    }
    /////////////////
    Tpl.SetTp8(false);
  }
}