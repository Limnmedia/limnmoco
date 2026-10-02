// SPDX-License-Identifier: BSD-3-Clause

#include "AimAwareTarget.h"

#include <cmath>

namespace limnmoco {
namespace {

bool finite_pose(const VirtualPose &pose) {
  return std::isfinite(pose.vtrack) && std::isfinite(pose.vew) &&
         std::isfinite(pose.vheight) && std::isfinite(pose.vpanDeg) &&
         std::isfinite(pose.vtiltDeg) && std::isfinite(pose.vrollDeg);
}

} // namespace

bool build_aim_aware_target(const VirtualPose &currentPose,
                            const VirtualAimState &aimState,
                            VirtualTargetAxis axis, float requestedValue,
                            AimAwareTarget *output) {
  if (!output || !finite_pose(currentPose) || !std::isfinite(requestedValue)) {
    return false;
  }

  AimAwareTarget target{currentPose, currentPose.vpanDeg, currentPose.vtiltDeg};
  if (!aimState.rollPresent) {
    target.pose.vrollDeg = 0.0f;
  }

  switch (axis) {
    case VirtualTargetAxis::kTrack:
      target.pose.vtrack = requestedValue;
      break;
    case VirtualTargetAxis::kEW:
      target.pose.vew = requestedValue;
      break;
    case VirtualTargetAxis::kNS:
      target.pose.vheight = requestedValue;
      break;
    case VirtualTargetAxis::kPan:
      if (aimState.aimEnabled) {
        target.panOffsetDeg = requestedValue;
      } else {
        target.pose.vpanDeg = requestedValue;
        target.panOffsetDeg = requestedValue;
      }
      break;
    case VirtualTargetAxis::kTilt:
      if (aimState.aimEnabled) {
        target.tiltOffsetDeg = requestedValue;
      } else {
        target.pose.vtiltDeg = requestedValue;
        target.tiltOffsetDeg = requestedValue;
      }
      break;
    case VirtualTargetAxis::kRoll:
      if (!aimState.rollPresent) {
        return false;
      }
      target.pose.vrollDeg = requestedValue;
      break;
    default:
      return false;
  }

  if (!aimState.aimEnabled) {
    *output = target;
    return true;
  }

  if (!std::isfinite(aimState.safeDistance) || aimState.safeDistance < 0.0f ||
      !std::isfinite(aimState.panOffsetDeg) ||
      !std::isfinite(aimState.tiltOffsetDeg)) {
    return false;
  }
  if (axis != VirtualTargetAxis::kPan) {
    target.panOffsetDeg = aimState.panOffsetDeg;
  }
  if (axis != VirtualTargetAxis::kTilt) {
    target.tiltOffsetDeg = aimState.tiltOffsetDeg;
  }

  const KuperPoint nodalPoint{
      target.pose.vew, target.pose.vheight, target.pose.vtrack};
  if (!aim_point_outside_safe_cylinder(nodalPoint, aimState.aimPoint,
                                       aimState.safeDistance)) {
    return false;
  }

  AimOrientation base{};
  if (!aim_base_orientation(nodalPoint, aimState.aimPoint,
                            target.pose.vrollDeg, &base) ||
      !aim_apply_relative_offsets(base, target.panOffsetDeg,
                                  target.tiltOffsetDeg, &base)) {
    return false;
  }
  target.pose.vpanDeg = base.vpanDeg;
  target.pose.vtiltDeg = base.vtiltDeg;
  target.pose.vrollDeg = base.vrollDeg;
  *output = target;
  return true;
}

} // namespace limnmoco
