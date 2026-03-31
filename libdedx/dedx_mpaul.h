#ifndef DEDX_MPAUL_H
#define DEDX_MPAUL_H
#include <math.h>

#include "dedx_file_access.h"

float dedx_internal_calculate_mspaul_coef(char mode, int ion, int target, float energy);

#endif // DEDX_MPAUL_H
