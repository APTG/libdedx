#ifndef DEDX_MSTAR_H_INCLUDED
#define DEDX_MSTAR_H_INCLUDED
#include "dedx_stopping_data.h"
#include "dedx_mpaul.h"

void _evaluate_compound_state_mstar(dedx_config * config, int *err);
void _dedx_convert_energy_to_mstar(stopping_data * in, stopping_data * out,char state, dedx_config * config, float * energy);

#endif // DEDX_MSTAR_H_INCLUDED
