// SPDX-License-Identifier: BSD-3-Clause

#include "named_move_fixture.h"
#include "test_support.h"

#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <vector>

namespace {

TEST(NamedMoveFixture, ParsesVersionedFixtureCases) {
  std::ifstream file(limnmoco::test::fixture_path("named_move_fixture_reader.csv"));
  ASSERT_TRUE(file.is_open());
  std::vector<limnmoco::test::NamedVirtualMoveCase> cases;
  std::string error;
  ASSERT_TRUE(limnmoco::test::read_named_move_fixture(file, &cases, &error)) << error;
  ASSERT_EQ(cases.size(), 2u);
  EXPECT_EQ(cases[0].name, "reader_zero_pose");
  EXPECT_EQ(cases[0].category, "reader");
  EXPECT_TRUE(cases[0].rollPresent);
  EXPECT_FALSE(cases[0].boomCompensationEnabled);
  EXPECT_EQ(cases[0].repeatedRoundTrips, 8u);
  EXPECT_NEAR(cases[0].boomLengthMm, 857.7f, 0.0001f);
  EXPECT_NEAR(cases[1].nodalOffsetZmm, 0.3179f, 0.0001f);
  EXPECT_FALSE(cases[1].rollPresent);
}

TEST(NamedMoveFixture, RejectsMalformedHeadersAndRecordsTransactionally) {
  std::istringstream input(
      "name,category,boom_length_mm\n"
      "bad,reader,857.7\n");
  std::vector<limnmoco::test::NamedVirtualMoveCase> cases{{}};
  std::string error;
  EXPECT_FALSE(limnmoco::test::read_named_move_fixture(input, &cases, &error));
  EXPECT_TRUE(cases.empty());
  EXPECT_NE(error.find("header"), std::string::npos);

  std::ifstream fixture(limnmoco::test::fixture_path("named_move_fixture_reader.csv"));
  ASSERT_TRUE(fixture.is_open());
  std::string header;
  std::string valid_record;
  ASSERT_TRUE(std::getline(fixture, header));
  ASSERT_TRUE(std::getline(fixture, valid_record));
  const std::size_t roll_present = valid_record.find(",1,0,8,");
  ASSERT_NE(roll_present, std::string::npos);
  valid_record.replace(roll_present + 1, 1, "not-a-bool");
  std::istringstream invalid_record(header + "\n" + valid_record + "\n");
  cases.push_back({});
  EXPECT_FALSE(limnmoco::test::read_named_move_fixture(
      invalid_record, &cases, &error));
  EXPECT_TRUE(cases.empty());
  EXPECT_NE(error.find("invalid"), std::string::npos);
}

} // namespace
