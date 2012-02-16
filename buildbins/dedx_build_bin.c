#include <stdio.h>
#include <stdlib.h>
#include "dedx.h"

int main(int argc, char *argv[])
{
  /* function to build all binary tables before installation */

  int i, err = 0;
  dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
  dedx_workspace *ws;
  int max_tables = 9; // maximum number of tables to be loaded

  int z[] =       {0,2,1,DEDX_ELECTRON,6,6,6,1,2,6  };
  int prog_id[] = {0,1,2,3            ,4,5,6,7,7,100};

  //  char str[100];
  char *str = (char *)malloc(100);

  ws = dedx_allocate_workspace(max_tables,&err);
  
  cfg->target = DEDX_WATER;

  for (i = 1; i<= max_tables; ++i) {
    cfg->prog = prog_id[i];
    cfg->ion = z[i];
    dedx_load_config2(ws,cfg,&err);
    printf("Setup %i %s ... ",
	cfg->prog, dedx_get_program_name(cfg->prog));
    
    if (err == 0) 
      printf("OK\n");
    else {
      printf("failed! Code %i:",err);
      dedx_get_error_code(str, err);      
      printf("  %s\n",str);
      printf("  %s ion z = %i on %s.\n",
	     dedx_get_program_name(cfg->prog),
	     z[i], 
	     dedx_get_material_name(cfg->target) );
    }
  }

  dedx_free_workspace(ws,&err);
  free(str);
  free(cfg);  
  return 0;
}
