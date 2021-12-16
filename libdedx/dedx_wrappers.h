#ifndef DEDX_WRAPPERS_INCLUDED
#define DEDX_WRAPPERS_INCLUDED

#include <stdlib.h>
#include "dedx.h"

void dedx_fill_program_list(int *program_list);

void dedx_fill_material_list(int program, int *material_list);

void dedx_fill_ion_list(int program, int *ion_list);

int dedx_get_stp_table(const int program, const int ion, const int target, const int no_of_points,
                       const float *energies, float *stps);

float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err);

#endif //DEDX_WRAPPERS_INCLUDED
