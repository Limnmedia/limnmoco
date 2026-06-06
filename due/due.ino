// SPDX-License-Identifier: BSD-3-Clause

#include "dmc_debug.hpp"

#define SERIAL_BAUD 115200

uint8_t name[32] = "limnmoco-debug";
uint8_t fw_major = 0;
uint8_t fw_minor = 1;
uint8_t fw_rev   = 0;
uint8_t motor_count = 8;
uint16_t dmx_count = 512;
uint8_t gio_out_count = 10;
uint8_t gio_in_count  = 10;
uint8_t hw_limit_count = 10;
uint32_t frame_count   = 1000;
uint32_t capabilities = 0xFFFF; // stub all capabilities.
uint16_t protocol_version = 2;

DmcDebug dmc;
DmcHiResponse device(
  name,
  fw_major,
  fw_minor,
  fw_rev,
  motor_count,
  dmx_count,
  gio_out_count,
  gio_in_count,
  hw_limit_count,
  frame_count,
  capabilities,
  protocol_version
);

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial1.begin(SERIAL_BAUD);

    dmc.setDevice(&device);
    dmc.bind(Serial1);
    dmc.bindDebug(Serial);
}

void loop() {
    dmc.receive();
}

