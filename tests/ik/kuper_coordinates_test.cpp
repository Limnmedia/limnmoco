// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(KuperCoordinates, ConvertsTrackAcrossTheSolverBoundary) { EXPECT_TRUE(limnmoco::test::kuper_track_convention()); }
TEST(KuperCoordinates, UsesNegativeRawTrackForNorthSouthCompensation) { EXPECT_TRUE(limnmoco::test::kuper_north_south_compensation()); }
