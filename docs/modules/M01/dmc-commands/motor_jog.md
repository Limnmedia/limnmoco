<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_JOG

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0036` · Request.

**Protocol:** Axis u8, speed u16, raw-step destination i32 (seven expected bytes); common ACK. PDF range is 1–10000, but this helper does not explicitly reject zero or values above 10000.

**State:** Temporarily scales velocity/acceleration, computes a finite horizon, restores configured limits, and updates mode/frame/synchronization for non-live axes.

**Motion/output:** Independent jog horizon or braking on reversal; destination is not a scaled virtual position.

**Performance considerations:** Measure horizon calculation plus plan construction at rest/velocity/reversal; consider repeated jog packet frequency.

**Current code path:** Incoming branch at baseline line 715 in [m7.ino](../../../../m7/m7.ino). `msg_motor_jog()` → `jogMotor()` → `calculateJogDestination()` and `calculatePointToPoint()` or `stopMotor()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing motion execution: Control → Motion subsystem → Intercore.

Motion subsystem uses its independent planner/executor; Control handles cross-mode policy without owning the axis state.

**Characterization cases:** Speed endpoints/out-of-range, same target, reversal, active playback/live axis, coupling, and software limits.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

