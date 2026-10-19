<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_UPLOAD_MOVE_END

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0103` · Request.

**Protocol:** Empty expected payload; common ACK, including when no upload is active.

**State:** If loading frames, extrapolates internal frame zero from positions 1/2 for every axis, then clears load state. Does not verify that all data arrived.

**Motion/output:** No move starts; subsequent playback can use the resulting arrays.

**Performance considerations:** Fixed eight-axis pass; capture effects for short uploads as well as ordinary tracks.

**Current code path:** Incoming branch at baseline line 846 in [m7.ino](../../../../m7/m7.ino). Inline end-upload branch in `loop()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Playback participates through its move-store component. Control consults its playback component for upload/execution ownership; storage and playback state stay with that boundary.

**Characterization cases:** End without begin, missing axes, one-frame upload, duplicate end, and malformed payload.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

