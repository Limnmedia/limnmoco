// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(BoomCompensation, ValidatesAndInterpolatesTables) { EXPECT_TRUE(limnmoco::test::boom_table_mapping()); }
TEST(BoomCompensation, PreservesDragonframeSignedStepEncoding) { EXPECT_TRUE(limnmoco::test::boom_signed_step_encoding()); }
TEST(BoomCompensation, MapsGeometricAngleToRawMotorSteps) { EXPECT_TRUE(limnmoco::test::boom_step_target_mapping()); }
