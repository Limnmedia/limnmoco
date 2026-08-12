// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_IK_TEST_CASES_H_
#define LIMNMOCO_IK_TEST_CASES_H_

namespace limnmoco::test {

bool fk_origin_normalization();
bool east_west_compensation();
bool boom_table_mapping();
bool boom_signed_step_encoding();
bool boom_step_target_mapping();
bool kuper_track_convention();
bool kuper_north_south_compensation();
bool camera_line_target();
bool camera_line_direction();
bool aim_geometry();
bool aim_aware_target();
bool aim_point_protocol();
bool compensation_fixture_linear();
bool compensation_fixture_rotations_with_offsets();
bool compensation_fixture_rotations_without_offsets();
bool blender_default_panel();
bool centered_target();
bool rotated_nodal_offset();

} // namespace limnmoco::test

#endif // LIMNMOCO_IK_TEST_CASES_H_
