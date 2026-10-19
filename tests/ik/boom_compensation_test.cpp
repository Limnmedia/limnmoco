// SPDX-License-Identifier: BSD-3-Clause

#include <BoomCompensation.h>
#include <gtest/gtest.h>

TEST(BoomCompensation, ValidatesAndInterpolatesTables) {
  limnmoco::BoomCompensationTable increasing{};
  limnmoco::BoomCompensationTable decreasing{};
  for (int index = 0; index < limnmoco::kBoomCompensationEntryCount; ++index) {
    const float angle = limnmoco::kBoomCompensationMinDegrees + index;
    increasing.motorSteps[index] = (angle * 0.8f +
        (angle >= 0.0f ? angle * angle * 0.001f : -angle * angle * 0.001f)) *
        10000.0f;
    decreasing.motorSteps[index] = -increasing.motorSteps[index];
  }

  EXPECT_TRUE(limnmoco::boom_compensation_table_is_valid(increasing));
  EXPECT_TRUE(limnmoco::boom_compensation_table_is_valid(decreasing));
  float motorSteps = 0.0f;
  float angle = 0.0f;
  ASSERT_TRUE(limnmoco::boom_angle_to_steps(increasing, 10.5f, &motorSteps));
  EXPECT_NEAR(motorSteps, 85105.0f, 0.01f);
  ASSERT_TRUE(limnmoco::boom_steps_to_angle(increasing, motorSteps, &angle));
  EXPECT_NEAR(angle, 10.5f, 0.00001f);
  ASSERT_TRUE(limnmoco::boom_angle_to_steps(decreasing, -12.25f, &motorSteps));
  ASSERT_TRUE(limnmoco::boom_steps_to_angle(decreasing, motorSteps, &angle));
  EXPECT_NEAR(angle, -12.25f, 0.00001f);
  EXPECT_FALSE(limnmoco::boom_angle_to_steps(increasing, 60.1f, &motorSteps));
  EXPECT_FALSE(limnmoco::boom_steps_to_angle(increasing, 10000000.0f, &angle));
  increasing.motorSteps[1] = increasing.motorSteps[0];
  EXPECT_FALSE(limnmoco::boom_compensation_table_is_valid(increasing));
}

TEST(BoomCompensation, PreservesDragonframeSignedStepEncoding) {
  const uint32_t rawNegativeSixty = 4289715602u;
  const float negativeSixtySteps =
      static_cast<float>(static_cast<int32_t>(rawNegativeSixty));
  EXPECT_NEAR(negativeSixtySteps, -5251694.0f, 0.1f);
  EXPECT_NEAR(85557.0f, 85557.0f, 0.1f);
}

TEST(BoomCompensation, MapsGeometricAngleToRawMotorSteps) {
  limnmoco::BoomCompensationTable table{};
  for (int index = 0; index < limnmoco::kBoomCompensationEntryCount; ++index) {
    table.motorSteps[index] = (limnmoco::kBoomCompensationMinDegrees + index) * 8600.0f;
  }
  table.motorSteps[63] = 27620.3f;
  table.motorSteps[64] = 37405.6f;
  float targetSteps = 0.0f;
  float targetDegrees = 0.0f;
  ASSERT_TRUE(limnmoco::boom_angle_to_steps(table, 3.342f, &targetSteps));
  EXPECT_NEAR(targetSteps, 30966.87f, 0.1f);
  ASSERT_TRUE(limnmoco::boom_steps_to_angle(table, targetSteps, &targetDegrees));
  EXPECT_NEAR(targetDegrees, 3.342f, 0.0001f);
}
