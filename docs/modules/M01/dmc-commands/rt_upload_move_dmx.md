<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_UPLOAD_MOVE_DMX

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0102` · Unsupported request.

**Protocol:** PDF layout: channel u16, start-index u32 with final flag, then level bytes. No handler; common UNSUPPORTED ACK unless an earlier gate rejects it.

**State:** No uploaded DMX storage is populated.

**Motion/output:** No axis or DMX output change.

**Performance considerations:** Measure framing/checksum versus packet size and rejection cost; no track-fill work should occur.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). `loop()` unsupported-command branch.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Control/Transport retain the unsupported-command outcome. This route does not introduce an implementation module for the unsupported feature.

**Characterization cases:** Variable payload sizes, final flag, active upload, and hard-stop gate precedence.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

