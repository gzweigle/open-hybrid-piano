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

#ifndef NETWORK_H_
#define NETWORK_H_

#include "stem_piano_ips2.h"

#ifdef ETHERNET_INSTALLED

#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

class Network
{
  public:
    Network();
    void Setup(bool, const char *, const char *, int, bool, int);
    void SendPianoPacket(const float *, bool, bool, int);

  private:
    int debug_level_;
    bool true_for_tcp_else_udp_;
    uint8_t mac_address_[6];
    int teensy_ip_[4];
    int computer_ip_[4];
    int port_;
    bool send_data_ok_;
    uint8_t ethernet_values_[2*(NUM_CHANNELS)];

    bool network_has_been_initialized_;
    bool switch_enable_ethernet_last_;

    EthernetUDP Udp;        // For UDP.
    EthernetClient Client;  // For TCP.

    void GetMacAddress();
    void SetIpAddresses(const char *, const char *, int);
    void SetupNetwork(bool, bool, bool);
    void EndNetwork(bool, bool);
};

#else

class Network
{
  public:
    Network();
    void Setup(const char *, const char *, int, bool, int);
    void SendPianoPacket(const float *, bool, int);
};

#endif

#endif
