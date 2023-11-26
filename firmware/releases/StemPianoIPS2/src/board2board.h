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
// board2board.h
//
// For ips pcb version 2.X
//
// Transfer damper position from damper board to hammer board using CAN Bus.

#ifndef BOARD2BOARD_H_
#define BOARD2BOARD_H_

#include <FlexCAN_T4.h>

#include "stem_piano_ips2.h"

// CAN Bus FD requires 64 bytes.
// #defined to clarify a magic number.
#define CANBUS_LENGTH 64

class Board2Board
{
  public:
    Board2Board();
    void Setup(bool);
    void SendDamperData(const float *);
    bool GetDamperData(float *);

  private:
    FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> can_;
    CANFD_message_t msg_tx_, msg_rx_;
    bool enable_;
};

#endif