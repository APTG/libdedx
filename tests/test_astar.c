#include "test_helpers.h"

// Reference values from the NIST ASTAR database: https://physics.nist.gov/PhysRefData/Star/Text/ASTAR.html

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, energy_grid[0], 1.824e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, energy_grid[1], 1.034e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, energy_grid[2], 1.815e2f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, energy_grid[3], 3.498e1f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, 250.0f, 1.557e1f);

    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_PMMA, energy_grid[0], 2.002e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_PMMA, energy_grid[1], 1.004e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_PMMA, energy_grid[2], 1.769e2f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_PMMA, energy_grid[3], 3.405e1f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_PMMA, 250.0f, 1.516e1f);

    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_ALANINE, energy_grid[0], 2.055e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_ALANINE, energy_grid[1], 1.025e3f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_ALANINE, energy_grid[2], 1.787e2f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_ALANINE, energy_grid[3], 3.429e1f);
    failures += check_stp(DEDX_ASTAR, DEDX_HELIUM, DEDX_ALANINE, 250.0f, 1.526e1f);

    return failures;
}
