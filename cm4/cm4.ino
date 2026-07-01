// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * \file cm4.ino
 * \brief limnmoco core m4 co-processor sketch
 *
 * \note ensure that the target core is the M4 co-processor
 * and the flash memory split is 1.5MiB CM7 + 0.5MiB CM4
 */

#include <RPC.h>

#include "config.hpp"
#include "shared.hpp"
#include "timer.hpp"
#include "global.hpp"


void setup() {
  SharedData::begin();
  Timer::begin();

  global.begin();

  // why the RPC if the shared data is placed into a static memory location
  // that is hard coded between the cores?
  //RPC.begin();
  //RPC.call("get_shared_data").as<uintptr_t>();
}


void loop() {
  global.update();
}

