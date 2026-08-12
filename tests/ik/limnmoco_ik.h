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

#include <LimnmocoIK.h>

namespace limnmoco {

struct CranePositions {
  float boom;
  float swing;
  float track;
  float pan;
  float tilt;
  float roll;
};

CraneSolveResult solveLimnmocoCrane(const VirtualPose &pose, const CraneGeometry &geometry);
VirtualPose solveForwardKinematics(const CranePositions &positions,
                                   const CraneGeometry &geometry);

Vec3 fk(const CranePositions &pos, const CraneGeometry &geometry);

} // namespace limnmoco

#endif // LIMNMOCO_IK_TEST_LIMNMOCO_IK_H_
