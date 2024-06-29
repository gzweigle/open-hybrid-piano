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
// auto_mute.h
//

#ifndef AUTO_MUTE_H_
#define AUTO_MUTE_H_

#include "stem_piano_ips2.h"

class AutoMute
{
  public:
    AutoMute();
    void Setup(int);
    int AutomaticallyDecreaseVolume(int, int);
 
  private:
    int mute_velocity_;
    int max_simultaneous_notes_;
    int simultaneous_note_count_;
    int max_simultaneous_volume_;
    int maximum_midi_velocity_;
    unsigned long last_loud_note_time_;
    unsigned long loud_note_interval_;

};

#endif
