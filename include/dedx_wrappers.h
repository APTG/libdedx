#ifndef DEDX_WRAPPERS_H
#define DEDX_WRAPPERS_H

/**
 * @file dedx_wrappers.h
 * @brief Convenience wrappers for common libdedx operations.
 *
 * These functions handle workspace allocation internally and are suitable
 * for one-off queries. For repeated evaluations use the core API in dedx.h.
 */

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
 *  @param[in]  energies     Array of energies in MeV/nucl (MeV per nucleon, length: no_of_points).
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
 *  @param[in]  energy   Kinetic energy in MeV/nucl (MeV per nucleon).
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
 *  @param[out] energies  Array to receive energies in MeV/nucl (MeV per nucleon).
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
 *  @param[in]  energies     Array of energies in MeV/nucl (MeV per nucleon, length: no_of_points).
 *  @param[out] csda_ranges  Array to receive CSDA ranges in g/cm².
 *  @return 0 on success, non-zero error code on failure.
 */
int dedx_get_csda_range_table(const int program,
                              const int ion,
                              const int target,
                              const int no_of_points,
                              const float *energies,
                              double *csda_ranges);

/** @brief One-call inverse CSDA: energy for a given CSDA range.
 *
 *  Allocates a fresh workspace and configuration internally, sets the nucleon
 *  number automatically, and is safe to call without managing object
 *  lifetimes (suitable for JavaScript/WASM and other flat-API consumers).
 *
 *  @param[in]  program  Program identifier.
 *  @param[in]  ion      Ion identifier.
 *  @param[in]  target   Target material identifier.
 *  @param[in]  range    CSDA range in g/cm².
 *  @param[out] err      Error code; 0 on success.
 *  @return Kinetic energy in MeV/nucl (MeV per nucleon), or -1 on failure.
 */
double dedx_get_inverse_csda_simple(int program, int ion, int target, double range, int *err);

/** @brief One-call inverse stopping power: energy for a given stopping power.
 *
 *  Allocates a fresh workspace and configuration internally and sets the
 *  nucleon number automatically. Because stopping power is non-monotonic the
 *  @p side parameter selects which branch to invert (see dedx_get_inverse_stp).
 *
 *  @param[in]  program  Program identifier.
 *  @param[in]  ion      Ion identifier.
 *  @param[in]  target   Target material identifier.
 *  @param[in]  stp      Target stopping power in MeV cm²/g.
 *  @param[in]  side     Branch selector (see dedx_get_inverse_stp).
 *  @param[out] err      Error code; 0 on success.
 *  @return Kinetic energy in MeV/nucl (MeV per nucleon), or -1 on failure.
 */
double dedx_get_inverse_stp_simple(int program, int ion, int target, double stp, int side, int *err);

/** @brief One-call Bragg peak: maximum stopping power for an ion/target pair.
 *
 *  Allocates a fresh workspace and configuration internally and sets the
 *  nucleon number automatically, then returns the peak (maximum) mass
 *  stopping power.
 *
 *  @param[in]  program  Program identifier.
 *  @param[in]  ion      Ion identifier.
 *  @param[in]  target   Target material identifier.
 *  @param[out] err      Error code; 0 on success.
 *  @return Peak mass stopping power in MeV cm²/g, or -1 on failure.
 */
double dedx_get_bragg_peak_stp_simple(int program, int ion, int target, int *err);

#endif // DEDX_WRAPPERS_H
