// SPDX-License-Identifier: BSD-3-Clause


#ifndef LIMNMOCO_CONV_HPP
#define LIMNMOCO_CONV_HPP

#include <cstdint>

uint32_t strlenu(uint32_t n);
uint32_t strleni(int32_t n);
uint32_t strlenf(float n);

uint32_t convu(uint8_t *out, size_t length, uint32_t n, uint8_t base);
uint32_t convi(uint8_t *out, size_t length, int32_t n, uint8_t base);
uint32_t convf(uint8_t *out, size_t length, float n);

#endif // !LIMNMOCO_CONV_HPP

