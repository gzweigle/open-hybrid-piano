// gcz 2023
//
// For ips pcb version 2.X
// For sca pcb version 0.0
//
// Main code for hammer board of Stem Piano.

#include "hammer_settings.h"

#include "stem_piano_ips2.h"

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
Switches SwIPS1;
Switches SwIPS2;
Switches SwSCA1;
Switches SwSCA2;
TestpointLed Tpl;
Timing Tmg;
TftDisplay Tft;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, mi);

void setup(void) {

  // This must be placed first and in the setup() function.
  Set.SetAllSettingValues();

  // These must be called second and in the setup() function.
  SwIPS1.Setup(Set.switch_debounce_micro, Set.switch11_ips_pin, Set.switch12_ips_pin);
  SwIPS2.Setup(Set.switch_debounce_micro, Set.switch21_ips_pin, Set.switch22_ips_pin);
  SwSCA1.Setup(Set.switch_debounce_micro, Set.switch11_sca_pin, Set.switch12_sca_pin);
  SwSCA2.Setup(Set.switch_debounce_micro, Set.switch21_sca_pin, Set.switch22_sca_pin);

  // Serial port setup.
  Serial.begin(Set.serial_baud_rate);
  #if DEBUG_LEVEL >= 1
    Serial.println("Beginning hammer board initialization.");
  #endif

  // Common to hammer and pedal board.
  Adc.Setup(Set.adc_spi_clock_frequency,
  Set.adc_extra_settling_time_nanoseconds, &Tpl);
  B2B.Setup(Set.canbus_enable);

  // Processing dampers, hammers, and pedals on hammer board.
  DspD.Setup(Set.damper_threshold, Set.velocity_scaling,
  Set.adc_sample_period_microseconds);
  DspH.Setup(Set.adc_sample_period_microseconds, Set.strike_threshold,
  Set.release_threshold, Set.min_repetition_seconds, Set.min_strike_velocity,
  Set.hammer_travel_meters, Set.initialize_hammer_wait_count);
  DspP.Setup(Set.pedal_sample_interval_microseconds, Set.pedal_threshold,
  Set.sustain_pin, Set.sustain_connected_pin, Set.sostenuto_pin,
  Set.sostenuto_connected_pin, Set.una_corda_pin, Set.una_corda_connected_pin);

  // Sending damper, hammer, and pedal data over MIDI.
  Midi.Setup(Set.midi_channel, &mi);

  // Common on hammer and pedal board.
  Eth.Setup(Set.computer_ip, Set.teensy_ip, Set.upd_port);
  Tpl.Setup(Set.serial_display_interval_samples);
  Tmg.Setup(Set.adc_sample_period_microseconds);
  Tft.Setup(Set.using_display);

  Tft.StartupDisplay();

  #if DEBUG_LEVEL >= 1
    Serial.println("Hello world of piano hammers.");
  #endif

}

// Temporary.
int led_count = 0;

// Data from ADC.
unsigned int raw_samples[NUM_CHANNELS];
int hammer_position_adc_counts[NUM_CHANNELS];

// Damper, hammer, and pedal data.
bool damper_event[NUM_CHANNELS], hammer_event[NUM_CHANNELS];
float damper_position[NUM_CHANNELS], hammer_position[NUM_CHANNELS];
float damper_velocity[NUM_CHANNELS], hammer_velocity[NUM_CHANNELS];

void loop() {

  // This if statement determines the sample rate.
  if (Tmg.AllowProcessing() == true) {
    Tpl.SetTp8(true);

    // Measure every processing interval so the pickup and
    // dropout timers update. In other places, when a switch
    // is read, what is actually read is the internal state
    // of timers.
    SwIPS1.updatePuDoState("IPS11", "IPS12");
    SwIPS2.updatePuDoState("IPS21", "IPS22");
    SwSCA1.updatePuDoState("SCA11", "SCA12");
    SwSCA2.updatePuDoState("SCA21", "SCA22");
    
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
    DspP.UpdatePedalState(hammer_position);

    // Sending data over MIDI and Ethernet.
    Midi.SendNoteOn(hammer_event, hammer_velocity);
    Midi.SendNoteOff(damper_event, damper_velocity);
    Midi.SendPedal(&DspP);
    Eth.SendPianoPacket(hammer_position_adc_counts[24],
    hammer_position_adc_counts[25]);

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
      Tpl.SetScaLedL(false);
      Tpl.SetScaLedR(false);
      Tpl.SetEthernetLED(false);
    }
    else if (led_count == Set.serial_display_interval_samples) {
      led_count = 0;
      // Display ADC output for two of the analog inputs.
      #if DEBUG_LEVEL >= 1
      Serial.print("Hammer Board ");
      #ifdef BOARD_BRINGUP
      Serial.print(" 24="); Serial.print(damper_position[24]);
      Serial.print(" 25="); Serial.print(hammer_position[25]);
      Serial.print(" 26="); Serial.print(hammer_position[26]);
      Serial.print(" 27="); Serial.print(hammer_position[27]);
      #else
      Serial.print(" 40="); Serial.print(damper_position[40]);
      Serial.print(" 41="); Serial.print(hammer_position[41]);
      Serial.print(" 90="); Serial.print(hammer_position[90]);
      Serial.print(" 91="); Serial.print(hammer_position[91]);
      Serial.print(" 92="); Serial.print(hammer_position[92]);
      Serial.print(" 93="); Serial.print(hammer_position[93]);
      Serial.print(" 94="); Serial.print(hammer_position[94]);
      Serial.print(" 95="); Serial.print(hammer_position[95]);
      #endif
      Serial.println("");
      #endif
      Tpl.SetTp10(true);
      Tpl.SetTp11(false);
      Tpl.SetLowerRightLED(true);
      Tpl.SetScaLedL(true);
      Tpl.SetScaLedR(true);
      Tpl.SetEthernetLED(true);
    }
    /////////////////

    Tpl.SetTp8(false);
  }
}