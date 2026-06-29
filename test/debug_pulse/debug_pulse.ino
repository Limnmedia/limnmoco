// SPDX-License_Identifier: GPL-3.0-or-later

#include "debug.hpp"

void setup() {
  debug_pin_enable(PIN_DBG_0);
  debug_pin_enable(PIN_DBG_1);
  debug_pin_enable(PIN_DBG_2);
  debug_pin_enable(PIN_DBG_3);
  debug_pin_enable(PIN_DBG_4);
  debug_pin_enable(PIN_DBG_5);
  debug_pin_enable(PIN_DBG_6);
  debug_pin_enable(PIN_DBG_7);
}

void loop() {
  debug_pulse(PIN_DBG_0);
  debug_pulse(PIN_DBG_1);
  debug_pulse(PIN_DBG_2);
  debug_pulse(PIN_DBG_3);
  debug_pulse(PIN_DBG_4);
  debug_pulse(PIN_DBG_5);
  debug_pulse(PIN_DBG_6);
  debug_pulse(PIN_DBG_7);
  delay(10);
}
