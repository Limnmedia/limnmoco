// SPDX-License-Identifier: BSD-3-Clause

#include "dmc_debug.hpp"
#include "debug.hpp"

#define SERIAL_BAUD 115200

DmcDebug dmc;

void setup() {
    Serial.begin(SERIAL_BAUD);
    SerialUSB.begin(SERIAL_BAUD);
    // while (!Serial || !SerialUSB) {}
    dmc.bind(SerialUSB);
    dmc.bindDebug(Serial);

    pinMode(PIN_DBG_0, OUTPUT);
    pinMode(PIN_DBG_1, OUTPUT);
    pinMode(PIN_DBG_2, OUTPUT);
    pinMode(PIN_DBG_3, OUTPUT);
    pinMode(PIN_DBG_4, OUTPUT);
    pinMode(PIN_DBG_5, OUTPUT);
    pinMode(PIN_DBG_6, OUTPUT);
    pinMode(PIN_DBG_7, OUTPUT);
}

void loop() {
    dmc.receive();
    dmc.transmit();
    dmc.transmitDebug();
}

