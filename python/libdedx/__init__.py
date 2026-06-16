"""Python binding for the libdedx stopping-power library.

The compiled :mod:`libdedx._core` nanobind extension statically links the
libdedx C library, so importing this package requires no separately installed
shared library. It exposes both a faithful low-level API (the ``Workspace`` and
``Config`` object model plus the module-level functions) and a few high-level
convenience helpers that mirror the historical ctypes binding.
"""

from __future__ import annotations

import numpy as np

from . import _core
from ._core import (
    Config,
    Workspace,
    composition,
    convert_units,
    csda_range_table,
    default_energy_stp_table,
    error_string,
    i_value,
    ion_list,
    ion_name,
    material_list,
    material_name,
    max_energy,
    min_energy,
    program_list,
    program_name,
    program_version,
    simple_stp,
    simple_stp_for_program,
    stp_table,
    stp_table_size,
    version,
    version_string,
)

__all__ = [
    # low-level extension module
    "_core",
    # object model
    "Config",
    "Workspace",
    # low-level functions
    "composition",
    "convert_units",
    "csda_range_table",
    "default_energy_stp_table",
    "error_string",
    "i_value",
    "ion_list",
    "ion_name",
    "material_list",
    "material_name",
    "max_energy",
    "min_energy",
    "program_list",
    "program_name",
    "program_version",
    "simple_stp",
    "simple_stp_for_program",
    "stp_table",
    "stp_table_size",
    "version",
    "version_string",
    # high-level convenience helpers
    "get_version",
    "get_stp",
    "get_stp_table",
    "get_default_table",
    "get_csda_table",
]


def get_version() -> str:
    """Return the libdedx version as a ``major.minor.patch`` string."""
    major, minor, patch = version()
    return f"{major}.{minor}.{patch}"


def get_stp(program: int, ion: int, target: int, energy: float) -> float:
    """Return mass stopping power in MeV cm2/g for a single energy in MeV/nucl."""
    return simple_stp_for_program(program, ion, target, float(energy))


def get_stp_table(program: int, ion: int, target: int, energies) -> np.ndarray:
    """Return stopping powers (MeV cm2/g) for an array of energies (MeV/nucl)."""
    energies = np.ascontiguousarray(energies, dtype=np.float64)
    return stp_table(program, ion, target, energies)


def get_default_table(program: int, ion: int, target: int) -> tuple[np.ndarray, np.ndarray]:
    """Return ``(energies, stps)`` for the built-in tabulated data points.

    ``energies`` are in MeV/nucl and ``stps`` in MeV cm2/g.
    """
    return default_energy_stp_table(program, ion, target)


def get_csda_table(program: int, ion: int, target: int, energies) -> np.ndarray:
    """Return CSDA ranges (g/cm2) for an array of energies (MeV/nucl)."""
    energies = np.ascontiguousarray(energies, dtype=np.float64)
    return csda_range_table(program, ion, target, energies)
