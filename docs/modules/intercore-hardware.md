<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Inter-core link, pulse engine, and board adapters

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

The inter-core link owns a single shared ABI declaration, fixed-point speed
conversion, publication/consumption, and emitted-step snapshots. M7 sends a bounded
output frame; M4 consumes it. Only this layer knows shared addresses, RPC bootstrap,
cache/memory ordering, and shared-memory layout.

Initially preserve the existing layout and timing. Add compile-time size/offset
checks on both cores and document field ownership. Specify how 64-bit accumulator
reads/resets are made consistent on the 32-bit cores; `volatile` alone is not an
atomicity or inter-core synchronization contract. Any handshake or layout change
requires paired firmware and hardware validation. Define stale-output behavior
before claiming fault containment if M7 stops publishing.

The pulse engine owns phase accumulation, pulse completion, direction transitions,
and phase-synchronous camera output. Extract its deterministic transition logic
for host tests; retain timer/interrupt/register access in the M4 board adapter.
Validate physical pulse and direction timing on hardware.

Board adapters own pin assignments, polarity, timer setup, serial/RPC startup,
and physical I/O. They implement narrow interfaces used by the application.
Diagnostics use a bounded optional sink and snapshots/events, with no Serial
dependency in numerical or motion code.

## Current implementation

Both `DmcSharedData` declarations, fixed shared address, RPC startup, speed conversion, and position macros; M4 timer callback/GPIO helpers and both `config.h` files.

## Interface and state contracts

- Declare the current ABI once and check size, alignment, field offsets, and constants under both core builds. Keep a field-ownership table, including initialization and exceptional reset ownership.
- M7 prepares a bounded output frame; the link adapter converts units and publishes it under the established handshake. M4 consumes the frame and owns running pulse accumulation. A rejected/busy publication cannot advance controller time twice.
- Expose emitted-step snapshots and reset operations rather than volatile pointers. Specify atomicity, cache visibility, ordering, and startup sequencing before modifying the current handshake.
- Extract pure pulse transitions into deterministic code while the board adapter retains timer interrupts and register writes. Camera phase behavior remains in this timing domain.
- Keep normal M7 stop commands separate from any future M4 stale-output safeguard. A watchdog or ABI version negotiation is a distinct reviewed behavior change, not an implied existing capability.

## Validation required for extraction

- Compile both cores against layout assertions; test fixed-point conversion, sign, bounds, and snapshot/reset sequences.
- Test pulse arithmetic over multiple ticks, reversals, stopping with a pulse high, and camera phase wrap.
- Use hardware traces to verify step width, direction setup/hold, camera timing, restart, and shared-memory behavior under load.
- Record M7/M4 flash, RAM, stack headroom, tick duration, and ISR duration before and after extraction.

## Open questions

- What consistent snapshot/reset mechanism should protect 64-bit values shared between the two 32-bit cores?
- What should M4 do when M7 output becomes stale, and how will recovery be negotiated?
- Which GIGA/Portenta configurations are supported and available for acceptance testing?

## Planned commits

[M21](TODO.md#m21), [M22](TODO.md#m22), [M25](TODO.md#m25).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [motion](motion.md), [camera auxiliary io](camera-auxiliary-io.md), [application coordinator](application-coordinator.md).
