#ifndef DEDX_LOOKUP_DATA_H_INCLUDED
#define DEDX_LOOKUP_DATA_H_INCLUDED

#include "dedx_elements.h"
#include "dedx_lookup_accelerator.h"
#include "dedx_spline_base.h"

typedef struct _dedx_lookup_data {
    _dedx_spline_base base[DEDX_MAX_ELEMENTS];
    int n;
    int prog;
    int target;
    int ion;
    int datapoints;
    _dedx_lookup_accelerator acc;
} _dedx_lookup_data;

#endif // DEDX_LOOKUP_DATA_H_INCLUDED
