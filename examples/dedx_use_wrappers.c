#include <stdio.h>
#include <dedx.h>

#include "dedx_wrappers.h"

int main() {
    int err = 0;
    float energies[10];
    float results[10];
    int program = DEDX_PSTAR;
    int ion = DEDX_HYDROGEN;
    int target = DEDX_WATER_LIQUID;

    int no_of_samples;

    no_of_samples = dedx_get_stp_table_size(program, ion,target);
    printf("No of data samples %d\n", no_of_samples);

//    int no_of_points = 10;
//    for (int i = 0; i < no_of_points; i++) {
//        energies[i] = 1.f + i * 1.1f;
//    }
//
//    err = dedx_get_stp_table(program, ion, target, no_of_points, energies, results);
//    if (err != 0)
//        printf("Encountered error no. %d", err);
//    else
//        for (int i = 0; i < no_of_points; i++)
//            printf("%f \n", results[i]);
//
//    float result = dedx_get_simple_stp_for_program(program, ion, target, energies[0], &err);
//    if (err != 0)
//        printf("%f", result);
//    else
//        printf("The program returned an error no. %d", err);

    return 0;
}
