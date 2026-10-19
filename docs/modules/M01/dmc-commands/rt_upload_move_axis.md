<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_UPLOAD_MOVE_AXIS

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0101` · Request.

**Protocol:** Axis u8, start-index u32 with high-bit final flag, then i32 positions. Common ACK/RANGE; input index is shifted to internal one-based storage.

**State:** Writes track positions directly; a final flag repeats the last value through the declared end. Checks starting index against declared count, not a fully validated buffer extent/payload shape.

**Motion/output:** No plan creation; stored values can feed subsequent or concurrent playback.

**Performance considerations:** Cost scales with supplied points and, for a final packet, the remaining track length. A small packet can trigger a long fill.

**Current code path:** Incoming branch at baseline line 800 in [m7.ino](../../../../m7/m7.ino). Inline axis upload loop and final-value fill in `loop()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Playback participates through its move-store component. Control consults its playback component for upload/execution ownership; storage and playback state stay with that boundary.

**Characterization cases:** Zero points, partial trailing i32, index/count boundary, too-large declared count, final-fill length, and records while playback reads the store.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

