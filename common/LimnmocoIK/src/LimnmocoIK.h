// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_IK_H_
#define LIMNMOCO_IK_H_

#include "Mat3.h"

namespace limnmoco {

struct VirtualPose {
  float vtrack;
  float vew;
  float vheight;
  float vpanDeg;
  float vtiltDeg;
  float vrollDeg;
};

struct CraneGeometry {
  float boomLength;
  float extensionLength;
  float offsetX;
  float offsetY;
  float offsetZ;
};

struct CraneSolveResult {
  Vec3 target;
  Vec3 offsetWorld;
  Vec3 panTarget;
  Vec3 base;
  Vec3 armTip;
  Vec3 panCenter;
  Vec3 nodal;
  Vec3 error;
  float errorLength;
  float track;
  float swingDeg;
  float boomDeg;
  bool boomClamped;
  bool swingClamped;
};

float angle_degrees(float radiansValue);
float angle_radians(float degreesValue);
CraneSolveResult solve_ik(const VirtualPose &pose, const CraneGeometry &geometry);
VirtualPose solve_fk(float boomDeg, float swingDeg, float track,
                     float panDeg, float tiltDeg, float rollDeg,
                     const CraneGeometry &geometry);

} // namespace limnmoco

#endif // LIMNMOCO_IK_H_
