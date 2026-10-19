<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# GIO_OUT

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0021` · Request.

**Protocol:** Reads a u32 trigger bitmask; normal success is ACK. No exact payload-length check in this branch.

**State:** `setTriggers()` writes two output pins. It does not itself update `_triggers`, the cached bitmask used by blip logic; record interactions instead of assuming one coherent state owner.

**Motion/output:** No motor plan; immediate M7 GPIO effect.

**Performance considerations:** Small constant handler; measure pin transition relative to ACK enqueue/transmit and subsequent tick activity.

**Current code path:** Incoming branch at baseline line 630 in [m7.ino](../../../../m7/m7.ino). `setTriggers(gio)` → `digitalWrite(LOGIC_OUT_0/1, ...)`.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control → Camera → Intercore.
- Response: Camera → Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.

**Characterization cases:** Both bits, ignored higher bits, overlap with playback/blips, and short payloads in a host harness.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

