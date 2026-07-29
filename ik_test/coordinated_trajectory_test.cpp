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
  using limnmoco::coordinated_minimum_duration;
  using limnmoco::coordinated_move_duration;
  using limnmoco::coordinated_position;

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

  std::cout << "All coordinated trajectory tests passed.\n";
  return EXIT_SUCCESS;
}
