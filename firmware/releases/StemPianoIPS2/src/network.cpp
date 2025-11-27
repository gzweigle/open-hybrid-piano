// Copyright (C) 2025 Greg C. Zweigle
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
// https://github.com/gzweigle/open-hybrid-piano
// https://github.com/stem-piano
//
// network.cpp
//
// This class is not hardware dependent.
//
// How to use UDP (server):
// - Connect an Ethernet cable then power on stem_piano.
// - Data automatically starts transmitting immediately.
// - Setup a UDP client on another computer to receive the data.
//
// How to use TCP (client):
// - Connect an Ethernet cable then power on stem_piano.
// - Data does not start transmitting.
// - Setup a TCP server on another computer, set it to begin listening for a client connection.
// - If not require_tcp_connection, switch the Ethernet DIP off then on.
// - If require_tcp_connection, it will continuously try to connect (Ethernet DIP must also be on). Piano won't be playable unless Ethernet data is streaming.

#include "network.h"

#ifdef ETHERNET_INSTALLED

#include "string.h"
#include <stdlib.h>
#include <stdio.h>

Network::Network() {}

void Network::Setup(bool true_for_tcp_else_udp, const char *computer_ip,
  const char *teensy_ip, int port, bool switch_enable_ethernet, int debug_level) {

  debug_level_ = debug_level;
  send_data_ok_ = false;

  // If this is true, TCP client connection.
  // If false, UDP server broadcast.
  true_for_tcp_else_udp_ = true_for_tcp_else_udp;

  // For checking configuration switch and Ethernet enable.
  network_has_been_initialized_ = false;
  switch_enable_ethernet_last_ = true;

  // Lower the delay (default is 1000 ms) so power-up is faster.
  Client.setConnectionTimeout(100);

  GetMacAddress();
  SetIpAddresses(computer_ip, teensy_ip, port);
  SetupNetwork(false, switch_enable_ethernet, false);

  for (int ind = 0; ind < NUM_CHANNELS; ind++) {
    true_for_hammer_else_damper_[ind] = true;
    send_ind_[ind] = ind;
  }

  // Send A0 damper data as Ethernet channel 1.
  true_for_hammer_else_damper_[1] = false;
  send_ind_[1] = 0;

}

void Network::SendPianoPacket(const float *hammer_in, const float *damper_in,
  bool switch_enable_ethernet, bool switch_require_tcp_connection, int test_index) {

  SetupNetwork(switch_require_tcp_connection,
    switch_enable_ethernet, switch_enable_ethernet_last_);
  if (true_for_tcp_else_udp_ == true) {
    EndNetwork(switch_enable_ethernet, switch_enable_ethernet_last_);
  }
  switch_enable_ethernet_last_ = switch_enable_ethernet;

  if (switch_enable_ethernet == true) {

    if (send_data_ok_ == true) {

      int data_int;

      for (int ind = 0; ind < NUM_CHANNELS; ind++) {

        // Input data is in range -1.0, ... 1.0 as floats.
        // Multiply by 2^15 and limit to signed 16-bit value.
        if (true_for_hammer_else_damper_[ind] == true) {
          data_int = static_cast<int>(hammer_in[send_ind_[ind]]*32767.0);
        }
        else {
          data_int = static_cast<int>(damper_in[send_ind_[ind]]*32767.0);
        }
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

      // Setup for UDP each time.
      if (true_for_tcp_else_udp_ == false) {
        IPAddress ip(computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3]);
        Udp.beginPacket(ip, port_);
      }

      if (true_for_tcp_else_udp_ == true) {

        // Send via TCP, as client.
        if (Client.connected()) {
          unsigned int available_space = Client.availableForWrite();
          if (available_space >= sizeof(ethernet_values_)) {
            unsigned int now = micros();
            if (test_index < 0) {  // Normal case.
              Client.write(ethernet_values_, sizeof(ethernet_values_));
            }
            else {
              Client.write(ethernet_values_, 2);
            }
            #ifdef QNETHERNET
            Client.flush();
            #endif
            if (debug_level_ >= DEBUG_INFO) {
              if (micros() - now > 20) {
                Serial.printf("Client.write = %d microseconds.\n", micros() - now);
                Serial.printf("%d \n",available_space);
              }
            }
          }
        }

      }
      // Send via UDP.
      else {
        if (test_index < 0) {  // Normal case.
          Udp.write(ethernet_values_, sizeof(ethernet_values_));
        }
        else {
          Udp.write(ethernet_values_, 2);
        }
      }

      // Finish UDP each time.
      if (true_for_tcp_else_udp_ == false) {
        Udp.endPacket();
        Udp.flush();
      }
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

// Teensy and Computer must be on same gateway.
void Network::SetIpAddresses(const char *computer_ip, const char *teensy_ip,
int port) {

  sscanf(computer_ip, "%d.%d.%d.%d",
  computer_ip_, computer_ip_ + 1, computer_ip_ + 2, computer_ip_ + 3);

  sscanf(teensy_ip, "%d.%d.%d.%d",
  teensy_ip_, teensy_ip_ + 1, teensy_ip_ + 2, teensy_ip_ + 3);

  port_ = port;

  if (debug_level_ >= DEBUG_INFO) {
    Serial.println("If using Ethernet:");
    Serial.println("  For proper Ethernet operation the following values");
    Serial.println("  must match for the program running on computer and");
    Serial.println("  attempting to communicate with the IPS 2.X board.");
    Serial.println("  (Ethernet values are set in the settings C++ file).");
    Serial.print("  The Computer IP address is: ");
    Serial.printf(" %d.%d.%d.%d:%d\n",computer_ip_[0], Serial.print(computer_ip_[1]),
    computer_ip_[2],computer_ip_[3], port_);
    Serial.print("  The Teensy IP address is:");
    Serial.printf(" %d.%d.%d.%d:%d\n",teensy_ip_[0], teensy_ip_[1],teensy_ip_[2],
    teensy_ip_[3], port_);
    #ifdef QNETHERNET
    Serial.print("  The subnet mask is: 255.255.255.0\n");
    Serial.print("  The gateway is:");
    Serial.printf(" %d.%d.%d.%d\n",teensy_ip_[0], teensy_ip_[1],teensy_ip_[2], 1);
    #endif
    if (true_for_tcp_else_udp_ == true) {
      Serial.println("  Connection is TCP client.");
    }
    else {
      Serial.println("  Connection is UDP server broadcast.");
    }
  }

}

void Network::SetupNetwork(bool switch_require_tcp_connection,
  bool switch_enable_ethernet, bool switch_enable_ethernet_last) {

  if ((switch_enable_ethernet == true && switch_enable_ethernet_last == false) || 
  switch_require_tcp_connection == true) {

    // For NativeEthernet:
    // If here and Ethernet is not connected, code hangs until
    // Ethernet is connected. With Ethernet disconnected, changing
    // configuration switch from enabled to disabled does not unhang.
    // Must restart with configuration switch set for no Ethernet
    // or connect an Ethernet cable.

    if (network_has_been_initialized_ == false) {
      #ifdef QNETHERNET
      Ethernet.macAddress(mac_address_);
      IPAddress ip(teensy_ip_[0], teensy_ip_[1], teensy_ip_[2], teensy_ip_[3]);
      IPAddress netmask(255, 255, 255, 0);
      IPAddress gateway(teensy_ip_[0], teensy_ip_[1], teensy_ip_[2], 1);
      Ethernet.begin(ip, netmask, gateway);
      #else
      IPAddress ip(teensy_ip_[0], teensy_ip_[1], teensy_ip_[2], teensy_ip_[3]);
      Ethernet.begin(mac_address_, ip);
      #endif
    }

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      if (debug_level_ >= DEBUG_INFO) {
        Serial.println("Error - no hardware found.");
      }
    }
    #ifndef QNETHERNET
    else if (Ethernet.linkStatus() == LinkOFF) {
      if (debug_level_ >= DEBUG_INFO) {
        Serial.println("Error - no ethernet cable.");
      }
    }
    #endif
    else {
      if (true_for_tcp_else_udp_ == true) {
        if (Client.connected() == false) {
          IPAddress ip(computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3]);
          if (debug_level_ >= DEBUG_INFO) {
            Serial.printf("Attempting TCP connection to %d.%d.%d.%d:%d\n",
            computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3], port_);
          }
          if (Client.connect(ip, port_)) {
            #ifdef QNE_ETHERNET
            Client.setNoDelay(true);
            #endif
            if (debug_level_ >= DEBUG_INFO) {
              Serial.println("TCP connection established.");
            }
            send_data_ok_ = true;
          }
          else {
            if (debug_level_ >= DEBUG_INFO) {
              Serial.println("No TCP connection established.");
            }
            send_data_ok_ = false;
          }
        }
      }
      else if (network_has_been_initialized_ == false) {
        Udp.begin(port_);
        send_data_ok_ = true;
      }
    }

    network_has_been_initialized_ = true;

    if (send_data_ok_ == true && debug_level_ >= DEBUG_INFO &&
      switch_require_tcp_connection == false) {
      Serial.println("Ethernet is setup.");
    }

  }

}

// Get here either on startup or if switch goes high to low.
void Network::EndNetwork(bool switch_enable_ethernet, bool switch_enable_ethernet_last) {
  if (switch_enable_ethernet == false && switch_enable_ethernet_last == true) {
    if (true_for_tcp_else_udp_ == true) {
      if (Client.connected()) {
        if (debug_level_ >= DEBUG_INFO) {
          Serial.println("Flushing the TCP connection.");
        }
        Client.flush();
        if (debug_level_ >= DEBUG_INFO) {
          Serial.println("Stopping the TCP connection.");
        }
        Client.stop();
        if (debug_level_ >= DEBUG_INFO) {
          Serial.println("Stopped the TCP connection.");
        }
      }
    }
  }
}

#else

Network::Network() {}
void Network::Setup(const char *a, const char *b, int c, bool d, int debug_level) {
  if (debug_level >= DEBUG_INFO) {
    Serial.println("Ethernet is not in build and is not used.");
  }
}
void Network::SendPianoPacket(const float * a, bool b, int c) {}

#endif