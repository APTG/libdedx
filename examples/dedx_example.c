#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>

int main()
{
  int err = 0;
  float energy  = 100; /* MeV/u */
  float ste;
  int cfg = 0;
  int bragg = 0;
  dedx_workspace *ws;

  printf("init\n");
  ws = dedx_allocate_workspace(1,&err);
  printf("initialized with err %i\n",err);

  cfg = dedx_load_config(ws,DEDX_PSTAR,1,DEDX_WATER,&bragg, &err);
  printf("config loaded with err %i\n",err);

  ste = dedx_get_stp(ws,cfg,energy,&err);

  printf("dedx_cleanup\n");
  dedx_free_workspace(ws,&err);
  printf("dEdx = %6.3E MeV cm2/g\n",ste);

  return 0;
}
