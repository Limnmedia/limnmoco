<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_POSITION_FRAME

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0110` · Request.

**Protocol:** Frame i32; ACK, MOVING when in SHOOT, or GENERAL with no move frames.

**State:** Clears `playBlipLocation` even before rejection. Sets JOG mode, frame-time position, camera accumulator, reported frame, and possibly trigger outputs.

**Motion/output:** Independent point-to-point moves for eligible uploaded axes. `positionFrame()` clamps the data index while frame reporting uses the requested frame; verify out-of-range behavior.

**Performance considerations:** Up to eight plans and trigger work; measure zero-distance and multi-axis cases plus camera accumulator reset.

**Current code path:** Incoming branch at baseline line 858 in [m7.ino](../../../../m7/m7.ino). `positionFrame()` → `calculatePointToPoint()` → `SET_MOTOR_POSITION` for camera → `setMovePositionFrame()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Positioning and trigger output: Control → Motion subsystem + Camera → Intercore.
- Numerical support: Motion subsystem → Mathematics.

**Characterization cases:** Before/after upload bounds, SHOOT rejection side effects, live-control axes, absent axis tracks, and trigger alignment.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

