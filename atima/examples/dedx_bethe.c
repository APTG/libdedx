#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

int main()
{
  /* TODO: more elaborate example */
  int err = 0;
  float energy  = 100;
  float ste;
  int cfg1 = 0;
  int cfg2 = 0;
  int bragg = 0;
  int z = 6;      // carbon ions
  int target = DEDX_SILICON;
  float I = 175.0; // mean excitation energy in eV
  dedx_workspace *ws;

  /* TODO: this example is still broken */ 

  printf("Test Bethe function. 100 MeV/u Carbon ions on Silicon target.\n");
  ws = dedx_allocate_workspace(2,&err);  // two ways of calling bethe
  if (err != 0) {
    printf("init err %i\n",err);
    exit(-1);
  }

  /* default I-value */
  cfg1 = dedx_load_config(ws, DEDX_BETHE, z, target, &bragg, &err);
  if (err != 0){
    printf("load bethe config2 err %i\n",err);
    exit(-1);
  }


  /* overriding I-value */ // TODO not implemented for compounds
  cfg2 = dedx_load_bethe_config(ws, z, target, I, &err);
  if (err != 0){
    printf("load bethe config1 err %i\n",err);
    exit(-1);
  }

  ste = dedx_get_stp(ws, cfg1,energy,&err);
  if (err != 0){
    printf("cfg1: get energy err %i\n",err);
    exit(-1);
  }

  printf("Default:\n");
  printf("I-value %3.1f eV: dE/dx = %6.3E MeV cm2/g\n", 
	 dedx_get_i_value(target,&err), ste);

  printf("Override:\n");
  ste = dedx_get_stp(ws, cfg2,energy,&err);
  if (err != 0){
    printf("cfg2: get energy err %i\n",err);
    exit(-1);
  }
  printf("I-value %3.1f eV: dE/dx = %6.3E MeV cm2/g\n",I,ste);

  dedx_free_workspace(ws,&err);
  return 0;
}
