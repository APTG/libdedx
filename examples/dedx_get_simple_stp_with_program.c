#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dedx.h>

int main(int argc, char *argv[]) {
    int err;
    float energy = 150.0;
    int program = DEDX_BETHE_EXT00;
    int ion = DEDX_HYDROGEN;
    int target = DEDX_WATER_LIQUID;

    float result = dedx_get_simple_stp_for_program(program, ion, target, energy , &err);
    if(err != 0) printf("%f", result);
    else prrintf("The program returned an error no. %d", err);

    return 0;
}