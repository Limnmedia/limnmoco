// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(CameraLineTarget, HoldsTheCapturedOrientationAcrossHorizons) { EXPECT_TRUE(limnmoco::test::camera_line_target()); }
TEST(CameraLineCoordinates, MapsKuperAxesAndCameraRotation) { EXPECT_TRUE(limnmoco::test::camera_line_direction()); }
