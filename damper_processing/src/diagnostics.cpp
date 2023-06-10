// gcz 2023

#include "diagnostics.h"

Diagnostics::Diagnostics() {}

void Diagnostics::Setup(int samples_per_second, Switches *Dip) {
  samples_per_second_ = samples_per_second;

  led_flash_count_ = 0;
  serial_display_count_ = 0;

  Dip_ = Dip;

  // The four test points / LEDs.
  pinMode(5, OUTPUT); digitalWrite(5, LOW);
  pinMode(4, OUTPUT); digitalWrite(4, LOW);
  pinMode(3, OUTPUT); digitalWrite(3, LOW);
  pinMode(2, OUTPUT); digitalWrite(2, LOW);

  // Extra Ethernet LED.
  pinMode(23, OUTPUT); digitalWrite(23, LOW);

}

void Diagnostics::DisplayState() {
  serial_display_count_++;
  if (serial_display_count_ == 2*samples_per_second_) {
    serial_display_count_ = 0;
    Serial.print("Damper Position Processor");
    if (Dip_->read_switch_11() == true) Serial.print(" 11=1");
    else                                Serial.print(" 11=0");
    if (Dip_->read_switch_12() == true) Serial.print(" 12=1");
    else                                Serial.print(" 12=0");
    if (Dip_->read_switch_21() == true) Serial.print(" 21=1");
    else                                Serial.print(" 21=0");
    if (Dip_->read_switch_22() == true) Serial.print(" 22=1");
    else                                Serial.print(" 22=0");
    if (Dip_->read_switch_31() == true) Serial.print(" 31=1");
    else                                Serial.print(" 31=0");
    if (Dip_->read_switch_32() == true) Serial.print(" 32=1");
    else                                Serial.print(" 32=0");
    if (Dip_->read_switch_41() == true) Serial.print(" 41=1");
    else                                Serial.print(" 41=0");
    if (Dip_->read_switch_42() == true) Serial.print(" 42=1");
    else                                Serial.print(" 42=0");
    Serial.println(".");
  }
}

void Diagnostics::ControlEthernetLed() {
  led_flash_count_++;
  if (led_flash_count_ == samples_per_second_) {
    digitalWrite(23, LOW);
  }
  else if (led_flash_count_ == 2*samples_per_second_) {
    digitalWrite(23, HIGH);
    led_flash_count_ = 0;
  }
}

void Diagnostics::SetTp11(bool set_value) {
  if (set_value == false) {
    digitalWrite(5, LOW);
  }
  else {
    digitalWrite(5, HIGH);
  }
}
void Diagnostics::SetTp10(bool set_value) {
  if (set_value == false) {
    digitalWrite(4, LOW);
  }
  else {
    digitalWrite(4, HIGH);
  }
}
void Diagnostics::SetTp9(bool set_value) {
  if (set_value == false) {
    digitalWrite(3, LOW);
  }
  else {
    digitalWrite(3, HIGH);
  }
}
void Diagnostics::SetTp8(bool set_value) {
  if (set_value == false) {
    digitalWrite(2, LOW);
  }
  else {
    digitalWrite(2, HIGH);
  }
}