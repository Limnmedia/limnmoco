// SPDX-License-Identifier: BSD-3-Clause

/**
 * \file dmc.hpp
 * \brief limnmoco dmc protocol implementation
 */

#ifndef LIMNMOCO_DMC_HPP
#define LIMNMOCO_DMC_HPP

#include <cstdint>
#include <cstddef>

#include <Arduino.h>

#include "config.hpp"

#define DMC_MSG_HI                      0x0001

#define DMC_MSG_DMX                     0x0020

#define DMC_MSG_GIO_OUT                 0x0021
#define DMC_MSG_GIO_IN                  0x0022
#define DMC_MSG_GIO_CAM                 0x0023

#define DMC_MSG_MOTOR_STATUS            0x0030
#define DMC_MSG_MOTOR_MOVE              0x0031
#define DMC_MSG_MOTOR_STOP              0x0032
#define DMC_MSG_MOTOR_STOP_ALL          0x0033
#define DMC_MSG_MOTOR_GET_POSITION      0x0034
#define DMC_MSG_MOTOR_RESET_POSITION    0x0035
#define DMC_MSG_MOTOR_JOG               0x0036
#define DMC_MSG_MOTOR_CONFIGURE         0x0037
#define DMC_MSG_MOTOR_SET_SPEED         0x0038
#define DMC_MSG_MOTOR_SET_LIMITS        0x0039
#define DMC_MSG_MOTOR_HARD_STOP         0x003A

#define DMC_MSG_RT_UPLOAD_MOVE_BEGIN    0x0100
#define DMC_MSG_RT_UPLOAD_MOVE_AXIS     0x0101
#define DMC_MSG_RT_UPLOAD_MOVE_DMX      0x0102
#define DMC_MSG_RT_UPLOAD_MOVE_END      0x0103
#define DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS 0x0104

#define DMC_MSG_RT_POSITION_FRAME       0x0110
#define DMC_MSG_RT_RUN_MOVE             0x0111
#define DMC_MSG_RT_SHOOT_FRAME          0x0112
#define DMC_MSG_RT_SHOOT_FRAME2         0x0115
#define DMC_MSG_RT_GO                   0x0113
#define DMC_MSG_RT_END                  0x0114
#define DMC_MSG_RT_STOP_LOOP            0x0116
#define DMC_MSG_RT_JOG_ALL              0x0120

#define DMC_MSG_VIRT_CONFIG             0x0200
#define DMC_MSG_VIRT_MOVE               0x0201
#define DMC_MSG_VIRT_STOP               0x0202
#define DMC_MSG_VIRT_JOG                0x0203
#define DMC_MSG_VIRT_GET_POSITION       0x0205
#define DMC_MSG_VIRT_JOG_ON_LINE        0x0206
#define DMC_MSG_VIRT_AIM_POINT          0x0207

// not in the manual???
#define DMC_MSG_FAN_CONTROL             0x0300

#define DMC_ACK_OK                      0x0010
#define DMC_ACK_ERR_CHECKSUM            0x0011
#define DMC_ACK_ERR_MOVING              0x0012
#define DMC_ACK_ERR_UNSUPPORTED         0x0013
#define DMC_ACK_ERR_RANGE               0x0014
#define DMC_ACK_ERR_GENERAL             0x0015
#define DMC_ACK_ERR_NOT_IN_POSITION     0x0016
#define DMC_ACK_ERR_PREROLL             0x0017
#define DMC_ACK_ERR_POSTROLL            0x0018
#define DMC_ACK_ERR_AIM_COD             0x0019

#define DMC_ACK_ERR_SOFT_UP             0x0020
#define DMC_ACK_ERR_SOFT_LOW            0x0021
#define DMC_ACK_ERR_HARD_UP             0x0022
#define DMC_ACK_ERR_HARD_LOW            0x0023

// capabilities
#define DMC_CAP_REAL_TIME                0x0001
#define DMC_CAP_GO_MOTION                0x0002
#define DMC_CAP_VIRTUAL_BOOM_SWING_TRACK 0x0004
#define DMC_CAP_VIRTUAL_SWING_PAN        0x0008
#define DMC_CAP_VIRTUAL_Y_SWING_TRACK    0x0010
#define DMC_CAP_VIRTUAL_X_Y_Z            0x0020
#define DMC_CAP_OBJECT_TRACKING          0x0040
#define DMC_CAP_GO_MOTION2               0x0080
#define DMC_CAP_COUPLE_MOTORS            0x0100
#define DMC_CAP_REAL_TIME_LOOP           0x0200
#define DMC_CAP_REAL_TIME_CAMERA         0x0400


// motor configuration flags
#define DMC_MOTOR_CONFIG_ENABLED      0x01
#define DMC_MOTOR_CONFIG_BLUR         0x02
#define DMC_MOTOR_CONFIG_VIRT         0x04 // this axis is used IN virtuals
#define DMC_MOTOR_CONFIG_LIVE_CONTROL 0x08
#define DMC_MOTOR_CONFIG_COUPLE       0x10
#define DMC_MOTOR_CONFIG_COUPLE_R     0x20

#define DMC_GIO_CAM_SHUTTER_FLAG 0x0001
#define DMC_GIO_CAM_METER_FLAG   0x0002

// real-time playback flags
#define DMC_RT_PLAYBACK_PING_PONG 0x0001
#define DMC_RT_PLAYBACK_LOOP      0x0002
#define DMC_RT_CAMERA_VIDEO       0x0010
#define DMC_RT_CAMERA_STILLS      0x0020

// virtuals support types
#define DMC_VIRT_TYPE_NONE             0x0000
#define DMC_VIRT_TYPE_BOOM_SWING_TRACK 0x0001
#define DMC_VIRT_TYPE_SWING_PAN        0x0002
#define DMC_VIRT_TYPE_Y_SWING_TRACK    0x0004
#define DMC_VIRT_TYPE_X_Y_Z            0x0008

#define DMC_VIRT_TRACK 1
#define DMC_VIRT_EW    2
#define DMC_VIRT_NS    3
#define DMC_VIRT_PAN   4
#define DMC_VIRT_TILT  5
#define DMC_VIRT_ROLL  6

#define DMC_BOOM_COMPENSATION_ANGLES 121 // -60, -59, ... 0, ... 59, 60

// message flags
#define DMC_DMX_FLAG_FINAL_SET 0x80000000

#define DMC_MSG_FLAG_ACK 0x8000

// jog
#define DMC_MSG_MOTOR_JOG_MIN_VELOCITY 1
#define DMC_MSG_MOTOR_JOG_MAX_VELOCITY 10000

// hard stop reasons
#define DMC_MSG_HARD_STOP_REASON_GENERAL   0
#define DMC_MSG_HARD_STOP_REASON_UPPER     1
#define DMC_MSG_HARD_STOP_REASON_LOWER     2
#define DMC_MSG_HARD_STOP_REASON_EXCEPTION 100

// rt run-move
#define DMC_MSG_RT_RUN_MOVE_FPS_SCALE       1000
#define DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG 0x01
#define DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP      0x02
#define DMC_MSG_RT_JOG_ALL_FPS_SCALE        1000

// virtuals
#define DMC_MSG_VIRT_CONFIG_POSITION_SCALE 100000
#define DMC_MSG_VIRT_CONFIG_LENGTH_SCALE   1000
#define DMC_MSG_VIRT_JOG_SPEED_INCHING     1
#define DMC_MSG_VIRT_JOG_SPEED_MAX         10000
#define DMC_MSG_VIRT_JOG_ON_LINE_X         0
#define DMC_MSG_VIRT_JOG_ON_LINE_Y         1
#define DMC_MSG_VIRT_JOG_ON_LINE_Z         2 // camera line
#define DMC_MSG_VIRT_JOG_ON_LINE_PAN       3
#define DMC_MSG_VIRT_JOG_ON_LINE_TILT      4

#define DMC_MSG_MAX_LENGTH 2048

typedef uint8_t   byte;
typedef uint16_t  word;
typedef uint32_t dword;

// ensure that these structures are defined without padding
#pragma pack(push, 1)

struct DmcHeader {
    byte  marker[2]; // 'D', 'F'
    dword id;
    word  type;
    word  length;
};

struct DmcAck {
    DmcHeader header;
    word      response_code;
    word      checksum;
};

struct DmcHi {
    DmcHeader header;
    word      checksum;
};

struct MsgHiResponse {
    DmcHeader header;
    byte      name[32];
    byte      fw_major;
    byte      fw_minor;
    byte      fw_rev;
    byte      motor_count;
    word      dmx_count;
    byte      gio_out_count;
    byte      gio_in_count;
    byte      hw_limit_count;
    dword     upload_frame_count;
    dword     capabilities;
    word      protocol_version;
    word      checksum;
};

struct DmcDmx {
    DmcHeader header;
    byte      ramp;
    word      start_channel;
    byte      light_values[];
};

struct DmcGioOut {
    DmcHeader header;
    dword     triggers;
    word      checksum;
};

struct DmcGioIn {
    DmcHeader header;
    dword     triggers;
    word      checksum;
};

struct DmcGioCam {
    DmcHeader header;
    dword     triggers;
    word      checksum;
};

struct DmcMotorStatus {
    DmcHeader header;
    dword     motor_status;
    byte      dmx_status;
    word      checksum;
};

struct DmcMotorMove {
    DmcHeader header;
    byte      motor;
    dword     position;
    word      checksum;
};

struct DmcMotorMoveResponse {
    DmcHeader header;
    byte      motor_status;
    word      checksum;
};

struct DmcMotorStop {
    DmcHeader header;
    byte      motor;
    word      checksum;
};

struct DmcMotorStopAll {
    DmcHeader header;
    dword     flags;
    word      checksum;
};

struct DmcMotorGetPosition {
    DmcHeader header;
    dword     move_time;
    dword     motor_positions[MOTOR_COUNT];
    word      checksum;
};

struct DmcMotorResetPosition {
    DmcHeader header;
    byte      motor;
    dword     position;
    word      checksum;
};

struct DmcMotorJog {
    DmcHeader header;
    byte      motor;
    word      speed;
    dword     destination;
    word      checksum;
};

struct DmcMotorConfigure {
    DmcHeader header;
    byte      motor;
    byte      flags;
    word      checksum;
};

struct DmcMotorSetSpeed {
    DmcHeader header;
    byte      motor;
    dword     max_velocity;
    dword     max_acceleration;
    word      checksum;
};

struct DmcMotorSetLimits {
    DmcHeader header;
    byte      motor;
    byte      lower_enable;
    dword     lower_limit;
    byte      upper_enable;
    dword     upper_limit;
    byte      hw_set; // this one doesn't make sense, swap high/low????
    word      checksum;
};

struct DmcMotorHardStop {
    DmcHeader header;
    byte      reason;
    byte      motor;
    word      checksum;
};

struct DmcRtUploadMoveBegin {
    DmcHeader header;
    dword     start_frame;
    dword     end_frame;
    word      checksum;
};

struct DmcRtUploadMoveAxis {
    DmcHeader header;
    byte      motor;
    dword     start_index;
    dword     positions[];
};

struct DmcRtUploadMoveDmx {
    DmcHeader header;
    word      channel;
    dword     start_index;
    byte      light_levels[];
};

struct MoveFrameValue {
    dword frame;
    dword values;
};

struct DmcRtUploadMoveTriggers {
    DmcHeader      header;
    dword          mask;
    MoveFrameValue move_frame_values[];
};

struct DmcRtUploadMoveEnd {
    DmcHeader header;
    word      checksum;
};

struct DmcRtPositionFrame {
    DmcHeader header;
    dword     frame;
    word      checksum;
};

struct DmcRtRunMove {
    DmcHeader header;
    dword     fps;
    dword     start_frame;
    dword     end_frame;
    dword     pre_roll_time;
    dword     post_roll_time;
    byte      sync_dmx;
    dword     bloop_location;
    word      bloop_dmx_channel;
    word      bloop_time;
    word      flags;
    word      checksum;
};

struct ShootFrameMotorBlur {
    byte  motor;
    dword position_A;
    dword position_B;
};

struct DmcRtShootFrame {
    DmcHeader           header;
    dword               frame;
    byte                direction;
    dword               exposure_time;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
    word                checksum;
};

struct DmcRtShootFrame2 {
    DmcHeader           header;
    dword               frame;
    dword               exposure_time;
    word                open_angle;
    word                close_angle;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
    word                checksum;
};

struct DmcRtGo {
    DmcHeader header;
    word      checksum;
};

struct DmcRtEnd {
    DmcHeader header;
    word      checksum;
};

struct DmcRtJogAll {
    DmcHeader header;
    dword     fps;
    dword     destination;
    word      checksum;
};

struct DmcRtStopLoop {
    DmcHeader header;
    word      checksum;
};

struct DmcVirtConfig {
    DmcHeader header;
    byte      type;
    byte      data[];
};

struct DmcVirtConfigBoomSwingTrack {
    DmcHeader header;
    byte      type;
    dword     boom_motor;
    dword     boom_spu;
    dword     boom_position;
    dword     swing_motor;
    dword     swing_spu;
    dword     swing_position;
    dword     track_motor;
    dword     track_spu;
    dword     track_position;
    dword     pan_motor;
    dword     pan_spu;
    dword     pan_position;
    dword     tilt_motor;
    dword     tilt_spu;
    dword     tilt_position;
    dword     roll_motor;
    dword     roll_spu;
    dword     roll_position;
    dword     boom_length;
    dword     boom_extension;
    dword     nodal_offset_x;
    dword     nodal_offset_y;
    dword     nodal_offset_z;
    dword     boom_compensation[DMC_BOOM_COMPENSATION_ANGLES];
    dword     safe_distance;
    word      checksum;
};

struct DmcVirtConfigSwingPan {
    DmcHeader header;
    byte      data;
    dword     swing_motor;
    dword     swing_spu;
    dword     pan_motor;
    dword     pan_spu;
    word      checksum;
};

struct DmcVirtMove {
    DmcHeader header;
    byte      motor;
    dword     position;
    word      checksum;
};

struct DmcVirtStop {
    DmcHeader header;
    byte      motor;
    word      checksum;
};

struct DmcVirtJog {
    DmcHeader header;
    byte      motor;
    word      speed;
    dword     destination;
    word      checksum;
};

struct DmcVirtJogOnLine {
    DmcHeader header;
    byte      axis;
    word      speed;
    word      checksum;
};

struct DmcVirtGetPosition {
    DmcHeader header;
    dword     track;
    dword     EW;
    dword     NS;
    dword     pan;
    dword     tilt;
    dword     roll;
    byte      aim_point;
    dword     aim_x;
    dword     aim_y;
    dword     aim_z;
    word      checksum;
};

struct DmcVirtAimPoint {
    DmcHeader header;
    byte      enable;
    dword     aim_x;
    dword     aim_y;
    dword     aim_z;
    word      checksum;
};

#pragma pack(pop)

uint16_t dmc_checksum(uint8_t *buffer, size_t length);
word dmc_checkbytes(uint16_t checksum);
void dmc_packet_switch(uint8_t *buffer, size_t length);

// define these routines to handle specific packets
void dmc_on_ack(DmcAck *ack);
void dmc_on_hi(DmcHi *hi);
void dmc_on_dmx(DmcDmx *dmx);
void dmc_on_gio_out(DmcGioOut *gio_out);
void dmc_on_gio_in(DmcGioIn *gio_in);
void dmc_on_gio_cam(DmcGioCam *gio_cam);
void dmc_on_motor_status(DmcMotorStatus *motor_status);
void dmc_on_motor_move(DmcMotorMove *motor_move);
void dmc_on_motor_stop(DmcMotorStop *motor_stop);
void dmc_on_motor_stop_all(DmcMotorStopAll *motor_stop_all);
void dmc_on_motor_get_position(DmcMotorGetPosition *motor_get_position);
void dmc_on_motor_reset_position(DmcMotorResetPosition *motor_reset_position);
void dmc_on_motor_jog(DmcMotorJog *motor_jog);
void dmc_on_motor_configure(DmcMotorConfigure *packet);
void dmc_on_motor_set_speed(DmcMotorSetSpeed *packet);
void dmc_on_motor_set_limits(DmcMotorSetLimits *packet);
void dmc_on_motor_hard_stop(DmcMotorHardStop *packet);
void dmc_on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet);
void dmc_on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet);
void dmc_on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet);
void dmc_on_rt_upload_move_end(DmcRtUploadMoveEnd *packet);
void dmc_on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet);
void dmc_on_rt_position_frame(DmcRtPositionFrame *packet);
void dmc_on_rt_run_move(DmcRtRunMove *packet);
void dmc_on_rt_shoot_frame(DmcRtShootFrame *packet);
void dmc_on_rt_shoot_frame_2(DmcRtShootFrame2 *packet);
void dmc_on_rt_go(DmcRtGo *packet);
void dmc_on_rt_end(DmcRtEnd *packet);
void dmc_on_rt_stop_loop(DmcRtStopLoop *packet);
void dmc_on_rt_jog_all(DmcRtJogAll *packet);
void dmc_on_virt_config(DmcVirtConfig *packet);
void dmc_on_virt_move(DmcVirtMove *packet);
void dmc_on_virt_stop(DmcVirtStop *packet);
void dmc_on_virt_jog(DmcVirtJog *packet);
void dmc_on_virt_get_position(DmcVirtGetPosition *packet);
void dmc_on_virt_jog_on_line(DmcVirtJogOnLine *packet);
void dmc_on_virt_aim_point(DmcVirtAimPoint *packet);
void dmc_on_unknown(DmcHeader *packet);

class DmcStream {
public:
    enum State {
      STATE_WAIT_D,
      STATE_WAIT_F,
      STATE_READ_HEADER,
      STATE_READ_PAYLOAD,
      STATE_CHECKSUM,
    };

    void reset() {
        state  = STATE_WAIT_D;
        index  = 0;
        length = 0;
    }

    void update(Stream &stream) {
        while (stream.available() > 0) {
            uint8_t b = stream.read();

            switch (state) {
            case STATE_WAIT_D: {
                if (b == 'D') {
                    buffer[index++] = b;
                    state = STATE_WAIT_F;
                }
                break;
            }

            case STATE_WAIT_F: {
                if (b == 'F') {
                    buffer[index++] = b;
                    state = STATE_READ_HEADER;
                    break;
                }

                if (b == 'D') {
                    // still waiting for byte 'F'
                    // we can ignore the extra 'D'
                    break;
                }
                // #NOTE: The only state transition into 'F'
                // is from 'D', and if the byte immediately following
                // 'D' is not 'F' the packet is malformed.
                reset();
                break;
            }

            case STATE_READ_HEADER: {
                buffer[index++] = b;

                if (index == sizeof(DmcHeader)) {
                    length = (buffer[8] << 8) | buffer[9];

                    size_t total = sizeof(DmcHeader) + length + 2;
                    if (total > DMC_MSG_MAX_LENGTH) {
                        reset(); // packet too larget
                        break;
                    }

                    if (length == 0) {
                        state = STATE_CHECKSUM;
                    } else {
                        state = STATE_READ_PAYLOAD;
                    }

                    length = total;
                }

                break;
            }

            case STATE_READ_PAYLOAD: {
                buffer[index++] = b;
                if (index == length) {
                    state = STATE_CHECKSUM;
                }

                break;
            }

            case STATE_CHECKSUM: {
                uint16_t checksum = dmc_checksum(buffer, length);
                if (checksum == 0) {
                    dmc_packet_switch(buffer, length);
                }
            }
            }
        }
    }

private:
    Stream  *stream;
    State    state;
    size_t   index;
    uint16_t length;

    alignas(4) uint8_t buffer[DMC_MSG_MAX_LENGTH];
};

#endif // !LIMNMOCO_DMC_HPP
