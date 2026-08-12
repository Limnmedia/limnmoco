// SPDX-License-Identifier: BSD-3-Clause

#include "CoordinatedTrajectory.h"
#include "test_support.h"
#include <gtest/gtest.h>

TEST(CoordinatedTrajectory, BuildsAVelocityPreservingHandoffProfile) {
  const limnmoco::CoordinatedAxisPlan plans[] = {
      {10.0f, 110.0f, 80.0f, 160.0f},
      {-5.0f, 45.0f, 40.0f, 80.0f},
  };
  const limnmoco::CoordinatedTrajectoryProfile profile =
      limnmoco::coordinated_make_handoff_profile(plans, 2, 0.2f);
  const limnmoco::CoordinatedTrajectoryProfile impossible =
      limnmoco::coordinated_make_handoff_profile(plans, 2, 2.0f);
  constexpr float kTolerance = limnmoco::test::kTrajectoryTolerance;
  EXPECT_GT(profile.duration, 0.0f);
  EXPECT_NEAR(limnmoco::coordinated_velocity(plans[0], profile, 0.0f), 20.0f, kTolerance);
  EXPECT_NEAR(limnmoco::coordinated_velocity(plans[1], profile, 0.0f), 10.0f, kTolerance);
  EXPECT_NEAR(limnmoco::coordinated_position(plans[0], profile, profile.duration), 110.0f, kTolerance);
  EXPECT_NEAR(limnmoco::coordinated_position(plans[1], profile, profile.duration), 45.0f, kTolerance);
  EXPECT_NEAR(limnmoco::coordinated_velocity(plans[0], profile, profile.duration), 0.0f, kTolerance);
  EXPECT_EQ(impossible.duration, 0.0f);
}
