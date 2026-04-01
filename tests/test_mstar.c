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
                "FAIL mode-equivalence: %s target=%d E=%.3e MeV/u got %.5e expected %.5e\n",
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

    return failures;
}
