// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include "test_support.h"

TEST(GoogleTestIntegration, IsAvailableToHostTests) {
  EXPECT_EQ(1 + 1, 2);
}

TEST(TestSupport, ProvidesAnAbsoluteFixturePath) {
  EXPECT_NE(limnmoco::test::fixture_path("fixture.csv").find("fixture.csv"),
            std::string::npos);
}
