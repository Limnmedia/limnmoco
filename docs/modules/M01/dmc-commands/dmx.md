<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# DMX

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0020` · Unsupported request.

**Protocol:** PDF layout: ramp byte, starting channel u16, then level bytes. No firmware handler; normal dispatch returns UNSUPPORTED ACK. Hello advertises zero DMX channels.

**State:** No DMX state exists in this path.

**Motion/output:** No axis or lighting change from the unsupported request.

**Performance considerations:** Measure parsing/checksum against payload length and short rejection/ACK cost.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). `loop()` unsupported-command branch; no DMX execution helper.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Control/Transport retain the unsupported-command outcome. This route does not introduce an implementation module for the unsupported feature.

**Characterization cases:** Small and maximum accepted payloads; verify no output changes and exact rejection.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

