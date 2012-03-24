#include <dedx.h>
#include <dedx_tools.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* gcc -g -lm test.c -o test -Wall -ldedx -Wall */

#define TEST_STP 1
#define TEST_CSDA 2
#define TEST_ISTP 3
#define TEST_ICSDA 4

int test_run(int test, dedx_config * config, char * text, 
	     float input_value, float result, float err_accept);

int err_count = 0;

int main()
{
  printf("--------------------------------------------------------------");
  printf("--------------------------------------------------------------\n");
  printf("%-70s : Calculated  - Expected       Conclusion\n","");
  printf("--------------------------------------------------------------");
  printf("--------------------------------------------------------------\n");


  float err_accept = 0.005; // accept 0.5 % deviation to either side
  dedx_config * config;
  //Test ASTAR
  dedx_config * astar = (dedx_config *)calloc(1,sizeof(dedx_config));
  astar->program = DEDX_ASTAR;
  astar->ion = DEDX_HELIUM;
  astar->target = DEDX_WATER;
  test_run(TEST_STP,astar,"01 Test ASTAR on Water",1,1.034e3,err_accept);

  //Test PSTAR
  dedx_config * pstar = (dedx_config *)calloc(1,sizeof(dedx_config));
  pstar->program = DEDX_PSTAR;
  pstar->ion = DEDX_PROTON;
  pstar->target = DEDX_WATER;
  test_run(TEST_STP,pstar,"02 Test PSTAR on Water",1,2.606E+02,err_accept);

  //Test MSTAR
  dedx_config * mstar = (dedx_config *)calloc(1,sizeof(dedx_config));
  mstar->program = DEDX_MSTAR;
  mstar->ion = DEDX_CARBON;
  mstar->target = DEDX_WATER;
  test_run(TEST_STP,mstar,"03 Test MSTAR on Water",1,6.587E+03,err_accept);

  //Test ICRU73_OLD
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU73_OLD;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"04 Test ICRU73_OLD on Water",1,7.199E+03,err_accept);
  free(config);

  //Test ICRU73
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU73;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"05 Test ICRU73 on Water",1,6.884E+03,err_accept);
  free(config);

  //Test ICRU73_AIR
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU73;
  config->ion = DEDX_CARBON;
  config->target = DEDX_AIR;
  test_run(TEST_STP,config,"06 Test ICRU73 on AIR",1,6.138E+03,err_accept);
  free(config);

  //Test Bethe on Hydrogen
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_HYDROGEN;
  test_run(TEST_STP,config,"07 Test Bethe on Hydrogen",1,1.948E+04,err_accept);
  free(config);

  //Test Bethe on Water With intern I value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"08 Test Bethe on Water with internal I value",1,7.447E+03,err_accept);
  free(config);

  //Test Bethe on Water with own I value
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  config->i_value = 78.0;
  test_run(TEST_STP,config,"09 Test Bethe on Water with I=78eV ",1,7.36088e+03,err_accept);
  free(config);

  //ICRU49 Hydrogen on water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU49;
  config->ion = DEDX_PROTON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"10 Test ICRU49 Hydrogen on Water",1,2.606E+02,err_accept);
  free(config);

  //ICRU49 Helium on water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU49;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"11 Test ICRU49 Helium on Water",1,1.034E+03,err_accept);
  free(config);

  //ICRU Wrapper Hydrogen On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_PROTON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"12 Test ICRU Hydrogen on Water",1,2.606E+02,err_accept);
  free(config);

  //ICRU Wrapper Helium On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"13 Test ICRU Helium on Water",1,1.034E+03,err_accept);
  free(config);

  //ICRU Wrapper Carbon On water
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ICRU;
  config->ion = DEDX_CARBON;
  config->target = DEDX_WATER;
  test_run(TEST_STP,config,"14 Test ICRU Carbon on Water",1,6.884E+03,err_accept);
  free(config);

	
  //Bethe hydrogen on hydrogen state gas
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_PROTON;
  config->target = DEDX_HYDROGEN;
  config->compound_state = DEDX_GAS;
  test_run(TEST_STP,config,"15 Test Bethe Hydrogen on Hydrogen gas",1,6.769E+02,err_accept);
  free(config);

  //Bethe hydrogen on hydrogen state condensed
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_BETHE_EXT00;
  config->ion = DEDX_PROTON;
  config->target = DEDX_HYDROGEN;
  config->compound_state = DEDX_CONDENSED;
  test_run(TEST_STP,config,"16 Test Bethe Hydrogen on Hydrogen condensed",1,6.58728e+02,err_accept);
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
  test_run(TEST_STP,config,"17 Test ASTAR on own composition of water by atoms",1,1.034E+03,err_accept);
  free(config);

  //ASTAR with own composition of water by weight
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->elements_id = calloc(2,sizeof(int));
  config->elements_id[0] = DEDX_HYDROGEN;
  config->elements_id[1] = DEDX_OXYGEN;
  config->elements_mass_fraction = calloc(2,sizeof(float));
  config->elements_mass_fraction[0] = 0.13;
  config->elements_mass_fraction[1] = 0.87;
  config->elements_length = 2;
  test_run(TEST_STP,config,"18 Test ASTAR on own composition of water by weight",1,1.034E+03,err_accept);
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
  test_run(TEST_STP,config,"19 Test Bethe on own composition of water by atoms",1,1.058E+03 ,err_accept);
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
  config->elements_mass_fraction[0] = 0.13; // crash
  config->elements_mass_fraction[1] = 0.87;
  config->elements_length = 2;
  config->rho = 1.000;
  test_run(TEST_STP,config,"20 Test Bethe on own composition of water by weight and I=78eV",1,1.058E+03,err_accept);
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
  test_run(TEST_STP,config,"21 Test Bethe on own composition of water by atoms and own I value",1,1.058E+03,err_accept);
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
  test_run(TEST_STP,config,"22 Test Bethe on own composition of alanine by atoms",1,2.578e+2,err_accept);
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
  test_run(TEST_STP,config,"23 Test Bethe on own composition of alanine by atoms",1,6.906e+3,err_accept);
  free(config);

  //Test ASTAR, tools
  config = (dedx_config *)calloc(1,sizeof(dedx_config));
  config->program = DEDX_ASTAR;
  config->ion = DEDX_HELIUM;
  config->target = DEDX_WATER;
  config->ion_a = 4;
  dedx_config temp;
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_CSDA,temp,"24 Test ASTAR CSDA in Water",100,7.760,err_accept);
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_ISTP,temp,"25 Test ASTAR iSTP in Water",1.03381e+03,1,err_accept);
  memcpy(temp,config,sizeof(dedx_config));
  test_run(TEST_ICSDA,temp,"26 Test ASTAR iCSDA in Water",7.760,100,err_accept);
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
    printf("%-70s : %.5e - %.5e... OUT OF BOUNDS\n",
	   text,test_result,result);
    err_count++;
  } else
    printf("%-70s : %.5e - %.5e... OK\n",text,test_result,result);
  dedx_free_workspace(ws,&err);
  return 0;	
}
