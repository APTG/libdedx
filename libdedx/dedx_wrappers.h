#ifndef DEDX_WRAPPERS_INCLUDED
#define DEDX_WRAPPERS_INCLUDED

/**
 * @file dedx_wrappers.h
 * @brief Convenience wrappers for common libdedx operations.
 *
 * These functions handle workspace allocation internally and are suitable
 * for one-off queries. For repeated evaluations use the core API in dedx.h.
 */

#include <stdio.h>
#include <stdlib.h>

#include "dedx.h"

/** @brief Fill an array with all supported program identifiers.
 *  @param[out] program_list  Caller-allocated array; must be large enough to
 *                            hold all programs (use dedx_get_program_list() to
 *                            determine the count).
 */
void dedx_fill_program_list(int *program_list);

/** @brief Fill an array with all materials supported by a program.
 *  @param[in]  program        Program identifier.
 *  @param[out] material_list  Caller-allocated array of material identifiers.
 */
void dedx_fill_material_list(int program, int *material_list);

/** @brief Fill an array with all ions supported by a program.
 *  @param[in]  program   Program identifier.
 *  @param[out] ion_list  Caller-allocated array of ion identifiers.
 */
void dedx_fill_ion_list(int program, int *ion_list);

/** @brief Evaluate stopping power at arbitrary energy points.
 *
 *  @param[in]  program      Program identifier.
 *  @param[in]  ion          Ion identifier.
 *  @param[in]  target       Target material identifier.
 *  @param[in]  no_of_points Number of energy points.
 *  @param[in]  energies     Array of energies in MeV/u (length: no_of_points).
 *  @param[out] stps         Array to receive stopping powers in MeV cm²/g.
 *  @return 0 on success, non-zero error code on failure.
 */
int dedx_get_stp_table(
    const int program, const int ion, const int target, const int no_of_points, const float *energies, float *stps);

/** @brief One-call stopping power evaluation for a specific program.
 *
 *  @param[in]  program  Program identifier.
 *  @param[in]  ion      Ion identifier.
 *  @param[in]  target   Target material identifier.
 *  @param[in]  energy   Kinetic energy in MeV/u.
 *  @param[out] err      Error code; 0 on success.
 *  @return Mass stopping power in MeV cm²/g.
 */
float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err);

/** @brief Return the number of tabulated data points for a program/ion/target.
 *
 *  Use this to allocate arrays for dedx_fill_default_energy_stp_table().
 *
 *  @param[in] program  Program identifier.
 *  @param[in] ion      Ion identifier.
 *  @param[in] target   Target material identifier.
 *  @return Number of data points, or 0 if not supported.
 */
int dedx_get_stp_table_size(const int program, const int ion, const int target);

/** @brief Fill arrays with the default (tabulated) energy and stopping power values.
 *
 *  @param[in]  program   Program identifier.
 *  @param[in]  ion       Ion identifier.
 *  @param[in]  target    Target material identifier.
 *  @param[out] energies  Array to receive energies in MeV/u.
 *  @param[out] stps      Array to receive stopping powers in MeV cm²/g.
 *  @return Number of points filled, or negative error code on failure.
 */
int dedx_fill_default_energy_stp_table(
    const int program, const int ion, const int target, float *energies, float *stps);

/** @brief Compute CSDA ranges at arbitrary energy points.
 *
 *  The Continuous Slowing Down Approximation (CSDA) range is the path length
 *  a particle travels while losing all its kinetic energy.
 *
 *  @param[in]  program      Program identifier.
 *  @param[in]  ion          Ion identifier.
 *  @param[in]  target       Target material identifier.
 *  @param[in]  no_of_points Number of energy points.
 *  @param[in]  energies     Array of energies in MeV/u (length: no_of_points).
 *  @param[out] csda_ranges  Array to receive CSDA ranges in g/cm².
 *  @return 0 on success, non-zero error code on failure.
 */
int dedx_get_csda_range_table(const int program,
                              const int ion,
                              const int target,
                              const int no_of_points,
                              const float *energies,
                              double *csda_ranges);

#endif // DEDX_WRAPPERS_INCLUDED
