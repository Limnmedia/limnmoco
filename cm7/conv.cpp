// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>
#include <cmath>

#include "conv.hpp"

uint32_t strlenu(uint32_t u) {
    if (u < 10) return 1;
    if (u < 100) return 2;
    if (u < 1000) return 3;
    if (u < 10000) return 4;
    if (u < 100000) return 5;
    if (u < 1000000) return 6;
    if (u < 10000000) return 7;
    if (u < 100000000) return 8;
    if (u < 1000000000) return 9;
    return 10;
}

uint32_t strleni(int32_t i) {
    if (i < 0) { 
        return 1 + strlenu((uint32_t)(-i));
    }

    return strlenu((uint32_t)i);
}

uint32_t strlenf(float n) {
    if (std::isnan(n)) return 3; // "nan"
    if (std::isinf(n)) return (n < 0.0) ? 4 : 3; // "-inf" or "inf"
    if (n == 0.0) return 1; // "0"

    uint32_t count = 0;
    if (n < 0.0) {
        n = -n;
        count++; // '-'
    }

    uint32_t integer = (uint32_t)n;
    count           += strlenu(integer);
    float remainder  = n - (float)integer;

    count++; // '.'

    remainder *= (1000000.0);
    integer    = (uint32_t)remainder;
    count     += strlenu(integer);
    return count;
}

uint32_t convu(uint8_t *out, size_t length, uint32_t n, uint8_t base) {
    size_t count = 0;
    uint32_t limit = strlenu(n);
    if (length < limit) { return count; }
    uint8_t buf[limit];
    uint8_t *cur = buf + (limit - 1);
    *cur = '\0';
    if (base < 2) { base = 10; }
    do {
        char c = n % base;
        n /= base;
        count++;
        *--cur = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);

    while (cur < (buf + limit)) {
        *out++ = *cur++;
    }

    return count;
}

uint32_t convi(uint8_t *out, size_t length, int32_t n, uint8_t base) {
    uint32_t count = 0;
    uint32_t limit = strleni(n);
    if (length < limit) { return count; }
    if (n < 0) {
        count++;
        *++out = '-';
        n = -n;
    }

    return convu(out, length, (uint32_t)n, base);
}

uint32_t convf(uint8_t *out, size_t length, float n) {
    uint8_t *cur = out;
    uint32_t count = 0;
    uint32_t limit = strlenf(n);
    if (length < limit) { return count; }

    if (std::isnan(n)) {
        memcpy(out, "nan", sizeof("nan"));
        return 3;
    }

    if (std::isinf(n)) {
        if (n < 0.0) {
            memcpy(out, "-inf", sizeof("-inf"));
            return 4;
        }
        memcpy(out, "inf", sizeof("inf"));
        return 3;
    }

    if (n < 0.0) {
        count++;
        *cur++ = '-';
        n = -n;
    }

    uint32_t int_part  = (uint32_t)n;
    uint32_t int_limit = strlenu(int_part);
    float remainder    = n - (float)int_part;
    count             += convu(cur, int_limit, int_part, 10);
    cur               += int_limit - 1;

    count++;
    *cur++ = '.';

    for (uint8_t i = 0; i < 6; ++i) {
        remainder *= 10;
        int_part   = (uint32_t)remainder;
        int_limit  = strlenu(int_part);
        count     += convu(cur, int_limit, int_part, 10);
        cur       += 1;
        remainder -= (float)int_part;
    }

    return count;
}

