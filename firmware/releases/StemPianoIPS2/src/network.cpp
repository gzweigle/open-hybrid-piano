// Copyright (C) 2023 Greg C. Zweigle
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
// Super simple send four measurements over UDP Ethernet. 
//
// TODO - Data to SendPianoPacket() is hardcoded int, int, float, float.
//        Make this more generic and configurable.
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
int debug_level) {
  debug_level_ = debug_level;
  network_ok_ = false;
  GetMacAddress();
  SetIpAddresses(computer_ip, teensy_ip, udp_port);
  SetupNetwork();
}

// Intended to be used to send two local hammer values as direct ADC counts
// and two remote (or local) damper values that arrive as float in range [0,1].
void Network::SendPianoPacket(float fl0, float fl1, float fl2, float fl3,
float fl4, float fl5, float fl6, float fl7) {

  if (network_ok_ == true) {

    // Four 16-bit values, each sent as two 8-bit values.
    uint8_t sv[16];
    sv[0] = static_cast<int>(65535*fl0)&255;
    sv[1] = (static_cast<int>(65535*fl0)>>8)&255;
    sv[2] = static_cast<int>(65535*fl1)&255;
    sv[3] = (static_cast<int>(65535*fl1)>>8)&255;
    sv[4] = static_cast<int>(65535*fl2)&255;
    sv[5] = (static_cast<int>(65535*fl2)>>8)&255;
    sv[6] = static_cast<int>(65535*fl3)&255;
    sv[7] = (static_cast<int>(65535*fl3)>>8)&255;
    sv[8] = static_cast<int>(65535*fl4)&255;
    sv[9] = (static_cast<int>(65535*fl4)>>8)&255;
    sv[10] = static_cast<int>(65535*fl5)&255;
    sv[11] = (static_cast<int>(65535*fl5)>>8)&255;
    sv[12] = static_cast<int>(65535*fl6)&255;
    sv[13] = (static_cast<int>(65535*fl6)>>8)&255;
    sv[14] = static_cast<int>(65535*fl7)&255;
    sv[15] = (static_cast<int>(65535*fl7)>>8)&255;

    // Send the data via UDP.
    IPAddress ip(computer_ip_[0], computer_ip_[1], computer_ip_[2], computer_ip_[3]);
    Udp.beginPacket(ip, udp_port_);
    Udp.write(sv, 16);
    Udp.endPacket();
    Udp.flush();
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

  if (debug_level_ >= 1) {
    Serial.println("For proper Ethernet operation the following values");
    Serial.println("must match for the program running on computer and");
    Serial.println("attempting to communicate with the IPS 2.X board.");
    Serial.println("(Ethernet values are set in the settings C++ file).");
    Serial.print("The assigned Computer IP address is: ");
    Serial.print(computer_ip_[0]);
    Serial.print(".");
    Serial.print(computer_ip_[1]);
    Serial.print(".");
    Serial.print(computer_ip_[2]);
    Serial.print(".");
    Serial.println(computer_ip_[3]);
    Serial.print("The assigned Teensy IP address is: ");
    Serial.print(teensy_ip_[0]);
    Serial.print(".");
    Serial.print(teensy_ip_[1]);
    Serial.print(".");
    Serial.print(teensy_ip_[2]);
    Serial.print(".");
    Serial.println(teensy_ip_[3]);
    Serial.print("The assigned UDP port is: ");
    Serial.println(udp_port_);
  }

}

void Network::SetupNetwork() {
  IPAddress ip(teensy_ip_[0], teensy_ip_[1], teensy_ip_[2], teensy_ip_[3]);
  Ethernet.begin(mac_address_, ip);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    if (debug_level_ >= 1) {
      Serial.println("Error - no hardware found.");
    }
  }
  else if (Ethernet.linkStatus() == LinkOFF) {
    if (debug_level_ >= 1) {
      Serial.println("Error - no ethernet cable.");
    }
  }
  else {
    Udp.begin(udp_port_);
    network_ok_ = true;
  }
}

#else

Network::Network() {}
void Network::Setup(int a, int b, int c, int debug_level) {
  if (debug_level >= 1) {
    Serial.println("Ethernet is not in build and is not used.");
  }
}
void Network::SendPianoPacket(int a, int b, float c, float d) {}

#endif