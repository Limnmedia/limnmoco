// SPDX-License-Identifier: GPL-3.0-or-later

#include <Arduino.h>

#include "debug.hpp"

void debug_pin_enable(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void debug_pulse(uint8_t pin) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(1);
  digitalWrite(pin, LOW);
}

