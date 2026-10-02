// SPDX-License-Identifier: BSD-3-Clause

#ifndef LIMNMOCO_KUPER_TRACK_CONVENTION_H_
#define LIMNMOCO_KUPER_TRACK_CONVENTION_H_

#include "LimnmocoIK.h"

namespace limnmoco {

// Kuper and Dragonframe expose track as world Z: negative is forward and
// positive is away from the subject. The Swing-Boom-Track solver uses a
// convenient local axis that is positive forward along the level boom.
// Real track motor positions use the Kuper/Dragonframe convention.
float kuper_track_to_solver(float track);
float solver_track_to_kuper(float track);

VirtualPose kuper_pose_to_solver(const VirtualPose &pose);
VirtualPose solver_pose_to_kuper(const VirtualPose &pose);

} // namespace limnmoco

#endif // LIMNMOCO_KUPER_TRACK_CONVENTION_H_
