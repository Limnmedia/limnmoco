// SPDX-License-Identifier: BSD-3-Clause

#include "dmc_debug.hpp"
#include "debug.hpp"

#define SERIAL_BAUD 115200

DmcDebug dmc;

void setup() {
    debug_pin_enable(PIN_DBG_0);
    debug_pin_enable(PIN_DBG_1);
    debug_pin_enable(PIN_DBG_2);
    debug_pin_enable(PIN_DBG_3);
    debug_pin_enable(PIN_DBG_4);
    debug_pin_enable(PIN_DBG_5);
    debug_pin_enable(PIN_DBG_6);
    debug_pin_enable(PIN_DBG_7);

    debug_pulse(PIN_DBG_0);
    debug_pulse(PIN_DBG_1);
    debug_pulse(PIN_DBG_2);
    debug_pulse(PIN_DBG_3);
    debug_pulse(PIN_DBG_4);
    debug_pulse(PIN_DBG_5);
    debug_pulse(PIN_DBG_6);
    debug_pulse(PIN_DBG_7);

    Serial.begin(SERIAL_BAUD);

    // Serial USB transfers data in blocks of 1024, with an interval
    // of some multiple of 125us
    SerialUSB.begin(SERIAL_BAUD);
    // while (!Serial || !SerialUSB) {}
    dmc.bind(SerialUSB);
    dmc.bindDebug(Serial);
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

    dmc.receive();
    dmc.transmit();
    dmc.transmitDebug();
}

