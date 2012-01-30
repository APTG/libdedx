#include <stdio.h>
#include <stdlib.h>
#include "dedx.h"

int main(int argc, char *argv[])
{
  /* function to build all binaray files before installation */

  int err = 0;
  int cfg = 0;  // gcc bug? Cant get rid of the warning here.
  int prog = 0;
  int target = DEDX_WATER;
  int i;
  int bragg = 0;
  dedx_workspace *ws;
  int max_tables = 9; // maximum number of tables to be loaded

  int z[] =       {0,2,1,DEDX_ELECTRON,6,6,6,1,2,6  };
  int prog_id[] = {0,1,2,3            ,4,5,6,7,7,100};

  //  char str[100];
  char *str = (char *)malloc(100);

  ws = dedx_allocate_workspace(max_tables,&err);

  // TODO: no helium ions in ICRU 73? 
  
  for (i = 1; i<= max_tables; ++i) {
    prog = prog_id[i];
    cfg = dedx_load_config(ws,prog,z[i],target,&bragg,&err);
    printf("Setup %i %s ... ",
	prog, dedx_get_program_name(prog));
    
    if (err == 0) 
      printf("OK\n");
    else {

      printf("failed! Code %i:",err);
      dedx_get_error_code(str, err);      
      printf("  %s\n",str);
      printf("  %s ion z = %i on %s.\n",
	     dedx_get_program_name(prog),
	     z[i], 
	     dedx_get_material_name(target) );
    }
  }

  dedx_free_workspace(ws,&err);
  //dedx_clean_up();
  free(str);
  
  return 0;
}
