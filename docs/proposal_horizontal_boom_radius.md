# Proposal: Separate Horizontal Boom Radius for Virtual Track Compensation

## Purpose

Improve virtual North/South and East/West track compensation for the Limnmoco
parallelogram-jib crane without changing the Kuper/Dragonframe coordinate
convention, real-axis behavior, or boom compensation table (BCT) semantics.

## Current findings

The virtual track coordinate now follows the Kuper convention:

```text
Forward             = negative Z / negative vTrack
Away from subject   = positive Z / positive vTrack
```

The `virtuals-Z-forward` implementation correctly converts between that public
coordinate and the solver's local boom-forward axis. Hardware testing shows the
reference joint stays very close to the laser datum through a virtual NS move.

The remaining error is geometric. The v3 BCT correctly produces the measured
boom angle: a nominal +200 mm virtual NS move reached approximately 13.56
degrees, consistent with the BCT data.

At that angle, the v3 measurement table gives an empirical real-track
compensation of approximately -22.5 mm. The present analytic model, which uses
the 857.7 mm boom length for both vertical and horizontal geometry, predicts
approximately -23.9 mm. The difference is about 1.4 mm at this point.

The higher-angle v3 samples indicate that vertical and horizontal motion are
best represented by different effective radii:

```text
Vertical boom radius:             857.7 mm
Estimated horizontal boom radius: approximately 808-810 mm
```

This is plausible for the physical parallelogram-jib linkage. It must not be
addressed by changing the BCT: the BCT maps real boom motor position to
geometric boom angle and is already independently validated by the
inclinometer.

## Proposed model

Extend `CraneGeometry` with an optional horizontal boom radius.

```text
verticalBoomLength   = boomLength
horizontalBoomLength = optional calibrated value; defaults to boomLength
```

The shared solver would use:

```text
height             = verticalBoomLength   * sin(boomAngle)
horizontal reach   = horizontalBoomLength * cos(boomAngle) + levelExtension
```

The level jib extension remains horizontal, as it is mechanically compensated
by the parallelogram. It contributes to horizontal reach but does not contribute
to height.

When `horizontalBoomLength` is unset or zero, it must default to `boomLength`.
That preserves the current behavior for every existing crane configuration.

## Measurement and validation plan

1. With physical track locked at zero, move only the boom to several measured
   angles: approximately 10, 13.5, 16, and 20 degrees.
2. Measure the track-direction displacement of the fixed boom-tip/pan-head
   reference joint relative to the datum.
3. Calculate the effective horizontal radius for each point:

   ```text
   horizontalRadius = abs(track compensation) / (1 - cos(boom angle))
   ```

4. Repeat each point while approaching from the same direction, allowing the
   crane to settle before measurement. This reduces mechanical backlash and
   hysteresis effects.
5. Use the repeatable higher-angle samples to choose a calibration value.
   The current v3 data suggests approximately 810 mm.
6. Validate the chosen value with virtual NS moves at +100 mm and +200 mm.
   The reference joint should remain on the laser/string datum within the
   measurement tolerance.

## Implementation plan

1. Add `horizontalBoomLength` to the shared `CraneGeometry` type with the
   backward-compatible fallback described above.
2. Update shared FK and IK so vertical calculations use `boomLength` and
   horizontal reach calculations use the effective horizontal boom radius.
3. Add host tests covering:
   - default behavior when no horizontal radius is supplied;
   - separate vertical and horizontal radii;
   - Kuper `-Z` forward virtual-track conversion remains unchanged;
   - a virtual NS target produces the measured negative raw-track target.
4. Integrate the calibrated value in M7 virtual-only FK/IK and virtual-jog
   paths. Do not modify direct motor commands, raw motor reports, limits, SPU,
   or M4 stepping.
5. Compile M7 and perform the hardware validation above.

## Configuration decision

The current Dragonframe virtual-config packet does not provide a separate
horizontal boom radius. The implementation therefore needs one of these
configuration approaches:

- Initial approach: a clearly documented, crane-specific M7 calibration
  constant.
- Longer-term approach: a backward-compatible local persistent setting or a
  negotiated protocol extension.

The first implementation should isolate this choice behind one M7 accessor so
the solver and protocol boundary do not need to change when persistent
configuration is added.

