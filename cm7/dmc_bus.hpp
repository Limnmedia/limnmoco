// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_DMC_BUS_HPP
#define LIMNMOCO_CM7_DMC_BUS_HPP

#include "config.hpp"
#include "dmc.hpp"
#ifdef LIMNMOCO_DEBUG
#include "dmc_print.hpp"
#endif

class DmcBus : public DmcStream {
public:
  void update();

#ifdef LIMNMOCO_DEBUG
  void bindPrint(Stream &stream);
  void transmitPrint();
#endif // !LIMNMOCO_DEBUG

private:
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

#ifdef LIMNMOCO_DEBUG
  DmcPrint print;
#endif // !LIMNMOCO_DEBUG
};

extern DmcBus dmc_bus;

#endif // !LIMNMOCO_CM7_DMC_BUS_HPP

