<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_HARD_STOP

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x003A` · Device event; incoming unsupported.

**Protocol:** Normally unsolicited ID 0. Queued general stop emits reason byte 0. Position/accumulator mismatch emits reason 100 plus axis byte; PDF only specifies the extra motor byte for reasons 1/2. No incoming execution branch.

**State:** Related paths update `hardStop`, counters, `exceptionCode`, and queued-message bits. Sending the event is distinct from performing the stop.

**Motion/output:** E-stop handling calls `stopAll(1)`; mismatch detection requests stopping in the velocity update. Verify actual outputs and whether the event precedes completion.

**Performance considerations:** Measure stimulus-to-stop-output and stimulus-to-event-transmit separately, including queue saturation.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). E-stop branch in `loop()` → queued flag → `transmitMessages()`; mismatch branch in `updateMotorVelocities()` emits directly.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Fault stimulus and stop policy: Intercore + Motion subsystem → Errors → Control → Virtual + Playback + Motion subsystem + Camera.
- Fault notification: Errors → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Stop output: Control → Motion subsystem + Camera → Intercore.

The incoming message remains unsupported. Notification and stop execution are separate paths and may complete at different times.

**Characterization cases:** General stop, mismatch reason/axis shape, fault recovery, repeated faults, incoming request, and event loss under congestion.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

