// SPDX-License-Identifier: BSD-3-Clause

#include "KuperTrackConvention.h"

namespace limnmoco {

float kuper_track_to_solver(float track) {
  return -track;
}

float solver_track_to_kuper(float track) {
  return -track;
}

VirtualPose kuper_pose_to_solver(const VirtualPose &pose) {
  return VirtualPose{
      kuper_track_to_solver(pose.vtrack), pose.vew, pose.vheight,
      pose.vpanDeg, pose.vtiltDeg, pose.vrollDeg};
}

VirtualPose solver_pose_to_kuper(const VirtualPose &pose) {
  return VirtualPose{
      solver_track_to_kuper(pose.vtrack), pose.vew, pose.vheight,
      pose.vpanDeg, pose.vtiltDeg, pose.vrollDeg};
}

} // namespace limnmoco
