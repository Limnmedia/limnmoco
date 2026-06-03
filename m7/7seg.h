// SPDX-License-Identifier: BSD-3-Clause

#include <cstdint>

// init 7 seg driver
// write a char to a digit
// write a short to all 4 digits

void setup_7seg();

void write_7seg(uint8_t data, uint8_t digit);
void write_7seg(uint16_t data);
