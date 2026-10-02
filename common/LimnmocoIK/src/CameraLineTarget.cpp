// SPDX-License-Identifier: BSD-3-Clause

#include "CameraLineTarget.h"

#include <cmath>

namespace limnmoco {

bool build_camera_line_target(const VirtualPose &currentPose,
                              const VirtualAimState &aimState,
                              const CameraLineOrientation &orientation,
                              CameraLineJogAxis axis, float signedDirection,
                              float translationDistance, float rotationStepDeg,
                              AimAwareTarget *output) {
  if (!output || !std::isfinite(signedDirection) || signedDirection == 0.0f ||
      !std::isfinite(translationDistance) || translationDistance <= 0.0f ||
      !std::isfinite(rotationStepDeg) || rotationStepDeg <= 0.0f) {
    return false;
  }

  AimAwareTarget target{currentPose, aimState.aimEnabled ? aimState.panOffsetDeg
                                                          : currentPose.vpanDeg,
                        aimState.aimEnabled ? aimState.tiltOffsetDeg
                                            : currentPose.vtiltDeg};
  const float direction = signedDirection > 0.0f ? 1.0f : -1.0f;
  if (axis == CameraLineJogAxis::kPan) {
    target.panOffsetDeg += direction * rotationStepDeg;
  } else if (axis == CameraLineJogAxis::kTilt) {
    target.tiltOffsetDeg += direction * rotationStepDeg;
  } else {
    CameraLineAxis translationAxis{};
    switch (axis) {
      case CameraLineJogAxis::kX: translationAxis = CameraLineAxis::kX; break;
      case CameraLineJogAxis::kY: translationAxis = CameraLineAxis::kY; break;
      case CameraLineJogAxis::kZ: translationAxis = CameraLineAxis::kZ; break;
      default: return false;
    }
    VirtualTranslationDelta delta{};
    if (!camera_line_direction(translationAxis, orientation.vpanDeg,
                               orientation.vtiltDeg, orientation.vrollDeg,
                               &delta, direction)) {
      return false;
    }
    target.pose.vtrack += delta.vtrack * translationDistance;
    target.pose.vew += delta.vew * translationDistance;
    target.pose.vheight += delta.vns * translationDistance;
  }

  if (!aimState.aimEnabled) {
    if (axis == CameraLineJogAxis::kPan) {
      target.pose.vpanDeg = target.panOffsetDeg;
    } else if (axis == CameraLineJogAxis::kTilt) {
      target.pose.vtiltDeg = target.tiltOffsetDeg;
    }
    *output = target;
    return true;
  }

  const KuperPoint nodal{target.pose.vew, target.pose.vheight,
                         target.pose.vtrack};
  AimOrientation base{};
  if (!aim_point_outside_safe_cylinder(nodal, aimState.aimPoint,
                                       aimState.safeDistance) ||
      !aim_base_orientation(nodal, aimState.aimPoint, target.pose.vrollDeg,
                            &base) ||
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
