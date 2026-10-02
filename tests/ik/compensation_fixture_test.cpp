// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"
#include "test_support.h"

#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> split_csv(const std::string &line) {
  std::vector<std::string> fields;
  std::stringstream stream(line);
  std::string field;
  while (std::getline(stream, field, ',')) {
    fields.push_back(field);
  }
  return fields;
}

void expect_compensation_fixture(const std::string &name,
                                 const limnmoco::CraneGeometry &geometry,
                                 bool rotational_fixture,
                                 std::size_t expected_rows) {
  std::ifstream file(limnmoco::test::fixture_path(name));
  ASSERT_TRUE(file.is_open()) << name;
  std::string line;
  std::getline(file, line);
  const limnmoco::CraneSolveResult baseline =
      limnmoco::solveLimnmocoCrane(limnmoco::VirtualPose{}, geometry);
  const float baseline_pan = -baseline.swingDeg;
  std::size_t row_count = 0;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    ++row_count;
    const std::vector<std::string> fields = split_csv(line);
    const std::size_t field_count = rotational_fixture ? 17 : 14;
    ASSERT_EQ(fields.size(), field_count) << name << " row " << row_count;
    const auto value = [&fields](std::size_t index) { return std::stof(fields[index]); };
    const limnmoco::VirtualPose pose{value(4), value(2), value(3),
        rotational_fixture ? value(5) : 0.0f,
        rotational_fixture ? value(6) : 0.0f,
        rotational_fixture ? value(7) : 0.0f};
    const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(pose, geometry);
    const std::size_t expected_start = rotational_fixture ? 8 : 5;
    const float actual[] = {result.boomDeg - baseline.boomDeg,
        result.swingDeg - baseline.swingDeg, result.track - baseline.track,
        (pose.vpanDeg - result.swingDeg) - baseline_pan, pose.vtiltDeg, pose.vrollDeg};
    for (std::size_t axis = 0; axis < 6; ++axis) {
      EXPECT_NEAR(actual[axis], value(expected_start + axis),
                  rotational_fixture ? 0.0002f : 0.0001f)
          << name << " row " << row_count << " axis " << axis;
    }
    EXPECT_EQ(result.boomClamped, value(rotational_fixture ? 14 : 11) != 0.0f)
        << name << " row " << row_count;
    EXPECT_EQ(result.swingClamped, value(rotational_fixture ? 15 : 12) != 0.0f)
        << name << " row " << row_count;
    EXPECT_NEAR(result.errorLength, value(rotational_fixture ? 16 : 13), 0.0001f)
        << name << " row " << row_count;
  }
  EXPECT_EQ(row_count, expected_rows) << name;
}

} // namespace

TEST(CsvFixtures, LinearDisplacements) {
  expect_compensation_fixture("expected_virtual_displacements.csv",
      {857.7f, 78.0f, 0.0f, 0.0f, 0.0f}, false, 63);
}
TEST(CsvFixtures, RotationsWithOffsets) {
  expect_compensation_fixture("expected_virtual_rotation_displacements_with_offsets.csv",
      {857.7f, 78.0f, -0.2727f, -0.1463f, 0.3179f}, true, 57);
}
TEST(CsvFixtures, RotationsWithoutOffsets) {
  expect_compensation_fixture("expected_virtual_rotation_displacements.csv",
      {857.7f, 78.0f, 0.0f, 0.0f, 0.0f}, true, 57);
}
