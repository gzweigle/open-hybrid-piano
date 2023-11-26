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
// dsp_hammer.cpp
//
// This class is not hardware dependent.
//
// Convert hammer position measurements into a velocity value
// and detect hammer strike events.
//
// TODO - Use of maximum velocity the hammer shank obtained during travel
//        toward string is an approximation of the velocity when hit the
//        string. Playability tests indicate this approxiation works ok.
//        However, more sophisticated algorithms are under consideration.
// TODO - The effect of repetition_counter_ and released_ overlap.
//        Need both?

#include "dsp_hammer.h"

DspHammer::DspHammer() {}

void DspHammer::Setup(int sample_period, float strike_threshold,
float release_threshold, float min_repetition_seconds, float min_strike_velocity, 
float hammer_travel_meters, int initialize_wait_count, int debug_level) {

  debug_level_ = debug_level;
  
  samples_per_second_ = static_cast<int>(1.0/(sample_period*1e-6));

  strike_threshold_ = strike_threshold;
  release_threshold_ = release_threshold;

  min_repetition_samples_ = static_cast<int>(min_repetition_seconds *
  (float) samples_per_second_);

  min_strike_velocity_ = min_strike_velocity;

  hammer_travel_meters_ = hammer_travel_meters;

  initialize_wait_count_ = initialize_wait_count;
  initialize_counter_ = 0;

  buffer_pointer_ = 0;
  for (int key = 0; key < NUM_CHANNELS; key++) {
    max_position_[key] = 0.0;
    max_velocity_[key] = 0.0;
    repetition_counter_[key] = 0;
    min_max_position_valid_[key] = false;
    hammer_threshold_[key] = strike_threshold_;
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
 
  // Wait after a reset juuuust to make sure don't get spurious ADC values.
  if (initialize_counter_ > initialize_wait_count_) {
    ComputeDerivative(position);
    UpdateHammerRange(position);
    DetectReleased(position);
    UpdateHammerThresholds();
    DetectHammerStrike(event, velocity, position);
    UpdateMaxHammerVelocity();  // Must be called AFTER DetectHammerStrike().
    DetectFirstHammer(event);
  }
  else {
    initialize_counter_++;
  }

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
    // Convert numerator of velocity to meters.
    velocity_[key] *= hammer_travel_meters_ / max_position_[key];
    // Remove the high-pass + boxcar filter scaling.
    velocity_[key] /= static_cast<float>(DERIVATIVE_AVERAGE_SAMPLES);
    velocity_buffer_[key][buffer_pointer_] = position[key];
  }
  buffer_pointer_++;
  if (buffer_pointer_ == DERIVATIVE_AVERAGE_SAMPLES) {
    buffer_pointer_ = 0;
  }
}

// Keep track of min and max hammer position.
void DspHammer::UpdateHammerRange(const float *position) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (position[key] > max_position_[key]) {
      max_position_[key] = position[key];
      if (debug_level_ >= 2) {
        Serial.print("UpdateHammerRange() ");
        Serial.print(" key=");
        Serial.print(key);
        Serial.print(" max_position_=");
        Serial.print(max_position_[key]);
        Serial.println("");
      }
    }
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

// Set the damper up/down threshold based on the min/max
// position as measured by the sensor. If the min/max position
// has yet to be determined, then use the threshold directly.
void DspHammer::UpdateHammerThresholds() {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (min_max_position_valid_[key] == true) {
      hammer_threshold_[key] = max_position_[key] * strike_threshold_;
    }
    else {
      hammer_threshold_[key] = strike_threshold_;
    }
  }
}

// Algorithm for detecting if a hammer struck the imaginary string.
void DspHammer::DetectHammerStrike(bool *event, float *velocity, const float *position) {

  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (enable_ == true && 
    // To avoid noise while near rest, check if hammer is close to string...
    position[key] >= hammer_threshold_[key] &&
    // and check if the hammer just bounced off something...
    velocity_[key] <= 0 &&
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
      if (debug_level_ >= 2) {
        Serial.println("DetectHammerStrike()");
        Serial.print("  key=");
        Serial.print(key);
        Serial.print(" pos=");
        Serial.print(position[key]);
        Serial.print(" thresh=");
        Serial.print(hammer_threshold_[key]);
        Serial.print(" maxv=");
        Serial.print(max_velocity_[key]);
        Serial.print(" repc=");
        Serial.print(repetition_counter_[key]);
        Serial.println("");
      }

      // Hooray, we got a hammer strike on virtual string!
      velocity[key] = max_velocity_[key];   // Velocity of strike = max when rising.
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

// Until a key is pressed once, the min/max positions will be incorrect.
// Therefore, check and set a flag that the min/max positions are valid.
void DspHammer::DetectFirstHammer(const bool *event) {
  for (int key = 0; key < NUM_CHANNELS; key++) {
    if (event[key] == true) {
      if (debug_level_ >= 2) {
        if (min_max_position_valid_[key] == false) {
          Serial.print("DetectFirstHammer() ");
          Serial.print(" key=");
          Serial.print(key);
          Serial.print(" min_max_position_valid_ is now true");
          Serial.println("");
        }
      }
      min_max_position_valid_[key] = true;
    }
  }
}

void DspHammer::Enable(bool enable) {
  enable_ = enable;
}