===============
This is libdEdx
===============

.. contents:: Table of Contents

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
