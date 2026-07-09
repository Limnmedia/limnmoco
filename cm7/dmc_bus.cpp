// SPDX-License-Identifier: GPL-3.0-or-later

#include "dmc_bus.hpp"

DmcBus dmc_bus;

#ifdef LIMNMOCO_DEBUG
void DmcBus::bindPrint(Stream &stream) {
  print.bind(stream);
}

void DmcBus::transmitPrint() {
  print.transmit();
}
#endif // !LIMNMOCO_DEBUG

void DmcBus::update() {
  receive();
  transmit();

#ifdef LIMNMOCO_DEBUG
  transmitPrint();
#endif
}

void DmcBus::on_ack(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
// #NOTE: I don't expect DragonFrame to ever send a DmcAck packet directly
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_hi(DmcHi *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  DmcStream::on_hi(packet);
}

void DmcBus::on_dmx(DmcDmx *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_gio_out(DmcGioOut *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_gio_in(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG

  // #NOTE: DmcGioIn is only pushed to Dragonframe
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_gio_cam(DmcGioCam *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG

  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_motor_status(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_move(DmcMotorMove *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_stop(DmcMotorStop *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_stop_all(DmcMotorStopAll *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_get_position(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_reset_position(DmcMotorResetPosition *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_jog(DmcMotorJog *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_configure(DmcMotorConfigure *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_set_speed(DmcMotorSetSpeed *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_set_limits(DmcMotorSetLimits *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG


}

void DmcBus::on_motor_hard_stop(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG

}

void DmcBus::on_rt_upload_move_begin(DmcRtUploadMoveBegin *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_upload_move_axis(DmcRtUploadMoveAxis *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_upload_move_dmx(DmcRtUploadMoveDmx *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_upload_move_triggers(DmcRtUploadMoveTriggers *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_upload_move_end(DmcRtUploadMoveEnd *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_position_frame(DmcRtPositionFrame *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_run_move(DmcRtRunMove *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_shoot_frame(DmcRtShootFrame *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_shoot_frame_2(DmcRtShootFrame2 *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_go(DmcRtGo *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_end(DmcRtEnd *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_jog_all(DmcRtJogAll *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_rt_stop_loop(DmcRtStopLoop *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_config(DmcVirtConfig *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_move(DmcVirtMove *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_stop(DmcVirtStop *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_jog(DmcVirtJog *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_get_position(DmcAck *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_jog_on_line(DmcVirtJogOnLine *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_virt_aim_point(DmcVirtAimPoint *packet) {
#ifdef LIMNMOCO_DEBUG
  print(packet);
#endif // !LIMNMOCO_DEBUG
  ack(packet->header, DMC_ACK_ERR_UNSUPPORTED);
}

void DmcBus::on_unknown(DmcHeader *header) {
#ifdef LIMNMOCO_DEBUG
  print(header);
#endif // !LIMNMOCO_DEBUG
  ack(*header, DMC_ACK_ERR_UNSUPPORTED);
}







