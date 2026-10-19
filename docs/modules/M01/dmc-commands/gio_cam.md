<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# GIO_CAM

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0023` · Request.

**Protocol:** Reads u32 camera flags, passed to the byte-valued `setCamera()`; normal success is ACK. PDF defines shutter and meter bits.

**State:** Changes `sharedData->cameraValue`; subsequent playback/countdown logic can overwrite it.

**Motion/output:** No motor trajectory. M4 drives camera outputs from the shared value only when camera phase motion is inactive; active phase motion derives output from the shutter window.

**Performance considerations:** Measure handler time separately from shared-value-to-M4-pin delay and exposure timing.

**Current code path:** Incoming branch at baseline line 635 in [m7.ino](../../../../m7/m7.ino). `setCamera(cam)` → shared camera value → M4 timer callback.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Camera → Intercore.
- Response: Camera → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

**Characterization cases:** Meter/shutter independently and together; phase-active versus idle; overlap with countdown/stop behavior.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

