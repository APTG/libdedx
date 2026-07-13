#include <dedx.h>
#include <dedx_tools.h>
#include <stdio.h>
#include <stdlib.h>

static int check_err(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got err=%d, expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

int main(void) {
    int failures = 0;
    int err = 0;
    dedx_workspace *ws;
    dedx_config *cfg;

    /* dedx_get_csda requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    /* ion_a intentionally left at 0 */
    err = 0;
    dedx_get_csda(ws, cfg, 1.0f, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "csda without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_inverse_csda requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    err = 0;
    dedx_get_inverse_csda(ws, cfg, 10.0f, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "inverse_csda without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_inverse_stp requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    err = 0;
    dedx_get_inverse_stp(ws, cfg, 100.0f, -1, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "inverse_stp without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* BETHE with target=0 and no rho: density is required */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_DEFAULT;
    cfg->ion = DEDX_PROTON;
    cfg->target = 0;
    /* rho intentionally left at 0 */
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_RHO_REQUIRED, "BETHE custom target without rho");
    failures += check_err(cfg->loaded, 0, "failed load should not mark config loaded");
    failures += check_err(cfg->cfg_id, -1, "failed load should not assign cfg_id");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_stp must reject invalid dataset ids before dereferencing */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->cfg_id = 0;
    err = 0;
    dedx_get_stp(ws, cfg, 1.0f, &err);
    failures += check_err(err, DEDX_ERR_INVALID_DATASET_ID, "stp with invalid dataset id");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* invalid interpolation mode must be rejected during config validation */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    cfg->interpolation_mode = 99;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_INVALID_INTERPOLATION_MODE, "invalid interpolation mode");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* BETHE custom target with invalid element id for default I-value fallback (with elements_i_value <= 0) */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = DEDX_PROTON;
    cfg->target = 0;
    cfg->rho = 1.0f;
    cfg->elements_id = calloc(1, sizeof(int));
    cfg->elements_id[0] = 999; /* invalid element */
    cfg->elements_atoms = calloc(1, sizeof(int));
    cfg->elements_atoms[0] = 1;
    cfg->elements_i_value = calloc(1, sizeof(float));
    cfg->elements_i_value[0] = 0.0f; /* force fallback */
    cfg->elements_length = 1;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_NOT_AN_ELEMENT, "invalid element for i-value fallback");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* BETHE custom target with invalid element id for default I-value fallback (elements_i_value is NULL) */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = DEDX_PROTON;
    cfg->target = 0;
    cfg->rho = 1.0f;
    cfg->elements_id = calloc(1, sizeof(int));
    cfg->elements_id[0] = 999; /* invalid element */
    cfg->elements_atoms = calloc(1, sizeof(int));
    cfg->elements_atoms[0] = 1;
    cfg->elements_length = 1;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_NOT_AN_ELEMENT, "invalid element for i-value fallback (NULL array)");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* DEDX_DEFAULT/DEDX_BETHE_EXT00 only support ions the periodic table covers
     * (1-112); an out-of-range ion must fail cleanly, not silently. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_DEFAULT;
    cfg->ion = 115; /* beyond dedx_periodic_table's 1-112 range */
    cfg->target = DEDX_WATER;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    /* Regression test: DEDX_DEFAULT/DEDX_BETHE_EXT00 with a compound target
     * decomposes into elements (dedx_internal_evaluate_compound(), called eagerly for
     * program >= 100) before check_ion() would ever run, so an invalid ion has to be
     * caught inside the Bethe evaluation itself. It previously wasn't: load_bethe_2()
     * called dedx_internal_get_atom_charge()/get_atom_mass() for the ion and then
     * again for the (valid) target element, and the target's success silently
     * overwrote the ion's DEDX_ERR_NOT_AN_ELEMENT, yielding a loaded config with a NaN
     * stopping power instead of a load failure. */
    failures += check_err(err, DEDX_ERR_NOT_AN_ELEMENT, "DEFAULT with invalid ion and compound target");
    failures += check_err(cfg->loaded, 0, "failed load should not mark config loaded");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* Same invalid ion, but with a plain elemental target: this path goes through
     * load_config_clean()'s check_ion() instead, which independently must also reject
     * ions the periodic table doesn't cover for DEDX_DEFAULT/DEDX_BETHE_EXT00/DEDX_AUTO. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_DEFAULT;
    cfg->ion = 115;
    cfg->target = DEDX_CARBON;
    cfg->rho = 1.0f;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_ION_NOT_SUPPORTED, "DEFAULT with invalid ion and elemental target");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}
