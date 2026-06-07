# libdedx Python binding

`libdedx` provides Python bindings for the
[libdedx](https://github.com/APTG/libdedx) charged-particle stopping-power
library. The bindings are a [nanobind](https://nanobind.readthedocs.io)
extension built with
[scikit-build-core](https://scikit-build-core.readthedocs.io). The libdedx C
library is **statically linked** into the extension and its data tables are
embedded, so wheels are self-contained — no separate shared library or system
install is required.

## Installation

```bash
pip install libdedx
```

## Quick start

```python
import libdedx
from libdedx import _core as dedx

# High-level convenience helpers
print(libdedx.get_version())
stp = libdedx.get_stp(dedx.PSTAR, dedx.HYDROGEN, dedx.WATER_LIQUID, 100.0)  # MeV cm2/g

# Low-level workspace/config object model
ws = dedx.Workspace()
cfg = dedx.Config()
cfg.program = dedx.PSTAR
cfg.ion = dedx.HYDROGEN
cfg.target = dedx.WATER_LIQUID
ws.load(cfg)

stp = ws.stp(cfg, 100.0)            # mass stopping power, MeV cm2/g
rng = ws.csda(cfg, 100.0)           # CSDA range, g/cm2
energy = ws.inverse_csda(cfg, rng)  # invert the range back to energy
```

Custom compounds, inverse stopping power, unit conversion, the
program/ion/material lists and names, composition and I-value accessors are all
exposed through `libdedx._core`. See `python/tests/` for further examples.

## Development

The whole project (C library + Python extension) is configured from the
top-level `pyproject.toml`. From the repository root:

```bash
pip install -e ".[dev]"   # builds the nanobind extension in place
pytest python/tests
ruff check .
```
