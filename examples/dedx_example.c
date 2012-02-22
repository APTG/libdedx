#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

int main()
{
  int err = 0;
  float energy  = 100; /* MeV/u */
  float ste;
  //int cfg = 0;
  //int bragg = 0;
  dedx_workspace *ws;
  dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));

  printf("init\n");
  ws = dedx_allocate_workspace(1,&err);
  printf("initialized with err %i\n",err);

  cfg->program = DEDX_PSTAR;
  cfg->target = DEDX_WATER;
  cfg->ion = DEDX_PROTON;

  dedx_load_config(ws, cfg, &err);
  printf("config loaded with err %i\n",err);

  ste = dedx_get_stp(ws,cfg,energy,&err);

  printf("1/rho dEdx = %6.3E MeV cm2/g\n",ste);

  //printf("dedx_cleanup\n");
  dedx_free_workspace(ws,&err);
  dedx_free_config(cfg,&err);
  return 0;
}
