<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Transport

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Transport owns serial reads/writes, bounded receive/transmit work, output queues,
and the existing diagnostic serial mirrors. It supplies bytes to the protocol
module and sends encoded responses. Define queue-full behavior explicitly;
reserve capacity or defer command acceptance when a required response cannot be
retained. Diagnostic traffic must not block motion execution.

The Transport maps valid packets to domain requests and domain results to
DMC ACKs, replies, and asynchronous notifications. It owns request IDs and the
rule for exactly one immediate command outcome. Domain controllers must not
construct packets or carry DMC error constants.

For HI, translate the agreed [Device](device.md) description
into DMC response fields. Keep capability-bit mappings and compatibility
exceptions here, separate from the actual supported feature set. DMC
owns their wire representation; Control routes the request.

These can be two components in the same firmware adapter package. Keeping their
interfaces separate allows protocol tests without Arduino and controller tests
without serialized packets.

## Current implementation

Serial reads, `writeOutputMessage()`, `transmitMessages()`, and the command dispatch block in `m7/m7.ino`.

## Interface and state contracts

- Transport owns byte queues and per-loop RX/TX budgets. It does not advance control time. The Transport owns request correlation and translates domain errors into DMC outcomes.
- Reserve bounded capacity for the immediate reply before committing a command. Define a separate bounded policy for later faults/events; telemetry and diagnostics may be coalesced or dropped only under an explicit policy.
- A validated request produces one immediate outcome: ACK, typed reply, or the characterized protocol-specific behavior. Asynchronous motion events are distinct from that outcome. A thin legacy adapter can temporarily serve unmigrated command families.

## Validation required for extraction

- Compare exact replies for each migrated command, including failures and requests that return data instead of an ACK.
- Fill the output queue and verify command state is not committed without the required reply capacity.
- Exercise short writes, RX bursts, and serial mirrors while the control tick continues within its budget.

## Open questions

- How should critical fault events be retained or prioritized under prolonged output congestion?
- Which serial mirrors remain enabled in normal builds, and what is their bounded drop policy?

## Planned commits

[M07](TODO.md#m07), [M20](TODO.md#m20), [M24](TODO.md#m24).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [DMC](DMC.md), [Control](control.md), [Errors](errors.md).
