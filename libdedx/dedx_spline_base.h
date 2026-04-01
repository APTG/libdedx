#ifndef DEDX_SPLINE_BASE_H
#define DEDX_SPLINE_BASE_H

#include "dedx_elements.h"

typedef struct {
    float a;
    float b;
    float c;
    float d;
    float x;
    float log_a;
    float log_x;
} _dedx_spline_base;

#endif // DEDX_SPLINE_BASE_H
