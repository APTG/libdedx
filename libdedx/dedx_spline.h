#ifndef DEDX_SPLINE_H_INCLUDED
#define DEDX_SPLINE_H_INCLUDED

#include "dedx_lookup_accelerator.h"
#include "dedx_spline_base.h"

/** @brief Build cubic-spline coefficients for one stopping-power table.
 *  @param[out] coef      Spline coefficient array to populate.
 *  @param[in]  energy    Monotonic energy grid.
 *  @param[in]  stopping  Stopping-power values on @p energy.
 *  @param[in]  n         Number of valid grid points.
 */
void dedx_internal_calculate_coefficients(_dedx_spline_base *coef, float *energy, float *stopping, int n);

/** @brief Evaluate a precomputed cubic spline at one energy value.
 *  @param[in]      coef  Spline coefficients.
 *  @param[in]      x     Energy at which to evaluate the spline.
 *  @param[in,out]  acc   Optional lookup cache for repeated evaluations; may be NULL.
 *  @param[in]      n     Number of valid spline intervals.
 *  @return Interpolated stopping power at @p x.
 */
float dedx_internal_evaluate_spline(_dedx_spline_base *coef, float x, _dedx_lookup_accelerator *acc, int n);

#endif // DEDX_SPLINE_H_INCLUDED
