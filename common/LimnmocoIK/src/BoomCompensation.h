// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_BOOM_COMPENSATION_H_
#define LIMNMOCO_BOOM_COMPENSATION_H_

namespace limnmoco {

constexpr int kBoomCompensationMinDegrees = -60;
constexpr int kBoomCompensationMaxDegrees = 60;
constexpr int kBoomCompensationEntryCount =
    kBoomCompensationMaxDegrees - kBoomCompensationMinDegrees + 1;

// Maps an ideal geometric boom angle to the configured boom motor's absolute
// step position. Dragonframe interpolates these signed step positions directly
// when it creates real-time move-axis targets, so SPU must not be applied to a
// value read from this table.
struct BoomCompensationTable {
  float motorSteps[kBoomCompensationEntryCount];
};

// A compensation table must be strictly monotonic so that the motor-step to
// geometric-angle lookup is unambiguous. Both increasing and decreasing motor
// coordinates are valid.
bool boom_compensation_table_is_valid(const BoomCompensationTable &table);

// Piecewise-linear forward and inverse table lookups. Both return false when
// the requested value lies outside the table range.
bool boom_angle_to_steps(const BoomCompensationTable &table,
                         float boomDegrees, float *motorSteps);
bool boom_steps_to_angle(const BoomCompensationTable &table,
                         float motorSteps, float *boomDegrees);

} // namespace limnmoco

#endif // LIMNMOCO_BOOM_COMPENSATION_H_
