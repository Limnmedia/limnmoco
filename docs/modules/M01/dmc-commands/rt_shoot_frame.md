<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_SHOOT_FRAME

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0112` · Request.

**Protocol:** Frame i32, direction u8, exposure u32 ms, blur u16, optional axis u8/posA i32/posB i32 records (11 + 9N bytes). Common ACK or RANGE/MOVING.

**State:** Clears overrides before frame/movement validation. Accepted path rebuilds Go Motion state and enters SHOOT_PREROLL; zero blur becomes 1000. Override axis index is used without the range guard present in version 2.

**Motion/output:** Plans blurred exposure and preroll positions; GO starts the shot after positioning.

**Performance considerations:** Measure override parsing, per-axis exposure calculations, and preroll planning; record zero exposure behavior with host tests before physical execution.

**Current code path:** Incoming branch at baseline line 985 in [m7.ino](../../../../m7/m7.ino). `clearGomoMove()` → `calculateGoMotionMove()` → per-axis `calculatePointToPoint()`; later `go()` and shooting tick branches.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing playback and live-axis execution: Control → Playback → Motion subsystem + Camera → Intercore.
- Numerical support: Playback + Motion subsystem → Mathematics.

Playback owns track/frame/exposure state; Motion subsystem owns physical-axis execution. Camera owns output intent and sequencing, including when no new camera intent is produced for a particular mode.

**Characterization cases:** Forward/reverse, blur zero/default, overrides, malformed/out-of-range override axis, rejection mutation, and completion notification versus shutter close.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

