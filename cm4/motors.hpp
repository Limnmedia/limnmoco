// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_MOTORS_HPP
#define LIMNMOCO_CM4_MOTORS_HPP

#include "status.h"

// okay, given that:
// 1. we only control N motors
// 2. we control all motors with the same update step
// 3. we are working on a single piece of hardware. that we would need major lifting to port elsewhere
//      (unless it's built around the same chipset, yada yada.)
// 
// why use classes at all? what organization does it bring?
// we could use a class for a single motor. but then, how do 
// we use a single bitset for all motor directions w/o breaking 
// encapsulation? 
// if we are using a single array for motors direction/veloctiy/acceleration
// then what use is it placing that code into a class?
// A namespace gives us the same overhead as a class, without the construction 
// issues. We can call all constructors in the main setup, which will stop hiding 
// away initialization across multiple files.:

namespace motors {

/**
 * \brief initialize global state for the motors and begin heartbeat
 */
void begin();

/**
 * \brief set each available motor direction
 *
 * \note direction is a bitset of the motor direction booleans.
 */
int32_t set_direction(uint32_t direction);

} // namespace motors

#endif // !LIMNMOCO_CM4_MOTORS_HPP

