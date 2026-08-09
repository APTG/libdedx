#include <dedx.h>
#include <dedx_tools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check_err(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got err=%d, expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

/* dedx_get_error_code() must resolve every DEDX_ERR_* code to a real message, not
 * silently fall back to "No such error code." -- that is exactly how
 * DEDX_ERR_INCONSISTENT_COMPOUND (211) went unnoticed before. There is no way to
 * enumerate C preprocessor macros at runtime, so this list has to be kept in sync
 * with dedx_error.h by hand; that is the point -- adding a new DEDX_ERR_* code
 * without extending this test (and dedx_get_error_code()'s table) now fails CI
 * instead of failing silently in the field. */
static int check_has_message(int code, const char *label) {
    char err_str[DEDX_ERROR_STRING_MAX];

    dedx_get_error_code(err_str, code);
    if (strcmp(err_str, "No such error code.") == 0) {
        fprintf(stderr, "FAIL %s: dedx_get_error_code(%d) has no message\n", label, code);
        return 1;
    }
    return 0;
}

static int test_error_code_strings_complete(void) {
    int failures = 0;

    failures += check_has_message(DEDX_OK, "DEDX_OK");
    failures += check_has_message(DEDX_ERR_NO_COMPOS_FILE, "DEDX_ERR_NO_COMPOS_FILE");
    failures += check_has_message(DEDX_ERR_NO_GAS_FILE, "DEDX_ERR_NO_GAS_FILE");
    failures += check_has_message(DEDX_ERR_NO_CHARGE_FILE, "DEDX_ERR_NO_CHARGE_FILE");
    failures += check_has_message(DEDX_ERR_NO_BINARY_DATA, "DEDX_ERR_NO_BINARY_DATA");
    failures += check_has_message(DEDX_ERR_NO_BINARY_ENERGY, "DEDX_ERR_NO_BINARY_ENERGY");
    failures += check_has_message(DEDX_ERR_WRITE_FAILED, "DEDX_ERR_WRITE_FAILED");
    failures += check_has_message(DEDX_ERR_NO_ENERGY_FILE, "DEDX_ERR_NO_ENERGY_FILE");
    failures += check_has_message(DEDX_ERR_NO_DATA_FILE, "DEDX_ERR_NO_DATA_FILE");
    failures += check_has_message(DEDX_ERR_NO_NAMES_FILE, "DEDX_ERR_NO_NAMES_FILE");
    failures += check_has_message(DEDX_ERR_NO_COMPOSITION, "DEDX_ERR_NO_COMPOSITION");
    failures += check_has_message(DEDX_ERR_ENERGY_OUT_OF_RANGE, "DEDX_ERR_ENERGY_OUT_OF_RANGE");
    failures += check_has_message(DEDX_ERR_TARGET_NOT_FOUND, "DEDX_ERR_TARGET_NOT_FOUND");
    failures += check_has_message(DEDX_ERR_COMBINATION_NOT_FOUND, "DEDX_ERR_COMBINATION_NOT_FOUND");
    failures += check_has_message(DEDX_ERR_INVALID_DATASET_ID, "DEDX_ERR_INVALID_DATASET_ID");
    failures += check_has_message(DEDX_ERR_NOT_AN_ELEMENT, "DEDX_ERR_NOT_AN_ELEMENT");
    failures += check_has_message(DEDX_ERR_ESTAR_NOT_IMPL, "DEDX_ERR_ESTAR_NOT_IMPL");
    failures += check_has_message(DEDX_ERR_ION_NOT_SUPPORTED_MSTAR, "DEDX_ERR_ION_NOT_SUPPORTED_MSTAR");
    failures += check_has_message(DEDX_ERR_ION_NOT_SUPPORTED, "DEDX_ERR_ION_NOT_SUPPORTED");
    failures += check_has_message(DEDX_ERR_RHO_REQUIRED, "DEDX_ERR_RHO_REQUIRED");
    failures += check_has_message(DEDX_ERR_ION_A_REQUIRED, "DEDX_ERR_ION_A_REQUIRED");
    failures += check_has_message(DEDX_ERR_INVALID_I_VALUE, "DEDX_ERR_INVALID_I_VALUE");
    failures += check_has_message(DEDX_ERR_INCONSISTENT_COMPOUND, "DEDX_ERR_INCONSISTENT_COMPOUND");
    failures += check_has_message(DEDX_ERR_INVALID_INTERPOLATION_MODE, "DEDX_ERR_INVALID_INTERPOLATION_MODE");
    failures += check_has_message(DEDX_ERR_NO_MEMORY, "DEDX_ERR_NO_MEMORY");

    /* An unrecognised code must still fall back to the default message rather than,
     * say, leaving the buffer untouched or crashing. */
    {
        char err_str[DEDX_ERROR_STRING_MAX];
        dedx_get_error_code(err_str, 99999);
        if (strcmp(err_str, "No such error code.") != 0) {
            fprintf(stderr, "FAIL unknown error code should report 'No such error code.', got \"%s\"\n", err_str);
            failures++;
        }
    }

    return failures;
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
    /* Regression test, updated for issue #149 finding A8: DEDX_DEFAULT/DEDX_BETHE_EXT00
     * with a compound target decomposes into elements (dedx_internal_evaluate_compound(),
     * called eagerly for program >= 100) -- this used to happen before check_ion() would
     * ever run, so an invalid ion was only ever caught deep inside the Bethe evaluation
     * itself, as DEDX_ERR_NOT_AN_ELEMENT (a NULL dereference away from load_bethe_2()'s
     * atom-charge/mass lookups silently overwriting the ion's own failure with the
     * target's success, and yielding a loaded config with a NaN stopping power instead
     * of a load failure). dedx_internal_validate_config() now calls
     * dedx_internal_check_ion() unconditionally, before dedx_internal_evaluate_compound()
     * ever runs, so this is now caught immediately and uniformly as
     * DEDX_ERR_ION_NOT_SUPPORTED -- the same code the elemental-target case below already
     * expects. */
    failures += check_err(err, DEDX_ERR_ION_NOT_SUPPORTED, "DEFAULT with invalid ion and compound target");
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

    /* Regression test for a NULL dereference: a tabulated program (< 100) with a
     * real target and a caller-supplied elements_id but no elements_mass_fraction
     * (and no elements_atoms to derive it from) used to sail through
     * dedx_internal_validate_config() untouched -- neither the program >= 100 branch
     * nor the target == 0 branch runs for this combination -- and then crash inside
     * load_compound(), which sums weight[i] * compound_data[i].data[j] with
     * weight == NULL. It must now fail cleanly instead. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    /* elements_atoms and elements_mass_fraction intentionally left NULL. */
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_INCONSISTENT_COMPOUND, "elements_id without weights must be rejected");
    failures += check_err(cfg->loaded, 0, "failed load should not mark config loaded");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* Regression test for issue #149 finding A8's own reproducer: dedx_load_config()
     * used to dispatch straight to load_compound() whenever elements_id != NULL,
     * bypassing check_ion() entirely (only load_config_clean()'s tabulated-program
     * path called it). A custom compound with an ion a tabulated, report-specific
     * program like DEDX_PSTAR (proton-only) does not support therefore failed deep
     * inside the Bragg decomposition with DEDX_ERR_COMBINATION_NOT_FOUND instead of
     * the clear, immediate DEDX_ERR_ION_NOT_SUPPORTED an elemental target already got.
     * dedx_internal_validate_config() now calls dedx_internal_check_ion() up front for
     * every dedx_load_config() call, compound or not, so both paths agree. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR; /* proton-only */
    cfg->ion = DEDX_CARBON;    /* PSTAR does not support carbon */
    cfg->target = 0;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_atoms = calloc(2, sizeof(int));
    cfg->elements_atoms[0] = 2;
    cfg->elements_atoms[1] = 1;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_ION_NOT_SUPPORTED, "PSTAR+carbon+custom compound (A8)");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    failures += test_error_code_strings_complete();

    return failures;
}
