// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_AIM_AWARE_TARGET_H_
#define LIMNMOCO_AIM_AWARE_TARGET_H_

#include "AimGeometry.h"
#include "LimnmocoIK.h"

namespace limnmoco {

enum class VirtualTargetAxis : unsigned char {
  kTrack,
  kEW,
  kNS,
  kPan,
  kTilt,
  kRoll,
};

// Persistent logical state needed to build one virtual target. PAN/TILT are
// aim-relative offsets only when aimEnabled is true.
struct VirtualAimState {
  bool aimEnabled;
  bool rollPresent;
  KuperPoint aimPoint;
  float safeDistance;
  float panOffsetDeg;
  float tiltOffsetDeg;
};

// The effective pose is passed to IK. The offsets are retained by M7 as the
// logical vPAN/vTILT values Dragonframe controls while aim is active.
struct AimAwareTarget {
  VirtualPose pose;
  float panOffsetDeg;
  float tiltOffsetDeg;
};

// Builds a fully validated target without mutating currentPose, aimState, or
// output on failure. requestedValue is the absolute public value for axis.
bool build_aim_aware_target(const VirtualPose &currentPose,
                            const VirtualAimState &aimState,
                            VirtualTargetAxis axis, float requestedValue,
                            AimAwareTarget *output);

} // namespace limnmoco

#endif // LIMNMOCO_AIM_AWARE_TARGET_H_
