// SPDX-License-Identifier: BSD-3-Clause

#include "dmc_debug.hpp"
#include "conv.hpp"

#include "debug.hpp"

// when we are stubbing dragonframe we need to play along with the
// protocol. and send back dummy data as if we are doing real work.
// when we are sitting alongside the limnmoco proper, we dont need
// to worry about communicating back. in fact, if we communicate back
// to the limnmoco, will our firmware get confused? not if we write
// the debug stream on the limnmoco to handle it.

DmcDebug::DmcDebug() 
    : DmcStream()
    , debug_stream(nullptr)
    , tx_buffer()
    , tx_head(0)
    , tx_tail(0) 
    , stub_gio_in(
        0,
        0
    )
    , stub_motor_status(
        0,
        0,
        0
    )
    , stub_motor_move_response(
        0,
        1
    )
    , stub_motor_positions()
    , stub_motor_get_position(
        0,
        0,
        stub_motor_positions
    )
    , stub_motor_hard_stop(
        0,
        0,
        0
    )
    , stub_virt_get_position(
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        1
    )
{
    memset(stub_motor_positions, 0, sizeof(int32_t) * MOTOR_COUNT);
    memcpy(stub_motor_get_position.motor_positions, stub_motor_positions, sizeof(int32_t) * MOTOR_COUNT);
    memset(tx_buffer, 0, tx_length);

    //print("limnmoco-debug, v0.1.1\n");
}

void DmcDebug::bindDebug(Stream &stream) {
    debug_stream = &stream;
}

void DmcDebug::transmitDebug() {
    for (uint32_t index = 0; index < 32; ++index) {
        if (tx_tail == tx_head) { break; }
        debug_stream->write(tx_buffer[tx_tail]);
        tx_tail = (tx_tail + 1) % tx_length;
    }

    // we transmit from head
    // we print to tail
    // so the amount we need to transmit
    // is tail - head
    // but the full condition is head === tail
    // so if tx_tail == tx_head than their difference is 0
    //if (tx_head == tx_tail) {
    //    debug_pulse(PIN_DBG_1);
    //    return;
    //}

    //if (tx_head < tx_tail) {
    //    debug_pulse(PIN_DBG_2);
    //    // the amount we need to transmit is simply
    //    // tail - head.
    //    size_t available = tx_tail - tx_head;
    //    // make sure we don't spend a lot of time
    //    // waiting for the UART.
    //    size_t amount = (available <= 32) ? available : 32;
    //    debug_stream->write(tx_buffer + tx_head, amount);
    //    tx_head = (tx_head + amount) % tx_length;
    //    debug_pulse(PIN_DBG_3);
    //    return;
    //}

    //debug_pulse(PIN_DBG_4);
    // tx_head >= tx_tail
    // the string left to transmit must wrap around the ring
    // the amount we can write in a single go is between head
    // and the end
    //size_t available = tx_length - tx_head;
    //size_t amount = (available <= 32) ? available : 32;
    //debug_stream->write(tx_buffer + tx_head, amount);
    //tx_head = (tx_head + amount) % tx_length;

    //debug_pulse(PIN_DBG_5);
    // if tx_head > tx_tail then we know we sent == 32 bytes.
    // and we must not have wrapped. so we can exit early.
    //if (tx_head > tx_tail) { 
    //    return; 
    //}
    // if tx_head wrapped, then we sent < 32 bytes.
    // and tx_head < tx_tail
    // so we can write the min(whats_left_to_send, 32 - what_we_sent)
    // we know the array is larger than 32, but that isn't generic.
    //debug_pulse(PIN_DBG_6);
    //available = tx_tail - tx_head;
    //amount = (32 - amount);
    //amount = (available <= amount) ? available : amount;
    //debug_stream->write(tx_buffer + tx_head, amount);
    //tx_head = (tx_head + amount) % tx_length;
    //debug_pulse(PIN_DBG_7);
}

void DmcDebug::enqueueDebug(uint8_t data) {
    uint32_t next_head = (tx_head + 1) % tx_length;
    if (next_head == tx_tail) {
        return;
    }

    tx_buffer[tx_head] = data;
    tx_head = next_head;
}

void DmcDebug::enqueueDebug(uint8_t *str, size_t length) {
    for (uint32_t index = 0; index < length; ++index) {
        enqueueDebug(str[index]);
    }
    //if (tx_tail == tx_head) {
    //    return; // tx buffer full, dropping string
    //}
    // we transmit from the head,
    // we print to the tail
    // so when we enqueue a string we are appending to the 
    // tail. this means that the amount of space between the 
    // tail, and the head, wrapping around the buffer is space
    // we have to write the string.

    //if (tx_head > tx_tail) {
    //    debug_pulse(PIN_DBG_1);
    //    size_t space = tx_head - tx_tail;
    //    if (space < length) { return; } // not enough space
    //    // since head > tail, there is no need to wrap 
    //    // the string around the ring.
    //    memcpy(tx_buffer + tx_tail, str, length);
    //    tx_tail = (tx_tail + length) % tx_length;
    //    return;
    //}

    //debug_pulse(PIN_DBG_2);
    // head < tail
    //size_t filled = tx_tail - tx_head;
    //size_t empty  = tx_length - filled;
    //if (empty < length) {
    //    debug_pulse(PIN_DBG_3);
    //    return; 
    //}

    //size_t available = tx_length - tx_tail;
    //if (available < length) {
    //    debug_pulse(PIN_DBG_4);
    //    // there is enough space between the tail and the end to 
    //    // fit the string
    //    memcpy(tx_buffer + tx_tail, str, length);
    //    tx_tail = (tx_tail + length) % tx_length;
    //    return;
    //}

    //debug_pulse(PIN_DBG_5);
    // we have to split the string around the ring
    // first fill the remaining space at the end of
    // the buffer.
    //memcpy(tx_buffer + tx_tail, str, available);
    //tx_tail   = 0; // wrap
    //length   -= available;
    //memcpy(tx_buffer + tx_tail, str + available, length);
    // since we just wrapped, we know head must be in 
    // the buffer, thus there is no way we could have written
    // enough to need to wrap
    //tx_tail = (tx_tail + length);
}

void DmcDebug::print(uint32_t value) {
    uint32_t length = strlenu(value);
    uint8_t buffer[length];

    convu(buffer, length, value, 10);
    enqueueDebug(buffer, length);
}

void DmcDebug::print(int32_t value) {
    uint32_t length = strleni(value);
    uint8_t  buffer[length];
    convi(buffer, length, value, 10);
    enqueueDebug(buffer, length);
}

void DmcDebug::print(float value) {
    uint32_t length = strlenf(value);
    uint8_t  buffer[length];
    convf(buffer, length, value);
    enqueueDebug(buffer, length);
}

void DmcDebug::print(char const *cstr) {
    uint32_t length = strlen(cstr);
    enqueueDebug((uint8_t *)cstr, length);
}

void DmcDebug::print(char c) {
    enqueueDebug((uint8_t)c);
}

void DmcDebug::print(char const *field, uint32_t value) {
    print(field);
    print(" = ");
    print(value);
    print('\n');
}

void DmcDebug::print(char const *field, int32_t value) {
    print(field);
    print(" = ");
    print(value);
    print('\n');
}

void DmcDebug::print(char const *field, float value) {
    print(field);
    print(" = ");
    print(value);
    print('\n');
}

void DmcDebug::print(uint32_t major, uint32_t minor, uint32_t rev) {
    print("version = ");
    print(major);
    print('.');
    print(minor);
    print('.');
    print(rev);
    print('\n');
}

void DmcDebug::print(char const *field, uint32_t index, uint32_t value) {
    print(field);
    print('[');
    print(index);
    print("] = ");
    print(value);
    print('\n');
}

void DmcDebug::print(char const *field, uint32_t index, int32_t value) {
    print(field);
    print('[');
    print(index);
    print("] = ");
    print(value);
    print('\n');
}

void DmcDebug::print(char const *field, uint32_t index, float value) {
    print(field);
    print('[');
    print(index);
    print("] = ");
    print(value);
    print('\n');
}

void DmcDebug::print(char const *field, uint8_t *const value, size_t length) {
    print(field);
    print(" = ");
    for (size_t i = 0; i < length; ++i) {
        if (value[i] == '\0') { break; }
        print((char)value[i]);
    }
    print('\n');
}

void DmcDebug::print(char const *field, char const *str) {
    print(field);
    print(" = ");
    print(str);
    print('\n');
}

void DmcDebug::print(DmcHeader &header) {
    print("marker", header.marker, sizeof(header.marker));
    print("id", header.id);
    switch (header.type) {
    case DMC_MSG_HI:                      print("type", "hi"); break;
    case DMC_MSG_DMX:                     print("type", "dmx"); break;
    case DMC_MSG_GIO_OUT:                 print("type", "gio_out"); break;
    case DMC_MSG_GIO_IN:                  print("type", "gio_in"); break;
    case DMC_MSG_GIO_CAM:                 print("type", "gio_cam"); break;
    case DMC_MSG_MOTOR_STATUS:            print("type", "motor_status"); break;
    case DMC_MSG_MOTOR_MOVE:              print("type", "motor_move"); break;
    case DMC_MSG_MOTOR_STOP:              print("type", "motor_stop"); break;
    case DMC_MSG_MOTOR_STOP_ALL:          print("type", "motor_stop_all"); break;
    case DMC_MSG_MOTOR_GET_POSITION:      print("type", "motor_get_position"); break;
    case DMC_MSG_MOTOR_RESET_POSITION:    print("type", "motor_reset_position"); break;
    case DMC_MSG_MOTOR_JOG:               print("type", "motor_jog"); break;
    case DMC_MSG_MOTOR_CONFIGURE:         print("type", "motor_configure"); break;
    case DMC_MSG_MOTOR_SET_SPEED:         print("type", "motor_set_speed"); break;
    case DMC_MSG_MOTOR_SET_LIMITS:        print("type", "motor_set_limits"); break;
    case DMC_MSG_MOTOR_HARD_STOP:         print("type", "motor_hard_stop"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_BEGIN:    print("type", "rt_upload_move_begin"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_AXIS:     print("type", "rt_upload_move_axis"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_DMX:      print("type", "rt_upload_move_dmx"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_END:      print("type", "rt_upload_move_end"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS: print("type", "rt_upload_move_triggers"); break;
    case DMC_MSG_RT_POSITION_FRAME:       print("type", "rt_position_frame"); break;
    case DMC_MSG_RT_RUN_MOVE:             print("type", "rt_run_move"); break;
    case DMC_MSG_RT_SHOOT_FRAME:          print("type", "rt_shoot_frame"); break;
    case DMC_MSG_RT_SHOOT_FRAME2:         print("type", "rt_shoot_frame_2"); break;
    case DMC_MSG_RT_GO:                   print("type", "rt_go"); break;
    case DMC_MSG_RT_END:                  print("type", "rt_end"); break;
    case DMC_MSG_RT_STOP_LOOP:            print("type", "rt_stop_loop"); break;
    case DMC_MSG_RT_JOG_ALL:              print("type", "rt_jog_all"); break;
    case DMC_MSG_VIRT_CONFIG:             print("type", "virt_config"); break;
    case DMC_MSG_VIRT_MOVE:               print("type", "virt_move"); break;
    case DMC_MSG_VIRT_STOP:               print("type", "virt_stop"); break;
    case DMC_MSG_VIRT_JOG:                print("type", "virt_jog"); break;
    case DMC_MSG_VIRT_GET_POSITION:       print("type", "virt_get_position"); break;
    case DMC_MSG_VIRT_JOG_ON_LINE:        print("type", "virt_jog_on_line"); break;
    case DMC_MSG_VIRT_AIM_POINT:          print("type", "virt_aim_point"); break;
    default:                              print("type", "unknown"); break;
    }
    print("length", (uint32_t)header.length);
}


void DmcDebug::on_ack(DmcAck *ack) {
    // #NOTE: I don't expect Dragonframe to ever send an 
    //  acknowledge packet on it's own.
    print(ack->header);
    switch (ack->response_code) {
    case DMC_ACK_OK:                  print("ack", "ok"); break;
    case DMC_ACK_ERR_CHECKSUM:        print("ack", "err_checksum"); break;
    case DMC_ACK_ERR_MOVING:          print("ack", "err_moving"); break;
    case DMC_ACK_ERR_UNSUPPORTED:     print("ack", "err_unsupported"); break;
    case DMC_ACK_ERR_RANGE:           print("ack", "err_range"); break;
    case DMC_ACK_ERR_GENERAL:         print("ack", "err_general"); break;
    case DMC_ACK_ERR_NOT_IN_POSITION: print("ack", "err_not_in_position"); break;
    case DMC_ACK_ERR_PREROLL:         print("ack", "err_preroll"); break;
    case DMC_ACK_ERR_POSTROLL:        print("ack", "err_postroll"); break;
    case DMC_ACK_ERR_AIM_COD:         print("ack", "err_aim_cod"); break;
    case DMC_ACK_ERR_SOFT_UP:         print("ack", "err_soft_up"); break;
    case DMC_ACK_ERR_SOFT_LOW:        print("ack", "err_soft_low"); break;
    case DMC_ACK_ERR_HARD_UP:         print("ack", "err_hard_up"); break;
    case DMC_ACK_ERR_HARD_LOW:        print("ack", "err_hard_low"); break;
    default:                          print("ack", "unknown"); break;
    }
}

void DmcDebug::on_hi(DmcHi *packet) {
    print(packet->header);
    if (packet->header.length > DMC_MSG_DATA_LENGTH(DmcHi)) {
        DmcDevice *packet = reinterpret_cast<DmcDevice *>(packet);
        print("name",               packet->name, sizeof(packet->name));
        print(packet->fw_major, packet->fw_minor, packet->fw_rev);
        print("motor_count",        (uint32_t)packet->motor_count);
        print("dmx_count",          (uint32_t)packet->dmx_count);
        print("gio_out_count",      (uint32_t)packet->gio_out_count);
        print("gio_in_count",       (uint32_t)packet->gio_in_count);
        print("hw_limit_count",     (uint32_t)packet->hw_limit_count);
        print("upload_frame_count", packet->upload_frame_count);
        print("capabilities",       packet->capabilities);
        print("protocol_version",   (uint32_t)packet->protocol_version);
    }

    DmcStream::on_hi(packet);
}

void DmcDebug::on_dmx(DmcDmx *packet) {
    print(packet->header);
    print("ramp", (uint32_t)packet->ramp);
    print("start_channel", (uint32_t)packet->start_channel);
    size_t length = packet->header.length - sizeof(byte) - sizeof(word);
    for (size_t i = 0; i < length; ++i) {
        print("light_value", i, (uint32_t)packet->light_values[i]);
    }

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_gio_out(DmcGioOut *packet) {
    print(packet->header);
    print("triggers", packet->triggers);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_gio_in(DmcAck *packet) {
    print(packet->header);
    print("response_code", (uint32_t)packet->response_code);

    // the size of a GIO_IN packet == sizeof ACK packet
    // and the Dragonframe Request packet will have the 
    // type "DMC_MSG_GIO_ACK", so there is no way to tell 
    // the difference between a message from dragonframe
    // and a message from limnmoco here.
    // "triggers will be equal to response code if it is
    // coming from limnmoco". Yes. and triggers could be 
    // equal to response code when coming from dragonframe
    // through coincidence. (I think we will be interpreting 
    // the checkbytes as if they are the response code
    // when we read a packet from dragonframe.)
    // it's fine, just make a note of it.
    if (packet->header.length == DMC_MSG_DATA_LENGTH(DmcGioIn)) {
        DmcGioIn *packet = reinterpret_cast<DmcGioIn *>(packet);
        print("triggers", packet->triggers);
    }
    
    enqueue(&stub_gio_in, sizeof(DmcGioIn));
}

void DmcDebug::on_gio_cam(DmcGioCam *packet) {
    print(packet->header);
    print("triggers.cam_shutter", (packet->triggers & DMC_GIO_CAM_SHUTTER_FLAG) ? "enable" : "disable"); 
    print("triggers.cam_meter", (packet->triggers & DMC_GIO_CAM_METER_FLAG) ? "enable" : "disable");

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_status(DmcAck *packet) {
    print(packet->header);
    print("response_code", (uint32_t)packet->response_code);

    if (packet->header.length == DMC_MSG_DATA_LENGTH(DmcMotorStatus)) {
        DmcMotorStatus *packet = reinterpret_cast<DmcMotorStatus *>(packet);
        print("motor_status", packet->motor_status ? "moving" : "stopped");
        print("dmx_status", packet->dmx_status ? "adjusting" : "stopped");
    }

    enqueue(&stub_motor_status, sizeof(DmcMotorStatus));
}

void DmcDebug::on_motor_move(DmcMotorMove *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("position", packet->position);

    enqueue(&stub_motor_move_response, sizeof(DmcMotorMoveResponse));
}

void DmcDebug::on_motor_stop(DmcMotorStop *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_stop_all(DmcMotorStopAll *packet) {
    print(packet->header);
    print("flags", packet->flags ? "silent" : "warning");
    
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_get_position(DmcAck *packet) {
    print(packet->header);

    // Dragonframe will only send an Ack packet, however 
    // when we are watching the Limnmoco, it will send out 
    // the full DmcMotorGetPosition packet. in order to 
    // properly handle that we could check the size reported 
    // by the header. Presumably Dragonframe will send a small 
    // Ack packet, and Limnmoco will send a full DmcMotorGetPosition
    if (packet->header.length == DMC_MSG_DATA_LENGTH(DmcMotorGetPosition)) {
        // we are reading a packet from the Limnmoco
        DmcMotorGetPosition *packet = reinterpret_cast<DmcMotorGetPosition *>(packet);
        print("move_time", packet->move_time);
        for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {
            print("motor_positions", i, packet->motor_positions[i]);
        }
    }

    enqueue(&stub_motor_get_position, sizeof(DmcMotorGetPosition));
}

void DmcDebug::on_motor_reset_position(DmcMotorResetPosition *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("position", packet->position);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_jog(DmcMotorJog *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("speed", (uint32_t)packet->speed);
    print("destination", (int32_t)packet->destination);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_configure(DmcMotorConfigure *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("flags.config", (packet->flags & DMC_MOTOR_CONFIG_ENABLED) ? "true" : "false");
    print("flags.blur",   (packet->flags & DMC_MOTOR_CONFIG_BLUR) ? "true" : "false");
    print("flags.virt",   (packet->flags & DMC_MOTOR_CONFIG_VIRT) ? "true" : "false");
    print("flags.live_control", (packet->flags & DMC_MOTOR_CONFIG_LIVE_CONTROL) ? "true" : "false");
    print("flags.couple", (packet->flags & DMC_MOTOR_CONFIG_COUPLE) ? "true" : "false");
    print("flags.couple_r", (packet->flags & DMC_MOTOR_CONFIG_COUPLE_R) ? "true" : "false");

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_set_speed(DmcMotorSetSpeed *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("max_velocity (steps/second)", packet->max_velocity);
    print("max_acceleration (steps/second/second)", packet->max_acceleration);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_set_limits(DmcMotorSetLimits *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("lower_enable", packet->lower_enable ? "true" : "false");
    print("lower_limit", packet->lower_limit);
    print("upper_enable", packet->upper_enable ? "true" : "false");
    print("upper_limit", packet->upper_limit);
    // hw_set 0x00 means the packet sets soft limits (check)
    // hw_set 0x01 means the packet sets hard limits (check)
    // hw_set flag 0x80 swaps high/low limits. (check)
    uint8_t swap = packet->hw_set & 0x80;
    uint8_t hard = (packet->hw_set & ~0x80);
    print("hw_set", hard ? "hard" : "soft");
    print("swap high/low", swap ? "true" : "false");

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_motor_hard_stop(DmcAck *packet) {
    print(packet->header);

    if (packet->header.length > DMC_MSG_DATA_LENGTH(DmcAck)) {
        DmcMotorHardStop *packet = reinterpret_cast<DmcMotorHardStop *>(packet);
        switch (packet->reason) {
        case DMC_MSG_HARD_STOP_REASON_UPPER:
            print("reason", "upper");
            break;

        case DMC_MSG_HARD_STOP_REASON_LOWER:
            print("reason", "lower");
            break;

        case DMC_MSG_HARD_STOP_REASON_EXCEPTION:
            print("reason", "exception");
            break;

        case DMC_MSG_HARD_STOP_REASON_GENERAL:
        default:
            print("reason", "general");
            break;
        }
        print("motor", (uint32_t)packet->motor);
    }

    enqueue(&stub_motor_hard_stop, sizeof(stub_motor_hard_stop));
}

void DmcDebug::on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet) {
    print(packet->header);
    print("start_frame", packet->start_frame);
    print("end_frame", packet->end_frame);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("start_index", packet->start_index);
    size_t length = (packet->header.length - sizeof(packet->motor) - sizeof(packet->start_index));
    size_t count  = length / sizeof(packet->positions[0]);
    for (size_t i = 0; i < count; ++i) {
        print("positions", i, packet->positions[i]);
    }
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet) {
    print(packet->header);
    print("channel", (uint32_t)packet->channel);
    print("start_index", packet->start_index);
    size_t length = packet->header.length - sizeof(packet->channel) - sizeof(packet->start_index);
    size_t count  = length / sizeof(packet->light_levels[0]);
    for (size_t i = 0; i < count; ++i) {
        print("light_levels", i, (uint32_t)packet->light_levels[i]);
    }
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet) {
    print(packet->header);
    print("mask", (uint32_t)packet->mask);
    size_t length = packet->header.length - sizeof(packet->mask);
    size_t count  = length / sizeof(packet->move_frame_values[0]);
    for (size_t i = 0; i < count; ++i) {
        MoveFrameValue mfv = packet->move_frame_values[i];
        print("frame", i, mfv.frame);
        print("values", i, mfv.values);
    }
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_upload_move_end(DmcRtUploadMoveEnd *packet) {
    print(packet->header);
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_position_frame(DmcRtPositionFrame *packet) {
    print(packet->header);
    print("frame", packet->frame);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_run_move(DmcRtRunMove *packet) {
    print(packet->header);
    print("fps", packet->fps);
    print("start_frame", packet->start_frame);
    print("end_frame", packet->end_frame);
    print("pre_roll_time", packet->pre_roll_time);
    print("post_roll_time", packet->post_roll_time);
    print("sync_dmx", packet->sync_dmx ? "true" : "false");
    print("bloop_location", packet->bloop_location);
    print("bloop_dmx_channel", (uint32_t)packet->bloop_dmx_channel);
    print("bloop_time", (uint32_t)packet->bloop_time);
    switch (packet->flags) {
    case DMC_MSG_RT_RUN_MOVE_FLAGS_PING_PONG:
        print("flags", "ping_pong");
        break;

    case DMC_MSG_RT_RUN_MOVE_FLAGS_LOOP:
        print("flags", "loop");
        break;

    default:
        print("flags", "none");
        break;
    }

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_shoot_frame(DmcRtShootFrame *packet) {
    print(packet->header);
    print("frame", packet->frame);
    print("direction", packet->direction ? "forward" : "backward");
    print("exposure_time", packet->exposure_time);
    print("blur_percent", (uint32_t)packet->blur_percent / DMC_MSG_RT_SHOOT_FRAME_BLUR_PERCENT_SCALE);
    size_t length = packet->header.length - sizeof(packet->frame) 
                                          - sizeof(packet->direction) 
                                          - sizeof(packet->exposure_time)
                                          - sizeof(packet->blur_percent);
    size_t count = length / sizeof(ShootFrameMotorBlur);
    for (size_t i = 0; i < count; ++i) {
        ShootFrameMotorBlur blur = packet->motor_blur[i];
        print("blur.motor", (uint32_t)blur.motor);
        print("blur.position_A", blur.position_A);
        print("blur.position_B", blur.position_B);
    }

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_shoot_frame_2(DmcRtShootFrame2 *packet) {
    print(packet->header);
    print("frame", packet->frame);
    print("exposure_time", packet->exposure_time);
    print("open_angle", (uint32_t)packet->open_angle);
    print("close_angle", (uint32_t)packet->close_angle);
    size_t length = packet->header.length - sizeof(packet->frame)
                                          - sizeof(packet->exposure_time)
                                          - sizeof(packet->open_angle)
                                          - sizeof(packet->close_angle);
    size_t count = length / sizeof(ShootFrameMotorBlur);
    for (size_t i = 0; i < count; ++i) {
        ShootFrameMotorBlur blur = packet->motor_blur[i];
        print("blur.motor", (uint32_t)blur.motor);
        print("blur.position_A", blur.position_A);
        print("blur.position_B", blur.position_B);
    }

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_go(DmcRtGo *packet) {
    print(packet->header);
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_end(DmcRtEnd *packet) {
    print(packet->header);
    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_jog_all(DmcRtJogAll *packet) {
    print(packet->header);
    print("fps", packet->fps);
    print("destination", packet->destination);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_rt_stop_loop(DmcRtStopLoop *packet) {
    print(packet->header);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_config(DmcVirtConfig *packet) {
    print(packet->header);
    switch(packet->type) {
    case DMC_VIRT_TYPE_BOOM_SWING_TRACK: {
        DmcVirtConfigBoomSwingTrack *bst = reinterpret_cast<DmcVirtConfigBoomSwingTrack *>(packet);
        print("type", "boom-swing-track");
        print("boom_motor", bst->boom_motor);
        print("boom_spu", bst->boom_spu);
        print("boom_position", (float)bst->boom_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("swing_motor", bst->swing_motor);
        print("swing_spu", bst->swing_spu);
        print("swing_position", (float)bst->swing_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("track_motor", bst->track_motor);
        print("track_spu", bst->track_spu);
        print("track_position", (float)bst->track_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("pan_motor", bst->pan_motor);
        print("pan_spu", bst->pan_spu);
        print("pan_position", (float)bst->pan_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("tilt_motor", bst->tilt_motor);
        print("tilt_spu", bst->tilt_spu);
        print("tilt_position", (float)bst->tilt_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("roll_motor", bst->roll_motor);
        print("roll_spu", bst->roll_spu);
        print("roll_position", (float)bst->roll_position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("boom_length", (float)bst->boom_length / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        print("boom_extension", (float)bst->boom_extension / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        print("nodal_offset_x", (float)bst->nodal_offset_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        print("nodal_offset_y", (float)bst->nodal_offset_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        print("nodal_offset_z", (float)bst->nodal_offset_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        if (bst->have_compensation()) {
            for (size_t i = 0; i < DMC_BOOM_COMPENSATION_ANGLES; ++i) {
                print("boom_compensation", i, (float)bst->boom_compensation[i]);
            }
            print("safe_distance", (float)bst->safe_distance / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        }
        break;
    }

    case DMC_VIRT_TYPE_SWING_PAN: {
        DmcVirtConfigSwingPan *sp = reinterpret_cast<DmcVirtConfigSwingPan *>(packet);
        print("type", "swing-pan");
        print("swing_motor", sp->swing_motor);
        print("swing_spu", sp->swing_spu);
        print("pan_motor", sp->pan_motor);
        print("pan_spu", sp->pan_spu);
        break;
    }

    default: 
        print("type", "unsupported");
        break;
    }

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_move(DmcVirtMove *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("position", (float)packet->position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_stop(DmcVirtStop *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_jog(DmcVirtJog *packet) {
    print(packet->header);
    print("motor", (uint32_t)packet->motor);
    print("speed", (uint32_t)packet->speed);
    print("destination", (int32_t)packet->destination);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_jog_on_line(DmcVirtJogOnLine *packet) {
    print(packet->header);
    switch (packet->axis) {
    case DMC_MSG_VIRT_JOG_ON_LINE_X:
        print("axis", "x");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_Y:
        print("axis", "y");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_Z:
        print("axis", "z (camera)");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_PAN:
        print("axis", "pan");
        break;

    case DMC_MSG_VIRT_JOG_ON_LINE_TILT:
        print("axis", "tilt");
        break;

    default:
        print("axis", "unknown");
        break;
    }
    print("speed", (uint32_t)packet->speed);

    ack(packet->header, DMC_ACK_OK);
}

void DmcDebug::on_virt_get_position(DmcAck *packet) {
    print(packet->header);

    if (packet->header.length > DMC_MSG_DATA_LENGTH(DmcAck)) {
        DmcVirtGetPosition *packet = reinterpret_cast<DmcVirtGetPosition *>(packet);
        print("track", packet->track / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("EW", packet->EW / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("NS", packet->NS / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("pan", packet->pan / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("tilt", packet->tilt / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        print("roll", packet->roll / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
        if (packet->have_aim_point()) {
            print("aim_point", "enabled");
            print("aim_x", packet->aim_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
            print("aim_y", packet->aim_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
            print("aim_z", packet->aim_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
        } else {
            print("aim_point", "disabled");
        }
    }

    enqueue(&stub_virt_get_position, sizeof(DmcVirtGetPosition));
}

void DmcDebug::on_virt_aim_point(DmcVirtAimPoint *packet) {
    print(packet->header);
    print("aim_point", packet->enable ? "enabled" : "disabled");
    print("aim_x", packet->aim_x / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    print("aim_y", packet->aim_y / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);
    print("aim_z", packet->aim_z / DMC_MSG_VIRT_CONFIG_LENGTH_SCALE);

    enqueue(packet, sizeof(*packet));
}

void DmcDebug::on_unknown(DmcHeader *packet) {
    debug_pulse(PIN_DBG_0);
    print(*packet);
    
    ack(*packet, DMC_ACK_ERR_GENERAL);
}

DmcPrint::DmcPrint()
    : stream(nullptr)
    , tx_buffer()
    , tx_head(0)
    , tx_tail(0)
{
    memset(tx_buffer, 0, sizeof(ex_buffer));
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

void DmcPrint::print(DmcHeader *header) {
    enqueue("marker", header.marker);
    enqueue("id", header.id);
    switch (header.type) {
    case DMC_MSG_HI:                      enqueue("type", "hi"); break;
    case DMC_MSG_DMX:                     enqueue(type", "dmx"); break;
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
    case DMC_MSG_RT_POSITION_FRAME:       eneueue("type", "rt_position_frame"); break;
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
    enqueue("length", (uint32_t)header.length);
}

void DmcPrint::print(DmcAck *packet) {
    enqueue(&ack->header);
    switch (ack->response_code) {
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

void DmcPrint::print(DmcHi *packet) {
    enqueue(&packet->header);
}

void DmcPrint::print(DmcDevice *packet) {
   enqueue(&packet->header);
   enqueue("name",    packet->name));
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

void DmcPrint::print(DmcDmx *packet) {
    enqueue(&packet->header);
    enqueue("ramp", (uint32_t)packet->ramp);
    enqueue("start_channel", (uint32_t)packet->start_channel);
    size_t length = packet->header.length - sizeof(byte) - sizeof(word);
    for (size_t i = 0; i < length; ++i) {
        enqueue("light_value", i, (uint32_t)packet->light_values[i]);
    }

}

void DmcPrint::print(DmcGioOut *packet) {
    enqueue(&packet->header);
    enqueue("triggers", packet->triggers);
}

void DmcPrint::print(DmcGioIn *packet) {
    enqueue(&packet->header);
    enqueue("response_code", (uint32_t)packet->response_code);
    enqueue("triggers", packet->triggers);
}

void DmcPrint::print(DmcGioCam *packet) {
    enqueue(&packet->header);
    enqueue("triggers.cam_shutter", (packet->triggers & DMC_GIO_CAM_SHUTTER_FLAG) ? "enable" : "disable"); 
    enqueue("triggers.cam_meter", (packet->triggers & DMC_GIO_CAM_METER_FLAG) ? "enable" : "disable");
}

void DmcPrint::print(DmcMotorStatus *packet) {
    enqueue(&packet->header);
    enqueue("motor_status", packet->motor_status ? "moving" : "stopped");
    enqueue("dmx_status", packet->dmx_status ? "adjusting" : "stopped");
}

void DmcPrint::print(DmcMotorMove *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("position", packet->position);
}

void DmcPrint::print(DmcMotorMoveResponse *packet) {
    enqueue(&packet->header);
    enqueue("motor_status", packet->motor_status ? "moving" : "stopped");
}

void DmcPrint::print(DmcMotorStop *packet) {
    enqueue(&packet->header);
    enqueue("motor", packet->motor);
}

void DmcPrint::print(DmcMotorStopAll *packet) {
    enqueue(&packet->header);
    enqueue("flags", packet->flags ? "silent" : "warning");
}

void DmcPrint::print(DmcMotorGetPosition *packet) {
    enqueue(&packet->header);
    enqueue("move_time", packet->move_time);
    for (uint8_t index; index < LIMNMOCO_MOTOR_COUNT; ++index) {
        enqueue("motor_positions", index, packet->motor_positions[index]);
    }
}

void DmcPrint::print(DmcMotorResetPosition *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("position", packet->position);
}

void DmcPrint::print(DmcMotorJog *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("speed", (uint32_t)packet->speed);
    enqueue("destination", (int32_t)packet->destination);
}

void DmcPrint::print(DmcMotorConfigure *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("flags.config", (packet->flags & DMC_MOTOR_CONFIG_ENABLED) ? "true" : "false");
    enqueue("flags.blur",   (packet->flags & DMC_MOTOR_CONFIG_BLUR) ? "true" : "false");
    enqueue("flags.virt",   (packet->flags & DMC_MOTOR_CONFIG_VIRT) ? "true" : "false");
    enqueue("flags.live_control", (packet->flags & DMC_MOTOR_CONFIG_LIVE_CONTROL) ? "true" : "false");
    enqueue("flags.couple", (packet->flags & DMC_MOTOR_CONFIG_COUPLE) ? "true" : "false");
    enqueue("flags.couple_r", (packet->flags & DMC_MOTOR_CONFIG_COUPLE_R) ? "true" : "false");
}

void DmcPrint::print(DmcMotorSetSpeed *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("max_velocity (steps/second)", packet->max_velocity);
    enqueue("max_acceleration (steps/second/second)", packet->max_acceleration);
}

void DmcPrint::print(DmcMotorSetLimits *packet) {
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

void DmcPrint::print(DmcMotorHardStop *packet) {
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

void DmcPrint::print(DmcRtUploadMoveBegin *packet) {
    enqueue(&packet->header);
    enqueue("start_frame", packet->start_frame);
    enqueue("end_frame", packet->end_frame);
}

void DmcPrint::print(DmcRtUploadMoveAxis *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("start_index", packet->start_index);
    size_t length = (packet->header.length - sizeof(packet->motor) - sizeof(packet->start_index));
    size_t count  = length / sizeof(packet->positions[0]);
    for (size_t i = 0; i < count; ++i) {
        enqueue("positions", i, packet->positions[i]);
    }
}

void DmcPrint::print(DmcRtUploadMoveDmx *packet) {
    enqueue(packet->header);
    enqueue("channel", (uint32_t)packet->channel);
    enqueue("start_index", packet->start_index);
    size_t length = packet->header.length - sizeof(packet->channel) - sizeof(packet->start_index);
    size_t count  = length / sizeof(packet->light_levels[0]);
    for (size_t i = 0; i < count; ++i) {
        enqueue("light_levels", i, (uint32_t)packet->light_levels[i]);
    }
}

void DmcPrint::print(DmcRtUploadMoveTriggers *packet) {
    enqueue(packet->header);
    enqueue("mask", (uint32_t)packet->mask);
    size_t length = packet->header.length - sizeof(packet->mask);
    size_t count  = length / sizeof(packet->move_frame_values[0]);
    for (size_t i = 0; i < count; ++i) {
        MoveFrameValue mfv = packet->move_frame_values[i];
        enqueue("frame", i, mfv.frame);
        enqueue("values", i, mfv.values);
    }
 
}

void DmcPrint::print(DmcRtUploadMoveEnd *packet) {
    enqueue(&packet->header);
}

void DmcPrint::print(DmcRtPositionFrame *packet) {
    enqueue(packet->header);
    enqueue("frame", packet->frame);
}

void DmcPrint::print(DmcRtRunMove *packet) {
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

void DmcPrint::print(DmcShootFrame *packet) {
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

void DmcPrint::print(DmcShootFrame2 *packet) {
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

void DmcPrint::print(DmcRtGo *packet) {
    enqueue(&packet->header);
}

void DmcPrint::print(DmcRtEnd *packet) {
    enqueue(&packet->header);
}

void DmcPrint::print(DmcRtJogAll *packet) {
    enqueue(&packet->header);
    enqueue("fps", packet->fps);
    enqueue("destination", packet->destination);
}

void DmcPrint::print(DmcRtStopLoop *packet) {
    enqueue(&packet->header);
}

void DmcPrint::print(DmcVirtConfig *packet) {
    enqueue(&packet->header);
    switch (packet->type) {
    case DMC_VIRT_TYPE_BOOM_SWING_TRACK:
        print(reinterpret_cast<DmcVirtConfigBoomSwingTrack *>(packet));
        break;
    case DMC_VIRT_TYPE_SWING_PAN:
        print(reinterpret_cast<DmcVirtConfigSwingPan *>(packet));
        break;
    default:
        print("type", "unsupported");
        break;
    }
}

void DmcPrint::print(DmcVirtConfigBoomSwingTrack *packet) {
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

void DmcPrint::print(DmcVirtConfigSwingPan *packet) {
    enqueue("type", "swing-pan");
    enqueue("swing_motor", packet->swing_motor);
    enqueue("swing_spu",   packet->swing_spu);
    enqueue("pan_motor",   packet->pan_motor);
    enqueue("pan_spu",     packet->pan_spu);
}

void DmcPrint::print(DmcVirtMove *packet) {
    enqueue(&packet->header);
    enqueue("motor",    (uint32_t)packet->motor);
    enqueue("position", (float)packet->position / DMC_MSG_VIRT_CONFIG_POSITION_SCALE);
}

void DmcPrint::print(DmcVirtStop *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
}

void DmcPrint::print(DmcVirtJog *packet) {
    enqueue(&packet->header);
    enqueue("motor", (uint32_t)packet->motor);
    enqueue("speed", (uint32_t)packet->speed);
    enqueue("destination", (int32_t)packet->destination);
}

void DmcPrint::print(DmcVirtJogOnLine *packet) {
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

void DmcPrint::print(DmcVirtGetPosition *packet) {
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

void DmcPrint::print(DmcVirtAimPoint *packet) {
    enqueue(packet->header);
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

void DmcPrint::enqueue(uint8_t *data, uint32_t length) {
    for (uint32_t index = 0; index < length; ++index) {
        enqueue(data[index]);
    }
}

void DmcPrint::enqueue(uint32_t data) {
    uint32_t length = strlenu(data);
    uint8_t  buffer[length];

    convu(buffer, length, value, 10);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(int32_t data) {
    uint32_t length = strleni(data);
    uint8_t  buffer[length];

    convi(buffer, length, value, 10);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(float data) {
    uint32_t length = strlenf(data);
    uint8_t  buffer[length];

    convf(buffer, length, value);
    enqueue(buffer, length);
}

void DmcPrint::enqueue(char const *data) {
    for (uint32_t index = 0; data[index] && index < UINT32_MAX; ++index) {
        enqueue(data[index]);
    }
}

void DmcPrint::enqueue(char const *field, uint32_t data) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, int32_t data) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, float data) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}

void DmcPrint::enqueue(char const *field, uint8_t major, uint8_t minor, uint8_ rev) {
    enqueue(version);
    enqueue(" = ");
    enqueue(major);
    enqueue('.');
    enqueue(minor);
    enqueue('.');
    enqueue(rev);
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

void DmcPrint::enqueue(char const *field, char const *value) {
    enqueue(field);
    enqueue(" = ");
    enqueue(value);
    enqueue('\n');
}





