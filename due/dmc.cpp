// SPDX-License-Identifier: BSD-3-Clause

#include "dmc.hpp"

uint16_t dmc_checksum(uint8_t *buffer, size_t length) {
    uint16_t sum1 = 0, sum2 = 0;

    while (length) {
        size_t tlen = ((length >= 20) ? 20 : length);
        length -= tlen;
        do {
            sum2 += sum1 += *buffer++;
            tlen--;
        } while (tlen);
        sum1 %= 0xff;
        sum2 %= 0xff;
    }
    return (sum2 << 8) | sum1;
}

word dmc_checkbytes(uint16_t checksum) {
    byte c0, c1, f0, f1;
    f0 = checksum & 0xFF;
    f1 = (checksum >> 8) & 0xFF;
    c0 = 0xFF - ((f0 + f1) % 0xFF);
    c1 = 0xFF - ((f0 + c0) % 0xFF);
    return (c1 << 8) | c0;
}

void dmc_packet_switch(uint8_t *buffer, size_t length) {
    DmcHeader *header = reinterpret_cast<DmcHeader *>(buffer);

    switch (header->type) {
        case DMC_MSG_HI:
            dmc_on_ack(reinterpret_cast<DmcAck *>(buffer));
            break;

        case DMC_MSG_DMX:
            dmc_on_dmx(reinterpret_cast<DmcDmx *>(buffer));
            break;

        case DMC_MSG_GIO_OUT:
            dmc_on_gio_out(reinterpret_cast<DmcGioOut *>(buffer));
            break;

        case DMC_MSG_GIO_IN:
            dmc_on_gio_in(reinterpret_cast<DmcGioIn *>(buffer));
            break;

        case DMC_MSG_GIO_CAM:
            dmc_on_gio_cam(reinterpret_cast<DmcGioCam *>(buffer));
            break;

        case DMC_MSG_MOTOR_STATUS:
            dmc_on_motor_status(reinterpret_cast<DmcMotorStatus *>(buffer));
            break;

        case DMC_MSG_MOTOR_MOVE:
            dmc_on_motor_move(reinterpret_cast<DmcMotorMove *>(buffer));
            break;

        case DMC_MSG_MOTOR_STOP:
            dmc_on_motor_stop(reinterpret_cast<DmcMotorStop *>(buffer));
            break;

        case DMC_MSG_MOTOR_STOP_ALL:
            dmc_on_motor_stop_all(reinterpret_cast<DmcMotorStopAll *>(buffer));
            break;

        case DMC_MSG_MOTOR_GET_POSITION:
            dmc_on_motor_get_position(reinterpret_cast<DmcMotorGetPosition *>(buffer));
            break;

        case DMC_MSG_MOTOR_RESET_POSITION:
            dmc_on_motor_reset_position(reinterpret_cast<DmcMotorResetPosition *>(buffer));
            break;

        case DMC_MSG_MOTOR_JOG:
            dmc_on_motor_jog(reinterpret_cast<DmcMotorJog *>(buffer));
            break;

        case DMC_MSG_MOTOR_CONFIGURE:
            dmc_on_motor_configure(reinterpret_cast<DmcMotorConfigure *>(buffer));
            break;

        case DMC_MSG_MOTOR_SET_SPEED:
            dmc_on_motor_set_speed(reinterpret_cast<DmcMotorSetSpeed *>(buffer));
            break;

        case DMC_MSG_MOTOR_SET_LIMITS:
            dmc_on_motor_set_limits(reinterpret_cast<DmcMotorSetLimits *>(buffer));
            break;

        case DMC_MSG_MOTOR_HARD_STOP:
            dmc_on_motor_hard_stop(reinterpret_cast<DmcMotorHardStop *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_BEGIN:
            dmc_on_rt_upload_move_begin(reinterpret_cast<DmcRtUploadMoveBegin *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_AXIS:
            dmc_on_rt_upload_move_axis(reinterpret_cast<DmcRtUploadMoveAxis *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_DMX:
            dmc_on_rt_upload_move_dmx(reinterpret_cast<DmcRtUploadMoveDmx *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_END:
            dmc_on_rt_upload_move_end(reinterpret_cast<DmcRtUploadMoveEnd *>(buffer));
            break;

        case DMC_MSG_RT_UPLOAD_MOVE_TRIGGERS:
            dmc_on_rt_upload_move_triggers(reinterpret_cast<DmcRtUploadMoveTriggers *>(buffer));
            break;

        case DMC_MSG_RT_POSITION_FRAME:
            dmc_on_rt_position_frame(reinterpret_cast<DmcRtPositionFrame *>(buffer));
            break;

        case DMC_MSG_RT_RUN_MOVE:
            dmc_on_rt_run_move(reinterpret_cast<DmcRtRunMove *>(buffer));
            break;

        case DMC_MSG_RT_SHOOT_FRAME:
            dmc_on_rt_shoot_frame(reinterpret_cast<DmcRtShootFrame *>(buffer));
            break;

        case DMC_MSG_RT_SHOOT_FRAME2:
            dmc_on_rt_shoot_frame_2(reinterpret_cast<DmcRtShootFrame2 *>(buffer));
            break;

        case DMC_MSG_RT_GO:
            dmc_on_rt_go(reinterpret_cast<DmcRtGo *>(buffer));
            break;

        case DMC_MSG_RT_END:
            dmc_on_rt_end(reinterpret_cast<DmcRtEnd *>(buffer));
            break;

        case DMC_MSG_RT_STOP_LOOP:
            dmc_on_rt_stop_loop(reinterpret_cast<DmcRtStopLoop *>(buffer));
            break;

        case DMC_MSG_RT_JOG_ALL:
            dmc_on_rt_jog_all(reinterpret_cast<DmcRtJogAll *>(buffer));
            break;

        case DMC_MSG_VIRT_CONFIG:
            dmc_on_virt_config(reinterpret_cast<DmcVirtConfig *>(buffer));
            break;

        case DMC_MSG_VIRT_MOVE:
            dmc_on_virt_move(reinterpret_cast<DmcVirtMove *>(buffer));
            break;

        case DMC_MSG_VIRT_STOP:
            dmc_on_virt_stop(reinterpret_cast<DmcVirtStop *>(buffer));
            break;

        case DMC_MSG_VIRT_JOG:
            dmc_on_virt_jog(reinterpret_cast<DmcVirtJog *>(buffer));
            break;

        case DMC_MSG_VIRT_GET_POSITION:
            dmc_on_virt_get_position(reinterpret_cast<DmcVirtGetPosition *>(buffer));
            break;

        case DMC_MSG_VIRT_JOG_ON_LINE:
            dmc_on_virt_jog_on_line(reinterpret_cast<DmcVirtJogOnLine *>(buffer));
            break;

        case DMC_MSG_VIRT_AIM_POINT:
            dmc_on_virt_aim_point(reinterpret_cast<DmcVirtAimPoint *>(buffer));
            break;

        default:
            dmc_on_unknown(header);
            break;
    }
}
