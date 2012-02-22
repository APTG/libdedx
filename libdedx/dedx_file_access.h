#ifndef DEDX_FILE_ACCESS_H_INCLUDED
#define DEDX_FILE_ACCESS_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dedx.h"
//#include "dedx_constants.h"
#include "dedx_stopping_data.h"
#include "dedx_file.h"
//#include "utils/dedx_string.h"
#include "dedx_config.h"
#include "dedx_split.h"
#include <sys/stat.h>

void _dedx_convert_to_binary(char * path, char * output, int *err);
void _dedx_read_binary_data(stopping_data * data, int prog, int ion, int target,int *err);

void _dedx_convert_energy_binary(char * path, char * output,int *err);
void _dedx_read_energy_data(float * energy, int prog,int *err);
float _dedx_read_effective_charge(int id,int *err);
size_t _dedx_target_is_gas(int target,int *err);
float _dedx_read_density(int id,int *err);
//void _dedx_get_short_name(int id,char * name, int *err);
float _dedx_get_i_value(int target,int state, int * err);
void _dedx_get_composition(int target, float composition[][2], unsigned int * length, int *err);

float * _dedx_get_atima_data(int target,int *err);
#endif // DEDX_FILE_ACCESS_H_INCLUDED
