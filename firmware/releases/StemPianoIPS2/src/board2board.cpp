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
// board2board.cpp
//
// This code runs on ips pcb version 2.X
//
// Transfer damper position from damper board to hammer board using CAN Bus.
//
// The code packs and unpacks position data from damper of 88 piano keys into
// the 64*8 = 512 bits of CAN FD as follows:
// A0  - 4 bits
// A#0 - 4 bits
// B0  - 4 bits
// C1  - 4 bits
// C#1 - 6 bits
//  :    :
// G#7 - 6 bits
// A7  - 4 bits
// A#7 - 4 bits
// B7  - 4 bits
// C8  - 4 bits
// From the above: (4+4)*4 + 80*6 = 512.  So, it fits.
//
// The boards have enough pins to extend a keyboard beyond 88 keys.
// In this case, the mapping described above would need to change
// in order to fit the extra keys into the 512 available bits.
//
// TODO - Only supports 88 dampers. If a piano with more keys
//        is built, and if want dampers on those keys, then will
//        need to extend to the extra keys.

#include "board2board.h"

Board2Board::Board2Board() {}

void Board2Board::Setup(bool enable) {

  enable_ = enable;

  // Connected to CAN Bus driver IC but not using these features
  // of the IC. Drive low and never change the state.
  pinMode(28, OUTPUT);
  pinMode(29, OUTPUT);
  digitalWrite(28, LOW);
  digitalWrite(29, LOW);

  if (enable_ == true) {
    CANFD_timings_t config;
    config.baudrate = 1000000;
    config.baudrateFD = 2000000;
    config.bus_length = 1;
    config.clock = CLK_24MHz;
    config.propdelay = 190;
    config.sample = 70;
    can_.begin();
    can_.setRegions(64);
    can_.setBaudRate(config);

    msg_tx_.id = 0x100;
    msg_tx_.len = CANBUS_LENGTH;
    for (int ind = 0; ind < msg_tx_.len; ind++) {
      msg_tx_.buf[ind] = 0;
    }
  }
}

// Pack the damper data into CAN Bus array and send it.
// This function is called by the damper board code.
void Board2Board::SendDamperData(const float *position) {
  if (enable_ == true) {
    int p_int[NUM_CHANNELS];
    for (int ind = 0; ind < NUM_CHANNELS; ind++) {
      if (ind < 4 || ind > 83) {
        p_int[ind] = static_cast<int>(16.0 * position[ind]) & 0x0F;
      }
      else {
        p_int[ind] = static_cast<int>(64.0 * position[ind]) & 0x3F;
      }
    }
    // First 4 and last 4 of 88 keys are 4 bits each.
    msg_tx_.buf[0]  = (p_int[0]<<4)  | p_int[1];
    msg_tx_.buf[1]  = (p_int[2]<<4)  | p_int[3];
    msg_tx_.buf[62] = (p_int[84]<<4) | p_int[85];
    msg_tx_.buf[63] = (p_int[86]<<4) | p_int[87];
    // The middle 80 keys are 6 bits, which is 480 total bits.
    // The following pattern repeats 480/8/3 = 20 times.
    // AAAAAABB, BBBBCCCC, CCDDDDDD
    for (int ind = 0; ind < 20; ind++) {
      msg_tx_.buf[3*ind+2] = ( p_int[4*ind+4]<<2)       | ((p_int[4*ind+5]>>4)&0x03);
      msg_tx_.buf[3*ind+3] = ((p_int[4*ind+5]<<4)&0xF0) | ((p_int[4*ind+6]>>2)&0x0F);
      msg_tx_.buf[3*ind+4] = ((p_int[4*ind+6]<<6)&0xC0) | ( p_int[4*ind+7]    &0x3F);
    }
    can_.write(msg_tx_);
  }
}

// Receive data from CAN Bus then unpack the damper data.
// This function is called by the hammer board.
bool Board2Board::GetDamperData(float *position) {
  if (enable_ == true) {
    int read_return = can_.read(msg_rx_);
    if (read_return) {
      int p_int[NUM_CHANNELS];
      // First 4 and last 4 of 88 keys are 4 bits each.
      p_int[0]  = (msg_rx_.buf[0]>>4)&0x0F;
      p_int[1]  = (msg_rx_.buf[0]>>0)&0x0F;
      p_int[2]  = (msg_rx_.buf[1]>>4)&0x0F;
      p_int[3]  = (msg_rx_.buf[1]>>0)&0x0F;
      p_int[84] = (msg_rx_.buf[62]>>4)&0x0F;
      p_int[85] = (msg_rx_.buf[62]>>0)&0x0F;
      p_int[86] = (msg_rx_.buf[63]>>4)&0x0F;
      p_int[87] = (msg_rx_.buf[63]>>0)&0x0F;
      // The middle 80 keys are 6 bits, which is 480 total bits.
      // The following pattern repeats 480/8/3 = 20 times.
      // AAAAAABB, BBBBCCCC, CCDDDDDD
      for (int ind = 0; ind < 20; ind++) {
        p_int[4*ind+4] = ((msg_rx_.buf[3*ind+2]>>2)&0x3F);
        p_int[4*ind+5] = ((msg_rx_.buf[3*ind+2]<<4)&0x30) | ((msg_rx_.buf[3*ind+3]>>4)&0x0F);
        p_int[4*ind+6] = ((msg_rx_.buf[3*ind+3]<<2)&0x3C) | ((msg_rx_.buf[3*ind+4]>>6)&0x03);
        p_int[4*ind+7] = ((msg_rx_.buf[3*ind+4]>>0)&0x3F);
      }
      for (int ind = 0; ind < NUM_CHANNELS; ind++) {
        if (ind < 88) {
          if (ind < 4 || ind > 83) {
            position[ind] = static_cast<float>(p_int[ind] / 16.0);
          }
          else {
            position[ind] = static_cast<float>(p_int[ind] / 64.0);
          }
        }
        else {
          // Zero out keys 88, 89, 90, 91, 92, 93, 94, 95.
          position[ind] = 0.0;
        }
      }
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}