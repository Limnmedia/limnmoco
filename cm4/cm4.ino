// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * \file cm4.ino
 * \brief limnmoco core m4 co-processor sketch
 *
 * \note ensure that the target core is the M4 co-processor
 * and the flash memory split is 1.5MiB CM7 + 0.5MiB CM4
 */

#include <RPC.h>

#include "shared.hpp"
#include "motors.hpp"

Motors motors;

void setup() {
  RPC.begin();
  RPC.bind("set_direction", motor::set_direction);

  Shared::begin();
  motors.begin();
}

void loop() {
  delay(1);

  // #NOTE: is this really okay? we are blindly updating the motor directions in 
  //        a tight loop? why? lets hold a copy of the direction in the motor struct
  //        which we can use to compare against the copy in the shared data, that way
  //        the cm7 core can communicate the requested direction, and we can update the 
  //        motors only if it's needed and only if it's safe to do so.
  motors.update_motor_directions();
}

