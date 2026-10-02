<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_RESET_POSITION

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0035` · Request.

**Protocol:** Exactly five bytes: axis u8 and new i32 position. Rejects if `motorsMoving`. On success sends ID-0 position data then common ACK; PDF describes ACK followed by position confirmation.

**State:** Writes both `motors[axis].position` and shared accumulator through `SET_MOTOR_POSITION`. No explicit virtual-origin refresh in this branch.

**Motion/output:** Relabels position without a requested move. Consistency with M4 accumulation and virtual reporting must be recorded.

**Performance considerations:** Measure reset/write and report costs separately; investigate coherent cross-core reset rather than timing only the assignment.

**Current code path:** Incoming branch at baseline line 653 in [m7.ino](../../../../m7/m7.ino). Inline assignment → `SET_MOTOR_POSITION` macro in `m7/dfx.h` → `sendMotorPositions()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem → Intercore.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Origin and frame-validity consultation: Control → Virtual + Playback → Control.

Motion subsystem owns the commanded position; Intercore owns synchronized access to emitted-step state. Consultation does not decide a new reset/origin policy.

**Characterization cases:** Idle versus moving, negative positions, invalid axis/length, configured virtual origin, and ACK/report order.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

