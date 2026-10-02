<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_MOVE

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0031` · Request.

**Protocol:** Exactly five bytes required: axis u8 and signed raw-step target i32. Firmware returns ACK, whereas PDF describes a one-byte moving-status response. A no-op move can enqueue a position report before ACK.

**State:** Validates axis and current mode/live-control flag. May set `moveState=JOG`, clear trigger synchronization, invalidate `movePositionFrame`, and replace per-axis segments.

**Motion/output:** Independent point-to-point planning from current position/velocity. Current helper does not establish a general group-reservation contract; interactions with coordinated motion need characterization.

**Performance considerations:** Measure planning branches for rest, moving, reversal, and no-op; include possible extra telemetry and planner stack use.

**Current code path:** Incoming branch at baseline line 640 in [m7.ino](../../../../m7/m7.ino). `msg_motor_move()` → `calculatePointToPoint()` in `m7/motion.cpp`; later `updateMotorVelocity()` and output publication.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing motion execution: Control → Motion subsystem → Intercore.

Motion subsystem uses its independent planner/executor; Control handles cross-mode policy without owning the axis state.

**Characterization cases:** Invalid axis/length, target at current position, direction reversal, limits, coupled axis, and command during virtual/playback execution.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

