// SPDX-License-Identifier: GPL_3.0-or-later

#include <RPC.h>

#include "config.hpp"
#include "dmc_bus.hpp"
#include "global.hpp"
#include "shared.hpp"

void setup() {
  RPC.begin(); // this contains the code to boot the m4 core

  shared::begin();

#ifndef LIMNMOCO_DEBUG
  Serial.begin(LIMNMOCO_SERIAL_BAUD);
#endif // !LIMNMOCO_DEBUG

  SerialUSB.begin(LIMNMOCO_SERIAL_BAUD);

  dmc_bus.bind(SerialUSB);
#ifndef LIMNMOCO_DEBUG
  dmc_bus.bindPrint(Serial);
#endif // !LIMNMOCO_DEBUG
}

void loop() {
  dmc_bus.update();
  global.update();
}
