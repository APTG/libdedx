"""Tests for the low-level libdedx._core API (workspace/config object model)."""

import numpy as np
import pytest

from libdedx import _core

PSTAR = _core.PSTAR
HYDROGEN = _core.HYDROGEN
WATER = _core.WATER_LIQUID


def test_version_tuple():
    major, minor, patch = _core.version()
    assert all(isinstance(v, int) for v in (major, minor, patch))


def test_names_and_lists():
    assert isinstance(_core.program_name(PSTAR), str)
    assert _core.program_name(PSTAR)
    assert isinstance(_core.material_name(WATER), str)
    assert isinstance(_core.ion_name(HYDROGEN), str)

    programs = _core.program_list()
    assert PSTAR in programs
    materials = _core.material_list(PSTAR)
    assert len(materials) > 0
    ions = _core.ion_list(PSTAR)
    assert HYDROGEN in ions


def test_energy_bounds():
    lo = _core.min_energy(PSTAR, HYDROGEN)
    hi = _core.max_energy(PSTAR, HYDROGEN)
    assert 0.0 < lo < hi


def test_accessors():
    iv = _core.i_value(WATER)
    assert iv > 0.0
    comp = _core.composition(WATER)
    assert comp.ndim == 2 and comp.shape[1] == 2
    assert comp.shape[0] >= 2  # water has at least H and O
    # mass fractions should roughly sum to 1
    assert abs(comp[:, 1].sum() - 1.0) < 1e-2


def test_workspace_config_roundtrip():
    ws = _core.Workspace(4)
    cfg = _core.Config()
    cfg.program = PSTAR
    cfg.ion = HYDROGEN
    cfg.target = WATER
    ws.load(cfg)
    assert cfg.loaded
    assert cfg.cfg_id >= 0
    assert cfg.ion_a > 0
    assert cfg.program_name
    assert cfg.target_name

    stp = ws.stp(cfg, 100.0)
    assert stp > 0.0


def test_csda_and_inverse():
    ws = _core.Workspace(8)
    cfg = _core.Config()
    cfg.program = PSTAR
    cfg.ion = HYDROGEN
    cfg.target = WATER
    ws.load(cfg)

    energy = 100.0
    rng = ws.csda(cfg, energy)
    assert rng > 0.0

    recovered = ws.inverse_csda(cfg, rng)
    assert recovered == pytest.approx(energy, rel=1e-2)

    stp = ws.stp(cfg, energy)
    e_high = ws.inverse_stp(cfg, stp, side=1)
    assert e_high == pytest.approx(energy, rel=5e-2)


def test_custom_compound_mass_fraction():
    # Water defined as a custom compound: H and O by mass fraction.
    ws = _core.Workspace(2)
    cfg = _core.Config()
    cfg.program = PSTAR
    cfg.ion = HYDROGEN
    cfg.target = 0  # custom compound
    cfg.elements_id = [1, 8]
    cfg.elements_mass_fraction = [0.111894, 0.888106]
    cfg.rho = 1.0
    ws.load(cfg)
    assert cfg.loaded
    assert cfg.bragg_used

    stp = ws.stp(cfg, 100.0)
    assert stp > 0.0


def test_custom_compound_by_atoms():
    # Water by atom counts (H2O).
    ws = _core.Workspace(2)
    cfg = _core.Config()
    cfg.program = PSTAR
    cfg.ion = HYDROGEN
    cfg.target = 0
    cfg.elements_id = [1, 8]
    cfg.elements_atoms = [2, 1]
    cfg.rho = 1.0
    ws.load(cfg)
    stp = ws.stp(cfg, 100.0)
    assert stp > 0.0
    # mass fractions get derived from the atom counts during load
    frac = cfg.elements_mass_fraction
    assert len(frac) == 2
    assert frac[1] > frac[0]


def test_convert_units():
    values = np.array([50.0, 25.0], dtype=np.float64)
    # MeV cm2/g -> keV/um for liquid water (rho ~ 1 g/cm3): factor ~ 0.1
    converted = _core.convert_units(_core.MEVCM2G, _core.KEVUM, WATER, values)
    assert converted.shape == values.shape
    assert np.all(converted > 0.0)
    # round trip
    back = _core.convert_units(_core.KEVUM, _core.MEVCM2G, WATER, converted)
    assert np.allclose(back, values, rtol=1e-4)
    # identity conversion returns the values unchanged
    same = _core.convert_units(_core.MEVCM2G, _core.MEVCM2G, WATER, values)
    assert np.allclose(same, values)


def test_stp_table_matches_scalar():
    energies = np.array([1.0, 10.0, 100.0], dtype=np.float64)
    table = _core.stp_table(PSTAR, HYDROGEN, WATER, energies)
    assert table.shape == energies.shape
    scalar = _core.simple_stp_for_program(PSTAR, HYDROGEN, WATER, 10.0)
    assert table[1] == pytest.approx(scalar, rel=1e-5)


def test_error_string():
    assert isinstance(_core.error_string(0), str)
    assert _core.error_string(201)  # target not found
