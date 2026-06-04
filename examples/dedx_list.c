#include <dedx.h>
#include <stdio.h>
#include <stdlib.h>

/* Lists all input IDs */
/* gcc -g -lm dedx_list.c -o dedx -Wall -ldedx*/

int main(int argc, char *argv[]) {

    int i;

    printf("List all known tables:\n");
    for (i = 0; i < 10; ++i)
        printf(" %3i %s\n", i, dedx_get_program_name(i));

    printf("\nList all known algorithms:\n");
    for (i = 100; i < 110; ++i)
        printf(" %3i %s\n", i, dedx_get_program_name(i));

    printf("\nList all known ions:\n");
    for (i = 0; i < 120; ++i)
        printf(" %3i %s\n", i, dedx_get_ion_name(i));

    printf("\nList all known materials:\n");
    for (i = 0; i < 300; ++i)
        printf(" %3i %s\n", i, dedx_get_material_name(i));

    /* Material / ion property accessors */
    {
        int err = 0;
        printf("\nCarbon ion: A=%d, atomic mass=%.4f u\n",
               dedx_get_nucleon_number(DEDX_CARBON, &err),
               dedx_get_atom_mass(DEDX_CARBON, &err));
        printf(
            "Water: density=%.4f g/cm^3, gas=%d\n", dedx_get_density(DEDX_WATER, &err), dedx_is_gas(DEDX_WATER, &err));
        printf("Air:   density=%.4g g/cm^3, gas=%d\n", dedx_get_density(DEDX_AIR, &err), dedx_is_gas(DEDX_AIR, &err));
    }

    // printf("NB: %i\n", DEDX_WATER);
    return 0;
}
