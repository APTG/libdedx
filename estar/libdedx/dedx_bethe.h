#ifndef DEDX_BETHE_H_INCLUDED
#define DEDX_BETHE_H_INCLUDED

#include "dedx_file_access.h"
#include "dedx.h"
//#include "dedx_gold_struct.h"
//#include "dedx_bethe_struct.h"

typedef struct
{
    double TZ0;
    double TA0;
    double potentiale;
    double rho;
    double PZ0;
    double PA0;
} _dedx_bethe_struct;

typedef struct
{
    double e_min;
    double e_max;
    double epsilon;
    double h;
    double e_zero;
    double e_extr;
    double f_extr;
    double e_sewn;
    double f_sewn;
} _dedx_gold_struct;

typedef struct
{
	_dedx_bethe_struct * bet;
	_dedx_gold_struct * gold;
} _dedx_bethe_coll_struct;

float _dedx_calculate_bethe_energy(_dedx_bethe_coll_struct * ws, float energy, float PZ, float PA, float TZ, float TA, float rho, float Io_Pot);

#endif // DEDX_BETHE_H_INCLUDED
