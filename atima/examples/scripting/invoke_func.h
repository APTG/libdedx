#ifndef INVOKE_FUNC_DEF
#define INVOKE_FUNC_DEF
#include <dedx.h>
#include <stdio.h>
enum {DEDX_SCRIPT_NONE=0,DEDX_SCRIPT_CONFIG,DEDX_SCRIPT_ENERGY};
struct func_struct 
{
	char cmd[20];
	int (*func_p)(char * params);
	struct func_struct * next;	
};
struct data_struct
{
	char name[20];
	dedx_config * config;
	dedx_config * config_temp;
	dedx_workspace * ws;
	float * energy;
	int e_length;
	int type;
	struct data_struct * next;
};
int build_functions();
int invoke_function(char * cmd, char * params);
int set_output(FILE *output);
#endif

