#ifndef DEDX_DATA_ACCESS_H
#define DEDX_DATA_ACCESS_H

#include <stddef.h>

#include "dedx_stopping_data.h"

/** @brief Read one tabulated stopping-power dataset from the embedded tables.
 *  @param[out] data    Destination dataset buffer.
 *  @param[in]  prog    Program identifier.
 *  @param[in]  ion     Projectile identifier.
 *  @param[in]  target  Target identifier.
 *  @param[out] err     Set to DEDX_OK or a data error code.
 */
void dedx_internal_read_binary_data(stopping_data *data, int prog, int ion, int target, int *err);

/** @brief Read the energy grid associated with a tabulated program.
 *  @param[out] energy  Destination array of DEDX_MAX_ELEMENTS values.
 *  @param[in]  prog    Program identifier.
 *  @param[out] err     Set to DEDX_OK or a data error code.
 */
void dedx_internal_read_energy_data(float *energy, int prog, int *err);

/** @brief Read an effective charge override for a heavy target or ion ID.
 *  @param[in]  id   Element or material identifier.
 *  @param[out] err  Set to DEDX_OK or a data error code.
 *  @return Effective charge, or 0 on failure.
 */
float dedx_internal_read_effective_charge(int id, int *err);

/** @brief Check whether a target is listed as gaseous in the embedded metadata.
 *  @param[in]  target  Target identifier.
 *  @param[out] err     Set to DEDX_OK or a data error code.
 *  @return Non-zero if the target is gaseous, zero otherwise.
 */
size_t dedx_internal_target_is_gas(int target, int *err);

/** @brief Read the default density for a target from the embedded metadata.
 *  @param[in]  id   Target identifier.
 *  @param[out] err  Set to DEDX_OK or a data error code.
 *  @return Density in g/cm^3, or 0 on failure.
 */
float dedx_internal_read_density(int id, int *err);

/** @brief Read the mean excitation potential for a target and state.
 *  @param[in]  target  Target identifier.
 *  @param[in]  state   Compound-state selector used by the embedded metadata.
 *  @param[out] err     Set to DEDX_OK or a data error code.
 *  @return Mean excitation potential in eV, or 0 on failure.
 */
float dedx_internal_get_i_value(int target, int state, int *err);

/** @brief Read the elemental composition of a compound target.
 *  @param[in]  target       Compound target identifier.
 *  @param[out] composition  Output array of `(element_id, weight)` pairs.
 *  @param[out] length       Number of populated rows in @p composition.
 *  @param[out] err          Set to DEDX_OK or a data error code.
 */
void dedx_internal_get_composition(int target, float composition[][2], unsigned int *length, int *err);
#endif // DEDX_DATA_ACCESS_H
