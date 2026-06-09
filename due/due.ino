// SPDX-License-Identifier: BSD-3-Clause

#include "dmc_debug.hpp"

#define SERIAL_BAUD 115200

DmcDebug dmc;

void setup() {
    Serial.begin(SERIAL_BAUD);
    SerialUSB.begin(SERIAL_BAUD);

    while (!Serial || !SerialUSB) {}

    dmc.bind(SerialUSB);
    dmc.bindDebug(Serial);
}

void loop() {
    dmc.receive();
    dmc.transmit();
    dmc.transmitDebug();
}

