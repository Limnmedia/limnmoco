// SPDX-License-Identifier: BSD-3-Clause

#include "CoordinatedTrajectory.h"
#include "test_support.h"

#include <gtest/gtest.h>

namespace {

using limnmoco::CoordinatedAxisPlan;
using limnmoco::CoordinatedTrajectoryProfile;
using limnmoco::coordinated_acceleration;
using limnmoco::coordinated_make_profile;
using limnmoco::coordinated_minimum_duration;
using limnmoco::coordinated_move_duration;
using limnmoco::coordinated_position;
using limnmoco::coordinated_velocity;

constexpr float kTolerance = limnmoco::test::kTrajectoryTolerance;

TEST(CoordinatedTrajectory, CalculatesTriangularAndTrapezoidalDurations) {
  EXPECT_NEAR(coordinated_minimum_duration(1.0f, 10.0f, 4.0f), 1.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_minimum_duration(100.0f, 10.0f, 4.0f), 12.5f,
              kTolerance);
}

TEST(CoordinatedTrajectory, SharesTheLongestAxisDuration) {
  const CoordinatedAxisPlan plans[] = {
      {0.0f, 100.0f, 10.0f, 4.0f},
      {0.0f, 1.0f, 10.0f, 4.0f},
  };
  const float duration = coordinated_move_duration(plans, 2);

  EXPECT_NEAR(duration, 12.5f, kTolerance);
  EXPECT_NEAR(coordinated_position(plans[0], duration, 0.0f), 0.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_position(plans[0], duration, duration), 100.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_position(plans[1], duration, duration), 1.0f,
              kTolerance);
}

TEST(CoordinatedTrajectory, UsesASharedTrapezoidalProfile) {
  const CoordinatedAxisPlan plans[] = {
      {0.0f, 100.0f, 10.0f, 4.0f},
      {0.0f, 1.0f, 10.0f, 4.0f},
  };
  const CoordinatedTrajectoryProfile profile =
      coordinated_make_profile(plans, 2);

  EXPECT_NEAR(profile.duration, 12.5f, kTolerance);
  EXPECT_NEAR(profile.accelerationTime, 2.5f, kTolerance);
  EXPECT_NEAR(profile.cruiseTime, 7.5f, kTolerance);
  EXPECT_NEAR(profile.peakProgressVelocity, 0.1f, kTolerance);
  EXPECT_NEAR(coordinated_position(plans[0], profile, 2.5f), 12.5f,
              kTolerance);
  EXPECT_NEAR(coordinated_velocity(plans[0], profile, 6.25f), 10.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plans[0], profile, 1.0f), 4.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plans[0], profile, 6.25f), 0.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plans[0], profile, 11.0f), -4.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_position(plans[0], profile, profile.duration),
              100.0f, kTolerance);
  EXPECT_NEAR(coordinated_position(plans[1], profile, 6.25f), 0.5f,
              kTolerance);
}

TEST(CoordinatedTrajectory, UsesAConstrainedTriangularProfileForShortMoves) {
  const CoordinatedAxisPlan plan[] = {
      {0.0f, -1.0f, 10.0f, 4.0f},
  };
  const CoordinatedTrajectoryProfile profile =
      coordinated_make_profile(plan, 1);

  EXPECT_NEAR(profile.duration, 1.0f, kTolerance);
  EXPECT_NEAR(profile.cruiseTime, 0.0f, kTolerance);
  EXPECT_NEAR(coordinated_velocity(plan[0], profile, 0.5f), -2.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plan[0], profile, 0.25f), -4.0f,
              kTolerance);
}

TEST(CoordinatedTrajectory, ScalesFastAxesToTheLimitingAxis) {
  const CoordinatedAxisPlan plans[] = {
      {0.0f, 100.0f, 100.0f, 100.0f},
      {0.0f, 10.0f, 1.0f, 0.5f},
  };
  const CoordinatedTrajectoryProfile profile =
      coordinated_make_profile(plans, 2);

  EXPECT_NEAR(profile.duration, 12.0f, kTolerance);
  EXPECT_NEAR(coordinated_velocity(plans[0], profile, 6.0f), 10.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_velocity(plans[1], profile, 6.0f), 1.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plans[0], profile, 1.0f), 5.0f,
              kTolerance);
  EXPECT_NEAR(coordinated_acceleration(plans[1], profile, 1.0f), 0.5f,
              kTolerance);
}

} // namespace
