// SPDX-License-Identifier: BSD-3-Clause


#ifndef LIMNMOCO_CONV_HPP
#define LIMNMOCO_CONV_HPP

#include <cstdint>

#define INT_MAX_STRLEN (8 * sizeof(long) + 1)
#define FLOAT_MAX_STRLEN (24)

bool convi(uint8_t *out, size_t length, uint64_t n, uint8_t base);
bool convf(uint8_t *out, size_t length, double n, uint8_t digits);

#endif // !LIMNMOCO_CONV_HPP

