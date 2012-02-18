#include "dedx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define n_test 9

void dedx_initialize_test();
void dedx_loadASTAR_test();
void dedx_loadPSTAR_test();
void dedx_loadMSTAR_test();
void dedx_loadIRCU49_PROTON_test();
void dedx_loadIRCU49_HELIUM_test();
void dedx_loadBethe_test();
void dedx_loadICRU73_test();
void dedx_loadICRU73_new_test();

int main(int argv, char * argc[])
{
	void (*func[])() = {dedx_initialize_test,dedx_loadASTAR_test,dedx_loadPSTAR_test,dedx_loadMSTAR_test,dedx_loadIRCU49_PROTON_test,dedx_loadIRCU49_HELIUM_test,dedx_loadBethe_test,dedx_loadICRU73_test,dedx_loadICRU73_new_test};
	int test = 0;
	if(argv < 1)
		return(-1);
	test = atoi(argc[1]);
	if(n_test < test)
		return(-1);
	(*func[test-1])();
	return 0;
}
void dedx_loadICRU73_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_ICRU73_OLD;
	cfg->ion = DEDX_LITHIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,5,3,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_loadICRU73_new_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_ICRU73;
	cfg->ion = DEDX_LITHIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,6,3,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);

}
void dedx_loadBethe_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_BETHE;
	cfg->ion = DEDX_HELIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,100,2,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_loadASTAR_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_ASTAR;
	cfg->ion = DEDX_HELIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,1,2,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_initialize_test()
{
	int err = 0; 
	dedx_workspace *ws;
	ws = dedx_allocate_workspace(1,&err);
	assert(err == 0);
	dedx_free_workspace(ws,&err);
}
void dedx_loadPSTAR_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_PSTAR;
	cfg->ion = DEDX_PROTON;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,2,1,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_loadMSTAR_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_MSTAR;
	cfg->ion = DEDX_LITHIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,4,3,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_loadIRCU49_PROTON_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_ICRU49;
	cfg->ion = DEDX_PROTON;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,7,1,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
}
void dedx_loadIRCU49_HELIUM_test()
{
	int err = 0;
	//int bragg = 0;
	dedx_workspace *ws;
	dedx_config *cfg = (dedx_config *)calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	cfg->program = DEDX_ICRU49;
	cfg->ion = DEDX_HELIUM;
	cfg->target = DEDX_WATER;
	dedx_load_config2(ws,cfg,&err);
	//dedx_load_config(ws,7,2,276,&bragg,&err);
	assert(err == 0);
	free(cfg);
	dedx_free_workspace(ws,&err);
	
}
