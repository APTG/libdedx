"""
libdedx — ctypes binding for the libdedx stopping-power library.

The shared library (libdedx.so / libdedx.dylib / dedx.dll) must be built
and locatable before importing this module. Set the LIBDEDX_SO environment
variable to the full path of the shared library to override library search,
which is useful during development or CI before the library is installed.

Example usage::

    import libdedx
    # PSTAR program (2), Hydrogen ion (1), Liquid water target (276)
    stp = libdedx.get_stp(2, 1, 276, 10.0)   # MeV cm²/g at 10 MeV/u
"""

import ctypes
import ctypes.util
import os


def _load_library():
    path = os.environ.get("LIBDEDX_SO")
    if path:
        return ctypes.CDLL(path)
    name = ctypes.util.find_library("dedx")
    if name is None:
        raise OSError(
            "libdedx shared library not found. "
            "Install libdedx or set LIBDEDX_SO to the path of the shared library."
        )
    return ctypes.CDLL(name)


_lib = _load_library()

_c_int_p = ctypes.POINTER(ctypes.c_int)
_c_float_p = ctypes.POINTER(ctypes.c_float)
_c_double_p = ctypes.POINTER(ctypes.c_double)

_lib.dedx_get_simple_stp_for_program.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_float, _c_int_p,
]
_lib.dedx_get_simple_stp_for_program.restype = ctypes.c_float

_lib.dedx_get_stp_table_size.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
_lib.dedx_get_stp_table_size.restype = ctypes.c_int

_lib.dedx_get_stp_table.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int, _c_float_p, _c_float_p,
]
_lib.dedx_get_stp_table.restype = ctypes.c_int

_lib.dedx_fill_default_energy_stp_table.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int, _c_float_p, _c_float_p,
]
_lib.dedx_fill_default_energy_stp_table.restype = ctypes.c_int

_lib.dedx_get_csda_range_table.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int, _c_float_p, _c_double_p,
]
_lib.dedx_get_csda_range_table.restype = ctypes.c_int

_lib.dedx_get_version.argtypes = [_c_int_p, _c_int_p, _c_int_p]
_lib.dedx_get_version.restype = None


def _check(err):
    if err.value != 0:
        raise RuntimeError(f"libdedx error code {err.value}")


def get_version():
    """Return the libdedx version as a ``major.minor.patch`` string."""
    major = ctypes.c_int(0)
    minor = ctypes.c_int(0)
    patch = ctypes.c_int(0)
    _lib.dedx_get_version(ctypes.byref(major), ctypes.byref(minor), ctypes.byref(patch))
    return f"{major.value}.{minor.value}.{patch.value}"


def get_stp(program, ion, target, energy):
    """Return mass stopping power in MeV cm²/g for a single energy in MeV/u."""
    err = ctypes.c_int(0)
    result = _lib.dedx_get_simple_stp_for_program(
        program, ion, target, float(energy), ctypes.byref(err)
    )
    _check(err)
    return float(result)


def get_stp_table(program, ion, target, energies):
    """Return stopping powers (MeV cm²/g) for a list of energies (MeV/u)."""
    n = len(energies)
    e_arr = (ctypes.c_float * n)(*energies)
    s_arr = (ctypes.c_float * n)()
    ret = _lib.dedx_get_stp_table(program, ion, target, n, e_arr, s_arr)
    if ret != 0:
        raise RuntimeError(f"libdedx error code {ret}")
    return list(s_arr)


def get_default_table(program, ion, target):
    """Return (energies, stps) for the built-in tabulated data points.

    energies — list of energies in MeV/u
    stps     — list of stopping powers in MeV cm²/g
    """
    n = _lib.dedx_get_stp_table_size(program, ion, target)
    if n < 0:
        raise RuntimeError(f"libdedx error code {n}")
    if n == 0:
        raise RuntimeError(
            f"No tabulated data for program={program}, ion={ion}, target={target}"
        )
    e_arr = (ctypes.c_float * n)()
    s_arr = (ctypes.c_float * n)()
    ret = _lib.dedx_fill_default_energy_stp_table(program, ion, target, e_arr, s_arr)
    if ret < 0:
        raise RuntimeError(f"libdedx error code {ret}")
    return list(e_arr), list(s_arr)


def get_csda_table(program, ion, target, energies):
    """Return CSDA ranges (g/cm²) for a list of energies (MeV/u)."""
    n = len(energies)
    e_arr = (ctypes.c_float * n)(*energies)
    r_arr = (ctypes.c_double * n)()
    ret = _lib.dedx_get_csda_range_table(program, ion, target, n, e_arr, r_arr)
    if ret != 0:
        raise RuntimeError(f"libdedx error code {ret}")
    return list(r_arr)
