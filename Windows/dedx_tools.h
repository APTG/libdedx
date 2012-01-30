#ifndef DEDX_TOOLS_H_INCLUDED
#define DEDX_TOOLS_H_INCLUDED
double dedx_get_csda(int program,int ion,double A,int target,float energy,int *err);
double dedx_get_inverse_stp(int program,int ion,int target,float stp,int side,int *err);
double dedx_get_inverse_csda(int program,int ion,double A,int target,float range,int *err);
#endif
