// SPDX-License-Identifier: BSD-3-Clause

#include <AimAwareTarget.h>
#include <AimGeometry.h>
#include <AimPointProtocol.h>
#include <CameraLine.h>
#include <gtest/gtest.h>

TEST(AimPointGeometry, DerivesOrientationAndEnforcesTheSafeCylinder) {
  using limnmoco::AimOrientation;
  using limnmoco::CameraLineAxis;
  using limnmoco::KuperPoint;
  using limnmoco::VirtualTranslationDelta;
  const KuperPoint nodal{};
  AimOrientation forward{}, right{}, left{}, above{}, below{}, offset{}, wrapped{};
  VirtualTranslationDelta aimForward{}, aimRight{}, aimAbove{};
  ASSERT_TRUE(limnmoco::aim_base_orientation(nodal, {0, 0, -10}, 23, &forward));
  ASSERT_TRUE(limnmoco::aim_base_orientation(nodal, {10, 0, 0}, 0, &right));
  ASSERT_TRUE(limnmoco::aim_base_orientation(nodal, {-10, 0, 0}, 0, &left));
  ASSERT_TRUE(limnmoco::aim_base_orientation(nodal, {0, 10, -1}, 0, &above));
  ASSERT_TRUE(limnmoco::aim_base_orientation(nodal, {0, -10, -1}, 0, &below));
  ASSERT_TRUE(limnmoco::aim_apply_relative_offsets(forward, 20, -10, &offset));
  ASSERT_TRUE(limnmoco::aim_apply_relative_offsets({170, 5, 7}, 20, -2, &wrapped));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, forward.vpanDeg, forward.vtiltDeg, forward.vrollDeg, &aimForward, -1));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, right.vpanDeg, right.vtiltDeg, right.vrollDeg, &aimRight, -1));
  ASSERT_TRUE(limnmoco::camera_line_direction(CameraLineAxis::kZ, above.vpanDeg, above.vtiltDeg, above.vrollDeg, &aimAbove, -1));
  EXPECT_NEAR(forward.vpanDeg, 0, 0.0001f); EXPECT_NEAR(forward.vtiltDeg, 0, 0.0001f); EXPECT_NEAR(forward.vrollDeg, 23, 0.0001f);
  EXPECT_NEAR(right.vpanDeg, -90, 0.0001f); EXPECT_NEAR(right.vtiltDeg, 0, 0.0001f);
  EXPECT_NEAR(left.vpanDeg, 90, 0.0001f); EXPECT_NEAR(left.vtiltDeg, 0, 0.0001f);
  EXPECT_NEAR(above.vpanDeg, 0, 0.0001f); EXPECT_NEAR(above.vtiltDeg, 84.2894f, 0.0001f);
  EXPECT_NEAR(below.vpanDeg, 0, 0.0001f); EXPECT_NEAR(below.vtiltDeg, -84.2894f, 0.0001f);
  EXPECT_NEAR(offset.vpanDeg, 20, 0.0001f); EXPECT_NEAR(offset.vtiltDeg, -10, 0.0001f); EXPECT_NEAR(offset.vrollDeg, 23, 0.0001f);
  EXPECT_NEAR(wrapped.vpanDeg, -170, 0.0001f); EXPECT_NEAR(wrapped.vtiltDeg, 3, 0.0001f); EXPECT_NEAR(wrapped.vrollDeg, 7, 0.0001f);
  EXPECT_NEAR(aimForward.vew, 0, 0.0001f); EXPECT_NEAR(aimForward.vns, 0, 0.0001f); EXPECT_NEAR(aimForward.vtrack, -1, 0.0001f);
  EXPECT_NEAR(aimRight.vew, 1, 0.0001f); EXPECT_NEAR(aimRight.vns, 0, 0.0001f); EXPECT_NEAR(aimRight.vtrack, 0, 0.0001f);
  EXPECT_NEAR(aimAbove.vew, 0, 0.0001f); EXPECT_NEAR(aimAbove.vns, 0.9950f, 0.0001f); EXPECT_NEAR(aimAbove.vtrack, -0.0995f, 0.0001f);
  EXPECT_FALSE(limnmoco::aim_base_orientation(nodal, nodal, 0, &forward));
  EXPECT_FALSE(limnmoco::aim_point_outside_safe_cylinder({0, 999, 0}, nodal, 10));
  EXPECT_FALSE(limnmoco::aim_point_outside_safe_cylinder({10, 0, 0}, nodal, 10));
  EXPECT_TRUE(limnmoco::aim_point_outside_safe_cylinder({10.001f, 0, 0}, nodal, 10));
  EXPECT_FALSE(limnmoco::aim_point_outside_safe_cylinder(nodal, nodal, -1));
}

TEST(AimAwareTarget, PreservesOffsetsAndRejectsInvalidTargets) {
  using limnmoco::AimAwareTarget;
  using limnmoco::KuperPoint;
  using limnmoco::VirtualAimState;
  using limnmoco::VirtualPose;
  using limnmoco::VirtualTargetAxis;
  const VirtualPose current{0, 0, 0, 3, 4, 5};
  const VirtualAimState noAim{false, true, KuperPoint{}, 0, 0, 0};
  const VirtualAimState aimed{true, true, {0, 0, -10}, 2, 20, -5};
  const VirtualAimState noRollAim{true, false, {0, 0, -10}, 2, 0, 0};
  AimAwareTarget direct{}, translated{}, panOffset{}, noRoll{};
  AimAwareTarget unchanged{{99, 98, 97, 96, 95, 94}, 93, 92};
  const AimAwareTarget sentinel = unchanged;
  ASSERT_TRUE(limnmoco::build_aim_aware_target(current, noAim, VirtualTargetAxis::kEW, 7, &direct));
  ASSERT_TRUE(limnmoco::build_aim_aware_target(current, aimed, VirtualTargetAxis::kEW, 1, &translated));
  ASSERT_TRUE(limnmoco::build_aim_aware_target(current, aimed, VirtualTargetAxis::kPan, 30, &panOffset));
  ASSERT_TRUE(limnmoco::build_aim_aware_target(VirtualPose{}, noRollAim, VirtualTargetAxis::kNS, 1, &noRoll));
  EXPECT_FALSE(limnmoco::build_aim_aware_target(current, aimed, VirtualTargetAxis::kTrack, -9, &unchanged));
  EXPECT_FALSE(limnmoco::build_aim_aware_target(current, noRollAim, VirtualTargetAxis::kRoll, 2, &unchanged));
  EXPECT_NEAR(direct.pose.vtrack, 0, 0.0001f); EXPECT_NEAR(direct.pose.vew, 7, 0.0001f); EXPECT_NEAR(direct.pose.vheight, 0, 0.0001f);
  EXPECT_NEAR(direct.pose.vpanDeg, 3, 0.0001f); EXPECT_NEAR(direct.pose.vtiltDeg, 4, 0.0001f); EXPECT_NEAR(direct.pose.vrollDeg, 5, 0.0001f);
  EXPECT_NEAR(translated.pose.vtrack, 0, 0.0001f); EXPECT_NEAR(translated.pose.vew, 1, 0.0001f); EXPECT_NEAR(translated.pose.vheight, 0, 0.0001f);
  EXPECT_NEAR(translated.pose.vpanDeg, 25.7106f, 0.0001f); EXPECT_NEAR(translated.pose.vtiltDeg, -5, 0.0001f); EXPECT_NEAR(translated.pose.vrollDeg, 5, 0.0001f);
  EXPECT_NEAR(translated.panOffsetDeg, 20, 0.0001f); EXPECT_NEAR(translated.tiltOffsetDeg, -5, 0.0001f);
  EXPECT_NEAR(panOffset.pose.vpanDeg, 30, 0.0001f); EXPECT_NEAR(panOffset.pose.vtiltDeg, -5, 0.0001f); EXPECT_NEAR(panOffset.panOffsetDeg, 30, 0.0001f); EXPECT_NEAR(panOffset.tiltOffsetDeg, -5, 0.0001f);
  EXPECT_NEAR(noRoll.pose.vrollDeg, 0, 0.0001f); EXPECT_NEAR(noRoll.panOffsetDeg, 0, 0.0001f); EXPECT_NEAR(noRoll.tiltOffsetDeg, 0, 0.0001f);
  EXPECT_FLOAT_EQ(unchanged.pose.vtrack, sentinel.pose.vtrack);
  EXPECT_FLOAT_EQ(unchanged.pose.vew, sentinel.pose.vew);
  EXPECT_FLOAT_EQ(unchanged.pose.vheight, sentinel.pose.vheight);
  EXPECT_FLOAT_EQ(unchanged.pose.vpanDeg, sentinel.pose.vpanDeg);
  EXPECT_FLOAT_EQ(unchanged.pose.vtiltDeg, sentinel.pose.vtiltDeg);
  EXPECT_FLOAT_EQ(unchanged.pose.vrollDeg, sentinel.pose.vrollDeg);
  EXPECT_FLOAT_EQ(unchanged.panOffsetDeg, sentinel.panOffsetDeg);
  EXPECT_FLOAT_EQ(unchanged.tiltOffsetDeg, sentinel.tiltOffsetDeg);
}

TEST(AimPointProtocol, EncodesAndDecodesSignedFixedPointCoordinates) {
  limnmoco::AimPointConfiguration parsed{};
  uint8_t enabled = 0;
  int32_t rawX = 0, rawY = 0, rawZ = 0;
  ASSERT_TRUE(limnmoco::aim_point_from_protocol(1, -1234, 5678, -9000, &parsed));
  EXPECT_TRUE(parsed.enabled);
  EXPECT_NEAR(parsed.point.x, -1.234f, 0.0001f); EXPECT_NEAR(parsed.point.y, 5.678f, 0.0001f); EXPECT_NEAR(parsed.point.z, -9.0f, 0.0001f);
  ASSERT_TRUE(limnmoco::aim_point_to_protocol(parsed, &enabled, &rawX, &rawY, &rawZ));
  EXPECT_EQ(enabled, 1); EXPECT_EQ(rawX, -1234); EXPECT_EQ(rawY, 5678); EXPECT_EQ(rawZ, -9000);
  EXPECT_FALSE(limnmoco::aim_point_from_protocol(2, 0, 0, 0, &parsed));
  EXPECT_FALSE(limnmoco::aim_point_from_protocol(1, 0, 0, 0, nullptr));
}
