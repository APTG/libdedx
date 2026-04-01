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

#include "dedx_embedded_metadata.h"

#include "data/dedx_composition.h"
#include "data/dedx_metadata.h"

static int find_target_index(int target) {
    int i;

    for (i = 0; i < (int) (sizeof(_dedx_compos_list) / sizeof(_dedx_compos_list[0])); i++) {
        if (_dedx_compos_list[i] == target) {
            return i;
        }
    }
    return -1;
}

int dedx_embedded_read_effective_charge(int id, float *charge) {
    int i;

    if (charge == NULL) {
        return -1;
    }
    if (id < 99) {
        *charge = (float) id;
        return 0;
    }

    for (i = 0; i < (int) (sizeof(dedx_embedded_charge_rows) / sizeof(dedx_embedded_charge_rows[0])); i++) {
        if (dedx_embedded_charge_rows[i].id == id) {
            *charge = dedx_embedded_charge_rows[i].charge;
            return 0;
        }
    }
    return -1;
}

int dedx_embedded_target_is_gas(int target) {
    int i;

    for (i = 0; i < (int) (sizeof(dedx_embedded_gas_targets) / sizeof(dedx_embedded_gas_targets[0])); i++) {
        if (dedx_embedded_gas_targets[i] == target) {
            return 1;
        }
    }
    return 0;
}

int dedx_embedded_read_density(int id, float *density) {
    int i;

    if (density == NULL) {
        return -1;
    }

    for (i = 0; i < (int) (sizeof(dedx_embedded_compos_rows) / sizeof(dedx_embedded_compos_rows[0])); i++) {
        if (dedx_embedded_compos_rows[i].id == id) {
            *density = dedx_embedded_compos_rows[i].density;
            return 0;
        }
    }
    return -1;
}

int dedx_embedded_get_i_value(int target, int state, float *pot) {
    float value = 0.0f;
    int i;

    if (pot == NULL) {
        return -1;
    }

    for (i = 0; i < (int) (sizeof(dedx_embedded_compos_rows) / sizeof(dedx_embedded_compos_rows[0])); i++) {
        if (dedx_embedded_compos_rows[i].id != target) {
            continue;
        }
        if (dedx_embedded_compos_rows[i].state != 0) {
            if (state == dedx_embedded_compos_rows[i].state) {
                value = dedx_embedded_compos_rows[i].i_value;
            }
            continue;
        }
        value = dedx_embedded_compos_rows[i].i_value;
        if (state == 2 && !dedx_embedded_target_is_gas(target) && target <= 99) {
            value *= 1.13f;
        }
    }

    if (value == 0.0f) {
        return -1;
    }
    *pot = value;
    return 0;
}

int dedx_embedded_get_composition(int target, float composition[][2], unsigned int *length) {
    int idx;
    int start;
    int count;
    int i;

    if (length == NULL) {
        return -1;
    }

    *length = 0;
    idx = find_target_index(target);
    if (idx < 0) {
        return -1;
    }

    start = _dedx_compos_cindx[idx];
    count = _dedx_compos_nele[idx];
    for (i = 0; i < count; i++) {
        composition[i][0] = (float) _dedx_compos_elem[start + i];
        composition[i][1] = _dedx_compos_frac[start + i];
    }
    *length = (unsigned int) count;
    return 0;
}
