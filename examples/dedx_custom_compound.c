#include <stdio.h>
#include <stdlib.h>

#include "dedx.h"

int main() {
    int err = DEDX_OK;
    char err_str[256];

    // Allocate workspace
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);
    if (err != DEDX_OK) {
        printf("Failed to allocate workspace.\n");
        return 1;
    }

    // Set up configuration for a custom compound (e.g. Water H2O)
    dedx_config *config = calloc(1, sizeof(dedx_config));
    config->program = DEDX_BETHE_EXT00;
    config->ion = DEDX_PROTON;
    config->target = 0; // 0 is used for custom compound, or any ID > 99

    // Custom composition parameters:
    // H2O -> 2 * Hydrogen, 1 * Oxygen
    config->elements_id = calloc(2, sizeof(int));
    config->elements_id[0] = DEDX_HYDROGEN;
    config->elements_id[1] = DEDX_OXYGEN;

    config->elements_atoms = calloc(2, sizeof(int));
    config->elements_atoms[0] = 2; // 2 atoms of H
    config->elements_atoms[1] = 1; // 1 atom of O

    config->elements_length = 2;
    config->rho = 1.000f; // Density in g/cm^3

    // We intentionally leave config->elements_i_value as NULL
    // so it will fetch default I-values for H and O.

    // Load configuration
    if (dedx_load_config(ws, config, &err) != 0) {
        dedx_get_error_code(err_str, err);
        printf("Failed to load configuration: %s\n", err_str);
        dedx_free_config(config, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    // Energies in MeV/u to test
    float energies[] = {1.0f, 10.0f, 100.0f};
    printf("Proton stopping power in custom H2O (Bethe):\n");
    for (int i = 0; i < 3; i++) {
        float stp = dedx_get_stp(ws, config, energies[i], &err);
        if (err != DEDX_OK) {
            dedx_get_error_code(err_str, err);
            printf("Error calculating stopping power: %s\n", err_str);
        } else {
            printf("  %6.1f MeV/u: %10.3e MeV cm^2 / g\n", energies[i], stp);
        }
    }

    // Clean up
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return 0;
}
