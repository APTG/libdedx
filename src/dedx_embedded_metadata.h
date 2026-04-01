#ifndef DEDX_EMBEDDED_METADATA_H
#define DEDX_EMBEDDED_METADATA_H

#include <stddef.h>

int dedx_embedded_read_effective_charge(int id, float *charge);
int dedx_embedded_target_is_gas(int target);
int dedx_embedded_read_density(int id, float *density);
int dedx_embedded_get_i_value(int target, int state, float *pot);
int dedx_embedded_get_composition(int target, float composition[][2], unsigned int *length);

#endif // DEDX_EMBEDDED_METADATA_H
