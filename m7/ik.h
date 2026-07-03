// SPDX-License-Identifier: BSD-3-Clause

#ifndef M7_IK_H_
#define M7_IK_H_

#include "vec3.h"

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

float ik_degrees(float radiansValue);
float ik_radians(float degreesValue);
CraneSolveResult solve_ik(const VirtualPose &pose, const CraneGeometry &geometry);

#endif // M7_IK_H_
