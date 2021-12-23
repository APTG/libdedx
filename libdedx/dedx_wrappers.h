#ifndef DEDX_WRAPPERS_INCLUDED
#define DEDX_WRAPPERS_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "dedx.h"

void dedx_fill_program_list(int *program_list);

void dedx_fill_material_list(int program, int *material_list);

void dedx_fill_ion_list(int program, int *ion_list);

int dedx_get_stp_table(const int program, const int ion, const int target, const int no_of_points,
                       const float *energies, float *stps);

float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err);

int dedx_get_stp_table_size(const int program, const int ion, const int target);

int dedx_fill_default_energy_stp_table(const int program, const int ion, const int target, float *energies, float *stps);

int dedx_get_csda_table(const int program, const int ion, const int target, const int no_of_points,
                        const float *energies, double *csda_ranges);
#endif //DEDX_WRAPPERS_INCLUDED
