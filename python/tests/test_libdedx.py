"""
Basic smoke tests for the libdedx Python binding.

Uses PSTAR (program=2), Hydrogen (ion=1), Liquid water (target=276)
as a reference combination with well-known stopping power values.
"""

import libdedx

PROGRAM = 2  # PSTAR
ION = 1  # Hydrogen (Z=1)
TARGET = 276  # Liquid water


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


# TODO: passing an invalid program number currently causes the library to
# segfault rather than returning an error code. A proper test can be added
# once input validation is implemented in the C library.
