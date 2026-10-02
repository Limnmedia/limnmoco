<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Mathematics

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own general-purpose vectors, matrices, rotations, angle utilities, and narrowly
shared numerical helpers. Functions are pure and have no crane, motor, protocol,
or Arduino dependency. State units and rotation conventions at API boundaries.

Move the existing shared `Vec3`/`Mat3` implementation here; retire M7 forwarding
wrappers after consumers migrate. Crane-specific coordinate mappings belong to
kinematics. Trajectory equations belong to motion, even though they use math.

## Current implementation

`common/LimnmocoIK/src/Vec3.*`, `Mat3.*`, angle helpers, and forwarding wrappers in `m7/vec3.*` and `m7/mat3.*`.

## Interface and state contracts

- Use small value types and pure functions. Public headers contain no protocol constants, motor types, or mutable singleton state.
- Document storage order, multiplication order, angle units, and rotation handedness. A geometry-specific rotation composition remains in kinematics.
- Preserve the existing float precision and numerical behavior during extraction. Tolerance changes or alternate algorithms require explicit regression evidence.

## Validation required for extraction

- Retain all existing numerical fixtures and add focused checks only for newly exposed mathematical contracts.
- Build a consumer using only the math target and verify it does not transitively require firmware or kinematics.
- Compare firmware size and runtime where inlining or linkage changes affect frequently executed code.

## Open questions

- Which helpers have more than one real consumer and therefore belong here rather than beside their algorithm?
- Should existing angle conversion names change, or remain as compatibility wrappers until consumers migrate?

## Planned commits

[M02](TODO.md#m02), [M23](TODO.md#m23).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [kinematics](kinematics.md), [motion](motion.md).
