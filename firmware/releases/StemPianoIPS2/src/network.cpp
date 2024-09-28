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

#include "network.h"

#ifdef ETHERNET_INSTALLED

#include "string.h"
#include <stdlib.h>
#include <stdio.h>

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

Network::Network() {}

void Network::Setup(const char *computer_ip, const char *teensy_ip, int udp_port, bool switch_enable_ethernet, int debug_level) {

  debug_level_ = debug_level;
  network_ok_ = false;

  // For checking configuration switch ane Ethernet enable.
  network_has_been_setup_ = false;
  switch_enable_ethernet_last_ = true;

  GetMacAddress();
  SetIpAddresses(computer_ip, teensy_ip, udp_port);
  SetupNetwork(switch_enable_ethernet);
}

void Network::SendPianoPacket(const float *data_in, bool switch_enable_ethernet,
int test_index) {

  // If switch goes from low to high, setup the network.
  if (switch_enable_ethernet == true && switch_enable_ethernet_last_ == false) {
    if (debug_level_ >= DEBUG_STATS) {
      Serial.println("Detected switch to enable Ethernet, setting up the network...");
    }
    SetupNetwork(true);
  }
  switch_enable_ethernet_last_ = switch_enable_ethernet;

  if (switch_enable_ethernet == true) {

    if (network_ok_ == true) {

      int data_int;
      for (int ind = 0; ind < NUM_CHANNELS; ind++) {

        // Input data is in range -1.0, ... 1.0 as floats.
        // Multiply by 2^15 and limit to signed 16-bit value.
        data_int = static_cast<int>(data_in[ind]*32767.0);
        if (data_int > 32767) {
          data_int = 32767;
        }
        else if (data_int < -32768) {
          data_int = -32768;
        }

        // Send a value as two bytes.
        ethernet_values_[2*ind+0] = data_int&255;
        ethernet_values_[2*ind+1] = (data_int>>8)&255;
      }

      // Send the data via UDP.
      IPAddress ip(computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3]);
      Udp.beginPacket(ip, udp_port_);

      if (test_index < 0) {
        Udp.write(ethernet_values_, sizeof(ethernet_values_));
      }
      else {
        // Send only the first sample.
        Udp.write(ethernet_values_, 3);
      }

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

  if (switch_enable_ethernet == true && network_has_been_setup_ == false) {

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

    if (debug_level_ >= DEBUG_STATS) {
      Serial.println("Ethernet is setup.");
    }

  }

}

#else

Network::Network() {}
void Network::Setup(const char *a, const char *b, int c, bool d, int debug_level) {
  if (debug_level >= DEBUG_STATS) {
    Serial.println("Ethernet is not in build and is not used.");
  }
}
void Network::SendPianoPacket(const float * a, bool b, int c) {}

#endif