// SPDX-License-Identifier: BSD-3-Clause

#include "dmc.hpp"

#define SERIAL_BAUD 115200

// const char *due_hex_map = "0123456789ABCDEF";

// char due_hex(int8_t value) {
//     if (value < 0)  { return '\n'; }
//     if (value < 16) { return due_hex_map[value]; }
//     return '\n';
// }

alignas(4) static uint8_t tx_buffer[DMC_MSG_MAX_LENGTH];
const size_t tx_buffer_length = DMC_MSG_MAX_LENGTH;

alignas(4) static uint8_t rx_buffer[DMC_MSG_MAX_LENGTH];
const size_t rx_buffer_length = DMC_MSG_MAX_LENGTH;

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial1.begin(SERIAL_BAUD);
}

void loop() {
    int incoming = Serial1.available();

    if (incoming >= sizeof(uint32_t)) {
        uint32_t command;
        Serial1.readBytes((uint8_t *)(&command), sizeof(uint32_t));
        write_message_type(command);
    }
}

void on_ack(DmcAck *ack) {
    Serial.println("command:ack");
    Serial.println()
}

void on_hi(DmcHi *hi) {
    Serial.println("command.hi");
}

void on_dmx(DmcDmx *dmx);
void on_gio_out(DmcGioOut *gio_out);
void on_gio_in(DmcGioIn *gio_in);
void on_gio_cam(DmcGioCam *gio_cam);
void on_motor_status(DmcMotorStatus *motor_status);
void on_motor_move(DmcMotorMove *motor_move);
void on_motor_stop(DmcMotorStop *motor_stop);
void on_motor_stop_all(DmcMotorStopAll *motor_stop_all);
void on_motor_get_position(DmcMotorGetPosition *motor_get_position);
void on_motor_reset_position(DmcMotorResetPosition *motor_reset_position);
void on_motor_jog(DmcMotorJog *motor_jog);
void on_motor_configure(DmcMotorConfigure *packet);
void on_motor_set_speed(DmcMotorSetSpeed *packet);
void on_motor_set_limits(DmcMotorSetLimits *packet);
void on_motor_hard_stop(DmcMotorHardStop *packet);
void on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet);
void on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet);
void on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet);
void on_rt_upload_move_end(DmcRtUploadMoveEnd *packet);
void on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet);
void on_rt_position_frame(DmcRtPositionFrame *packet);
void on_rt_run_move(DmcRtRunMove *packet);
void on_rt_shoot_frame(DmcRtShootFrame *packet);
void on_rt_shoot_frame_2(DmcRtShootFrame2 *packet);
void on_rt_go(DmcRtGo *packet);
void on_rt_end(DmcRtEnd *packet);
void on_rt_stop_loop(DmcRtStopLoop *packet);
void on_rt_jog_all(DmcRtJogAll *packet);
void on_virt_config(DmcVirtConfig *packet);
void on_virt_move(DmcVirtMove *packet);
void on_virt_stop(DmcVirtStop *packet);
void on_virt_jog(DmcVirtJog *packet);
void on_virt_get_position(DmcVirtGetPosition *packet);
void on_virt_jog_on_line(DmcVirtJogOnLine *packet);
void on_virt_aim_point(DmcVirtAimPoint *packet);
void on_unknown(DmcHeader *packet);

void write_message_type(uint32_t command) {
    switch (command) {
    case DMC_MSG_HI:                      Serial.println("command.hi"); break;
    case DMC_MSG_DMX:                     Serial.println("command.dmx"); break;
    case DMC_MSG_GIO_OUT:                 Serial.println("command.gio_out"); break;
    case DMC_MSG_GIO_IN:                  Serial.println("command.gio_in"); break;
    case DMC_MSG_GIO_CAM:                 Serial.println("command.gio_cam"); break;
    case DMC_MSG_MOTOR_STATUS:            Serial.println("command.motor_status"); break;
    case DMC_MSG_MOTOR_MOVE:              Serial.println("command.motor_move"); break;
    case DMC_MSG_MOTOR_STOP:              Serial.println("command.motor_stop"); break;
    case DMC_MSG_MOTOR_STOP_ALL:          Serial.println("command.motor_stop_all"); break;
    case DMC_MSG_MOTOR_GET_POSITION:      Serial.println("command.motor_get_position"); break;
    case DMC_MSG_MOTOR_RESET_POSITION:    Serial.println("command.motor_reset_position"); break;
    case DMC_MSG_MOTOR_JOG:               Serial.println("command.motor_jog"); break;
    case DMC_MSG_MOTOR_CONFIGURE:         Serial.println("command.motor_configure"); break;
    case DMC_MSG_MOTOR_SET_SPEED:         Serial.println("command.motor_set_speed"); break;
    case DMC_MSG_MOTOR_SET_LIMITS:        Serial.println("command.motor_set_limits"); break;
    case DMC_MSG_MOTOR_HARD_STOP:         Serial.println("command.motor_hard_stop"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_BEGIN:    Serial.println("command.rt_upload_move_begin"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_AXIS:     Serial.println("command.rt_upload_move_axis"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_DMX:      Serial.println("command.rt_upload_move_dmx"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_END:      Serial.println("command.rt_upload_move_end"); break;
    case DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS: Serial.println("command.rt_upload_move_triggers"); break;
    case DMC_MSG_RT_POSITION_FRAME:       Serial.println("command.rt_position_frame"); break;
    case DMC_MSG_RT_RUN_MOVE:             Serial.println("command.rt_run_move"); break;
    case DMC_MSG_RT_SHOOT_FRAME:          Serial.println("command.rt_shoot_frame"); break;
    case DMC_MSG_RT_SHOOT_FRAME2:         Serial.println("command.rt_shoot_frame_2"); break;
    case DMC_MSG_RT_GO:                   Serial.println("command.rt_go"); break;
    case DMC_MSG_RT_END:                  Serial.println("command.rt_end"); break;
    case DMC_MSG_RT_STOP_LOOP:            Serial.println("command.rt_stop_loop"); break;
    case DMC_MSG_RT_JOG_ALL:              Serial.println("command.rt_jog_all"); break;
    case DMC_MSG_VIRT_CONFIG:             Serial.println("command.virt_config"); break;
    case DMC_MSG_VIRT_MOVE:               Serial.println("command.virt_move"); break;
    case DMC_MSG_VIRT_STOP:               Serial.println("command.virt_stop"); break;
    case DMC_MSG_VIRT_JOG:                Serial.println("command.virt_jog"); break;
    case DMC_MSG_VIRT_GET_POSITION:       Serial.println("command.virt_get_position"); break;
    case DMC_MSG_VIRT_JOG_ON_LINE:        Serial.println("command.jog_on_line"); break;
    case DMC_MSG_VIRT_AIM_POINT:          Serial.println("command.virt_aim_point"); break;
    default:                              Serial.println("command.unknown"); break;
    }
}
