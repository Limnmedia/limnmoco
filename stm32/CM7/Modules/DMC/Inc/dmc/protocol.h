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

#include <stdint.h>
#include <stddef.h>

// DMC message type field options
inline constexpr uint16_t DMC_MSG_HI                                = 0x0001;
inline constexpr uint16_t DMC_MSG_DMX                               = 0x0020;
inline constexpr uint16_t DMC_MSG_GIO_OUT                           = 0x0021;
inline constexpr uint16_t DMC_MSG_GIO_IN                            = 0x0022;
inline constexpr uint16_t DMC_MSG_GIO_CAM                           = 0x0023;
inline constexpr uint16_t DMC_MSG_MOTOR_STATUS                      = 0x0030;
inline constexpr uint16_t DMC_MSG_MOTOR_MOVE                        = 0x0031;
inline constexpr uint16_t DMC_MSG_MOTOR_STOP                        = 0x0032;
inline constexpr uint16_t DMC_MSG_MOTOR_STOP_ALL                    = 0x0033;
inline constexpr uint16_t DMC_MSG_MOTOR_GET_POSITION                = 0x0034;
inline constexpr uint16_t DMC_MSG_MOTOR_RESET_POSITION              = 0x0035;
inline constexpr uint16_t DMC_MSG_MOTOR_JOG                         = 0x0036;
inline constexpr uint16_t DMC_MSG_MOTOR_CONFIGURE                   = 0x0037;
inline constexpr uint16_t DMC_MSG_MOTOR_SET_SPEED                   = 0x0038;
inline constexpr uint16_t DMC_MSG_MOTOR_SET_LIMITS                  = 0x0039;
inline constexpr uint16_t DMC_MSG_MOTOR_HARD_STOP                   = 0x003A;
inline constexpr uint16_t DMC_MSG_RT_UPLOAD_MOVE_BEGIN              = 0x0100;
inline constexpr uint16_t DMC_MSG_RT_UPLOAD_MOVE_AXIS               = 0x0101;
inline constexpr uint16_t DMC_MSG_RT_UPLOAD_MOVE_DMX                = 0x0102;
inline constexpr uint16_t DMC_MSG_RT_UPLOAD_MOVE_END                = 0x0103;
inline constexpr uint16_t DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS           = 0x0104;
inline constexpr uint16_t DMC_MSG_RT_POSITION_FRAME                 = 0x0110;
inline constexpr uint16_t DMC_MSG_RT_RUN_MOVE                       = 0x0111;
inline constexpr uint16_t DMC_MSG_RT_SHOOT_FRAME                    = 0x0112;
inline constexpr uint16_t DMC_MSG_RT_SHOOT_FRAME2                   = 0x0115;
inline constexpr uint16_t DMC_MSG_RT_GO                             = 0x0113;
inline constexpr uint16_t DMC_MSG_RT_END                            = 0x0114;
inline constexpr uint16_t DMC_MSG_RT_STOP_LOOP                      = 0x0116;
inline constexpr uint16_t DMC_MSG_RT_JOG_ALL                        = 0x0120;
inline constexpr uint16_t DMC_MSG_VIRT_CONFIG                       = 0x0200;
inline constexpr uint16_t DMC_MSG_VIRT_MOVE                         = 0x0201;
inline constexpr uint16_t DMC_MSG_VIRT_STOP                         = 0x0202;
inline constexpr uint16_t DMC_MSG_VIRT_JOG                          = 0x0203;
inline constexpr uint16_t DMC_MSG_VIRT_GET_POSITION                 = 0x0205;
inline constexpr uint16_t DMC_MSG_VIRT_JOG_ON_LINE                  = 0x0206;
inline constexpr uint16_t DMC_MSG_VIRT_AIM_POINT                    = 0x0207;
inline constexpr uint16_t DMC_MSG_FAN_CONTROL                       = 0x0300; // @note: not in the manual.

// DMC message type response codes
inline constexpr uint16_t DMC_ACK_OK                                = 0x0010;
inline constexpr uint16_t DMC_ACK_ERR_CHECKSUM                      = 0x0011;
inline constexpr uint16_t DMC_ACK_ERR_MOVING                        = 0x0012;
inline constexpr uint16_t DMC_ACK_ERR_UNSUPPORTED                   = 0x0013;
inline constexpr uint16_t DMC_ACK_ERR_RANGE                         = 0x0014;
inline constexpr uint16_t DMC_ACK_ERR_GENERAL                       = 0x0015;
inline constexpr uint16_t DMC_ACK_ERR_NOT_IN_POSITION               = 0x0016;
inline constexpr uint16_t DMC_ACK_ERR_PREROLL                       = 0x0017;
inline constexpr uint16_t DMC_ACK_ERR_POSTROLL                      = 0x0018;
inline constexpr uint16_t DMC_ACK_ERR_AIM_COD                       = 0x0019;
inline constexpr uint16_t DMC_ACK_ERR_SOFT_UP                       = 0x0020;
inline constexpr uint16_t DMC_ACK_ERR_SOFT_LOW                      = 0x0021;
inline constexpr uint16_t DMC_ACK_ERR_HARD_UP                       = 0x0022;
inline constexpr uint16_t DMC_ACK_ERR_HARD_LOW                      = 0x0023;

// DMC device capabilities
inline constexpr uint32_t DMC_CAP_REAL_TIME                         = 0x0001;
inline constexpr uint32_t DMC_CAP_GO_MOTION                         = 0x0002;
inline constexpr uint32_t DMC_CAP_VIRTUAL_BOOM_SWING_TRACK          = 0x0004;
inline constexpr uint32_t DMC_CAP_VIRTUAL_SWING_PAN                 = 0x0008;
inline constexpr uint32_t DMC_CAP_VIRTUAL_Y_SWING_TRACK             = 0x0010;
inline constexpr uint32_t DMC_CAP_VIRTUAL_X_Y_Z                     = 0x0020;
inline constexpr uint32_t DMC_CAP_OBJECT_TRACKING                   = 0x0040;
inline constexpr uint32_t DMC_CAP_GO_MOTION2                        = 0x0080;
inline constexpr uint32_t DMC_CAP_COUPLE_MOTORS                     = 0x0100;
inline constexpr uint32_t DMC_CAP_REAL_TIME_LOOP                    = 0x0200;
inline constexpr uint32_t DMC_CAP_REAL_TIME_CAMERA                  = 0x0400;

// DMC motor configuration flags
inline constexpr uint8_t DMC_MOTOR_CONFIG_ENABLED                   = 0x01;
inline constexpr uint8_t DMC_MOTOR_CONFIG_BLUR                      = 0x02;
inline constexpr uint8_t DMC_MOTOR_CONFIG_VIRT                      = 0x04; // this axis is used IN virtuals
inline constexpr uint8_t DMC_MOTOR_CONFIG_LIVE_CONTROL              = 0x08;
inline constexpr uint8_t DMC_MOTOR_CONFIG_COUPLE                    = 0x10;
inline constexpr uint8_t DMC_MOTOR_CONFIG_COUPLE_R                  = 0x20;

// DMC GIO Camera Flags
inline constexpr uint32_t DMC_GIO_CAM_SHUTTER_FLAG                  = 0x0001;
inline constexpr uint32_t DMC_GIO_CAM_METER_FLAG                    = 0x0002;

// DMC real-time playback flags
inline constexpr uint16_t DMC_RT_PLAYBACK_PING_PONG                 = 0x0001;
inline constexpr uint16_t DMC_RT_PLAYBACK_LOOP                      = 0x0002;
inline constexpr uint16_t DMC_RT_CAMERA_VIDEO                       = 0x0010;
inline constexpr uint16_t DMC_RT_CAMERA_STILLS                      = 0x0020;


// DMC virtuals support flags
inline constexpr uint8_t DMC_VIRT_TYPE_NONE                         = 0x00;
inline constexpr uint8_t DMC_VIRT_TYPE_BOOM_SWING_TRACK             = 0x01;
inline constexpr uint8_t DMC_VIRT_TYPE_SWING_PAN                    = 0x02;
inline constexpr uint8_t DMC_VIRT_TYPE_Y_SWING_TRACK                = 0x04;
inline constexpr uint8_t DMC_VIRT_TYPE_X_Y_Z                        = 0x08;

// DMC Virtual Motor Numbers
inline constexpr uint8_t DMC_VIRT_TRACK                             = 0x01;
inline constexpr uint8_t DMC_VIRT_EW                                = 0x02;
inline constexpr uint8_t DMC_VIRT_NS                                = 0x03;
inline constexpr uint8_t DMC_VIRT_PAN                               = 0x04;
inline constexpr uint8_t DMC_VIRT_TILT                              = 0x05;
inline constexpr uint8_t DMC_VIRT_ROLL                              = 0x06;

// The boom compensation table is physically an array of stepper 
// motor step positions corresponding to the angles from 
// -60, -59, ..., 0, ..., 59, 60 degrees. See the manual for more 
// details:
// Using Dragonframe 2025 (https://www.dragonframe.com/download/Using%20Dragonframe%202025.pdf)
//  chapter 16 section "Virtual Axis Configuration with DMC"
inline constexpr uint8_t DMC_NUM_BOOM_COMPENSATION_ANGLES           = 121;

inline constexpr uint16_t DMC_MSG_FLAG_ACK                          = 0x8000;

// jog
inline constexpr uint16_t DMC_MSG_MOTOR_JOG_MIN_VELOCITY            = 1;
inline constexpr uint16_t DMC_MSG_MOTOR_JOG_MAX_VELOCITY            = 10000;

// hard stop reasons
inline constexpr uint8_t DMC_MSG_HARD_STOP_REASON_GENERAL           = 0;
inline constexpr uint8_t DMC_MSG_HARD_STOP_REASON_UPPER             = 1;
inline constexpr uint8_t DMC_MSG_HARD_STOP_REASON_LOWER             = 2;
inline constexpr uint8_t DMC_MSG_HARD_STOP_REASON_EXCEPTION         = 100;

// rt run-move
inline constexpr uint16_t DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG       = 0x01;
inline constexpr uint16_t DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP            = 0x02;
inline constexpr uint32_t DMC_MSG_RT_SCALE_RUN_MOVE_FPS             = 1000;
inline constexpr uint16_t DMC_MSG_RT_SCALE_SHOOT_FRAME_BLUR_PERCENT = 10;
inline constexpr uint32_t DMC_MSG_RT_SCALE_JOG_ALL_FPS              = 1000;

// virtuals
inline constexpr uint32_t DMC_MSG_VIRT_CONFIG_SCALE_POSITION        = 100000;
inline constexpr uint32_t DMC_MSG_VIRT_CONFIG_SCALE_LENGTH          = 1000;
inline constexpr uint16_t DMC_MSG_VIRT_JOG_SPEED_INCHING            = 1;
inline constexpr uint16_t DMC_MSG_VIRT_JOG_SPEED_MAX                = 10000;
inline constexpr uint8_t DMC_MSG_VIRT_JOG_ON_LINE_X                 = 0;
inline constexpr uint8_t DMC_MSG_VIRT_JOG_ON_LINE_Y                 = 1;
inline constexpr uint8_t DMC_MSG_VIRT_JOG_ON_LINE_Z                 = 2; // camera line
inline constexpr uint8_t DMC_MSG_VIRT_JOG_ON_LINE_PAN               = 3;
inline constexpr uint8_t DMC_MSG_VIRT_JOG_ON_LINE_TILT              = 4;

inline constexpr uint32_t DMC_MSG_MAX_LENGTH                        = 2048;

// ensure that these structures are defined without padding. such that their 
// layout matches what is seen on the wire.
#pragma pack(push, 1)

/**
 * Every message has a Header. Additionally, some messages contain a 
 * variable-length Data section.
 *
 * All multi-byte values are sent in Little-Endian order. 
 * A WORD is a two-byte value.
 * A DWORD is a four-byte value.
 */
typedef struct _DmcHeader {
    uint8_t  marker[2]; // 'D' 'F'
    uint32_t id;
    uint16_t type;
    uint16_t length;
} DmcHeader;

/**
 * The device must respond to every message it receives. Some requests have specific responses
 * that they expect. Some just expect a response with a MSG_FLAG_ACK set and a response
 * code.
 * 
 * The presence of this flag means that the Data section will contain a response code, rather than
 * any other expected data.
 * 
 * The response should use the message Type and ID from the received message.
 *    Section    | Size |    Description
 * --------------|------|--------------------------------------
 * Response Code | WORD | OK (0x0010) or appropriate Error Code
 *
 *      Response Code    |   Value   |   Description
 * ----------------------|-----------|----------------------------------
 *  OK                   |  0x0010   | Received, understood, and acted on message
 *  ERR_CHECKSUM         |  0x0011   | Checksum didn’t match
 *  ERR_MOVING           |  0x0012   | Can’t handle this command while the rig is moving
 *  ERR_UNSUPPORTED      |  0x0013   | The message type is not known or supported
 *  ERR_RANGE            |  0x0014   | A parameter was out of range
 *  ERR_GENERAL          |  0x0015   | General Error
 *  ERR_NOT_IN_POSITION  |  0x0016   | Rig is not in position to perform requested operation.
 *  ERR_PREROLL          |  0x0017   | A real-time move preroll failed, usually because preroll would send motors past limits.
 *  ERR_POSTROLL         |  0x0018   | A real-time move postroll failed, usually because a limit check failed.
 *  ERR_SOFT_UP          |  0x0020   | Software upper limit hit
 *  ERR_SOFT_LOW         |  0x0021   | Software lower limit hit
 *  ERR_HARD_UP          |  0x0022   | Hardware upper limit hit
 *  ERR_HARD_LOW         |  0x0023   | Hardware lower limit hit

 */
typedef struct _DmcAck {
    DmcHeader header;
    uint16_t  response_code;
} DmcAck;

/**
 * Request basic information about the device. Dragonframe always starts with this request.
 * Also, the device should issue a MSG_HI whenever it starts up. This is helpful to Dragonframe to
 * determine if the device had a random reset. (Some devices can maintain their USB connection
 * while resetting the controller.)
 */
typedef struct _DmcHiRequest {
    DmcHeader header;
} DmcHiRequest;

/**
 * We want to construct a DmcHiResponse with the correct information for Dragonframe.
 * This information can live inside the *.c file which defines the construction routine,
 * or we can place the information inside a config file which is set up by the build 
 * system. In either case we must be careful to ensure that the reported capabilities
 * match what the system is capable of.
 *        Section        |   Size   |   Description
 * ----------------------|----------|---------------------------------------------------------
 *   NAME32              |   BYTES  | Device name, up to 32 bytes, UTF-8 encoded. Fill with \0 characters if less than 32 bytes
 *   FW MAJOR            |   BYTE   | FW major number
 *   FW MINOR            |   BYTE   | FW minor number
 *   FW REV              |   BYTE   | FW revision number
 *   MOTOR COUNT         |   BYTE   | 0-32: Number of motors the device supports
 *   DMX COUNT           |   WORD   | 0-512: Number of DMX channels the device supports
 *   GIO OUT COUNT       |   BYTE   | 0-32: Number of general IO output triggers (relay, logic out)
 *   GIO INPUT COUNT     |   BYTE   | 0-32: Number of input triggers
 *   HW LIMIT COUNT      |   BYTE   | 0-32: How many hardware limit set inputs the device has
 *   UPLOAD FRAME COUNT  |   DWORD  | How many frames of position/dmx data can the device hold
 *   CAPABILITIES        |   DWORD  | Other capabilities the device can advertise
 *   PROTOCOL VERSION    |   WORD   | the DMC protocol is version 2 
 * 
 * Each bit in the capabilities parameter represents functionality the device supports.
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
    uint8_t   name[32];
    uint8_t   fw_major;
    uint8_t   fw_minor;
    uint8_t   fw_rev;
    uint8_t   motor_count;
    uint16_t  dmx_count;
    uint8_t   gio_out_count;
    uint8_t   gio_in_count;
    uint8_t   hw_limit_count;
    uint32_t  upload_frame_count;
    uint32_t  capabilities;
    uint16_t  protocol_version;
} DmcHiResponse;

/**
 * Set DMX light values. The request specifies the initial channel to change, and then one or more
 * light values.
 *
 * If RAMP is set to 1, the device should ramp the light value of each channel up or down to reach
 * the target channel, rather than switching to it immediately.
 */
typedef struct _DmcDmxRequest {
    DmcHeader header;
    uint8_t   ramp;
    uint16_t  start_channel;
    uint8_t   light_values[];
} DmcDmxRequest;

typedef struct _DmcDmxResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcDmxResponse;

/**
 * Set the state for general I/O output triggers.
 */
typedef struct _DmcGioOutRequest {
    DmcHeader header;
    uint32_t  triggers;
} DmcGioOutRequest;

typedef struct _DmcGioOutResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcGioOutResponse;

/**
 * Request the state for general I/O input triggers. Dragonframe may request the input state with
 * this message. The device should also send this message, unsolicited, any time the inputs
 * change. (Of course, the device should use some hysteresis to ensure the inputs don’t change
 * frantically.)
 */
typedef struct _DmcGioInRequest {
    DmcHeader header;
    uint16_t  response_code;
} DmcGioInRequest;

typedef struct _DmcGioInResponse {
    DmcHeader header;
    uint32_t  triggers;
} DmcGioInResponse;

/**
 * Test the camera outputs.
 */
typedef struct _DmcGioCamRequest {
    DmcHeader header;
    uint32_t  triggers;
} DmcGioCamRequest;

typedef struct _DmcGioCamResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcGioCamResponse;

/**
 * Request the motor (and dmx) status. This is only a status of whether something is
 * moving/adjusting.
 */
typedef struct _DmcMotorStatusRequest {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorStatusRequest;

typedef struct _DmcMotorStatusResponse {
    DmcHeader header;
    uint32_t  motor_status;
    uint8_t   dmx_status;
} DmcMotorStatusResponse;

/**
 * Instruct the device to move a motor to a new position.
 */
typedef struct _DmcMotorMoveRequest {
    DmcHeader header;
    uint8_t   motor;
    int32_t   position;
} DmcMotorMoveRequest;

typedef struct _DmcMotorMoveResponse {
    DmcHeader header;
    uint8_t   motor_status;
} DmcMotorMoveResponse;

/**
 * Instruct the device to stop a motor.
 */
typedef struct _DmcMotorStopRequest {
    DmcHeader header;
    uint8_t   motor;
} DmcMotorStopRequest;

typedef struct _DmcMotorStopResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorStopResponse;


/**
 * Instruct the device to stop all motors. The device should decelerate. If the device gets a second
 * STOP_ALL soon after a first, it should HARD STOP. Meaning it should decelerate faster than
 * normal. (If a controller may move large/heavy equipment, it should always decelerate rather
 * than simply stop sending signals.)
 */
typedef struct _DmcMotorStopAllRequest {
    DmcHeader header;
    uint32_t  flags;
} DmcMotorStopAllRequest;

typedef struct _DmcMotorStopAllResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorStopAllResponse;


/**
 * Request the motor positions and move ‘time’.
 * The device should send these out, about every 0.10 seconds, if any of the motors are moving,
 * to keep Dragonframe informed of the positions.
 */
typedef struct _DmcMotorGetPositionRequest {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorGetPositionRequest;

typedef struct _DmcMotorGetPositionResponse {
    DmcHeader header;
    uint32_t  move_time;
    int32_t   motor_positions[MOTOR_COUNT];
} DmcMotorGetPositionResponse;

/**
 * Instruct the device to reset a motor position to a new value. This does not move the motor.
 * After changing the internal position and responding to this message, the device should send a
 * MSG_MOTOR_GET_POSITION message to confirm the new position.
 */
typedef struct _DmcMotorResetPositionRequest {
    DmcHeader header;
    uint8_t   motor;
    int32_t   position;
} DmcMotorResetPositionRequest;

typedef struct _DmcMotorResetPositionResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorResetPositionResponse;

/** 
 * Instruct the device to jog/inch a motor.
 * A speed of 1 means the device should move very slowly, for precise adjustments.
 * A speed of 10,000 means the device should jog using the max velocity and acceleration.
 * A value in between 1 and 10,000 should provide an appropriate speed between 1% and 100%
 * of the jog speed.
 */
typedef struct _DmcMotorJogRequest {
    DmcHeader header;
    uint8_t   motor;
    uint16_t  speed;
    int32_t  destination;
} DmcMotorJogRequest;

typedef struct _DmcMotorJogResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorJogResponse;

/**
 * Configure the motor.
 */
typedef struct _DmcMotorConfigureRequest {
    DmcHeader header;
    uint8_t   motor;
    uint8_t   flags;
} DmcMotorConfigureRequest;

typedef struct _DmcMotorConfigureResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorConfigureResponse;

/**
 * Set the motor speed and acceleration.
 */
typedef struct _DmcMotorSetSpeedRequest {
    DmcHeader header;
    uint8_t   motor;
    uint32_t  max_velocity;
    uint32_t  max_acceleration;
} DmcMotorSetSpeedRequest;

typedef struct _DmcMotorSetSpeedResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorSetSpeedResponse;

/**
 * Set the motor software and hardware limits.
 */
typedef struct _DmcMotorSetLimitsRequest {
    DmcHeader header;
    uint8_t   motor;
    uint8_t   lower_enable;
    uint32_t  lower_limit;
    uint8_t   upper_enable;
    uint32_t  upper_limit;
    uint8_t   hw_set; // this one doesn't make sense, swap high/low????
} DmcMotorSetLimitsRequest;

typedef struct _DmcMotorSetLimitsResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorSetLimitsResponse;

/**
 * The device can send this when a limit has been hit, or if the device’s “emergency stop” button
 * has been pressed.
 */
typedef struct _DmcMotorHardStopRequest {
    DmcHeader header;
    uint16_t  response_code;
} DmcMotorHardStopRequest;

typedef struct _DmcMotorHardStopResponse {
    DmcHeader header;
    uint8_t   reason;
    uint8_t   motor;
} DmcMotorHardStopResponse;

/**
 * Initial message for uploading move data. Any previously loaded move will be cleared.
 */
typedef struct _DmcRtUploadMoveBeginRequest {
    DmcHeader header;
    uint32_t  start_frame;
    uint32_t  end_frame;
} DmcRtUploadMoveBeginRequest;

typedef struct _DmcRtUploadMoveBeginResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtUploadMoveBeginResponse;

/**
 * Send a section of axis frame data.
 */
typedef struct _DmcRtUploadMoveAxisRequest {
    DmcHeader header;
    uint8_t   motor;
    uint32_t  start_index;
    uint32_t  positions[];
} DmcRtUploadMoveAxisRequest;

typedef struct _DmcRtUploadMoveAxisResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtUploadMoveAxisResponse;

/**
 * Send a section of DMX frame data.
 *
 * @note for later, the documentation for this structure is a bit long,
 * and it won't fit into a simple markdown table. Look into HTML tables
 * for converting over the documentation from the DMC-Protocol manual.
 */
typedef struct _DmcRtUploadMoveDmxRequest {
    DmcHeader header;
    uint16_t  channel;
    uint32_t  start_index;
    uint8_t   light_levels[];
} DmcRtUploadMoveRequest;

typedef struct DmcRtUploadMoveDmxResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtUploadMoveDmxResponse;



struct MoveFrameValue {
    uint32_t frame;
    uint32_t values;
};

/**
 * Introduced in Protocol: 2
 * Send a section of trigger data.
 * Only sent if the DMX program has triggers.
 * Only sends frame/value pairs where one or more triggers is on.
 */
typedef struct _DmcRtUploadMoveTriggersRequest {
    DmcHeader      header;
    uint32_t       mask;
    MoveFrameValue move_frame_values[];
} DmcRtUploadMoveTriggersRequest;

typedef struct _DmcRtUploadMoveTriggersResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtUploadMoveTriggersResponse;

/**
 * Final message for uploading move data.
 */
typedef struct _DmcRtUploadMoveEndRequest {
    DmcHeader header;
} DmcRtUploadMoveEndRequest;

typedef struct _DmcRtUploadMoveEndResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtUploadMoveEndResponse;

/**
 * Instructs the device to send all motors (and DMX if applicable) to the uploaded frame position.
 */
typedef struct _DmcRtPositionFrameRequest {
    DmcHeader header;
    uint32_t  frame;
} DmcRtPositionFrameRequest;

typedef struct _DmcRtPositionFrameResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtPositionFrameResponse;

/**
 * Prepares the device to run a section of the move live.
 * The rig must move into a pre-roll position that it calculates, 
 * so that it can accelerate into full speed after “pre-roll time”.
 * Once the rig moves into pre-roll position, it must wait for a MSG_RT_GO to begin the move.
 */
typedef struct _DmcRtRunMoveRequest {
    DmcHeader header;
    uint32_t  fps;
    uint32_t  start_frame;
    uint32_t  end_frame;
    uint32_t  pre_roll_time;
    uint32_t  post_roll_time;
    uint8_t   sync_dmx;
    uint32_t  bloop_location;
    uint16_t  bloop_dmx_channel;
    uint16_t  bloop_time;
    uint16_t  flags;
} DmcRtRunMoveRequest;

typedef struct _DmcRtRunMoveResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtRunMoveResponse;


struct ShootFrameMotorBlur {
    uint8_t  motor;
    uint32_t position_A;
    uint32_t position_B;
};

/** 
 * Set up a blur-motion frame capture
 */
typedef struct _DmcRtShootFrameRequest {
    DmcHeader           header;
    uint32_t            frame;
    uint8_t             direction;
    uint32_t            exposure_time;
    uint16_t            blur_percent;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
} DmcRtShootFrameRequest;

typedef struct _DmcRtShootFrameResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtShootFrameResponse;

/**
 * Set up a blur-motion frame capture (alternate version)
 * The sets of motor numbers and positions are optional.
 * Each motor provided will be part of the blur.
 */
typedef struct _DmcRtShootFrame2Request {
    DmcHeader           header;
    uint32_t            frame;
    uint32_t            exposure_time;
    uint16_t            open_angle;
    uint16_t            close_angle;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
} DmcRtShootFrame2Request;

typedef struct _DmcRtShootFrame2Response {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtShootFrame2Response;

/**
 * After a MSG_RT_RUN_MOVE or MSG_RT_SHOOT_FRAME the device may move the motors
 * into position. When the device has stopped moving, a MSG_RT_GO command will perform the
 * run or shoot of the frame.
 *
 * If the device is running a live move (configured via MSG_RT_RUN_MOVE), it must send a
 * MSG_MOTOR_GET_POSITION each time it reaches a new frame in the move. (If it is slightly
 * after, that is ok.)
 */
typedef struct _DmcRtGoRequest {
    DmcHeader header;
} DmcRtGoRequest;

typedef struct _DmcRtGoResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRgGoResponse;

/**
 * After a MSG_RT_RUN_MOVE or MSG_RT_SHOOT_FRAME has finished shooting, the device
 * sends MSG_RT_END to notify Dragonframe that the process is over.
 */
typedef struct _DmcRtEndRequest {
    DmcHeader header;
} DmcRtEndRequest;

typedef struct _DmcRtEndResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtEndResponse;

/**
 * Jog all motors towards the destination frame.
 * The rig must already be on a frame position that is uploaded to the device.
 */
typedef struct _DmcRtJogAllRequest {
    DmcHeader header;
    uint32_t  fps;
    uint32_t  destination;
} DmcRtJogAllRequest;

typedef struct _DmcRtJogAllResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtJogAllResponse;

/**
 * If the device supports 'real-time looping' capability, it must support this command.
 * The currently running loop/ping-pong must stop after it reaches the next end point.
 */
typedef struct _DmcRtStopLoopRequest {
    DmcHeader header;
} DmcRtStopLoopRequest;

typedef struct DmcRtStopLoopResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcRtStopLoopResponse;

/**
 * Configure virtuals.
 */
typedef struct _DmcVirtConfigRequest {
    DmcHeader header;
    uint8_t   type;
    uint8_t   data[];
} DmcVirtConfigRequest;

typedef struct _DmcVirtConfigRequestBoomSwingTrack {
    DmcHeader    header;
    uint8_t      type;
    uint32_t     boom_motor;
    uint32_t     boom_spu;
    uint32_t     boom_position;
    uint32_t     swing_motor;
    uint32_t     swing_spu;
    uint32_t     swing_position;
    uint32_t     track_motor;
    uint32_t     track_spu;
    uint32_t     track_position;
    uint32_t     pan_motor;
    uint32_t     pan_spu;
    uint32_t     pan_position;
    uint32_t     tilt_motor;
    uint32_t     tilt_spu;
    uint32_t     tilt_position;
    uint32_t     roll_motor;
    uint32_t     roll_spu;
    uint32_t     roll_position;
    uint32_t     boom_length;
    uint32_t     boom_extension;
    uint32_t     nodal_offset_x;
    uint32_t     nodal_offset_y;
    uint32_t     nodal_offset_z;
    uint32_t     boom_compensation[DMC_BOOM_COMPENSATION_ANGLES];
    uint32_t     safe_distance;
} DmcVirtConfigRequestBoomSwingTrack;

typedef struct _DmcVirtConfigRequestSwingPan {
    DmcHeader header;
    uint8_t   type;
    uint32_t  swing_motor;
    uint32_t  swing_spu;
    uint32_t  pan_motor;
    uint32_t  pan_spu;
} DmcVirtConfigRequestSwingPan;

typedef struct _DmcVirtConfigResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcVirtConfigResponse;

/**
 * Send the virtual motor to a new position.
 */
typedef struct _DmcVirtMoveRequest {
    DmcHeader header;
    uint8_t   motor;
    float     position;
} DmcVirtMoveRequest;

typedef struct _DmcVirtMoveResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcVirtMoveResponse;

/**
 * Stop the virtual motor.
 */
typedef struct _DmcVirtStopRequest {
    DmcHeader header;
    uint8_t   motor;
} DmcVirtStopRequest;

typedef struct _DmcVirtStopResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcVirtStopResponse;

/**
 * Instruct the device to jog/inch a virtual motor.
 * A speed of 1 means the device should move very slowly, for precise adjustments.
 * A speed of 10,000 means the device should jog using the max velocity and acceleration.
 * For now, those are the only speeds sent.
 * In the future, we may send intermediate values to signify a target % of the max velocity.
 */
typedef struct _DmcVirtJogRequest {
    DmcHeader header;
    uint8_t   motor;
    uint16_t  speed;
    int32_t   destination; // step position
} DmcVirtJogRequest;

typedef struct _DmcVirtJogResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcVirtJogResponse;

/**
 * Instruct the device to jog along the direction of the camera.
 * Speed is -10000->10000, with 1 being the slowest and 10000 being the fastest.
 */
typedef struct _DmcVirtJogOnLineRequest {
    DmcHeader header;
    uint8_t   axis;
    uint16_t  speed;
} DmcVirtJogOnLineRequest;

typedef struct _DmcVirtJogOnLineResponse {
    DmcHeader header;
    uint16_t  response_code;
} DmcVirtJogOnLineResponse;

/**
 * Request the virtual motor positions.
 */
typedef struct _DmcVirtGetPositionRequest {
    DmcHeader header;
} DmcVirtGetPositionRequest;

typedef struct _DmcVirtGetPositionResponse {
    DmcHeader    header;
    uint32_t     track;
    uint32_t     EW;
    uint32_t     NS;
    uint32_t     pan;
    uint32_t     tilt;
    uint32_t     roll;
    uint8_t      aim_point;
    uint32_t     aim_x;
    uint32_t     aim_y;
    uint32_t     aim_z;
} DmcVirtGetPositionResponse;

/**
 * Introduced in Protocol: 2
 * Configure aim point.
 */
typedef struct _DmcVirtAimPointRequest {
    DmcHeader header;
    uint8_t   enable;
    uint32_t  aim_x;
    uint32_t  aim_y;
    uint32_t  aim_z;
} DmcVirtAimPointRequest;

typedef struct _DmcVirtAimPointResponse {
    DmcHeader header;
    uint8_t   enable;
    uint32_t  aim_x;
    uint32_t  aim_y;
    uint32_t  aim_z;
} DmcVirtAimPointResponse;

#pragma pack(pop)

void dmc_msg_hi_response_init(DmcHiResponse *msg);

#endif // !LIMNMOCO_CM7_MODULE_DMC_PROTOCOL_H

