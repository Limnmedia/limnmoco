// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_SHARED_HPP
#define LIMNMOCO_CM7_SHARED_HPP

#include <cstdint>

#include "config.hpp"

// #NOTE: Keep in sync with cm7/shared.hpp

struct SharedData {
  static void begin();
  static SharedData *get();
  static SharedData *ptr;

  volatile uint32_t motor_velocity[LIMNMOCO_MOTOR_COUNT];
  volatile uint32_t motor_direction;
  volatile uint32_t camera_velocity;
  volatile uint16_t camera_angle_open;
  volatile uint16_t camera_angle_close;
  volatile uint8_t  camera_value;
};

uintptr_t get_shared_data();

#endif // !LIMNMOCO_CM7_SHARED_HPP


