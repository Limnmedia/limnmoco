// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_COORDINATED_TRAJECTORY_H_
#define LIMNMOCO_COORDINATED_TRAJECTORY_H_

namespace limnmoco {

struct CoordinatedAxisPlan {
  float start;
  float target;
  float maxVelocity;
  float maxAcceleration;
};

// A single normalized 0..1 trajectory shared by every axis in a coordinated
// move. Axis position is start + progress * (target - start).
struct CoordinatedTrajectoryProfile {
  float duration;
  float accelerationTime;
  float cruiseTime;
  float peakProgressVelocity;
  float progressAcceleration;
  // Non-zero only for a validated in-flight handoff.  It is expressed in the
  // shared normalized 0..1 progress coordinate.
  float initialProgressVelocity;
};

float coordinated_minimum_duration(float distance, float maxVelocity,
                                   float maxAcceleration);

float coordinated_move_duration(const CoordinatedAxisPlan *plans,
                                int axisCount);

CoordinatedTrajectoryProfile coordinated_make_profile(
    const CoordinatedAxisPlan *plans, int axisCount);

// Builds a profile that starts at initialProgressVelocity and ends at rest.
// The caller must establish that all physical axes share this normalized
// velocity and that it points toward their new targets.
CoordinatedTrajectoryProfile coordinated_make_handoff_profile(
    const CoordinatedAxisPlan *plans, int axisCount,
    float initialProgressVelocity);

float coordinated_profile_progress(const CoordinatedTrajectoryProfile &profile,
                                   float elapsed);
float coordinated_profile_velocity(const CoordinatedTrajectoryProfile &profile,
                                   float elapsed);
float coordinated_profile_acceleration(
    const CoordinatedTrajectoryProfile &profile, float elapsed);

float coordinated_position(const CoordinatedAxisPlan &plan,
                           const CoordinatedTrajectoryProfile &profile,
                           float elapsed);
float coordinated_velocity(const CoordinatedAxisPlan &plan,
                           const CoordinatedTrajectoryProfile &profile,
                           float elapsed);
float coordinated_acceleration(const CoordinatedAxisPlan &plan,
                               const CoordinatedTrajectoryProfile &profile,
                               float elapsed);

// Legacy duration-only evaluator. New coordinated motion must use the shared
// CoordinatedTrajectoryProfile overload above so velocity and acceleration
// limits are honored.
float coordinated_position(const CoordinatedAxisPlan &plan,
                            float duration, float elapsed);

} // namespace limnmoco

#endif // LIMNMOCO_COORDINATED_TRAJECTORY_H_
