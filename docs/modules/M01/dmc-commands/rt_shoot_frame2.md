<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_SHOOT_FRAME2

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0115` · Request.

**Protocol:** Frame i32, exposure u32 ms, open/close angles interpreted as i16, optional axis/posA/posB records (12 + 9N bytes). Common ACK or RANGE/MOVING.

**State:** Clears overrides before validation; zero exposure defaults to 1000. Invalid override axes are read but ignored. Rebuilds Go Motion state and enters SHOOT_PREROLL.

**Motion/output:** Plans exposure over angle windows, including pre/post extension; zero angle span reaches a division in the calculator and needs characterization.

**Performance considerations:** Compare with version 1: optional-record parsing, window arithmetic, per-axis plans, and recurring exposure timing.

**Current code path:** Incoming branch at baseline line 1041 in [m7.ino](../../../../m7/m7.ino). `clearGomoMove()` → `calculateGoMotionMove2()` → per-axis `calculatePointToPoint()`; later shooting tick path.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing playback and live-axis execution: Control → Playback → Motion subsystem + Camera → Intercore.
- Numerical support: Playback + Motion subsystem → Mathematics.

Playback owns track/frame/exposure state; Motion subsystem owns physical-axis execution. Camera owns output intent and sequencing, including when no new camera intent is produced for a particular mode.

**Characterization cases:** Negative/open-above-range angles, equal/reversed windows in host tests, default exposure, malformed records, and raw-frame versus upload-relative indexing.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

