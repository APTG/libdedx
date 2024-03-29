===============
This is libdEdx
===============

.. contents:: Table of Contents
   :local: 
   :backlinks: none


*************************
0. License and disclaimer
*************************
|  libdEdx is licensed under GPL version 3
|  (c) Copyright 2010-2021 by
|  the libdEdx team
|  https://github.com/APTG/libdedx/graphs/contributors

|  This program is free software: you can redistribute it and/or modify
|  it under the terms of the GNU General Public License as published by
|  the Free Software Foundation, either version 3 of the License, or
|  (at your option) any later version.

|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.

|  You should have received a copy of the GNU General Public License
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************
0.1 Acknowledgements
********************
We are grateful for the support by Prof. Helmut Paul from the University of 
Linz, Austria. 
Niels Bassler acknowledges support from the Danish Cancer Society.

***************
1. Introduction
***************

libdEdx is an easy to use library which provides electronic stopping powers of fast charged particles.
It is a collection of commonly known stopping power tables and stopping-power routines.
You can either use it as a library, by including the :code:`<dedx.h>` file and linking against the dedx library,
or manually access a value with the :code:`getdedx` command.

A testing version of the web front end is available on https://aptg.github.io/web_dev/

*****************
2. Data available
*****************

* ASTAR:
   :Energy range: 0.25 keV/u to 250 MeV/u
   :Particles: only helium nuclei
   :Targets: See astar_targets


* PSTAR:
   :Energy range: 1 keV/u to 10 GeV/u
   :Particles: only protons
   :Targets: See pstar_and_astar_targets

* ESTAR:
   :Energy range: 1 keV/u to 10 GeV/u
   :Particles: only electrons
   :Targets: See estar_targets

* ICRU:
   :Energy range: 25 keV/u to 1 GeV/u
   :Particles: See icru73_particle_targets
   :Targets: See icru73_particle_targets

* BETHE_EXT00: 
   :Comment: Bethe equation with Linhard-Scharff at lower energies. BETHE_EXT00 is optimized for particle therapy, i.e. works well at lower energies and lighter particles and targets. It is not recommended for heavy targets and heavy/fast ions. BETHE_EXT00 is the same algorithm used in SHIELD-HIT12A, but default I-values may vary.
   :Energy range: 1 keV/u to 1 GeV/u
   :Particles: Any, but will be most correct at light ions
   :Targets: Any, but will be most correct at light targets

For all data tables and equations, it is possible to use Bragg's additivity rule, if a specific target is unavailable by default.

***************
3. Installation
***************

Configuration and installation are handled with CMAKE.
Install CMake from your local Linux distribution, or get it from http://www.cmake.org if using Windows

From source code::

   1) go to the directory where you have unpacked libdedx
   2) cmake -S . -B build
   3) cmake --build build

which will build the .so or .dll file depending on the platform.
Additionally, the example and auxiliary programs will be built, and the binary data tables will be generated.

Finally, run::

   5) cmake --install build

this will copy the binary tables and tables to the relevant places on your system. On Linux, you will need to prefix this command with `sudo` or run it as root.


******
4. Use
******

UNITS: Energies are always in terms of MeV/nucleon, except for ESTAR, where the electron energy must be specified in terms of MeV. The resulting mass stopping power is in MeV cm2/g.

Stopping power values can be retrieved in two different ways: 

1. a simple method for simple implementation, 
2. a bit more complicated way, but more suitable for fast and multithreading applications.

Method 1) involves a single function call:

.. code-block:: C
        
        float dedx_get_simple_stp(int ion, 
				  int target, 
				  float energy, 
				  int * err);

The function returns the stopping power of the specific configuration, 
using the ICRU49 and ICRU73 data table if possible, else the BETHE_EXT00 table.

:ion: is the Z value of the particle
:target: is the id of the target, the id follows the ICRU naming convention the id can be found in the respective target files for each program.
:energy: is the kinetic energy for the ion per nucleon (MeV/nucl).
:\*err: contains the error code and is 0 if no error was encountered. An explanation of the error codes can be found in the error_codes.txt file.

Thus

.. code-block:: C

	a = dedx_get_simple_stp(DEDX_CARBON, DEDX_WATER, 100.0, &err);

will store the mass stopping power of a 100 MeV/u carbon ion in a water target.

A list of ions and target media is given in appendix A.3.


Method 2) is the proper method of using dedx if several stopping powers are to be retrieved. It is (hopefully) thread-safe and was optimized to be fast.

First memory for a workspace and a config struct must be allocated.

.. code-block:: C

  dedx_workspace *ws;
  dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));

  ws = dedx_allocate_workspace(int datasets, int *err)

The function returns a struct of the type ``dedx_workspace *ws``

:datasets: is the number of ion - target combinations you want to use at the same time.
:\*err: pointer to an integer holding an error code.

Next you must initialize your configuration, by writing to the cfg struct.

.. code-block:: C

   typedef struct
   {
      int cfg_id;
      int program;
      int target;            // target can either be an element or a compound
      int ion;               // id number of projectile
      int ion_a;             // nucleon number of projectile
      int bragg_used;        // is 1 if Braggs additivity rule was applied
      int compound_state;    // DEDX_DEFAULT=0,  DEDX_GAS DEDX_CONDENSED ... 
      unsigned int elements_length;   // elements_length  --- number of unique elements in comp.
      int * elements_id;     // elements_id      --- Z of each element
      int * elements_atoms;  // elements_atoms   --- number of atoms per comp. unit
      char mstar_mode;
      float i_value;         // i_value   --- mean excitation potential of target 
      float rho;
      float * elements_mass_fraction;     // mass_fraction of each element
      float * elements_i_value;           // i_value of each element
      const char * target_name;
      const char * ion_name;
      const char * program_name;
   } dedx_config;


Description of the elements:

:cfg_id: configuration id, which is set by ``dedx_config``. Don't touch.

:program: must be specified by the user, see appendix A.3

:target: see appendix A.3. If another compound is requested which is not in the list, then the user must specify compound constituents by ``*elements_id`` and either ``*elements_mass_fraction`` or ``*elements_atoms``.

:ion: must be specified by the user, see appendix A.3

:compound: is set to ``TRUE`` by ``dedx_load_config()``, if the target was not found in the default list, but generated from individual elements instead.

:compound_state: is assumed to be DEDX_DEFAULT_STATE, which means normal state of 
 aggregation. It could also be: DEDX_GAS or DEDX_CONDENSED
 From version 1.2.1 the state parameter (i.e. the I-values) 
 of the Bethe function will be affected, but only in the case 
 where ``element_id`` is specified and ``element_i_value`` is not. 
 This difference applies for elements which is naturally found
 in gas state, following ICRU49 recommendations. The I-value is 
 multiplied with 1.13 to get the liquid/solid I-value phase,
 except for the following elements, where these I-values are used in 
 condensed phase:

  - Hydrogen:	21.8 eV
  - Carbon:		81 eV
  - Nitrogen:	82 eV
  - Oxygen:		106 eV
  - Fluor		112 eV
  - Chlorine	180 eV

 ICRU49 is ambiguous here since it also recommends using 19.2 eV for
 liquids in table 2.11, which contradicts 21.8 eV from table 2.8. 
 Moreover, oxygen is stated as 95.0 eV in table 2.8 and 97 eV for gasses in table 2.11. Here, table 2.8 is used in case of ambiguous values,
 since libdEdx does not discriminate between the I-values of elements and atomic constituents in compounds. If other values are needed they can be specified with the ``*elements_i_value parameter``.
 When using MSTAR read the ``mstar_mode`` function carefully too.
 The compound_state will apply equally to all constituents when working with compounds.
                
:elements_length: number of unique elements in a compound. Must be specified if the target is undefined (`DEDX_UNDEFINED`)

:\*elements_id: Z of each constituent element, must be specified if target is undefined

:\*elements_atoms: number of atoms per comp. unit must be specified if the target is undefined.

:mstar_mode: MSTAR features several modes of operation, depending on the state of the compound.

 :DEDX_MSTAR_MODE_A: will work for most compounds. Automatic selection of state, depending on the state table in the appendix. This mode 'a' will select 'g' mode for gas phase and 'c' mode for condensed phase.
 :DEDX_MSTAR_MODE_B: recommended and default mode of operation. However, not all elements work. This mode 'b' will select 'h' for gas and 'd' for condensed phase, depending on the state table in the appendix.
 :DEDX_MSTAR_MODE_C: Condensed phase for 'a' mode.
 :DEDX_MSTAR_MODE_D: Condensed phase for recommended 'b' mode.
 :DEDX_MSTAR_MODE_G: Gas phase for 'a' mode.
 :DEDX_MSTAR_MODE_H: Gas phase for recommended 'b' mode.

 if `DEDX_DEFAULT`, then 'b' method of MSTAR is used, as recommended by MSTAR author Helmut Paul. In case of an overspecified, or even 
 contradicting system (e.g. DEDX_GAS was set in compound_state and
 DEDX_MSTAR_MODE_D  mode requested), then libdEdx will follow 
 mstar_mode and ignore compound_state.

 The condensed modes 'c' or 'd' will be selected if
 DEDX_CONDENSED is requested in compound_state. 'c' is the condensed
 phase for the 'a' mode of MSTAR. 'd' is the same for the recommended
 'b' mode of operation. The value in mstar_mode will be updated accordingly after dedx_load_config() was applied.
		
 The 'd' mode is not allowed on Hydrogen, Helium and Lithium. In that case
 libdEdx will switch to 'c' mode. mstar_mode will NOT be updated in this case.
 The reason is, that when 'd' was requested for a compound, then only the elements Hydrogen, Helium and Lithium will be affected, leaving all other elements in 'd' mode. 

 If DEDX_GAS is requested, then 'g' or 'h' is attempted,
 depending on if 'a' or 'b' mode was requested, respectively. The value in
 mstar_mode will be updated, accordingly, after dedx_load_config() was applied.

 However, for Hydrogen and Helium targets, only the 'g' mode is allowed for DEDX_GAS in MSTAR, i.e. 'h' mode is not allowed. 
 libdEdx will then switch to 'g' in that case. mstar_mode will NOT be updated in this case. E.g. when working with a compound with 'h' 
 requested, only Hydrogen and Helium will be calculated using 'g' mode,
 and all other constituents remain in 'h' mode.
 Confusing? Yes.

:i_value: if unspecified, then ICRU I-values are used for target 
  compound. If target is set, or if target is 0, then it is 
  calculated from the individual i-values set in 
  ``*elements_i_value``, but only when the ``*elements_i_value are empty``,
  i.e. uninitialized.

:\*elements_mass_fraction: must be specified if target and elements_atoms is 
			 left undefined. If both are specified, then only 
			 elements_mass_fraction is considered, and element_atoms
			 is ignored entirely. Mass fraction is the summed atomic 
			 mass of a constituing element, divided by the total 
			 atomic mass of the compound.

:\*elements_i_value: if target is 0, then individual I-values of elements can 
		   be specified here. If any values are found in 
		   ``*elements_i_value``, then i_value is ignored. Zero is not allowed. If any
		   of the I-values are specified, then they must be specified for all 
		   elements.

As a minimum, you should specify program, target and ion, i.e.

.. code-block:: C

   cfg->ion = DEDX_CARBON;
   cfg->program = DEDX_ICRU;
   cfg->target = DEDX_PMMA;

and then load the config

.. code-block:: C

    void dedx_load_config(dedx_workspace *ws, 
                          dedx_config *config, 
                          int *err);

which will initialize the remaining configure options which may be needed.
The options can be probed by the user, but beware that some hold NULL pointers.

You have to call ``dedx_load_config()`` for each target/ion combination. 
If multiple combinations are used, you must allocate memory for each 
``*config`` element, and call ``dedx_load_config()`` for each configuration.
Since it, there had been observed some misbehave of the library using
malloc for allocating memory to the config struct, it is recommended 
to use ``calloc`` or similar.

Stopping power values are returned by:

.. code-block:: C

        float dedx_get_stp(dedx_workspace *ws, 
	                   int config, 
			   float energy, 
			   int *err)

energy: kinetic energy of a particle in MeV/nucleon.

When you are done with the library you have to run 

.. code-block:: C

        dedx_free_workspace(dedx_workspace *ws, int *err);
	dedx_free_config(dedx_config * config, int *err);

to free the allocated memory.

- Bragg additivity rule:
  Braggs additivity rule is applied automatically if you request a target material that is not on the list in that particular stopping power routine. 

- Own compounds:
  You can set up your own compounds by specifying each element in the dedx_config struct. Here is an example for water, set up by mass fraction:

.. code-block:: C

	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->prog = DEDX_ASTAR;
	config->ion = DEDX_HELIUM;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_mass_fraction = calloc(2,sizeof(float));
	config->elements_mass_fraction[0] = 0.111894;
	config->elements_mass_fraction[1] = 0.888106; 
	config->elements_length = 2;

Mass fractions are particularly useful if you want to use special
isotopic compositions, instead of natural compositions.

Alternatively, you can set it up by the relative amount of elements:

.. code-block:: C

	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->prog = DEDX_BETHE_EXT00;
	config->ion = DEDX_HELIUM;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_atoms = calloc(2,sizeof(int));
	config->elements_atoms[0] = 2;
	config->elements_atoms[1] = 1;
	config->elements_length = 2;

Then libdEdx will use the natural isotope compositions, e.g. 12.0107 for natural 
carbon which also contains C-13 and C-14.

- Overriding I-value:
  Instead of using the default, I value for a compound, determined by either the 
  predefined ICRU material list or Braggs additivity rule of the compound, you
  can specify the I-value manually for the BETHE-type algorithms:

.. code-block:: C

	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->prog = DEDX_BETHE_EXT00;
	config->ion = DEDX_HELIUM;
	config->i_value = 78.0;                  // new I-value in eV
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_atoms = calloc(2,sizeof(int));
	config->elements_atoms[0] = 2;
	config->elements_atoms[1] = 1;
	config->elements_length = 2;

****************
5. Release notes
****************

Version: 1.2.1
==============

Changes:
 - several bug fixes regarding the state of the compound when using Bragg's rule.
 - better testing of library
 - completed the ICRU material list on which elements is on the gas phase, see
   Appendix 

Version: 1.2
============
Changes:
 - New API, which should be more stable for future enhancements
 - I-values can be specified for compounds
 - bound checking
 - functions for compound data look-up, version number and energy bounds
 - dedx_tools.h for inverse look-ups
 - should be thread-safe
 - bug fixes
 - memory leak fixes
 - Python bindings
Known limitations:
 - ESTAR is still not implemented.

Version 1.0.1
=============
Known limitations:
 - ESTAR is not implemented
 - WIN32/MINGW build not tested, this will be a UNIX/LINUX only release.
 - Bethe function: I-value can only be set for elements, not compounds.

*******
A.1 API
*******

List of functions available in dedx.h:

.. code-block:: C

  dedx_workspace * dedx_allocate_workspace(unsigned int count, int *err);
  void             dedx_free_config(dedx_config *config, int *err);
  void             dedx_free_workspace(dedx_workspace *ws, int *err);
  void             dedx_get_composition(int target, float composition[][2], unsigned int * comp_len, int *err);
  void             dedx_get_error_code(char *err_str, int err);
  float            dedx_get_i_value(int target, int *err);
  const int *      dedx_get_ion_list(int program);
  const char *     dedx_get_ion_name(int ion);
  const int *      dedx_get_material_list(int program);
  const char *     dedx_get_material_name(int material);
  float            dedx_get_min_energy(int program, int ion);
  float            dedx_get_max_energy(int program, int ion);
  const int *      dedx_get_program_list(void);
  const char *     dedx_get_program_name(int program);
  const char *     dedx_get_program_version(int program);
  float            dedx_get_simple_stp(int ion, int target, float energy, int *err);
  float            dedx_get_stp(dedx_workspace *ws, dedx_config *config, float energy, int *err);
  void             dedx_get_version(int *major, int *minor, int *patch);
  void             dedx_load_config(dedx_workspace *ws, dedx_config *config, int *err);

***************
A.2 Error codes
***************

- 1-100 IO error
- 101-200 Out of bounds errors
- 201-300	invalid input

- 1 Composition file compos.txt does not exist
- 2 MSTAR file mstar_gas_states.dat does not exist
- 3 MSTAR effective_charge.dat file does not exist
- 4 Unable to access binary data file
- 5 Unable to access binary energy file
- 6 Unable to write to disk
- 7 Unable to read energy file 
- 8 Unable to read data file 
- 9 Unable to read short_names file
- 10 Unable to read composition file

- 101 Energy out of bounds 

- 201 Target is not in composition file
- 202 Target and ion combination is not in data file
- 203 ID does not exist
- 204 Target is not an atomic element
- 205 ESTAR is not implemented yet
- 206 Ion is not supported for MSTAR
- 207 Ion is not supported for requested table
- 208 Rho must be specified in this configuration.
- 209 Mass of ion (ion_a) must be specified in this configuration.
- 210 I value must be larger than zero.

********************
A.3 Additional lists
********************
All names can be prefixed with ``DEDX_``

List all known data tables and algorithms:

|   0 (N/A)
|   1 ASTAR
|   2 PSTAR
|   3 ESTAR (not implemented yet)
|   4 MSTAR
|   5 ICRU73_OLD
|   6 ICRU73
|   7 ICRU49
|   8 
|   9 
| 100 BETHE_EXT00
| 101 
| 102 
| 103 
| 104 
| 105 
| 106 
| 107 
| 108 
| 109 


List all known ions:

|   1: HYDROGEN
|   2: HELIUM
|   3: LITHIUM
|   4: BERYLLIUM
|   5: BORON
|   6: CARBON
|   7: NITROGEN
|   8: OXYGEN
|   9: FLUORINE
|  10: NEON
|  11: SODIUM
|  12: MAGNESIUM
|  13: ALUMINUM
|  14: SILICON
|  15: PHOSPHORUS
|  16: SULFUR
|  17: CHLORINE
|  18: ARGON
|  19: POTASSIUM
|  20: CALCIUM
|  21: SCANDIUM
|  22: TITANIUM
|  23: VANADIUM
|  24: CHROMIUM
|  25: MANGANESE
|  26: IRON
|  27: COBALT
|  28: NICKEL
|  29: COPPER
|  30: ZINC
|  31: GALLIUM
|  32: GERMANIUM
|  33: ARSENIC
|  34: SELENIUM
|  35: BROMINE
|  36: KRYPTON
|  37: RUBIDIUM
|  38: STRONTIUM
|  39: YTTRIUM
|  40: ZIRCONIUM
|  41: NIOBIUM
|  42: MOLYBDENUM
|  43: TECHNETIUM
|  44: RUTHENIUM
|  45: RHODIUM
|  46: PALLADIUM
|  47: SILVER
|  48: CADMIUM
|  49: INDIUM
|  50: TIN
|  51: ANTIMONY
|  52: TELLURIUM
|  53: IODINE
|  54: XENON
|  55: CESIUM
|  56: BARIUM
|  57: LANTHANUM
|  58: CERIUM
|  59: PRASEODYMIUM
|  60: NEODYMIUM
|  61: PROMETHIUM
|  62: SAMARIUM
|  63: EUROPIUM
|  64: GADOLINIUM
|  65: TERBIUM
|  66: DYSPROSIUM
|  67: HOLMIUM
|  68: ERBIUM
|  69: THULIUM
|  70: YTTERBIUM
|  71: LUTETIUM
|  72: HAFNIUM
|  73: TANTALUM
|  74: TUNGSTEN
|  75: RHENIUM
|  76: OSMIUM
|  77: IRIDIUM
|  78: PLATINUM
|  79: GOLD
|  80: MERCURY
|  81: THALLIUM
|  82: LEAD
|  83: BISMUTH
|  84: POLONIUM
|  85: ASTATINE
|  86: RADON
|  87: FRANCIUM
|  88: RADIUM
|  89: ACTINIUM
|  90: THORIUM
|  91: PROTACTINIUM
|  92: URANIUM
|  93: NEPTUNIUM
|  94: PLUTONIUM
|  95: AMERICIUM
|  96: CURIUM
|  97: BERKELIUM
|  98: CALIFORNIUM
|  99: EINSTEINIUM
| 100: FERMIUM
| 101: MENDELEVIUM
| 102: NOBELIUM
| 103: LAWRENCIUM
| 104: RUTHERFORDNIUM
| 105: DUBNIUM
| 106: SEABORGIUM
| 107: BOHRIUM
| 108: HASSIUM
| 109: MEITNERIUM
| 110: DARMSTADTIUM
| 111: ROENTGENIUM
| 112: COPERNICUM


List all known target materials (following ICRU naming convention):

|   1: HYDROGEN
|   2: HELIUM
|   3: LITHIUM
|   4: BERYLLIUM
|   5: BORON
|   6: CARBON
|   7: NITROGEN
|   8: OXYGEN
|   9: FLUORINE
|  10: NEON
|  11: SODIUM
|  12: MAGNESIUM
|  13: ALUMINUM
|  14: SILICON
|  15: PHOSPHORUS
|  16: SULFUR
|  17: CHLORINE
|  18: ARGON
|  19: POTASSIUM
|  20: CALCIUM
|  21: SCANDIUM
|  22: TITANIUM
|  23: VANADIUM
|  24: CHROMIUM
|  25: MANGANESE
|  26: IRON
|  27: COBALT
|  28: NICKEL
|  29: COPPER
|  30: ZINC
|  31: GALLIUM
|  32: GERMANIUM
|  33: ARSENIC
|  34: SELENIUM
|  35: BROMINE
|  36: KRYPTON
|  37: RUBIDIUM
|  38: STRONTIUM
|  39: YTTRIUM
|  40: ZIRCONIUM
|  41: NIOBIUM
|  42: MOLYBDENUM
|  43: TECHNETIUM
|  44: RUTHENIUM
|  45: RHODIUM
|  46: PALLADIUM
|  47: SILVER
|  48: CADMIUM
|  49: INDIUM
|  50: TIN
|  51: ANTIMONY
|  52: TELLURIUM
|  53: IODINE
|  54: XENON
|  55: CESIUM
|  56: BARIUM
|  57: LANTHANUM
|  58: CERIUM
|  59: PRASEODYMIUM
|  60: NEODYMIUM
|  61: PROMETHIUM
|  62: SAMARIUM
|  63: EUROPIUM
|  64: GADOLINIUM
|  65: TERBIUM
|  66: DYSPROSIUM
|  67: HOLMIUM
|  68: ERBIUM
|  69: THULIUM
|  70: YTTERBIUM
|  71: LUTETIUM
|  72: HAFNIUM
|  73: TANTALUM
|  74: TUNGSTEN
|  75: RHENIUM
|  76: OSMIUM
|  77: IRIDIUM
|  78: PLATINUM
|  79: GOLD
|  80: MERCURY
|  81: THALLIUM
|  82: LEAD
|  83: BISMUTH
|  84: POLONIUM
|  85: ASTATINE
|  86: RADON
|  87: FRANCIUM
|  88: RADIUM
|  89: ACTINIUM
|  90: THORIUM
|  91: PROTACTINIUM
|  92: URANIUM
|  93: NEPTUNIUM
|  94: PLUTONIUM
|  95: AMERICIUM
|  96: CURIUM
|  97: BERKELIUM
|  98: CALIFORNIUM
|  99: A150_TISSUE_EQUIVALENT_PLASTIC
| 100: ACETONE
| 101: ACETYLENE
| 102: ADENINE
| 103: ADIPOSETISSUE_ICRP
| 104: AIR
| 105: ALANINE
| 106: ALUMINUMOXIDE
| 107: AMBER
| 108: AMMONIA
| 109: ANILINE
| 110: ANTHRACENE
| 111: B100
| 112: BAKELITE
| 113: BARIUMFLUORIDE
| 114: BARIUMSULFATE
| 115: BENZENE
| 116: BERYLLIUMOXIDE
| 117: BISMUTHGERMANIUMOXIDE
| 118: BLOOD_ICRP
| 119: BONE_COMPACT_ICRU
| 120: BONE_CORTICAL_ICRP
| 121: BORONCARBIDE
| 122: BORONOXIDE
| 123: BRAIN_ICRP
| 124: BUTANE
| 125: N_BUTYLALCOHOL
| 126: C552
| 127: CADMIUMTELLURIDE
| 128: CADMIUMTUNGSTATE
| 129: CALCIUMCARBONATE
| 130: CALCIUMFLUORIDE
| 131: CALCIUMOXIDE
| 132: CALCIUMSULFATE
| 133: CALCIUMTUNGSTATE
| 134: CARBONDIOXIDE
| 135: CARBONTETRACHLORIDE
| 136: CELLULOSEACETATE_CELLOPHANE
| 137: CELLULOSEACETATEBUTYRATE
| 138: CELLULOSENITRATE
| 139: CERICSULFATEDOSIMETERSOLUTION
| 140: CESIUMFLUORIDE
| 141: CESIUMIODIDE
| 142: CHLOROBENZENE
| 143: CHLOROFORM
| 144: CONCRETE_PORTLAND
| 145: CYCLOHEXANE
| 146: DICHLOROBENZENE
| 147: DICHLORODIETHYLETHER
| 148: DICHLOROETHANE
| 149: DIETHYLETHER
| 150: N_N_DIMETHYLFORMAMIDE
| 151: DIMETHYLSULFOXIDE
| 152: ETHANE
| 153: ETHYLALCOHOL
| 154: ETHYLCELLULOSE
| 155: ETHYLENE
| 156: EYELENS_ICRP
| 157: FERRICOXIDE
| 158: FERROBORIDE
| 159: FERROUSOXIDE
| 160: FERROUSSULFATEDOSIMETERSOLUTION
| 161: FREON_12
| 162: FREON_12B2
| 163: FREON_13
| 164: FREON_13B1
| 165: FREON_13I1
| 166: GADOLINIUMOXYSULFIDE
| 167: GALLIUMARSENIDE
| 168: GELINPHOTOGRAPHICEMULSION
| 169: GLASS_PYREX
| 170: GLASS_LEAD
| 171: GLASS_PLATE
| 172: GLUCOSE
| 173: GLUTAMINE
| 174: GLYCEROL
| 175: GUANINE
| 176: GYPSUM_PLASTEROFPARIS
| 177: N_HEPTANE
| 178: N_HEXANE
| 179: KAPTONPOLYIMIDEFILM
| 180: LANTHANUMOXYBROMIDE
| 181: LANTHANUMOXYSULFIDE
| 182: LEADOXIDE
| 183: LITHIUMAMIDE
| 184: LITHIUMCARBONATE
| 185: LITHIUMFLUORIDE
| 186: LITHIUMHYDRIDE
| 187: LITHIUMIODIDE
| 188: LITHIUMOXIDE
| 189: LITHIUMTETRABORATE
| 190: LUNG_ICRP
| 191: M3WAX
| 192: MAGNESIUMCARBONATE
| 193: MAGNESIUMFLUORIDE
| 194: MAGNESIUMOXIDE
| 195: MAGNESIUMTETRABORATE
| 196: MERCURICIODIDE
| 197: METHANE
| 198: METHANOL
| 199: MIXDWAX
| 200: MS20TISSUESUBSTITUTE
| 201: MUSCLE_SKELETAL
| 202: MUSCLE_STRIATED
| 203: MUSCLE_EQUIVALENTLIQUID_SUCROSE
| 204: MUSCLE_EQUIVALENTLIQUID_NOSUCROSE
| 205: NAPHTHALENE
| 206: NITROBENZENE
| 207: NITROUSOXIDE
| 208: NYLON_DUPONTELVAMIDE8062
| 209: NYLON_TYPE6AND6_6
| 210: NYLON_TYPE6_10
| 211: NYLON_TYPE11_RILSAN
| 212: OCTANE_LIQUID
| 213: PARAFFINWAX
| 214: N_PENTANE
| 215: PHOTOGRAPHICEMULSION
| 216: PLASTICSCINTILLATOR_VINYLTOLUENEBASED
| 217: PLUTONIUMDIOXIDE
| 218: POLYACRYLONITRILE
| 219: POLYCARBONATE_MAKROLON_LEXAN
| 220: POLYCHLOROSTYRENE
| 221: POLYETHYLENE
| 222: MYLAR
| 223: PMMA
| 224: POLYOXYMETHYLENE
| 225: POLYPROPYLENE
| 226: POLYSTYRENE
| 227: POLYTETRAFLUOROETHYLENE (TEFLON)
| 228: POLYTRIFLUOROCHLOROETHYLENE
| 229: POLYVINYLACETATE
| 230: POLYVINYLALCOHOL
| 231: POLYVINYLBUTYRAL
| 232: POLYVINYLCHLORIDE
| 233: SARAN
| 234: POLYVINYLIDENEFLUORIDE
| 235: POLYVINYLPYRROLIDONE
| 236: POTASSIUMIODIDE
| 237: POTASSIUMOXIDE
| 238: PROPANE
| 239: PROPANE_LIQUID
| 240: N_PROPYLALCOHOL
| 241: PYRIDINE
| 242: RUBBER_BUTYL
| 243: RUBBER_NATURAL
| 244: RUBBER_NEOPRENE
| 245: SILICONDIOXIDE
| 246: SILVERBROMIDE
| 247: SILVERCHLORIDE
| 248: SILVERHALIDESINPHOTOGRAPHICEMULSION
| 249: SILVERIODIDE
| 250: SKIN_ICRP
| 251: SODIUMCARBONATE
| 252: SODIUMIODIDE
| 253: SODIUMMONOXIDE
| 254: SODIUMNITRATE
| 255: STILBENE
| 256: SUCROSE
| 257: TERPHENYL
| 258: TESTES_ICRP
| 259: TETRACHLOROETHYLENE
| 260: THALLIUMCHLORIDE
| 261: TISSUE_SOFT_ICRP
| 262: TISSUE_SOFT_ICRUFOUR_COMPONENT
| 263: TISSUE_EQUIVALENTGAS_METHANEBASED
| 264: TISSUE_EQUIVALENTGAS_PROPANEBASED
| 265: TITANIUMDIOXIDE
| 266: TOLUENE
| 267: TRICHLOROETHYLENE
| 268: TRIETHYLPHOSPHATE
| 269: TUNGSTENHEXAFLUORIDE
| 270: URANIUMDICARBIDE
| 271: URANIUMMONOCARBIDE
| 272: URANIUMOXIDE
| 273: UREA
| 274: VALINE
| 275: VITONFLUOROELASTOMER
| 276: WATER
| 277: WATERVAPOR
| 278: XYLENE
| 906: GRAPHITE

List of elements and compounds which are on gas phase by default:

|   1: HYDROGEN
|   2: HELIUM
|   7: NITROGEN
|   8: OXYGEN
|   9: FLUORINE
|  10: NEON
|  17: CHLORINE
|  18: ARGON
|  36: KRYPTON
|  54: XENON
|  86: RADON
| 101: ACETYLENE
| 104: AIR
| 108: AMMONIA
| 124: BUTANE
| 134: CARBONDIOXIDE
| 152: ETHANE
| 155: ETHYLENE
| 161: FREON_12
| 162: FREON_12B2
| 163: FREON_13
| 164: FREON_13B1
| 165: FREON_13I1
| 197: METHANE
| 207: NITROUSOXIDE
| 238: PROPANE
| 263: TISSUE_EQUIVALENTGAS_METHANEBASED
| 264: TISSUE_EQUIVALENTGAS_PROPANEBASED
| 277: WATERVAPOR

In your computer code, all materials and ions can also be accessed by their name
via the ``DEDX_`` prefix. However, there are occasionally small variations in the naming scheme.
Enums are defined in ``dedx.h``, but are listed here for convenience:

.. code-block:: C

  enum {DEDX_ASTAR=1, DEDX_PSTAR, DEDX_ESTAR,
        DEDX_MSTAR, DEDX_ICRU73_OLD, DEDX_ICRU73, DEDX_ICRU49, _DEDX_0008, 
        DEDX_ICRU, DEDX_DEFAULT=100, DEDX_BETHE_EXT00};
  
  enum {DEDX_DEFAULT_STATE=0,DEDX_GAS,DEDX_CONDENSED};
  
  enum {DEDX_HYDROGEN=1, DEDX_HELIUM, DEDX_LITHIUM, DEDX_BERYLLIUM, DEDX_BORON,
        DEDX_CARBON, DEDX_GRAPHITE=906, DEDX_NITROGEN=7, DEDX_OXYGEN,
        DEDX_FLUORINE, DEDX_NEON, DEDX_SODIUM, DEDX_MAGNESIUM,
        DEDX_ALUMINUM, DEDX_SILICON, DEDX_PHOSPHORUS, DEDX_SULFUR,
        DEDX_CHLORINE, DEDX_ARGON, DEDX_POTASSIUM, DEDX_CALCIUM, DEDX_SCANDIUM,
        DEDX_TITeANIUM, DEDX_VANADIUM, DEDX_CHROMIUM, DEDX_MANGANESE, DEDX_IRON,
        DEDX_COBALT, DEDX_NICKEL, DEDX_COPPER, DEDX_ZINC, DEDX_GALLIUM,
        DEDX_GERMANIUM, DEDX_ARSENIC, DEDX_SELENIUM, DEDX_BROMINE, DEDX_KRYPTON,
        DEDX_RUBIDIUM, DEDX_STRONTIUM, DEDX_YTTRIUM, DEDX_ZIRCONIUM, DEDX_NIOBIUM,
        DEDX_MOLYBDENUM, DEDX_TECHNETIUM, DEDX_RUTHENIUM, DEDX_RHODIUM,
        DEDX_PALLADIUM, DEDX_SILVER, DEDX_CADMIUM, DEDX_INDIUM, DEDX_TIN,
        DEDX_ANTIMONY, DEDX_TELLURIUM, DEDX_IODINE, DEDX_XENON, DEDX_CESIUM,
        DEDX_BARIUM, DEDX_LANTHANUM, DEDX_CERIUM, DEDX_PRASEODYMIUM,
        DEDX_NEODYMIUM, DEDX_PROMETHIUM, DEDX_SAMARIUM, DEDX_EUROPIUM,
        DEDX_GADOLINIUM, DEDX_TERBIUM, DEDX_DYSPROSIUM, DEDX_HOLMIUM,
        DEDX_ERBIUM, DEDX_THULIUM, DEDX_YTTERBIUM, DEDX_LUTETIUM, DEDX_HAFNIUM,
        DEDX_TANTALUM, DEDX_TUNGSTEN, DEDX_RHENIUM, DEDX_OSMIUM, DEDX_IRIDIUM,
        DEDX_PLATINUM, DEDX_GOLD, DEDX_MERCURY, DEDX_THALLIUM, DEDX_LEAD,
        DEDX_BISMUTH, DEDX_POLONIUM, DEDX_ASTATINE, DEDX_RADON, DEDX_FRANCIUM,
        DEDX_RADIUM, DEDX_ACTINIUM, DEDX_THORIUM, DEDX_PROTACTINIUM,
        DEDX_URANIUM, DEDX_NEPTUNIUM, DEDX_PLUTONIUM, DEDX_AMERICIUM,
        DEDX_CURIUM, DEDX_BERKELIUM, DEDX_CALIFORNIUM,
        DEDX_A150_TISSUE_EQUIVALENT_PLASTIC, DEDX_ACETONE, DEDX_ACETYLENE,
        DEDX_ADENINE, DEDX_ADIPOSE_TISSUE_ICRP, DEDX_AIR_DRY_NEAR_SEA_LEVEL,
        DEDX_ALANINE, DEDX_ALUMINUMOXIDE, DEDX_AMBER, DEDX_AMMONIA, DEDX_ANILINE,
        DEDX_ANTHRACENE, DEDX_B100, DEDX_BAKELITE, DEDX_BARIUM_FLUORIDE,
        DEDX_BARIUM_SULFATE, DEDX_BENZENE, DEDX_BERYLLIUM_OXIDE,
        DEDX_BISMUTH_GERMANIUM_OXIDE, DEDX_BLOOD_ICRP, DEDX_BONE_COMPACT_ICRU,
        DEDX_BONE_CORTICAL_ICRP, DEDX_BORON_CARBIDE, DEDX_BORON_OXIDE,
        DEDX_BRAIN_ICRP, DEDX_BUTANE, DEDX_N_BUTYLALCOHOL,
        DEDX_C552_AIR_EQUIVALENT_PLASTIC, DEDX_CADMIUM_TELLURIDE,
        DEDX_CADMIUM_TUNGSTATE, DEDX_CALCIUM_CARBONATE, DEDX_CALCIUM_FLUORIDE,
        DEDX_CALCIUM_OXIDE, DEDX_CALCIUM_SULFATE, DEDX_CALCIUM_TUNGSTATE,
        DEDX_CARBON_DIOXIDE, DEDX_CARBON_TETRACHLORIDE,
        DEDX_CELLULOSE_ACETATE_CELLOPHANE, DEDX_CELLULOSE_ACETATE_BUTYRATE,
        DEDX_CELLULOSE_NITRATE, DEDX_CERIC_SULFATE_DOSIMETER_SOLUTION,
        DEDX_CESIUM_FLUORIDE, DEDX_CESIUM_IODIDE, DEDX_CHLORO_BENZENE,
        DEDX_CHLOROFORM, DEDX_CONCRETE_PORTLAND, DEDX_CYCLOHEXANE,
        DEDX_DICHLOROBENZENE, DEDX_DICHLORODIETHYL_ETHER, DEDX_DICHLOROETHANE,
        DEDX_DIETHYLETHER, DEDX_N_N_DIMETHYL_FORMAMIDE, DEDX_DIMETHYL_SULFOXIDE,
        DEDX_ETHANE, DEDX_ETHYL_ALCOHOL, DEDX_ETHYL_CELLULOSE, DEDX_ETHYLENE,
        DEDX_EYE_LENS_ICRP, DEDX_FERRIC_OXIDE, DEDX_FERRO_BORIDE,
        DEDX_FERROUS_OXIDE, DEDX_FERROUS_SULFATE_DOSIMETER_SOLUTION,
        DEDX_FREON_12, DEDX_FREON_12B2, DEDX_FREON_13, DEDX_FREON_13B1,
        DEDX_FREON_13I1, DEDX_GADOLINIUM_OXYSULFIDE, DEDX_GALLIUM_ARSENIDE,
        DEDX_GEL_IN_PHOTOGRAPHIC_EMULSION, 
        DEDX_GLASS_PYREX, DEDX_GLASS_LEAD, DEDX_GLASS_PLATE, /* 169,170,171 */
        DEDX_GLUCOSE, DEDX_GLUTAMINE, DEDX_GLYCEROL,
        DEDX_GUANINE, DEDX_GYPSUM_PLASTER_OF_PARIS, DEDX_N_HEPTANE, DEDX_N_HEXANE,
        DEDX_KAPTON_POLYIMIDE_FILM, DEDX_LANTHANUM_OXYBROMIDE,
        DEDX_LANTHANUM_OXYSULFIDE, DEDX_LEAD_OXIDE, DEDX_LITHIUM_AMIDE,
        DEDX_LITHIUM_CARBONATE, DEDX_LITHIUM_FLUORIDE, DEDX_LITHIUM_HYDRIDE,
        DEDX_LITHIUM_IODIDE, DEDX_LITHIUM_OXIDE, DEDX_LITHIUM_TETRABORATE,
        DEDX_LUNG_ICRP, DEDX_M3_WAX, DEDX_MAGNESIUM_CARBONATE,
        DEDX_MAGNESIUM_FLUORIDE, DEDX_MAGNESIUM_OXIDE, DEDX_MAGNESIUM_TETRABORATE,
        DEDX_MERCURIC_IODIDE, DEDX_METHANE, DEDX_METHANOL, DEDX_MIX_D_WAX,
        DEDX_MS20_TISSUE_SUBSTITUTE, DEDX_MUSCLE_SKELETAL, DEDX_MUSCLE_STRIATED,
        DEDX_MUSCLE_EQUIVALENT_LIQUID_WITH_SUCROSE,
        DEDX_MUSCLE_EQUIVALENT_LIQUID_WITHOUT_SUCROSE, DEDX_NAPHTHALENE,
        DEDX_NITROBENZENE, DEDX_NITROUS_OXIDE, DEDX_NYLON_DUPONT_ELVAMIDE_8062,
        DEDX_NYLON_TYPE_6_AND_6_6, DEDX_NYLON_TYPE_6_10,
        DEDX_NYLON_TYPE_11_RILSAN, DEDX_OCTANE_LIQUID, DEDX_PARAFFIN_WAX,
        DEDX_N_PENTANE, DEDX_PHOTOGRAPHIC_EMULSION,
        DEDX_PLASTIC_SCINTILLATOR_VINYLTOLUENE_BASED, DEDX_PLUTONIUM_DIOXIDE,
        DEDX_POLYACRYLONITRILE, DEDX_POLYCARBONATE_MAKROLON_LEXAN,
        DEDX_POLYCHLOROSTYRENE, DEDX_POLYETHYLENE, DEDX_MYLAR,
        DEDX_LUCITE_PERSPEX_PMMA, DEDX_POLYOXYMETHYLENE,
        DEDX_POLYPROPYLENE, DEDX_POLYSTYRENE, DEDX_POLYTETRAFLUOROETHYLENE,
        DEDX_POLYTRIFLUOROCHLOROETHYLENE, DEDX_POLYVINYL_ACETATE,
        DEDX_POLYVINYL_ALCOHOL, DEDX_POLYVINYL_BUTYRAL, DEDX_POLYVINYL_CHLORIDE,
        DEDX_POLYVINYLIDENE_CHLORIDE_SARAN, DEDX_POLYVINYLIDENE_FLUORIDE,
        DEDX_POLYVINYL_PYRROLIDONE, DEDX_POTASSIUM_IODIDE, DEDX_POTASSIUM_OXIDE,
        DEDX_PROPANE, DEDX_PROPANE_LIQUID, DEDX_N_PROPYL_ALCOHOL, DEDX_PYRIDINE,
        DEDX_RUBBER_BUTYL, DEDX_RUBBER_NATURAL, DEDX_RUBBER_NEOPRENE,
        DEDX_SILICON_DIOXIDE, DEDX_SILVER_BROMIDE, DEDX_SILVER_CHLORIDE,
        DEDX_SILVER_HALIDES_IN_PHOTOGRAPHIC_EMULSION, DEDX_SILVER_IODIDE,
        DEDX_SKIN_ICRP, DEDX_SODIUM_CARBONATE, DEDX_SODIUM_IODIDE,
        DEDX_SODIUM_MONOXIDE, DEDX_SODIUM_NITRATE, DEDX_STILBENE,
        DEDX_SUCROSE, DEDX_TERPHENYL, DEDX_TESTES_ICRP,
        DEDX_TETRACHLOROETHYLENE, DEDX_THALLIUM_CHLORIDE, DEDX_TISSUE_SOFT_ICRP,
        DEDX_TISSUE_SOFT_ICRU_FOUR_COMPONENT,
        DEDX_TISSUE_EQUIVALENT_GAS_METHANE_BASED,
        DEDX_TISSUE_EQUIVALENT_GAS_PROPANE_BASED, DEDX_TITANIUM_DIOXIDE,
        DEDX_TOLUENE, DEDX_TRICHLOROETHYLENE, DEDX_TRIETHYL_PHOSPHATE,
        DEDX_TUNGSTEN_HEXAFLUORIDE, DEDX_URANIUM_DICARBIDE,
        DEDX_URANIUM_MONOCARBIDE, DEDX_URANIUM_OXIDE, DEDX_UREA, DEDX_VALINE,
        DEDX_VITON_FLUOROELASTOMER, DEDX_WATER_LIQUID, DEDX_WATER_VAPOR,
        DEDX_XYLENE};
  
  /* aliases */
  #define DEDX_PROTON     1
  #define DEDX_ELECTRON   1001
  #define DEDX_POSITRON   1002
  #define DEDX_PIMINUS    1003
  #define DEDX_PIPLUS     1004
  #define DEDX_PIZERO     1005
  #define DEDX_ANTIPROTON 1006
  
  #define DEDX_WATER    DEDX_WATER_LIQUID
  #define DEDX_AIR      DEDX_AIR_DRY_NEAR_SEA_LEVEL
  #define DEDX_PMMA     DEDX_LUCITE_PERSPEX_PMMA
  #define DEDX_PERSPEX  DEDX_LUCITE_PERSPEX_PMMA
  #define DEDX_LUCITE   DEDX_LUCITE_PERSPEX_PMMA
  #define DEDX_TEFLON   DEDX_POLYTETRAFLUOROETHYLENE
  #define DEDX_CONCRETE DEDX_CONCRETE_PORTLAND
  #define DEDX_CAESIUM  DEDX_CESIUM
