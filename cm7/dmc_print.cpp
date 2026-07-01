// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "dmc_print.hpp"
#include "conv.hpp"

DmcPrint::DmcPrint()
    : stream(nullptr)
    , tx_buffer()
    , tx_head(0)
    , tx_tail(0)
{
    memset(tx_buffer, 0, sizeof(tx_buffer));
}

void DmcPrint::bind(Stream &stream) {
    this->stream = &stream;
}

void DmcPrint::transmit() {
    for (uint8_t index = 0; index < 32; ++index) {
        if (tx_tail == tx_head) { break; }
        stream->write(tx_buffer[tx_tail]);
        tx_tail = (tx_tail + 1) % tx_length;
    }
}

void DmcPrint::operator()(DmcHeader *packet) {
    enqueue(packet);
}

void DmcPrint::operator()(DmcAck *packet) {
    enqueue(&packet->header);
    switch (packet->response_code) {
    case DMC_ACK_OK:                  enqueue("ack", "ok"); break;
    case DMC_ACK_ERR_CHECKSUM:        enqueue("ack", "err_checksum"); break;
    case DMC_ACK_ERR_MOVING:          enqueue("ack", "err_moving"); break;
    case DMC_ACK_ERR_UNSUPPORTED:     enqueue("ack", "err_unsupported"); break;
    case DMC_ACK_ERR_RANGE:           enqueue("ack", "err_range"); break;
    case DMC_ACK_ERR_GENERAL:         enqueue("ack", "err_general"); break;
    case DMC_ACK_ERR_NOT_IN_POSITION: enqueue("ack", "err_not_in_position"); break;
    case DMC_ACK_ERR_PREROLL:         enqueue("ack", "err_preroll"); break;
    case DMC_ACK_ERR_POSTROLL:        enqueue("ack", "err_postroll"); break;
    case DMC_ACK_ERR_AIM_COD:         enqueue("ack", "err_aim_cod"); break;
    case DMC_ACK_ERR_SOFT_UP:         enqueue("ack", "err_soft_up"); break;
    case DMC_ACK_ERR_SOFT_LOW:        enqueue("ack", "err_soft_low"); break;
    case DMC_ACK_ERR_HARD_UP:         enqueue("ack", "err_hard_up"); break;
    case DMC_ACK_ERR_HARD_LOW:        enqueue("ack", "err_hard_low"); break;
    default:                          enqueue("ack", "unknown"); break;
    }
}

void DmcPrint::operator()(DmcHi *packet) {
    enqueue(&packet->header);
}

void DmcPrint::operator()(DmcDevice *packet) {
   enqueue(&packet->header);
   enqueue("name",    packet->name, (uint32_t)32);
   enqueue("version", packet->fw_major, packet->fw_minor, packet->fw_rev);
   enqueue("motor_count",        (uint32_t)packet->motor_count);
   enqueue("dmx_count",          (uint32_t)packet->dmx_count);
   enqueue("gio_out_count",      (uint32_t)packet->gio_out_count);
   enqueue("gio_in_count",       (uint32_t)packet->gio_in_count);
   enqueue("hw_limit_count",     (uint32_t)packet->hw_limit_count);
   enqueue("upload_frame_count", packet->upload_frame_count);
   enqueue("capabilities",       packet->capabilities);
   enqueue("protocol_version",   (uint32_t)packet->protocol_version);
}

void DmcPrint::operator()(DmcDmx *packet) {
    enqueue(&packet->header);
    enqueue("ramp", (uint32_t)packet->ramp);
    enqueue("start_channel", (uint32_t)packet->start_channel);
    size_t length = packet->header.length - sizeof(byte) - sizeof(word);
    for (size_t i = 0; i < length; ++i) {
        enqueue("light_value", i, (uint32_t)packet->light_values[i]);
    }

}

void DmcPrint::operator()(DmcGioOut *packet) {
    enqueue(&packet->header);
    enqueue("triggers", packet->triggers);
}

void DmcPrint::operator()(DmcGioIn *packet) {
    enqueue(&packet->header);
    enqueue("triggers", packet->triggers);
}

void DmcPrint::operator()(DmcGioCam *packet) {
    enqueue(&packet->header);
    enqueue("triggers.cam_shutter", (packet->triggers & DMC_GIO_CAM_SHUTTER_FLAG) ? "enable" : "disable"); 
    enqueue("triggers.cam_meter", (packet->triggers & DMC_GIO_CAM_METER_FLAG) ? "enable" : "disable");
}

void DmcPrint::operator()(DmcMotorStatus *packet) {
    enqueue(&packet->header);
    enqueue("motor_status", packet->motor_status ? "moving" : "stopped");
    enqueue("dmx_status", packet->dmx_status ? "adjusting" : "stopped");
}

void DmcPrint::operator()(DmcMotorMove *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("position", packet->position);
}

void DmcPrint::operator()(DmcMotorMoveResponse *packet) {
    enqueue(&packet->header);
    enqueue("motor_status", packet->motor_status ? "moving" : "stopped");
}

void DmcPrint::operator()(DmcMotorStop *packet) {
    enqueue(&packet->header);
    enqueue("motor", packet->motor);
}

void DmcPrint::operator()(DmcMotorStopAll *packet) {
    enqueue(&packet->header);
    enqueue("flags", packet->flags ? "silent" : "warning");
}

void DmcPrint::operator()(DmcMotorGetPosition *packet) {
    enqueue(&packet->header);
    enqueue("move_time", packet->move_time);
    for (uint8_t index; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        enqueue("motor_positions", index, packet->motor_positions[index]);
    }
}

void DmcPrint::operator()(DmcMotorResetPosition *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("position", packet->position);
}

void DmcPrint::operator()(DmcMotorJog *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("speed", (uint32_t)packet->speed);
    enqueue("destination", (int32_t)packet->destination);
}

void DmcPrint::operator()(DmcMotorConfigure *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("flags.config", (packet->flags & DMC_MOTOR_CONFIG_ENABLED) ? "true" : "false");
    enqueue("flags.blur",   (packet->flags & DMC_MOTOR_CONFIG_BLUR) ? "true" : "false");
    enqueue("flags.virt",   (packet->flags & DMC_MOTOR_CONFIG_VIRT) ? "true" : "false");
    enqueue("flags.live_control", (packet->flags & DMC_MOTOR_CONFIG_LIVE_CONTROL) ? "true" : "false");
    enqueue("flags.couple", (packet->flags & DMC_MOTOR_CONFIG_COUPLE) ? "true" : "false");
    enqueue("flags.couple_r", (packet->flags & DMC_MOTOR_CONFIG_COUPLE_R) ? "true" : "false");
}

void DmcPrint::operator()(DmcMotorSetSpeed *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("max_velocity (steps/second)", packet->max_velocity);
    enqueue("max_acceleration (steps/second/second)", packet->max_acceleration);
}

void DmcPrint::operator()(DmcMotorSetLimits *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("lower_enable", packet->lower_enable ? "true" : "false");
    enqueue("lower_limit", packet->lower_limit);
    enqueue("upper_enable", packet->upper_enable ? "true" : "false");
    enqueue("upper_limit", packet->upper_limit);
    // hw_set 0x00 means the packet sets soft limits (check)
    // hw_set 0x01 means the packet sets hard limits (check)
    // hw_set flag 0x80 swaps high/low limits. (check)
    uint8_t swap = packet->hw_set & 0x80;
    uint8_t hard = (packet->hw_set & ~0x80);
    enqueue("hw_set", hard ? "hard" : "soft");
    enqueue("swap high/low", swap ? "true" : "false");
}

void DmcPrint::operator()(DmcMotorHardStop *packet) {
    enqueue(&packet->header);

    switch (packet->reason) {
    case DMC_MSG_HARD_STOP_REASON_UPPER:
        enqueue("reason", "upper");
        break;
    case DMC_MSG_HARD_STOP_REASON_LOWER:
        enqueue("reason", "lower");
        break;
    case DMC_MSG_HARD_STOP_REASON_EXCEPTION:
        enqueue("reason", "exception");
        break;
    case DMC_MSG_HARD_STOP_REASON_GENERAL:
    default:
        enqueue("reason", "general");
        break;
    }

    enqueue("motor", (uint32_t)packet->motor);
}

void DmcPrint::operator()(DmcRtUploadMoveBegin *packet) {
    enqueue(&packet->header);
    enqueue("start_frame", packet->start_frame);
    enqueue("end_frame", packet->end_frame);
}

void DmcPrint::operator()(DmcRtUploadMoveAxis *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("start_index", packet->start_index);
    size_t length = (packet->header.length - sizeof(packet->motor) - sizeof(packet->start_index));
    size_t count  = length / sizeof(packet->positions[0]);
    for (size_t i = 0; i < count; ++i) {
        enqueue("positions", i, packet->positions[i]);
    }
}

void DmcPrint::operator()(DmcRtUploadMoveDmx *packet) {
    enqueue(&packet->header);
    enqueue("channel", (uint32_t)packet->channel);
    enqueue("start_index", packet->start_index);
    size_t length = packet->header.length - sizeof(packet->channel) - sizeof(packet->start_index);
    size_t count  = length / sizeof(packet->light_levels[0]);
    for (size_t i = 0; i < count; ++i) {
        enqueue("light_levels", i, (uint32_t)packet->light_levels[i]);
    }
}

void DmcPrint::operator()(DmcRtUploadMoveTriggers *packet) {
    enqueue(&packet->header);
    enqueue("mask", (uint32_t)packet->mask);
    size_t length = packet->header.length - sizeof(packet->mask);
    size_t count  = length / sizeof(packet->move_frame_values[0]);
    for (size_t i = 0; i < count; ++i) {
        MoveFrameValue mfv = packet->move_frame_values[i];
        enqueue("frame", i, mfv.frame);
        enqueue("values", i, mfv.values);
    }
 
}

void DmcPrint::operator()(DmcRtUploadMoveEnd *packet) {
    enqueue(&packet->header);
}

void DmcPrint::operator()(DmcRtPositionFrame *packet) {
    enqueue(&packet->header);
    enqueue("frame", packet->frame);
}

void DmcPrint::operator()(DmcRtRunMove *packet) {
    enqueue(&packet->header);
    enqueue("fps", packet->fps);
    enqueue("start_frame", packet->start_frame);
    enqueue("end_frame", packet->end_frame);
    enqueue("pre_roll_time", packet->pre_roll_time);
    enqueue("post_roll_time", packet->post_roll_time);
    enqueue("sync_dmx", packet->sync_dmx ? "true" : "false");
    enqueue("bloop_location", packet->bloop_location);
    enqueue("bloop_dmx_channel", (uint32_t)packet->bloop_dmx_channel);
    enqueue("bloop_time", (uint32_t)packet->bloop_time);
    switch (packet->flags) {
    case DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG:
        enqueue("flags", "ping_pong");
        break;

    case DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP:
        enqueue("flags", "loop");
        break;

    default:
        enqueue("flags", "none");
        break;
    }
}

void DmcPrint::operator()(DmcRtShootFrame *packet) {
    enqueue(&packet->header);
    enqueue("frame", packet->frame);
    enqueue("direction", packet->direction ? "forward" : "backward");
    enqueue("exposure_time", packet->exposure_time);
    enqueue("blur_percent", (uint32_t)packet->blur_percent / DMC_MSG_RT_SHOOT_FRAME_BLUR_PERCENT_SCALE);
    size_t length = packet->header.length - sizeof(packet->frame) 
                                          - sizeof(packet->direction) 
                                          - sizeof(packet->exposure_time)
                                          - sizeof(packet->blur_percent);
    size_t count = length / sizeof(ShootFrameMotorBlur);
    for (size_t i = 0; i < count; ++i) {
        ShootFrameMotorBlur blur = packet->motor_blur[i];
        enqueue("blur.motor", (uint32_t)blur.motor);
        enqueue("blur.position_A", blur.position_A);
        enqueue("blur.position_B", blur.position_B);
    }
}

void DmcPrint::operator()(DmcRtShootFrame2 *packet) {
    enqueue(&packet->header);
    enqueue("frame", packet->frame);
    enqueue("exposure_time", packet->exposure_time);
    enqueue("open_angle", (uint32_t)packet->open_angle);
    enqueue("close_angle", (uint32_t)packet->close_angle);
    size_t length = packet->header.length - sizeof(packet->frame)
                                          - sizeof(packet->exposure_time)
                                          - sizeof(packet->open_angle)
                                          - sizeof(packet->close_angle);
    size_t count = length / sizeof(ShootFrameMotorBlur);
    for (size_t i = 0; i < count; ++i) {
        ShootFrameMotorBlur blur = packet->motor_blur[i];
        enqueue("blur.motor", (uint32_t)blur.motor);
        enqueue("blur.position_A", blur.position_A);
        enqueue("blur.position_B", blur.position_B);
    }
}

void DmcPrint::operator()(DmcRtGo *packet) {
    enqueue(&packet->header);
}

void DmcPrint::operator()(DmcRtEnd *packet) {
    enqueue(&packet->header);
}

void DmcPrint::operator()(DmcRtJogAll *packet) {
    enqueue(&packet->header);
    enqueue("fps", packet->fps);
    enqueue("destination", packet->destination);
}

void DmcPrint::operator()(DmcRtStopLoop *packet) {
    enqueue(&packet->header);
}

void DmcPrint::operator()(DmcVirtConfig *packet) {
    enqueue(&packet->header);
    switch (packet->type) {
    case DMC_VIRT_TYPE_BOOM_SWING_TRACK:
        (*this)(reinterpret_cast<DmcVirtConfigBoomSwingTrack *>(packet));
        break;
    case DMC_VIRT_TYPE_SWING_PAN:
        (*this)(reinterpret_cast<DmcVirtConfigSwingPan *>(packet));
        break;
    default:
        enqueue("type", "unsupported");
        break;
    }
}

void DmcPrint::operator()(DmcVirtConfigBoomSwingTrack *packet) {
    enqueue("type", "boom-swing-track");
    enqueue("boom_motor",     packet->boom_motor);
    enqueue("boom_spu",       packet->boom_spu);
    enqueue("boom_position",  (float)packet->boom_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("swing_motor",    packet->swing_motor);
    enqueue("swing_spu",      packet->swing_spu);
    enqueue("swing_position", (float)packet->swing_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("track_motor",    packet->track_motor);
    enqueue("track_spu",      packet->track_spu);
    enqueue("track_position", (float)packet->track_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("pan_motor",      packet->pan_motor);
    enqueue("pan_spu",        packet->pan_spu);
    enqueue("pan_position",   (float)packet->pan_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("tilt_motor",     packet->tilt_motor);
    enqueue("tilt_spu",       packet->tilt_spu);
    enqueue("tilt_position",  (float)packet->tilt_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("roll_motor",     packet->roll_motor);
    enqueue("roll_spu",       packet->roll_spu);
    enqueue("roll_position",  (float)packet->roll_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("boom_length",    (float)packet->boom_length / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("boom_extension", (float)packet->boom_extension / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("nodal_offset_x", (float)packet->nodal_offset_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("nodal_offset_y", (float)packet->nodal_offset_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("nodal_offset_z", (float)packet->nodal_offset_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    // #TODO:
    //if (packet->have_compensation()) {
    //    for (size_t i = 0; i < DMC_BOOM_COMPENSATION_ANGLES; ++i) {
    //        enqueue("boom_compensation", i, (float)packet->boom_compensation[i]);
    //    }
    //    enqueue("safe_distance", (float)pcaket->safe_distance / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    //}
}

void DmcPrint::operator()(DmcVirtConfigSwingPan *packet) {
    enqueue("type", "swing-pan");
    enqueue("swing_motor", packet->swing_motor);
    enqueue("swing_spu",   packet->swing_spu);
    enqueue("pan_motor",   packet->pan_motor);
    enqueue("pan_spu",     packet->pan_spu);
}

void DmcPrint::operator()(DmcVirtMove *packet) {
    enqueue(&packet->header);
    enqueue("motor",    (uint32_t)packet->motor);
    enqueue("position", (float)packet->position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
}

void DmcPrint::operator()(DmcVirtStop *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
}

void DmcPrint::operator()(DmcVirtJog *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("speed", (uint32_t)packet->speed);
    enqueue("destination", (int32_t)packet->destination);
}

void DmcPrint::operator()(DmcVirtJogOnLine *packet) {
    enqueue(&packet->header);
    switch (packet->axis) {
    case DMC_MSG_VIRT_JOG_ON_LINE_X:
        enqueue("axis", "x");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_Y:
        enqueue("axis", "y");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_Z:
        enqueue("axis", "z (camera)");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_PAN:
        enqueue("axis", "pan");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_TILT:
        enqueue("axis", "tilt");
        break;

    default:
        enqueue("axis", "unknown");
        break;
    }
    enqueue("speed", (uint32_t)packet->speed);
}

void DmcPrint::operator()(DmcVirtGetPosition *packet) {
    enqueue(&packet->header);
    enqueue("track", (float)packet->track / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("EW",    (float)packet->EW / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("NS",    (float)packet->NS / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("pan",   (float)packet->pan / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("tilt",  (float)packet->tilt / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    enqueue("roll",  (float)packet->roll / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
    // #TODO:
    //if (packet->have_aim_point()) {
    //    enqueue("aim_point", "enabled");
    //    enqueue("aim_x", packet->aim_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    //    enqueue("aim_y", packet->aim_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    //    enqueue("aim_z", packet->aim_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    //} else {
    //    enqueue("aim_point", "disabled");
    //}
 
}

void DmcPrint::operator()(DmcVirtAimPoint *packet) {
    enqueue(&packet->header);
    enqueue("aim_point", packet->enable ? "enabled" : "disabled");
    enqueue("aim_x",     (float)packet->aim_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("aim_y",     (float)packet->aim_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    enqueue("aim_z",     (float)packet->aim_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
}


void DmcPrint::enqueue(uint8_t data) {
    uint32_t next = (tx_head + 1) % tx_length;
    if (next == tx_tail) {
        return;
    }

    tx_buffer[tx_head] = data;
    tx_head            = next;
}

void DmcPrint::enqueue(char data) {
    enqueue(static_cast<uint8_t>(data));
}

void DmcPrint::enqueue(uint8_t *data, uint32_t length) {
    for (uint32_t index = 0; index < length; ++index) {
        enqueue(data[index]);
    }
}

void DmcPrint::enqueue(uint32_t value) {
    uint32_t length = strlenu(value);
    uint8_t  buffer[length];

    convu(buffer, length, value, 10);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(int32_t value) {
    uint32_t length = strleni(value);
    uint8_t  buffer[length];

    convi(buffer, length, value, 10);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(float value) {
    uint32_t length = strlenf(value);
    uint8_t  buffer[length];

    convf(buffer, length, value);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(char const *data) {
    for (uint32_t index = 0; data[index] && index < UINT32_MAX; ++index) {
        enqueue(data[index]);
    }
}

void DmcPrint::enqueue(char const *field, uint8_t value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint32_t value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, int32_t value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, float value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint8_t major, uint8_t minor, uint8_t rev) {
    enqueue(field);
    enqueue(" = ");
    enqueue(major);
    enqueue('.');
    enqueue(minor);
    enqueue('.');
    enqueue(rev);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint32_t index, char value) {
    enqueue(field);
    enqueue('[');
    enqueue(index);
    enqueue("] = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint32_t index, uint32_t value) {
    enqueue(field);
    enqueue('[');
    enqueue(index);
    enqueue("] = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint32_t index, int32_t value) {
    enqueue(field);
    enqueue('[');
    enqueue(index);
    enqueue("] = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint32_t index, float value) {
    enqueue(field);
    enqueue('[');
    enqueue(index);
    enqueue("] = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint8_t *value, uint32_t length) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value, length);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, char const *value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(DmcHeader *header) {
    enqueue("marker", (uint32_t)0, (char)header->marker[0]);
    enqueue("marker", (uint32_t)1, (char)header->marker[1]);
    enqueue("id", header->id);
    switch (header->type) {
    case DMC_MSG_HI:                      enqueue("type", "hi"); break;
    case DMC_MSG_DMX:                     enqueue("type", "dmx"); break;
    case DMC_MSG_GIO_OUT:                 enqueue("type", "gio_out"); break;
    case DMC_MSG_GIO_IN:                  enqueue("type", "gio_in"); break;
    case DMC_MSG_GIO_CAM:                 enqueue("type", "gio_cam"); break;
    case DMC_MSG_MOTOR_STATUS:            enqueue("type", "motor_status"); break;
    case DMC_MSG_MOTOR_MOVE:              enqueue("type", "motor_move"); break;
    case DMC_MSG_MOTOR_STOP:              enqueue("type", "motor_stop"); break;
    case DMC_MSG_MOTOR_STOP_ALL:          enqueue("type", "motor_stop_all"); break;
    case DMC_MSG_MOTOR_GET_POSITION:      enqueue("type", "motor_get_position"); break;
    case DMC_MSG_MOTOR_RESET_POSITION:    enqueue("type", "motor_reset_position"); break;
    case DMC_MSG_MOTOR_JOG:               enqueue("type", "motor_jog"); break;
    case DMC_MSG_MOTOR_CONFIGURE:         enqueue("type", "motor_configure"); break;
    case DMC_MSG_MOTOR_SET_SPEED:         enqueue("type", "motor_set_speed"); break;
    case DMC_MSG_MOTOR_SET_LIMITS:        enqueue("type", "motor_set_limits"); break;
    case DMC_MSG_MOTOR_HARD_STOP:         enqueue("type", "motor_hard_stop"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_BEGIN:    enqueue("type", "rt_upload_move_begin"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_AXIS:     enqueue("type", "rt_upload_move_axis"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_DMX:      enqueue("type", "rt_upload_move_dmx"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_END:      enqueue("type", "rt_upload_move_end"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS: enqueue("type", "rt_upload_move_triggers"); break;
    case DMC_MSG_RT_POSITION_FRAME:       enqueue("type", "rt_position_frame"); break;
    case DMC_MSG_RT_RUN_MOVE:             enqueue("type", "rt_run_move"); break;
    case DMC_MSG_RT_SHOOT_FRAME:          enqueue("type", "rt_shoot_frame"); break;
    case DMC_MSG_RT_SHOOT_FRAME2:         enqueue("type", "rt_shoot_frame_2"); break;
    case DMC_MSG_RT_GO:                   enqueue("type", "rt_go"); break;
    case DMC_MSG_RT_END:                  enqueue("type", "rt_end"); break;
    case DMC_MSG_RT_STOP_LOOP:            enqueue("type", "rt_stop_loop"); break;
    case DMC_MSG_RT_JOG_ALL:              enqueue("type", "rt_jog_all"); break;
    case DMC_MSG_VIRT_CONFIG:             enqueue("type", "virt_config"); break;
    case DMC_MSG_VIRT_MOVE:               enqueue("type", "virt_move"); break;
    case DMC_MSG_VIRT_STOP:               enqueue("type", "virt_stop"); break;
    case DMC_MSG_VIRT_JOG:                enqueue("type", "virt_jog"); break;
    case DMC_MSG_VIRT_GET_POSITION:       enqueue("type", "virt_get_position"); break;
    case DMC_MSG_VIRT_JOG_ON_LINE:        enqueue("type", "virt_jog_on_line"); break;
    case DMC_MSG_VIRT_AIM_POINT:          enqueue("type", "virt_aim_point"); break;
    default:                              enqueue("type", "unknown"); break;
    }
    enqueue("length", (uint32_t)header->length);
}


