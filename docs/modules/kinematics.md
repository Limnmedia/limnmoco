<!--
 Copyright 2026 cade.
 SPDX-License-Identifier: GPL-3.0-only
-->

# Kinematics and coordinate geometry

[Architecture overview](README.md) · [Global TODO](TODO.md)

Status: proposed boundary; extraction is not complete. Interface names and
contracts below are design targets, not existing APIs.

## Responsibility and ownership

Own pure FK/IK, crane geometry, nodal offsets, coordinate-frame transformations,
aim geometry, and camera-line direction calculations. Accept physical joint
coordinates and geometry; return a pose or joint solution with explicit validity
information. Do not know motor indices, packet scales, GPIO, or current motion.

Keep solver/public Kuper conversions explicit. Avoid reusing an unqualified
`Vec3` or `VirtualPose` where its frame or units are ambiguous. Lightweight types
or descriptive field names are sufficient; a general units framework is not a
prerequisite.

The proposed [horizontal boom radius](../proposal_horizontal_boom_radius.md) fits
in `CraneGeometry`. Its implementation and calibration remain a separate behavior
change from module extraction.

## Current implementation

`LimnmocoIK.*`, `KuperTrackConvention.*`, `AimGeometry.*`, and `CameraLine.*` under `common/LimnmocoIK/src/`.

## Interface and state contracts

- Inputs are a geometry snapshot and physical joint coordinates or a pose in a named frame. Results distinguish a reachable solution from clamping, invalid geometry, and non-finite inputs.
- Public Kuper coordinates, solver coordinates, and camera-local directions are explicit at conversion points. Origin offsets are supplied values; the solver does not capture or mutate an origin.
- No step counts, axis IDs, BCT tables, wire scales, or motion state enter the solver. Aim geometry returns orientation and geometric validity; the Virtual module owns persistent aim behavior.

## Validation required for extraction

- Preserve named FK/IK round trips, coordinate signs, rotation order, nodal offsets, and geometry rejection cases.
- Exercise camera-local directions and aim geometry independently of motors and packets.
- Keep physical calibration tolerances out of numerical solver tolerances.

## Open questions

- Which result statuses should replace existing clamped flags without changing accepted targets?
- How should frame-specific pose types be introduced with minimal churn to fixtures?

## Planned commits

[M03](TODO.md#m03), [M23](TODO.md#m23).

The global TODO owns completion status and commit order. Record module-specific
decisions here with rationale and the relevant commit ID; do not maintain a second
progress checklist.

## Discussion and decisions

No additional decisions recorded yet. Add dated entries here as this boundary is
reviewed; update the contracts above when a decision is accepted.

Related boundaries: [mathematics](mathematics.md), [Calibration](calibration.md), [Virtual](virtual.md).
