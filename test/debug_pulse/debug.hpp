// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_DUE_DEBUG_HPP
#define LIMNMOCO_DUE_DEBUG_HPP

#include <cstdint>

#define PIN_DBG_0 0
#define PIN_DBG_1 1
#define PIN_DBG_2 2
#define PIN_DBG_3 3
#define PIN_DBG_4 4
#define PIN_DBG_5 5
#define PIN_DBG_6 6
#define PIN_DBG_7 7

void debug_pin_enable(uint8_t pin);
void debug_pulse(uint8_t pin);

#endif // !LIMNMOCO_DUE_DEBUG_HPP
