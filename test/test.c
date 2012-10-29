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
int test_stp_i(int *nr, int program, int ion, int target, float energy, float result);
int test_cstp(int *nr, dedx_config *config, char *str, float energy, float result);

int test_run(int test, dedx_config * config, char * text, 
	     float input_value, float result, float err_accept);
void spacer(void);
void skip(int *nr);


int err_count = 0;
int err;

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


  /* TEST PSTAR 001-015 */
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

  /* TEST ASTAR 016-030 */
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

  /* TEST MSTAR DEFAULT MODE (should be equal to b-mode*/
  /* 031-045 */
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
  /* 'd' mode, since it is Braggs rule for condensed matter */
  /* but 'd' is not allowed for hydrogen, falling back to a then,  */
  /* at least this should be libdEdx default behaviour */
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],6.312e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],6.533e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.614e3);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],3.105e2);
  test_stp(&counter,DEDX_MSTAR,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.772e1);
  spacer();


  /* TEST MSTAR 'a' MODE, differences are especially at low E = 0.07 MeV/nucl */
  /* 46 - 60 */
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  config->mstar_mode = DEDX_MSTAR_MODE_A;
  test_cstp(&counter,config,"a mode",energy_grid[0],5.634e3);
  test_cstp(&counter,config,"a mode",energy_grid[1],6.593e3);
  test_cstp(&counter,config,"a mode",energy_grid[2],1.639e3);
  test_cstp(&counter,config,"a mode",energy_grid[3],3.166e2);
  test_cstp(&counter,config,"a mode",energy_grid[4],7.994e1);
  dedx_free_config(config, &err);
  
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->target = DEDX_PMMA;
  config->mstar_mode = DEDX_MSTAR_MODE_A;
  test_cstp(&counter,config,"a mode",energy_grid[0],6.185e3);
  test_cstp(&counter,config,"a mode",energy_grid[1],6.400e3);
  test_cstp(&counter,config,"a mode",energy_grid[2],1.598e3);
  test_cstp(&counter,config,"a mode",energy_grid[3],3.082e2);
  test_cstp(&counter,config,"a mode",energy_grid[4],7.755e1);
  dedx_free_config(config, &err);

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_MSTAR;
  config->ion = DEDX_CARBON;
  config->target = DEDX_ALANINE;
  config->mstar_mode = DEDX_MSTAR_MODE_C;
  test_cstp(&counter,config,"c mode",energy_grid[0],6.349e3);
  test_cstp(&counter,config,"c mode",energy_grid[1],6.538e3);
  test_cstp(&counter,config,"c mode",energy_grid[2],1.614e3);
  test_cstp(&counter,config,"c mode",energy_grid[3],3.103e2);
  test_cstp(&counter,config,"c mode",energy_grid[4],7.767e1);
  dedx_free_config(config, &err);
  spacer();


  /* TEST ICRU Protons */
  /* 61 - 75 */
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[0],8.183e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[1],2.606e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[2],4.564e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[3],8.791e0);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_WATER,energy_grid[4],2.211e0);

  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[0],9.319e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[1],2.530e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[2],4.450e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[3],8.558e0);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_PMMA,energy_grid[4],2.145e0);

  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],9.324e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],2.581e2);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],4.495e1);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],8.617e0);
  test_stp(&counter,DEDX_ICRU,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],2.148e0);
  spacer();

  /* 76 - 90 */
  /* TEST ICRU Helium ions */
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_WATER,energy_grid[0],1.824e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_WATER,energy_grid[1],1.034e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_WATER,energy_grid[2],1.815e2);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_WATER,energy_grid[3],3.498e1);
  skip(&counter);

  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_PMMA,energy_grid[0],2.002e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_PMMA,energy_grid[1],1.004e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_PMMA,energy_grid[2],1.769e2);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_PMMA,energy_grid[3],3.405e1);
  skip(&counter);

  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_ALANINE,energy_grid[0],2.055e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_ALANINE,energy_grid[1],1.025e3);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_ALANINE,energy_grid[2],1.787e2);
  test_stp(&counter,DEDX_ICRU,DEDX_HELIUM,DEDX_ALANINE,energy_grid[3],3.429e1);
  skip(&counter);
  spacer();


  /* TEST ICRU Carbon ions */ 
  /* 91 - 105 */
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[0],6.329e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[1],6.884e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.630e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.144e2);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_WATER,energy_grid[4],7.968e1);

  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[0],6.880e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[1],6.978e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.623e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.108e2);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.845e1);

  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],6.671e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],6.912e3);
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.612e3);
  //test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);  //skipped, since no data
  skip(&counter); //not possible
  test_stp(&counter,DEDX_ICRU,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.812e1);
  spacer();

  /* TEST ICRU49 Protons */
  /* 106 - 120 */
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[0],8.183e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[1],2.606e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[2],4.564e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[3],8.791e0);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_WATER,energy_grid[4],2.211e0);

  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[0],9.319e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[1],2.530e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[2],4.450e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[3],8.558e0);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_PMMA,energy_grid[4],2.145e0);

  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],9.324e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],2.581e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],4.495e1);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],8.617e0);
  test_stp(&counter,DEDX_ICRU49,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],2.148e0);
  spacer();

  /* TEST ICRU49 Helium ions */
  /* 121 - 135 */
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_WATER,energy_grid[0],1.824e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_WATER,energy_grid[1],1.034e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_WATER,energy_grid[2],1.815e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_WATER,energy_grid[3],3.498e1);
  skip(&counter);

  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_PMMA,energy_grid[0],2.002e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_PMMA,energy_grid[1],1.004e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_PMMA,energy_grid[2],1.769e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_PMMA,energy_grid[3],3.405e1);
  skip(&counter);

  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_ALANINE,energy_grid[0],2.055e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_ALANINE,energy_grid[1],1.025e3);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_ALANINE,energy_grid[2],1.787e2);
  test_stp(&counter,DEDX_ICRU49,DEDX_HELIUM,DEDX_ALANINE,energy_grid[3],3.429e1);
  skip(&counter);
  spacer();


  /* TEST ICRU73 carbon ions */
  /* 136 - 150 */
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[0],6.329e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[1],6.884e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.630e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.144e2);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_WATER,energy_grid[4],7.968e1);

  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[0],6.880e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[1],6.978e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.623e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.108e2);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.845e1);

  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],6.671e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],6.912e3);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.612e3);
  //test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);  //skipped, since no data
  skip(&counter);
  test_stp(&counter,DEDX_ICRU73,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.812e1);
  spacer();

  /* TEST ICRU73_OLD carbon ions */
  /* 151 - 165 */ 
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[0],7.049e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[1],7.199e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.673e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.206e2);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_WATER,energy_grid[4],8.088e1);

  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[0],6.880e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[1],6.978e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.623e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.108e2);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.845e1);

  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],6.671e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],6.912e3);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.612e3);
  //test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],1.111e1);  //skipped, since no data
  skip(&counter);
  test_stp(&counter,DEDX_ICRU73_OLD,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.812e1);
  spacer();


  /* TEST BETHE_EXT00 protons */
  /* 166 - 180 */
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[0],7.094e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[1],2.672e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[2],4.577e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[3],8.791e0);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[4],2.197e0);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[0],7.144e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[1],2.649e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[2],4.500e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[3],8.619e0);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[4],2.150e0);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],7.028e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],2.634e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],4.484e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],8.592e0);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],2.145e0);
  spacer();

  /* TEST BETHE_EXT00 Helium ions */
  /* 181 - 195 */
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[0],1.987e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[1],1.048e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[2],1.831e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[3],3.517e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[4],8.787e0);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[0],1.964e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[1],1.039e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[2],1.800e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[3],3.447e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[4],8.599e0);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[0],1.935e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[1],1.033e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[2],1.793e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[3],3.437e1);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[4],8.580e0);
  spacer();

  /* TEST BETHE_EXT00 Carbon ions */
  /* 196 - 210 */ 
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[0],5.109e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[1],7.380e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.635e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.165e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[4],7.908e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[0],5.010e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[1],7.317e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.608e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.103e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.739e1);

  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],4.944e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],7.269e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.601e3);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],3.093e2);
  test_stp(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.722e1);
  spacer();


  /* TEST BETHE_EXT00 new I protons */
  /* 211 - 225 */
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[0],7.336e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[1],2.692e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[2],4.597e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[3],8.821e0);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_WATER,energy_grid[4],2.202e0);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[0],7.318e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[1],2.663e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[2],4.515e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[3],8.640e0);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_PMMA,energy_grid[4],2.153e0);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[0],7.523e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[1],2.688e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[2],4.538e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[3],8.670e0);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_PROTON,DEDX_ALANINE,energy_grid[4],2.162e0);
  spacer();

  /* TEST BETHE_EXT00 new I Helium ions */
  /* 226 - 240 */
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[0],2.053e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[1],1.056e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[2],1.839e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[3],3.529e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_WATER,energy_grid[4],8.808e0);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[0],2.012e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[1],1.045e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[2],1.806e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[3],3.456e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_PMMA,energy_grid[4],8.614e0);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[0],2.055e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[1],1.055e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[2],1.815e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[3],3.468e1);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_HELIUM,DEDX_ALANINE,energy_grid[4],8.648e0);
  spacer();

  /* TEST BETHE_EXT00 new I Carbon ions */
  /* 241 - 255 */ 
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[0],5.226e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[1],7.439e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[2],1.642e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[3],3.175e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_WATER,energy_grid[4],7.927e1);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[0],5.097e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[1],7.359e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[2],1.613e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[3],3.110e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_PMMA,energy_grid[4],7.752e1);

  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[0],5.143e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[1],7.418e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[2],1.621e3);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[3],3.121e2);
  test_stp_i(&counter,DEDX_BETHE_EXT00,DEDX_CARBON,DEDX_ALANINE,energy_grid[4],7.783e1);
  spacer();




  /* -------- SO FAR SO GOOD ----------------------------------------------------- */
  // alanine C3H7NO2
  /*SRIM 
    H 1 MeV  S_elec = 2.479e+2 MeV cm^2/g
    C 1 MeV/u S_elec = 7.21e+3 MeV cm^2/g
    
    BETHE+BRAGG
    H 1 MeV  S_elec = 2.578e+2 MeV cm^2/g
    C 1 MeV/u S_elec = 6.906e+3 MeV cm^2/g
  */
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
  config->elements_i_value = calloc(4,sizeof(float)); 
  // this will force libdEdx to use default I-values per element, which is what SH12A also does.
  sprintf(temp, "%3i - %s %.3e MeV/u %s on %s",
	  *nr, 
	  dedx_get_program_name(program),
	  energy,
	  dedx_get_ion_name(ion),
	  dedx_get_material_name(target));
	  
  test_run(TEST_STP,config,temp,energy,result,err_accept);


 /*  if (target==DEDX_WATER) { */
 /*    printf("i: %f %f %f          ", config->i_value, */
 /* 	   config->elements_i_value[0], */
 /* 	   config->elements_i_value[1] */
 /* 	   ); */
 /*  } */

 /* if (target==DEDX_PMMA) { */
 /*    printf("i: %f %f %f %f       ", config->i_value, */
 /* 	   config->elements_i_value[0], */
 /* 	   config->elements_i_value[1], */
 /* 	   config->elements_i_value[2] */
 /* 	   ); */
 /* } */
 /* if (target==DEDX_ALANINE) { */
 /*    printf("i: %f %f %f %f %f    ", config->i_value, */
 /* 	   config->elements_i_value[0], */
 /* 	   config->elements_i_value[1], */
 /* 	   config->elements_i_value[2], */
 /* 	   config->elements_i_value[3] */
 /* 	   ); */
 /* } */


  (*nr)++;
  free(config);
  return 0;
}

int test_stp_i(int *nr, int program, int ion, int target, float energy, float result)
{
  dedx_config * config;
  char temp[80];
  float err_accept=5e-3;

  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = program;
  config->ion = ion;
  switch(target)
    {
    case DEDX_WATER:
      //config->compound_state = DEDX_CONDENSED; does not make sense if I values are specified manually
      config->elements_id = calloc(2,sizeof(int));
      config->elements_id[0] = DEDX_HYDROGEN;
      config->elements_id[1] = DEDX_OXYGEN;
      config->elements_atoms = calloc(2,sizeof(int));
      config->elements_atoms[0] = 2;
      config->elements_atoms[1] = 1;
      config->elements_length = 2;
      config->elements_i_value = calloc(2,sizeof(float));
      config->elements_i_value[0] = 19.2; // override default val of 21.8 for compounds
      // TODO: is it sufficient to only specify one value?
      // this should be mentioned in the README
      config->rho = 1.000;
      break;
    case DEDX_PMMA:
      //config->compound_state = DEDX_CONDENSED;
      config->elements_id = calloc(3,sizeof(int));
      config->elements_id[0] = DEDX_HYDROGEN;
      config->elements_id[1] = DEDX_CARBON;
      config->elements_id[2] = DEDX_OXYGEN;
      config->elements_atoms = calloc(3,sizeof(int));
      config->elements_atoms[0] = 8;
      config->elements_atoms[1] = 5;
      config->elements_atoms[2] = 2;
      config->elements_i_value = calloc(3,sizeof(float));
      config->elements_i_value[0] = 19.2; // override default val of 21.8 for compounds
      // C: 81.0 eV, O: 106.0 eV.
      config->elements_length = 3;
      config->rho = 1.190;
      break;
    case DEDX_ALANINE:
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
      config->elements_i_value = calloc(4,sizeof(float));
      /* from shieldhit12a_r370_dedxtable.dat where default accidentaly was gas-phase alanine */
      config->elements_i_value[0] = 19.2; // override libdEdx defaults
      config->elements_i_value[1] = 78.0; // --"--
      config->elements_i_value[2] = 82.0; // --"--
      config->elements_i_value[3] = 95.0; // --"--
      config->elements_length = 4;
      config->rho = 1.230;
      break;
    default:
      fprintf(stderr,"error: bad target\n");
      exit(-1);
    }

  sprintf(temp, "%3i - %s %.3e MeV/u %s on %s",
	  *nr, 
	  dedx_get_program_name(program),
	  energy,
	  dedx_get_ion_name(ion),
	  dedx_get_material_name(target));
  
  test_run(TEST_STP,config,temp,energy,result,err_accept);
  // TODO: could be nice if these could be probed
  // printf("i: %f   ",config->i_value);
  (*nr)++;
  free(config);
  return 0;
}

int test_cstp(int *nr, dedx_config *config, char *str, float energy, float result) {

  char temp[80];
  float err_accept=5e-3;

  sprintf(temp, "%3i - %s %.3e MeV/u %s on %s - %s",
	  *nr, 
	  dedx_get_program_name(config->program),
	  energy,
	  dedx_get_ion_name(config->ion),
	  dedx_get_material_name(config->target),
	  str);
	  
  test_run(TEST_STP,config,temp,energy,result,err_accept);
  (*nr)++;
  //  free(config);
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

void skip(int *nr) {
  printf("%3i - Skipped\n",*nr);
  (*nr)++;
}
