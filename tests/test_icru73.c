#include "test_helpers.h"

// Reference values from ICRU Report 73 (2005): Stopping of Ions Heavier than Helium

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_WATER, energy_grid[0], 6.329e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_WATER, energy_grid[1], 6.884e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_WATER, energy_grid[2], 1.630e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_WATER, energy_grid[3], 3.144e2f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_WATER, energy_grid[4], 7.968e1f);

    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_PMMA, energy_grid[0], 6.880e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_PMMA, energy_grid[1], 6.978e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_PMMA, energy_grid[2], 1.623e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_PMMA, energy_grid[3], 3.108e2f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_PMMA, energy_grid[4], 7.845e1f);

    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_ALANINE, energy_grid[0], 6.671e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_ALANINE, energy_grid[1], 6.912e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_ALANINE, energy_grid[2], 1.612e3f);
    failures += check_stp(DEDX_ICRU73, DEDX_CARBON, DEDX_ALANINE, energy_grid[4], 7.812e1f);

    return failures;
}
