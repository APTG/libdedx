#ifndef DEDX_TOOLS_H_INCLUDED
#define DEDX_TOOLS_H_INCLUDED
#include "dedx.h" 
double dedx_get_csda(dedx_workspace *ws,dedx_config *config,float energy,int *err);
double dedx_get_inverse_stp(dedx_workspace * ws,dedx_config * config,float stp,int side,int *err);
double dedx_get_inverse_csda(dedx_workspace *ws,dedx_config * config,float range,int *err);
#endif
