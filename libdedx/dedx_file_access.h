#ifndef DEDX_FILE_ACCESS_H_INCLUDED
#define DEDX_FILE_ACCESS_H_INCLUDED

#include <stddef.h>

#include "dedx_stopping_data.h"

void dedx_internal_read_binary_data(stopping_data *data, int prog, int ion, int target, int *err);

void dedx_internal_read_energy_data(float *energy, int prog, int *err);
float dedx_internal_read_effective_charge(int id, int *err);
size_t dedx_internal_target_is_gas(int target, int *err);
float dedx_internal_read_density(int id, int *err);
float dedx_internal_get_i_value(int target, int state, int *err);
void dedx_internal_get_composition(int target, float composition[][2], unsigned int *length, int *err);
#endif // DEDX_FILE_ACCESS_H_INCLUDED
