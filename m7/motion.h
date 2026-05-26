// SPDX-License-Identifier: BSD-3-Clause
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com) 
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file motion.h
 * \brief limnmoco motion calculation functions for the m7 core
 */

#ifndef MOTION_H_
#define MOTION_H_

#include "dfx.h"

void calculatePointToPoint(Motor *motor, int32_t destination, float minTime);

float evaluateMove(AxisMoveData *axisMove, float t);

void calculateGoMotionMove(Motor *motors, AxisMoveData *axisMoves, GoMotionMove *goMotionMove, int32_t frame,
                           int32_t dir, int32_t exposure, int32_t blur, GoMotionOverride *overrides);
void calculateGoMotionMove2(Motor *motors, AxisMoveData *axisMoves, GoMotionMove *goMotionMove, int32_t frame,
                            int32_t exposure, int16_t shutterOpen, int16_t shutterClose, GoMotionOverride *overrides);

void calculateRunLive(Motor *motors, AxisMoveData *axisMoves, GoMotionMove *goMotionMove, int32_t startFrame,
                      int32_t endFrame, float fps, float accelTime, float decelTime);
void calculateRunLivePingPong(GoMotionMove *goMotionMove);
void calculateRunLiveLoop(GoMotionMove *goMotionMove);

#endif /* MOTION_H_ */
