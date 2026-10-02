<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_UPLOAD_MOVE_TRIGGERS

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0104` · Request.

**Protocol:** Mask u32 followed by frame-index u32/value u32 pairs. Common ACK; values/mask are stored in byte-sized fields.

**State:** Replaces `triggerMask` and writes `triggerData[index+1]` for accepted indices; record truncation of high bits and signed-cast boundary checks.

**Motion/output:** No axis plan; later synchronized playback writes GIO triggers.

**Performance considerations:** Cost scales with pair count; measure partial records and output impact during concurrent playback separately.

**Current code path:** Incoming branch at baseline line 831 in [m7.ino](../../../../m7/m7.ino). Inline trigger upload branch; later `setMovePositionFrame()` → `setTriggers()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Later trigger playback: Control → Playback → Camera → Intercore.

**Characterization cases:** Repeated masks, first/last frame, high-bit indices, partial pairs, byte truncation, and out-of-order uploads.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

