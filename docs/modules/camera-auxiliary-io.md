<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Camera and auxiliary I/O

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own camera meter/shutter state, timed pulses, trigger output state, blips, and
input debounce as small components. Playback submits timed intents; direct GIO
commands submit immediate intents under a defined precedence policy. Return
output values/events rather than calling Arduino APIs in the controller.

Preserve the two timing domains: ordinary countdowns follow the M7 control tick;
phase-dependent shutter transitions stay with the M4 timer. Carry camera phase
and shutter-window data explicitly across the output boundary. The existing ninth
accumulator can remain in the initial ABI without presenting it as a motor to
application code.

## Current implementation

Camera/trigger globals, `setCamera()`, `setTriggers()`, input debounce, countdowns, and playback blips in `m7/m7.ino`; phase-window camera output in `m4/m4.ino`.

## Interface and state contracts

- Accept direct or timeline-owned intents and return output snapshots. State records the intent owner so playback cancellation does not accidentally erase an unrelated direct command.
- Represent M7 tick pulses and M4 phase windows separately. Do not move phase-dependent shutter transitions onto the slower M7 tick during extraction.
- Input adapters provide raw levels; portable debounce produces logical events using explicit tick/time input. Board-specific polarity remains in the board adapter.

## Validation required for extraction

- Test pulse lengths, overlapping intents, trigger masks, debounce transitions, and cancellation during exposure.
- Test loop/shoot/stop camera sequences against characterized traces.
- Validate actual shutter phase alignment and GPIO polarity on hardware after adapter changes.

## Open questions

- What precedence applies between direct commands, playback, and stop/fault actions?
- Which outputs are latched on stop and which are cleared, including the existing video toggle pulse?
- Should an input debounce counter remain tick-based or become time-based in a separate behavior change?

## Planned commits

[M16](TODO.md#m16), [M18](TODO.md#m18), [M19](TODO.md#m19), [M22](TODO.md#m22).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [playback move store](playback-move-store.md), [fault stop policy](fault-stop-policy.md), [intercore hardware](intercore-hardware.md).
