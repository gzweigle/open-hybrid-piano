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
// stem_piano_ips2.h
//
// For ips pcb version 2.X
//
// The only extra libraries needed for Stem Piano IPS 2.X
// are for the TFT display. Because the TFT is not required
// for system functionality, normally the TFT_INSTALLED #define
// (see code below) is not set. See Adafruit TFT documentation
// for the libraries that need to be installed.
//
// TODO - Find a better solution for SdFat library conflict (see below).
// TODO - If using a keyboard with more than 88 keys, the NUM_NOTES
//        value and associated functionality needs work. 

#ifndef STEM_PIANO_IPS2_H_
#define STEM_PIANO_IPS2_H_

#include <Arduino.h>

// The following steps are required for the TFT.
//
//  1. Purchase and connect the 2.8 inch TFT from AdaFruit.
//
//  2. Install libraries required fro 2.8 inch TFT per AdaFruit documentation.
//
//  3. The TFT is using an AdaFruit library for SD card that conflicts
//     with the library for SD card on Teensy.  As a temorary fix,
//     delete the SdFat/ directory from Teensy library.
//     (instead of deleting its probably better to move it to another
//     location in case it is needed for another project in future).
//     Deleting SdFat/ will cause warnings when building the project.
//
// The TFT is not necessary for the piano to work. If comment out the
// following #define, then the above steps are not required.
#define TFT_INSTALLED

// If Ethernet was not soldered onto the IPS 2.X board, comment this out.
#define ETHERNET_INSTALLED

// Hardcode based on pcb layout.
// Using a #define because statically allocates arrays.
#define NUM_CHANNELS (16*6)

// Hardcode that the first 88 connections are A0 - C7.
// This could be a setting variable but having the #define
// immediately below NUM_CHANNELS reminds of the relationship.
#define NUM_NOTES 88

// Better if this is in the midi class.
#define MIDI_BUFFER_SIZE 128

#if NUM_NOTES > NUM_CHANNELS
#error "ERROR - number of notes is greater than number of physical channels."
#endif

// Shared among all files.
#define DEBUG_NONE  0   // Nothing displayed except startup info.
#define DEBUG_INFO  1   // Occasional code state information.
#define DEBUG_NOTES 2   // Above plus info about note changes.
#define DEBUG_ALG   3   // Above plus algorithm details.
#define DEBUG_ALL   4   // Above plus useless stuff.

#endif