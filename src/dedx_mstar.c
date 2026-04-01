/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dedx_mstar.h"

#include "dedx_data_access.h"

static char resolve_mstar_mode(char state, dedx_config *config, int *err) {
    int target_state = config->compound_state;

    if (state != DEDX_MSTAR_MODE_A && state != DEDX_MSTAR_MODE_B)
        return state;

    if (target_state == DEDX_DEFAULT_STATE) {
        if (dedx_internal_target_is_gas(config->target, err) != 0) {
            target_state = DEDX_GAS;
        } else {
            if (*err != DEDX_OK)
                return state;
            target_state = DEDX_CONDENSED;
        }
    }

    if (target_state == DEDX_GAS) {
        return (state == DEDX_MSTAR_MODE_A) ? DEDX_MSTAR_MODE_G : DEDX_MSTAR_MODE_H;
    }
    return (state == DEDX_MSTAR_MODE_A) ? DEDX_MSTAR_MODE_C : DEDX_MSTAR_MODE_D;
}

void dedx_internal_evaluate_compound_state_mstar(dedx_config *config, int *err) { /* LCOV_EXCL_START */
    *err = DEDX_OK;
    config->mstar_mode = resolve_mstar_mode(config->mstar_mode, config, err);
} /* LCOV_EXCL_STOP */

void dedx_internal_convert_energy_to_mstar(
    stopping_data *in, stopping_data *out, char state, dedx_config *config, float *energy, int *err) {
    *err = DEDX_OK;
    state = resolve_mstar_mode(state, config, err);
    if (*err != DEDX_OK)
        return;
    int n = in->length;
    int i;
    for (i = 0; i < n; i++) {
        energy[i] = energy[i] / 4.0;
    }
    for (i = 0; i < n; i++) {
        out->data[i] = dedx_internal_calculate_mspaul_coef(state, in->ion, in->target, energy[i]) * in->data[i] * 1000;
    }
    out->length = in->length;
    out->target = in->target;
    out->ion = in->ion;
}
