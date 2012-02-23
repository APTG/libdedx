#ifndef DEDX_SPLINE_H_INCLUDED
#define DEDX_SPLINE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "dedx_spline_base.h"
#include <math.h>
#include "dedx_lookup_accelerator.h"


void _dedx_calculate_coefficients(_dedx_spline_base * coef, 
				  float * energy, 
				  float * stopping, 
				  int n);

float _dedx_evaluate_spline(_dedx_spline_base * coef, 
			    float x, 
			    _dedx_lookup_accelerator * acc, 
			    int n);

#endif // DEDX_SPLINE_H_INCLUDED
