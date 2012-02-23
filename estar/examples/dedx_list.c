#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

/* Lists all input IDs */
/* gcc -g -lm dedx_list.c -o dedx -Wall -ldedx*/

int main(int argc, char *argv[])
{
  
  int i;

  printf("List all known tables:\n");
  for (i=0; i<10; ++i)
    printf(" %3i %s\n", i, dedx_get_program_name(i));

  printf("\nList all known algorithms:\n");
  for (i=100; i<110; ++i)
    printf(" %3i %s\n", i, dedx_get_program_name(i));

  printf("\nList all known ions:\n");
  for (i=0; i<120; ++i)
    printf(" %3i %s\n", i, dedx_get_ion_name(i));

  printf("\nList all known materials:\n");
  for (i=0; i<300; ++i)
        printf(" %3i %s\n", i, dedx_get_material_name(i));

  //printf("NB: %i\n", DEDX_WATER);
  return 0;
}
