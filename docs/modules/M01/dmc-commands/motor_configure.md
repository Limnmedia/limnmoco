<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# MOTOR_CONFIGURE

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0037` · Request.

**Protocol:** Axis u8 and flags u8; common ACK or RANGE. Firmware recognizes additional virtual/live-control/coupling bits beyond the PDF’s basic enabled/blur description.

**State:** Directly replaces `motors[axis].config` without a local movement gate or merge with virtual-owned flags.

**Motion/output:** No immediate plan, but changes future playback participation, coupling, and live control.

**Performance considerations:** Constant assignment; measure later execution impact separately from command latency.

**Current code path:** Incoming branch at baseline line 742 in [m7.ino](../../../../m7/m7.ino). Inline axis validation and `motors[motor].config = ...`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Motion subsystem.
- Response: Motion subsystem → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Mapping and participation consultation: Control → Virtual + Playback → Control.

**Characterization cases:** Every supported flag, configure while moving, overwrite virtual flag, coupled followers, and rejection without unintended updates.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

