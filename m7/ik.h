// SPDX-License-Identifier: BSD-3-Clause

#ifndef M7_IK_H_
#define M7_IK_H_

#include <LimnmocoIK.h>
#include <BoomCompensation.h>
#include <KuperTrackConvention.h>

using limnmoco::CraneGeometry;
using limnmoco::CraneSolveResult;
using limnmoco::BoomCompensationTable;
using limnmoco::Vec3;
using limnmoco::VirtualPose;
using limnmoco::solve_fk;
using limnmoco::solve_ik;
using limnmoco::kuper_track_to_solver;
using limnmoco::solver_track_to_kuper;

inline float ik_degrees(float radiansValue) {
  return limnmoco::angle_degrees(radiansValue);
}

inline float ik_radians(float degreesValue) {
  return limnmoco::angle_radians(degreesValue);
}

#endif // M7_IK_H_
