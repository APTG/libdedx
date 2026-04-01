#ifndef DEDX_EMBEDDED_DATA_H
#define DEDX_EMBEDDED_DATA_H

#include "dedx.h"

/** @brief In-memory descriptor for one tabulated stopping-power program. */
typedef struct {
    const float *energy;   /**< Energy grid in MeV/u. */
    int energy_len;        /**< Number of energy grid points. */
    const int *ion_ids;    /**< Supported ion IDs for this program. */
    int ion_count;         /**< Number of supported ions. */
    const int *target_ids; /**< Supported target IDs for this program. */
    int target_count;      /**< Number of supported targets. */
    const float *stp;      /**< Flattened table data in [ion][target][energy] order. */
} dedx_embedded_program_data;

/** @brief Return the embedded descriptor for a program, or NULL if not embedded yet. */
const dedx_embedded_program_data *dedx_embedded_get_program_data(int program);

/** @brief Check whether an embedded program contains a given ion/target table.
 *  @param[in] program  Program identifier.
 *  @param[in] ion      Ion identifier.
 *  @param[in] target   Target identifier.
 *  @return 1 if the table exists in the embedded dataset, 0 otherwise.
 */
int dedx_embedded_has_table(int program, int ion, int target);

/** @brief Resolve a logical program to the best matching embedded raw dataset.
 *  @param[in]  program           Requested program identifier.
 *  @param[in]  ion               Ion identifier.
 *  @param[in]  target            Target identifier.
 *  @param[out] resolved_program  Receives the raw dataset program identifier.
 *  @return 0 on success, -1 if no embedded resolution is available.
 */
int dedx_embedded_resolve_program(int program, int ion, int target, int *resolved_program);

/** @brief Resolve one embedded table by program, ion, and target.
 *
 *  @param[in]  program   Program identifier.
 *  @param[in]  ion       Ion identifier.
 *  @param[in]  target    Target identifier.
 *  @param[out] energy    Receives pointer to the shared energy grid.
 *  @param[out] energy_len Receives the energy-grid length.
 *  @param[out] stp       Receives pointer to the stopping-power row of length energy_len.
 *  @return 0 on success, -1 if the program or combination is not embedded.
 */
int dedx_embedded_find_table(
    int program, int ion, int target, const float **energy, int *energy_len, const float **stp);

#endif // DEDX_EMBEDDED_DATA_H
