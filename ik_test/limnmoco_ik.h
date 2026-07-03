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
  double x;
  double y;
  double z;
};

struct VirtualPose {
  double vtrack;
  double vew;
  double vheight;
  double vpanDeg;
  double vtiltDeg;
  double vrollDeg;
};

struct CraneGeometry {
  double boomLength;
  double extensionLength;
  double offsetX;
  double offsetY;
  double offsetZ;
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
  double errorLength;
  double track;
  double swingDeg;
  double boomDeg;
  bool boomClamped;
  bool swingClamped;
};

double degrees(double radians);
double radians(double degrees);
CraneSolveResult solveLimnmocoCrane(const VirtualPose &pose, const CraneGeometry &geometry);

} // namespace limnmoco

#endif // LIMNMOCO_IK_TEST_LIMNMOCO_IK_H_
