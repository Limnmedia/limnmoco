// SPDX-License-Identifier: BSD-3-Clause

#include "CoordinatedTrajectory.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

void requireNear(float actual, float expected, float tolerance,
                 const char *message) {
  if (std::fabs(actual - expected) > tolerance) {
    std::cerr << "[FAIL] " << message << ": expected " << expected
              << ", got " << actual << "\n";
    std::exit(EXIT_FAILURE);
  }
}

} // namespace

int main() {
  using limnmoco::CoordinatedAxisPlan;
  using limnmoco::CoordinatedTrajectoryProfile;
  using limnmoco::coordinated_acceleration;
  using limnmoco::coordinated_make_profile;
  using limnmoco::coordinated_minimum_duration;
  using limnmoco::coordinated_move_duration;
  using limnmoco::coordinated_position;
  using limnmoco::coordinated_velocity;

  requireNear(coordinated_minimum_duration(1.0f, 10.0f, 4.0f), 1.0f,
              1e-5f, "triangular minimum duration");
  requireNear(coordinated_minimum_duration(100.0f, 10.0f, 4.0f), 12.5f,
              1e-5f, "trapezoidal minimum duration");

  const CoordinatedAxisPlan plans[] = {
      {0.0f, 100.0f, 10.0f, 4.0f},
      {0.0f, 1.0f, 10.0f, 4.0f},
  };
  const float duration = coordinated_move_duration(plans, 2);
  requireNear(duration, 12.5f, 1e-5f, "common duration");

  requireNear(coordinated_position(plans[0], duration, 0.0f), 0.0f,
              1e-5f, "position at start");
  requireNear(coordinated_position(plans[0], duration, duration), 100.0f,
              1e-5f, "position at end");
  requireNear(coordinated_position(plans[1], duration, duration), 1.0f,
              1e-5f, "short axis reaches target with long axis");

  const CoordinatedTrajectoryProfile longProfile =
      coordinated_make_profile(plans, 2);
  requireNear(longProfile.duration, 12.5f, 1e-5f,
              "shared trapezoidal duration");
  requireNear(longProfile.accelerationTime, 2.5f, 1e-5f,
              "shared trapezoidal acceleration time");
  requireNear(longProfile.cruiseTime, 7.5f, 1e-5f,
              "shared trapezoidal cruise time");
  requireNear(longProfile.peakProgressVelocity, 0.1f, 1e-5f,
              "shared trapezoidal peak progress velocity");
  requireNear(coordinated_position(plans[0], longProfile, 2.5f), 12.5f,
              1e-5f, "long axis position at cruise start");
  requireNear(coordinated_velocity(plans[0], longProfile, 6.25f), 10.0f,
              1e-5f, "long axis never exceeds configured maximum velocity");
  requireNear(coordinated_acceleration(plans[0], longProfile, 1.0f), 4.0f,
              1e-5f, "long axis configured acceleration");
  requireNear(coordinated_acceleration(plans[0], longProfile, 6.25f), 0.0f,
              1e-5f, "long axis cruise acceleration");
  requireNear(coordinated_acceleration(plans[0], longProfile, 11.0f), -4.0f,
              1e-5f, "long axis configured deceleration");
  requireNear(coordinated_position(plans[0], longProfile, longProfile.duration),
              100.0f, 1e-5f, "long axis shared-profile endpoint");
  requireNear(coordinated_position(plans[1], longProfile, 6.25f), 0.5f,
              1e-5f, "short axis shares normalized progress");

  const CoordinatedAxisPlan shortPlan[] = {
      {0.0f, -1.0f, 10.0f, 4.0f},
  };
  const CoordinatedTrajectoryProfile shortProfile =
      coordinated_make_profile(shortPlan, 1);
  requireNear(shortProfile.duration, 1.0f, 1e-5f,
              "short triangular duration");
  requireNear(shortProfile.cruiseTime, 0.0f, 1e-5f,
              "short move has no cruise");
  requireNear(coordinated_velocity(shortPlan[0], shortProfile, 0.5f), -2.0f,
              1e-5f, "short move lower signed peak velocity");
  requireNear(coordinated_acceleration(shortPlan[0], shortProfile, 0.25f), -4.0f,
              1e-5f, "short move configured signed acceleration");

  const CoordinatedAxisPlan mixedLimitPlans[] = {
      {0.0f, 100.0f, 100.0f, 100.0f},
      {0.0f, 10.0f, 1.0f, 0.5f},
  };
  const CoordinatedTrajectoryProfile mixedLimitProfile =
      coordinated_make_profile(mixedLimitPlans, 2);
  requireNear(mixedLimitProfile.duration, 12.0f, 1e-5f,
              "mixed limits shared duration");
  requireNear(coordinated_velocity(mixedLimitPlans[0], mixedLimitProfile, 6.0f),
              10.0f, 1e-5f, "fast axis is scaled below its velocity limit");
  requireNear(coordinated_velocity(mixedLimitPlans[1], mixedLimitProfile, 6.0f),
              1.0f, 1e-5f, "slow axis reaches its velocity limit");
  requireNear(coordinated_acceleration(mixedLimitPlans[0], mixedLimitProfile, 1.0f),
              5.0f, 1e-5f, "fast axis is scaled below its acceleration limit");
  requireNear(coordinated_acceleration(mixedLimitPlans[1], mixedLimitProfile, 1.0f),
              0.5f, 1e-5f, "slow axis reaches its acceleration limit");

  std::cout << "All coordinated trajectory tests passed.\n";
  return EXIT_SUCCESS;
}
