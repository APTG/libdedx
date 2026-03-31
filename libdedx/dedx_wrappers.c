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

#include "dedx_wrappers.h"

#include <stdlib.h>

#include "dedx_lookup_data.h"
#include "dedx_periodic_table.h"
#include "dedx_tools.h"

static dedx_config *allocate_wrapper_config(int program, int ion, int target, int *err);
static dedx_workspace *allocate_wrapper_workspace(int *err);

void dedx_fill_program_list(int *program_list) {
    /* fill list of available programs, terminated with -1 */
    int i = 0;
    const int *available_programs = dedx_get_program_list();

    while (available_programs[i] != -1) {
        program_list[i] = available_programs[i];
        i++;
    }
    program_list[i] = -1;
}

void dedx_fill_material_list(int program, int *material_list) {
    /* fills list of available materials, terminated with -1 */
    const int *available_material_list = dedx_get_material_list(program);
    int i = 0;
    while (available_material_list[i] != -1) {
        material_list[i] = available_material_list[i];
        i++;
    }
    material_list[i] = -1;
}

void dedx_fill_ion_list(int program, int *ion_list) {
    int i = 0;
    const int *available_ion_list = dedx_get_ion_list(program);
    while (available_ion_list[i] != -1) {
        ion_list[i] = available_ion_list[i];
        i++;
    }
    ion_list[i] = -1;
}

static dedx_config *allocate_wrapper_config(int program, int ion, int target, int *err) {
    dedx_config *config;

    *err = DEDX_OK;
    config = (dedx_config *) calloc(1, sizeof(dedx_config));
    if (config == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        return NULL;
    }

    config->program = program;
    config->ion = ion;
    config->target = target;
    return config;
}

static dedx_workspace *allocate_wrapper_workspace(int *err) {
    return dedx_allocate_workspace(1, err);
}

int dedx_get_stp_table(
    const int program, const int ion, const int target, const int no_of_points, const float *energies, float *stps) {
    int err = 0;
    int cleanup_err = DEDX_OK;
    int i;
    dedx_config *config = allocate_wrapper_config(program, ion, target, &err);
    dedx_workspace *ws;

    if (err != 0)
        return err;
    ws = allocate_wrapper_workspace(&err);
    if (err != 0) {
        dedx_free_config(config, &cleanup_err);
        return err;
    }
    dedx_load_config(ws, config, &err);
    if (err != 0) {
        dedx_free_config(config, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return err;
    }

    for (i = 0; i < no_of_points; i++) {
        stps[i] = dedx_get_stp(ws, config, energies[i], &err);
        if (err != 0)
            break;
    }
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return err;
}

float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err) {
    int cleanup_err = DEDX_OK;
    float stp;
    dedx_config *config = allocate_wrapper_config(program, ion, target, err);
    dedx_workspace *ws;

    if (*err != 0)
        return 0.0f;
    ws = allocate_wrapper_workspace(err);
    if (*err != 0) {
        dedx_free_config(config, &cleanup_err);
        return 0.0f;
    }
    dedx_load_config(ws, config, err);
    if (*err != 0) {
        dedx_free_config(config, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return 0.0f;
    }

    stp = dedx_get_stp(ws, config, energy, err);
    dedx_free_config(config, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);
    if (*err != 0)
        return 0.0f;

    return stp;
}

int dedx_get_stp_table_size(const int program, const int ion, const int target) {
    int err = 0;
    int cleanup_err = DEDX_OK;
    int result = -1;
    dedx_config *cfg = allocate_wrapper_config(program, ion, target, &err);
    dedx_workspace *ws;

    if (err != 0)
        return -1;
    ws = allocate_wrapper_workspace(&err);
    if (err != 0) {
        dedx_free_config(cfg, &cleanup_err);
        return -1;
    }

    dedx_load_config(ws, cfg, &err);
    if (err != 0) {
        dedx_free_config(cfg, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return -1;
    }

    if (ws->active_datasets > 0) {
        result = ws->loaded_data[cfg->cfg_id]->n;
    }

    dedx_free_config(cfg, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);
    return result;
}

int dedx_fill_default_energy_stp_table(
    const int program, const int ion, const int target, float *energies, float *stps) {
    int err = 0;
    int cleanup_err = DEDX_OK;
    int i;
    dedx_config *cfg = allocate_wrapper_config(program, ion, target, &err);
    dedx_workspace *ws;

    if (err != 0)
        return -1;
    ws = allocate_wrapper_workspace(&err);
    if (err != 0) {
        dedx_free_config(cfg, &cleanup_err);
        return -1;
    }

    dedx_load_config(ws, cfg, &err);
    if (err != 0) {
        dedx_free_config(cfg, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return -1;
    }

    if (ws->active_datasets > 0) {
        for (i = 0; i < ws->loaded_data[cfg->cfg_id]->n; i++) {
            energies[i] = ws->loaded_data[cfg->cfg_id]->base[i].x;
            stps[i] = dedx_get_stp(ws, cfg, energies[i], &err);
            if (err != 0) {
                dedx_free_config(cfg, &cleanup_err);
                dedx_free_workspace(ws, &cleanup_err);
                return -1;
            }
        }
    }

    dedx_free_config(cfg, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);
    return 0;
}

int dedx_get_csda_range_table(const int program,
                              const int ion,
                              const int target,
                              const int no_of_points,
                              const float *energies,
                              double *csda_ranges) {
    int err = 0;
    int cleanup_err = DEDX_OK;
    int i;
    dedx_config *config = allocate_wrapper_config(program, ion, target, &err);
    dedx_workspace *ws;

    if (err != 0)
        return err;
    config->ion_a = dedx_internal_get_nucleon(config->ion, &err);
    if (err != 0) {
        dedx_free_config(config, &cleanup_err);
        return err;
    }
    ws = allocate_wrapper_workspace(&err);
    if (err != 0) {
        dedx_free_config(config, &cleanup_err);
        return err;
    }
    dedx_load_config(ws, config, &err);
    if (err != 0) {
        dedx_free_config(config, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return err;
    }

    for (i = 0; i < no_of_points; i++) {
        csda_ranges[i] = dedx_get_csda(ws, config, energies[i], &err);
        if (err != 0)
            break;
    }
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return err;
}
