// SPDX-License-Identifier: BSD-3-Clause


#ifndef LIMNMOCO_DMC_DEBUG_HPP
#define LIMNMOCO_DMC_DEBUG_HPP

#include "dmc.hpp"

class DmcDebug : public DmcStream {
public:
    DmcDebug();

    void bindDebug(Stream &stream);
    void transmitDebug();

private:
    void enqueueDebug(uint8_t data);
    void enqueueDebug(uint8_t *str, size_t length);

    void print(uint8_t value);
    void print(uint32_t value);
    void print(int32_t value);
    void print(float value);
    void print(char const *cstr);
    void print(char c);

    void print(char const *field, uint32_t value);
    void print(char const *field, int32_t value);
    void print(char const *field, float value);
    void print(uint32_t major, uint32_t minor, uint32_t rev);
    void print(char const *field, uint32_t index, uint32_t value);
    void print(char const *field, uint32_t index, int32_t value);
    void print(char const *field, uint32_t index, float value);
    void print(char const *field, uint8_t *const value, size_t length);
    void print(char const *field, char const *str);
    void print(DmcHeader &header);

    void on_ack(DmcAck *packet) override;
    void on_hi(DmcHi *packet) override;
    void on_dmx(DmcDmx *packet) override;
    void on_gio_out(DmcGioOut *packet) override;
    void on_gio_in(DmcAck *packet) override;
    void on_gio_cam(DmcGioCam *packet) override;
    void on_motor_status(DmcAck *packet) override;
    void on_motor_move(DmcMotorMove *packet) override;
    void on_motor_stop(DmcMotorStop *packet) override;
    void on_motor_stop_all(DmcMotorStopAll *packet) override;
    void on_motor_get_position(DmcAck *packet) override;
    void on_motor_reset_position(DmcMotorResetPosition *packet) override;
    void on_motor_jog(DmcMotorJog *packet) override;
    void on_motor_configure(DmcMotorConfigure *packet) override;
    void on_motor_set_speed(DmcMotorSetSpeed *packet) override;
    void on_motor_set_limits(DmcMotorSetLimits *packet) override;
    void on_motor_hard_stop(DmcAck *packet) override;
    void on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet) override;
    void on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet) override;
    void on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet) override;
    void on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet) override;
    void on_rt_upload_move_end(DmcRtUploadMoveEnd *packet) override;
    void on_rt_position_frame(DmcRtPositionFrame *packet) override;
    void on_rt_run_move(DmcRtRunMove *packet) override;
    void on_rt_shoot_frame(DmcRtShootFrame *packet) override;
    void on_rt_shoot_frame_2(DmcRtShootFrame2 *packet) override;
    void on_rt_go(DmcRtGo *packet) override;
    void on_rt_end(DmcRtEnd *packet) override;
    void on_rt_jog_all(DmcRtJogAll *packet) override;
    void on_rt_stop_loop(DmcRtStopLoop *packet) override;
    void on_virt_config(DmcVirtConfig *packet) override;
    void on_virt_move(DmcVirtMove *packet) override;
    void on_virt_stop(DmcVirtStop *packet) override;
    void on_virt_jog(DmcVirtJog *packet) override;
    void on_virt_get_position(DmcAck *packet) override;
    void on_virt_jog_on_line(DmcVirtJogOnLine *packet) override;
    void on_virt_aim_point(DmcVirtAimPoint *packet) override;
    void on_unknown(DmcHeader *packet) override;

    Stream              *debug_stream;
    DmcGioIn             stub_gio_in;
    DmcMotorStatus       stub_motor_status;
    DmcMotorMoveResponse stub_motor_move_response;
    int32_t              stub_motor_positions[MOTOR_COUNT];
    DmcMotorGetPosition  stub_motor_get_position;
    DmcMotorHardStop     stub_motor_hard_stop;
    DmcVirtGetPosition   stub_virt_get_position;

    static const size_t tx_length = 1024;
    uint8_t             tx_buffer[tx_length];
    size_t              tx_head;
    size_t              tx_tail;
};

#endif // !LIMNMOCO_DMC_DEBUG_HPP

