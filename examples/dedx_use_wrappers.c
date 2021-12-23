#include <stdio.h>
#include <dedx.h>

#include "dedx_wrappers.h"

int main() {
    int err = 0;
    float energies[10];
    float stps[10];
    double csdas[10];
    const int program = DEDX_PSTAR;
    const int ion = DEDX_HYDROGEN;
    const int target = DEDX_WATER_LIQUID;

    int no_of_samples;

    no_of_samples = dedx_get_stp_table_size(program, ion, target);
    printf("No of data samples %d\n", no_of_samples);

    float *default_energies = calloc(sizeof(float), no_of_samples);
    float *default_stps = calloc(sizeof(float), no_of_samples);

    dedx_fill_default_energy_stp_table(program, ion, target, default_energies, default_stps);

    free(default_energies);
    free(default_stps);

    int no_of_points = 10;
    for (int i = 0; i < no_of_points; i++) {
        energies[i] = 1.f + i * 1.1f;
    }

    err = dedx_get_stp_table(program, ion, target, no_of_points, energies, stps);
    if (err != 0)
        printf("Encountered error no. %d", err);
    else
        for (int i = 0; i < no_of_points; i++)
            printf("%f \n", stps[i]);

    err = dedx_get_csda_table(program, ion, target, no_of_points, energies, csdas);
    if (err != 0)
        printf("Encountered error no. %d", err);
    else
        for (int i = 0; i < no_of_points; i++)
            printf("%f \n", csdas[i]);

    float result = dedx_get_simple_stp_for_program(program, ion, target, energies[0], &err);
    if (err != 0)
        printf("%f", result);
    else
        printf("The program returned an error no. %d", err);

    return 0;
}
