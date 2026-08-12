// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_NAMED_MOVE_FIXTURE_H_
#define LIMNMOCO_NAMED_MOVE_FIXTURE_H_

#include <iosfwd>
#include <string>
#include <vector>

namespace limnmoco::test {

struct NamedMovePhysicalPose {
  float boomDeg;
  float swingDeg;
  float trackMm;
  float panDeg;
  float tiltDeg;
  float rollDeg;
};

struct NamedVirtualMoveCase {
  std::string name;
  std::string category;
  float boomLengthMm;
  float extensionLengthMm;
  float nodalOffsetXmm;
  float nodalOffsetYmm;
  float nodalOffsetZmm;
  bool rollPresent;
  bool boomCompensationEnabled;
  NamedMovePhysicalPose startingPhysical;
  float targetVtrackMm;
  float targetVewMm;
  float targetVnsMm;
  float targetVpanDeg;
  float targetVtiltDeg;
  float targetVrollDeg;
  NamedMovePhysicalPose expectedPhysical;
  float translationToleranceMm;
  float rotationToleranceDeg;
};

bool read_named_move_fixture(std::istream &input,
                             std::vector<NamedVirtualMoveCase> *cases,
                             std::string *error);

} // namespace limnmoco::test

#endif // LIMNMOCO_NAMED_MOVE_FIXTURE_H_
