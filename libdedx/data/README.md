# Data Notes

This directory contains the raw `*.dat` / `*Eng.dat` source tables and the
generated embedded `dedx_*.h` headers used by libdedx.

## Energy Convention

The generated headers are intended to use `MeV/u` consistently.

For the checked-in alpha tables:

- `ASTAR.dat` + `astarEng.dat`
- `ICRU_ASTAR.dat` + `icru_astarEng.dat`

the `*Eng.dat` grids are already aligned with the `MeV/u` convention used by
libdedx and must not be scaled again during header generation.

## ASTAR vs ICRU_ASTAR

`ASTAR.dat` and `ICRU_ASTAR.dat` are not duplicates.

- The energy grids are identical.
- The target sets are identical.
- The stopping-power values differ slightly.

This means `ICRU_ASTAR` should be kept as a distinct dataset rather than
aliased to `ASTAR`.

## Stilbene Check

The largest difference observed while comparing `ASTAR.dat` and
`ICRU_ASTAR.dat` was:

- ion: `He` (`Z = 2`)
- target: `STILBENE` (`id = 255`)
- energy: `1.0 MeV/u`
- `ASTAR.dat`: `1.0155e+03 MeV cm^2/g`
- `ICRU_ASTAR.dat`: `1.0160e+03 MeV cm^2/g`
- relative difference: about `4.9e-4` (`0.049%`)

Manual cross-check:

- NIST ASTAR matches the `ASTAR.dat` value (`1.015e+03`)
- ICRU49 matches the `ICRU_ASTAR.dat` value (`1.016e+03`)

So the small difference is real and source-driven.
