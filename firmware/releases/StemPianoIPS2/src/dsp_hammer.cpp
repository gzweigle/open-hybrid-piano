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
// dsp_hammer.cpp
//
// This class is not hardware dependent.
//
// Convert hammer position measurements into a velocity value
// and detect hammer strike events.
//
// Computation is over all channels. It is up to the MIDI function
// to determine which correspond to actual keys vs. pedals.

#include "dsp_hammer.h"

DspHammer::DspHammer() {}

void DspHammer::Setup(int hammer_strike_algorithm, int sample_period, float strike_threshold,
float release_threshold, float min_repetition_seconds, float min_strike_velocity, 
float hammer_travel_meters, int debug_level) {

  debug_level_ = debug_level;

  hammer_strike_algorithm_ = hammer_strike_algorithm;
  
  samples_per_second_ = static_cast<int>(1.0/(sample_period*1e-6));

  strike_threshold_ = strike_threshold;
  release_threshold_ = release_threshold;

  min_repetition_samples_ = static_cast<int>(min_repetition_seconds *
  (float) samples_per_second_);

  min_strike_velocity_ = min_strike_velocity;

  hammer_travel_meters_ = hammer_travel_meters;

  buffer_pointer_ = 0;
  for (int key = 0; key < NUM_CHANNELS; key++) {
    max_velocity_[key] = 0.0;
    repetition_counter_[key] = 0;
    released_[key] = true;
    for (int ind = 0; ind < DERIVATIVE_AVERAGE_SAMPLES; ind++) {
      velocity_buffer_[key][ind] = 0.0;
    }
  }

  enable_ = true;

}

// Position units is [0.0 to 1.0], where 1.0 is maximum ADC value.
// Velocity units is meters / second.
void DspHammer::GetHammerEventData(bool *event, float *velocity, const float *position) {
  ComputeDerivative(position);
  DetectReleased(position);
  DetectHammerStrike(event, velocity, position);
  UpdateMaxHammerVelocity();  // Must be called AFTER DetectHammerStrike().
}

// Convert hammer position into hammer velocity.
// Derivative is inherently a high-pass filter which enhances noise.
// Therefore, a boxcar average is convolved into the filter.
// This is why looking back DERIVATIVE_AVERAGE_SAMPLES into buffer.
void DspHammer::ComputeDerivative(const float *position) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    // This is the high-pass + boxcar filter.
    velocity_[key] = position[key] - velocity_buffer_[key][buffer_pointer_];
    // Convert denominator of velocity to seconds.
    velocity_[key] *= static_cast<float>(samples_per_second_);
    velocity_[key] *= hammer_travel_meters_;
    // Remove the high-pass + boxcar filter scaling.
    velocity_[key] /= static_cast<float>(DERIVATIVE_AVERAGE_SAMPLES);
    velocity_buffer_[key][buffer_pointer_] = position[key];
  }
  buffer_pointer_++;
  if (buffer_pointer_ >= DERIVATIVE_AVERAGE_SAMPLES) {
    buffer_pointer_ = 0;
  }
}

// As long as hammer is below the release threshold, keep saying its released.
// The released_[] value is set to false when a hammer strikes the string.
// Then, before allowed to strike again, it must fall below release threshold.
void DspHammer::DetectReleased(const float *position) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (position[key] < release_threshold_) {
      released_[key] = true;
    }
  }
}

// Keep track of highest velocity since the hammer started moving toward string.
// Set to max zero if velocity goes negative in order to reset the max value
// during oscillations in velocity prior to traveling up to hit the string.
// However, this means it must be called after DetectHammerStrike(). Otherwise
// a hammer strike velocity will always be zero.
void DspHammer::UpdateMaxHammerVelocity() {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (velocity_[key] < 0) {
      max_velocity_[key] = 0;
    }
    else {
      if (velocity_[key] > max_velocity_[key])
        max_velocity_[key] = velocity_[key];
    }
  }
}

// Algorithm for detecting if a hammer struck the imaginary string.
void DspHammer::DetectHammerStrike(bool *event, float *velocity, const float *position) {

  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (enable_ == true && 
    // To avoid noise while near rest, check if hammer is close to string...
    position[key] >= strike_threshold_ &&
    // and check if the hammer just bounced off something...
    (velocity_[key] <= 0 || hammer_strike_algorithm_ == 1) &&
    // and its not noise from recent strike (as hammer stop deforms when hit)...
    repetition_counter_[key] >= min_repetition_samples_ &&
    // and velocity was fast enough that hammer actually flew off the jack and
    // hit the string (a change in velocity due to hammer reversing direction
    // due to key motion, but not due to hammer hitting string, is a slow-motion
    // "silent" strike)...
    max_velocity_[key] > min_strike_velocity_ &&
    // and, in rare cases (usually during testing), when a hammer is held close
    // to a string small noise can make it appear to have a velocity reversal.
    // Therefore, do not allow a 2nd strike until the hammer drops below a threshold.
    released_[key] == true)
    {
      if (debug_level_ >= DEBUG_ALL) {
        Serial.printf("DetectHammerStrike()");
        Serial.printf("  key=%d pos=%f v_max=%f v_inst=%f repc=%d bufp=%d\n",
        key, position[key], max_velocity_[key], velocity_[key],
        repetition_counter_[key], buffer_pointer_);
      }

      // Hooray, we got a hammer strike on virtual string!
      if (hammer_strike_algorithm_ == 0) {
        velocity[key] = max_velocity_[key];// Velocity of strike = max when rising.
      }
      else {
        velocity[key] = velocity_[key];
      }
      max_velocity_[key] = 0.0;             // Reset max velocity.
      repetition_counter_[key] = 0;         // Start check for double bounce.
      released_[key] = false;               // Must wait for a release.
      event[key] = true;                    // Tell the world.
    }

    else {
      // Limit to avoid any risk of a rollover bug.
      if (repetition_counter_[key] <= min_repetition_samples_)
        repetition_counter_[key]++;
      event[key] = false;
    }
  }
}

void DspHammer::Enable(bool enable) {
  enable_ = enable;
}