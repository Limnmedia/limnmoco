// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_BOOM_COMPENSATION_H_
#define LIMNMOCO_BOOM_COMPENSATION_H_

namespace limnmoco {

constexpr int kBoomCompensationMinDegrees = -60;
constexpr int kBoomCompensationMaxDegrees = 60;
constexpr int kBoomCompensationEntryCount =
    kBoomCompensationMaxDegrees - kBoomCompensationMinDegrees + 1;

// Maps an ideal geometric boom angle to the configured boom motor's position
// units. Dragonframe transmits these motor-unit positions in VIRT_SCALE fixed
// point form in MSG_VIRT_CONFIG.
struct BoomCompensationTable {
  float motorUnits[kBoomCompensationEntryCount];
};

// A compensation table must be strictly monotonic so that the motor-unit to
// geometric-angle lookup is unambiguous. Both increasing and decreasing motor
// coordinates are valid.
bool boom_compensation_table_is_valid(const BoomCompensationTable &table);

// Piecewise-linear forward and inverse table lookups. Both return false when
// the requested value lies outside the table range.
bool boom_angle_to_motor_units(const BoomCompensationTable &table,
                               float boomDegrees, float *motorUnits);
bool boom_motor_units_to_angle(const BoomCompensationTable &table,
                               float motorUnits, float *boomDegrees);

} // namespace limnmoco

#endif // LIMNMOCO_BOOM_COMPENSATION_H_
