#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

int main()
{
    int err = 0;
    float energies[100];
    float results[100];
    int program = DEDX_BETHE_EXT00;
    int ion = DEDX_HYDROGEN;
    int target = DEDX_WATER_LIQUID;
    int no_of_points = 100;
    for(int i = 1; i < no_of_points + 1; i++){
        energies[i - 1] = i * 1.1;
    }

    err = dedx_get_stp_table(program, ion, target, no_of_points, energies, results);
    if (err != 0) printf("Encountered error no. %d", err);
    else
        for(int i = 0; i < no_of_points; i++)
            printf("%f \n", results[i]);

    return 0;
}
