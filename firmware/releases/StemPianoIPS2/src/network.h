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
// netwwork.h
//
// This class is not hardware dependent.
//
// Super simple send measurements over UDP Ethernet. 

#ifndef NETWORK_H_
#define NETWORK_H_

#include "stem_piano_ips2.h"

#ifdef ETHERNET_INSTALLED

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

#define MAX_ETHERNET_BYTES (2*(NUM_CHANNELS))
#define ETHERNET_PACKET_SIZE_FLOATS 12

class Network
{
  public:
    Network();
    void Setup(const char *, const char *, int, int, int);
    void SendPianoPacket(const float *, bool, bool, float);

  private:
    int debug_level_;
    uint8_t mac_address_[6];
    int teensy_ip_[4];
    int computer_ip_[4];
    int udp_port_;
    bool network_ok_;
    bool disable_network_;
    uint8_t ethernet_values_[3*(MAX_ETHERNET_BYTES)];

    bool network_has_been_setup_;

    int switch_debounce_micro_;
    unsigned long start_time_micros_; 
    bool first_network_setup_call_;
    bool startup_delay_finished_;

    int ethernet_start_ind_;

    EthernetUDP Udp;

    void GetMacAddress();
    void SetIpAddresses(const char *, const char *, int);
    void SetupNetwork(bool);
    void BuildPacket(float *, const float *, bool, float);
};

#else

class Network
{
  public:
    Network();
    void Setup(int, int, int);
    void SendPianoPacket(int, int, float, float);
};

#endif

#endif
