// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_DEBUG_HPP
#define LIMNMOCO_CM7_DEBUG_HPP

#include <cstdint>

#define PIN_DBG_0 2
#define PIN_DBG_1 3
#define PIN_DBG_2 4
#define PIN_DBG_3 5
#define PIN_DBG_4 6
#define PIN_DBG_5 7
#define PIN_DBG_6 8
#define PIN_DBG_7 9

void debug_pin_enable(uint8_t pin);
void debug_pulse(uint8_t pin);

#endif // !LIMNMOCO_CM7_DEBUG_HPP
