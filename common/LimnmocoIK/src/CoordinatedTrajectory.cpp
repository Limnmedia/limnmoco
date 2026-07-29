// SPDX-License-Identifier: BSD-3-Clause

#include "CoordinatedTrajectory.h"

#include <algorithm>
#include <cmath>

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
