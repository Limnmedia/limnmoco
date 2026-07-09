// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * \file cm4.ino
 * \brief limnmoco core m4 co-processor sketch
 *
 * \note ensure that the target core is the M4 co-processor
 * and the flash memory split is 1.5MiB CM7 + 0.5MiB CM4
 */

//#include <RPC.h>

#include "shared.hpp"
#include "motors.hpp"

void setup() {
  RPC.begin();
  RPC.bind("set_direction", motor::set_direction);

  shared::begin();
  motors::begin();
}

void loop() {
  delay(1);
}

