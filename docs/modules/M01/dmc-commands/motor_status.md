<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_STATUS

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0030` · Request.

**Protocol:** Empty request; same-type, same-ID reply containing u32 moving-bitmask and one zero DMX-status byte. Suppresses common ACK.

**State:** Reads eight `motors[].moving` flags without advancing motion.

**Motion/output:** None requested; report reflects software movement flags, not measured mechanics.

**Performance considerations:** Eight-axis scan and fixed 17-byte framed response; measure under concurrent progression and output congestion.

**Current code path:** Incoming branch at baseline line 681 in [m7.ino](../../../../m7/m7.ino). Inline status loop → `dmc_msg_prepare()` → `writeOutputMessage()`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

**Characterization cases:** Idle, independent/coordinated motion, stopping, playback, and hard-stop gate behavior.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

