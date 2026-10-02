<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Application coordinator

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own construction/wiring, command routing, cross-controller supersession policy,
and control-loop ordering. Receive command and input events, consult fault policy,
invoke the appropriate controller, advance motion, and publish outputs/responses.

Keep this a small explicit coordinator. It must not become a new location for
trajectory algorithms, upload buffers, or every module's mutable state. Use direct
calls and typed results; a global event bus, service locator, RTOS task per module,
or general plugin system is unnecessary.

## Current implementation

`setup()`, `loop()`, mode transitions, command dispatch, and global initialization in `m7/m7.ino`.

## Interface and state contracts

- Construct concrete adapters and controller instances once. Receive already decoded domain requests; wire representation stays in the command adapter.
- Own an explicit supersession matrix for independent, coordinated, virtual, playback, configuration/reset, and fault commands. Policy grants a transition; motion enforces actual reservations.
- Define tick ordering around link readiness: sample inputs/observations, apply policy and pending commands, prepare continuation/timeline work, execute one motion interval, assemble outputs, and publish. Characterize the current order before changing observable timing.
- Route typed completion/fault/camera events directly to their owners. Keep callbacks bounded and avoid reentrant command execution. No universal mutable context or global event bus is required.

## Validation required for extraction

- Test a command-by-active-mode matrix, including rejected supersession leaving the prior activity untouched.
- Test fault and stop at tick boundaries, queued commands, configuration revisions, and late events from canceled groups.
- Verify one accepted interval yields one timeline advance and one publication; blocked transport cannot stall execution.

## Open questions

- At which tick boundary are commands committed, and what does an ACK promise about acceptance versus physical execution?
- Which cross-mode transitions require stopping first versus a validated velocity-preserving handoff?
- How are publication readiness and asynchronous faults prioritized without changing current timing accidentally?

## Planned commits

[M07](TODO.md#m07), [M08](TODO.md#m08), [M19](TODO.md#m19), [M20](TODO.md#m20), [M23](TODO.md#m23).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [transport command adapter](transport-command-adapter.md), [motion](motion.md), [virtual controller](virtual-controller.md), [playback move store](playback-move-store.md), [fault stop policy](fault-stop-policy.md).
