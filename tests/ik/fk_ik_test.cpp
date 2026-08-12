// SPDX-License-Identifier: BSD-3-Clause

#include "ik_test_cases.h"
#include <gtest/gtest.h>

TEST(IkForwardKinematics, NormalizesTheConfigurationOrigin) { EXPECT_TRUE(limnmoco::test::fk_origin_normalization()); }
TEST(IkVirtualAxes, EastWestCompensatesTrackWithoutNorthSouthMotion) { EXPECT_TRUE(limnmoco::test::east_west_compensation()); }
TEST(IkSolver, MatchesTheBlenderDefaultPanelValues) { EXPECT_TRUE(limnmoco::test::blender_default_panel()); }
TEST(IkSolver, CentersAnUnrotatedTargetWithoutOffsets) { EXPECT_TRUE(limnmoco::test::centered_target()); }
TEST(IkSolver, ReconstructsARotatedNodalOffset) { EXPECT_TRUE(limnmoco::test::rotated_nodal_offset()); }
