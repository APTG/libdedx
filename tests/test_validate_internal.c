#include <dedx.h>
#include <stdio.h>
#include <stdlib.h>

#include "dedx_error.h"
#include "dedx_validate.h"

static int check_int(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %d expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

static int check_true(int condition, const char *label) {
    if (!condition) {
        fprintf(stderr, "FAIL %s\n", label);
        return 1;
    }
    return 0;
}

static dedx_config *alloc_config(void) {
    return calloc(1, sizeof(dedx_config));
}

static int test_validate_state_gas(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = alloc_config();

    cfg->target = DEDX_AIR;
    cfg->compound_state = DEDX_DEFAULT_STATE;

    dedx_internal_validate_state(cfg, &err);
    failures += check_int(err, DEDX_OK, "validate_state gas err");
    failures += check_int(cfg->compound_state, DEDX_GAS, "validate_state gas branch");

    dedx_free_config(cfg, &err);
    return failures;
}

static int test_validate_config_custom_atoms(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = alloc_config();

    cfg->program = DEDX_PSTAR;
    cfg->target = 0;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_atoms = calloc(2, sizeof(int));

    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_atoms[0] = 2;
    cfg->elements_atoms[1] = 1;

    dedx_internal_validate_config(cfg, &err);
    failures += check_int(err, DEDX_OK, "validate_config custom atoms err");
    failures += check_true(cfg->bragg_used == 1, "validate_config should enable Bragg rule");
    failures += check_true(cfg->elements_mass_fraction != NULL, "validate_config should derive mass fractions");
    if (cfg->elements_mass_fraction != NULL) {
        failures += check_true(cfg->elements_mass_fraction[0] > 0.0f, "derived hydrogen mass fraction");
        failures += check_true(cfg->elements_mass_fraction[1] > cfg->elements_mass_fraction[0],
                               "oxygen mass fraction should dominate water-like mixture");
    }

    dedx_free_config(cfg, &err);
    return failures;
}

/*
 * Regression test for issue #104: a custom compound (target == 0) described by
 * an atom count and validated with an analytical program (program >= 100, here
 * DEDX_BETHE_EXT00) used to fail with DEDX_ERR_INCONSISTENT_COMPOUND. The
 * compound was evaluated twice and the second pass rejected the already-derived
 * mass fractions. Validation must now succeed and keep the derived fractions.
 */
static int test_validate_config_bethe_custom_atoms(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = alloc_config();

    cfg->program = DEDX_BETHE_EXT00;
    cfg->target = 0;
    cfg->rho = 1.0f; /* required for Bethe-type programs with a custom compound */
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_atoms = calloc(2, sizeof(int));

    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_atoms[0] = 2;
    cfg->elements_atoms[1] = 1;

    dedx_internal_validate_config(cfg, &err);
    failures += check_int(err, DEDX_OK, "bethe custom atoms should validate");
    failures += check_true(cfg->bragg_used == 1, "bethe custom atoms should enable Bragg rule");
    failures += check_true(cfg->elements_mass_fraction != NULL, "bethe custom atoms should derive mass fractions");
    if (cfg->elements_mass_fraction != NULL) {
        failures += check_true(cfg->elements_mass_fraction[0] > 0.0f, "derived hydrogen mass fraction");
        failures += check_true(cfg->elements_mass_fraction[1] > cfg->elements_mass_fraction[0],
                               "oxygen mass fraction should dominate water-like mixture");
    }
    /* The Bragg-averaged compound I-value must be derived from the elements. */
    failures += check_true(cfg->i_value > 0.0f, "bethe custom atoms should derive compound I value");

    dedx_free_config(cfg, &err);
    return failures;
}

/*
 * Companion to the test above covering the documented alternative of supplying
 * mass fractions directly (elements_id + elements_mass_fraction, no atom counts).
 * The broadened idempotency branch must accept this caller-provided compound for
 * analytical programs and leave the supplied fractions untouched.
 */
static int test_validate_config_bethe_mass_fraction(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = alloc_config();

    cfg->program = DEDX_BETHE_EXT00;
    cfg->target = 0;
    cfg->rho = 1.0f;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_mass_fraction = calloc(2, sizeof(float));

    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_mass_fraction[0] = 0.111894f;
    cfg->elements_mass_fraction[1] = 0.888106f;

    dedx_internal_validate_config(cfg, &err);
    failures += check_int(err, DEDX_OK, "bethe mass-fraction compound should validate");
    failures += check_true(cfg->elements_mass_fraction != NULL, "mass fractions should remain allocated");
    if (cfg->elements_mass_fraction != NULL) {
        /* Supplied fractions must be preserved exactly, not recomputed. */
        failures += check_true(cfg->elements_mass_fraction[0] == 0.111894f, "hydrogen mass fraction preserved");
        failures += check_true(cfg->elements_mass_fraction[1] == 0.888106f, "oxygen mass fraction preserved");
    }
    failures += check_true(cfg->i_value > 0.0f, "bethe mass-fraction compound should derive I value");

    dedx_free_config(cfg, &err);
    return failures;
}

/*
 * The validators only set *err on failure, so dedx_internal_validate_config must
 * reset it on entry. Otherwise a stale non-zero value supplied by the caller would
 * be mistaken for a validation error on the first `if (*err != 0)` check.
 */
static int test_validate_config_resets_stale_err(void) {
    int failures = 0;
    int err = 12345; /* stale value left over from an earlier call */
    dedx_config *cfg = alloc_config();

    cfg->program = DEDX_PSTAR;
    cfg->target = 0;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_atoms = calloc(2, sizeof(int));

    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_atoms[0] = 2;
    cfg->elements_atoms[1] = 1;

    dedx_internal_validate_config(cfg, &err);
    failures += check_int(err, DEDX_OK, "stale err should be reset before validation");

    dedx_free_config(cfg, &err);
    return failures;
}

static int test_evaluate_i_pot_custom_elements(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = alloc_config();

    cfg->target = 0;
    cfg->compound_state = DEDX_CONDENSED;
    cfg->elements_length = 2;
    cfg->elements_id = calloc(2, sizeof(int));
    cfg->elements_mass_fraction = calloc(2, sizeof(float));

    cfg->elements_id[0] = DEDX_HYDROGEN;
    cfg->elements_id[1] = DEDX_OXYGEN;
    cfg->elements_mass_fraction[0] = 0.111894f;
    cfg->elements_mass_fraction[1] = 0.888106f;

    dedx_internal_evaluate_i_pot(cfg, &err);
    failures += check_int(err, DEDX_OK, "evaluate_i_pot custom err");
    failures += check_true(cfg->elements_i_value != NULL, "evaluate_i_pot should allocate element I values");
    if (cfg->elements_i_value != NULL) {
        failures += check_true(cfg->elements_i_value[0] > 0.0f, "hydrogen I value");
        failures += check_true(cfg->elements_i_value[1] > 0.0f, "oxygen I value");
    }
    failures += check_true(cfg->i_value > 0.0f, "compound I value");

    dedx_free_config(cfg, &err);
    return failures;
}

int main(void) {
    int failures = 0;

    failures += test_validate_state_gas();
    failures += test_validate_config_custom_atoms();
    failures += test_validate_config_bethe_custom_atoms();
    failures += test_validate_config_bethe_mass_fraction();
    failures += test_validate_config_resets_stale_err();
    failures += test_evaluate_i_pot_custom_elements();

    return failures;
}
