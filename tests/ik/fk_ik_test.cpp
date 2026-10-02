// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"
#include <gtest/gtest.h>

TEST(IkForwardKinematics, NormalizesTheConfigurationOrigin) {
  const limnmoco::CraneGeometry geometry{857.7f, 78.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose zero = limnmoco::solveForwardKinematics({}, geometry);
  const limnmoco::VirtualPose starting = limnmoco::solveForwardKinematics(
      {0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f}, geometry);
  const limnmoco::VirtualPose moved = limnmoco::solveForwardKinematics(
      {0.0f, 0.0f, 12.6f, 0.0f, 0.0f, 0.0f}, geometry);
  const float normalized_track = moved.vtrack - starting.vtrack;
  const limnmoco::CraneSolveResult direct_track = limnmoco::solveLimnmocoCrane(
      {zero.vtrack + 1.0f, zero.vew, zero.vheight, zero.vpanDeg,
       zero.vtiltDeg, zero.vrollDeg}, geometry);
  EXPECT_NEAR(zero.vtrack, 935.7f, 0.0001f);
  EXPECT_NEAR(normalized_track, 12.5f, 0.0001f);
  EXPECT_NEAR(direct_track.track, 1.0f, 0.0001f);
}

TEST(IkVirtualAxes, EastWestCompensatesTrackWithoutNorthSouthMotion) {
  const limnmoco::CraneGeometry geometry{857.7f, 78.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose current = limnmoco::solveForwardKinematics(
      {0.0f, 2.5f, 0.0f, 0.0f, 0.0f, 0.0f}, geometry);
  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(
      {current.vtrack, current.vew + 0.502f, current.vheight,
       current.vpanDeg, current.vtiltDeg, current.vrollDeg}, geometry);
  EXPECT_NEAR(result.track, 0.0219f, 0.001f);
  EXPECT_NEAR(result.errorLength, 0.0f, 0.0001f);
  EXPECT_FALSE(result.boomClamped);
  EXPECT_FALSE(result.swingClamped);
}

TEST(IkSolver, MatchesTheBlenderDefaultPanelValues) {
  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(
      {8.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f}, {10.0f, 2.8f, 0, 0, 0});
  EXPECT_NEAR(result.track, -4.17623f, 0.00001f);
  EXPECT_NEAR(result.swingDeg, 9.32780f, 0.00001f);
  EXPECT_NEAR(result.boomDeg, 17.4576f, 0.00001f);
  EXPECT_NEAR(result.errorLength, 0.0f, 0.00001f);
  EXPECT_FALSE(result.boomClamped || result.swingClamped);
}

TEST(IkSolver, CentersAnUnrotatedTargetWithoutOffsets) {
  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(
      {12.8f, 0, 0, 0, 0, 0}, {10.0f, 2.8f, 0, 0, 0});
  EXPECT_NEAR(result.track, 0.0f, 0.00001f);
  EXPECT_NEAR(result.swingDeg, 0.0f, 0.00001f);
  EXPECT_NEAR(result.boomDeg, 0.0f, 0.00001f);
  EXPECT_NEAR(result.errorLength, 0.0f, 0.00001f);
  EXPECT_FALSE(result.boomClamped || result.swingClamped);
}

TEST(IkSolver, ReconstructsARotatedNodalOffset) {
  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(
      {8.0f, 2.0f, 3.0f, 20.0f, 10.0f, -5.0f},
      {10.0f, 2.8f, 0.5f, 0.25f, -0.1f});
  EXPECT_NEAR(result.errorLength, 0.0f, 0.00001f);
  EXPECT_FALSE(result.boomClamped || result.swingClamped);
}
