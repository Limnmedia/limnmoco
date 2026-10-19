<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_JOG_ALL

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0120` · Request.

**Protocol:** FPS u32/1000 and destination frame i32 (eight expected bytes). ACK, MOVING outside eligible mode, or NOT_IN_POSITION.

**State:** Initializes/updates frame-time pseudo-motor, sets ALL_JOG, and resets frame-stop count.

**Motion/output:** Moves through uploaded physical tracks by advancing frame time; live-control motors have a separate update path.

**Performance considerations:** Measure frame-clock planning at acceptance and per-tick interpolation across enabled axes; compare reversal and stop-alignment costs.

**Current code path:** Incoming branch at baseline line 1110 in [m7.ino](../../../../m7/m7.ino). Inline frame-clock setup → `calculatePointToPoint(&frameTimeMotor, ...)`; tick uses `evaluateMove()` per uploaded axis.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Continuing playback and live-axis execution: Control → Playback → Motion subsystem + Camera → Intercore.
- Numerical support: Playback + Motion subsystem → Mathematics.

Playback owns track/frame/exposure state; Motion subsystem owns physical-axis execution. Camera owns output intent and sequencing, including when no new camera intent is produced for a particular mode.

**Characterization cases:** Not-in-position, same frame, reverse timeline, limits/out-of-range destination, active upload, and STOP_ALL alignment.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

