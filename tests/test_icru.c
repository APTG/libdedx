#include "test_helpers.h"

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_WATER, energy_grid[0], 8.183e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_WATER, energy_grid[1], 2.606e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_WATER, energy_grid[2], 4.564e1f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_WATER, energy_grid[3], 8.791e0f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_WATER, energy_grid[4], 2.211e0f);

    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_PMMA, energy_grid[0], 9.319e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_PMMA, energy_grid[1], 2.530e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_PMMA, energy_grid[2], 4.450e1f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_PMMA, energy_grid[3], 8.558e0f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_PMMA, energy_grid[4], 2.145e0f);

    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_ALANINE, energy_grid[0], 9.324e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_ALANINE, energy_grid[1], 2.581e2f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_ALANINE, energy_grid[2], 4.495e1f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_ALANINE, energy_grid[3], 8.617e0f);
    failures += check_stp(DEDX_ICRU, DEDX_PROTON, DEDX_ALANINE, energy_grid[4], 2.148e0f);

    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_WATER, energy_grid[0], 1.824e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_WATER, energy_grid[1], 1.034e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_WATER, energy_grid[2], 1.815e2f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_WATER, energy_grid[3], 3.498e1f);

    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_PMMA, energy_grid[0], 2.002e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_PMMA, energy_grid[1], 1.004e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_PMMA, energy_grid[2], 1.769e2f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_PMMA, energy_grid[3], 3.405e1f);

    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_ALANINE, energy_grid[0], 2.055e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_ALANINE, energy_grid[1], 1.025e3f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_ALANINE, energy_grid[2], 1.787e2f);
    failures += check_stp(DEDX_ICRU, DEDX_HELIUM, DEDX_ALANINE, energy_grid[3], 3.429e1f);

    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, energy_grid[0], 6.329e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, energy_grid[1], 6.884e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, energy_grid[2], 1.630e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, energy_grid[3], 3.144e2f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, energy_grid[4], 7.968e1f);

    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_PMMA, energy_grid[0], 6.880e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_PMMA, energy_grid[1], 6.978e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_PMMA, energy_grid[2], 1.623e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_PMMA, energy_grid[3], 3.108e2f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_PMMA, energy_grid[4], 7.845e1f);

    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_ALANINE, energy_grid[0], 6.671e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_ALANINE, energy_grid[1], 6.912e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_ALANINE, energy_grid[2], 1.612e3f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_ALANINE, energy_grid[4], 7.812e1f);

    return failures;
}
