<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# GIO_IN

[DMC command inventory](README.md) · [M01 baseline guide](../README.md) · [Global TODO](../../TODO.md#m01)


Code: `0x0022` · Device event; incoming query unsupported.

**Protocol:** PDF permits an empty query and unsolicited changes. Firmware emits a u32 logical input value with ID 0 on a debounced transition, but has no incoming-query branch; such queries return UNSUPPORTED ACK.

**State:** `switchInputCounter` and `switchInput` track debounce; a transition can also start playback while waiting.

**Motion/output:** The notification itself does not schedule motion; the same input transition can separately call `go()`.

**Performance considerations:** Measure raw input-to-debounced event latency, event queue delay, and shared tick cost; do not attribute debounce delay to packet parsing.

**Current code path:** Event/rejection paths in [m7.ino](../../../../m7/m7.ino). `logicSwitchInput()` → debounce in `loop()` → `dmc_msg_prepare(DMC_MSG_GIO_IN, 0)`; incoming request falls through.

**Proposed modules:**

- Request: Transport (transport role) → DMC → Transport (command-adapter role) → Control.
- Response: Control → Transport (command-adapter role) → DMC → Transport (transport role).
- Command-gating consultation: Control → Errors → Control.
- Input notification: Intercore → Camera → Control → Transport (command-adapter role) → DMC → Transport (transport role).

The incoming query remains unsupported in the behavior baseline; the separate notification path uses Camera as the logical input-state owner. A playback-starting input transition additionally involves Playback as listed for RT_GO.

**Characterization cases:** Bounce patterns, input disabled at build time, transition while SHOOT_WAIT, and unsupported polling.

**Evidence to record:** protocol fixture/capture — pending; state/motion or no-op trace — pending; timing/memory — pending.

