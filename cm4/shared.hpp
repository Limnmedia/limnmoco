// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_SHARED_HPP
#define LIMNMOCO_CM7_SHARED_HPP

#include <cstdint>
#include <bitset>

#include "config.hpp"

// #NOTE: Keep in sync with cm7/shared.hpp

namespace shared {

using Direction = std::bitset<LIMNMOCO_MOTOR_COUNT>;

struct Data {
  volatile Direction motor_direction;
  volatile int32_t   motor_position[LIMNMOCO_MOTOR_COUNT];
  volatile uint32_t  motor_velocity[LIMNMOCO_MOTOR_COUNT];
  volatile uint32_t  motor_acceleration[LIMNMOCO_MOTOR_COUNT];
};

void begin();
extern Data *ptr;

} // namespace shared


#endif // !LIMNMOCO_CM7_SHARED_HPP


