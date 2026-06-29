// SPDX-License-Identifier: BSD-3-clause

#include <Arduino.h>

#include "debug.hpp"

void debug_pin_enable(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void debug_pulse(uint8_t pin) {
  digitalWrite(pin, HIGH);
  //delayMicroseconds(10);
  for (uint8_t i = 0; i < 10; ++i) {}
  digitalWrite(pin, LOW);
}

