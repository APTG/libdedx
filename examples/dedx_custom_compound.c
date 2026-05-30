#include <stdio.h>
#include <stdlib.h>

#include "dedx.h"

int main() {
    int err = DEDX_OK;
    char err_str[256];

    // Allocate workspace for 3 datasets
    dedx_workspace *ws = dedx_allocate_workspace(3, &err);
    if (err != DEDX_OK) {
        printf("Failed to allocate workspace.\n");
        return 1;
    }

    // Energies in MeV/u to test
    float energies[] = {1.0f, 10.0f, 100.0f};

    // ---------------------------------------------------------
    // 1. Custom Water (H2O)
    // ---------------------------------------------------------
    dedx_config *config_custom_h2o = calloc(1, sizeof(dedx_config));
    config_custom_h2o->program = DEDX_BETHE_EXT00;
    config_custom_h2o->ion = DEDX_PROTON;
    config_custom_h2o->target = 0; // 0 is used for custom compound, or any ID > 99

    // Custom composition parameters:
    // H2O -> 2 * Hydrogen, 1 * Oxygen
    config_custom_h2o->elements_id = calloc(2, sizeof(int));
    config_custom_h2o->elements_id[0] = DEDX_HYDROGEN;
    config_custom_h2o->elements_id[1] = DEDX_OXYGEN;

    config_custom_h2o->elements_atoms = calloc(2, sizeof(int));
    config_custom_h2o->elements_atoms[0] = 2; // 2 atoms of H
    config_custom_h2o->elements_atoms[1] = 1; // 1 atom of O

    config_custom_h2o->elements_length = 2;
    config_custom_h2o->rho = 1.000f; // Density in g/cm^3

    // We intentionally leave elements_i_value as NULL
    // so it will fetch default I-values for H and O.

    // Load configuration
    if (dedx_load_config(ws, config_custom_h2o, &err) != 0) {
        dedx_get_error_code(err_str, err);
        printf("Failed to load custom H2O configuration: %s\n", err_str);
        return 1;
    }

    // ---------------------------------------------------------
    // 2. Predefined Water (H2O)
    // ---------------------------------------------------------
    dedx_config *config_predef_h2o = calloc(1, sizeof(dedx_config));
    config_predef_h2o->program = DEDX_BETHE_EXT00;
    config_predef_h2o->ion = DEDX_PROTON;
    config_predef_h2o->target = DEDX_WATER; // Use predefined water

    // Load configuration
    if (dedx_load_config(ws, config_predef_h2o, &err) != 0) {
        dedx_get_error_code(err_str, err);
        printf("Failed to load predefined H2O configuration: %s\n", err_str);
        return 1;
    }

    // ---------------------------------------------------------
    // 3. Custom Ethanol (C2H5OH)
    // ---------------------------------------------------------
    dedx_config *config_ethanol = calloc(1, sizeof(dedx_config));
    config_ethanol->program = DEDX_BETHE_EXT00;
    config_ethanol->ion = DEDX_PROTON;
    config_ethanol->target = 0; // Custom compound

    // Custom composition parameters:
    // C2H5OH -> 2 * Carbon, 6 * Hydrogen, 1 * Oxygen
    config_ethanol->elements_id = calloc(3, sizeof(int));
    config_ethanol->elements_id[0] = DEDX_CARBON;
    config_ethanol->elements_id[1] = DEDX_HYDROGEN;
    config_ethanol->elements_id[2] = DEDX_OXYGEN;

    config_ethanol->elements_atoms = calloc(3, sizeof(int));
    config_ethanol->elements_atoms[0] = 2; // 2 atoms of C
    config_ethanol->elements_atoms[1] = 6; // 6 atoms of H
    config_ethanol->elements_atoms[2] = 1; // 1 atom of O

    config_ethanol->elements_length = 3;
    config_ethanol->rho = 0.78945f; // Density of ethanol in g/cm^3

    // Load configuration
    if (dedx_load_config(ws, config_ethanol, &err) != 0) {
        dedx_get_error_code(err_str, err);
        printf("Failed to load ethanol configuration: %s\n", err_str);
        return 1;
    }

    // Print comparisons
    printf("Proton stopping power (Bethe) in MeV cm^2 / g:\n\n");
    printf("%-15s %-25s %-25s %-25s\n", "Energy (MeV/u)", "Custom H2O", "Predefined H2O", "Custom Ethanol (C2H5OH)");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < 3; i++) {
        float stp_custom_h2o = dedx_get_stp(ws, config_custom_h2o, energies[i], &err);
        float stp_predef_h2o = dedx_get_stp(ws, config_predef_h2o, energies[i], &err);
        float stp_ethanol = dedx_get_stp(ws, config_ethanol, energies[i], &err);

        printf("%-15.1f %-25.3e %-25.3e %-25.3e\n", energies[i], stp_custom_h2o, stp_predef_h2o, stp_ethanol);
    }

    // Clean up
    dedx_free_config(config_custom_h2o, &err);
    dedx_free_config(config_predef_h2o, &err);
    dedx_free_config(config_ethanol, &err);
    dedx_free_workspace(ws, &err);

    return 0;
}
