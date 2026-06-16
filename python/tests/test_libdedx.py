"""High-level smoke tests for the libdedx Python binding.

Uses PSTAR, hydrogen (Z=1) and liquid water as a reference combination with
well-known stopping power values.
"""

import libdedx

PROGRAM = libdedx._core.PSTAR
ION = libdedx._core.HYDROGEN
TARGET = libdedx._core.WATER_LIQUID


def test_get_version():
    version = libdedx.get_version()
    assert version.count(".") == 2


def test_version_string_nonempty():
    assert isinstance(libdedx.version_string(), str)
    assert libdedx.version_string()


def test_get_stp_returns_positive():
    stp = libdedx.get_stp(PROGRAM, ION, TARGET, 10.0)
    assert stp > 0.0


def test_get_stp_table():
    energies = [1.0, 10.0, 100.0]
    stps = libdedx.get_stp_table(PROGRAM, ION, TARGET, energies)
    assert len(stps) == len(energies)
    assert all(s > 0.0 for s in stps)


def test_get_default_table_nonempty():
    energies, stps = libdedx.get_default_table(PROGRAM, ION, TARGET)
    assert len(energies) > 0
    assert len(energies) == len(stps)
    assert all(e > 0.0 for e in energies)
    assert all(s > 0.0 for s in stps)


def test_get_csda_table():
    energies = [10.0, 100.0]
    ranges = libdedx.get_csda_table(PROGRAM, ION, TARGET, energies)
    assert len(ranges) == len(energies)
    assert all(r > 0.0 for r in ranges)
