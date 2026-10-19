<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_UPLOAD_MOVE_BEGIN

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0100` · Request.

**Protocol:** Start-frame i32 and end-frame i32; common ACK. No range/capacity check before state/storage updates in this branch.

**State:** Sets load state, clears trigger mask/data and all eight axis stores, computes inclusive frame count, and initializes per-axis frame counts.

**Motion/output:** No deliberate move, but replacing data during playback can affect the active trajectory; no local playback exclusion is established here.

**Performance considerations:** Important large-clear path: eight 10,000-entry int32 arrays plus metadata and 10,000 trigger bytes are cleared even for a short upload. Measure handler duration and control-update delay.

**Current code path:** Incoming branch at baseline line 782 in [m7.ino](../../../../m7/m7.ino). Inline upload-begin branch → `clearAxisMove()`/`memset()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Playback participates through its move-store component. Control consults its playback component for upload/execution ownership; storage and playback state stay with that boundary.

**Characterization cases:** Empty/reversed/maximum ranges, replacement during motion, incomplete upload, and preservation versus invalidation of previous data.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

