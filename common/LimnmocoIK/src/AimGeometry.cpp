// SPDX-License-Identifier: BSD-3-Clause

#include "AimGeometry.h"

#include "KuperTrackConvention.h"
#include "LimnmocoIK.h"

#include <cmath>

namespace limnmoco {
namespace {

constexpr float kEpsilon = 1e-6f;

bool finite_point(const KuperPoint &point) {
  return std::isfinite(point.x) && std::isfinite(point.y) &&
         std::isfinite(point.z);
}

float normalize_pan_degrees(float degrees) {
  float normalized = std::fmod(degrees + 180.0f, 360.0f);
  if (normalized < 0.0f) {
    normalized += 360.0f;
  }
  return normalized - 180.0f;
}

} // namespace

bool aim_base_orientation(const KuperPoint &nodalPoint,
                          const KuperPoint &aimPoint, float vrollDeg,
                          AimOrientation *output) {
  if (!output || !finite_point(nodalPoint) || !finite_point(aimPoint) ||
      !std::isfinite(vrollDeg)) {
    return false;
  }

  // Convert the Kuper target direction into solver coordinates. The camera's
  // Kuper -Z optical-forward vector is solver +Y at the zero orientation.
  const float solverX = aimPoint.x - nodalPoint.x;
  const float solverY = kuper_track_to_solver(aimPoint.z - nodalPoint.z);
  const float solverZ = aimPoint.y - nodalPoint.y;
  const float horizontal = std::hypot(solverX, solverY);
  if (horizontal <= kEpsilon) {
    return false;
  }

  *output = AimOrientation{
      normalize_pan_degrees(angle_degrees(std::atan2(-solverX, solverY))),
      angle_degrees(std::atan2(solverZ, horizontal)),
      vrollDeg};
  return true;
}

bool aim_apply_relative_offsets(const AimOrientation &base, float panOffsetDeg,
                                float tiltOffsetDeg, AimOrientation *output) {
  if (!output || !std::isfinite(base.vpanDeg) ||
      !std::isfinite(base.vtiltDeg) || !std::isfinite(base.vrollDeg) ||
      !std::isfinite(panOffsetDeg) || !std::isfinite(tiltOffsetDeg)) {
    return false;
  }

  *output = AimOrientation{
      normalize_pan_degrees(base.vpanDeg + panOffsetDeg),
      base.vtiltDeg + tiltOffsetDeg,
      base.vrollDeg};
  return true;
}

bool aim_point_outside_safe_cylinder(const KuperPoint &nodalPoint,
                                    const KuperPoint &aimPoint, float radius) {
  if (!finite_point(nodalPoint) || !finite_point(aimPoint) ||
      !std::isfinite(radius) || radius < 0.0f) {
    return false;
  }

  const float xDelta = nodalPoint.x - aimPoint.x;
  const float zDelta = nodalPoint.z - aimPoint.z;
  return std::hypot(xDelta, zDelta) > radius;
}

} // namespace limnmoco
