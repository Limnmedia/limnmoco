// SPDX-License-Identifier: BSD-3-Clause

#include "named_move_fixture.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace limnmoco::test {
namespace {

constexpr char kHeader[] =
    "name,category,boom_length_mm,extension_length_mm,nodal_offset_x_mm,"
    "nodal_offset_y_mm,nodal_offset_z_mm,roll_present,bct_enabled,"
    "bct_steps_per_degree,expected_boom_motor_steps,round_trip_count,"
    "start_boom_deg,start_swing_deg,start_track_mm,start_pan_deg,"
    "start_tilt_deg,start_roll_deg,target_vtrack_mm,target_vew_mm,"
    "target_vns_mm,target_vpan_deg,target_vtilt_deg,target_vroll_deg,"
    "expected_boom_deg,expected_swing_deg,expected_track_mm,"
    "expected_pan_deg,expected_tilt_deg,expected_roll_deg,"
    "translation_tolerance_mm,rotation_tolerance_deg";
constexpr std::size_t kFieldCount = 32;

std::vector<std::string> split_csv(const std::string &line) {
  std::vector<std::string> fields;
  std::stringstream stream(line);
  std::string field;
  while (std::getline(stream, field, ',')) {
    fields.push_back(field);
  }
  return fields;
}

bool parse_bool(const std::string &field, bool *value) {
  if (field == "0") {
    *value = false;
    return true;
  }
  if (field == "1") {
    *value = true;
    return true;
  }
  return false;
}

bool parse_float(const std::string &field, float *value) {
  std::size_t parsed = 0;
  try {
    *value = std::stof(field, &parsed);
  } catch (const std::exception &) {
    return false;
  }
  return parsed == field.size() && std::isfinite(*value);
}

bool parse_round_trip_count(const std::string &field, uint32_t *value) {
  std::size_t parsed = 0;
  try {
    const unsigned long count = std::stoul(field, &parsed);
    if (parsed != field.size() || count == 0 ||
        count > std::numeric_limits<uint32_t>::max()) {
      return false;
    }
    *value = static_cast<uint32_t>(count);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

bool parse_pose(const std::vector<std::string> &fields, std::size_t start,
                NamedMovePhysicalPose *pose) {
  return parse_float(fields[start], &pose->boomDeg) &&
         parse_float(fields[start + 1], &pose->swingDeg) &&
         parse_float(fields[start + 2], &pose->trackMm) &&
         parse_float(fields[start + 3], &pose->panDeg) &&
         parse_float(fields[start + 4], &pose->tiltDeg) &&
         parse_float(fields[start + 5], &pose->rollDeg);
}

} // namespace

bool read_named_move_fixture(std::istream &input,
                             std::vector<NamedVirtualMoveCase> *cases,
                             std::string *error) {
  if (cases == nullptr || error == nullptr) {
    return false;
  }

  cases->clear();
  error->clear();
  std::string line;
  if (!std::getline(input, line)) {
    *error = "fixture is empty";
    return false;
  }
  if (line != kHeader) {
    *error = "fixture header does not match the named-move schema";
    return false;
  }

  std::size_t line_number = 1;
  while (std::getline(input, line)) {
    ++line_number;
    if (line.empty()) {
      continue;
    }
    const std::vector<std::string> fields = split_csv(line);
    if (fields.size() != kFieldCount) {
      *error = "line " + std::to_string(line_number) + " has " +
          std::to_string(fields.size()) + " fields; expected " +
          std::to_string(kFieldCount);
      cases->clear();
      return false;
    }
    if (fields[0].empty() || fields[1].empty()) {
      *error = "line " + std::to_string(line_number) +
          " requires a name and category";
      cases->clear();
      return false;
    }

    NamedVirtualMoveCase move{fields[0], fields[1]};
    const bool valid =
        parse_float(fields[2], &move.boomLengthMm) &&
        parse_float(fields[3], &move.extensionLengthMm) &&
        parse_float(fields[4], &move.nodalOffsetXmm) &&
        parse_float(fields[5], &move.nodalOffsetYmm) &&
        parse_float(fields[6], &move.nodalOffsetZmm) &&
        parse_bool(fields[7], &move.rollPresent) &&
        parse_bool(fields[8], &move.boomCompensationEnabled) &&
        parse_float(fields[9], &move.boomCompensationStepsPerDegree) &&
        parse_float(fields[10], &move.expectedBoomMotorSteps) &&
        parse_round_trip_count(fields[11], &move.repeatedRoundTrips) &&
        parse_pose(fields, 12, &move.startingPhysical) &&
        parse_float(fields[18], &move.targetVtrackMm) &&
        parse_float(fields[19], &move.targetVewMm) &&
        parse_float(fields[20], &move.targetVnsMm) &&
        parse_float(fields[21], &move.targetVpanDeg) &&
        parse_float(fields[22], &move.targetVtiltDeg) &&
        parse_float(fields[23], &move.targetVrollDeg) &&
        parse_pose(fields, 24, &move.expectedPhysical) &&
        parse_float(fields[30], &move.translationToleranceMm) &&
        parse_float(fields[31], &move.rotationToleranceDeg);
    if (!valid || move.boomLengthMm <= 0.0f || move.extensionLengthMm < 0.0f ||
        move.translationToleranceMm <= 0.0f || move.rotationToleranceDeg <= 0.0f ||
        (move.boomCompensationEnabled &&
         move.boomCompensationStepsPerDegree <= 0.0f) ||
        (!move.boomCompensationEnabled &&
         (move.boomCompensationStepsPerDegree != 0.0f ||
          move.expectedBoomMotorSteps != 0.0f))) {
      *error = "line " + std::to_string(line_number) +
          " contains invalid named-move values";
      cases->clear();
      return false;
    }
    cases->push_back(move);
  }
  if (cases->empty()) {
    *error = "fixture contains no move cases";
    return false;
  }
  return true;
}

} // namespace limnmoco::test
