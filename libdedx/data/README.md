# Data Layout

`libdedx/data/` is now split by role:

- `embedded/`: generated headers compiled into the library
- `raw/`: raw source tables and metadata used to regenerate embedded data
- `tools/`: generator scripts

At runtime, libdedx uses only the files in `embedded/`.

Typical regeneration flow:

```bash
python3 libdedx/data/tools/pdf2dat.py
python3 libdedx/data/tools/dat2c.py all
```

The first step refreshes ICRU90 intermediate `*.dat` / `*Eng.dat` files in
`raw/`. The second step regenerates the embeddable headers in `embedded/`.

## Retained Raw Files

Not every historical intermediate file is still kept in `raw/`.

- The stopping-power `*.dat` / `*Eng.dat` files are retained as regeneration
  inputs.
- `effective_charge.dat` and `gas_states.dat` are retained as the remaining
  raw metadata inputs behind `embedded/dedx_metadata.h`.
- The older generated intermediates `compos.txt` and `composition` are no
  longer kept, because they were not authoritative external sources and are no
  longer used by the build or regeneration scripts.

## Energy Convention

The generated headers are intended to use `MeV/u` consistently.

For the checked-in alpha tables:

- `raw/ASTAR.dat` + `raw/astarEng.dat`
- `raw/ICRU_ASTAR.dat` + `raw/icru_astarEng.dat`

the `*Eng.dat` grids are already aligned with the `MeV/u` convention used by
libdedx and must not be scaled again during header generation.

## ASTAR vs ICRU_ASTAR

`raw/ASTAR.dat` and `raw/ICRU_ASTAR.dat` are not duplicates.

- The energy grids are identical.
- The target sets are identical.
- The stopping-power values differ slightly.

This means `ICRU_ASTAR` should be kept as a distinct dataset rather than
aliased to `ASTAR`.

## Stilbene Check

The largest difference observed while comparing `raw/ASTAR.dat` and
`raw/ICRU_ASTAR.dat` was:

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
