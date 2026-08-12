// SPDX-License-Identifier: BSD-3-Clause

#include <AimAwareTarget.h>
#include <CameraLine.h>
#include <CameraLineTarget.h>
#include <gtest/gtest.h>

#include <cmath>

TEST(CameraLineTarget, HoldsTheCapturedOrientationAcrossHorizons) {
  const limnmoco::VirtualPose current{0.0f, 10.0f, 20.0f, 90.0f, 0.0f, 0.0f};
  const limnmoco::VirtualAimState noAim{false, true, {}, 0.0f, 0.0f, 0.0f};
  const limnmoco::CameraLineOrientation captured{90.0f, 0.0f, 0.0f};
  limnmoco::AimAwareTarget first{};
  limnmoco::AimAwareTarget second{};
  ASSERT_TRUE(limnmoco::build_camera_line_target(
      current, noAim, captured, limnmoco::CameraLineJogAxis::kZ,
      -1.0f, 10.0f, 1.0f, &first));
  ASSERT_TRUE(limnmoco::build_camera_line_target(
      first.pose, noAim, captured, limnmoco::CameraLineJogAxis::kZ,
      -1.0f, 10.0f, 1.0f, &second));
  const limnmoco::VirtualAimState aiming{
      true, true, {10.0f, 100.0f, 0.0f}, 1.0f, 3.0f, -2.0f};
  limnmoco::AimAwareTarget aimed{};
  EXPECT_TRUE(limnmoco::build_camera_line_target(
      limnmoco::VirtualPose{}, aiming,
      limnmoco::CameraLineOrientation{}, limnmoco::CameraLineJogAxis::kX,
      1.0f, 2.0f, 1.0f, &aimed));
  limnmoco::AimAwareTarget unsafe{};
  EXPECT_FALSE(limnmoco::build_camera_line_target(
      limnmoco::VirtualPose{},
      limnmoco::VirtualAimState{true, true, {0.0f, 100.0f, 0.0f}, 2.0f, 0.0f, 0.0f},
      limnmoco::CameraLineOrientation{}, limnmoco::CameraLineJogAxis::kX,
      1.0f, 1.0f, 1.0f, &unsafe));
  EXPECT_NEAR(first.pose.vew, 0.0f, 0.0001f);
  EXPECT_NEAR(first.pose.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(first.pose.vheight, 20.0f, 0.0001f);
  EXPECT_NEAR(second.pose.vew, -10.0f, 0.0001f);
  EXPECT_NEAR(aimed.panOffsetDeg, 3.0f, 0.0001f);
  EXPECT_NEAR(aimed.tiltOffsetDeg, -2.0f, 0.0001f);
}

TEST(CameraLineCoordinates, MapsKuperAxesAndCameraRotation) {
  using limnmoco::CameraLineAxis;
  using limnmoco::VirtualTranslationDelta;
  VirtualTranslationDelta x{}, y{}, z{}, forward{}, panRight{}, panLeft{};
  VirtualTranslationDelta tiltUp{}, tiltDown{}, rolledX{}, combined{};
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kX, 0, 0, 0, &x));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kY, 0, 0, 0, &y));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, 0, 0, 0, &z));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, 0, 0, 0, &forward, -1));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, 90, 0, 0, &panRight));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, -90, 0, 0, &panLeft));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, 0, 90, 0, &tiltUp));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, 0, -90, 0, &tiltDown));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kX, 0, 0, 90, &rolledX));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kY, 20, -15, 30, &combined));
  EXPECT_NEAR(x.vew, 1.0f, 0.0001f); EXPECT_NEAR(x.vns, 0.0f, 0.0001f); EXPECT_NEAR(x.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(y.vew, 0.0f, 0.0001f); EXPECT_NEAR(y.vns, 1.0f, 0.0001f); EXPECT_NEAR(y.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(z.vew, 0.0f, 0.0001f); EXPECT_NEAR(z.vns, 0.0f, 0.0001f); EXPECT_NEAR(z.vtrack, 1.0f, 0.0001f);
  EXPECT_NEAR(forward.vew, 0.0f, 0.0001f); EXPECT_NEAR(forward.vns, 0.0f, 0.0001f); EXPECT_NEAR(forward.vtrack, -1.0f, 0.0001f);
  EXPECT_NEAR(panRight.vew, 1.0f, 0.0001f); EXPECT_NEAR(panRight.vns, 0.0f, 0.0001f); EXPECT_NEAR(panRight.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(panLeft.vew, -1.0f, 0.0001f); EXPECT_NEAR(panLeft.vns, 0.0f, 0.0001f); EXPECT_NEAR(panLeft.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(tiltUp.vew, 0.0f, 0.0001f); EXPECT_NEAR(tiltUp.vns, -1.0f, 0.0001f); EXPECT_NEAR(tiltUp.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(tiltDown.vew, 0.0f, 0.0001f); EXPECT_NEAR(tiltDown.vns, 1.0f, 0.0001f); EXPECT_NEAR(tiltDown.vtrack, 0.0f, 0.0001f);
  EXPECT_NEAR(rolledX.vew, 0.0f, 0.0001f); EXPECT_NEAR(rolledX.vns, -1.0f, 0.0001f); EXPECT_NEAR(rolledX.vtrack, 0.0f, 0.0001f);
  const float length = std::sqrt(combined.vew * combined.vew + combined.vns * combined.vns + combined.vtrack * combined.vtrack);
  EXPECT_NEAR(length, 1.0f, 0.0001f);
}
