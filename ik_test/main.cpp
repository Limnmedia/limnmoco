// SPDX-License-Identifier: BSD-3-Clause

#include "limnmoco_ik.h"

#include <cmath>
#include <iomanip>
#include <iostream>
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

  const bool ok = near(zeroAbsolute.vtrack, 935.7f, 1e-4f) &&
                  near(configuredOrigin.vtrack, 0.0f, 1e-6f) &&
                  near(movedVirtual.vtrack, 12.5f, 1e-4f);
  std::cout << (ok ? "[PASS] " : "[FAIL] ")
            << "FK origin normalization removes static boom reach\n";
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
