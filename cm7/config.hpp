// SPDX-License-Identifier: GPL-3.0-or-later
//
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com)
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file config.hpp
 * \brief limnmoco configuration for the core m7
 */

#ifndef LIMNMOCO_CM7_CONFIG_HPP
#define LIMNMOCO_CM7_CONFIG_HPP

#ifdef ARDUINO_ARCH_MBED_GIGA

#define LIMNMOCO_DEBUG

#define LIMNMOCO_DEVICE_NAME      "limnmoco"
#define LIMNMOCO_FW_MAJOR         0
#define LIMNMOCO_FW_MINOR         1
#define LIMNMOCO_FW_REV           2
#define LIMNMOCO_MOTOR_COUNT      8
#define LIMNMOCO_DMX_COUNT        0
#define LIMNMOCO_GIO_OUT          2
#define LIMNMOCO_GIO_IN           1
#define LIMNMOCO_HW_LIMIT         1
#define LIMNMOCO_FRAME_COUNT      10000
// DMC_CAP_REAL_TIME | DMC_CAP_GO_MOTION | DMC_CAP_GO_MOTION_2 | DMC_CAP_COUPLE_MOTORS
// DMC_CAP_REAL_TIME_LOOP | DMC_CAP_REAL_TIME_CAMERA | DMC_CAP_VIRTUAL_BOOM_SWING_TRACK
// 0x0001 | 0x0002 | 0x0004 | 0x0080 | 0x0100 | 0x0200 | 0x0400
// 0x0787
#define LIMNMOCO_CAPABILITIES     0x0787
#define LIMNMOCO_PROTOCOL_VERSION 2

// #NOTE: logic output is 3.3V TTL. Generally don't use to drive current directly.
//   Using a transistor to isolate the chip from large current draw is highly reccommended.
#define LIMNMOCO_LOGIC_OUT_0 D40
#define LIMNMOCO_LOGIC_OUT_1 D41
uint8_t limnmoco_logic_out[LIMNMOCO_GIO_OUT] = { D40, D41 };

// #NOTE: logic input uses internal pull-up, so switch needs to connect to ground.
#define LIMNMOCO_LOGIC_IN_0 D49
uint8_t limnmoco_logic_in[LIMNMOCO_GIO_IN] = { D49 };

// #NOTE: e-stop input uses internal pull-up, so switch needs to connect to ground.
#define LIMNMOCO_KILL_SWITCH D48

// #NOTE: e-stop switch normally closed -> 1 | normally open -> 0
#define LIMNMOCO_KILL_SWITCH_NORMALLY_CLOSED 0

#define LIMNMOCO_SERIAL_BAUD 115200

#else
#error "Board not supported for limnmoco sketch"
#endif // !ARDUINO_ARCH_MBED_GIGA

#endif // !LIMNMOCO_CM7_CONFIG_HPP


