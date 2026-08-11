// SPDX-License-Identifier: BSD-3-Clause

#include "CoordinatedTrajectory.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace limnmoco {
namespace {

float positive_or_zero(float value) {
  return value > 0.0f ? value : 0.0f;
}

} // namespace

float coordinated_minimum_duration(float distance, float maxVelocity,
                                   float maxAcceleration) {
  const float d = std::fabs(distance);
  const float v = positive_or_zero(maxVelocity);
  const float a = positive_or_zero(maxAcceleration);
  if (d == 0.0f || v == 0.0f || a == 0.0f) {
    return d == 0.0f ? 0.0f : 0.0f;
  }

  const float distanceAtMaxVelocity = (v * v) / a;
  if (d <= distanceAtMaxVelocity) {
    return 2.0f * std::sqrt(d / a);
  }

  return 2.0f * v / a + (d - distanceAtMaxVelocity) / v;
}

float coordinated_move_duration(const CoordinatedAxisPlan *plans,
                                int axisCount) {
  float duration = 0.0f;
  for (int i = 0; i < axisCount; ++i) {
    duration = std::max(duration, coordinated_minimum_duration(
        plans[i].target - plans[i].start,
        plans[i].maxVelocity, plans[i].maxAcceleration));
  }
  return duration;
}

CoordinatedTrajectoryProfile coordinated_make_profile(
    const CoordinatedAxisPlan *plans, int axisCount) {
  return coordinated_make_handoff_profile(plans, axisCount, 0.0f);
}

CoordinatedTrajectoryProfile coordinated_make_handoff_profile(
    const CoordinatedAxisPlan *plans, int axisCount,
    float initialProgressVelocity) {
  CoordinatedTrajectoryProfile profile{};
  if (plans == nullptr || axisCount <= 0 ||
      !std::isfinite(initialProgressVelocity) || initialProgressVelocity < 0.0f) {
    return profile;
  }

  float maxProgressVelocity = std::numeric_limits<float>::infinity();
  float maxProgressAcceleration = std::numeric_limits<float>::infinity();
  bool hasMotion = false;
  for (int i = 0; i < axisCount; ++i) {
    const float distance = std::fabs(plans[i].target - plans[i].start);
    if (distance == 0.0f) {
      continue;
    }

    const float velocity = positive_or_zero(plans[i].maxVelocity);
    const float acceleration = positive_or_zero(plans[i].maxAcceleration);
    if (velocity == 0.0f || acceleration == 0.0f) {
      return CoordinatedTrajectoryProfile{};
    }

    hasMotion = true;
    maxProgressVelocity = std::min(maxProgressVelocity, velocity / distance);
    maxProgressAcceleration = std::min(
        maxProgressAcceleration, acceleration / distance);
  }

  if (!hasMotion) {
    return profile;
  }

  if (initialProgressVelocity > maxProgressVelocity) {
    return CoordinatedTrajectoryProfile{};
  }

  const float distanceToReachPeak =
      (maxProgressVelocity * maxProgressVelocity -
       initialProgressVelocity * initialProgressVelocity) /
          (2.0f * maxProgressAcceleration) +
      (maxProgressVelocity * maxProgressVelocity) /
          (2.0f * maxProgressAcceleration);
  profile.progressAcceleration = maxProgressAcceleration;
  profile.initialProgressVelocity = initialProgressVelocity;
  if (distanceToReachPeak >= 1.0f) {
    const float peakSquared = maxProgressAcceleration +
        0.5f * initialProgressVelocity * initialProgressVelocity;
    if (peakSquared < initialProgressVelocity * initialProgressVelocity) {
      return CoordinatedTrajectoryProfile{};
    }
    profile.peakProgressVelocity = std::sqrt(peakSquared);
    profile.accelerationTime =
        (profile.peakProgressVelocity - initialProgressVelocity) /
        maxProgressAcceleration;
    profile.cruiseTime = 0.0f;
  } else {
    profile.accelerationTime =
        (maxProgressVelocity - initialProgressVelocity) /
        maxProgressAcceleration;
    profile.cruiseTime =
        (1.0f - distanceToReachPeak) / maxProgressVelocity;
    profile.peakProgressVelocity = maxProgressVelocity;
  }
  profile.duration = 2.0f * profile.accelerationTime + profile.cruiseTime;
  return profile;
}

float coordinated_profile_progress(const CoordinatedTrajectoryProfile &profile,
                                   float elapsed) {
  if (profile.duration <= 0.0f) {
    return 1.0f;
  }

  const float t = std::max(0.0f, std::min(profile.duration, elapsed));
  const float accelDistance = 0.5f * profile.progressAcceleration *
                              profile.accelerationTime * profile.accelerationTime +
                              profile.initialProgressVelocity *
                                  profile.accelerationTime;
  if (t <= profile.accelerationTime) {
    return profile.initialProgressVelocity * t +
           0.5f * profile.progressAcceleration * t * t;
  }

  const float cruiseEnd = profile.accelerationTime + profile.cruiseTime;
  if (t <= cruiseEnd) {
    return accelDistance + profile.peakProgressVelocity *
           (t - profile.accelerationTime);
  }

  const float remaining = profile.duration - t;
  return 1.0f - 0.5f * profile.progressAcceleration * remaining * remaining;
}

float coordinated_profile_velocity(const CoordinatedTrajectoryProfile &profile,
                                   float elapsed) {
  if (profile.duration <= 0.0f || elapsed < 0.0f || elapsed >= profile.duration) {
    return 0.0f;
  }

  if (elapsed <= profile.accelerationTime) {
    return profile.initialProgressVelocity +
           profile.progressAcceleration * elapsed;
  }

  const float cruiseEnd = profile.accelerationTime + profile.cruiseTime;
  if (elapsed <= cruiseEnd) {
    return profile.peakProgressVelocity;
  }

  return profile.progressAcceleration * (profile.duration - elapsed);
}

float coordinated_profile_acceleration(
    const CoordinatedTrajectoryProfile &profile, float elapsed) {
  if (profile.duration <= 0.0f || elapsed < 0.0f || elapsed > profile.duration) {
    return 0.0f;
  }
  if (elapsed < profile.accelerationTime) {
    return profile.progressAcceleration;
  }
  if (elapsed < profile.accelerationTime + profile.cruiseTime) {
    return 0.0f;
  }
  if (elapsed < profile.duration) {
    return -profile.progressAcceleration;
  }
  return 0.0f;
}

float coordinated_position(const CoordinatedAxisPlan &plan,
                           const CoordinatedTrajectoryProfile &profile,
                           float elapsed) {
  return plan.start + coordinated_profile_progress(profile, elapsed) *
                      (plan.target - plan.start);
}

float coordinated_velocity(const CoordinatedAxisPlan &plan,
                           const CoordinatedTrajectoryProfile &profile,
                           float elapsed) {
  return coordinated_profile_velocity(profile, elapsed) *
         (plan.target - plan.start);
}

float coordinated_acceleration(const CoordinatedAxisPlan &plan,
                               const CoordinatedTrajectoryProfile &profile,
                               float elapsed) {
  return coordinated_profile_acceleration(profile, elapsed) *
         (plan.target - plan.start);
}

float coordinated_position(const CoordinatedAxisPlan &plan,
                            float duration, float elapsed) {
  if (duration <= 0.0f) {
    return plan.target;
  }

  const float t = std::max(0.0f, std::min(duration, elapsed));
  const float distance = plan.target - plan.start;
  const float halfDuration = duration * 0.5f;
  const float acceleration = 4.0f * distance / (duration * duration);
  float displacement;

  if (t <= halfDuration) {
    displacement = 0.5f * acceleration * t * t;
  } else {
    const float remaining = duration - t;
    displacement = distance - 0.5f * acceleration * remaining * remaining;
  }

  return plan.start + displacement;
}

} // namespace limnmoco
