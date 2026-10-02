<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Packet protocol

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own DMC framing, checksum, incremental stream assembly/resynchronization, typed
payload codecs, wire lengths, command identifiers, and response encoding.
Malformed bytes must fail before application state changes.

Keep two internal layers: a frame codec and command-specific payload codecs.
Accept byte buffers and return packets or structured errors. Do not read Serial,
own motors, decide whether a move is allowed, or schedule motion here.

Move `AimPointProtocol` and fixed-point wire scales out of the kinematics
library. Keep protocol representation separate from domain values: the command
adapter converts packet fields into typed axis IDs, poses, and configuration.
The existing `std::vector`/`std::string` host API is a useful starting reference;
the firmware path needs bounded buffers, explicit ownership/lifetimes, and no
per-tick allocation. Preserve convenient allocating wrappers for host tests if
useful.

## Current implementation

`m7/dmc_msg.*`, `common/LimnmocoProtocol/src/*`, and `common/LimnmocoIK/src/AimPointProtocol.*`.

## Interface and state contracts

- A parser instance owns its bounded assembly buffer and cursor. Feeding a byte chunk reports consumed bytes and zero or more completed frames/errors; incomplete input is normal, not a command failure.
- Frame/payload views have explicit lifetimes. Decode or copy required fields before the next feed can reuse storage. Packet codecs validate structure; domain controllers validate whether an operation is admissible.
- Encoding writes into caller-provided capacity and returns a byte count or error without partial publication. Fixed-point conversions have defined signedness, range, and rounding.

## Validation required for extraction

- Split valid frames at every byte boundary; test multiple frames in one input chunk and resynchronization after corrupt input.
- Cover maximum payload, oversize/truncated lengths, wrong types, checksums, signed endpoints, and exact existing ACK/reply bytes.
- Demonstrate instance isolation and bounded memory; never dispatch a malformed packet.

## Open questions

- What should happen to an incomplete frame when transport disconnects or goes idle?
- Which legacy permissive decodes are compatibility requirements, and which should become separately documented validation fixes?

## Planned commits

[M05](TODO.md#m05), [M06](TODO.md#m06), [M07](TODO.md#m07).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [transport command adapter](transport-command-adapter.md), [kinematics](kinematics.md).
