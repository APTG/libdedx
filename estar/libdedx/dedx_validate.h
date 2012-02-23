#ifndef DEDX_VALIDATE_H_INCLUDED
#define DEDX_VALIDATE_H_INCLUDED
#include "dedx.h"

int _dedx_set_names(dedx_config * config, int *err);
int _dedx_validate_state(dedx_config *config,int *err);
int _dedx_validate_config(dedx_config * config,int *err);
int _dedx_evaluate_i_pot(dedx_config * config, int *err);
int _dedx_validate_rho(dedx_config * config, int *err);
int _dedx_evaluate_compound(dedx_config * config,int *err);
int _dedx_calculate_element_i_pot(dedx_config * config,int *err);

#endif

