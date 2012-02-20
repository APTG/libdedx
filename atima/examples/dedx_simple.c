#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

/* Simplest possible example.  */

int main()
{  
  int err = 0;        // for error return code
  int z = 2;          // carbon ions
  float energy = 100; // MeV/amu
  float ste = 0;

  ste =  dedx_get_simple_stp(DEDX_ASTAR,z,DEDX_WATER,energy, &err);
  ste =  dedx_get_simple_stp(DEDX_ASTAR,z,DEDX_WATER,energy, &err);

  printf("dE/dx = %6.3E MeV cm2/g\n",ste);
  printf("Error %d\n",err);

  dedx_clean_up();

  return 0;
}
