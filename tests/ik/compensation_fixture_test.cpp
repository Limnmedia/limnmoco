// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(CsvFixtures, LinearDisplacements) { EXPECT_TRUE(limnmoco::test::compensation_fixture_linear()); }
TEST(CsvFixtures, RotationsWithOffsets) { EXPECT_TRUE(limnmoco::test::compensation_fixture_rotations_with_offsets()); }
TEST(CsvFixtures, RotationsWithoutOffsets) { EXPECT_TRUE(limnmoco::test::compensation_fixture_rotations_without_offsets()); }
