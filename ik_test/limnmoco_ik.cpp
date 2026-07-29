// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

namespace limnmoco {

CraneSolveResult solveLimnmocoCrane(const VirtualPose &pose,
                                    const CraneGeometry &geometry) {
  return solve_ik(pose, geometry);
}

VirtualPose solveForwardKinematics(const CranePositions &positions,
                                   const CraneGeometry &geometry) {
  return solve_fk(positions.boom, positions.swing, positions.track,
                  positions.pan, positions.tilt, positions.roll, geometry);
}

Vec3 fk(const CranePositions &positions, const CraneGeometry &geometry) {
  const VirtualPose pose = solveForwardKinematics(positions, geometry);
  return Vec3{pose.vew, pose.vtrack, pose.vheight};
}

} // namespace limnmoco
