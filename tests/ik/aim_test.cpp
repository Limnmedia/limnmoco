// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(AimPointGeometry, DerivesOrientationAndEnforcesTheSafeCylinder) { EXPECT_TRUE(limnmoco::test::aim_geometry()); }
TEST(AimAwareTarget, PreservesOffsetsAndRejectsInvalidTargets) { EXPECT_TRUE(limnmoco::test::aim_aware_target()); }
TEST(AimPointProtocol, EncodesAndDecodesSignedFixedPointCoordinates) { EXPECT_TRUE(limnmoco::test::aim_point_protocol()); }
