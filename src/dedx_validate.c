#include "dedx_validate.h"

#include <math.h>
#include <stdlib.h>

#include "dedx_data_access.h"
#include "dedx_periodic_table.h"

int dedx_internal_set_names(dedx_config *config, int *err) {
    if (config->target != 0)
        config->target_name = dedx_get_material_name(config->target);

    config->ion_name = dedx_get_ion_name(config->ion);
    config->program_name = dedx_get_program_name(config->program);
    *err = DEDX_OK;
    return 0;
}

int dedx_internal_validate_rho(dedx_config *config, int *err) {
    if (config->rho <= 0.0 && config->target != 0) {
        int density_err = DEDX_OK;
        float density = dedx_internal_read_density(config->target, &density_err);

        if (density_err == DEDX_OK) {
            config->rho = density;
        } else if (config->program >= DEDX_DEFAULT) {
            /* Only programs that evaluate the Bethe-Bloch formula directly on this
             * exact target (program >= DEDX_DEFAULT) definitely need rho here.
             * Tabulated report lookups (PSTAR, ICRU*, MSTAR, ...) never read
             * config->rho at all, and DEDX_AUTO only needs it if it falls through to
             * the Bethe tier for a specific ion/element -- which load_bethe_2() now
             * checks itself right before it would otherwise divide/log by a
             * rho-derived term (see there), rather than speculatively failing every
             * combination up front just because the embedded density table happens
             * to be missing a row for this target. That used to block 407
             * tabulated-program combinations for exactly one such gap (FERROUSOXIDE,
             * id 159) -- see issue #149 finding A5. */
            *err = density_err;
        }
        /* else: a tabulated program (or DEDX_AUTO, conditionally) with a target that
         * has no embedded density row. config->rho stays <= 0.0; that's fine unless
         * something downstream actually needs it, which is checked there instead. */
    } else if (config->rho <= 0.0 && config->target == 0 && config->program >= 100) {
        *err = DEDX_ERR_RHO_REQUIRED;
    }
    return 0;
}

static int dedx_internal_validate_interpolation_mode(dedx_config *config, int *err) {
    if (config->interpolation_mode != DEDX_INTERPOLATION_LOG_LOG
        && config->interpolation_mode != DEDX_INTERPOLATION_LINEAR) {
        *err = DEDX_ERR_INVALID_INTERPOLATION_MODE;
        return -1;
    }
    return 0;
}

int dedx_internal_check_ion(int prog, int ion) {
    const int *ion_list;
    const int *programs;
    int prog_known = 0;
    int i = 0;

    if (prog >= DEDX_DEFAULT || prog == DEDX_AUTO) {
        /* The Bethe evaluation path (dedx_internal_get_atom_mass/charge, via
         * dedx_periodic_table) only has data for ions 1-112, matching
         * dedx_full_ion_list -- the same list dedx_get_ion_list() returns for these
         * programs -- so accept nothing wider than that here. */
        if ((ion < 1) || (ion > 112))
            return 0;
        return 1;
    }

    /* prog must be a recognized program id before it's safe to hand to
     * dedx_get_ion_list(): that indexes dedx_program_available_ions[] directly by
     * prog, and rows the table doesn't explicitly initialize are zero-filled, not
     * -1-terminated, so an unrecognized id (a typo, a stale constant, prog < 0) makes
     * the ion_list loop below walk off the end of the table looking for a terminator
     * that row will never have -- a real, ASan-reproducible global-buffer-overflow.
     * This was already latent on the elemental-target path before this PR (see issue
     * #149 finding B3, deferred to Phase 3 for the full accessor-hardening sweep), but
     * moving the check_ion() call here made it newly reachable from the
     * custom-compound path too (finding A8) -- that path used to bypass this function
     * entirely. Guard it here rather than let A8 grow the crash's reach. */
    programs = dedx_get_program_list();
    for (i = 0; programs[i] != -1; i++) {
        if (programs[i] == prog) {
            prog_known = 1;
            break;
        }
    }
    if (!prog_known)
        return 0;

    ion_list = dedx_get_ion_list(prog);
    i = 0;
    while (ion_list[i] != -1) {
        if (ion_list[i] == ion)
            return 1;
        ++i;
    }
    return 0;
}

/* config->mstar_mode is only ever read on the MSTAR path (dedx_internal_convert_
 * energy_to_mstar(), via find_data()), so a garbage value left in it is otherwise
 * inert for every other program -- don't reject a config over a field it will never
 * use. On the MSTAR path itself, an unrecognized mode used to fall through
 * dedx_internal_calculate_mspaul_coef()'s cascading if/else chain into a silent
 * "illegal mode" branch that left the computed coefficient at 0, so the config loaded
 * successfully but every stopping-power query came back as 0 with err == DEDX_OK
 * (see issue #149 finding A7). '\0' (the calloc-zeroed default) means "use
 * DEDX_MSTAR_MODE_DEFAULT" and is valid; anything else must be one of the six
 * documented DEDX_MSTAR_MODE_* letters. */
static int dedx_internal_validate_mstar_mode(dedx_config *config, int *err) {
    char mode = config->mstar_mode;

    if (config->program != DEDX_MSTAR) {
        return 0;
    }
    if (mode == '\0' || mode == DEDX_MSTAR_MODE_A || mode == DEDX_MSTAR_MODE_B || mode == DEDX_MSTAR_MODE_G
        || mode == DEDX_MSTAR_MODE_H || mode == DEDX_MSTAR_MODE_C || mode == DEDX_MSTAR_MODE_D) {
        return 0;
    }
    *err = DEDX_ERR_INVALID_MSTAR_MODE;
    return -1;
}

int dedx_internal_evaluate_i_pot(dedx_config *config, int *err) {
    unsigned int i;

    if (config->elements_i_value == NULL && config->target != 0) {
        if (config->i_value == 0.0) {
            config->i_value = dedx_internal_get_i_value(config->target, config->compound_state, err);
        }
        if (*err != 0)
            return -1;
    } else if (config->i_value == 0.0 && config->target == 0 && config->elements_i_value == NULL) {
        if (config->elements_length == 0) {
            *err = DEDX_ERR_TARGET_NOT_FOUND;
            return -1;
        }
        config->elements_i_value = calloc(config->elements_length, sizeof(float));
        if (config->elements_i_value == NULL) {
            *err = DEDX_ERR_NO_MEMORY;
            return -1;
        }
        for (i = 0; i < config->elements_length; i++) {
            config->elements_i_value[i] =
                dedx_internal_get_i_value(config->elements_id[i], config->compound_state, err);
            if (*err != 0)
                return -1;
        }
    }
    if (config->elements_id != NULL && config->elements_i_value == NULL) {
        dedx_internal_calculate_element_i_pot(config, err);
        if (*err != 0)
            return -1;
    }
    if (config->elements_i_value != NULL && config->i_value == 0.0) {
        if (config->elements_id == NULL || config->elements_mass_fraction == NULL) {
            *err = DEDX_ERR_INCONSISTENT_COMPOUND;
            return -1;
        }
        float charge_avg = 0.0;
        for (i = 0; i < config->elements_length; i++) {
            config->i_value += config->elements_mass_fraction[i] * log(config->elements_i_value[i])
                               * config->elements_id[i] / dedx_internal_get_atom_mass(config->elements_id[i], err);
            charge_avg += config->elements_mass_fraction[i] * config->elements_id[i]
                          / dedx_internal_get_atom_mass(config->elements_id[i], err);
            if (*err != 0)
                return -1;
        }
        config->i_value = exp(config->i_value / charge_avg);
    }
    return 0;
}

int dedx_internal_evaluate_compound(dedx_config *config, int *err) {
    unsigned int i = 0;

    if (config->target > 0 && config->target <= DEDX_MAX_ELEMENT_ID) {
        *err = DEDX_OK;
        return 0;
    }

    config->bragg_used = 1;
    if (config->elements_id == NULL) {
        unsigned int compos_len;
        float composition[20][2];

        dedx_internal_get_composition(config->target, composition, &compos_len, err);
        if (*err != 0) {
            return -1;
        }
        if (compos_len == 0) { /* LCOV_EXCL_START */
            *err = DEDX_ERR_TARGET_NOT_FOUND;
            return -1;
        } /* LCOV_EXCL_STOP */

        config->elements_id = (int *) malloc(sizeof(int) * compos_len);
        config->elements_mass_fraction = (float *) malloc(sizeof(float) * compos_len);
        if (config->elements_id == NULL || config->elements_mass_fraction == NULL) {
            free(config->elements_id);
            free(config->elements_mass_fraction);
            config->elements_id = NULL;
            config->elements_mass_fraction = NULL;
            *err = DEDX_ERR_NO_MEMORY;
            return -1;
        }

        for (i = 0; i < compos_len; i++) {
            config->elements_id[i] = (int) composition[i][0];
            config->elements_mass_fraction[i] = composition[i][1];
        }
        config->elements_length = compos_len;
    } else if (config->elements_mass_fraction == NULL && config->elements_atoms != NULL) {
        unsigned int length = config->elements_length;
        int *atoms_per_element = config->elements_atoms;
        float *density = malloc(sizeof(float) * length);
        float *weight = malloc(sizeof(float) * length);
        float f, sum = 0;

        if (length == 0) {
            free(density);
            free(weight);
            *err = DEDX_ERR_TARGET_NOT_FOUND;
            return -1;
        }
        if (density == NULL || weight == NULL) {
            free(density);
            free(weight);
            *err = DEDX_ERR_NO_MEMORY;
            return -1;
        }
        for (i = 0; i < length; i++) {
            f = dedx_internal_get_atom_mass(config->elements_id[i], err);
            if (*err != 0) {
                free(density);
                free(weight);
                return -1;
            }
            density[i] = f;
            sum += atoms_per_element[i] * f;
        }

        for (i = 0; i < length; i++) {
            weight[i] = atoms_per_element[i] * density[i] / sum;
        }
        free(density);
        config->elements_mass_fraction = weight;
    } else if (config->elements_mass_fraction != NULL) {
        /* Mass fractions are already available, so there is nothing to derive.
         * This covers two cases:
         *   1. A caller-provided compound described directly via elements_id +
         *      elements_mass_fraction (elements_atoms == NULL), which the public
         *      API documents as an alternative to elements_atoms (see dedx.h).
         *   2. The second of two calls for analytical programs (program >= 100)
         *      with a custom compound (target == 0): dedx_internal_validate_config()
         *      invokes this function once to prepare I-potentials and again in the
         *      generic custom-compound path; the first call already populated the
         *      mass fractions.
         * Either way, return success without touching the existing array. A compound
         * still needs at least one element, so reject an empty list. */
        if (config->elements_length == 0) {
            *err = DEDX_ERR_TARGET_NOT_FOUND;
            return -1;
        }
        *err = DEDX_OK;
        return 0;
    } else {
        *err = DEDX_ERR_INCONSISTENT_COMPOUND;
        return -1;
    }
    return 0;
}

int dedx_internal_validate_config(dedx_config *config, int *err) {
    /* Start from a clean slate: the helper validators below only set *err on
     * failure, so a stale non-zero value left in *err by the caller would
     * otherwise be mistaken for a validation error. */
    *err = DEDX_OK;

    if (!dedx_internal_check_ion(config->program, config->ion)) {
        *err = DEDX_ERR_ION_NOT_SUPPORTED;
        return -1;
    }

    dedx_internal_validate_mstar_mode(config, err);
    if (*err != 0) {
        return -1;
    }

    dedx_internal_validate_interpolation_mode(config, err);
    if (*err != 0) {
        return -1;
    }

    dedx_internal_validate_rho(config, err);
    if (*err != 0) {
        return -1;
    }

    if (config->program >= 100) {
        /* For analytical programs the compound must be evaluated first so that
         * elements_mass_fraction is available before dedx_internal_evaluate_i_pot
         * computes the Bragg-averaged mean excitation energy. The three calls
         * below must remain in this order. */
        dedx_internal_evaluate_compound(config, err);
        if (*err != 0)
            return -1;
        dedx_internal_validate_state(config, err);
        if (*err != 0)
            return -1;
        dedx_internal_evaluate_i_pot(config, err);
        if (*err != 0)
            return -1;
    }

    /* Ensure tabulated programs also resolve custom compounds (target == 0).
     * When program >= 100 this is a second call; dedx_internal_evaluate_compound
     * detects the already-populated elements_mass_fraction and returns early. */
    if (config->target == 0 && config->elements_id != NULL) {
        dedx_internal_evaluate_compound(config, err);
        if (*err != 0)
            return -1;
    }

    /* load_compound() sums weight[i] * compound_data[i].data[j] over
     * elements_mass_fraction whenever elements_id is set, regardless of program or
     * target. Neither block above runs for a tabulated program (program < 100) with
     * a non-zero target, so a caller-supplied elements_id without elements_mass_fraction
     * (and nothing to derive it from) would otherwise reach that sum with a NULL
     * weight array. Reject it here instead of dereferencing NULL later. */
    if (config->elements_id != NULL && config->elements_mass_fraction == NULL) {
        *err = DEDX_ERR_INCONSISTENT_COMPOUND;
        return -1;
    }
    return 0;
}

int dedx_internal_validate_state(dedx_config *config, int *err) {
    if (config->compound_state == DEDX_DEFAULT_STATE) {
        if (dedx_internal_target_is_gas(config->target, err)) {
            config->compound_state = DEDX_GAS;
        } else {
            config->compound_state = DEDX_CONDENSED;
        }
    }
    return 0;
}

int dedx_internal_calculate_element_i_pot(dedx_config *config, int *err) {
    unsigned int i;
    float charge_avg = 0;
    float avg_pot = 0;
    float log_x, i_pot_x;
    int target;

    if (config->elements_length == 0) {
        *err = DEDX_ERR_TARGET_NOT_FOUND;
        return -1;
    }

    for (i = 0; i < config->elements_length; i++) {
        target = config->elements_id[i];
        charge_avg += config->elements_mass_fraction[i] * target / dedx_internal_get_atom_mass(target, err);
        avg_pot += config->elements_mass_fraction[i] * target / dedx_internal_get_atom_mass(target, err)
                   * log(dedx_internal_get_i_value(target, config->compound_state, err));
        if (*err != 0)
            return -1;
    }

    log_x = log(config->i_value);
    log_x -= avg_pot / charge_avg;
    i_pot_x = exp(log_x);
    config->elements_i_value = (float *) malloc(sizeof(float) * config->elements_length);
    if (config->elements_i_value == NULL) {
        *err = DEDX_ERR_NO_MEMORY;
        return -1;
    }

    for (i = 0; i < config->elements_length; i++) {
        config->elements_i_value[i] =
            dedx_internal_get_i_value(config->elements_id[i], config->compound_state, err) * i_pot_x;
        if (*err != 0)
            return -1;
    }

    return 0;
}
