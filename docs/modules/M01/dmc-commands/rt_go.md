<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_GO

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0113` · Request + input-originated event.

**Protocol:** Empty request; ACK only if SHOOT_WAIT, otherwise NOT_IN_POSITION. A debounced input trigger can independently start the action and queue an empty ID-0 RT_GO event.

**State:** Changes wait to shoot, initializes Go Motion progression, axis moving flags, and camera/blip state.

**Motion/output:** Starts previously prepared playback/shooting; does not parse or build a new uploaded trajectory.

**Performance considerations:** Measure command-to-first-moving-output and input-trigger-to-output separately from short handler latency.

**Current code path:** Incoming branch at baseline line 1095 in [m7.ino](../../../../m7/m7.ino). `go(&motorsMoving)` from dispatch or input transition; input-originated notification through `messageQueue`/`transmitMessages()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Playback → Motion subsystem + Camera.
- Response: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Input-triggered start: Intercore → Camera → Control → Playback → Motion subsystem + Camera → Intercore.
- Input-start notification: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Continuing execution: Control → Playback → Motion subsystem + Camera → Intercore.

**Characterization cases:** Early/duplicate GO, both shooting modes/live run, input versus host start, and event/ACK ordering.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

