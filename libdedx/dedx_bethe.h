#ifndef DEDX_BETHE_H_INCLUDED
#define DEDX_BETHE_H_INCLUDED

#include "dedx_file_access.h"
#include "dedx.h"
//#include "dedx_gold_struct.h"
//#include "dedx_bethe_struct.h"

float _dedx_calculate_bethe_energy(float energy, float PZ, float PA, float TZ, float TA, float rho, float Io_Pot);

#endif // DEDX_BETHE_H_INCLUDED
