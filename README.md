# libdedx

[![CI](https://github.com/APTG/libdedx/actions/workflows/ci.yml/badge.svg)](https://github.com/APTG/libdedx/actions/workflows/ci.yml)
[![Coverage](https://codecov.io/gh/APTG/libdedx/branch/main/graph/badge.svg)](https://app.codecov.io/gh/APTG/libdedx/tree/main)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A C library for stopping power calculations (dE/dx) — the energy loss of a charged particle per unit length of material.

All tabulated stopping-power data is embedded into the library. No external
`.bin` files are required at runtime.

Full documentation: https://aptg.github.io/libdedx/

## Supported programs

| Program         | Description                                      | Ions          |
|-----------------|--------------------------------------------------|---------------|
| `DEDX_PSTAR`    | NIST PSTAR database                              | Protons       |
| `DEDX_ASTAR`    | NIST ASTAR database                              | Alpha particles |
| `DEDX_MSTAR`    | MSTAR code                                       | Heavy ions    |
| `DEDX_ICRU49`   | ICRU Report 49 (1993)                            | p, He         |
| `DEDX_ICRU73`   | ICRU Report 73 (2005)                            | Heavy ions    |
| `DEDX_ICRU73_OLD` | ICRU Report 73, older parametrization          | Heavy ions    |
| `DEDX_BETHE_EXT00` | Bethe formula with extensions                 | All ions      |
| `DEDX_ICRU`     | Auto-selects the most current embedded ICRU data | p, He, heavy  |

Note:
For compound targets, `libdedx` currently extends the native coverage of some original programs by falling back to Bragg/stoichiometric weighting when the upstream database does not provide that compound directly. This applies in particular to programs such as `DEDX_PSTAR`, `DEDX_ASTAR`, and `DEDX_MSTAR`, whose original target coverage is more limited than the full set of ICRU/ESTAR-style materials exposed by `libdedx`.

This behavior is useful, but it means that a result returned under a given program label is not always a direct value from the original upstream program. Future releases may separate native program coverage more explicitly from these weighted extension modes.

## Quick start

One-call API for a single stopping power value:

```c
#include <dedx.h>

int err = 0;
float stp = dedx_get_simple_stp(DEDX_PROTON, DEDX_WATER, 100.0f, &err);
// stp in MeV cm² / g at 100 MeV/u
```

Full API with workspace for repeated evaluations:

```c
#include <dedx.h>
#include <stdlib.h>

int err = 0;
dedx_workspace *ws = dedx_allocate_workspace(1, &err);
dedx_config *cfg   = calloc(1, sizeof(dedx_config));

cfg->program = DEDX_PSTAR;
cfg->ion     = DEDX_PROTON;
cfg->target  = DEDX_WATER;

dedx_load_config(ws, cfg, &err);
float stp = dedx_get_stp(ws, cfg, 100.0f, &err);

dedx_free_config(cfg, &err);
dedx_free_workspace(ws, &err);
```

See the [examples/](examples/) directory for more usage patterns.

## Building

Requires CMake 3.21+ and a C11 compiler.

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

To install:

```bash
cmake --preset release -DCMAKE_INSTALL_PREFIX=/your/prefix
cmake --build --preset release
cmake --install build-release
```

Installed CMake consumers can use:

```cmake
find_package(dedx CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE dedx::dedx)
```

## Release artifacts

GitHub release workflows can publish:

- Windows packages with `dedx.dll`, import library, headers, CMake config, and `getdedx`
- Android `libdedx.so` artifacts for supported ABIs
- Linux `.deb` and `.rpm` packages

## License

libdedx is free software, distributed under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0).
