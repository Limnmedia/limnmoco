<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_STOP_ALL

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0033` · Request.

**Protocol:** Empty or optional u32 flags; normal success is ACK. With moving motors and zero flags, handler sets `hardStop`; the shared gate can reject later STOP_ALL while latched.

**State:** Changes stop timing/counters, playback state, and sometimes camera countdown. Repeated calls within the helper’s time window select `emergency`, but the current `stopMotor()` body does not use that parameter; do not infer a distinct profile from its name.

**Motion/output:** Stops the frame clock in ALL_JOG or prepares per-axis braking otherwise. Explicit coordinated/line-jog invalidation is not in this helper; verify whether subsequent ticks can overwrite braking.

**Performance considerations:** Measure full-axis stop setup, frame-alignment branches, first affected output interval, and final stopping duration.

**Current code path:** Incoming branch at baseline line 703 in [m7.ino](../../../../m7/m7.ino). Inline flags/latch handling → `stopAll(0)` → `stopMotor()`; later `updateMotorVelocities()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Virtual + Playback + Motion subsystem + Camera.
- Response: Virtual + Playback + Motion subsystem + Camera → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Braking and camera output: Control → Motion subsystem + Camera → Intercore.

The listed state owners cooperate on cancellation and stopping; the plus signs do not prescribe a call order. Errors owns stop severity/recovery policy, not motor trajectories or camera state.

**Characterization cases:** First/repeated/silent stop, fault gate, ALL_JOG alignment, active line jog, coordinated motion, and video camera pulse.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

