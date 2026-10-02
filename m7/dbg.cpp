// SPDX-License-Identifier: BSD-3-clause

#include <Arduino.h>

#include "dbg.h"

void dbg(uint8_t pin) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(1);
  digitalWrite(pin, LOW);
}

void dbg_pin(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

