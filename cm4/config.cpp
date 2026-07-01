// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.hpp"

uint8_t limnmoco_pin_step_buffer[LIMNMOCO_MOTOR_COUNT] = {
    LIMNMOCO_PIN_STEP_0,
    LIMNMOCO_PIN_STEP_1,
    LIMNMOCO_PIN_STEP_2,
    LIMNMOCO_PIN_STEP_3,
    LIMNMOCO_PIN_STEP_4,
    LIMNMOCO_PIN_STEP_5,
    LIMNMOCO_PIN_STEP_6,
    LIMNMOCO_PIN_STEP_7,
};

uint8_t limnmoco_pin_direction_buffer[LIMNMOCO_MOTOR_COUNT] = {
    LIMNMOCO_PIN_DIRECTION_0,
    LIMNMOCO_PIN_DIRECTION_1,
    LIMNMOCO_PIN_DIRECTION_2,
    LIMNMOCO_PIN_DIRECTION_3,
    LIMNMOCO_PIN_DIRECTION_4,
    LIMNMOCO_PIN_DIRECTION_5,
    LIMNMOCO_PIN_DIRECTION_6,
    LIMNMOCO_PIN_DIRECTION_7,
};

uint8_t *limnmoco_pin_step      = limnmoco_pin_step_buffer;
uint8_t *limnmoco_pin_direction = limnmoco_pin_direction_buffer;


