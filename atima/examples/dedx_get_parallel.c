#include <stdio.h>
#include <stdlib.h>
#include <dedx.h>
/* gcc -g -lm dedx_get.c -o dedx -Wall -ldedx*/

int main(int argc, char *argv[])
{
  int err = 0;
  float energy  = 100;
  int z = 1;
  float stp;
  int cfg = 0;
  int prog = 0;
  int target = DEDX_WATER;
  char *str = (char *)malloc(100);
  int bragg = 0;
  int vmaj = 0, vmin = 0, vpatch = 0, vsvn=0; // version number
  dedx_workspace *ws;

  // TODO: option parsing (getopt)

  if (argc != 5) {
    dedx_get_version(&vmaj,&vmin,&vpatch,&vsvn);
    printf("\n This is getdedx using libdEdx version %i.%i.%i-svn%i\n",
	   vmaj,vmin,vpatch,vsvn);
    printf(" (c) 2012 by Jakob Toftegaard <jakob.toftegaard@gmail.com>\n");
    printf(" and Niels Bassler <bassler@phys.au.dk>\n");
    printf(" See http://libdedx.sf.net\n\n");
    printf("Usage: %s program_id  Z icru_target_id energy.\n", argv[0]);
    printf("E.g. 100 MeV protons, PSTAR on water:\n   %s 2 1 276 100\n",argv[0]);
    exit(0);
  }

  prog = atof(argv[1]);
  z = atoi(argv[2]);
  target = atoi(argv[3]);
  energy = atof(argv[4]);

  if(prog == DEDX_ESTAR) {    
    // only z = 1 makes sense here, so lets override user setting 
    z = -1;
    printf("%f MeV ELECTRONS on %s target using %s\n", 	   
	   energy, 
	   dedx_get_material_name(target), 
	   dedx_get_program_name(prog));  
  }
  else {
    printf("%f MeV/u %s ions on %s target using %s\n", 
	   energy, 
	   dedx_get_ion_name(z),
	   dedx_get_material_name(target), 
	   dedx_get_program_name(prog)); 
  }

  ws = dedx_allocate_workspace(1,&err);
  if (err != 0) {
    fprintf(stderr,"dedx_initialize, error %i:", err);
    dedx_get_error_code(str, err);      
    fprintf(stderr, "  %s\n",str);
    exit(1);
  }

  cfg = dedx_load_config(ws,prog,z,target,&bragg, &err);
  if (err != 0) {
    fprintf(stderr,"dedx_load_config, error %i:", err);
    dedx_get_error_code(str, err);      
    fprintf(stderr, "  %s\n",str);
    exit(1);
  }

  stp = dedx_get_stp(ws,cfg,energy,&err);
  if (err != 0) {
    fprintf(stderr,"dedx_read_energy, error %i:", err);
    dedx_get_error_code(str, err);      
    fprintf(stderr, "  %s\n",str);
    exit(1);
  }

  dedx_free_workspace(ws,&err);
  free(str);

  if (bragg)
    printf(" Bragg's additivity rule was applied,\n since compound %s is not in %s data table.\n", dedx_get_material_name(target),  dedx_get_program_name(prog));
  printf("dEdx = %6.3E MeV cm2/g\n",stp);

  return 0;
}
