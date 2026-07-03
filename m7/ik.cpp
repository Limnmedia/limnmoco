// SPDX-License-Identifier: BSD-3-Clause

#include "ik.h"

#include <algorithm>
#include <cmath>

#include "mat3.h"

namespace {

constexpr float kPi = 3.14159265f;

float clamp(float value, float lo, float hi) {
  return std::max(lo, std::min(hi, value));
}

Mat3 rotationMatrixFromVirtualAxes(float vpanDeg, float vtiltDeg, float vrollDeg) {
  return mat3_multiply(mat3_multiply(mat3_rotation_z(ik_radians(vpanDeg)),
                                     mat3_rotation_x(ik_radians(vtiltDeg))),
                       mat3_rotation_y(ik_radians(vrollDeg)));
}

} // namespace

float ik_degrees(float radiansValue) {
  return radiansValue * 180.0f / kPi;
}

float ik_radians(float degreesValue) {
  return degreesValue * kPi / 180.0f;
}

CraneSolveResult solve_ik(const VirtualPose &pose, const CraneGeometry &geometry) {
  CraneSolveResult result{};

  result.target = Vec3{
      pose.vew,
      pose.vtrack,
      pose.vheight,
  };

  const Mat3 rotation = rotationMatrixFromVirtualAxes(pose.vpanDeg, pose.vtiltDeg, pose.vrollDeg);
  const Vec3 offsetLocal = Vec3{
      geometry.offsetX,
      geometry.offsetY,
      geometry.offsetZ,
  };

  result.offsetWorld = mat3_multiply_v(rotation, offsetLocal);
  result.panTarget = vec3_sub(result.target, result.offsetWorld);

  const float boomLength = std::max(0.001f, geometry.boomLength);
  const float boomRawRatio = result.panTarget.z / boomLength;
  const float boomRatio = clamp(boomRawRatio, -1.0f, 1.0f);
  const float boomRad = std::asin(boomRatio);
  result.boomClamped = boomRawRatio != boomRatio;

  float horizontalReach = boomLength * std::cos(boomRad) + geometry.extensionLength;
  if (std::abs(horizontalReach) < 0.000001f) {
    horizontalReach = 0.000001f;
  }

  const float swingRawRatio = result.panTarget.x / horizontalReach;
  const float swingRatio = clamp(swingRawRatio, -1.0f, 1.0f);
  const float swingRad = std::asin(swingRatio);
  result.swingClamped = swingRawRatio != swingRatio;

  result.track = result.panTarget.y - horizontalReach * std::cos(swingRad);
  result.swingDeg = ik_degrees(swingRad);
  result.boomDeg = ik_degrees(boomRad);

  result.base = Vec3{0.0f, result.track, 0.0f};
  result.armTip = Vec3{
      boomLength * std::sin(swingRad) * std::cos(boomRad),
      result.track + boomLength * std::cos(swingRad) * std::cos(boomRad),
      boomLength * std::sin(boomRad),
  };

  const Vec3 levelExtensionVector = Vec3{
      geometry.extensionLength * std::sin(swingRad),
      geometry.extensionLength * std::cos(swingRad),
      0.0f,
  };

  result.panCenter = vec3_add(result.armTip, levelExtensionVector);
  result.nodal = vec3_add(result.panCenter, result.offsetWorld);
  result.error = vec3_sub(result.target, result.nodal);
  result.errorLength = vec3_length(result.error);

  return result;
}
