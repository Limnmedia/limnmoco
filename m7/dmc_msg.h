// SPDX-License-Identifier: BSD-3-Clause
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com) 
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file dmc_msg.h
 * \brief limnmoco dmc protocol message handling functions for the m7 core
 */

#ifndef SRC_DMC_MSG_H_
#define SRC_DMC_MSG_H_

#include <stdint.h>

#define DMC_DMX_FLAG_FINAL_SET 0x80000000

#define DMC_MSG_FLAG_ACK 0x8000

#define DMC_MSG_HI 0x0001
#define DMC_MSG_DMX 0x0020
#define DMC_MSG_GIO_OUT 0x0021
#define DMC_MSG_GIO_IN 0x0022
#define DMC_MSG_GIO_CAM 0x0023
#define DMC_MSG_MOTOR_STATUS 0x0030
#define DMC_MSG_MOTOR_MOVE 0x0031
#define DMC_MSG_MOTOR_STOP 0x0032
#define DMC_MSG_MOTOR_STOP_ALL 0x0033
#define DMC_MSG_MOTOR_GET_POSITION 0x0034
#define DMC_MSG_MOTOR_RESET_POSITION 0x0035
#define DMC_MSG_MOTOR_JOG 0x0036
#define DMC_MSG_MOTOR_CONFIGURE 0x0037
#define DMC_MSG_MOTOR_SET_SPEED 0x0038
#define DMC_MSG_MOTOR_SET_LIMITS 0x0039
#define DMC_MSG_MOTOR_HARD_STOP 0x003A

#define DMC_MSG_RT_UPLOAD_MOVE_BEGIN 0x0100
#define DMC_MSG_RT_UPLOAD_MOVE_AXIS 0x0101
#define DMC_MSG_RT_UPLOAD_MOVE_DMX 0x0102
#define DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS 0x0104
#define DMC_MSG_RT_UPLOAD_MOVE_END 0x0103

#define DMC_MSG_RT_POSITION_FRAME 0x0110
#define DMC_MSG_RT_RUN_MOVE 0x0111
#define DMC_MSG_RT_SHOOT_FRAME 0x0112
#define DMC_MSG_RT_SHOOT_FRAME2 0x0115
#define DMC_MSG_RT_GO 0x0113
#define DMC_MSG_RT_END 0x0114
#define DMC_MSG_RT_STOP_LOOP 0x0116
#define DMC_MSG_RT_JOG_ALL 0x0120

#define DMC_MSG_VIRT_CONFIG 0x0200
#define DMC_MSG_VIRT_MOVE 0x0201
#define DMC_MSG_VIRT_STOP 0x0202
#define DMC_MSG_VIRT_JOG 0x0203
#define DMC_MSG_VIRT_GET_POSITION 0x0205
#define DMC_MSG_VIRT_JOG_ON_LINE 0x0206
#define DMC_MSG_VIRT_AIM_POINT 0x0207

#define DMC_MSG_FAN_CONTROL 0x0300

// ack/error codes
#define DMC_ACK_OK 0x0010
#define DMC_ACK_ERR_CHECKSUM 0x0011
#define DMC_ACK_ERR_MOVING 0x0012
#define DMC_ACK_ERR_UNSUPPORTED 0x0013
#define DMC_ACK_ERR_RANGE 0x0014
#define DMC_ACK_ERR_GENERAL 0x0015
#define DMC_ACK_ERR_NOT_IN_POSITION 0x0016
#define DMC_ACK_ERR_PREROLL 0x0017
#define DMC_ACK_ERR_POSTROLL 0x0018
#define DMC_ACK_ERR_AIM_COD 0x0019

#define DMC_ACK_ERR_SOFT_UP 0x0020
#define DMC_ACK_ERR_SOFT_LOW 0x0021
#define DMC_ACK_ERR_HARD_UP 0x0022
#define DMC_ACK_ERR_HARD_LOW 0x0023

// capabilities
#define DMC_CAP_REAL_TIME 0x0001
#define DMC_CAP_GO_MOTION 0x0002
#define DMC_CAP_VIRTUAL_BOOM_SWING_TRACK 0x0004
#define DMC_CAP_VIRTUAL_SWING_PAN 0x0008
#define DMC_CAP_VIRTUAL_Y_SWING_TRACK 0x0010
#define DMC_CAP_VIRTUAL_X_Y_Z 0x0020
#define DMC_CAP_AIM_POINT 0x0040
#define DMC_CAP_GO_MOTION2 0x0080
#define DMC_CAP_COUPLE_MOTORS 0x0100
#define DMC_CAP_REAL_TIME_LOOP   0x0200
#define DMC_CAP_REAL_TIME_CAMERA 0x0400

// motor configuration flags
#define DMC_MOTOR_CONFIG_ENABLED 0x01
#define DMC_MOTOR_CONFIG_BLUR 0x02
#define DMC_MOTOR_CONFIG_VIRT 0x04 // this axis is used IN virtuals
#define DMC_MOTOR_CONFIG_LIVE_CONTROL 0x08
#define DMC_MOTOR_CONFIG_COUPLE 0x10
#define DMC_MOTOR_CONFIG_COUPLE_R 0x20

#define DMC_GIO_CAM_SHUTTER_FLAG 0x0001
#define DMC_GIO_CAM_METER_FLAG 0x0002

// real-time playback flags
#define DMC_RT_PLAYBACK_PING_PONG 0x0001
#define DMC_RT_PLAYBACK_LOOP      0x0002
#define DMC_RT_CAMERA_VIDEO       0x0010
#define DMC_RT_CAMERA_STILLS      0x0020

// virtual config support types
#define DMC_VIRT_TYPE_NONE             0x0000
#define DMC_VIRT_TYPE_BOOM_SWING_TRACK 0x0001
#define DMC_VIRT_TYPE_SWING_PAN        0x0002
#define DMC_VIRT_TYPE_Y_SWING_TRACK    0x0004
#define DMC_VIRT_TYPE_X_Y_Z            0x0008

// Jog on line axis numberings
#define DMC_VIRT_JOG_ON_LINE_AXIS_X    0
#define DMC_VIRT_JOG_ON_LINE_AXIS_Y    1
#define DMC_VIRT_JOG_ON_LINE_AXIS_Z    2
#define DMC_VIRT_JOG_ON_LINE_AXIS_PAN  3
#define DMC_VIRT_JOG_ON_LINE_AXIS_TILT 4

// the number of entries in the boom compensation table
#define DMC_VIRT_CONFIG_BOOM_COMPENSATION_ANGLES 121 // -60, -59, ... 0, ... 59, 60

#define DMC_VIRT_TRACK 1
#define DMC_VIRT_EW    2
#define DMC_VIRT_NS    3
#define DMC_VIRT_PAN   4
#define DMC_VIRT_TILT  5
#define DMC_VIRT_ROLL  6



// allocate internally used resources
void dmc_msg_init();
// free internally used resources
void dmc_msg_destroy();

/*
 * process an input character
 * returns 0 if no message, or a message type
 * pointers to validChecksum, msgId and msgLength must be provided
 */
uint16_t dmc_msg_process_character(uint8_t data, uint8_t *validChecksum, uint32_t *msgId, uint16_t *msgLength);

// read data out of message if it is valid
uint8_t dmc_msg_read_byte();
uint16_t dmc_msg_read_word();
uint32_t dmc_msg_read_dword();
uint8_t dmc_msg_read_at_end();
int32_t dmc_msg_read_left();

// create output message buffer
void dmc_msg_prepare(uint16_t type, uint32_t id);

// write data into output message buffer
void dmc_msg_out_byte(uint8_t data);
void dmc_msg_out_word(uint16_t data);
void dmc_msg_out_dword(uint32_t data);

// get pointer to output message buffer. do not free it!
uint8_t *dmc_msg_finalize_output(uint16_t *length);

#endif /* SRC_DMC_MSG_H_ */
