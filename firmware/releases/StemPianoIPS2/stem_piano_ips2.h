// gcz 2023
//
// For ips pcb version 2.0

#ifndef STEM_PIANO_IPS2_H_
#define STEM_PIANO_IPS2_H_

#include <Arduino.h>

// Hardcode based on pcb layout.
// Using a #define because this is used for statically allocated arrays.
#define NUM_CHANNELS (16*6)

// Hardcode that the first 88 connections are A0 - C7.
// This could be a setting variable but it makes me
// feel better that the #define is immediately below
// NUM_CHANNELS so I am reminded of the relationship.
#define NUM_NOTES 88

#if NUM_NOTES > NUM_CHANNELS
#error "ERROR. Number of notes is greater than number of physical channels."
#endif

#endif