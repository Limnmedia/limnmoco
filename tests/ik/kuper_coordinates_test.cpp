// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <KuperTrackConvention.h>
#include <gtest/gtest.h>

TEST(KuperCoordinates, ConvertsTrackAcrossTheSolverBoundary) {
  const limnmoco::VirtualPose kuperPose{-5.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
  const limnmoco::VirtualPose solverPose = limnmoco::kuper_pose_to_solver(kuperPose);
  const limnmoco::VirtualPose roundTrip = limnmoco::solver_pose_to_kuper(solverPose);
  EXPECT_FLOAT_EQ(limnmoco::kuper_track_to_solver(-5.0f), 5.0f);
  EXPECT_FLOAT_EQ(limnmoco::solver_track_to_kuper(5.0f), -5.0f);
  EXPECT_FLOAT_EQ(solverPose.vtrack, 5.0f);
  EXPECT_FLOAT_EQ(solverPose.vew, kuperPose.vew);
  EXPECT_FLOAT_EQ(solverPose.vheight, kuperPose.vheight);
  EXPECT_FLOAT_EQ(solverPose.vpanDeg, kuperPose.vpanDeg);
  EXPECT_FLOAT_EQ(solverPose.vtiltDeg, kuperPose.vtiltDeg);
  EXPECT_FLOAT_EQ(solverPose.vrollDeg, kuperPose.vrollDeg);
  EXPECT_FLOAT_EQ(roundTrip.vtrack, kuperPose.vtrack);
  EXPECT_FLOAT_EQ(roundTrip.vew, kuperPose.vew);
  EXPECT_FLOAT_EQ(roundTrip.vheight, kuperPose.vheight);
  EXPECT_FLOAT_EQ(roundTrip.vpanDeg, kuperPose.vpanDeg);
  EXPECT_FLOAT_EQ(roundTrip.vtiltDeg, kuperPose.vtiltDeg);
  EXPECT_FLOAT_EQ(roundTrip.vrollDeg, kuperPose.vrollDeg);
}

TEST(KuperCoordinates, UsesNegativeRawTrackForNorthSouthCompensation) {
  const limnmoco::CraneGeometry geometry{857.7f, 78.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose origin = limnmoco::solve_fk(
      0.0f, 0.0f, limnmoco::kuper_track_to_solver(0.0f), 0.0f, 0.0f, 0.0f, geometry);
  limnmoco::VirtualPose target = limnmoco::kuper_pose_to_solver(
      limnmoco::VirtualPose{0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f});
  target.vtrack += origin.vtrack;
  const limnmoco::CraneSolveResult result = limnmoco::solve_ik(target, geometry);
  EXPECT_NEAR(result.track, 5.8494f, 0.001f);
  EXPECT_NEAR(limnmoco::solver_track_to_kuper(result.track), -5.8494f, 0.001f);
  EXPECT_FLOAT_EQ(limnmoco::solver_track_to_kuper(
                      limnmoco::kuper_track_to_solver(-5.0f)), -5.0f);
}
