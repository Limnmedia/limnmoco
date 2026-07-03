// SPDX-License-Identifier: GPL-3.0-or-later
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com) 
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file config.hpp
 * \brief limnmoco configuration for the core m4 co-processor
 *
 * \note all signals are 3.3V TTL
 */

#ifndef LIMNMOCO_CM4_CONFIG_HPP
#define LIMNMOCO_CM4_CONFIG_HPP

#include <cstdint>

#ifdef ARDUINO_ARCH_MBED_GIGA

// #NOTE: ensure pins do not overlap with pins used by the CM7 main processor
//   defined in cm7/config.hpp

#define LIMNMOCO_PIN_STEP_0 22
#define LIMNMOCO_PIN_STEP_1 24
#define LIMNMOCO_PIN_STEP_2 26
#define LIMNMOCO_PIN_STEP_3 28
#define LIMNMOCO_PIN_STEP_4 30
#define LIMNMOCO_PIN_STEP_5 32
#define LIMNMOCO_PIN_STEP_6 34
#define LIMNMOCO_PIN_STEP_7 36

extern uint8_t *limnmoco_pin_step;

#define LIMNMOCO_PIN_DIRECTION_0 23
#define LIMNMOCO_PIN_DIRECTION_1 25
#define LIMNMOCO_PIN_DIRECTION_2 27
#define LIMNMOCO_PIN_DIRECTION_3 29
#define LIMNMOCO_PIN_DIRECTION_4 31
#define LIMNMOCO_PIN_DIRECTION_5 33
#define LIMNMOCO_PIN_DIRECTION_6 35
#define LIMNMOCO_PIN_DIRECTION_7 37

extern uint8_t *limnmoco_pin_direction;

#define LIMNMOCO_PIN_CAMERA_METER   52
#define LIMNMOCO_PIN_CAMERA_SHUTTER 53

#define LIMNMOCO_CAMERA_OFF     0x0
#define LIMNMOCO_CAMERA_SHUTTER 0x1
#define LIMNMOCO_CAMERA_METER   0x2

// #NOTE: keep in sync with LIMNMOCO_MOTOR_COUNT in cm7/config.hpp
#define LIMNMOCO_MOTOR_COUNT 8

// #NOTE:the maximum number of steps per second for a given motor
#define LIMNMOCO_MOTOR_MAX_VELOCITY 200'000

#else
#error "Board unsupported for arduino sketch"
#endif // !ARDUINO_ARCH_MBED_GIGA

#ifndef CORE_CM4
#error "Make sure to target the core M4 co-processor with flash memory split 1.5MiB CM7 + 0.5MiB CM4"
#endif

#endif // !LIMNMOCO_CM4_CONFIG_HPP


