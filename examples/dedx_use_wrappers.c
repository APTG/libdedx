#include <stdio.h>
#include <dedx.h>

#include "dedx_wrappers.h"

int main() {
    int err = 0;
    float energies[100];
    float results[100];
    int program = DEDX_BETHE_EXT00;
    int ion = DEDX_HYDROGEN;
    int target = DEDX_WATER_LIQUID;
    int no_of_points = 100;
    for (int i = 1; i < no_of_points + 1; i++) {
        energies[i - 1] = i * 1.1f;
    }

    err = dedx_get_stp_table(program, ion, target, no_of_points, energies, results);
    if (err != 0)
        printf("Encountered error no. %d", err);
    else
        for (int i = 0; i < no_of_points; i++)
            printf("%f \n", results[i]);

    float result = dedx_get_simple_stp_for_program(program, ion, target, energies[0], &err);
    if (err != 0)
        printf("%f", result);
    else
        printf("The program returned an error no. %d", err);

    return 0;
}
