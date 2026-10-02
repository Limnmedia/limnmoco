<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# FAN_CONTROL

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0300` · Unsupported request; header only.

**Protocol:** Code 0x0300 is defined in the firmware header but has no handler and no command section in the bundled PDF. Payload schema is unestablished here; common UNSUPPORTED ACK unless the hard-stop gate intervenes.

**State:** No fan state or configuration mutation.

**Motion/output:** No fan or motor action.

**Performance considerations:** Only parser/checksum and rejection/ACK costs; do not invent a fan-control benchmark.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). `loop()` unsupported-command branch.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

Control/Transport retain the unsupported-command outcome. This route does not introduce an implementation module for the unsupported feature.

**Characterization cases:** Empty/opaque payload, fault gate, and no hardware output changes.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.
