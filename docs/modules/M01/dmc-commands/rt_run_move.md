<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_RUN_MOVE

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0111` · Request.

**Protocol:** FPS u32/1000, start/end i32, preroll/postroll u32 milliseconds, sync byte, blip location u32, DMX channel u16, blip duration u16, flags u16; still-camera flags add open/close u16 angles. Base read order is 31 bytes, plus four with still angles. ACK or MOVING/NOT_IN_POSITION/PREROLL/POSTROLL.

**State:** Clears/rebuilds Go Motion data and writes sync, blip, camera-window and flag state before final preroll/postroll validation. Successful preparation sets ALL_JOG and plans frame-clock positioning.

**Motion/output:** Positions toward start, then preroll/wait; actual run requires GO. Loops zero postroll time; live-control axes may execute separately.

**Performance considerations:** Profile full-axis trajectory preparation, limit scans, frame-clock planning, and subsequent playback tick cost. Source divides blip duration by five despite tick-based countdown; measure observed duration.

**Current code path:** Incoming branch at baseline line 877 in [m7.ino](../../../../m7/m7.ino). `clearGomoMove()` → `calculateRunLive()` → limit checks → frame-time `calculatePointToPoint()`; later mode transitions, `go()`, and `updateMotorVelocities()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing playback and live-axis execution: Control → Playback → Motion subsystem + Camera → Intercore.
- Numerical support: Playback + Motion subsystem → Mathematics.

Playback owns track/frame/exposure state; Motion subsystem owns physical-axis execution. Camera owns output intent and sequencing, including when no new camera intent is produced for a particular mode.

**Characterization cases:** All flag combinations, optional angles, zero FPS/times in host tests, reverse range, rejected preroll side effects, loops, and mixed live control. PDF sync field is labeled DMX; firmware uses it for triggers and reads additional camera flags.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

