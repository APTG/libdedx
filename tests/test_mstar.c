#include "test_helpers.h"

static dedx_config *make_mstar_mode_config(int target, char mode) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_MSTAR;
    cfg->ion = DEDX_CARBON;
    cfg->target = target;
    cfg->mstar_mode = mode;
    return cfg;
}

static int report_mode_equivalence_error(const char *stage, const char *label, int err) {
    fprintf(stderr, "FAIL %s: %s err=%d\n", stage, label, err);
    return 1;
}

static int check_err(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got err=%d, expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

/*
 * Regression test for issue #149 finding A7: an mstar_mode outside the documented
 * DEDX_MSTAR_MODE_* set used to fall through dedx_mpaul.c's mode switch to an empty
 * "illegal mode" branch, silently leaving the computed table all zero with err == 0
 * instead of reporting an error. dedx_internal_validate_config() must now reject an
 * unrecognized mode up front, and leave every documented mode (including the '\0'
 * default) working exactly as before. */
static int test_invalid_mstar_mode(void) {
    int failures = 0;
    int err = 0;
    dedx_config *cfg = make_mstar_mode_config(DEDX_WATER, 'Z');
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_INVALID_MSTAR_MODE, "MSTAR invalid mode 'Z'");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* The documented default ('\0', unset by a caller who never touches mstar_mode)
     * must still validate cleanly -- the new check must not reject the common case. */
    err = 0;
    cfg = make_mstar_mode_config(DEDX_WATER, '\0');
    ws = dedx_allocate_workspace(1, &err);
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_OK, "MSTAR default mode '\\0'");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* mstar_mode is only meaningful for DEDX_MSTAR itself; an unrelated program must
     * not be rejected just because a caller left garbage in the field (e.g. a struct
     * reused across loads without being re-zeroed). */
    err = 0;
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    cfg->mstar_mode = 'Z';
    ws = dedx_allocate_workspace(1, &err);
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_OK, "PSTAR ignores unrelated garbage mstar_mode");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

/*
 * Regression pin raised in review of issue #149's Phase 2 PR (finding B1):
 * load_compound() resolving config->compound_state before its per-constituent loop
 * (added for A3, so Bethe-type constituents get the *compound's* gas/condensed state
 * rather than the public dedx_get_i_value()'s hardcoded gas default) has a side effect
 * on DEDX_MSTAR specifically. resolve_mstar_mode() (dedx_mstar.c) also reads
 * config->compound_state, and only falls back to its own per-target gas check when
 * compound_state is still DEDX_DEFAULT_STATE; once load_compound() resolves it first,
 * MSTAR mode 'a'/'b' letters now pick gas ('g'/'h') or condensed ('c'/'d') from the
 * *compound's* own state for every constituent, rather than each constituent element's
 * own state as before. For a gas compound like BUTANE this changes MSTAR's numeric
 * output by a large margin (mode flips from 'g' to 'h' end to end) -- previously an
 * undocumented, untested side effect of A3; not a computed-value regression on its
 * own (the new behavior is arguably more physically correct: Bragg additivity treats
 * a compound's condensed/gas state as a property of the compound, not of each atom in
 * isolation -- the same convention this fix already applies for I-values), but it
 * needed to be a pinned, visible number instead of an unremarked diff.
 *
 * These values are the current output of this branch, not independently verified
 * against dedx_web or the MSTAR/ICRU literature -- that cross-check is out of scope
 * for what this test can do. Their job is only to make a *future* change to this
 * resolution path show up here instead of silently drifting again.
 */
static int test_gas_compound_uses_compound_state_not_constituent(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    /* BUTANE is a gas: with the default mstar_mode ('\0' -> DEDX_MSTAR_MODE_DEFAULT ==
     * 'b'), resolve_mstar_mode() must now pick 'h' from the compound's own state. */
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_BUTANE, '\0'), energy_grid[0], 7027.838867f, "mstar-butane-gas");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_BUTANE, '\0'), energy_grid[1], 7598.953613f, "mstar-butane-gas");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_BUTANE, '\0'), energy_grid[2], 1849.043213f, "mstar-butane-gas");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_BUTANE, '\0'), energy_grid[3], 350.166901f, "mstar-butane-gas");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_BUTANE, '\0'), energy_grid[4], 86.426956f, "mstar-butane-gas");

    /* WATER is condensed, so its resolved mode ('d') is unaffected either way -- these
     * match the pre-existing explicit-mode-'d' values below exactly, confirming the
     * default-mode path still resolves the same way for a condensed compound. */
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_WATER, '\0'), energy_grid[0], 5589.206055f, "mstar-water-condensed");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, '\0'), energy_grid[4], 80.005943f, "mstar-water-condensed");

    return failures;
}

static int check_mode_equivalence(int target, char lhs_mode, char rhs_mode, float energy, const char *label) {
    int err = 0;
    int failures = 0;
    dedx_workspace *ws = dedx_allocate_workspace(2, &err);
    dedx_config *lhs = make_mstar_mode_config(target, lhs_mode);
    dedx_config *rhs = make_mstar_mode_config(target, rhs_mode);
    float lhs_value = 0.0f;
    float rhs_value = 0.0f;

    if (err != 0 || ws == NULL || lhs == NULL || rhs == NULL) {
        failures = report_mode_equivalence_error("alloc", label, err);
        dedx_free_config(lhs, &err);
        dedx_free_config(rhs, &err);
        dedx_free_workspace(ws, &err);
        return failures;
    }

    dedx_load_config(ws, lhs, &err);
    if (err != 0)
        failures = report_mode_equivalence_error("load lhs", label, err);

    if (failures == 0) {
        dedx_load_config(ws, rhs, &err);
        if (err != 0)
            failures = report_mode_equivalence_error("load rhs", label, err);
    }

    if (failures == 0) {
        lhs_value = (float) dedx_get_stp(ws, lhs, energy, &err);
        if (err != 0)
            failures = report_mode_equivalence_error("stp lhs", label, err);
    }

    if (failures == 0) {
        rhs_value = (float) dedx_get_stp(ws, rhs, energy, &err);
        if (err != 0)
            failures = report_mode_equivalence_error("stp rhs", label, err);
    }

    if (failures == 0 && check_result(lhs_value, rhs_value)) {
        fprintf(stderr,
                "FAIL mode-equivalence: %s target=%d E=%.3e MeV/nucl got %.5e expected %.5e\n",
                label,
                target,
                energy,
                lhs_value,
                rhs_value);
        failures = 1;
    }

    dedx_free_config(lhs, &err);
    dedx_free_config(rhs, &err);
    dedx_free_workspace(ws, &err);
    return failures;
}

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    /* Reference values below were extracted from the original MSTAR 3.12
     * Fortran sources via MSTAR1/MSPAUL.
     */
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_C), energy_grid[0], 5.634276e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_C), energy_grid[1], 6.592632e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_C), energy_grid[2], 1.639345e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_C), energy_grid[3], 3.165557e2f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_C), energy_grid[4], 7.993779e1f, "mstar-c");

    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_D), energy_grid[0], 5.589206e3f, "mstar-d");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_D), energy_grid[1], 6.586625e3f, "mstar-d");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_D), energy_grid[2], 1.639723e3f, "mstar-d");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_D), energy_grid[3], 3.167525e2f, "mstar-d");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_D), energy_grid[4], 8.000594e1f, "mstar-d");

    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_G), energy_grid[0], 4.340127e3f, "mstar-g");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_G), energy_grid[1], 5.262383e3f, "mstar-g");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_G), energy_grid[2], 1.442963e3f, "mstar-g");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_G), energy_grid[3], 2.808873e2f, "mstar-g");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_G), energy_grid[4], 7.126015e1f, "mstar-g");

    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_H), energy_grid[0], 4.140607e3f, "mstar-h");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_H), energy_grid[1], 5.216270e3f, "mstar-h");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_H), energy_grid[2], 1.447108e3f, "mstar-h");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_H), energy_grid[3], 2.808873e2f, "mstar-h");
    failures += check_config_stp(
        make_mstar_mode_config(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_H), energy_grid[4], 7.126015e1f, "mstar-h");

    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_WATER, energy_grid[0], 5.589e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_WATER, energy_grid[1], 6.587e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_WATER, energy_grid[2], 1.640e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_WATER, energy_grid[3], 3.168e2f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_WATER, energy_grid[4], 8.001e1f);

    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_PMMA, energy_grid[0], 6.135e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_PMMA, energy_grid[1], 6.395e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_PMMA, energy_grid[2], 1.599e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_PMMA, energy_grid[3], 3.094e2f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_PMMA, energy_grid[4], 7.762e1f);

    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[0], 5.634e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[1], 6.593e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[2], 1.639e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[3], 3.166e2f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[4], 7.994e1f, "mstar-a");

    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[0], 6.185e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[1], 6.400e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[2], 1.598e3f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[3], 3.082e2f, "mstar-a");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[4], 7.755e1f, "mstar-a");

    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[0], 6.349e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[1], 6.538e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[2], 1.614e3f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[3], 3.103e2f, "mstar-c");
    failures +=
        check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[4], 7.767e1f, "mstar-c");

    failures +=
        check_mode_equivalence(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_A, DEDX_MSTAR_MODE_G, 10.0f, "mstar-a-gas");
    failures +=
        check_mode_equivalence(DEDX_AIR_DRY_NEAR_SEA_LEVEL, DEDX_MSTAR_MODE_B, DEDX_MSTAR_MODE_H, 10.0f, "mstar-b-gas");

    failures += test_invalid_mstar_mode();
    failures += test_gas_compound_uses_compound_state_not_constituent();

    return failures;
}
