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

float coordinated_minimum_duration(float distance, float maxVelocity,
                                   float maxAcceleration);

float coordinated_move_duration(const CoordinatedAxisPlan *plans,
                                int axisCount);

float coordinated_position(const CoordinatedAxisPlan &plan,
                            float duration, float elapsed);

} // namespace limnmoco

#endif // LIMNMOCO_COORDINATED_TRAJECTORY_H_
