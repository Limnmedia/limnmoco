// SPDX-License-Identifier: GPL_3.0-or-later

#include "config.hpp"
#include "dmc_bus.hpp"

DmcBus dmc_bus;
Gio gio;

void setup() {
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
  dmc_bus.receive();
  dmc_bus.transmit();
#ifdef LIMNMOCO_DEBUG
  dmc_bus.transmitPrint();
#endif // !LIMNMOCO_DEBUG


}
