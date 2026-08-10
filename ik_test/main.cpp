// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <AimGeometry.h>
#include <BoomCompensation.h>
#include <CameraLine.h>
#include <KuperTrackConvention.h>

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct TestCase {
  std::string name;
  limnmoco::VirtualPose pose;
  limnmoco::CraneGeometry geometry;
  float expectedTrack;
  float expectedSwingDeg;
  float expectedBoomDeg;
  bool checkExactAxes;
  bool expectClamp;
};

bool near(float actual, float expected, float tolerance) {
  return std::abs(actual - expected) <= tolerance;
}

bool runBoomCompensationTableTest() {
  limnmoco::BoomCompensationTable increasing{};
  limnmoco::BoomCompensationTable decreasing{};
  for (int index = 0; index < limnmoco::kBoomCompensationEntryCount; ++index) {
    const float angle = limnmoco::kBoomCompensationMinDegrees + index;
    increasing.motorSteps[index] = (angle * 0.8f +
        (angle >= 0.0f ? angle * angle * 0.001f : -angle * angle * 0.001f)) *
        10000.0f;
    decreasing.motorSteps[index] = -increasing.motorSteps[index];
  }

  bool ok = limnmoco::boom_compensation_table_is_valid(increasing) &&
            limnmoco::boom_compensation_table_is_valid(decreasing);
  float motorSteps = 0.0f;
  float angle = 0.0f;
  ok = ok && limnmoco::boom_angle_to_steps(increasing, 10.5f, &motorSteps) &&
       near(motorSteps, 85105.0f, 0.01f) &&
       limnmoco::boom_steps_to_angle(increasing, motorSteps, &angle) &&
       near(angle, 10.5f, 0.00001f) &&
       limnmoco::boom_angle_to_steps(decreasing, -12.25f, &motorSteps) &&
       limnmoco::boom_steps_to_angle(decreasing, motorSteps, &angle) &&
       near(angle, -12.25f, 0.00001f) &&
       !limnmoco::boom_angle_to_steps(increasing, 60.1f, &motorSteps) &&
       !limnmoco::boom_steps_to_angle(increasing, 10000000.0f, &angle);

  increasing.motorSteps[1] = increasing.motorSteps[0];
  ok = ok && !limnmoco::boom_compensation_table_is_valid(increasing);

  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Boom compensation table mapping\n";
  return ok;
}

bool runDragonframeBoomCompensationEncodingTest() {
  // Captured MSG_VIRT_CONFIG entries are signed physical boom step positions.
  // -60 degrees is 0xffafdd92 (signed -5,251,694), zero is zero,
  // and +1 degree is 85,557 steps.
  const uint32_t rawNegativeSixty = 4289715602u;
  const float negativeSixtySteps =
      static_cast<float>(static_cast<int32_t>(rawNegativeSixty));
  const float positiveOneSteps = 85557.0f;
  const bool ok = near(negativeSixtySteps, -5251694.0f, 0.1f) &&
                  near(positiveOneSteps, 85557.0f, 0.1f);
  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Dragonframe boom table step encoding\n";
  return ok;
}

bool runBoomCompensationStepTargetRegressionTest() {
  limnmoco::BoomCompensationTable table{};
  for (int index = 0; index < limnmoco::kBoomCompensationEntryCount; ++index) {
    const float angle = limnmoco::kBoomCompensationMinDegrees + index;
    table.motorSteps[index] = angle * 8600.0f;
  }

  // A correctly scaled version of the captured table's +3 and +4 degree
  // entries. Dragonframe must interpolate this directly to a motor step
  // target; applying VIRT_SCALE or SPU again would create a 10x-scale error.
  table.motorSteps[63] = 27620.3f; // +3 degrees
  table.motorSteps[64] = 37405.6f; // +4 degrees

  float targetSteps = 0.0f;
  float targetDegrees = 0.0f;
  const bool ok = limnmoco::boom_angle_to_steps(
                      table, 3.342f, &targetSteps) &&
                  near(targetSteps, 30966.87f, 0.1f) &&
                  limnmoco::boom_steps_to_angle(
                      table, targetSteps, &targetDegrees) &&
                  near(targetDegrees, 3.342f, 0.0001f);
  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " BCT interpolation produces physical boom steps\n";
  return ok;
}

bool runKuperTrackConventionTest() {
  const limnmoco::VirtualPose kuperPose{
      -5.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
  const limnmoco::VirtualPose solverPose =
      limnmoco::kuper_pose_to_solver(kuperPose);
  const limnmoco::VirtualPose roundTrip =
      limnmoco::solver_pose_to_kuper(solverPose);

  const bool ok =
      near(limnmoco::kuper_track_to_solver(-5.0f), 5.0f, 0.0f) &&
      near(limnmoco::solver_track_to_kuper(5.0f), -5.0f, 0.0f) &&
      near(solverPose.vtrack, 5.0f, 0.0f) &&
      near(solverPose.vew, kuperPose.vew, 0.0f) &&
      near(solverPose.vheight, kuperPose.vheight, 0.0f) &&
      near(solverPose.vpanDeg, kuperPose.vpanDeg, 0.0f) &&
      near(solverPose.vtiltDeg, kuperPose.vtiltDeg, 0.0f) &&
      near(solverPose.vrollDeg, kuperPose.vrollDeg, 0.0f) &&
      near(roundTrip.vtrack, kuperPose.vtrack, 0.0f) &&
      near(roundTrip.vew, kuperPose.vew, 0.0f) &&
      near(roundTrip.vheight, kuperPose.vheight, 0.0f) &&
      near(roundTrip.vpanDeg, kuperPose.vpanDeg, 0.0f) &&
      near(roundTrip.vtiltDeg, kuperPose.vtiltDeg, 0.0f) &&
      near(roundTrip.vrollDeg, kuperPose.vrollDeg, 0.0f);
  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Kuper track coordinate convention\n";
  return ok;
}

bool runKuperNorthSouthCompensationTest() {
  const limnmoco::CraneGeometry geometry{
      857.7f, 78.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose origin = limnmoco::solve_fk(
      0.0f, 0.0f, limnmoco::kuper_track_to_solver(0.0f),
      0.0f, 0.0f, 0.0f, geometry);
  const limnmoco::VirtualPose publicTarget{
      0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f};
  limnmoco::VirtualPose solverTarget =
      limnmoco::kuper_pose_to_solver(publicTarget);
  solverTarget.vtrack += origin.vtrack;
  const limnmoco::CraneSolveResult result =
      limnmoco::solve_ik(solverTarget, geometry);
  const float rawTrack = limnmoco::solver_track_to_kuper(result.track);

  const bool ok = near(result.track, 5.8494f, 0.001f) &&
                  near(rawTrack, -5.8494f, 0.001f) &&
                  near(limnmoco::solver_track_to_kuper(
                         limnmoco::kuper_track_to_solver(-5.0f)),
                       -5.0f, 0.0f);
  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Kuper NS compensation commands negative raw track\n";
  return ok;
}

bool runCameraLineDirectionTest() {
  using limnmoco::CameraLineAxis;
  using limnmoco::VirtualTranslationDelta;

  VirtualTranslationDelta x{};
  VirtualTranslationDelta y{};
  VirtualTranslationDelta z{};
  VirtualTranslationDelta forward{};
  VirtualTranslationDelta panRight{};
  VirtualTranslationDelta panLeft{};
  VirtualTranslationDelta tiltUp{};
  VirtualTranslationDelta tiltDown{};
  VirtualTranslationDelta rolledX{};
  VirtualTranslationDelta combined{};

  bool ok =
      limnmoco::camera_line_direction(CameraLineAxis::kX,
                                      0.0f, 0.0f, 0.0f, &x) &&
      limnmoco::camera_line_direction(CameraLineAxis::kY,
                                      0.0f, 0.0f, 0.0f, &y) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      0.0f, 0.0f, 0.0f, &z) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      0.0f, 0.0f, 0.0f, &forward, -1.0f) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      90.0f, 0.0f, 0.0f, &panRight) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      -90.0f, 0.0f, 0.0f, &panLeft) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      0.0f, 90.0f, 0.0f, &tiltUp) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      0.0f, -90.0f, 0.0f, &tiltDown) &&
      limnmoco::camera_line_direction(CameraLineAxis::kX,
                                      0.0f, 0.0f, 90.0f, &rolledX) &&
      limnmoco::camera_line_direction(CameraLineAxis::kY,
                                      20.0f, -15.0f, 30.0f, &combined);

  const float combinedLength = std::sqrt(
      combined.vew * combined.vew + combined.vns * combined.vns +
      combined.vtrack * combined.vtrack);
  ok = ok &&
      near(x.vew, 1.0f, 0.0001f) && near(x.vns, 0.0f, 0.0001f) &&
      near(x.vtrack, 0.0f, 0.0001f) &&
      near(y.vew, 0.0f, 0.0001f) && near(y.vns, 1.0f, 0.0001f) &&
      near(y.vtrack, 0.0f, 0.0001f) &&
      near(z.vew, 0.0f, 0.0001f) && near(z.vns, 0.0f, 0.0001f) &&
      near(z.vtrack, 1.0f, 0.0001f) &&
      near(forward.vew, 0.0f, 0.0001f) &&
      near(forward.vns, 0.0f, 0.0001f) &&
      near(forward.vtrack, -1.0f, 0.0001f) &&
      near(panRight.vew, 1.0f, 0.0001f) &&
      near(panRight.vns, 0.0f, 0.0001f) &&
      near(panRight.vtrack, 0.0f, 0.0001f) &&
      near(panLeft.vew, -1.0f, 0.0001f) &&
      near(panLeft.vns, 0.0f, 0.0001f) &&
      near(panLeft.vtrack, 0.0f, 0.0001f) &&
      near(tiltUp.vew, 0.0f, 0.0001f) && near(tiltUp.vns, -1.0f, 0.0001f) &&
      near(tiltUp.vtrack, 0.0f, 0.0001f) &&
      near(tiltDown.vew, 0.0f, 0.0001f) && near(tiltDown.vns, 1.0f, 0.0001f) &&
      near(tiltDown.vtrack, 0.0f, 0.0001f) &&
      near(rolledX.vew, 0.0f, 0.0001f) && near(rolledX.vns, -1.0f, 0.0001f) &&
      near(rolledX.vtrack, 0.0f, 0.0001f) &&
      near(combinedLength, 1.0f, 0.0001f);

  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Kuper camera-line direction mapping\n";
  return ok;
}

bool runAimGeometryTest() {
  using limnmoco::AimOrientation;
  using limnmoco::CameraLineAxis;
  using limnmoco::KuperPoint;
  using limnmoco::VirtualTranslationDelta;

  const KuperPoint nodal{0.0f, 0.0f, 0.0f};
  AimOrientation forward{};
  AimOrientation right{};
  AimOrientation left{};
  AimOrientation above{};
  AimOrientation below{};
  AimOrientation offset{};
  AimOrientation wrapped{};
  VirtualTranslationDelta aimForwardDirection{};
  VirtualTranslationDelta aimRightDirection{};
  VirtualTranslationDelta aimAboveDirection{};

  bool ok =
      limnmoco::aim_base_orientation(nodal, KuperPoint{0.0f, 0.0f, -10.0f},
                                     23.0f, &forward) &&
      limnmoco::aim_base_orientation(nodal, KuperPoint{10.0f, 0.0f, 0.0f},
                                     0.0f, &right) &&
      limnmoco::aim_base_orientation(nodal, KuperPoint{-10.0f, 0.0f, 0.0f},
                                     0.0f, &left) &&
      limnmoco::aim_base_orientation(nodal, KuperPoint{0.0f, 10.0f, -1.0f},
                                     0.0f, &above) &&
      limnmoco::aim_base_orientation(nodal, KuperPoint{0.0f, -10.0f, -1.0f},
                                     0.0f, &below) &&
      limnmoco::aim_apply_relative_offsets(forward, 20.0f, -10.0f, &offset) &&
      limnmoco::aim_apply_relative_offsets(
          AimOrientation{170.0f, 5.0f, 7.0f}, 20.0f, -2.0f, &wrapped) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      forward.vpanDeg, forward.vtiltDeg,
                                      forward.vrollDeg, &aimForwardDirection,
                                      -1.0f) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      right.vpanDeg, right.vtiltDeg,
                                      right.vrollDeg, &aimRightDirection,
                                      -1.0f) &&
      limnmoco::camera_line_direction(CameraLineAxis::kZ,
                                      above.vpanDeg, above.vtiltDeg,
                                      above.vrollDeg, &aimAboveDirection,
                                      -1.0f);

  ok = ok &&
      near(forward.vpanDeg, 0.0f, 0.0001f) &&
      near(forward.vtiltDeg, 0.0f, 0.0001f) &&
      near(forward.vrollDeg, 23.0f, 0.0001f) &&
      near(right.vpanDeg, -90.0f, 0.0001f) &&
      near(right.vtiltDeg, 0.0f, 0.0001f) &&
      near(left.vpanDeg, 90.0f, 0.0001f) &&
      near(left.vtiltDeg, 0.0f, 0.0001f) &&
      near(above.vpanDeg, 0.0f, 0.0001f) &&
      near(above.vtiltDeg, 84.2894f, 0.0001f) &&
      near(below.vpanDeg, 0.0f, 0.0001f) &&
      near(below.vtiltDeg, -84.2894f, 0.0001f) &&
      near(offset.vpanDeg, 20.0f, 0.0001f) &&
      near(offset.vtiltDeg, -10.0f, 0.0001f) &&
      near(offset.vrollDeg, 23.0f, 0.0001f) &&
      near(wrapped.vpanDeg, -170.0f, 0.0001f) &&
      near(wrapped.vtiltDeg, 3.0f, 0.0001f) &&
      near(wrapped.vrollDeg, 7.0f, 0.0001f) &&
      near(aimForwardDirection.vew, 0.0f, 0.0001f) &&
      near(aimForwardDirection.vns, 0.0f, 0.0001f) &&
      near(aimForwardDirection.vtrack, -1.0f, 0.0001f) &&
      near(aimRightDirection.vew, 1.0f, 0.0001f) &&
      near(aimRightDirection.vns, 0.0f, 0.0001f) &&
      near(aimRightDirection.vtrack, 0.0f, 0.0001f) &&
      near(aimAboveDirection.vew, 0.0f, 0.0001f) &&
      near(aimAboveDirection.vns, 0.9950f, 0.0001f) &&
      near(aimAboveDirection.vtrack, -0.0995f, 0.0001f) &&
      !limnmoco::aim_base_orientation(nodal, nodal, 0.0f, &forward) &&
      !limnmoco::aim_point_outside_safe_cylinder(
          KuperPoint{0.0f, 999.0f, 0.0f}, nodal, 10.0f) &&
      !limnmoco::aim_point_outside_safe_cylinder(
          KuperPoint{10.0f, 0.0f, 0.0f}, nodal, 10.0f) &&
      limnmoco::aim_point_outside_safe_cylinder(
          KuperPoint{10.001f, 0.0f, 0.0f}, nodal, 10.0f) &&
      !limnmoco::aim_point_outside_safe_cylinder(nodal, nodal, -1.0f);

  std::cout << (ok ? "[PASS]" : "[FAIL]")
            << " Aim point geometry and safe cylinder\n";
  return ok;
}

std::vector<std::string> splitCsv(const std::string &line) {
  std::vector<std::string> fields;
  std::stringstream stream(line);
  std::string field;
  while (std::getline(stream, field, ',')) {
    fields.push_back(field);
  }
  return fields;
}

bool openFixture(const std::string &name, std::ifstream *file) {
  file->open(name);
  if (file->is_open()) {
    return true;
  }

  file->clear();
  file->open("ik_test/" + name);
  return file->is_open();
}

bool runCompensationFixture(const std::string &name,
                            const limnmoco::CraneGeometry &geometry,
                            bool rotationalFixture,
                            std::size_t expectedRows) {
  std::ifstream file;
  if (!openFixture(name, &file)) {
    std::cerr << "[FAIL] Could not open IK fixture: " << name << "\n";
    return false;
  }

  std::string line;
  std::getline(file, line); // header
  const limnmoco::CraneSolveResult baseline =
      limnmoco::solveLimnmocoCrane(
          limnmoco::VirtualPose{0, 0, 0, 0, 0, 0}, geometry);
  const float baselinePan = -baseline.swingDeg;

  bool ok = true;
  std::size_t rowCount = 0;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    ++rowCount;
    const std::vector<std::string> fields = splitCsv(line);
    const std::size_t expectedFieldCount = rotationalFixture ? 17 : 14;
    if (fields.size() != expectedFieldCount) {
      std::cerr << "[FAIL] " << name << ": row " << rowCount
                << " has " << fields.size() << " fields, expected "
                << expectedFieldCount << "\n";
      ok = false;
      continue;
    }

    auto value = [&fields](std::size_t index) {
      return std::stof(fields[index]);
    };

    const limnmoco::VirtualPose pose{
        value(4), value(2), value(3),
        rotationalFixture ? value(5) : 0.0f,
        rotationalFixture ? value(6) : 0.0f,
        rotationalFixture ? value(7) : 0.0f};
    const limnmoco::CraneSolveResult result =
        limnmoco::solveLimnmocoCrane(pose, geometry);

    const std::size_t expectedStart = rotationalFixture ? 8 : 5;
    const float actualCompensations[] = {
        result.boomDeg - baseline.boomDeg,
        result.swingDeg - baseline.swingDeg,
        result.track - baseline.track,
        (pose.vpanDeg - result.swingDeg) - baselinePan,
        pose.vtiltDeg,
        pose.vrollDeg,
    };
    for (std::size_t axis = 0; axis < 6; ++axis) {
      if (!near(actualCompensations[axis], value(expectedStart + axis),
                rotationalFixture ? 0.0002f : 0.0001f)) {
        std::cerr << "[FAIL] " << name << ": row " << rowCount
                  << ", compensation column " << axis << "\n";
        ok = false;
      }
    }

    const bool expectedBoomClamp = value(rotationalFixture ? 14 : 11) != 0.0f;
    const bool expectedSwingClamp = value(rotationalFixture ? 15 : 12) != 0.0f;
    if (result.boomClamped != expectedBoomClamp ||
        result.swingClamped != expectedSwingClamp ||
        !near(result.errorLength, value(rotationalFixture ? 16 : 13),
              0.0001f)) {
      std::cerr << "[FAIL] " << name << ": row " << rowCount
                << ", solver status or reconstruction error\n";
      ok = false;
    }
  }

  if (rowCount != expectedRows) {
    std::cerr << "[FAIL] " << name << ": found " << rowCount
              << " rows, expected " << expectedRows << "\n";
    ok = false;
  }

  std::cout << (ok ? "[PASS] " : "[FAIL] ")
            << "CSV IK fixture: " << name << " (" << rowCount << " rows)\n";
  return ok;
}

bool runForwardKinematicsOriginTest() {
  const limnmoco::CraneGeometry geometry{
      857.7f,  // boomLength
      78.0f,   // extensionLength
      0.0f,    // offsetX
      0.0f,    // offsetY
      0.0f,    // offsetZ
  };
  const limnmoco::CranePositions zeroPositions{
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::CranePositions originPositions{
      0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose configuredOrigin{
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose zeroAbsolute =
      limnmoco::solveForwardKinematics(zeroPositions, geometry);
  const limnmoco::VirtualPose startingAbsolute =
      limnmoco::solveForwardKinematics(originPositions, geometry);

  const limnmoco::CranePositions movedPositions{
      0.0f, 0.0f, 12.6f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose movedAbsolute =
      limnmoco::solveForwardKinematics(movedPositions, geometry);
  const limnmoco::VirtualPose movedVirtual{
      configuredOrigin.vtrack + movedAbsolute.vtrack - startingAbsolute.vtrack,
      configuredOrigin.vew + movedAbsolute.vew - startingAbsolute.vew,
      configuredOrigin.vheight + movedAbsolute.vheight - startingAbsolute.vheight,
      configuredOrigin.vpanDeg + movedAbsolute.vpanDeg - startingAbsolute.vpanDeg,
      configuredOrigin.vtiltDeg + movedAbsolute.vtiltDeg - startingAbsolute.vtiltDeg,
      configuredOrigin.vrollDeg + movedAbsolute.vrollDeg - startingAbsolute.vrollDeg,
  };

  const limnmoco::VirtualPose directTrackTarget{
      zeroAbsolute.vtrack + 1.0f,
      zeroAbsolute.vew,
      zeroAbsolute.vheight,
      zeroAbsolute.vpanDeg,
      zeroAbsolute.vtiltDeg,
      zeroAbsolute.vrollDeg};
  const limnmoco::CraneSolveResult directTrackResult =
      limnmoco::solveLimnmocoCrane(directTrackTarget, geometry);

  const bool ok = near(zeroAbsolute.vtrack, 935.7f, 1e-4f) &&
                  near(configuredOrigin.vtrack, 0.0f, 1e-6f) &&
                  near(movedVirtual.vtrack, 12.5f, 1e-4f) &&
                  near(directTrackResult.track, 1.0f, 1e-4f);
  std::cout << (ok ? "[PASS] " : "[FAIL] ")
            << "FK origin normalization removes static boom reach\n";
  return ok;
}

bool runEastWestCompensationTest() {
  const limnmoco::CraneGeometry geometry{
      857.7f, 78.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::CranePositions currentPositions{
      0.0f, 2.5f, 0.0f, 0.0f, 0.0f, 0.0f};
  const limnmoco::VirtualPose current =
      limnmoco::solveForwardKinematics(currentPositions, geometry);
  const limnmoco::VirtualPose target{
      current.vtrack,
      current.vew + 0.502f,
      current.vheight,
      current.vpanDeg,
      current.vtiltDeg,
      current.vrollDeg};
  const limnmoco::CraneSolveResult result =
      limnmoco::solveLimnmocoCrane(target, geometry);

  const bool ok = near(result.track, 0.0219f, 0.001f) &&
                  near(result.errorLength, 0.0f, 1e-4f) &&
                  !result.boomClamped && !result.swingClamped;
  std::cout << (ok ? "[PASS] " : "[FAIL] ")
            << "EW jog computes track compensation without NS motion\n";
  return ok;
}

void printVec(const char *label, limnmoco::Vec3 v) {
  std::cout << "  " << label << ": (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

bool runTest(const TestCase &test) {
  constexpr float kAxisTolerance = 1e-5f;
  constexpr float kErrorTolerance = 1e-5f;

  const limnmoco::CraneSolveResult result = limnmoco::solveLimnmocoCrane(test.pose, test.geometry);

  bool ok = true;
  if (test.checkExactAxes) {
    ok = ok && near(result.track, test.expectedTrack, kAxisTolerance);
    ok = ok && near(result.swingDeg, test.expectedSwingDeg, kAxisTolerance);
    ok = ok && near(result.boomDeg, test.expectedBoomDeg, kAxisTolerance);
  }

  ok = ok && near(result.errorLength, 0.0, kErrorTolerance);
  ok = ok && ((result.boomClamped || result.swingClamped) == test.expectClamp);

  std::cout << (ok ? "[PASS] " : "[FAIL] ") << test.name << "\n";
  std::cout << std::fixed << std::setprecision(6);
  std::cout << "  track: " << result.track << "\n";
  std::cout << "  swingDeg: " << result.swingDeg << "\n";
  std::cout << "  boomDeg: " << result.boomDeg << "\n";
  std::cout << "  errorLength: " << result.errorLength << "\n";
  std::cout << "  clamped: " << ((result.boomClamped || result.swingClamped) ? "yes" : "no") << "\n";
  printVec("target", result.target);
  printVec("panTarget", result.panTarget);
  printVec("nodal", result.nodal);
  std::cout << "\n";

  return ok;
}

} // namespace

int main() {
  const limnmoco::CraneGeometry defaultGeometry{
      10.0, // boomLength
      2.8,  // extensionLength
      0.0,  // offsetX
      0.0,  // offsetY
      0.0,  // offsetZ
  };

  const std::vector<TestCase> tests = {
      {
          "Blender default panel values",
          limnmoco::VirtualPose{
              8.0, // vtrack / Y
              2.0, // vew / X
              3.0, // vheight / Z
              0.0, // vpanDeg
              0.0, // vtiltDeg
              0.0, // vrollDeg
          },
          defaultGeometry,
          -4.17623f,
          9.32780f,
          17.4576f,
          true,
          false,
      },
      {
          "Centered target, no offset",
          limnmoco::VirtualPose{
              12.8,
              0.0,
              0.0,
              0.0,
              0.0,
              0.0,
          },
          defaultGeometry,
          0.0,
          0.0,
          0.0,
          true,
          false,
      },
      {
          "Rotated nodal offset",
          limnmoco::VirtualPose{
              8.0,
              2.0,
              3.0,
              20.0,
              10.0,
              -5.0,
          },
          limnmoco::CraneGeometry{
              10.0,
              2.8,
              0.5,
              0.25,
              -0.1,
          },
          0.0,
          0.0,
          0.0,
          false,
          false,
      },
  };

  int failures = 0;
  if (!runForwardKinematicsOriginTest()) {
    ++failures;
  }
  if (!runEastWestCompensationTest()) {
    ++failures;
  }
  if (!runBoomCompensationTableTest()) {
    ++failures;
  }
  if (!runDragonframeBoomCompensationEncodingTest()) {
    ++failures;
  }
  if (!runBoomCompensationStepTargetRegressionTest()) {
    ++failures;
  }
  if (!runKuperTrackConventionTest()) {
    ++failures;
  }
  if (!runKuperNorthSouthCompensationTest()) {
    ++failures;
  }
  if (!runCameraLineDirectionTest()) {
    ++failures;
  }
  if (!runAimGeometryTest()) {
    ++failures;
  }
  if (!runCompensationFixture(
          "expected_virtual_displacements.csv",
          limnmoco::CraneGeometry{857.7f, 78.0f, 0.0f, 0.0f, 0.0f},
          false, 63)) {
    ++failures;
  }
  if (!runCompensationFixture(
          "expected_virtual_rotation_displacements_with_offsets.csv",
          limnmoco::CraneGeometry{857.7f, 78.0f, -0.2727f, -0.1463f, 0.3179f},
          true, 57)) {
    ++failures;
  }
  if (!runCompensationFixture(
          "expected_virtual_rotation_displacements.csv",
          limnmoco::CraneGeometry{857.7f, 78.0f, 0.0f, 0.0f, 0.0f},
          true, 57)) {
    ++failures;
  }
  for (const TestCase &test : tests) {
    if (!runTest(test)) {
      ++failures;
    }
  }

  if (failures) {
    std::cerr << failures << " IK test(s) failed.\n";
    return 1;
  }

  std::cout << "All IK tests passed.\n";
  return 0;
}
