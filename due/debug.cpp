// SPDX-License-Identifier: BSD-3-clause

#include <Arduino.h>

#include "debug.hpp"

void debug_pulse(uint8_t pin) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(100);
  digitalWrite(pin, LOW);
}

