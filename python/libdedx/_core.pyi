"""Type stubs for the libdedx._core nanobind extension."""

from typing import Optional

import numpy as np
import numpy.typing as npt

# --- program identifiers ---
ASTAR: int
PSTAR: int
ESTAR: int
MSTAR: int
ICRU73_OLD: int
ICRU73: int
ICRU49: int
ICRU: int
DEFAULT: int
BETHE_EXT00: int

# --- aggregate states ---
DEFAULT_STATE: int
GAS: int
CONDENSED: int

# --- MSTAR modes ---
MSTAR_MODE_A: int
MSTAR_MODE_B: int
MSTAR_MODE_G: int
MSTAR_MODE_H: int
MSTAR_MODE_C: int
MSTAR_MODE_D: int
MSTAR_MODE_DEFAULT: int

# --- interpolation modes ---
INTERPOLATION_LOG_LOG: int
INTERPOLATION_LINEAR: int
INTERPOLATION_DEFAULT: int

# --- stopping-power units ---
MEVCM2G: int
MEVCM: int
KEVUM: int

# --- common ions / materials ---
HYDROGEN: int
PROTON: int
HELIUM: int
CARBON: int
WATER: int
WATER_LIQUID: int
WATER_VAPOR: int
AIR: int

class Config:
    def __init__(self) -> None: ...
    program: int
    target: int
    ion: int
    compound_state: int
    interpolation_mode: int
    mstar_mode: int
    i_value: float
    rho: float
    elements_id: list[int]
    elements_atoms: list[int]
    elements_mass_fraction: list[float]
    elements_i_value: list[float]
    ion_a: int
    @property
    def cfg_id(self) -> int: ...
    @property
    def bragg_used(self) -> bool: ...
    @property
    def loaded(self) -> bool: ...
    @property
    def target_name(self) -> Optional[str]: ...
    @property
    def ion_name(self) -> Optional[str]: ...
    @property
    def program_name(self) -> Optional[str]: ...

class Workspace:
    def __init__(self, count: int = 1) -> None: ...
    def load(self, config: Config) -> int: ...
    def stp(self, config: Config, energy: float) -> float: ...
    def csda(self, config: Config, energy: float) -> float: ...
    def inverse_stp(self, config: Config, stp: float, side: int) -> float: ...
    def inverse_csda(self, config: Config, range: float) -> float: ...
    @property
    def datasets(self) -> int: ...
    @property
    def active_datasets(self) -> int: ...

def version() -> tuple[int, int, int]: ...
def version_string() -> str: ...
def error_string(err: int) -> str: ...
def program_name(program: int) -> str: ...
def program_version(program: int) -> str: ...
def material_name(material: int) -> str: ...
def ion_name(ion: int) -> str: ...
def program_list() -> list[int]: ...
def material_list(program: int) -> list[int]: ...
def ion_list(program: int) -> list[int]: ...
def min_energy(program: int, ion: int) -> float: ...
def max_energy(program: int, ion: int) -> float: ...
def i_value(target: int) -> float: ...
def composition(target: int) -> npt.NDArray[np.float64]: ...
def simple_stp(ion: int, target: int, energy: float) -> float: ...
def simple_stp_for_program(program: int, ion: int, target: int, energy: float) -> float: ...
def stp_table_size(program: int, ion: int, target: int) -> int: ...
def stp_table(program: int, ion: int, target: int, energies: npt.ArrayLike) -> npt.NDArray[np.float64]: ...
def csda_range_table(
    program: int, ion: int, target: int, energies: npt.ArrayLike
) -> npt.NDArray[np.float64]: ...
def default_energy_stp_table(
    program: int, ion: int, target: int
) -> tuple[npt.NDArray[np.float64], npt.NDArray[np.float64]]: ...
def convert_units(
    old_unit: int, new_unit: int, material: int, values: npt.ArrayLike
) -> npt.NDArray[np.float64]: ...
