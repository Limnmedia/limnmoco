<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_GET_POSITION

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0034` · Request + periodic report.

**Protocol:** Empty request. `sendMotorPositions()` emits same-type data with ID 0: frame time u32 plus eight i32 step positions (36-byte payload). The incoming handler also leaves the common success ACK enabled, using the request ID. Periodic reports have no request ACK.

**State:** Reads software positions and frame clock; updates `lastPositionTime`.

**Motion/output:** No motion request. Values are software-commanded/reconciled positions, not encoder readings.

**Performance considerations:** Fixed 48-byte framed data plus 16-byte ACK for a request; compare with periodic reporting and shared virtual-position reporting.

**Current code path:** Incoming branch at baseline line 778 in [m7.ino](../../../../m7/m7.ino). `sendMotorPositions()`; called from command handling, periodic `loop()` logic, resets/no-ops, and playback.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem + Playback.
- Response: Motion subsystem + Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Periodic report: Control → Motion subsystem + Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).

**Characterization cases:** Capture two-packet order and IDs, signed positions, frame scaling, hard-stop exception, and periodic cadence.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

