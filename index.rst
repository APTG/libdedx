=======
libdedx
=======

.. contents:: Table of Contents
   :local:
   :backlinks: none


**********************
License and disclaimer
**********************

libdedx is licensed under GPL version 3 or later.

Copyright 2010-2026 by the libdedx contributors:
https://github.com/APTG/libdedx/graphs/contributors

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.


****************
Acknowledgements
****************

We are grateful for the support by Prof. Helmut Paul from the University
of Linz, Austria. Niels Bassler acknowledges support from the Danish
Cancer Society.


************
Introduction
************

libdedx is a C library for charged-particle stopping-power calculations
(``dE/dx``). It provides tabulated data from common reference datasets,
analytic Bethe-style calculations, and a command-line frontend
(``getdedx``).

The public API is exposed through ``<dedx.h>``. Stopping powers are
returned as mass stopping power in ``MeV cm^2 / g``.

All tabulated stopping-power data is embedded into the library at build
time. There is no longer any runtime dependency on external ``.bin`` or
auxiliary data files.


******************
Supported programs
******************

.. list-table::
   :header-rows: 1

   * - Program
     - Description
     - Ion coverage
   * - ``DEDX_PSTAR``
     - NIST PSTAR
     - Protons
   * - ``DEDX_ASTAR``
     - NIST ASTAR
     - Helium ions
   * - ``DEDX_MSTAR``
     - MSTAR
     - Heavier ions
   * - ``DEDX_ICRU49``
     - ICRU Report 49
     - Protons, helium
   * - ``DEDX_ICRU73_OLD``
     - ICRU Report 73, older parametrization
     - Heavier ions
   * - ``DEDX_ICRU73``
     - ICRU Report 73, updated parametrization where available
     - Heavier ions
   * - ``DEDX_ICRU``
     - Generic ICRU selector using the most current embedded ICRU data available for the requested ion/target pair
     - Protons, helium, heavier ions
   * - ``DEDX_BETHE_EXT00``
     - Bethe formula with low-energy extensions
     - All ions

Notes:

* Energies are given in ``MeV/u`` (``MeV/nucleon``) for ions.
* Compound targets can be evaluated by Bragg additivity when a program
  does not provide that compound directly.
* ``DEDX_ESTAR`` still exists as an identifier in the API, but normal
  ESTAR support is not currently implemented.


************
Installation
************

Building from source requires CMake 3.21+ and a C11 compiler.

From the source tree::

   cmake -S . -B build
   cmake --build build
   ctest --test-dir build --output-on-failure

To install::

   cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
   cmake --build build-release
   cmake --install build-release

This installs:

* ``libdedx`` as shared and static libraries
* public headers under ``include/``
* exported CMake package files
* the ``getdedx`` command-line tool

Installed CMake consumers can use::

   find_package(dedx CONFIG REQUIRED)
   target_link_libraries(your_target PRIVATE dedx::dedx)

Minimal Debian and RPM packages can also be generated with CPack.
Release workflows additionally publish Windows binaries and Android
``.so`` artifacts.


************
Using libdedx
************

The simplest API is a one-shot lookup:

.. code-block:: c

   #include <dedx.h>

   int err = 0;
   float stp = dedx_get_simple_stp(DEDX_CARBON, DEDX_WATER, 100.0f, &err);

This returns the mass stopping power of a 100 MeV/u carbon ion in water.

For repeated evaluations, use a workspace and loaded configuration:

.. code-block:: c

   #include <dedx.h>
   #include <stdlib.h>

   int err = 0;
   dedx_workspace *ws = dedx_allocate_workspace(1, &err);
   dedx_config *cfg = calloc(1, sizeof(dedx_config));

   cfg->program = DEDX_ICRU;
   cfg->ion = DEDX_CARBON;
   cfg->target = DEDX_WATER;

   dedx_load_config(ws, cfg, &err);
   float stp = dedx_get_stp(ws, cfg, 100.0f, &err);

   dedx_free_config(cfg, &err);
   dedx_free_workspace(ws, &err);

Custom compounds can be described by filling ``elements_id`` plus either
``elements_atoms`` or ``elements_mass_fraction`` before
``dedx_load_config()``.

Additional API reference is available through the generated Doxygen
documentation for:

* ``dedx.h``
* ``dedx_tools.h``
* ``dedx_wrappers.h``


***************
Command-line use
***************

The ``getdedx`` helper provides quick lookups and table inspection::

   getdedx PSTAR HYDROGEN WATER 100
   getdedx ICRU CARBON WATER 100
   getdedx --version

Passing ``-1`` for the program, ion, or target slot lists the available
values for that level.


*****
Notes
*****

* The default interpolation mode for tabulated stopping-power data is
  log-log interpolation.
* ``DEDX_BETHE_EXT00`` is intended as a broadly useful fallback model,
  but for supported ion/target pairs the tabulated programs are usually
  preferred.
* ``dedx_get_program_version()`` reports the source/version string of a
  specific stopping-power program, while ``dedx_get_version_string()``
  reports the full libdedx build version string.
