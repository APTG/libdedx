#include <dedx.h>
#include <dedx_tools.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* gcc -g -lm test.c -o test -Wall -ldedx -Wall */

#define TEST_STP 1
#define TEST_CSDA 2
#define TEST_ISTP 3
#define TEST_ICSDA 4

int test_stp(int *nr, int program, int ion, int target, float energy, float result);
int test_cstp(int *nr, dedx_config *config, char *str, float energy, float result);

int test_run(int test, dedx_config * config, char * text, 
	     float input_value, float result, float err_accept);
void spacer(void);

int err_count = 0;

int main()
{

  float err_accept = 0.005; // accept 0.5 % deviation to either side
  dedx_config * config;

  int vmajor,vminor,vpatch,vsvn;
  int i,counter = 1;
  float energy_grid[] = {0.07,1,10,78,1000};

  dedx_get_version(&vmajor,&vminor,&vpatch,&vsvn);

  spacer();
  printf("Test#     libdEdx, version %i.%i.%i-svn%i",
	 vmajor,vminor,vpatch,vsvn);
  printf("%-31s : Calculated  - Expected      - Conclusion    %%%%\n",
	 "");
  spacer();


  /* TEST PSTAR */
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_WATER,energy_grid[0],8.183e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_WATER,energy_grid[1],2.606e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_WATER,energy_grid[2],4.564e1);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_WATER,energy_grid[3],8.791e0);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_WATER,energy_grid[4],2.211e0);

  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_PMMA,energy_grid[0],9.319e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_PMMA,energy_grid[1],2.520e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_PMMA,energy_grid[2],4.450e1);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_PMMA,energy_grid[3],8.558e0);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_PMMA,energy_grid[4],2.145e0);

  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],9.324e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],2.581e2);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],4.495e1);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],8.617e0);
  test_stp(&counter,DEDX_PSTAR,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],2.148e0);
  spacer();

  /* TEST ASTAR */
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_WATER,energy_grid[0],1.824e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_WATER,energy_grid[1],1.034e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_WATER,energy_grid[2],1.815e2);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_WATER,energy_grid[3],3.498e1);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_WATER,250.0         ,1.557e1);

  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_PMMA,energy_grid[0],2.002e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_PMMA,energy_grid[1],1.004e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_PMMA,energy_grid[2],1.769e2);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_PMMA,energy_grid[3],3.405e1);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_PMMA,250.0         ,1.516e1);

  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_ALANINE,energy_grid[0],2.055e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_ALANINE,energy_grid[1],1.025e3);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_ALANINE,energy_grid[2],1.787e2);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_ALANINE,energy_grid[3],3.429e1);
  test_stp(&counter,DEDX_ASTAR,DEDX_HELIUM,DEDX_ALANINE,250.0         ,1.526e1);
  spacer();

  /* TEST MSTAR */
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_WATER,energy_grid[0],5.589e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_WATER,energy_grid[1],6.587e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.640e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.168e2);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_WATER,energy_grid[4],8.001e1);

  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_PMMA,energy_grid[0],6.135e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_PMMA,energy_grid[1],6.395e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.599e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.094e2);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.762e1);
  //// ____ up to here values are finialized ____ /NB
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST ICRU Protons */
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST ICRU49 Protons */
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();


  /* TEST ICRU Carbon ions */
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST ICRU73 Carbon ions */
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST ICRU73_OLD Carbon ions */
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST BETHE_EXT00 protons */
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  /* TEST BETHE_EXT00 Carbon ions */
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[4],1.111e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],1.111e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.111e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],1.111e1);
  spacer();

  //Test ASTAR
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  test_cstp(&counter,config,"foobar",250.0,2.145e0);




  //Test Bethe on Hydrogen
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_HYDROGEN;
  test_run(TEST_STP,config,"07 Test Bethe C on Hydrogen",1,1.948E+04,err_accept);
  free(config);

  //Test Bethe on Water With intern I value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"08 Test Bethe C on Water with internal I value",1,7.447E+03,err_accept);
  free(config);

  //Test Bethe on Water with own I value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  config->i_value = 78.0;
  test_run(TEST_STP,config,"09 Test Bethe C on Water with I=78eV ",1,7.36088e+03,err_accept);
  free(config);

  //ICRU49 Hydrogen on water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU49;
  config->ion = DEDX_PROTON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"10 Test ICRU49 H on Water",1,2.606E+02,err_accept);
  free(config);

  //ICRU49 Helium on water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU49;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"11 Test ICRU49 He on Water",1,1.034E+03,err_accept);
  free(config);

  //ICRU Wrapper Hydrogen On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_PROTON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"12 Test ICRU H on Water",1,2.606E+02,err_accept);
  free(config);

  //ICRU Wrapper Helium On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"13 Test ICRU He on Water",1,1.034E+03,err_accept);
  free(config);

  //ICRU Wrapper Carbon On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"14 Test ICRU C on Water",1,6.884E+03,err_accept);
  free(config);

	
  //Bethe hydrogen on hydrogen state gas
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_PROTON;
  config->target = DEDX_HYDROGEN;
  config->compound_state = DEDX_GAS;
  test_run(TEST_STP,config,"15 Test Bethe H on Hydrogen gas",1,6.769E+02,err_accept);
  free(config);

  //Bethe hydrogen on hydrogen state condensed
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_PROTON;
  config->target = DEDX_HYDROGEN;
  config->compound_state = DEDX_CONDENSED;
  test_run(TEST_STP,config,"16 Test Bethe H on Hydrogen condensed",1,6.58728e+02,err_accept);
  free(config);

  //ASTAR with own composition of water by atoms
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_atoms = calloc(2,sizeof(int));
  config->elements_atoms[0] = 2;
  config->elements_atoms[1] = 1;
  config->elements_length = 2;
  test_run(TEST_STP,config,"17 Test ASTAR He on own composition of water by atoms",1,1.063E+03,err_accept);
  free(config);

  //ASTAR with own composition of water by weight
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_mass_fraction = calloc(2,sizeof(float));
  config->elements_mass_fraction[0] = 0.1119;
  config->elements_mass_fraction[1] = 0.8881;
  config->elements_length = 2;
  test_run(TEST_STP,config,"18 Test ASTAR He on own composition of water by weight",1,1.063E+03,err_accept);
  free(config);
    
  //Bethe with own composition
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_HELIUM;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_atoms = calloc(2,sizeof(int));
  config->elements_atoms[0] = 2;
  config->elements_atoms[1] = 1;
  config->elements_length = 2;
  config->rho = 1.000;
  test_run(TEST_STP,config,"19 Test Bethe He on own composition of water by atoms",1,1.049E+03 ,err_accept);
  free(config);

  //Bethe with own composition and own i value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_HELIUM;
  config->i_value = 78;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_mass_fraction = calloc(2,sizeof(float));
  config->elements_mass_fraction[0] = 0.1119; 
  config->elements_mass_fraction[1] = 0.8881;
  config->elements_length = 2;
  config->rho = 1.000;
  test_run(TEST_STP,config,"20 Test Bethe He on own composition of water by weight and I=78eV",1,1.045E+03,err_accept);
  free(config);
  
  //Bethe with own composition and own i value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_HELIUM;
  config->i_value = 78;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_atoms = calloc(2,sizeof(int));
  config->elements_atoms[0] = 2;
  config->elements_atoms[1] = 1;
  config->elements_length = 2;
  config->rho = 1.000;
  test_run(TEST_STP,config,"21 Test Bethe He on own composition of water by atoms and I=78eV",1,1.045E+03,err_accept);
  free(config);

  // alanine C3H7NO2
  /*SRIM 
    H 1 MeV  S_elec = 2.479e+2 MeV cm^2/g
    C 1 MeV/u S_elec = 7.21e+3 MeV cm^2/g
    
    BETHE+BRAGG
    H 1 MeV  S_elec = 2.578e+2 MeV cm^2/g
    C 1 MeV/u S_elec = 6.906e+3 MeV cm^2/g
  */
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_PROTON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->rho = 1.42;
  test_run(TEST_STP,config,"22 Test Bethe H on own composition of alanine by atoms",100,6.500e+0,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->rho = 1.42;
  test_run(TEST_STP,config,"23 Test Bethe C on own composition of alanine by atoms",100,2.34e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->rho = 1.42;
  test_run(TEST_STP,config,"24 Test MSTAR C on own composition of alanine by atoms",100,2.340e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->rho = 1.42;
  test_run(TEST_STP,config,"25 Test ICRU C on own composition of alanine by atoms",100,2.320e+2,err_accept);
  free(config);


  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  test_run(TEST_STP,config,
	   "26 Test ICRU C on own composition of alanine by atoms, no rho",
	   100,2.320e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->rho = 1.23;
  test_run(TEST_STP,config,
	   "27 Test ICRU C on own composition of alanine by atoms, NPL rho",
	   100,2.320e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  test_run(TEST_STP,config,
	   "28 Test MSTAR C on own composition of alanine by atoms, no rho",
	   100,2.340e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->elements_id = calloc(4,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_CARBON;
  config->elements_id[2] = DEDX_NITROGEN;
  config->elements_id[3] = DEDX_OXYGEN;
  config->elements_atoms = calloc(4,sizeof(int));
  config->elements_atoms[0] = 7;
  config->elements_atoms[1] = 3;
  config->elements_atoms[2] = 1;
  config->elements_atoms[3] = 2;
  config->elements_length = 4;
  config->compound_state = DEDX_GAS;
  config->rho = 1.23;
  test_run(TEST_STP,config,
	   "29 Test MSTAR C on own composition of alanine by atoms, NPL rho",
	   100,2.340e+2,err_accept);
  free(config);


  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->target = DEDX_ALANINE;
  config->compound_state = DEDX_GAS;
  test_run(TEST_STP,config,
	   "30 Test MSTAR C on DEDX_ALANINE",
	   100,2.340e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->target = DEDX_ALANINE;
  //config->compound_state = DEDX_CONDENSED;
  test_run(TEST_STP,config,
	   "31 Test ICRU C on DEDX_ALANINE",
	   100,2.320e+2,err_accept);
  free(config);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_ALANINE;
  test_run(TEST_STP,config,
	   "32 Test Bethe C on DEDX_ALANINE",
	   100,2.320e+2,err_accept);
  free(config);

  //Test ASTAR, tools
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  config->ion_a = 4;
  dedx_config * temp = calloc(1,sizeof(dedx_config));
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_CSDA,temp,"31 Test ASTAR He CSDA in Water",100,7.760,err_accept);
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_ISTP,temp,"32 Test ASTAR He iSTP in Water",1.03381e+03,1,err_accept);
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_ICSDA,temp,"33 Test ASTAR He iCSDA in Water",7.760,100,err_accept);
  free(config);
  free(temp);


  printf("--------------------------------------------------------------");
  printf("--------------------------------------------------------------\n");
  if(err_count == 0)
    printf("All test passed.\n");
  else
    printf("Found %i errors.\n", err_count);
  return 0;
}


int test_stp(int *nr, int program, int ion, int target, float energy, float result)
{
  dedx_config * config;
  char temp[80];
  float err_accept=5e-3;


  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = program;
  config->ion = ion;
  config->target = target;
  sprintf(temp, "%02i - %s %.3e MeV/u %s on %s",
	  *nr, 
	  dedx_get_program_name(program),
	  energy,
	  dedx_get_ion_name(ion),
	  dedx_get_material_name(target));
	  
  test_run(TEST_STP,config,temp,energy,result,err_accept);
  (*nr)++;
  free(config);
  return 0;
}

int test_cstp(int *nr, dedx_config *config, char *str, float energy, float result) {

  char temp[80];
  float err_accept=5e-3;

  sprintf(temp, "%02i - %s %.3e MeV/u %s on %s - %s",
	  *nr, 
	  dedx_get_program_name(config->program),
	  energy,
	  dedx_get_ion_name(config->ion),
	  dedx_get_material_name(config->target),
	  str);
	  
  test_run(TEST_STP,config,temp,energy,result,err_accept);
  (*nr)++;
  free(config);
  return 0;
}


int test_run(int test, dedx_config * config, char * text, 
	     float input_value, float result, float err_accept)
{
  int err = 0;
  float test_result;
  char err_str[100];
  dedx_workspace *ws = dedx_allocate_workspace(2,&err);

  dedx_load_config(ws,config,&err);
  
  if(err != 0)
    {
      err_count++;
      dedx_get_error_code(err_str,err);
      printf("%s : %s\n",text,err_str);
      return err;
    }
  switch(test) {
  case TEST_STP:    
    test_result = dedx_get_stp(ws,config,input_value,&err);
    break;
  case TEST_CSDA:
    test_result = dedx_get_csda(ws,config,input_value,&err);
    break;
  case TEST_ISTP:
    test_result = dedx_get_inverse_stp(ws,config,input_value,1,&err); /* side = 1 */
    break;
  case TEST_ICSDA:
    test_result = dedx_get_inverse_csda(ws,config,input_value,&err);
    break;
  default:
    fprintf(stderr,"Invalid test no %i\n", test);
    exit(-1);
    break;
  }


  if(err != 0)
    {
      err_count++;
      dedx_get_error_code(err_str,err);
      printf("%s : %s\n",text,err_str);
      return err;
    }
 
  /* check if result is acceptible */ 
  if ((test_result > result*(1+err_accept)) || 
      (test_result < result*(1-err_accept))) {	 	  
    printf("%-70s : %.5e - %.5e ... OUT OF BOUNDS %+.1f\n",
	   text,test_result,result, (test_result-result)*1e3/result);
    err_count++;
  } else
    printf("%-70s : %.5e - %.5e ... OK            %+.1f\n",
	   text,test_result,result, (test_result-result)*1e3/result);
  dedx_free_workspace(ws,&err);
  return 0;	
}

void spacer(void){

  printf("--------------------------------------------------------------");
  printf("--------------------------------------------------------------\n");

}
