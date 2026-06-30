// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * \file dmc.hpp
 * \brief limnmoco dmc protocol implementation
 */

#ifndef LIMNMOCO_DMC_HPP
#define LIMNMOCO_DMC_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>

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

// virtual motor number assignments
#define DMC_VIRT_TRACK 1
#define DMC_VIRT_EW    2
#define DMC_VIRT_NS    3
#define DMC_VIRT_PAN   4
#define DMC_VIRT_TILT  5
#define DMC_VIRT_ROLL  6

// length of the boom compensation table
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
#define DMC_MSG_RT_RUN_MOVE_FPS_SCALE             1000
#define DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG       0x01
#define DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP            0x02
#define DMC_MSG_RT_SHOOT_FRAME_BLUR_PERCENT_SCALE 10
#define DMC_MSG_RT_JOG_ALL_FPS_SCALE              1000

// virtuals constants
#define DMC_MSG_VIRT_CONFIG_POSITION_SCALE 100000.0f
#define DMC_MSG_VIRT_CONFIG_LENGTH_SCALE   1000.0f
#define DMC_MSG_VIRT_JOG_SPEED_INCHING     1
#define DMC_MSG_VIRT_JOG_SPEED_MAX         10000
#define DMC_MSG_VIRT_JOG_ON_LINE_X         0
#define DMC_MSG_VIRT_JOG_ON_LINE_Y         1
#define DMC_MSG_VIRT_JOG_ON_LINE_Z         2 // camera line
#define DMC_MSG_VIRT_JOG_ON_LINE_PAN       3
#define DMC_MSG_VIRT_JOG_ON_LINE_TILT      4

// dmc message constants
#define DMC_MSG_MAX_LENGTH 2048

// dmc message helper macros
#define DMC_MSG_DATA_LENGTH(T) (sizeof(T) - sizeof(DmcHeader))

// #NOTE: ensure that these structures are defined without padding.
//   this is so we can reinterpret_cast a buffer of unsigned char
//   into Dmc Packets. This is so we can avoid copying data as much 
//   as possible. This way, as soon as we read the bytes into the 
//   receive buffer, we have 'constructed' the packet. it's just 
//   a matter of interpreting the bytes as if it were one already.
//   This also relies on the state machine parsing the incoming packets
//   to be correct, though that would be true even if we were constructing 
//   copies of the incoming data.
#pragma pack(push, 1)

struct DmcHeader {
    uint8_t  marker[2]; // 'D', 'F'
    uint32_t id;
    uint16_t type;
    uint16_t length;

    DmcHeader(uint32_t id, uint16_t type, uint16_t length)
        : marker()
        , id(id)
        , type(type)
        , length(length)
    {
        marker[0] = 'D';
        marker[1] = 'F';
    }
};

struct DmcAck {
    DmcHeader header;
    uint16_t  response_code;

    DmcAck(uint32_t id, uint16_t type, uint16_t response_code)
        : header(id, type | DMC_MSG_FLAG_ACK, DMC_MSG_DATA_LENGTH(DmcAck))
        , response_code(response_code)
    {}
};

struct DmcHi {
    DmcHeader header;

    DmcHi(uint32_t id)
        : header(id, DMC_MSG_HI, DMC_MSG_DATA_LENGTH(DmcHi))
    {}
};

struct DmcDevice {
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

    DmcDevice(char const *name,
              uint8_t fw_major,
              uint8_t fw_minor,
              uint8_t fw_rev,
              uint8_t motor_count,
              uint16_t dmx_count,
              uint8_t gio_out_count,
              uint8_t gio_in_count,
              uint8_t hw_limit_count,
              uint32_t upload_frame_count,
              uint32_t capabilities,
              uint16_t  protocol_version)
        : header(0, DMC_MSG_HI, DMC_MSG_DATA_LENGTH(DmcDevice))
        , name()
        , fw_major(fw_major)
        , fw_minor(fw_minor)
        , fw_rev(fw_rev)
        , motor_count(motor_count)
        , dmx_count(dmx_count)
        , gio_out_count(gio_out_count)
        , gio_in_count(gio_in_count)
        , hw_limit_count(hw_limit_count)
        , upload_frame_count(upload_frame_count)
        , capabilities(capabilities)
        , protocol_version(protocol_version)
    {
        memset(this->name, 0, sizeof(this->name));
        strncpy((char *)this->name, name, sizeof(this->name));
    }
};

struct DmcDmx {
    DmcHeader header;
    uint8_t   ramp;
    uint16_t  start_channel;
    uint8_t   light_values[];
};

struct DmcGioOut {
    DmcHeader header;
    uint32_t  triggers;
};

struct DmcGioIn {
    DmcHeader header;
    uint32_t  triggers;

    DmcGioIn(uint32_t id, uint32_t triggers)
        : header(id, DMC_MSG_GIO_IN, DMC_MSG_DATA_LENGTH(DmcGioIn))
        , triggers(triggers)
    {}
};

struct DmcGioCam {
    DmcHeader header;
    uint32_t  triggers;
};

struct DmcMotorStatus {
    DmcHeader header;
    uint32_t  motor_status;
    uint8_t   dmx_status;

    DmcMotorStatus(uint32_t id, uint32_t motor_status, uint8_t dmx_status) 
        : header(id, DMC_MSG_MOTOR_STATUS, DMC_MSG_DATA_LENGTH(DmcMotorStatus))
        , motor_status(motor_status)
        , dmx_status(dmx_status)
    {}
};

struct DmcMotorMove {
    DmcHeader header;
    uint8_t   motor;
    int32_t   position;
};

struct DmcMotorMoveResponse {
    DmcHeader header;
    uint8_t   motor_status;

    DmcMotorMoveResponse(uint32_t id, uint8_t motor_status) 
        : header(id, DMC_MSG_MOTOR_MOVE, DMC_MSG_DATA_LENGTH(DmcMotorMoveResponse))
        , motor_status(motor_status)
    {}
};

struct DmcMotorStop {
    DmcHeader header;
    uint8_t   motor;
};

struct DmcMotorStopAll {
    DmcHeader header;
    uint32_t  flags;
};

struct DmcMotorGetPosition {
    DmcHeader header;
    uint32_t  move_time;
    int32_t   motor_positions[MOTOR_COUNT];

    DmcMotorGetPosition(uint32_t id, uint32_t move_time, int32_t *motor_positions)
        : header(id, DMC_MSG_MOTOR_GET_POSITION, DMC_MSG_DATA_LENGTH(DmcMotorGetPosition))
        , move_time(move_time)
        , motor_positions()
    {
        memcpy(this->motor_positions, motor_positions, sizeof(int32_t) * MOTOR_COUNT);
    }
};

struct DmcMotorResetPosition {
    DmcHeader header;
    uint8_t   motor;
    int32_t   position;
};

struct DmcMotorJog {
    DmcHeader header;
    uint8_t   motor;
    uint16_t  speed;
    int32_t  destination;
};

struct DmcMotorConfigure {
    DmcHeader header;
    uint8_t   motor;
    uint8_t   flags;
};

struct DmcMotorSetSpeed {
    DmcHeader header;
    uint8_t   motor;
    uint32_t  max_velocity;
    uint32_t  max_acceleration;
};

struct DmcMotorSetLimits {
    DmcHeader header;
    uint8_t   motor;
    uint8_t   lower_enable;
    uint32_t  lower_limit;
    uint8_t   upper_enable;
    uint32_t  upper_limit;
    uint8_t   hw_set; // this one doesn't make sense, swap high/low????
};

struct DmcMotorHardStop {
    DmcHeader header;
    uint8_t   reason;
    uint8_t   motor;

    DmcMotorHardStop(uint32_t id, uint8_t reason, uint8_t motor)
        : header(id, DMC_MSG_MOTOR_HARD_STOP, DMC_MSG_DATA_LENGTH(DmcMotorHardStop))
        , reason(reason)
        , motor(motor)
    {}
};

struct DmcRtUploadMoveBegin {
    DmcHeader header;
    uint32_t  start_frame;
    uint32_t  end_frame;
};

struct DmcRtUploadMoveAxis {
    DmcHeader header;
    uint8_t   motor;
    uint32_t  start_index;
    uint32_t  positions[];
};

struct DmcRtUploadMoveDmx {
    DmcHeader header;
    uint16_t  channel;
    uint32_t  start_index;
    uint8_t   light_levels[];
};

struct MoveFrameValue {
    uint32_t frame;
    uint32_t values;
};

struct DmcRtUploadMoveTriggers {
    DmcHeader      header;
    uint32_t       mask;
    MoveFrameValue move_frame_values[];
};

struct DmcRtUploadMoveEnd {
    DmcHeader header;
};

struct DmcRtPositionFrame {
    DmcHeader header;
    uint32_t  frame;
};

struct DmcRtRunMove {
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
};

struct ShootFrameMotorBlur {
    uint8_t  motor;
    uint32_t position_A;
    uint32_t position_B;
};

struct DmcRtShootFrame {
    DmcHeader           header;
    uint32_t            frame;
    uint8_t             direction;
    uint32_t            exposure_time;
    uint16_t            blur_percent;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
};

struct DmcRtShootFrame2 {
    DmcHeader           header;
    uint32_t            frame;
    uint32_t            exposure_time;
    uint16_t            open_angle;
    uint16_t            close_angle;
    ShootFrameMotorBlur motor_blur[MOTOR_COUNT];
};

struct DmcRtGo {
    DmcHeader header;
};

struct DmcRtEnd {
    DmcHeader header;
};

struct DmcRtJogAll {
    DmcHeader header;
    uint32_t  fps;
    uint32_t  destination;
};

struct DmcRtStopLoop {
    DmcHeader header;
};

struct DmcVirtConfig {
    DmcHeader header;
    uint8_t   type;
    uint8_t   data[];
};

struct DmcVirtConfigBoomSwingTrack {
    DmcHeader header;
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

    bool have_compensation() {
        // if the compensation table exists in the packet, then 
        // the length of the packet will include it.
        size_t total_length = sizeof(DmcVirtConfig);
        size_t reported_length = header.length;
        return reported_length == total_length;
    }
};

struct DmcVirtConfigSwingPan {
    DmcHeader header;
    uint8_t   type;
    uint32_t  swing_motor;
    uint32_t  swing_spu;
    uint32_t  pan_motor;
    uint32_t  pan_spu;
};

struct DmcVirtMove {
    DmcHeader header;
    uint8_t   motor;
    uint32_t  position;
};

struct DmcVirtStop {
    DmcHeader header;
    uint8_t   motor;
};

struct DmcVirtJog {
    DmcHeader header;
    uint8_t   motor;
    uint16_t  speed;
    int32_t   destination; // step position
};

struct DmcVirtJogOnLine {
    DmcHeader header;
    uint8_t   axis;
    uint16_t  speed;
};

struct DmcVirtGetPosition {
    DmcHeader header;
    uint32_t  track;
    uint32_t  EW;
    uint32_t  NS;
    uint32_t  pan;
    uint32_t  tilt;
    uint32_t  roll;
    uint8_t   data[];

    DmcVirtGetPosition(uint32_t id,
                       float track,
                       float EW,
                       float NS,
                       float pan,
                       float tilt,
                       float roll)
        : header(id, DMC_MSG_VIRT_GET_POSITION, DMC_MSG_DATA_LENGTH(DmcVirtGetPosition))
        , track(track)
        , EW(EW)
        , NS(NS)
        , pan(pan)
        , tilt(tilt)
        , roll(roll)
    {}

    bool have_aim_point() {
        return DMC_MSG_DATA_LENGTH(DmcVirtGetPosition) == header.length;
    }
};

struct DmcVirtAimPoint {
    DmcHeader header;
    uint8_t   enable;
    uint32_t  aim_x;
    uint32_t  aim_y;
    uint32_t  aim_z;
};

#pragma pack(pop)

// define these routines to handle specific packets
class DmcStream {
public:
    enum State {
      STATE_WAIT_D,
      STATE_WAIT_F,
      STATE_READ_HEADER,
      STATE_READ_PAYLOAD,
      STATE_CHECKSUM,
    };

    DmcStream();

    void bind(Stream &stream);
    void reset();
    void receive();
    void transmit();

protected:
    void enqueue(void *packet, uint16_t length);
    void ack(uint32_t id, uint16_t type, uint32_t response);
    void ack(DmcHeader &header, uint32_t response);

    static uint16_t checksum(void *buffer, size_t length);
    static uint16_t checkbytes(uint16_t checksum);
    void packet_switch(void *buffer, size_t length);

    virtual void on_ack(DmcAck *packet) = 0;
    virtual void on_hi(DmcHi *packet);
    virtual void on_dmx(DmcDmx *packet) = 0;
    virtual void on_gio_out(DmcGioOut *packet) = 0;
    virtual void on_gio_in(DmcAck *packet) = 0;
    virtual void on_gio_cam(DmcGioCam *packet) = 0;
    virtual void on_motor_status(DmcAck *packet) = 0;
    virtual void on_motor_move(DmcMotorMove *packet) = 0;
    virtual void on_motor_stop(DmcMotorStop *packet) = 0;
    virtual void on_motor_stop_all(DmcMotorStopAll *packet) = 0;
    virtual void on_motor_get_position(DmcAck *packet) = 0;
    virtual void on_motor_reset_position(DmcMotorResetPosition *packet) = 0;
    virtual void on_motor_jog(DmcMotorJog *packet) = 0;
    virtual void on_motor_configure(DmcMotorConfigure *packet) = 0;
    virtual void on_motor_set_speed(DmcMotorSetSpeed *packet) = 0;
    virtual void on_motor_set_limits(DmcMotorSetLimits *packet) = 0;
    virtual void on_motor_hard_stop(DmcAck *packet) = 0;
    virtual void on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet) = 0;
    virtual void on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet) = 0;
    virtual void on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet) = 0;
    virtual void on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet) = 0;
    virtual void on_rt_upload_move_end(DmcRtUploadMoveEnd *packet) = 0;
    virtual void on_rt_position_frame(DmcRtPositionFrame *packet) = 0;
    virtual void on_rt_run_move(DmcRtRunMove *packet) = 0;
    virtual void on_rt_shoot_frame(DmcRtShootFrame *packet) = 0;
    virtual void on_rt_shoot_frame_2(DmcRtShootFrame2 *packet) = 0;
    virtual void on_rt_go(DmcRtGo *packet) = 0;
    virtual void on_rt_end(DmcRtEnd *packet) = 0;
    virtual void on_rt_jog_all(DmcRtJogAll *packet) = 0;
    virtual void on_rt_stop_loop(DmcRtStopLoop *packet) = 0;
    virtual void on_virt_config(DmcVirtConfig *packet) = 0;
    virtual void on_virt_move(DmcVirtMove *packet) = 0;
    virtual void on_virt_stop(DmcVirtStop *packet) = 0;
    virtual void on_virt_jog(DmcVirtJog *packet) = 0;
    virtual void on_virt_get_position(DmcAck *packet) = 0;
    virtual void on_virt_jog_on_line(DmcVirtJogOnLine *packet) = 0;
    virtual void on_virt_aim_point(DmcVirtAimPoint *packet) = 0;
    virtual void on_unknown(DmcHeader *packet) = 0;

private:
    DmcDevice dmc_device;
    Stream   *stream;
    State     state;
    size_t    index;
    uint16_t  length;

    alignas(4) uint8_t rx_buffer[DMC_MSG_MAX_LENGTH];

    struct QueuedPacketLarge {
      uint16_t length;
      uint16_t index;
      uint8_t  buffer[DMC_MSG_MAX_LENGTH];
    };

    static const uint8_t         tx_queue_large_length = 2;
    uint8_t                      tx_queue_large_head;
    uint8_t                      tx_queue_large_tail;
    alignas(4) QueuedPacketLarge tx_queue_large[tx_queue_large_length];

    struct QueuedPacketSmall {
        uint16_t length;
        uint16_t index;
        uint8_t  buffer[64];
    };

    static const uint8_t         tx_queue_small_length = 10;
    uint8_t                      tx_queue_small_head;
    uint8_t                      tx_queue_small_tail;
    alignas(4) QueuedPacketSmall tx_queue_small[tx_queue_small_length];

    bool tx_large_queue_active;
};

#endif // !LIMNMOCO_DMC_HPP
