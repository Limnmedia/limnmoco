// SPDX-License-Identifier: BSD-3-Clause


#include <Arduino.h>

#include "7seg.h"
#include "config.h"

const uint8_t digits[16][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 0, 0, 1, 1}, // 9
    {1, 1, 1, 0, 1, 1, 1}, // A
    {0, 0, 1, 1, 1, 1, 1}, // B
    {1, 0, 0, 1, 1, 1, 0}, // C
    {0, 1, 1, 1, 1, 0, 1}, // D
    {1, 0, 0, 1, 1, 1, 1}, // E
    {1, 0, 0, 0, 1, 1, 1}, // F
};

void setup_7seg() {
    pinMode(PIN_7SEG_A, OUTPUT);
    pinMode(PIN_7SEG_B, OUTPUT);
    pinMode(PIN_7SEG_C, OUTPUT);
    pinMode(PIN_7SEG_D, OUTPUT);
    pinMode(PIN_7SEG_E, OUTPUT);
    pinMode(PIN_7SEG_F, OUTPUT);
    pinMode(PIN_7SEG_G, OUTPUT);
    pinMode(PIN_7SEG_DP, OUTPUT);

    pinMode(PIN_7SEG_DIG1, OUTPUT);
    pinMode(PIN_7SEG_DIG2, OUTPUT);
    pinMode(PIN_7SEG_DIG3, OUTPUT);
    pinMode(PIN_7SEG_DIG4, OUTPUT);
}

void write_7seg(uint8_t data, uint8_t digit);
void write_7seg(uint16_t data);
