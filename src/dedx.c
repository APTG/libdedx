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
#include "dedx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dedx_bethe.h"
#include "dedx_data_access.h"
#include "dedx_embedded_data.h"
#include "dedx_lookup_data.h"
#include "dedx_mstar.h"
#include "dedx_periodic_table.h"
#include "dedx_program_const.h"
#include "dedx_spline.h"
#include "dedx_stopping_data.h"
#include "dedx_validate.h"

#ifndef DEDX_VERSION_MAJOR
#define DEDX_VERSION_MAJOR 0
#endif

#ifndef DEDX_VERSION_MINOR
#define DEDX_VERSION_MINOR 0
#endif

#ifndef DEDX_VERSION_PATCH
#define DEDX_VERSION_PATCH 0
#endif

#ifndef DEDX_VERSION_STRING
#define DEDX_VERSION_STRING "0.0.0-unknown"
#endif

static int load_data(dedx_workspace *ws, stopping_data *data, float *energy, dedx_config *config, int *err);
static int check_energy_bounds(dedx_internal_lookup_data *data, float energy);
static float get_min_energy_icru(int ion);
static float get_max_energy_icru(int ion);
static int check_ion(int prog, int ion);
static int load_config_clean(dedx_workspace *ws, dedx_config *config, int *err);
static int find_data(stopping_data *data, dedx_config *config, float *energy, int *err);
static int load_compound(dedx_workspace *ws, dedx_config *config, int *err);
static int load_bethe_2(stopping_data *data, dedx_config *config, float *energy, int *err);
static int load_bethe_fallback(stopping_data *data, dedx_config *config, float *energy, int *err);
static int element_supported_for_ion(int program, int ion, int element);
static int material_id_supported(int program, int ion, int material);

dedx_workspace *dedx_allocate_workspace(unsigned int count, int *err) {
    int i = 0;
    *err = DEDX_OK;

    dedx_workspace *temp = calloc(1, sizeof(dedx_workspace));
    if (temp == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        return NULL;
    }
    temp->loaded_data = (dedx_internal_lookup_data **) calloc(count, sizeof(dedx_internal_lookup_data *));
    if (temp->loaded_data == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        free(temp);
        return NULL;
    }
    for (i = 0; i < count; i++) {
        temp->loaded_data[i] = calloc(1, sizeof(dedx_internal_lookup_data));
        if (temp->loaded_data[i] == NULL) { /* LCOV_EXCL_START */
            int j;
            for (j = 0; j < i; j++)
                free(temp->loaded_data[j]);
            free((void *) temp->loaded_data);
            free(temp);
            *err = DEDX_ERR_NO_MEMORY;
            return NULL;
        } /* LCOV_EXCL_STOP */
    }
    temp->datasets = count;
    temp->active_datasets = 0;
    return temp;
}

void dedx_free_workspace(dedx_workspace *workspace, int *err) {
    int i = 0;
    *err = DEDX_OK;

    if (workspace == NULL)
        return;

    for (i = 0; i < workspace->datasets; i++) {
        free(workspace->loaded_data[i]);
    }
    free((void *) workspace->loaded_data);
    free(workspace);
}

/*Return an explanation to the error code*/
void dedx_get_error_code(char *err_str, int err) {
    switch (err) {
    case DEDX_OK:
        strcpy(err_str, "No error.");
        break;
    case DEDX_ERR_NO_COMPOS_FILE:
        strcpy(err_str, "Embedded density/I-value metadata is unavailable.");
        break;
    case DEDX_ERR_NO_GAS_FILE:
        strcpy(err_str, "Embedded gas-state metadata is unavailable.");
        break;
    case DEDX_ERR_NO_CHARGE_FILE:
        strcpy(err_str, "Embedded effective-charge metadata is unavailable.");
        break;
    case DEDX_ERR_NO_BINARY_DATA:
        strcpy(err_str, "Embedded stopping-power data is unavailable.");
        break;
    case DEDX_ERR_NO_BINARY_ENERGY:
        strcpy(err_str, "Embedded energy-grid data is unavailable.");
        break;
    case DEDX_ERR_WRITE_FAILED:
        strcpy(err_str, "Unable to write to disk.");
        break;
    case DEDX_ERR_NO_ENERGY_FILE:
        strcpy(err_str, "Legacy energy source data is unavailable.");
        break;
    case DEDX_ERR_NO_DATA_FILE:
        strcpy(err_str, "Legacy stopping-power source data is unavailable.");
        break;
    case DEDX_ERR_NO_NAMES_FILE:
        strcpy(err_str, "Reserved internal metadata code.");
        break;
    case DEDX_ERR_NO_COMPOSITION:
        strcpy(err_str, "Embedded elemental composition metadata is unavailable.");
        break;
    case DEDX_ERR_ENERGY_OUT_OF_RANGE:
        strcpy(err_str, "Energy out of bounds.");
        break;
    case DEDX_ERR_TARGET_NOT_FOUND:
        strcpy(err_str, "Target is not in the embedded metadata.");
        break;
    case DEDX_ERR_COMBINATION_NOT_FOUND:
        strcpy(err_str, "Target and ion combination is not in the embedded data.");
        break;
    case DEDX_ERR_INVALID_DATASET_ID:
        strcpy(err_str, "ID does not exist.");
        break;
    case DEDX_ERR_NOT_AN_ELEMENT:
        strcpy(err_str, "Target is not an atomic element.");
        break;
    case DEDX_ERR_ESTAR_NOT_IMPL:
        strcpy(err_str, "ESTAR is not implemented yet.");
        break;
    case DEDX_ERR_ION_NOT_SUPPORTED_MSTAR:
        strcpy(err_str, "Ion is not supported for MSTAR.");
        break;
    case DEDX_ERR_ION_NOT_SUPPORTED:
        strcpy(err_str, "Ion is not supported for requested table.");
        break;
    case DEDX_ERR_RHO_REQUIRED:
        strcpy(err_str, "Rho must be specified in this configuration.");
        break;
    case DEDX_ERR_ION_A_REQUIRED:
        strcpy(err_str, "ion_a must be specified in this configuration.");
        break;
    case DEDX_ERR_INVALID_I_VALUE:
        strcpy(err_str, "I value must be larger than zero.");
        break;
    case DEDX_ERR_INVALID_INTERPOLATION_MODE:
        strcpy(err_str, "Interpolation mode is not supported.");
        break;
    case DEDX_ERR_NO_MEMORY:
        strcpy(err_str, "Out of memory");
        break;

    default:
        strcpy(err_str, "No such error code.");
        break;
    }
}

const char *dedx_get_program_name(int program) {
    return dedx_program_table[program];
}

const char *dedx_get_program_version(int program) {
    return dedx_program_version_table[program];
}

const char *dedx_get_material_name(int material) {
    return dedx_material_table[material];
}

const char *dedx_get_ion_name(int ion) {
    return dedx_ion_table[ion];
}

void dedx_get_version(int *major, int *minor, int *patch) {
    *major = DEDX_VERSION_MAJOR;
    *minor = DEDX_VERSION_MINOR;
    *patch = DEDX_VERSION_PATCH;
    return;
}

const char *dedx_get_version_string(void) {
    return DEDX_VERSION_STRING;
}

void dedx_get_composition(int target, float composition[][2], unsigned int *comp_len, int *err) {
    dedx_internal_get_composition(target, composition, comp_len, err);
}

float dedx_get_i_value(int target, int *err) {
    return dedx_internal_get_i_value(target, DEDX_GAS, err);
}

const int *dedx_get_program_list(void) {
    /* returns a list of available programs, terminated with -1 */
    return dedx_available_programs;
}

const int *dedx_get_material_list(int program) {
    /* returns a list of available materials, terminated with -1 */
    if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT || program == DEDX_AUTO)
        return dedx_program_available_materials[0];
    else
        return dedx_program_available_materials[program];
}

/* Checks whether a single element (target <= 99) is reachable for program/ion,
 * mirroring find_data()'s own dispatch (MSTAR's ion remap, DEDX_AUTO's Bethe-Bloch
 * fallback, the resolve-across-sub-tables behavior of the ICRU-family programs) so
 * the answer stays consistent with what dedx_load_config() will actually do.
 *
 * Returns 1 if dedx_load_config() is expected to succeed for this exact
 * (program, ion, element) combination, 0 otherwise. `element` is assumed to already
 * be a plain element id (1-99); callers are responsible for that precondition --
 * see material_id_supported()'s `material <= 99` check below. */
static int element_supported_for_ion(int program, int ion, int element) {
    int ion_load = ion;
    int resolved;

    if (program == DEDX_DEFAULT || program == DEDX_BETHE_EXT00 || program == DEDX_AUTO) {
        return 1; /* Bethe formula: any known element, given caller-supplied rho */
    }
    /* DEDX_ESTAR is not handled specially here: dedx_get_material_list_for_ion() -- the
     * only caller -- already rejects it with DEDX_ERR_ESTAR_NOT_IMPL before reaching this
     * function, and dedx_embedded_has_table() below returns 0 for it regardless (no
     * embedded program data is registered for DEDX_ESTAR). */
    if (program == DEDX_MSTAR && ion > 1) {
        ion_load = 2;
    }
    if (program == DEDX_ICRU || program == DEDX_ICRU49 || program == DEDX_ICRU73) {
        return dedx_embedded_resolve_program(program, ion_load, element, &resolved) == 0;
    }
    return dedx_embedded_has_table(program, ion_load, element);
}

/* Checks whether a material (element or compound) is reachable for program/ion.
 * Compounds are available iff their composition is known and every constituent
 * element is itself reachable, matching the Bragg-additivity decomposition
 * load_compound() performs at calculation time.
 *
 * Returns 1 if dedx_load_config() is expected to succeed for this exact
 * (program, ion, material) combination, 0 otherwise -- for a compound, this recurses
 * once per constituent element and is 1 only if every one of them is reachable. */
static int material_id_supported(int program, int ion, int material) {
    float composition[20][2];
    unsigned int comp_len;
    unsigned int i;
    int comp_err = DEDX_OK;

    if (material <= 0) { /* LCOV_EXCL_START -- master material list never contains this, and
                             composition rows are always positive element IDs in practice */
        return 0;
    } /* LCOV_EXCL_STOP */
    if (material <= 99) {
        return element_supported_for_ion(program, ion, material);
    }

    dedx_internal_get_composition(material, composition, &comp_len, &comp_err);
    if (comp_err != DEDX_OK || comp_len == 0) { /* LCOV_EXCL_START -- every compound in the master
                                                    material list has known embedded composition */
        return 0;
    } /* LCOV_EXCL_STOP */
    for (i = 0; i < comp_len; i++) {
        if (!material_id_supported(program, ion, (int) composition[i][0])) {
            return 0;
        }
    }
    return 1;
}

void dedx_get_material_list_for_ion(
    int program, int ion, int *materials, unsigned int max_materials, unsigned int *materials_len, int *err) {
    const int *candidates = dedx_program_available_materials[0];
    unsigned int count = 0;
    int i;

    *err = DEDX_OK;
    *materials_len = 0;

    if (!check_ion(program, ion)) {
        *err = DEDX_ERR_ION_NOT_SUPPORTED;
        return;
    }
    if (program == DEDX_ESTAR) {
        *err = DEDX_ERR_ESTAR_NOT_IMPL;
        return;
    }
    if ((program == DEDX_DEFAULT || program == DEDX_BETHE_EXT00 || program == DEDX_AUTO) && (ion < 1 || ion > 112)) {
        /* check_ion() accepts any ion in [1, 120] for these programs, but the Bethe
         * evaluation path (dedx_internal_get_atom_mass/charge, via dedx_periodic_table)
         * only has data for ions 1-112. Keep this function consistent with what
         * dedx_load_config() will actually do for the same ion. */
        *err = DEDX_ERR_ION_NOT_SUPPORTED;
        return;
    }

    for (i = 0; candidates[i] != -1 && count < max_materials; i++) {
        if (material_id_supported(program, ion, candidates[i])) {
            materials[count++] = candidates[i];
        }
    }
    *materials_len = count;
}

const int *dedx_get_ion_list(int program) {
    /* Returns a -1-terminated list of ions available for the program. Both branches
     * return immutable const data -- the shared full-ion table for unrestricted
     * programs, or the program's row in dedx_program_available_ions -- so the function
     * performs no writes and is safe to call concurrently from multiple threads. */
    if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT || program == DEDX_AUTO) /* any ion, no restrictions */
        return dedx_full_ion_list;
    else
        return dedx_program_available_ions[program];
}

float dedx_get_min_energy(int program, int ion) {
    float energy_min = 0;

    switch (program) {
    case DEDX_ASTAR:
        energy_min = 0.001 / 4.0;
        break;
    case DEDX_PSTAR:
        energy_min = 0.001;
        break;
    case DEDX_ESTAR:
        energy_min = 0.001;
        break;
    case DEDX_MSTAR:
        energy_min = 0.001;
        break;
    case DEDX_ICRU73_OLD:
        energy_min = get_min_energy_icru(ion);
        break;
    case DEDX_ICRU73:
        energy_min = get_min_energy_icru(ion);
        break;
    case DEDX_ICRU49:
        energy_min = get_min_energy_icru(ion);
        break;
    case DEDX_ICRU:
    case DEDX_AUTO:
        /* DEDX_AUTO resolves through the same tabulated sub-programs as DEDX_ICRU
         * before falling back to Bethe-Bloch, so these advisory bounds mirror
         * DEDX_ICRU's. These are best-effort hints, not authoritative: the actual
         * bounds of whichever tier ends up serving a given target (a tabulated
         * report, or the DEDX_BETHE_EXT00 fallback) may differ; dedx_get_stp()
         * still enforces the real bounds of the dataset it loaded. */
        if (ion == DEDX_CARBON)
            energy_min = 0.001f / 12.0f;
        else
            energy_min = get_min_energy_icru(ion);
        break;
    case DEDX_DEFAULT:
        energy_min = 0.001;
        break;
    case DEDX_BETHE_EXT00:
        energy_min = 0.001;
        break;
    }
    return energy_min;
}

float dedx_get_max_energy(int program, int ion) {
    float energy_max = 0;

    switch (program) {
    case DEDX_ASTAR:
        energy_max = 1000.0 / 4.0;
        break;
    case DEDX_PSTAR:
        energy_max = 10000.0;
        break;
    case DEDX_ESTAR:
        energy_max = 10000.0;
        break;
    case DEDX_MSTAR:
        energy_max = 1000.0;
        break;
    case DEDX_ICRU73_OLD:
        energy_max = get_max_energy_icru(ion);
        break;
    case DEDX_ICRU73:
        energy_max = get_max_energy_icru(ion);
        break;
    case DEDX_ICRU49:
        energy_max = get_max_energy_icru(ion);
        break;
    case DEDX_ICRU:
    case DEDX_AUTO: /* see the comment in dedx_get_min_energy() */
        if (ion == DEDX_CARBON)
            energy_max = 10000.0f / 12.0f;
        else
            energy_max = get_max_energy_icru(ion);
        break;
    case DEDX_DEFAULT:
        energy_max = 1000.0;
        break;
    case DEDX_BETHE_EXT00:
        energy_max = 1000.0;
        break;
    }
    return energy_max;
}

int dedx_load_config(dedx_workspace *ws, dedx_config *config, int *err) {
    int cfg_id = -1;

    config->loaded = 0;
    config->cfg_id = -1;
    dedx_internal_validate_config(config, err);
    if (*err != 0)
        return -1;
    if (config->elements_id != NULL)
        cfg_id = load_compound(ws, config, err);
    else
        cfg_id = load_config_clean(ws, config, err);
    if (*err != 0 || cfg_id < 0)
        return -1;
    dedx_internal_set_names(config, err);
    if (*err != 0)
        return -1;
    config->cfg_id = cfg_id;
    config->loaded = 1;
    return 0;
}

float dedx_get_stp(dedx_workspace *ws, dedx_config *config, float energy, int *err) {
    int id = config->cfg_id;

    *err = DEDX_OK;

    if (id < 0 || id >= ws->active_datasets) {
        *err = DEDX_ERR_INVALID_DATASET_ID;
        return 0;
    }

    // Check that the energy is within the boundary
    *err = check_energy_bounds(ws->loaded_data[id], energy);
    if (*err != DEDX_OK) {
        return 0;
    }

    // Evaluating the spline function
    return dedx_internal_evaluate_spline(ws->loaded_data[id]->base,
                                         energy,
                                         &(ws->loaded_data[id]->acc),
                                         ws->loaded_data[id]->n,
                                         ws->loaded_data[id]->interpolation_mode);
}

void dedx_free_config(dedx_config *config, int *err) {
    if (config != NULL) {
        if (config->elements_id != NULL)
            free(config->elements_id);
        if (config->elements_atoms != NULL)
            free(config->elements_atoms);
        if (config->elements_mass_fraction != NULL)
            free(config->elements_mass_fraction);
        if (config->elements_i_value != NULL)
            free(config->elements_i_value);
        free(config);
    }
    *err = DEDX_OK;
}

float dedx_get_simple_stp(int ion, int target, float energy, int *err) {
    dedx_config *config = NULL;
    dedx_workspace *ws = NULL;
    int cleanup_err = DEDX_OK;
    float stp;

    *err = DEDX_OK;

    config = calloc(1, sizeof(dedx_config));
    if (config == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        return 0.0f;
    }

    config->ion = ion;
    config->target = target;
    config->program = DEDX_ICRU;

    ws = dedx_allocate_workspace(1, err);
    if (*err != DEDX_OK) {
        dedx_free_config(config, &cleanup_err);
        return 0.0f;
    }

    if (dedx_load_config(ws, config, err) != 0) {
        dedx_free_config(config, &cleanup_err);
        config = calloc(1, sizeof(dedx_config));
        if (config == NULL) {
            *err = DEDX_ERR_NO_MEMORY;
            dedx_free_workspace(ws, &cleanup_err);
            return 0.0f;
        }

        config->ion = ion;
        config->target = target;
        config->program = DEDX_DEFAULT;
        if (dedx_load_config(ws, config, err) != 0) {
            dedx_free_config(config, &cleanup_err);
            dedx_free_workspace(ws, &cleanup_err);
            return 0.0f;
        }
    }

    stp = dedx_get_stp(ws, config, energy, err);
    dedx_free_config(config, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);
    if (*err != DEDX_OK)
        return 0.0f;

    return stp;
}

static int load_data(dedx_workspace *ws, stopping_data *data, float *energy, dedx_config *config, int *err) {
    int active_dataset = ws->active_datasets;
    int prog = config->program;

    *err = DEDX_OK;

    if (active_dataset >= ws->datasets) {
        *err = DEDX_ERR_INVALID_DATASET_ID;
        return -1;
    }

    dedx_internal_calculate_coefficients(
        ws->loaded_data[active_dataset]->base, energy, data->data, data->length, config->interpolation_mode);

    ws->loaded_data[active_dataset]->acc.cache = 0;
    ws->loaded_data[active_dataset]->n = data->length;
    ws->loaded_data[active_dataset]->prog = prog;
    ws->loaded_data[active_dataset]->ion = data->ion;
    ws->loaded_data[active_dataset]->target = data->target;
    ws->loaded_data[active_dataset]->datapoints = data->length;
    ws->loaded_data[active_dataset]->interpolation_mode = config->interpolation_mode;

    ws->active_datasets++;
    return active_dataset;
}

static int check_energy_bounds(dedx_internal_lookup_data *data, float energy) {
    int length = data->datapoints;
    float low = data->base[0].x;
    float high = data->base[length - 1].x;

    if (energy < low || energy > high) {
        return DEDX_ERR_ENERGY_OUT_OF_RANGE;
    }
    return 0;
}

static float get_min_energy_icru(int ion) {
    float energy_min = 0;

    switch (ion) {
    case 1:
        energy_min = 0.001;
        break;
    case 2:
        energy_min = 0.001 / 4.0;
        break;
    default:
        energy_min = 0.025;
        break;
    }
    return energy_min;
}

static float get_max_energy_icru(int ion) {
    float energy_max = 0;

    switch (ion) {
    case 1:
        energy_max = 10000.0;
        break;
    case 2:
        energy_max = 1000.0 / 4.0;
        break;
    default:
        energy_max = 1000.0;
        break;
    }
    return energy_max;
}

static int check_ion(int prog, int ion) {
    const int *ion_list;
    int i = 0;

    if (prog >= DEDX_DEFAULT || prog == DEDX_AUTO) {
        if ((ion < 1) || (ion > 120))
            return 0;
        return 1;
    }

    ion_list = dedx_get_ion_list(prog);
    while (ion_list[i] != -1) {
        if (ion_list[i] == ion)
            return 1;
        ++i;
    }
    return 0;
}

static int load_config_clean(dedx_workspace *ws, dedx_config *config, int *err) {
    float energy[DEDX_MAX_ELEMENTS];
    int cfg;
    int prog = config->program;
    int ion = config->ion;
    int target = config->target;
    stopping_data data;

    config->bragg_used = 0;
    *err = DEDX_OK;

    if (!check_ion(prog, ion)) {
        *err = DEDX_ERR_ION_NOT_SUPPORTED;
        return -1;
    }
    config->_temp_i_value = config->i_value;
    find_data(&data, config, energy, err);

    if (*err != 0) {
        if (*err == DEDX_ERR_COMBINATION_NOT_FOUND && target > 99) {
            *err = DEDX_OK;
            dedx_internal_evaluate_compound(config, err);
            if (*err != 0)
                return -1;
            if (config->elements_length == 0) {
                printf("error \n");
                *err = DEDX_ERR_TARGET_NOT_FOUND;
                return -1;
            }
            cfg = load_compound(ws, config, err);
            if (*err != 0)
                return -1;
        }
        if (*err != 0)
            return -1;
    } else {
        cfg = load_data(ws, &data, energy, config, err);
    }
    return cfg;
}

static int find_data(stopping_data *data, dedx_config *config, float *energy, int *err) {
    int prog = config->program;
    int target = config->target;
    int ion = config->ion;
    int prog_load = prog;
    int ion_load = ion;
    int target_load = target;

    *err = DEDX_OK;

    if (prog == DEDX_ICRU) {
    } else if (prog == DEDX_ICRU49) {
    } else if (prog == DEDX_AUTO) {
        /* Resolve exactly like DEDX_ICRU (its tabulated sub-programs by ion type);
         * the only difference is what happens below if that resolution fails. */
        prog_load = DEDX_ICRU;
    } else if (prog == DEDX_ESTAR) {
        *err = DEDX_ERR_ESTAR_NOT_IMPL;
        return -1;
    } else if (prog == DEDX_MSTAR && ion > 1) {
        ion_load = 2;
    } else if (prog == DEDX_BETHE_EXT00 || prog == DEDX_DEFAULT) {
        int prog_temp = config->program;

        prog_load = DEDX_BETHE_EXT00;
        dedx_internal_read_energy_data(energy, prog_load, err);
        config->program = prog_load;
        load_bethe_2(data, config, energy, err);
        config->program = prog_temp;
        return 0;
    }

    if (dedx_embedded_resolve_program(prog_load, ion_load, target_load, &prog_load) != 0) {
        /* DEDX_AUTO layers a Bethe-Bloch fallback on top of DEDX_ICRU's tabulated
         * auto-selection: when no tabulated report covers this element (e.g. Boron,
         * which none of them tabulate), compute it analytically instead of failing
         * outright. Compounds (target > 99) are excluded here: they are handled below
         * via Bragg-additivity decomposition into elements, each of which goes through
         * this same fallback individually. DEDX_ICRU itself, and the report-specific
         * programs (PSTAR, ASTAR, ICRU49, ICRU73, ...), intentionally stay literal to
         * their published data and never get this fallback -- see DEDX_AUTO's docs
         * in dedx.h for why. */
        if (prog == DEDX_AUTO && target_load > 0 && target_load <= 99) {
            return load_bethe_fallback(data, config, energy, err);
        }
        if (prog == DEDX_ICRU || prog == DEDX_ICRU49 || prog == DEDX_ICRU73 || prog == DEDX_AUTO) {
            *err = DEDX_ERR_COMBINATION_NOT_FOUND;
            return -1;
        }
    }
    dedx_internal_read_binary_data(data, prog_load, ion_load, target_load, err);
    if (*err != 0)
        return -1;

    dedx_internal_read_energy_data(energy, prog_load, err);
    if (prog == DEDX_MSTAR) {
        stopping_data out;
        char mode = 'b';

        if (config->mstar_mode != '\0') {
            mode = config->mstar_mode;
        }
        data->ion = config->ion;
        dedx_internal_convert_energy_to_mstar(data, &out, mode, config, energy, err);
        if (*err != 0)
            return -1;
        memcpy(data, &out, sizeof(stopping_data));
        data->ion = ion;
    }
    return 0;
}

static int load_compound(dedx_workspace *ws, dedx_config *config, int *err) {
    int i = 0;
    int j = 0;
    int length = config->elements_length;
    int *targets = config->elements_id;
    float *weight;
    float energy[DEDX_MAX_ELEMENTS];
    float i_value;
    int target;
    stopping_data data;
    stopping_data *compound_data = malloc(sizeof(stopping_data) * length);

    *err = DEDX_OK;

    if (compound_data == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        return -1;
    }
    weight = config->elements_mass_fraction;
    i_value = config->i_value;
    target = config->target;
    for (i = 0; i < length; i++) {
        config->target = targets[i];
        // For custom compounds (target > 99 or target == 0), the elements_i_value
        // array might be provided explicitly or omitted entirely.
        if (config->elements_i_value != NULL) {
            config->_temp_i_value = config->elements_i_value[i];
            // If the explicit I-value for this element is < 0.0, it is an error.
            // If it is exactly 0.0, we fallback to the default I-value for the element.
            if (config->elements_i_value[i] < 0.0) {
                *err = DEDX_ERR_INVALID_I_VALUE;
                free(compound_data);
                return -1;
            }
            if (config->elements_i_value[i] == 0.0) {
                config->_temp_i_value = dedx_get_i_value(targets[i], err);
                if (*err != 0) {
                    free(compound_data);
                    return -1;
                }
            }
        } else {
            // If the elements_i_value array was not provided at all, we must initialize
            // the _temp_i_value for the Bethe algorithm using the default I-value for the element.
            config->_temp_i_value = dedx_get_i_value(targets[i], err);
            if (*err != 0) {
                free(compound_data);
                return -1;
            }
        }
        find_data(&compound_data[i], config, energy, err);
        if (*err != 0) {
            free(compound_data);
            return -1;
        }
    }

    config->i_value = i_value;
    config->target = target;
    for (j = 0; j < compound_data[0].length; j++) {
        data.data[j] = 0.0;
        for (i = 0; i < length; i++) {
            data.data[j] += weight[i] * compound_data[i].data[j];
        }
    }
    data.length = compound_data[0].length;
    free(compound_data);
    return load_data(ws, &data, energy, config, err);
}

static int load_bethe_2(stopping_data *data, dedx_config *config, float *energy, int *err) {
    int i = 0;
    float PZ, PA, TZ, TA, rho, pot;

    *err = DEDX_OK;
    if (config->target > 99) {
        *err = DEDX_ERR_COMBINATION_NOT_FOUND;
        return -1;
    }

    PZ = dedx_internal_get_atom_charge(config->ion, err);
    PA = dedx_internal_get_atom_mass(config->ion, err);
    TZ = dedx_internal_get_atom_charge(config->target, err);
    TA = dedx_internal_get_atom_mass(config->target, err);
    rho = config->rho;
    pot = config->_temp_i_value;
    data->length = 122;
    dedx_internal_read_energy_data(energy, DEDX_BETHE_EXT00, err);
    if (*err != 0)
        return -1;

    dedx_internal_bethe_workspace *bethe =
        (dedx_internal_bethe_workspace *) calloc(1, sizeof(dedx_internal_bethe_workspace));
    for (i = 0; i < data->length; i++) {
        data->data[i] = dedx_internal_calculate_bethe_energy(bethe, energy[i], PZ, PA, TZ, TA, rho, pot);
    }
    if (bethe->bet != NULL)
        free(bethe->bet);
    if (bethe->gold)
        free(bethe->gold);
    free(bethe);
    return 0;
}

/* Last-resort tier for DEDX_AUTO: compute via the Bethe-Bloch formula using the
 * element's embedded density and mean excitation potential. config->rho is already
 * populated by dedx_internal_validate_rho(), which runs unconditionally before
 * find_data(). config->_temp_i_value, however, is not: DEDX_AUTO is deliberately
 * grouped with the tabulated report identifiers (< DEDX_DEFAULT), so
 * dedx_internal_validate_config() skips dedx_internal_evaluate_i_pot() for it (that
 * step only runs for program >= 100). For a compound element reached via
 * load_compound(), _temp_i_value is already set per-element there; for a direct
 * elemental target, it still defaults to 0 and must be fetched here.
 *
 * Returns 0 on success, with `data` filled in with the computed Bethe-Bloch
 * stopping-power values and *err set to DEDX_OK, or -1 on failure, with *err set to
 * the error code -- the same success/failure convention as find_data()/load_bethe_2(),
 * which this function is a thin wrapper around. */
static int load_bethe_fallback(stopping_data *data, dedx_config *config, float *energy, int *err) {
    int prog_temp = config->program;
    int state;

    *err = DEDX_OK;

    if (config->_temp_i_value == 0.0f) {
        state = config->compound_state;
        if (state == DEDX_DEFAULT_STATE) {
            state = dedx_internal_target_is_gas(config->target, err) ? DEDX_GAS : DEDX_CONDENSED;
        }
        config->_temp_i_value = dedx_internal_get_i_value(config->target, state, err);
        if (*err != DEDX_OK) { /* LCOV_EXCL_START -- every element in the valid 1-99
                                   range that reaches this function has embedded I-value data */
            return -1;
        } /* LCOV_EXCL_STOP */
    }

    config->program = DEDX_BETHE_EXT00;
    load_bethe_2(data, config, energy, err);
    config->program = prog_temp;

    return (*err == DEDX_OK) ? 0 : -1;
}
