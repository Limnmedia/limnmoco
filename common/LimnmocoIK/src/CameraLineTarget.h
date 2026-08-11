// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_CAMERA_LINE_TARGET_H_
#define LIMNMOCO_CAMERA_LINE_TARGET_H_

#include "AimAwareTarget.h"
#include "CameraLine.h"

namespace limnmoco {

enum class CameraLineJogAxis : unsigned char {
  kX = 0,
  kY = 1,
  kZ = 2,
  kPan = 3,
  kTilt = 4,
};

// Captured effective camera orientation.  It remains unchanged for the life
// of a translation line jog even if aim compensation rotates the camera.
struct CameraLineOrientation {
  float vpanDeg;
  float vtiltDeg;
  float vrollDeg;
};

// Builds one finite horizon target. signedDirection must be positive or
// negative. translationDistance is millimetres for X/Y/Z; rotationStepDeg is
// used for PAN/TILT.  The function is transactional and leaves output
// unchanged on failure.
bool build_camera_line_target(const VirtualPose &currentPose,
                              const VirtualAimState &aimState,
                              const CameraLineOrientation &orientation,
                              CameraLineJogAxis axis, float signedDirection,
                              float translationDistance, float rotationStepDeg,
                              AimAwareTarget *output);

} // namespace limnmoco

#endif // LIMNMOCO_CAMERA_LINE_TARGET_H_
