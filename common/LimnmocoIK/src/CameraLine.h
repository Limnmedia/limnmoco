// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_CAMERA_LINE_H_
#define LIMNMOCO_CAMERA_LINE_H_

namespace limnmoco {

// Dragonframe/Kuper camera-line axes. X maps to vEW, Y maps to vNS, and Z
// maps to vTrack. Negative Z is optical forward.
enum class CameraLineAxis : unsigned char {
  kX = 0,
  kY = 1,
  kZ = 2,
};

// A unit translation expressed at the public virtual-coordinate boundary.
// vtrack is Kuper track: negative is forward, positive is away from set.
struct VirtualTranslationDelta {
  float vtrack;
  float vew;
  float vns;
};

// Converts a signed camera-local Kuper axis to a normalized public virtual
// translation delta using the established virtual pan/tilt/roll convention.
// direction must be finite and non-zero. Returns false for an invalid axis or
// direction and leaves output unchanged.
bool camera_line_direction(CameraLineAxis axis, float vpanDeg, float vtiltDeg,
                           float vrollDeg, VirtualTranslationDelta *output,
                           float direction = 1.0f);

} // namespace limnmoco

#endif // LIMNMOCO_CAMERA_LINE_H_
