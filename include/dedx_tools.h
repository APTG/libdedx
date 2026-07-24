#ifndef DEDX_TOOLS_H
#define DEDX_TOOLS_H

/**
 * @file dedx_tools.h
 * @brief Additional tools: CSDA range, inverse stopping power, and unit conversion.
 */

#include "dedx.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 *  @param[in]  energy  Kinetic energy in MeV/nucl (MeV per nucleon).
 *  @param[out] err     Error code; 0 on success.
 *  @return CSDA range in g/cm².
 */
double dedx_get_csda(dedx_workspace *ws, dedx_config *config, float energy, int *err);

/** @brief Find the energy corresponding to a given stopping power value.
 *
 *  Inverts the stopping power curve. Stopping power rises from a program's
 *  minimum tabulated energy to the energy of maximum stopping power, then
 *  falls off; when the requested @p stp corresponds to two energies, the
 *  @p side parameter selects which branch to use. If the curve is
 *  monotonically decreasing over the whole tabulated range (no interior
 *  peak), @p side is ignored and the single descending branch is used.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[in]  stp     Target stopping power in MeV cm²/g. Must lie between
 *                       the STP at the program's max energy and the maximum
 *                       stopping power (inclusive, see dedx_get_max_stp());
 *                       otherwise the value is unreachable and an error is
 *                       returned.
 *  @param[in]  side    0 = low-energy (ascending) branch, 1 = high-energy
 *                      (descending) branch. Ignored when the curve has no
 *                      interior peak.
 *  @param[out] err     Error code; 0 on success.
 *  @return Energy in MeV/nucl (MeV per nucleon), or -1 on error.
 */
double dedx_get_inverse_stp(dedx_workspace *ws, dedx_config *config, float stp, int side, int *err);

/** @brief Find the maximum stopping power over a program's whole tabulated
 *  energy range.
 *
 *  This is the peak of the stopping-power-vs-energy curve, not the Bragg
 *  peak of a depth-dose curve (which also depends on range straggling and is
 *  not computed by this library).
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[out] err     Error code; 0 on success.
 *  @return Maximum stopping power in MeV cm²/g, or -1 on error.
 */
double dedx_get_max_stp(dedx_workspace *ws, dedx_config *config, int *err);

/** @brief Find the energy corresponding to a given CSDA range.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration.
 *  @param[in]  range   CSDA range in g/cm².
 *  @param[out] err     Error code; 0 on success.
 *  @return Energy in MeV/nucl (MeV per nucleon).
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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEDX_TOOLS_H
