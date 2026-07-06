// SPDX-License-Identifier: BSD-3-Clause
/**
 * \file limnmoco_ik.h
 * \brief Host-side LIMNMOCO crane IK reference solver.
 *
 * This is a plain C++ port of the Blender reference solver in
 * limnmoco-blender-addon/LIMNMOCO_CG_RIG/rigs/limnmoco_crane.py.
 * It intentionally has no Arduino or Blender dependencies.
 */

#ifndef LIMNMOCO_IK_TEST_LIMNMOCO_IK_H_
#define LIMNMOCO_IK_TEST_LIMNMOCO_IK_H_

namespace limnmoco {

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Mat3 {
  float m[3][3];
};

struct VirtualPose {
  float vtrack;
  float vew;
  float vheight;
  float vpanDeg;
  float vtiltDeg;
  float vrollDeg;
};

struct CranePositions {
  float boom;
  float swing;
  float track;
  float pan;
  float tilt;
  float roll;
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

float degrees(float radians);
float radians(float degrees);
CraneSolveResult solveLimnmocoCrane(const VirtualPose &pose, const CraneGeometry &geometry);

Vec3 fk(const CranePositions &pos, const CraneGeometry &geometry);

} // namespace limnmoco

#endif // LIMNMOCO_IK_TEST_LIMNMOCO_IK_H_
