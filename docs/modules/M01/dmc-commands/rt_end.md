<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# RT_END

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0114` · Device event; incoming unsupported.

**Protocol:** Empty ID-0 notification; incoming requests have no dedicated branch. PDF describes completion of live run or shooting.

**State:** Current producer sets the queued event when live-run main movement enters deceleration. No equivalent queued producer was found for shooting modes; confirm with traces.

**Motion/output:** Event does not itself stop motion. It may precede completion of deceleration/postroll; loops may produce repeated events.

**Performance considerations:** Measure main-segment end, notification enqueue/transmit, shutter behavior, and actual rest as separate timestamps.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). `updateMotorVelocities()` GO_MO_MOVE→GO_MO_DECEL branch → message flag → `transmitMessages()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Playback notification: Playback → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Remaining execution or camera work: Control → Playback + Motion subsystem + Camera → Intercore.

The incoming message remains unsupported. Notification timing does not, by itself, establish that Motion subsystem has reached rest.

**Characterization cases:** Live run, each shot mode, loop boundaries, postroll, queued event coalescing, and incoming rejection.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

