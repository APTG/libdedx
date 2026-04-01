#ifndef DEDX_SPLINE_H
#define DEDX_SPLINE_H

#include "dedx.h"
#include "dedx_lookup_accelerator.h"
#include "dedx_spline_base.h"

/** @brief Build natural cubic-spline coefficients for one stopping-power table.
 *
 *  DEDX_INTERPOLATION_LOG_LOG stores positive stopping-power tables in log-log
 *  space: `ln(stopping)` as a function of `ln(energy)`, matching the
 *  interpolation guidance used by the underlying ICRU/NIST tabulations.
 *  DEDX_INTERPOLATION_LINEAR stores a natural cubic spline in linear space.
 *  If log-log is requested for a table with non-positive values, the
 *  implementation falls back to linear-space coefficients.
 *
 *  @param[out] coef      Spline coefficient array to populate.
 *  @param[in]  energy    Monotonic energy grid.
 *  @param[in]  stopping  Stopping-power values on @p energy.
 *  @param[in]  n         Number of valid grid points.
 *  @param[in]  interpolation_mode  Requested interpolation mode.
 */
void dedx_internal_calculate_coefficients(
    _dedx_spline_base *coef, float *energy, float *stopping, int n, int interpolation_mode);

/** @brief Evaluate a precomputed stopping-power spline at one energy value.
 *
 *  Exact tabulated energies return the raw stored stopping power.
 *
 *  @param[in]      coef  Spline coefficients.
 *  @param[in]      x     Energy at which to evaluate the spline.
 *  @param[in,out]  acc   Optional lookup cache for repeated evaluations; may be NULL.
 *  @param[in]      n     Number of valid spline intervals.
 *  @param[in]      interpolation_mode  Interpolation mode stored with the dataset.
 *  @return Interpolated stopping power at @p x.
 */
float dedx_internal_evaluate_spline(
    _dedx_spline_base *coef, float x, _dedx_lookup_accelerator *acc, int n, int interpolation_mode);

#endif // DEDX_SPLINE_H
