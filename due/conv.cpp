// SPDX-License-Identifier: BSD-3-Clause

#include <cstring>
#include <cmath>

#include "conv.hpp"

size_t convi(uint8_t *out, size_t length, uint64_t n, uint8_t base) {
    size_t count = 0;
    if (length < INT_MAX_STRLEN) { return count; }
    out = out + (length - 1);
    *out = '\0';
    if (base < 2) { base = 10; }
    do {
        char c = n % base;
        n /= base;
        count++;
        *--out = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);

    return count;
}

size_t convf(uint8_t *out, size_t length, double n, uint8_t digits) {
    size_t count = 0;

    if (length < FLOAT_MAX_STRLEN) { return count; }

    if (isnan(n)) {
        memcpy(out, "nan", sizeof("nan"));
        return true;
    }

    if (isinf(n)) {
        memcpy(out, "inf", sizeof("inf"));
        return true;
    }

    if (n > 429467040.0) {
        memcpy(out, "ovf", sizeof("ovf"));
        return true;
    }

    if (n < -429467040.0) {
        memcpy(out, "ovf", sizeof("ovf"));
        return true;
    }

    if (n < 0.0) {
        count++;
        *++out = '-';
        n = -n;
    }

    double rounding = 0.5;
    for (uint8_t i = 0; i < digits; ++i) {
        rounding /= 10.0;
    }
    n += rounding;

    uint64_t int_part = (uint64_t)n;
    double remainder  = n - (double)int_part;
    count += convi(out, length, int_part, 10);
    out += count;

    if (digits > 0) {
        count++;
        *++out = '.';
    }

    while (digits-- > 0) {
        remainder *= 10.0;
        uint64_t int_part = (uint64_t)remainder;
        count += convi(out, length, int_part, 10);
        out += count;
        remainder -= (double)int_part;
    }

    return count;
}

