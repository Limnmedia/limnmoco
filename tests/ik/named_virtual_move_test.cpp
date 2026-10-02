// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"
#include "named_move_fixture.h"
#include "test_support.h"

#include <BoomCompensation.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

namespace {

limnmoco::CraneGeometry geometry_for(const limnmoco::test::NamedVirtualMoveCase &move) {
  return {move.boomLengthMm, move.extensionLengthMm, move.nodalOffsetXmm,
          move.nodalOffsetYmm, move.nodalOffsetZmm};
}

limnmoco::CranePositions positions_for(
    const limnmoco::test::NamedMovePhysicalPose &pose) {
  return {pose.boomDeg, pose.swingDeg, pose.trackMm, pose.panDeg,
          pose.tiltDeg, pose.rollDeg};
}

limnmoco::VirtualPose target_for(const limnmoco::test::NamedVirtualMoveCase &move) {
  return {move.targetVtrackMm, move.targetVewMm, move.targetVnsMm,
          move.targetVpanDeg, move.targetVtiltDeg, move.targetVrollDeg};
}

limnmoco::test::NamedMovePhysicalPose solve_target(
    const limnmoco::VirtualPose &target, const limnmoco::CraneGeometry &geometry) {
  const limnmoco::CraneSolveResult result =
      limnmoco::solveLimnmocoCrane(target, geometry);
  return {result.boomDeg, result.swingDeg, result.track,
          target.vpanDeg - result.swingDeg, target.vtiltDeg, target.vrollDeg};
}

void expect_physical_near(const limnmoco::test::NamedMovePhysicalPose &actual,
                          const limnmoco::test::NamedMovePhysicalPose &expected,
                          const limnmoco::test::NamedVirtualMoveCase &move,
                          const char *phase) {
  SCOPED_TRACE(move.name + " " + phase);
  EXPECT_NEAR(actual.boomDeg, expected.boomDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.swingDeg, expected.swingDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.trackMm, expected.trackMm, move.translationToleranceMm);
  EXPECT_NEAR(actual.panDeg, expected.panDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.tiltDeg, expected.tiltDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.rollDeg, expected.rollDeg, move.rotationToleranceDeg);
}

void expect_virtual_near(const limnmoco::VirtualPose &actual,
                         const limnmoco::VirtualPose &expected,
                         const limnmoco::test::NamedVirtualMoveCase &move,
                         const char *phase) {
  SCOPED_TRACE(move.name + " " + phase);
  EXPECT_NEAR(actual.vtrack, expected.vtrack, move.translationToleranceMm);
  EXPECT_NEAR(actual.vew, expected.vew, move.translationToleranceMm);
  EXPECT_NEAR(actual.vheight, expected.vheight, move.translationToleranceMm);
  EXPECT_NEAR(actual.vpanDeg, expected.vpanDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.vtiltDeg, expected.vtiltDeg, move.rotationToleranceDeg);
  EXPECT_NEAR(actual.vrollDeg, expected.vrollDeg, move.rotationToleranceDeg);
}

void expect_boom_compensation_near(
    const limnmoco::test::NamedVirtualMoveCase &move,
    const limnmoco::test::NamedMovePhysicalPose &physical) {
  if (!move.boomCompensationEnabled) {
    return;
  }
  limnmoco::BoomCompensationTable table{};
  for (int index = 0; index < limnmoco::kBoomCompensationEntryCount; ++index) {
    const float angle = limnmoco::kBoomCompensationMinDegrees + index;
    table.motorSteps[index] = angle * move.boomCompensationStepsPerDegree;
  }
  ASSERT_TRUE(limnmoco::boom_compensation_table_is_valid(table));
  float motor_steps = 0.0f;
  float recovered_boom = 0.0f;
  ASSERT_TRUE(limnmoco::boom_angle_to_steps(table, physical.boomDeg, &motor_steps));
  EXPECT_NEAR(motor_steps, move.expectedBoomMotorSteps,
              move.rotationToleranceDeg * move.boomCompensationStepsPerDegree);
  ASSERT_TRUE(limnmoco::boom_steps_to_angle(table, motor_steps, &recovered_boom));
  EXPECT_NEAR(recovered_boom, physical.boomDeg, move.rotationToleranceDeg);
}

} // namespace

TEST(NamedVirtualMoves, PreserveNamedMoveRoundTrips) {
  std::ifstream file(limnmoco::test::fixture_path("basic_virtual_moves.csv"));
  ASSERT_TRUE(file.is_open());
  std::vector<limnmoco::test::NamedVirtualMoveCase> cases;
  std::string error;
  ASSERT_TRUE(limnmoco::test::read_named_move_fixture(file, &cases, &error)) << error;
  ASSERT_EQ(cases.size(), 11u);

  for (const limnmoco::test::NamedVirtualMoveCase &move : cases) {
    SCOPED_TRACE(move.name);
    const limnmoco::CraneGeometry geometry = geometry_for(move);
    const limnmoco::VirtualPose start_virtual =
        limnmoco::solveForwardKinematics(positions_for(move.startingPhysical), geometry);
    const limnmoco::test::NamedMovePhysicalPose start_round_trip =
        solve_target(start_virtual, geometry);
    expect_physical_near(start_round_trip, move.startingPhysical, move,
                         "physical-to-virtual-to-physical");

    const limnmoco::VirtualPose target = target_for(move);
    limnmoco::test::NamedMovePhysicalPose physical = solve_target(target, geometry);
    expect_physical_near(physical, move.expectedPhysical, move,
                         "virtual-target-to-physical");
    expect_boom_compensation_near(move, physical);
    if (!move.rollPresent) {
      EXPECT_FLOAT_EQ(target.vrollDeg, 0.0f);
      EXPECT_FLOAT_EQ(physical.rollDeg, 0.0f);
    }

    for (uint32_t iteration = 0; iteration < move.repeatedRoundTrips; ++iteration) {
      const limnmoco::VirtualPose reconstructed =
          limnmoco::solveForwardKinematics(positions_for(physical), geometry);
      expect_virtual_near(reconstructed, target, move, "repeated-physical-to-virtual");
      physical = solve_target(reconstructed, geometry);
      expect_physical_near(physical, move.expectedPhysical, move,
                           "repeated-virtual-to-physical");
    }
  }
}
