#ifndef DEDX_LOOKUP_DATA_H
#define DEDX_LOOKUP_DATA_H

#include "dedx_elements.h"
#include "dedx_lookup_accelerator.h"
#include "dedx_spline_base.h"

/** @brief Internal representation of one loaded stopping-power table.
 *
 *  Each dataset stores the spline coefficients, source program id, ion/target
 *  identifiers, and the lookup accelerator used by repeated spline queries.
 */
typedef struct dedx_internal_lookup_data {
    dedx_internal_spline_base base[DEDX_MAX_ELEMENTS];
    int n;
    int prog;
    int target;
    int ion;
    int datapoints;
    int interpolation_mode;
    dedx_internal_lookup_accelerator acc;
} dedx_internal_lookup_data;

#endif // DEDX_LOOKUP_DATA_H
