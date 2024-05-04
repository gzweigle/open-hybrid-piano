// Copyright (C) 2024 Greg C. Zweigle
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//
// Location of documentation, code, and design:
// https://github.com/gzweigle/DIY-Grand-Digital-Piano
//
// network.cpp
//
// This class is not hardware dependent.
//
// Super simple send measurements over UDP Ethernet. 
//
// TODO - Needs rethinking.
// TODO - If Ethernet is not plugged in, it hangs in Ethernet.begin().

#include "network.h"

#ifdef ETHERNET_INSTALLED

#include "string.h"
#include <stdlib.h>
#include <stdio.h>

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

Network::Network() {}

void Network::Setup(const char *computer_ip, const char *teensy_ip, int udp_port,
int start_ind, int end_ind, int switch_debounce_micro, int debug_level) {

  debug_level_ = debug_level;
  network_ok_ = false;

  // For checking configuration switch ane Ethernet enable.
  switch_debounce_micro_ = switch_debounce_micro;
  first_network_setup_call_ = false;
  startup_delay_finished_ = false;
  network_has_been_setup_ = false;

  // Temporary - this needs work!
  start_ind_ = start_ind;
  end_ind_ = end_ind;
  if (end_ind_ - start_ind_ + 1 > MAX_ETHERNET_BYTES) {
    Serial.println("Error - too many values to send over Ethernet");
    disable_network_ = true;
  }
  else {
    disable_network_ = false;
  }

  GetMacAddress();
  SetIpAddresses(computer_ip, teensy_ip, udp_port);
}

// Send floats as 3 bytes for a total of 24-bits.
void Network::SendPianoPacket(const float *data, bool switch_enable_ethernet) {

  SetupNetwork(switch_enable_ethernet);

  if (switch_enable_ethernet == true) {

    int data_int;

    if (network_ok_ == true && disable_network_ == false) {

      int num_send_values = end_ind_ - start_ind_ + 1;

      for (int i = 0; i < num_send_values; i++) {

        // Multiply by 2^23 and limit to signed 24-bit value.
        data_int = static_cast<int>(data[start_ind_ + i]*8388608.0);
        if (data_int > 8388607) {
          data_int = 8388607;
        }
        else if (data_int < -8388608) {
          data_int = -8388608;
        }

        // Send a value in range [-2^23, ..., 2^23-1] as three bytes.
        ethernet_values_[3*i+0] = data_int&255;
        ethernet_values_[3*i+1] = (data_int>>8)&255;
        ethernet_values_[3*i+2] = (data_int>>16)&255;
      }

      // Send the data via UDP.
      IPAddress ip(computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3]);
      Udp.beginPacket(ip, udp_port_);
      Udp.write(ethernet_values_, 3*num_send_values);
      Udp.endPacket();
      Udp.flush();
    }
  }
}

void Network::GetMacAddress() {
  for(uint8_t x = 0; x < 2; x++) {
    mac_address_[x] = (HW_OCOTP_MAC1 >> ((1-x)*8)) & 0xFF;
  }
  for(uint8_t x = 0; x < 4; x++) {
    mac_address_[x+2] = (HW_OCOTP_MAC0 >> ((3-x)*8)) & 0xFF;
  }
}

// Teensy and Computer must be on same default gateway.
void Network::SetIpAddresses(const char *computer_ip, const char *teensy_ip,
int udp_port) {

  sscanf(computer_ip, "%d.%d.%d.%d",
  computer_ip_, computer_ip_ + 1, computer_ip_ + 2, computer_ip_ + 3);

  sscanf(teensy_ip, "%d.%d.%d.%d",
  teensy_ip_, teensy_ip_ + 1, teensy_ip_ + 2, teensy_ip_ + 3);

  udp_port_ = udp_port;

  if (debug_level_ >= DEBUG_STATS) {
    Serial.println("If using Ethernet:");
    Serial.println("  For proper Ethernet operation the following values");
    Serial.println("  must match for the program running on computer and");
    Serial.println("  attempting to communicate with the IPS 2.X board.");
    Serial.println("  (Ethernet values are set in the settings C++ file).");
    Serial.print("  The assigned Computer IP address is: ");
    Serial.print(computer_ip_[0]);
    Serial.print(".");
    Serial.print(computer_ip_[1]);
    Serial.print(".");
    Serial.print(computer_ip_[2]);
    Serial.print(".");
    Serial.println(computer_ip_[3]);
    Serial.print("  The assigned Teensy IP address is: ");
    Serial.print(teensy_ip_[0]);
    Serial.print(".");
    Serial.print(teensy_ip_[1]);
    Serial.print(".");
    Serial.print(teensy_ip_[2]);
    Serial.print(".");
    Serial.println(teensy_ip_[3]);
    Serial.print("  The assigned UDP port is: ");
    Serial.println(udp_port_);
  }

}

void Network::SetupNetwork(bool switch_enable_ethernet) {

  // Why this code?
  // Teensy will hang in setup below if Ethernet cable is not connected.
  // Therefore, setup code only runs if the configuration switch is set
  // for using Ethernet.
  // However, must ensure the configuration switch is in a valid state
  // after turn on the board.
  if (first_network_setup_call_ == false) {
    // Get the start time once, after power up.
    // Can't put in setup() because need the delay
    // while switches update in loop().
    start_time_micros_ = micros();
  }
  first_network_setup_call_ = true;
  if (startup_delay_finished_ == false) {
    // Check once to avoid micros() rollover.
    // 5X the debounce for extra margin.
    if (micros() > start_time_micros_ +
    static_cast<unsigned long>(5 * switch_debounce_micro_)) {
      startup_delay_finished_ = true;
    }
  }

  if (switch_enable_ethernet == true && network_has_been_setup_ == false &&
  startup_delay_finished_ == true) {

    // If here and Ethernet is not connected, code hangs until
    // Ethernet is connected. With Ethernet disconnected, changing
    // configuration switch from enabled to disabled does not unhang.
    // Must restart with configuration switch set for no Ethernet
    // or connect an Ethernet cable.

    IPAddress ip(teensy_ip_[0], teensy_ip_[1], teensy_ip_[2], teensy_ip_[3]);
    Ethernet.begin(mac_address_, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      if (debug_level_ >= DEBUG_STATS) {
        Serial.println("Error - no hardware found.");
      }
    }
    else if (Ethernet.linkStatus() == LinkOFF) {
      if (debug_level_ >= DEBUG_STATS) {
        Serial.println("Error - no ethernet cable.");
      }
    }
    else {
      Udp.begin(udp_port_);
      network_ok_ = true;
    }

    network_has_been_setup_ = true;

  }

}

#else

Network::Network() {}
void Network::Setup(int a, int b, int c, int debug_level) {
  if (debug_level >= DEBUG_STATS) {
    Serial.println("Ethernet is not in build and is not used.");
  }
}
void Network::SendPianoPacket(int a, int b, float c, float d) {}

#endif