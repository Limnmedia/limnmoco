<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_STOP_LOOP

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0116` · Request.

**Protocol:** Empty expected payload; common ACK.

**State:** Clears loop and ping-pong bits in `goMotionMove.flags`, with no local active-mode check.

**Motion/output:** Allows current iteration to finish; does not initiate immediate braking.

**Performance considerations:** Constant handler; measure remaining iteration duration separately from ACK latency.

**Current code path:** Incoming branch at baseline line 1106 in [m7.ino](../../../../m7/m7.ino). Inline flag clear; later loop selection in `updateMotorVelocities()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Completion of current iteration: Control → Playback → Motion subsystem + Camera → Intercore.

**Characterization cases:** Before run, mid-loop, near endpoint, ping-pong direction changes, and fault gate.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

