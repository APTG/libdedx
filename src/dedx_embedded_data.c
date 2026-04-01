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

#include "dedx_embedded_data.h"

#include <stddef.h>

#include "data/embedded/dedx_astar.h"
#include "data/embedded/dedx_bethe.h"
#include "data/embedded/dedx_icru73.h"
#include "data/embedded/dedx_icru73new.h"
#include "data/embedded/dedx_icru90_C.h"
#include "data/embedded/dedx_icru90_a.h"
#include "data/embedded/dedx_icru90_p.h"
#include "data/embedded/dedx_icru_astar.h"
#include "data/embedded/dedx_icru_pstar.h"
#include "data/embedded/dedx_mstar.h"
#include "data/embedded/dedx_pstar.h"

enum {
    DEDX_EMBEDDED_ICRU90_P = 1001,
    DEDX_EMBEDDED_ICRU90_A,
    DEDX_EMBEDDED_ICRU90_C,
};

static const dedx_embedded_program_data dedx_embedded_astar = {
    .energy = dedx_astar_energy,
    .energy_len = (int) (sizeof(dedx_astar_energy) / sizeof(dedx_astar_energy[0])),
    .ion_ids = dedx_astar_ion_ids,
    .ion_count = (int) (sizeof(dedx_astar_ion_ids) / sizeof(dedx_astar_ion_ids[0])),
    .target_ids = dedx_astar_target_ids,
    .target_count = (int) (sizeof(dedx_astar_target_ids) / sizeof(dedx_astar_target_ids[0])),
    .stp = &dedx_astar_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_pstar = {
    .energy = dedx_pstar_energy,
    .energy_len = (int) (sizeof(dedx_pstar_energy) / sizeof(dedx_pstar_energy[0])),
    .ion_ids = dedx_pstar_ion_ids,
    .ion_count = (int) (sizeof(dedx_pstar_ion_ids) / sizeof(dedx_pstar_ion_ids[0])),
    .target_ids = dedx_pstar_target_ids,
    .target_count = (int) (sizeof(dedx_pstar_target_ids) / sizeof(dedx_pstar_target_ids[0])),
    .stp = &dedx_pstar_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru_pstar = {
    .energy = dedx_icru_pstar_energy,
    .energy_len = (int) (sizeof(dedx_icru_pstar_energy) / sizeof(dedx_icru_pstar_energy[0])),
    .ion_ids = dedx_icru_pstar_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru_pstar_ion_ids) / sizeof(dedx_icru_pstar_ion_ids[0])),
    .target_ids = dedx_icru_pstar_target_ids,
    .target_count = (int) (sizeof(dedx_icru_pstar_target_ids) / sizeof(dedx_icru_pstar_target_ids[0])),
    .stp = &dedx_icru_pstar_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru_astar = {
    .energy = dedx_icru_astar_energy,
    .energy_len = (int) (sizeof(dedx_icru_astar_energy) / sizeof(dedx_icru_astar_energy[0])),
    .ion_ids = dedx_icru_astar_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru_astar_ion_ids) / sizeof(dedx_icru_astar_ion_ids[0])),
    .target_ids = dedx_icru_astar_target_ids,
    .target_count = (int) (sizeof(dedx_icru_astar_target_ids) / sizeof(dedx_icru_astar_target_ids[0])),
    .stp = &dedx_icru_astar_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_mstar = {
    .energy = dedx_mstar_energy,
    .energy_len = (int) (sizeof(dedx_mstar_energy) / sizeof(dedx_mstar_energy[0])),
    .ion_ids = dedx_mstar_ion_ids,
    .ion_count = (int) (sizeof(dedx_mstar_ion_ids) / sizeof(dedx_mstar_ion_ids[0])),
    .target_ids = dedx_mstar_target_ids,
    .target_count = (int) (sizeof(dedx_mstar_target_ids) / sizeof(dedx_mstar_target_ids[0])),
    .stp = &dedx_mstar_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru73_old = {
    .energy = dedx_icru73_energy,
    .energy_len = (int) (sizeof(dedx_icru73_energy) / sizeof(dedx_icru73_energy[0])),
    .ion_ids = dedx_icru73_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru73_ion_ids) / sizeof(dedx_icru73_ion_ids[0])),
    .target_ids = dedx_icru73_target_ids,
    .target_count = (int) (sizeof(dedx_icru73_target_ids) / sizeof(dedx_icru73_target_ids[0])),
    .stp = &dedx_icru73_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru73 = {
    .energy = dedx_icru73new_energy,
    .energy_len = (int) (sizeof(dedx_icru73new_energy) / sizeof(dedx_icru73new_energy[0])),
    .ion_ids = dedx_icru73new_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru73new_ion_ids) / sizeof(dedx_icru73new_ion_ids[0])),
    .target_ids = dedx_icru73new_target_ids,
    .target_count = (int) (sizeof(dedx_icru73new_target_ids) / sizeof(dedx_icru73new_target_ids[0])),
    .stp = &dedx_icru73new_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru90_p = {
    .energy = dedx_icru90_p_energy,
    .energy_len = (int) (sizeof(dedx_icru90_p_energy) / sizeof(dedx_icru90_p_energy[0])),
    .ion_ids = dedx_icru90_p_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru90_p_ion_ids) / sizeof(dedx_icru90_p_ion_ids[0])),
    .target_ids = dedx_icru90_p_target_ids,
    .target_count = (int) (sizeof(dedx_icru90_p_target_ids) / sizeof(dedx_icru90_p_target_ids[0])),
    .stp = &dedx_icru90_p_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru90_a = {
    .energy = dedx_icru90_a_energy,
    .energy_len = (int) (sizeof(dedx_icru90_a_energy) / sizeof(dedx_icru90_a_energy[0])),
    .ion_ids = dedx_icru90_a_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru90_a_ion_ids) / sizeof(dedx_icru90_a_ion_ids[0])),
    .target_ids = dedx_icru90_a_target_ids,
    .target_count = (int) (sizeof(dedx_icru90_a_target_ids) / sizeof(dedx_icru90_a_target_ids[0])),
    .stp = &dedx_icru90_a_stp[0][0][0],
};

static const dedx_embedded_program_data dedx_embedded_icru90_C = {
    .energy = dedx_icru90_C_energy,
    .energy_len = (int) (sizeof(dedx_icru90_C_energy) / sizeof(dedx_icru90_C_energy[0])),
    .ion_ids = dedx_icru90_C_ion_ids,
    .ion_count = (int) (sizeof(dedx_icru90_C_ion_ids) / sizeof(dedx_icru90_C_ion_ids[0])),
    .target_ids = dedx_icru90_C_target_ids,
    .target_count = (int) (sizeof(dedx_icru90_C_target_ids) / sizeof(dedx_icru90_C_target_ids[0])),
    .stp = &dedx_icru90_C_stp[0][0][0],
};

static int find_index(const int *values, int count, int key) {
    int i;

    for (i = 0; i < count; i++) {
        if (values[i] == key) {
            return i;
        }
    }
    return -1;
}

const dedx_embedded_program_data *dedx_embedded_get_program_data(int program) {
    switch (program) {
    case DEDX_ASTAR:
        return &dedx_embedded_astar;
    case _DEDX_0008:
        return &dedx_embedded_icru_pstar;
    case DEDX_ICRU49:
        return &dedx_embedded_icru_astar;
    case DEDX_ICRU73_OLD:
        return &dedx_embedded_icru73_old;
    case DEDX_ICRU73:
        return &dedx_embedded_icru73;
    case DEDX_EMBEDDED_ICRU90_P:
        return &dedx_embedded_icru90_p;
    case DEDX_EMBEDDED_ICRU90_A:
        return &dedx_embedded_icru90_a;
    case DEDX_EMBEDDED_ICRU90_C:
        return &dedx_embedded_icru90_C;
    case DEDX_MSTAR:
        return &dedx_embedded_mstar;
    case DEDX_PSTAR:
        return &dedx_embedded_pstar;
    default:
        return NULL;
    }
}

int dedx_embedded_has_table(int program, int ion, int target) {
    const dedx_embedded_program_data *data = dedx_embedded_get_program_data(program);

    if (data == NULL) {
        return 0;
    }

    return find_index(data->ion_ids, data->ion_count, ion) >= 0
           && find_index(data->target_ids, data->target_count, target) >= 0;
}

int dedx_embedded_resolve_program(int program, int ion, int target, int *resolved_program) {
    if (resolved_program == NULL) {
        return -1;
    }

    switch (program) {
    case DEDX_ICRU49:
        if (ion == 1 && dedx_embedded_has_table(_DEDX_0008, ion, target)) {
            *resolved_program = _DEDX_0008;
            return 0;
        }
        if (ion == 2 && dedx_embedded_has_table(DEDX_ICRU49, ion, target)) {
            *resolved_program = DEDX_ICRU49;
            return 0;
        }
        return -1;
    case DEDX_ICRU73:
        if (dedx_embedded_has_table(DEDX_ICRU73, ion, target)) {
            *resolved_program = DEDX_ICRU73;
            return 0;
        }
        if (dedx_embedded_has_table(DEDX_ICRU73_OLD, ion, target)) {
            *resolved_program = DEDX_ICRU73_OLD;
            return 0;
        }
        return -1;
    case DEDX_ICRU:
        if (ion == 1) {
            if (dedx_embedded_has_table(DEDX_EMBEDDED_ICRU90_P, ion, target)) {
                *resolved_program = DEDX_EMBEDDED_ICRU90_P;
                return 0;
            }
            if (dedx_embedded_has_table(_DEDX_0008, ion, target)) {
                *resolved_program = _DEDX_0008;
                return 0;
            }
            return -1;
        }
        if (ion == 2) {
            if (dedx_embedded_has_table(DEDX_EMBEDDED_ICRU90_A, ion, target)) {
                *resolved_program = DEDX_EMBEDDED_ICRU90_A;
                return 0;
            }
            if (dedx_embedded_has_table(DEDX_ICRU49, ion, target)) {
                *resolved_program = DEDX_ICRU49;
                return 0;
            }
            return -1;
        }
        if (ion == 6 && dedx_embedded_has_table(DEDX_EMBEDDED_ICRU90_C, ion, target)) {
            *resolved_program = DEDX_EMBEDDED_ICRU90_C;
            return 0;
        }
        if (dedx_embedded_has_table(DEDX_ICRU73, ion, target)) {
            *resolved_program = DEDX_ICRU73;
            return 0;
        }
        if (dedx_embedded_has_table(DEDX_ICRU73_OLD, ion, target)) {
            *resolved_program = DEDX_ICRU73_OLD;
            return 0;
        }
        return -1;
    default:
        if (dedx_embedded_get_program_data(program) != NULL) {
            *resolved_program = program;
            return 0;
        }
        return -1;
    }
}

int dedx_embedded_find_table(
    int program, int ion, int target, const float **energy, int *energy_len, const float **stp) {
    const dedx_embedded_program_data *data = dedx_embedded_get_program_data(program);
    int ion_index;
    int target_index;
    size_t offset;

    if (data == NULL) {
        return -1;
    }

    ion_index = find_index(data->ion_ids, data->ion_count, ion);
    target_index = find_index(data->target_ids, data->target_count, target);
    if (ion_index < 0 || target_index < 0) {
        return -1;
    }

    offset = (size_t) ion_index * (size_t) data->target_count * (size_t) data->energy_len;
    offset += (size_t) target_index * (size_t) data->energy_len;

    if (energy != NULL) {
        *energy = data->energy;
    }
    if (energy_len != NULL) {
        *energy_len = data->energy_len;
    }
    if (stp != NULL) {
        *stp = data->stp + offset;
    }

    return 0;
}

int dedx_embedded_get_energy_grid(int program, const float **energy, int *energy_len) {
    const dedx_embedded_program_data *data = dedx_embedded_get_program_data(program);

    if (program == DEDX_BETHE_EXT00) {
        if (energy != NULL) {
            *energy = dedx_bethe_energy;
        }
        if (energy_len != NULL) {
            *energy_len = (int) (sizeof(dedx_bethe_energy) / sizeof(dedx_bethe_energy[0]));
        }
        return 0;
    }

    if (data == NULL) {
        return -1;
    }

    if (energy != NULL) {
        *energy = data->energy;
    }
    if (energy_len != NULL) {
        *energy_len = data->energy_len;
    }

    return 0;
}
