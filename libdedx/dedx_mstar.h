#ifndef DEDX_MSTAR_H
#define DEDX_MSTAR_H

#include "dedx.h"
#include "dedx_mpaul.h"
#include "dedx_stopping_data.h"

/** @brief Resolve MSTAR auto modes `a`/`b` to the actual condensed or gaseous mode.
 *  @param[in,out] config  Configuration whose `mstar_mode` may be rewritten.
 *  @param[out]    err     Error code; 0 on success.
 *
 *  MSTAR mode semantics follow the original `MSTAR1` routine:
 *  `a` selects `c` for condensed targets and `g` for gaseous targets,
 *  while `b` selects `d` for condensed targets and `h` for gaseous targets.
 */
void dedx_internal_evaluate_compound_state_mstar(dedx_config *config, int *err);

/** @brief Convert alpha-particle stopping data to heavy-ion MSTAR stopping data.
 *  @param[in]     in      Input alpha stopping-power table.
 *  @param[out]    out     Output heavy-ion stopping-power table.
 *  @param[in]     state   Requested MSTAR mode (`a`,`b`,`c`,`d`,`g`,`h`).
 *  @param[in]     config  Resolved configuration, including target state when available.
 *  @param[in,out] energy  Energy grid, converted in place from alpha to ion scaling.
 *  @param[out]    err     Error code; 0 on success.
 */
void dedx_internal_convert_energy_to_mstar(
    stopping_data *in, stopping_data *out, char state, dedx_config *config, float *energy, int *err);

#endif // DEDX_MSTAR_H
