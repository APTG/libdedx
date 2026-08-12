#ifndef DEDX_VALIDATE_H
#define DEDX_VALIDATE_H
#include "dedx.h"

int dedx_internal_set_names(dedx_config *config, int *err);
int dedx_internal_validate_state(dedx_config *config, int *err);
int dedx_internal_validate_config(dedx_config *config, int *err);
int dedx_internal_evaluate_i_pot(dedx_config *config, int *err);
int dedx_internal_validate_rho(dedx_config *config, int *err);
int dedx_internal_evaluate_compound(dedx_config *config, int *err);
int dedx_internal_calculate_element_i_pot(dedx_config *config, int *err);

/** @brief Whether `ion` is a valid projectile for `prog`.
 *
 *  For DEDX_DEFAULT/DEDX_BETHE_EXT00/DEDX_AUTO this is the 1-112 range the periodic
 *  table backing the Bethe-Bloch evaluation covers (dedx_internal_get_atom_charge()/
 *  get_atom_mass() enforce the same bound); for every other program it's membership
 *  in dedx_get_ion_list(prog). Shared between dedx.c (dedx_get_material_list_for_ion())
 *  and dedx_internal_validate_config() below, which is the sole place
 *  dedx_load_config() enforces it -- previously only load_config_clean() did, so a
 *  custom compound (which dispatches straight to load_compound()) skipped it and
 *  could fail with a confusing DEDX_ERR_COMBINATION_NOT_FOUND instead of
 *  DEDX_ERR_ION_NOT_SUPPORTED for an unsupported ion (see issue #149 finding A8). */
int dedx_internal_check_ion(int prog, int ion);

#endif // DEDX_VALIDATE_H
