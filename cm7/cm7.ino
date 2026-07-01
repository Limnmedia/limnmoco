// SPDX-License-Identifier: GPL_3.0-or-later

//#include <RPC.h>

#include "config.hpp"
#include "dmc_bus.hpp"
#include "global.hpp"
#include "shared.hpp"

void setup() {
  SharedData::begin();

#ifndef LIMNMOCO_DEBUG
  Serial.begin(LIMNMOCO_SERIAL_BAUD);
#endif // !LIMNMOCO_DEBUG

  SerialUSB.begin(LIMNMOCO_SERIAL_BAUD);

  dmc_bus.bind(SerialUSB);
#ifndef LIMNMOCO_DEBUG
  dmc_bus.bindPrint(Serial);
#endif // !LIMNMOCO_DEBUG

//  RPC.begin();
//  RPC.bind("get_shared_data", get_shared_data);

}

void loop() {
  dmc_bus.update();
  global.update();
}
