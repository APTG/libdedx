#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <dedx.h>
/* gcc -g -lm dedx_get.c -o dedx -Wall -ldedx*/

int main(int argc, char *argv[])
{
  int err = 0;
  float energy  = -1.0;
  int z = -1;
  float stp;
  int cfg_id = 0;
  dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));

  int prog = -1;
  int target = DEDX_WATER;
  char *str = (char *)malloc(100);
  int bragg = 0;
  int vmaj = 0, vmin = 0, vpatch = 0, vsvn=0; // version number
  const int *ion_list, *mat_list, *prog_list;
  int i = 0;
  //  char buff[127];
  dedx_workspace *ws;
  
  //if ((argc > 5) || (argc < 2)){
  if (argc != 5) {
    dedx_get_version(&vmaj,&vmin,&vpatch,&vsvn);
    printf("\n This is getdedx using libdEdx version %i.%i.%i-svn%i\n",
	   vmaj,vmin,vpatch,vsvn);
    printf(" (c) 2012 by Jakob Toftegaard <jakob.toftegaard@gmail.com>\n");
    printf(" and Niels Bassler <bassler@phys.au.dk>\n");
    printf(" See http://libdedx.sf.net\n\n");
    printf("Usage: %s program_id  Z icru_target_id energy\n\n", argv[0]);
    printf("Use numbers or capital letters for specifying program, ion and target.\n");
    printf("Negative numbers prints available parameters and exits.\n\n");
    printf("Examples:\n");
    printf("   %s 2 1 276 100\n",argv[0]);
    printf("   %s MSTAR CARBON WATER 100\n",argv[0]);
    printf("   %s PSTAR HYDROGEN -1 100\n", argv[0]);
    printf("\n");
    return 0;
  }
  
  opterr = 0;

  if (isdigit(*argv[1]))
    prog = atoi(argv[1]);
  else if (*argv[1] == '-'){
    prog = -1;
  }
  else {    
    i = 0;
    prog_list = dedx_get_program_list();
    while (prog_list[i] != -1) {
      //printf("ff '%s' '%s' %i\n",argv[1],dedx_get_program_name(prog_list[i]),i);fflush(stdout);
      if(strcmp(argv[1],dedx_get_program_name(prog_list[i])) == 0) {
	prog=prog_list[i];
	//printf("%s - this is %s %i\n",argv[1],dedx_get_program_name(prog),prog);
      }
      i++;
    }
  }

  if (isdigit(*argv[2])) 
    z = atoi(argv[2]);
  else if (*argv[2] == '-'){
    z = -1;
  }
  else {
    i = 0;
    ion_list = dedx_get_ion_list(prog);
    while (ion_list[i] != -1) {
      if(strcmp(argv[2],dedx_get_ion_name(ion_list[i])) == 0) {
	z = ion_list[i];
	//printf("%s - this is %s %i\n",argv[2],dedx_get_ion_name(z),z);
      }
      i++;
    }
  }


  if (isdigit(*argv[3]))
    target = atoi(argv[3]);
  else if (*argv[3] == '-'){
    target = -1;
  }
  else {
    i = 0;
    mat_list = dedx_get_material_list(prog);
    while (mat_list[i] != -1) {
      if(strcmp(argv[3],dedx_get_material_name(mat_list[i])) == 0) {
	target = mat_list[i];
	//printf("%s - this is %s %i\n",argv[3],dedx_get_material_name(target),target);
      }
      i++;
    }
  }

  energy = atof(argv[4]);

  if (prog == -1) {
    printf ("These stopping power tables/programs are availabe in libdEdx:\n");
    prog_list = dedx_get_program_list();
    i = 0;
    while (prog_list[i] != -1) {
      printf("%i: %s\n", prog_list[i], dedx_get_program_name(prog_list[i]));
      i++;
    }
    printf("\n");
    return 0;
  }


  if (z==-1) {
    printf ("%s can handle the following ions:\n",dedx_get_program_name(prog));
    ion_list = dedx_get_ion_list(prog);
    i = 0;
    while (ion_list[i] != -1) {
      printf("%i: %s\n", ion_list[i], dedx_get_ion_name(ion_list[i]));
      i++;
    }
    printf("\n");
    return 0;
  }



  if (target == -1) {
    printf ("%s can handle the following materials:\n",
	    dedx_get_program_name(prog));
    mat_list = dedx_get_material_list(prog);
    i = 0;
    while (mat_list[i] != -1) {
      printf("%i: %s\n", mat_list[i], dedx_get_material_name(mat_list[i]));
      i++;
    }
    printf("\nAdditional materials are possible by Braggs additivity rule,\n");
    printf("as long as the constituent elements are availble in the list above.");
    printf("\n");
    return 0;
  }

  if (energy == -1) {
    printf("%s allows %s in the energy interval from %.1e MeV/nucl to %.0f MeV/nucl.\n",
	   dedx_get_program_name(prog),
	   dedx_get_ion_name(z),
	   dedx_get_min_energy(prog,z),
	   dedx_get_max_energy(prog,z)
	   );
    return 0;
  }

  if ((energy < dedx_get_min_energy(prog,z) ) || (energy > dedx_get_max_energy(prog,z))) 
    {
      printf("%s allows %s in the energy interval from %.1e MeV/nucl to %.0f MeV/nucl.\n",
	     dedx_get_program_name(prog),
	     dedx_get_ion_name(z),
	     dedx_get_min_energy(prog,z),
	     dedx_get_max_energy(prog,z)
	     ); 
      exit(-1);
    }
  
  if(prog == DEDX_ESTAR) {    
    // only z = 1 makes sense here, so lets override user setting 
    z = -1;
    printf("%f MeV ELECTRONS on %s target using %s\n", 	   
	   energy, 
	   dedx_get_material_name(target), 
	   dedx_get_program_name(prog));  
  }
  else {
    printf("%f MeV/nucl %s ions on %s target using %s\n", 
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

  cfg->prog = prog;
  cfg->target = target;
  cfg->ion = z;
  cfg_id = dedx_load_config2(ws,cfg,&err);  

  if (err != 0) {
    fprintf(stderr,"dedx_load_config, error %i:", err);
    dedx_get_error_code(str, err);      
    fprintf(stderr, "  %s\n",str);
    exit(1);
  }

  stp = dedx_get_stp(ws,cfg_id,energy,&err);
  if (err != 0) {
    fprintf(stderr,"dedx_read_energy, error %i:", err);
    dedx_get_error_code(str, err);      
    fprintf(stderr, "  %s\n",str);
    exit(1);
  }

  dedx_free_workspace(ws,&err);
  free(str);
  free(cfg);

  if (bragg) {
    printf(" Bragg's additivity rule was applied,\n");
    printf("since compound %s is not in %s data table.\n", 
	   dedx_get_material_name(target),  
	   dedx_get_program_name(prog));
  }
  printf("1/rho dE/dx = %6.3E MeV cm2/g\n",stp);

  return 0;
}
