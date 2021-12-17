#ifndef DEDX_TOOLS_H_INCLUDED
#define DEDX_TOOLS_H_INCLUDED
#include "dedx.h"
#include "dedx_file_access.h"

enum dedx_stp_units {
    DEDX_MEVCM2G, DEDX_MEVCM, DEDX_KEVUM
};
double dedx_get_csda(dedx_workspace *ws,dedx_config *config,float energy,int *err);
double dedx_get_inverse_stp(dedx_workspace * ws,dedx_config * config,float stp,int side,int *err);
double dedx_get_inverse_csda(dedx_workspace *ws,dedx_config * config,float range,int *err);

float _conversion_factor(const int old_unit, const int new_unit, const int material, int *err);
int convert_units(const int old_unit, const int new_unit, const int material, const int no_of_points, const float * old_values, float * new_values);
#endif
