// Copyright (C) 2026 Cade Weinberg
//
// This file is part of Limnmoco.
//
// Limnmoco is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Limnmoco is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Limnmoco.  If not, see <https://www.gnu.org/licenses/>.

// @note: okay, This file is going to hold all of the typedefs
// and all of the constants required to work with the DMC protocol.
// It is undecided if this also includes the packet parsing and
// queuing behavior that is required for I/O with Dragonframe.
//
// This module will be paired with a sister module in Testing which
// stubs Dragonframe, such that the packet parsing and queuing
// behavior can be tested.

// @note: see DMC-Protocol-2024-02-13.pdf (bundled with Dragonframe install)

#ifndef LIMNMOCO_CM7_MODULE_DMC_PROTOCOL_H
#define LIMNMOCO_CM7_MODULE_DMC_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "config/config.h"

// DMC message type field options
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
#define DMC_MSG_RT_UPLOAD_MOVE_END 0x0103
#define DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS 0x0104
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

// @note: not in the manual.
#define DMC_MSG_FAN_CONTROL 0x0300

// DMC message type response codes
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

// DMC device capabilities
#define DMC_CAP_REAL_TIME 0x0001
#define DMC_CAP_GO_MOTION 0x0002
#define DMC_CAP_VIRTUAL_BOOM_SWING_TRACK 0x0004
#define DMC_CAP_VIRTUAL_SWING_PAN 0x0008
#define DMC_CAP_VIRTUAL_Y_SWING_TRACK 0x0010
#define DMC_CAP_VIRTUAL_X_Y_Z 0x0020
#define DMC_CAP_OBJECT_TRACKING 0x0040
#define DMC_CAP_GO_MOTION2 0x0080
#define DMC_CAP_COUPLE_MOTORS 0x0100
#define DMC_CAP_REAL_TIME_LOOP 0x0200
#define DMC_CAP_REAL_TIME_CAMERA 0x0400

// DMC motor configuration flags
#define DMC_MOTOR_CONFIG_ENABLED 0x01
#define DMC_MOTOR_CONFIG_BLUR 0x02
// this axis is used IN virtuals
#define DMC_MOTOR_CONFIG_VIRT 0x04
#define DMC_MOTOR_CONFIG_LIVE_CONTROL 0x08
#define DMC_MOTOR_CONFIG_COUPLE 0x10
#define DMC_MOTOR_CONFIG_COUPLE_R 0x20

// DMC GIO Camera Flags
#define DMC_GIO_CAM_SHUTTER_FLAG 0x0001
#define DMC_GIO_CAM_METER_FLAG 0x0002

// DMC real-time playback flags
#define DMC_RT_PLAYBACK_PING_PONG 0x0001
#define DMC_RT_PLAYBACK_LOOP 0x0002
#define DMC_RT_CAMERA_VIDEO 0x0010
#define DMC_RT_CAMERA_STILLS 0x0020

// DMC virtuals support flags
#define DMC_VIRT_TYPE_NONE 0x00
#define DMC_VIRT_TYPE_BOOM_SWING_TRACK 0x01
#define DMC_VIRT_TYPE_SWING_PAN 0x02
#define DMC_VIRT_TYPE_Y_SWING_TRACK 0x04
#define DMC_VIRT_TYPE_X_Y_Z 0x08

// DMC Virtual Motor Numbers
#define DMC_VIRT_TRACK 0x01
#define DMC_VIRT_EW 0x02
#define DMC_VIRT_NS 0x03
#define DMC_VIRT_PAN 0x04
#define DMC_VIRT_TILT 0x05
#define DMC_VIRT_ROLL 0x06

// The boom compensation table is physically an array of stepper
// motor step positions corresponding to the angles from
// -60, -59, ..., 0, ..., 59, 60 degrees. See the manual for more
// details:
// Using Dragonframe 2025
// (https://www.dragonframe.com/download/Using%20Dragonframe%202025.pdf)
//  chapter 16 section "Virtual Axis Configuration with DMC"
#define DMC_NUM_BOOM_COMPENSATION_ANGLES 121

#define DMC_MSG_FLAG_ACK 0x8000

// jog
#define DMC_MSG_MOTOR_JOG_MIN_VELOCITY 1
#define DMC_MSG_MOTOR_JOG_MAX_VELOCITY 10000

// hard stop reasons
#define DMC_MSG_HARD_STOP_REASON_GENERAL 0
#define DMC_MSG_HARD_STOP_REASON_UPPER 1
#define DMC_MSG_HARD_STOP_REASON_LOWER 2
#define DMC_MSG_HARD_STOP_REASON_EXCEPTION 100

// rt run-move
#define DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG 0x01
#define DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP 0x02
#define DMC_MSG_RT_SCALE_RUN_MOVE_FPS 1000
#define DMC_MSG_RT_SCALE_SHOOT_FRAME_BLUR_PERCENT 10
#define DMC_MSG_RT_SCALE_JOG_ALL_FPS 1000

// virtuals
#define DMC_MSG_VIRT_CONFIG_SCALE_POSITION 100000
#define DMC_MSG_VIRT_CONFIG_SCALE_LENGTH 1000
#define DMC_MSG_VIRT_JOG_SPEED_INCHING 1
#define DMC_MSG_VIRT_JOG_SPEED_MAX 10000
#define DMC_MSG_VIRT_JOG_ON_LINE_X 0
#define DMC_MSG_VIRT_JOG_ON_LINE_Y 1
// camera line
#define DMC_MSG_VIRT_JOG_ON_LINE_Z 2
#define DMC_MSG_VIRT_JOG_ON_LINE_PAN 3
#define DMC_MSG_VIRT_JOG_ON_LINE_TILT 4

#define DMC_MSG_MAX_LENGTH 1048

#define LIMNMOCO_CAPABILITIES                                                  \
  (DMC_CAP_COUPLE_MOTORS | DMC_CAP_VIRTUAL_BOOM_SWING_TRACK |                  \
   DMC_CAP_VIRTUAL_SWING_PAN)

/**
 * Every message has a Header. Additionally, some messages contain a
 * variable-length Data section.
 *
 * All multi-byte values are sent in Little-Endian order.
 * A WORD is a two-byte value.
 * A DWORD is a four-byte value.
 */
typedef struct _DmcHeader {
  uint8_t marker[2]; // 'D' 'F'
  uint32_t id;
  uint16_t type;
  uint16_t length;
} DmcHeader;

/**
 * The device must respond to every message it receives. Some requests have
 specific responses
 * that they expect. Some just expect a response with a MSG_FLAG_ACK set and a
 response
 * code.
 *
 * The presence of this flag means that the Data section will contain a response
 code, rather than
 * any other expected data.
 *
 * The response should use the message Type and ID from the received message.
 *    Section    | Size |    Description
 * --------------|------|--------------------------------------
 * Response Code | WORD | OK (0x0010) or appropriate Error Code
 *
 *      Response Code    |   Value   |   Description
 * ----------------------|-----------|----------------------------------
 *  OK                   |  0x0010   | Received, understood, and acted on
 message
 *  ERR_CHECKSUM         |  0x0011   | Checksum didn’t match
 *  ERR_MOVING           |  0x0012   | Can’t handle this command while the rig
 is moving
 *  ERR_UNSUPPORTED      |  0x0013   | The message type is not known or
 supported
 *  ERR_RANGE            |  0x0014   | A parameter was out of range
 *  ERR_GENERAL          |  0x0015   | General Error
 *  ERR_NOT_IN_POSITION  |  0x0016   | Rig is not in position to perform
 requested operation.
 *  ERR_PREROLL          |  0x0017   | A real-time move preroll failed, usually
 because preroll would send motors past limits.
 *  ERR_POSTROLL         |  0x0018   | A real-time move postroll failed, usually
 because a limit check failed.
 *  ERR_SOFT_UP          |  0x0020   | Software upper limit hit
 *  ERR_SOFT_LOW         |  0x0021   | Software lower limit hit
 *  ERR_HARD_UP          |  0x0022   | Hardware upper limit hit
 *  ERR_HARD_LOW         |  0x0023   | Hardware lower limit hit

 */
typedef struct _DmcAck {
  DmcHeader header;
  uint16_t response_code;
} DmcAck;

/**
 * Request basic information about the device. Dragonframe always starts with
 * this request. Also, the device should issue a MSG_HI whenever it starts up.
 * This is helpful to Dragonframe to determine if the device had a random reset.
 * (Some devices can maintain their USB connection while resetting the
 * controller.)
 */
typedef struct _DmcHiRequest {
  DmcHeader header;
} DmcHiRequest;

/**
 * We want to construct a DmcHiResponse with the correct information for
 * Dragonframe. This information can live inside the *.c file which defines the
 * construction routine, or we can place the information inside a config file
 * which is set up by the build system. In either case we must be careful to
 * ensure that the reported capabilities match what the system is capable of.
 *        Section        |   Size   |   Description
 * ----------------------|----------|---------------------------------------------------------
 *   NAME32              |   BYTES  | Device name, up to 32 bytes, UTF-8
 * encoded. Fill with \0 characters if less than 32 bytes FW MAJOR            |
 * BYTE   | FW major number FW MINOR            |   BYTE   | FW minor number FW
 * REV              |   BYTE   | FW revision number MOTOR COUNT         |   BYTE
 * | 0-32: Number of motors the device supports DMX COUNT           |   WORD   |
 * 0-512: Number of DMX channels the device supports GIO OUT COUNT       | BYTE
 * | 0-32: Number of general IO output triggers (relay, logic out) GIO INPUT
 * COUNT     |   BYTE   | 0-32: Number of input triggers HW LIMIT COUNT      |
 * BYTE   | 0-32: How many hardware limit set inputs the device has UPLOAD FRAME
 * COUNT  |   DWORD  | How many frames of position/dmx data can the device hold
 *   CAPABILITIES        |   DWORD  | Other capabilities the device can
 * advertise PROTOCOL VERSION    |   WORD   | the DMC protocol is version 2
 *
 * Each bit in the capabilities parameter represents functionality the device
 * supports.
 *
 *    Value     |   Capability
 * -------------|----------------------------------------
 *    0x0001    | Real-time moves (upload move / jog all / play)
 *    0x0002    | Go-motion shoot frame
 *    0x0004    | Virtual mode Boom/Swing/Track
 *    0x0008    | Virtual mode Swing/Pan
 *    0x0010    | Virtual mode Y/Swing/Track
 *    0x0020    | Virtual mode X/Y/Z
 *    0x0040    | Aim point
 *    0x0080    | Go-motion shoot frame version #2
 *    0x0100    | Couple motors
 *    0x0200    | Real-time looping or ping-pong playback
 */
typedef struct _DmcHiResponse {
  DmcHeader header;
  uint8_t name[32];
  uint8_t fw_major;
  uint8_t fw_minor;
  uint8_t fw_rev;
  uint8_t motor_count;
  uint16_t dmx_count;
  uint8_t gio_out_count;
  uint8_t gio_in_count;
  uint8_t hw_limit_count;
  uint32_t upload_frame_count;
  uint32_t capabilities;
  uint16_t protocol_version;
} DmcHiResponse;

/**
 * Set DMX light values. The request specifies the initial channel to change,
 * and then one or more light values.
 *
 * If RAMP is set to 1, the device should ramp the light value of each channel
 * up or down to reach the target channel, rather than switching to it
 * immediately.
 */
typedef struct _DmcDmxRequest {
  DmcHeader header;
  uint8_t ramp;
  uint16_t start_channel;
  uint8_t light_values[];
} DmcDmxRequest;

/**
 * Set the state for general I/O output triggers.
 */
typedef struct _DmcGioOutRequest {
  DmcHeader header;
  uint32_t triggers;
} DmcGioOutRequest;

/**
 * Request the state for general I/O input triggers. Dragonframe may request the
 * input state with this message. The device should also send this message,
 * unsolicited, any time the inputs change. (Of course, the device should use
 * some hysteresis to ensure the inputs don’t change frantically.)
 */
typedef struct _DmcGioInRequest {
  DmcHeader header;
} DmcGioInRequest;

/**
 * @brief Reports the current state of the general I/O input triggers; devices
 * also send this message when inputs change.
 */
typedef struct _DmcGioInResponse {
  DmcHeader header;
  uint32_t triggers;
} DmcGioInResponse;

/**
 * Test the camera outputs.
 */
typedef struct _DmcGioCamRequest {
  DmcHeader header;
  uint32_t triggers;
} DmcGioCamRequest;

/**
 * Request the motor (and dmx) status. This is only a status of whether
 * something is moving/adjusting.
 */
typedef struct _DmcMotorStatusRequest {
  DmcHeader header;
} DmcMotorStatusRequest;

/**
 * @brief Reports whether each motor and the DMX output are currently moving or
 * adjusting.
 */
typedef struct _DmcMotorStatusResponse {
  DmcHeader header;
  uint32_t motor_status;
  uint8_t dmx_status;
} DmcMotorStatusResponse;

/**
 * Instruct the device to move a motor to a new position.
 */
typedef struct _DmcMotorMoveRequest {
  DmcHeader header;
  uint8_t motor;
  int32_t position;
} DmcMotorMoveRequest;

/**
 * @brief Reports the motor status resulting from a motor move request.
 */
typedef struct _DmcMotorMoveResponse {
  DmcHeader header;
  uint8_t motor_status;
} DmcMotorMoveResponse;

/**
 * Instruct the device to stop a motor.
 */
typedef struct _DmcMotorStopRequest {
  DmcHeader header;
  uint8_t motor;
} DmcMotorStopRequest;

/**
 * Instruct the device to stop all motors. The device should decelerate. If the
 * device gets a second STOP_ALL soon after a first, it should HARD STOP.
 * Meaning it should decelerate faster than normal. (If a controller may move
 * large/heavy equipment, it should always decelerate rather than simply stop
 * sending signals.)
 */
typedef struct _DmcMotorStopAllRequest {
  DmcHeader header;
  uint32_t flags;
} DmcMotorStopAllRequest;

/**
 * Request the motor positions and move ‘time’.
 * The device should send these out, about every 0.10 seconds, if any of the
 * motors are moving, to keep Dragonframe informed of the positions.
 */
typedef struct _DmcMotorGetPositionRequest {
  DmcHeader header;
} DmcMotorGetPositionRequest;

/**
 * @brief Reports elapsed move time and the current position of each motor.
 */
typedef struct _DmcMotorGetPositionResponse {
  DmcHeader header;
  uint32_t move_time;
  int32_t motor_positions[LIMNMOCO_MOTOR_COUNT];
} DmcMotorGetPositionResponse;

/**
 * Instruct the device to reset a motor position to a new value. This does not
 * move the motor. After changing the internal position and responding to this
 * message, the device should send a MSG_MOTOR_GET_POSITION message to confirm
 * the new position.
 */
typedef struct _DmcMotorResetPositionRequest {
  DmcHeader header;
  uint8_t motor;
  int32_t position;
} DmcMotorResetPositionRequest;

/**
 * Instruct the device to jog/inch a motor.
 * A speed of 1 means the device should move very slowly, for precise
 * adjustments. A speed of 10,000 means the device should jog using the max
 * velocity and acceleration. A value in between 1 and 10,000 should provide an
 * appropriate speed between 1% and 100% of the jog speed.
 */
typedef struct _DmcMotorJogRequest {
  DmcHeader header;
  uint8_t motor;
  uint16_t speed;
  int32_t destination;
} DmcMotorJogRequest;

/**
 * Configure the motor.
 */
typedef struct _DmcMotorConfigureRequest {
  DmcHeader header;
  uint8_t motor;
  uint8_t flags;
} DmcMotorConfigureRequest;

/**
 * Set the motor speed and acceleration.
 */
typedef struct _DmcMotorSetSpeedRequest {
  DmcHeader header;
  uint8_t motor;
  uint32_t max_velocity;
  uint32_t max_acceleration;
} DmcMotorSetSpeedRequest;

/**
 * Set the motor software and hardware limits.
 */
typedef struct _DmcMotorSetLimitsRequest {
  DmcHeader header;
  uint8_t motor;
  uint8_t lower_enable;
  uint32_t lower_limit;
  uint8_t upper_enable;
  uint32_t upper_limit;
  uint8_t hw_set; // this one doesn't make sense, swap high/low????
} DmcMotorSetLimitsRequest;

/**
 * The device can send this when a limit has been hit, or if the device’s
 * “emergency stop” button has been pressed.
 */
typedef struct _DmcMotorHardStopRequest {
  DmcHeader header;
  uint16_t response_code;
} DmcMotorHardStopRequest;

/**
 * @brief Acknowledgement of a hard-stop notification.
 */
typedef struct _DmcMotorHardStopResponse {
  DmcHeader header;
  uint8_t reason;
  uint8_t motor;
} DmcMotorHardStopResponse;

/**
 * Initial message for uploading move data. Any previously loaded move will be
 * cleared.
 */
typedef struct _DmcRtUploadMoveBeginRequest {
  DmcHeader header;
  uint32_t start_frame;
  uint32_t end_frame;
} DmcRtUploadMoveBeginRequest;

/**
 * Send a section of axis frame data.
 */
typedef struct _DmcRtUploadMoveAxisRequest {
  DmcHeader header;
  uint8_t motor;
  uint32_t start_index;
  uint32_t positions[];
} DmcRtUploadMoveAxisRequest;

/**
 * Send a section of DMX frame data.
 *
 * @note for later, the documentation for this structure is a bit long,
 * and it won't fit into a simple markdown table. Look into HTML tables
 * for converting over the documentation from the DMC-Protocol manual.
 */
typedef struct _DmcRtUploadMoveDmxRequest {
  DmcHeader header;
  uint16_t channel;
  uint32_t start_index;
  uint8_t light_levels[];
} DmcRtUploadMoveRequest;

/**
 * @brief A frame number and its trigger bit values in a real-time trigger
 * upload.
 */
typedef struct _MoveFrameValue {
  uint32_t frame;
  uint32_t values;
} MoveFrameValue;

/**
 * Introduced in Protocol: 2
 * Send a section of trigger data.
 * Only sent if the DMX program has triggers.
 * Only sends frame/value pairs where one or more triggers is on.
 */
typedef struct _DmcRtUploadMoveTriggersRequest {
  DmcHeader header;
  uint32_t mask;
  MoveFrameValue move_frame_values[];
} DmcRtUploadMoveTriggersRequest;

/**
 * Final message for uploading move data.
 */
typedef struct _DmcRtUploadMoveEndRequest {
  DmcHeader header;
} DmcRtUploadMoveEndRequest;

/**
 * Instructs the device to send all motors (and DMX if applicable) to the
 * uploaded frame position.
 */
typedef struct _DmcRtPositionFrameRequest {
  DmcHeader header;
  uint32_t frame;
} DmcRtPositionFrameRequest;

/**
 * Prepares the device to run a section of the move live.
 * The rig must move into a pre-roll position that it calculates,
 * so that it can accelerate into full speed after “pre-roll time”.
 * Once the rig moves into pre-roll position, it must wait for a MSG_RT_GO to
 * begin the move.
 */
typedef struct _DmcRtRunMoveRequest {
  DmcHeader header;
  uint32_t fps;
  uint32_t start_frame;
  uint32_t end_frame;
  uint32_t pre_roll_time;
  uint32_t post_roll_time;
  uint8_t sync_dmx;
  uint32_t bloop_location;
  uint16_t bloop_dmx_channel;
  uint16_t bloop_time;
  uint16_t flags;
} DmcRtRunMoveRequest;

/**
 * @brief Motor number and the two positions used to define its blur movement
 * during a shot.
 */
typedef struct _ShootFrameMotorBlur {
  uint8_t motor;
  uint32_t position_A;
  uint32_t position_B;
} ShootFrameMotorBlur;

/**
 * Set up a blur-motion frame capture
 */
typedef struct _DmcRtShootFrameRequest {
  DmcHeader header;
  uint32_t frame;
  uint8_t direction;
  uint32_t exposure_time;
  uint16_t blur_percent;
  ShootFrameMotorBlur motor_blur[LIMNMOCO_MOTOR_COUNT];
} DmcRtShootFrameRequest;

/**
 * Set up a blur-motion frame capture (alternate version)
 * The sets of motor numbers and positions are optional.
 * Each motor provided will be part of the blur.
 */
typedef struct _DmcRtShootFrame2Request {
  DmcHeader header;
  uint32_t frame;
  uint32_t exposure_time;
  uint16_t open_angle;
  uint16_t close_angle;
  ShootFrameMotorBlur motor_blur[LIMNMOCO_MOTOR_COUNT];
} DmcRtShootFrame2Request;

/**
 * After a MSG_RT_RUN_MOVE or MSG_RT_SHOOT_FRAME the device may move the motors
 * into position. When the device has stopped moving, a MSG_RT_GO command will
 * perform the run or shoot of the frame.
 *
 * If the device is running a live move (configured via MSG_RT_RUN_MOVE), it
 * must send a MSG_MOTOR_GET_POSITION each time it reaches a new frame in the
 * move. (If it is slightly after, that is ok.)
 */
typedef struct _DmcRtGoRequest {
  DmcHeader header;
} DmcRtGoRequest;

/**
 * After a MSG_RT_RUN_MOVE or MSG_RT_SHOOT_FRAME has finished shooting, the
 * device sends MSG_RT_END to notify Dragonframe that the process is over.
 */
typedef struct _DmcRtEndRequest {
  DmcHeader header;
} DmcRtEndRequest;

/**
 * Jog all motors towards the destination frame.
 * The rig must already be on a frame position that is uploaded to the device.
 */
typedef struct _DmcRtJogAllRequest {
  DmcHeader header;
  uint32_t fps;
  uint32_t destination;
} DmcRtJogAllRequest;

/**
 * If the device supports 'real-time looping' capability, it must support this
 * command. The currently running loop/ping-pong must stop after it reaches the
 * next end point.
 */
typedef struct _DmcRtStopLoopRequest {
  DmcHeader header;
} DmcRtStopLoopRequest;

/**
 * Configure virtuals.
 *
 *   Section  |   Size   |  Description
 *------------|----------|------------------------------
 *    TYPE    |   BYTE   | 0=none, 1=boom-swing-track, 2=swing-pan,
 * 3=y-swing-track, 4=x-y-z DATA    |  depends | Depends on type
 */
typedef struct _DmcVirtConfigRequest {
  DmcHeader header;
  uint8_t type;
  uint8_t data[];
} DmcVirtConfigRequest;

/**
 * @brief Configures virtual boom, swing, track, pan, tilt, and roll axes,
 * including boom geometry and compensation data.
 *
 * Section     |      Size     |     Description
 *-------------|---------------|-----------------
 * BOOM motor  |      DWORD    |     BOOM motor (up/down)
 * BOOM spu    |      DWORD    |     BOOM steps per unit
 * BOOM pos    |      DWORD    |     BOOM position (in units) * 100000
 * SWING motor |      DWORD    |     SWING motor (up/down)
 * SWING spu   |      DWORD    |     SWING steps per unit
 * SWING pos   |      DWORD    |     SWING position (in units) * 100000
 * TRACK motor |      DWORD    |     TRACK motor (up/down)
 * TRACK spu   |      DWORD    |     TRACK steps per unit
 * TRACK pos   |      DWORD    |     TRACK position (in units) * 100000
 * PAN motor   |      DWORD    |     PAN motor (up/down)
 * PAN spu     |      DWORD    |     PAN steps per unit
 * PAN pos     |      DWORD    |     PAN position (in units) * 100000
 * TILT motor  |      DWORD    |     TILT motor (up/down)
 * TILT spu    |      DWORD    |     TILT steps per unit
 * TILT pos    |      DWORD    |     TILT position (in units) * 100000
 * ROLL motor  |      DWORD    |     ROLL motor (up/down)
 * ROLL spu    |      DWORD    |     ROLL steps per unit
 * ROLL pos    |      DWORD    |     ROLL position (in units) * 100000
 * BOOM LENGTH |      DWORD    |     Boom length * 1000
 * BOOM EXT    |      DWORD    |     Boom extension length * 1000
 * NODAL OFF X |      DWORD    |     Nodal offset for x * 1000
 * NODAL OFF Y |      DWORD    |     Nodal offset for y * 1000
 * NODAL OFF Z |      DWORD    |     Nodal offset for z * 1000
 * Boom Compensation Table | DWORD * 121 | Optional Boom Compensation Data.
 * Position of Boom arm at -60, -59, … 0, 1, 2, … 59, 60 degrees SAFE DISTANCE |
 *      DWORD    |     Safe distance * 1000 (optional)
 */
typedef struct _DmcVirtConfigRequestBoomSwingTrack {
  DmcHeader header;
  uint8_t type;
  uint32_t boom_motor;
  uint32_t boom_spu;
  uint32_t boom_position;
  uint32_t swing_motor;
  uint32_t swing_spu;
  uint32_t swing_position;
  uint32_t track_motor;
  uint32_t track_spu;
  uint32_t track_position;
  uint32_t pan_motor;
  uint32_t pan_spu;
  uint32_t pan_position;
  uint32_t tilt_motor;
  uint32_t tilt_spu;
  uint32_t tilt_position;
  uint32_t roll_motor;
  uint32_t roll_spu;
  uint32_t roll_position;
  uint32_t boom_length;
  uint32_t boom_extension;
  uint32_t nodal_offset_x;
  uint32_t nodal_offset_y;
  uint32_t nodal_offset_z;
  uint32_t boom_compensation[DMC_NUM_BOOM_COMPENSATION_ANGLES];
  uint32_t safe_distance;
} DmcVirtConfigRequestBoomSwingTrack;

/**
 * @brief Configures the virtual swing and pan axes.
 *
 * Section     | Size  | Description
 * ---         | ---   | ---
 * SWING motor | DWORD | SWING motor (up/down)
 * SWING spu   | DWORD | SWING steps per unit
 * PAN motor   | DWORD | PAN motor (up/down)
 * PAN spu     | DWORD | PAN steps per unit
 */
typedef struct _DmcVirtConfigRequestSwingPan {
  DmcHeader header;
  uint8_t type;
  uint32_t swing_motor;
  uint32_t swing_spu;
  uint32_t pan_motor;
  uint32_t pan_spu;
} DmcVirtConfigRequestSwingPan;

/**
 * Send the virtual motor to a new position.
 */
typedef struct _DmcVirtMoveRequest {
  DmcHeader header;
  uint8_t motor;
  float position;
} DmcVirtMoveRequest;

/**
 * Stop the virtual motor.
 */
typedef struct _DmcVirtStopRequest {
  DmcHeader header;
  uint8_t motor;
} DmcVirtStopRequest;

/**
 * Instruct the device to jog/inch a virtual motor.
 * A speed of 1 means the device should move very slowly, for precise
 * adjustments. A speed of 10,000 means the device should jog using the max
 * velocity and acceleration. For now, those are the only speeds sent. In the
 * future, we may send intermediate values to signify a target % of the max
 * velocity.
 */
typedef struct _DmcVirtJogRequest {
  DmcHeader header;
  uint8_t motor;
  uint16_t speed;
  int32_t destination; // step position
} DmcVirtJogRequest;

/**
 * Instruct the device to jog along the direction of the camera.
 * Speed is -10000->10000, with 1 being the slowest and 10000 being the fastest.
 */
typedef struct _DmcVirtJogOnLineRequest {
  DmcHeader header;
  uint8_t axis;
  uint16_t speed;
} DmcVirtJogOnLineRequest;

/**
 * Request the virtual motor positions.
 */
typedef struct _DmcVirtGetPositionRequest {
  DmcHeader header;
} DmcVirtGetPositionRequest;

/**
 * @brief Reports the current virtual motor positions and aim point coordinates.
 */
typedef struct _DmcVirtGetPositionResponse {
  DmcHeader header;
  uint32_t track;
  uint32_t EW;
  uint32_t NS;
  uint32_t pan;
  uint32_t tilt;
  uint32_t roll;
  uint8_t aim_point;
  uint32_t aim_x;
  uint32_t aim_y;
  uint32_t aim_z;
} DmcVirtGetPositionResponse;

/**
 * Introduced in Protocol: 2
 * Configure aim point.
 */
typedef struct _DmcVirtAimPointRequest {
  DmcHeader header;
  uint8_t enable;
  uint32_t aim_x;
  uint32_t aim_y;
  uint32_t aim_z;
} DmcVirtAimPointRequest;

/**
 * @brief Acknowledges or reports the configured aim point enable state and
 * coordinates.
 */
typedef struct _DmcVirtAimPointResponse {
  DmcHeader header;
  uint8_t enable;
  uint32_t aim_x;
  uint32_t aim_y;
  uint32_t aim_z;
} DmcVirtAimPointResponse;

// #NOTE: The goal with the protocol is to be able to serialize and deserialize
// messages. we want to do this given a buffer of bytes, such that we are
// decoupled from how we receive the buffer of bytes and how we transmit it.
// This will allow for host-side testing of the serialization and
// deserialization logic. The strategy was to use packed structures, as this is
// how the data is transmitted over the wire. However, this approach introduces
// the risk of undefined behavior when serializing or deserializing messages
// directly from the packed structures. It also introduces misaligned access to
// struct members throughout the codebase. The alternative approach is to use
// explicit serialization and deserialization functions for each message type.
// Noting that requests are always incoming, and so are exclusively
// deserialized, while responses are always outgoing, and so are exclusively
// serialized.
//
// A serialization routine is probably going to have a signature like:
// bool dmc_msg_x_serialize(const DmcX* msg, uint8_t* buffer, uint32_t
// buffer_size);
//
// A deserialization routine is probably going to have a signature like:
// bool dmc_msg_x_deserialize(DmcX* msg, const uint8_t* buffer, uint32_t
// buffer_size);
//
// a more explicit return type signaling the kind of error may be warranted as
// well. where X is replaced with the actual type of message.
//
// The transport layer can then have a function such as
// bool dmc_msg_x_send(const DmcX* msg);
//
// which enqueues the message for transmission by serializing it into the
// transport layer's transmit queue.
//
// The transport layer can then have a function such as
// bool dmc_msg_x_receive(DmcX* msg);
//
// which dequeues a message from the transport layer's receive queue and
// deserializes it into the provided message structure.
// This function can be called when a message is actually received, and so we
// need some form of signalling mechanism between the transport layer and the
// main event loop.
//
// So this protocol file can define the message structures and provide
// some basic initialization functions for each message type. plus the
// serialization and deserialization functions for each message type.
//
// a variable length struct acting as a record of the specific message has a few
// points to consider when interacting with serialization and deserialization.
// when serializing, the size of the given record is held within the structure
// itself, so the caller must ensure that the structure is allocated with enough
// space to hold all of the variable data. Then, the buffer size check makes
// sense in the serialize routine, since the required size is known. when
// deserializing, the required buffer size is not known until the header has
// been deserialized, so the caller of the function cannot call the function
// safely until the header has been deserialized. This means that we need to
// handle deserializing variable length messages in two steps: first deserialize
// the header, then allocate the message structure with enough space for the
// variable data before deserializing the remainder.
//
// given that we want to avoid dynamic memory allocations if at all possbile.
// we may wish to allocate a buffer large enough to hold the largest possible
// message. or a queue of these buffers, and have the system pass around a
// pointer to the active message when handling each message. How can that work
// in C? if we allocate an array of uint8_t, we can cast a pointer to the
// beginning of the array to the message type, Then we know that structure has
// enough space for its flexible array member. even when statically allocated.

/**
 * @brief Convert between DMC wire messages and their corresponding structures.
 *
 * @param buffer_size Number of bytes available in the input or output wire
 * buffer. The checksum is handled by the transport layer and is not included in
 * these codecs.
 * @note For requests with flexible array members, allocate at least
 * sizeof(the request structure) plus the trailing payload length for @p msg.
 */

uint8_t *dmc_msg_header_serialize(const DmcHeader *msg, uint8_t *buffer,
                                  uint32_t buffer_size);
const uint8_t *dmc_msg_header_deserialize(DmcHeader *msg, const uint8_t *buffer,
                                          uint32_t buffer_size);

uint8_t *dmc_msg_ack_serialize(const DmcAck *msg, uint8_t *buffer,
                               uint32_t buffer_size);
const uint8_t *dmc_msg_ack_deserialize(DmcAck *msg, const uint8_t *buffer,
                                       uint32_t buffer_size);

uint8_t *dmc_msg_hi_request_serialize(const DmcHiRequest *msg, uint8_t *buffer,
                                      uint32_t buffer_size);
const uint8_t *dmc_msg_hi_request_deserialize(DmcHiRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size);

uint8_t *dmc_msg_hi_response_serialize(const DmcHiResponse *msg,
                                       uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_hi_response_deserialize(DmcHiResponse *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size);

uint8_t *dmc_msg_dmx_request_serialize(const DmcDmxRequest *msg,
                                       uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_dmx_request_deserialize(DmcDmxRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size);

uint8_t *dmc_msg_gio_out_request_serialize(const DmcGioOutRequest *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size);
const uint8_t *dmc_msg_gio_out_request_deserialize(DmcGioOutRequest *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size);

uint8_t *dmc_msg_gio_in_request_serialize(const DmcGioInRequest *msg,
                                          uint8_t *buffer,
                                          uint32_t buffer_size);
const uint8_t *dmc_msg_gio_in_request_deserialize(DmcGioInRequest *msg,
                                                  const uint8_t *buffer,
                                                  uint32_t buffer_size);

uint8_t *dmc_msg_gio_in_response_serialize(const DmcGioInResponse *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size);
const uint8_t *dmc_msg_gio_in_response_deserialize(DmcGioInResponse *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size);

uint8_t *dmc_msg_gio_cam_request_serialize(const DmcGioCamRequest *msg,
                                           uint8_t *buffer,
                                           uint32_t buffer_size);
const uint8_t *dmc_msg_gio_cam_request_deserialize(DmcGioCamRequest *msg,
                                                   const uint8_t *buffer,
                                                   uint32_t buffer_size);

uint8_t *
dmc_msg_motor_status_request_serialize(const DmcMotorStatusRequest *msg,
                                       uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_status_request_deserialize(
    DmcMotorStatusRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *
dmc_msg_motor_status_response_serialize(const DmcMotorStatusResponse *msg,
                                        uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_status_response_deserialize(
    DmcMotorStatusResponse *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_move_request_serialize(const DmcMotorMoveRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size);
const uint8_t *dmc_msg_motor_move_request_deserialize(DmcMotorMoveRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size);

uint8_t *dmc_msg_motor_move_response_serialize(const DmcMotorMoveResponse *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size);
const uint8_t *dmc_msg_motor_move_response_deserialize(
    DmcMotorMoveResponse *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_stop_request_serialize(const DmcMotorStopRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size);
const uint8_t *dmc_msg_motor_stop_request_deserialize(DmcMotorStopRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size);

uint8_t *
dmc_msg_motor_stop_all_request_serialize(const DmcMotorStopAllRequest *msg,
                                         uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_stop_all_request_deserialize(
    DmcMotorStopAllRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_get_position_request_serialize(
    const DmcMotorGetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_motor_get_position_request_deserialize(DmcMotorGetPositionRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size);

uint8_t *dmc_msg_motor_get_position_response_serialize(
    const DmcMotorGetPositionResponse *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_motor_get_position_response_deserialize(
    DmcMotorGetPositionResponse *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_motor_reset_position_request_serialize(
    const DmcMotorResetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_motor_reset_position_request_deserialize(
    DmcMotorResetPositionRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_motor_jog_request_serialize(const DmcMotorJogRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size);
const uint8_t *dmc_msg_motor_jog_request_deserialize(DmcMotorJogRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size);

uint8_t *dmc_msg_motor_configure_request_serialize(
    const DmcMotorConfigureRequest *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_configure_request_deserialize(
    DmcMotorConfigureRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_set_speed_request_serialize(
    const DmcMotorSetSpeedRequest *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_set_speed_request_deserialize(
    DmcMotorSetSpeedRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_set_limits_request_serialize(
    const DmcMotorSetLimitsRequest *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_set_limits_request_deserialize(
    DmcMotorSetLimitsRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_hard_stop_request_serialize(
    const DmcMotorHardStopRequest *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_hard_stop_request_deserialize(
    DmcMotorHardStopRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_motor_hard_stop_response_serialize(
    const DmcMotorHardStopResponse *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_motor_hard_stop_response_deserialize(
    DmcMotorHardStopResponse *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_rt_upload_move_begin_request_serialize(
    const DmcRtUploadMoveBeginRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_rt_upload_move_begin_request_deserialize(
    DmcRtUploadMoveBeginRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_rt_upload_move_axis_request_serialize(
    const DmcRtUploadMoveAxisRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_rt_upload_move_axis_request_deserialize(DmcRtUploadMoveAxisRequest *msg,
                                                const uint8_t *buffer,
                                                uint32_t buffer_size);

uint8_t *
dmc_msg_rt_upload_move_request_serialize(const DmcRtUploadMoveRequest *msg,
                                         uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_rt_upload_move_request_deserialize(
    DmcRtUploadMoveRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_rt_upload_move_triggers_request_serialize(
    const DmcRtUploadMoveTriggersRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_rt_upload_move_triggers_request_deserialize(
    DmcRtUploadMoveTriggersRequest *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_rt_upload_move_end_request_serialize(
    const DmcRtUploadMoveEndRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_rt_upload_move_end_request_deserialize(DmcRtUploadMoveEndRequest *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size);

uint8_t *dmc_msg_rt_position_frame_request_serialize(
    const DmcRtPositionFrameRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_rt_position_frame_request_deserialize(DmcRtPositionFrameRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size);

uint8_t *dmc_msg_rt_run_move_request_serialize(const DmcRtRunMoveRequest *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size);
const uint8_t *dmc_msg_rt_run_move_request_deserialize(DmcRtRunMoveRequest *msg,
                                                       const uint8_t *buffer,
                                                       uint32_t buffer_size);

uint8_t *
dmc_msg_rt_shoot_frame_request_serialize(const DmcRtShootFrameRequest *msg,
                                         uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_rt_shoot_frame_request_deserialize(
    DmcRtShootFrameRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_rt_shoot_frame2_request_serialize(
    const DmcRtShootFrame2Request *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_rt_shoot_frame2_request_deserialize(
    DmcRtShootFrame2Request *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_rt_go_request_serialize(const DmcRtGoRequest *msg,
                                         uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_rt_go_request_deserialize(DmcRtGoRequest *msg,
                                                 const uint8_t *buffer,
                                                 uint32_t buffer_size);

uint8_t *dmc_msg_rt_end_request_serialize(const DmcRtEndRequest *msg,
                                          uint8_t *buffer,
                                          uint32_t buffer_size);
const uint8_t *dmc_msg_rt_end_request_deserialize(DmcRtEndRequest *msg,
                                                  const uint8_t *buffer,
                                                  uint32_t buffer_size);

uint8_t *dmc_msg_rt_jog_all_request_serialize(const DmcRtJogAllRequest *msg,
                                              uint8_t *buffer,
                                              uint32_t buffer_size);
const uint8_t *dmc_msg_rt_jog_all_request_deserialize(DmcRtJogAllRequest *msg,
                                                      const uint8_t *buffer,
                                                      uint32_t buffer_size);

uint8_t *dmc_msg_rt_stop_loop_request_serialize(const DmcRtStopLoopRequest *msg,
                                                uint8_t *buffer,
                                                uint32_t buffer_size);
const uint8_t *dmc_msg_rt_stop_loop_request_deserialize(
    DmcRtStopLoopRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_virt_config_request_serialize(const DmcVirtConfigRequest *msg,
                                               uint8_t *buffer,
                                               uint32_t buffer_size);
const uint8_t *dmc_msg_virt_config_request_deserialize(
    DmcVirtConfigRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_virt_config_request_boom_swing_track_serialize(
    const DmcVirtConfigRequestBoomSwingTrack *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_virt_config_request_boom_swing_track_deserialize(
    DmcVirtConfigRequestBoomSwingTrack *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_virt_config_request_swing_pan_serialize(
    const DmcVirtConfigRequestSwingPan *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *dmc_msg_virt_config_request_swing_pan_deserialize(
    DmcVirtConfigRequestSwingPan *msg, const uint8_t *buffer,
    uint32_t buffer_size);

uint8_t *dmc_msg_virt_move_request_serialize(const DmcVirtMoveRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size);
const uint8_t *dmc_msg_virt_move_request_deserialize(DmcVirtMoveRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size);

uint8_t *dmc_msg_virt_stop_request_serialize(const DmcVirtStopRequest *msg,
                                             uint8_t *buffer,
                                             uint32_t buffer_size);
const uint8_t *dmc_msg_virt_stop_request_deserialize(DmcVirtStopRequest *msg,
                                                     const uint8_t *buffer,
                                                     uint32_t buffer_size);

uint8_t *dmc_msg_virt_jog_request_serialize(const DmcVirtJogRequest *msg,
                                            uint8_t *buffer,
                                            uint32_t buffer_size);
const uint8_t *dmc_msg_virt_jog_request_deserialize(DmcVirtJogRequest *msg,
                                                    const uint8_t *buffer,
                                                    uint32_t buffer_size);

uint8_t *dmc_msg_virt_jog_on_line_request_serialize(
    const DmcVirtJogOnLineRequest *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_virt_jog_on_line_request_deserialize(
    DmcVirtJogOnLineRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_virt_get_position_request_serialize(
    const DmcVirtGetPositionRequest *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_virt_get_position_request_deserialize(DmcVirtGetPositionRequest *msg,
                                              const uint8_t *buffer,
                                              uint32_t buffer_size);

uint8_t *dmc_msg_virt_get_position_response_serialize(
    const DmcVirtGetPositionResponse *msg, uint8_t *buffer,
    uint32_t buffer_size);
const uint8_t *
dmc_msg_virt_get_position_response_deserialize(DmcVirtGetPositionResponse *msg,
                                               const uint8_t *buffer,
                                               uint32_t buffer_size);

uint8_t *
dmc_msg_virt_aim_point_request_serialize(const DmcVirtAimPointRequest *msg,
                                         uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_virt_aim_point_request_deserialize(
    DmcVirtAimPointRequest *msg, const uint8_t *buffer, uint32_t buffer_size);

uint8_t *dmc_msg_virt_aim_point_response_serialize(
    const DmcVirtAimPointResponse *msg, uint8_t *buffer, uint32_t buffer_size);
const uint8_t *dmc_msg_virt_aim_point_response_deserialize(
    DmcVirtAimPointResponse *msg, const uint8_t *buffer, uint32_t buffer_size);

// Initializes the compile-time device information in a DMC HI response.
void dmc_msg_header_init(DmcHeader *msg, uint32_t id, uint16_t type,
                         uint16_t length);
void dmc_msg_hi_response_init(DmcHiResponse *msg);
#endif // !LIMNMOCO_CM7_MODULE_DMC_PROTOCOL_H
