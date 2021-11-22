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
|  (c) Copyright 2010-2012 by
|  Jakob Toftegaard, jakob.toftegaard@gmail.com
|  Niels Bassler, bassler@phys.au.dk
|  for the Aarhus Particle Therapy Group 
|  http://www.phys.au.dk/aptg

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

libdEdx is an easy to use library which provides electronic stopping powers of fast charged particles. It is a collection of commonly known stopping power tables and stopping-power routines. You can either use it as a library, by including the :code:`<dedx.h>` file and linking against the dedx library, or manually access a value with the :code:`getdedx` command.

A web front end is available on http://dedx.au.dk

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

See the INSTALL file.

******
4. Use
******

UNITS: Energies are always in terms of MeV/nucleon, except for ESTAR, where the electron energy must be specified in terms of MeV. The resulting mass stopping power is in MeV cm2/g.

Stopping power values can be retrieved in two different ways: 

1. a simple method for simple implementation, 
2. a bit more complicated way, but more suitable for fast and multithreaded applications.

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
      int bragg_used;        // is 1 if braggs additivity rule was applied
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
