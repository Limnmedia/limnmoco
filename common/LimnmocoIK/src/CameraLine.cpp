// SPDX-License-Identifier: BSD-3-Clause

#include "CameraLine.h"

#include "KuperTrackConvention.h"
#include "LimnmocoIK.h"

#include <cmath>

namespace limnmoco {

bool camera_line_direction(CameraLineAxis axis, float vpanDeg, float vtiltDeg,
                           float vrollDeg, VirtualTranslationDelta *output,
                           float direction) {
  if (!output || !std::isfinite(direction) || direction == 0.0f) {
    return false;
  }

  // The shared IK rotation matrix operates in solver coordinates:
  // X=EW, Y=solver track, Z=NS. Convert the public Kuper basis first.
  Vec3 localSolver{};
  switch (axis) {
    case CameraLineAxis::kX:
      localSolver = Vec3{direction, 0.0f, 0.0f};
      break;
    case CameraLineAxis::kY:
      localSolver = Vec3{0.0f, 0.0f, direction};
      break;
    case CameraLineAxis::kZ:
      localSolver = Vec3{0.0f, kuper_track_to_solver(direction), 0.0f};
      break;
    default:
      return false;
  }

  const Vec3 rotated = mat3_multiply_v(
      virtual_rotation_matrix(vpanDeg, vtiltDeg, vrollDeg), localSolver);
  const float length = vec3_length(rotated);
  if (!std::isfinite(length) || length == 0.0f) {
    return false;
  }

  const float inverseLength = 1.0f / length;
  *output = VirtualTranslationDelta{
      solver_track_to_kuper(rotated.y) * inverseLength,
      rotated.x * inverseLength,
      rotated.z * inverseLength};
  return true;
}

} // namespace limnmoco
