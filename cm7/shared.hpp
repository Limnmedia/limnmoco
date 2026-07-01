// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_SHARED_HPP
#define LIMNMOCO_CM7_SHARED_HPP

#include <cstdint>

#include "config.hpp"

// #NOTE: Keep in sync with cm4/shared.hpp

struct SharedData {
  static void begin();
  static SharedData *get();
  static SharedData *ptr;

  volatile int64_t  motor_next_speed[LIMNMOCO_MOTOR_COUNT];
  volatile int64_t  motor_accumulator[LIMNMOCO_MOTOR_COUNT];
  volatile uint32_t motor_data_loaded;
  volatile uint32_t motor_direction;
  volatile int64_t  camera_next_speed;
  volatile int64_t  camera_accumulator;
  volatile uint16_t camera_open_angle;
  volatile uint16_t camera_close_angle;
  volatile uint8_t  camera_value;
};

uintptr_t get_shared_data();

#endif // !LIMNMOCO_CM7_SHARED_HPP


