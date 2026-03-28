#include "test_helpers.h"

static dedx_config *make_mstar_mode_config(int target, char mode) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_MSTAR;
    cfg->ion = DEDX_CARBON;
    cfg->target = target;
    cfg->mstar_mode = mode;
    return cfg;
}

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

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

    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_ALANINE, energy_grid[0], 6.312e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_ALANINE, energy_grid[1], 6.533e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_ALANINE, energy_grid[2], 1.614e3f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_ALANINE, energy_grid[3], 3.105e2f);
    failures += check_stp(DEDX_MSTAR, DEDX_CARBON, DEDX_ALANINE, energy_grid[4], 7.772e1f);

    failures += check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[0], 5.634e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[1], 6.593e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[2], 1.639e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[3], 3.166e2f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_WATER, DEDX_MSTAR_MODE_A), energy_grid[4], 7.994e1f, "mstar-a");

    failures += check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[0], 6.185e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[1], 6.400e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[2], 1.598e3f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[3], 3.082e2f, "mstar-a");
    failures += check_config_stp(make_mstar_mode_config(DEDX_PMMA, DEDX_MSTAR_MODE_A), energy_grid[4], 7.755e1f, "mstar-a");

    failures += check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[0], 6.349e3f, "mstar-c");
    failures += check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[1], 6.538e3f, "mstar-c");
    failures += check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[2], 1.614e3f, "mstar-c");
    failures += check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[3], 3.103e2f, "mstar-c");
    failures += check_config_stp(make_mstar_mode_config(DEDX_ALANINE, DEDX_MSTAR_MODE_C), energy_grid[4], 7.767e1f, "mstar-c");

    return failures;
}
