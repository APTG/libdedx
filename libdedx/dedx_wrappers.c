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
#include "dedx_tools.h"
#include "dedx_periodic_table.h"

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


//energy - in MeV/nucleon, unless  program is ESTAR, then MeV
//stp - in MeVcm2/g
int dedx_get_stp_table(const int program, const int ion, const int target, const int no_of_points,
                       const float *energies, float *stps) {
    int err = 0;
    dedx_config *config = (dedx_config *) calloc(1, sizeof(dedx_config));
    config->target = target;
    config->ion = ion;
    config->program = program;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    if (err != 0) return err;
    dedx_load_config(ws, config, &err);

    for (int i = 0; i < no_of_points; i++) {
        stps[i] = dedx_get_stp(ws, config, energies[i], &err);
    }
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return err;
}

float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err) {
    float stp;
    dedx_config *config = (dedx_config *) calloc(1, sizeof(dedx_config));
    config->target = target;
    config->ion = ion;
    config->program = program;
    dedx_workspace *ws = dedx_allocate_workspace(1, err);
    if (*err != 0)
        return 0;
    dedx_load_config(ws, config, err);

    stp = dedx_get_stp(ws, config, energy, err);
    if (*err != 0)
        return 0;
    dedx_free_config(config, err);
    dedx_free_workspace(ws, err);
    return stp;
}

int dedx_get_stp_table_size(const int program, const int ion, const int target) {
    int err = 0;
    int result = -1;

    dedx_workspace *ws;
    dedx_config *cfg = (dedx_config *) calloc(1, sizeof(dedx_config));
    ws = dedx_allocate_workspace(1, &err);

    // set program + ion + target combination
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;

    // load spline database from the data files, it contains number of energy+stp samples
    dedx_load_config(ws, cfg, &err);

    // config loading failed, returning exit code
    if (err != 0)
        return -1;

    // assume only one dataset is loaded, extract number of samples
    if (ws->datasets == 1) {
        result = ws->loaded_data[0]->n;
    }

    return result;
};

int dedx_fill_default_energy_stp_table(const int program, const int ion, const int target, float *energies, float *stps) {
    int err = 0;
    int i;

    dedx_workspace *ws;
    dedx_config *cfg = (dedx_config *) calloc(1, sizeof(dedx_config));
    ws = dedx_allocate_workspace(1, &err);

    // set program + ion + target combination
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;

    // load spline database from the data files
    dedx_load_config(ws, cfg, &err);

    // config loading failed, returning exit code
    if (err != 0)
        return -1;

    // assume only one dataset is loaded, extract default no of energies and stopping powers
    if (ws->datasets == 1) {
        for (i = 0; i < ws->loaded_data[0]->n; i++) {
            energies[i] = ws->loaded_data[0]->base[i].x;
            stps[i] = dedx_get_stp(ws, cfg, energies[i], &err);
            if (err != 0)
                return -1;
        }
    }

    return 0;

};

int dedx_get_csda_range_table(const int program, const int ion, const int target, const int no_of_points,
                       const float *energies, double *csda_ranges) {
    int err = 0;
    dedx_config *config = (dedx_config *) calloc(1, sizeof(dedx_config));
    config->target = target;
    config->ion = ion;
    config->program = program;
    config->ion_a = _dedx_get_nucleon(config->ion, &err);
    if (err != 0) return err;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    if (err != 0) return err;
    dedx_load_config(ws, config, &err);

    for (int i = 0; i < no_of_points; i++) {
        csda_ranges[i] = dedx_get_csda(ws, config, energies[i], &err);
    }
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return err;
}