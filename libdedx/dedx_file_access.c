/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dedx_file_access.h"

#include <string.h>

#include "dedx_embedded_data.h"
#include "dedx_embedded_metadata.h"

static int read_embedded_stopping_data(stopping_data *data, int prog, int ion, int target) {
    const float *stp = NULL;
    int energy_len = 0;

    if (dedx_embedded_find_table(prog, ion, target, NULL, &energy_len, &stp) != 0) {
        return -1;
    }

    memset(data, 0, sizeof(*data));
    data->target = target;
    data->ion = ion;
    data->length = (unsigned int) energy_len;
    memcpy(data->data, stp, sizeof(float) * (size_t) energy_len);
    return 0;
}

static int read_embedded_energy_data(float *energy, int prog) {
    const float *embedded_energy = NULL;
    int energy_len = 0;

    if (dedx_embedded_get_energy_grid(prog, &embedded_energy, &energy_len) != 0) {
        return -1;
    }

    memset(energy, 0, sizeof(float) * DEDX_MAX_ELEMENTS);
    memcpy(energy, embedded_energy, sizeof(float) * (size_t) energy_len);
    return 0;
}

void dedx_internal_read_binary_data(stopping_data *data, int prog, int ion, int target, int *err) {
    *err = DEDX_OK;
    if (read_embedded_stopping_data(data, prog, ion, target) == 0) {
        return;
    }
    if (dedx_embedded_get_program_data(prog) != NULL ||
        prog == DEDX_ICRU || prog == DEDX_ICRU49 || prog == DEDX_ICRU73) {
        *err = DEDX_ERR_COMBINATION_NOT_FOUND;
    } else {
        *err = DEDX_ERR_NO_BINARY_DATA;
    }
}

void dedx_internal_read_energy_data(float *energy, int prog, int *err) {
    *err = DEDX_OK;
    if (read_embedded_energy_data(energy, prog) == 0) {
        return;
    }
    *err = DEDX_ERR_NO_BINARY_ENERGY;
}

float dedx_internal_read_effective_charge(int id, int *err) {
    float charge = -1.0f;

    *err = DEDX_OK;
    if (dedx_embedded_read_effective_charge(id, &charge) == 0) {
        return charge;
    }
    return charge;
}

size_t dedx_internal_target_is_gas(int target, int *err) {
    *err = DEDX_OK;
    return (size_t) dedx_embedded_target_is_gas(target);
}

float dedx_internal_read_density(int id, int *err) {
    float density = 0.0f;

    *err = DEDX_OK;
    if (dedx_embedded_read_density(id, &density) != 0) {
        *err = DEDX_ERR_TARGET_NOT_FOUND;
        return 0.0f;
    }
    return density;
}

float dedx_internal_get_i_value(int target, int state, int *err) {
    float pot = 0.0f;

    *err = DEDX_OK;
    if (dedx_embedded_get_i_value(target, state, &pot) != 0) {
        *err = DEDX_ERR_TARGET_NOT_FOUND;
        return 0.0f;
    }
    return pot;
}

void dedx_internal_get_composition(int target, float composition[][2], unsigned int *length, int *err) {
    *err = DEDX_OK;
    if (dedx_embedded_get_composition(target, composition, length) != 0) {
        *length = 0;
    }
}
