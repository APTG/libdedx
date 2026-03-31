#ifndef DEDX_TOOLS_H_INCLUDED
#define DEDX_TOOLS_H_INCLUDED

/**
 * @file dedx_tools.h
 * @brief Additional tools: CSDA range, inverse stopping power, and unit conversion.
 */

#include "dedx.h"

/**
 * @brief Units for stopping power values.
 */
enum dedx_stp_units {
    DEDX_MEVCM2G, /**< MeV cm²/g  — mass stopping power (default) */
    DEDX_MEVCM,   /**< MeV/cm     — linear stopping power */
    DEDX_KEVUM    /**< keV/µm     — linear stopping power */
};

/** @brief Compute the CSDA range at a given energy.
 *
 *  The Continuous Slowing Down Approximation (CSDA) range is the path length
 *  a particle travels while losing all its kinetic energy.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[in]  energy  Kinetic energy in MeV/u.
 *  @param[out] err     Error code; 0 on success.
 *  @return CSDA range in g/cm².
 */
double dedx_get_csda(dedx_workspace *ws, dedx_config *config, float energy, int *err);

/** @brief Find the energy corresponding to a given stopping power value.
 *
 *  Inverts the stopping power curve. Since stopping power is non-monotonic,
 *  the @p side parameter selects which branch to use.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[in]  stp     Target stopping power in MeV cm²/g.
 *  @param[in]  side    0 = low-energy branch, 1 = high-energy branch.
 *  @param[out] err     Error code; 0 on success.
 *  @return Energy in MeV/u.
 */
double dedx_get_inverse_stp(dedx_workspace *ws, dedx_config *config, float stp, int side, int *err);

/** @brief Find the energy corresponding to a given CSDA range.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[in]  range   CSDA range in g/cm².
 *  @param[out] err     Error code; 0 on success.
 *  @return Energy in MeV/u.
 */
double dedx_get_inverse_csda(dedx_workspace *ws, dedx_config *config, float range, int *err);

/** @brief Convert an array of stopping power values between unit systems.
 *
 *  @param[in]  old_unit      Source unit (dedx_stp_units).
 *  @param[in]  new_unit      Target unit (dedx_stp_units).
 *  @param[in]  material      Material identifier (needed for density when converting to/from linear units).
 *  @param[in]  no_of_points  Number of values to convert.
 *  @param[in]  old_values    Input array (length: no_of_points).
 *  @param[out] new_values    Output array (length: no_of_points).
 *  @return 0 on success, non-zero on failure.
 */
int convert_units(const int old_unit,
                  const int new_unit,
                  const int material,
                  const int no_of_points,
                  const float *old_values,
                  float *new_values);

#endif // DEDX_TOOLS_H_INCLUDED
