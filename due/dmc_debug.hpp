// SPDX-License-Identifier: BSD-3-Clause


#ifndef LIMNMOCO_DMC_DEBUG_HPP
#define LIMNMOCO_DMC_DEBUG_HPP

#include "dmc.hpp"

class DmcDebug : public DmcStream {
public:
    DmcDebug();

    void bindDebug(Stream &stream);
    // #TODO: in order to support non-blocking printing 
    //   of the debug messages, do we want to write to 
    //   and internal buffer, and only write out bits of
    //   data at a time? I'm worried about blocking on 
    //   write for a long time, especially when writing 
    //   out large messages.
    //   It might be that while printing out the body of 
    //   rt_upload_move_triggers, the rx buffer fills up
    //   and we drop a packet.
    void transmitDebug();

private:
    void enqueueDebug(char const *str, size_t length);

    void print(uint32_t value);
    void print(int32_t value);
    void print(float value);
    void print(char const *cstr);
    void print(char c);

    void print(char const *field, uint32_t value);
    void print(char const *field, int32_t value);
    void print(char const *field, float value);
    void print(uint8_t major, uint8_t minor, uint8_t rev);
    void print(char const *field, size_t index, uint32_t value);
    void print(char const *field, uint8_t *const value, size_t length);
    void print(char const *field, char const *str);
    void print(DmcHeader &header);

    void on_ack(DmcAck *ack) override;
    void on_hi(DmcHi *hi) override;
    void on_dmx(DmcDmx *dmx) override;
    void on_gio_out(DmcGioOut *gio_out) override;
    void on_gio_in(DmcGioIn *gio_in) override;
    void on_gio_cam(DmcGioCam *gio_cam) override;
    void on_motor_status(DmcMotorStatus *motor_status) override;
    void on_motor_move(DmcMotorMove *motor_move) override;
    void on_motor_stop(DmcMotorStop *motor_stop) override;
    void on_motor_stop_all(DmcMotorStopAll *motor_stop_all) override;
    void on_motor_get_position(DmcMotorGetPosition *motor_get_position) override;
    void on_motor_reset_position(DmcMotorResetPosition *motor_reset_position) override;
    void on_motor_jog(DmcMotorJog *motor_jog) override;
    void on_motor_configure(DmcMotorConfigure *packet) override;
    void on_motor_set_speed(DmcMotorSetSpeed *packet) override;
    void on_motor_set_limits(DmcMotorSetLimits *packet) override;
    void on_motor_hard_stop(DmcMotorHardStop *packet) override;
    void on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet) override;
    void on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet) override;
    void on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet) override;
    void on_rt_upload_move_end(DmcRtUploadMoveEnd *packet) override;
    void on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet) override;
    void on_rt_position_frame(DmcRtPositionFrame *packet) override;
    void on_rt_run_move(DmcRtRunMove *packet) override;
    void on_rt_shoot_frame(DmcRtShootFrame *packet) override;
    void on_rt_shoot_frame_2(DmcRtShootFrame2 *packet) override;
    void on_rt_go(DmcRtGo *packet) override;
    void on_rt_end(DmcRtEnd *packet) override;
    void on_rt_stop_loop(DmcRtStopLoop *packet) override;
    void on_rt_jog_all(DmcRtJogAll *packet) override;
    void on_virt_config(DmcVirtConfig *packet) override;
    void on_virt_move(DmcVirtMove *packet) override;
    void on_virt_stop(DmcVirtStop *packet) override;
    void on_virt_jog(DmcVirtJog *packet) override;
    void on_virt_get_position(DmcVirtGetPosition *packet) override;
    void on_virt_jog_on_line(DmcVirtJogOnLine *packet) override;
    void on_virt_aim_point(DmcVirtAimPoint *packet) override;
    void on_unknown(DmcHeader *packet) override;

    Stream *debug_stream;
    DmcDevice stub_device;

    static const size_t tx_length = 1024;
    char tx_buffer[tx_length];
    size_t tx_head;
    size_t tx_tail;
};

#endif // !LIMNMOCO_DMC_DEBUG_HPP

