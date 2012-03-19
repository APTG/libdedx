#include <dedx.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* gcc -g -lm test.c -o test -Wall -ldedx */


int testConfig(dedx_config * config, char * text, float energy);
int success;
int only_error = 0;
int main()
{
	success = 1;
	dedx_config * config;
	//Test ASTAR
	dedx_config * astar = (dedx_config *)calloc(1,sizeof(dedx_config));
	astar->program = DEDX_ASTAR;
	astar->ion = DEDX_HELIUM;
	astar->target = DEDX_WATER;
	testConfig(astar,"Test ASTAR on Water",1);

	//Test PSTAR
	dedx_config * pstar = (dedx_config *)calloc(1,sizeof(dedx_config));
	pstar->program = DEDX_PSTAR;
	pstar->ion = DEDX_PROTON;
	pstar->target = DEDX_WATER;
	testConfig(pstar,"Test PSTAR on Water",1);

	//Test MSTAR
	dedx_config * mstar = (dedx_config *)calloc(1,sizeof(dedx_config));
	mstar->program = DEDX_MSTAR;
	mstar->ion = DEDX_CARBON;
	mstar->target = DEDX_WATER;
	testConfig(mstar,"Test MSTAR on Water",1);

	//Test ICRU73_OLD
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU73_OLD;
	config->ion = DEDX_CARBON;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU73_OLD on Water",1);
	free(config);

	//Test ICRU73
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU73;
	config->ion = DEDX_CARBON;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU73 on Water",1);
	free(config);

	//Test ICRU73_AIR
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU73;
	config->ion = DEDX_CARBON;
	config->target = DEDX_AIR;
	testConfig(config,"Test ICRU73 on AIR",1);
	free(config);

	//Test Bethe on Hydrogen
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = DEDX_CARBON;
	config->target = DEDX_HYDROGEN;
	testConfig(config,"Test Bethe on Hydrogen",1);
	free(config);

	//Test Bethe on Water With intern I value
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = DEDX_CARBON;
	config->target = DEDX_WATER;
	testConfig(config,"Test Bethe on Water with intern I value",1);
	free(config);

	//Test Bethe on Water with own I value
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = DEDX_CARBON;
	config->target = DEDX_WATER;
	config->i_value = 78.0;
	testConfig(config,"Test Bethe on Water with own I value",1);
	free(config);

	//ICRU49 Hydrogen on water
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU49;
	config->ion = DEDX_PROTON;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU49 Hydrogen on Water",1);
	free(config);

	//ICRU49 Helium on water
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU49;
	config->ion = DEDX_HELIUM;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU49 Helium on Water",1);
	free(config);

	//ICRU Wrapper Hydrogen On water
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU;
	config->ion = DEDX_PROTON;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU Hydrogen on Water",1);
	free(config);

	//ICRU Wrapper Helium On water
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU;
	config->ion = DEDX_HELIUM;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU Helium on Water",1);
	free(config);

	//ICRU Wrapper Carbon On water
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_ICRU;
	config->ion = DEDX_CARBON;
	config->target = DEDX_WATER;
	testConfig(config,"Test ICRU Carbon on Water",1);
	free(config);

	
	//Bethe hydrogen on hydrogen state gas
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = DEDX_PROTON;
	config->target = DEDX_HYDROGEN;
	config->compound_state = DEDX_GAS;
	testConfig(config,"Test Bethe Hydrogen on Hydrogen gas",1);
	free(config);

	//Bethe hydrogen on hydrogen state condensed
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = DEDX_PROTON;
	config->target = DEDX_HYDROGEN;
	config->compound_state = DEDX_CONDENSED;
	testConfig(config,"Test Bethe Hydrogen on Hydrogen condensed",1);
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
	testConfig(config,"Test ASTAR on own composition of water by atoms",1);
	free(config);

	//ASTAR with own composition of water by weight
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = 1;
	config->ion = 2;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_mass_fraction = calloc(2,sizeof(float));
	config->elements_mass_fraction[0] = 0.13;
	config->elements_mass_fraction[1] = 0.87;
	config->elements_length = 2;
	testConfig(config,"Test ASTAR on own composition of water by weight",1);
	free(config);

	//Bethe with own composition
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = 2;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_atoms = calloc(2,sizeof(int));
	config->elements_atoms[0] = 2;
	config->elements_atoms[1] = 1;
	config->elements_length = 2;
	testConfig(config,"Test Bethe on own composition of water by atoms",1);
	free(config);

	//Bethe with own composition and own i value
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = 2;
	config->i_value = 78;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_atoms = calloc(2,sizeof(int));
	config->elements_atoms[0] = 2;
	config->elements_atoms[1] = 1;
	config->elements_length = 2;
	testConfig(config,"Test Bethe on own composition of water by atoms and own I value",1);
	free(config);

	//Bethe with own composition and own i value
	config = (dedx_config *)calloc(1,sizeof(dedx_config));
	config->program = DEDX_BETHE_EXT00;
	config->ion = 2;
	config->i_value = 78;
	config->elements_id = calloc(2,sizeof(int));
	config->elements_id[0] = DEDX_HYDROGEN;
	config->elements_id[1] = DEDX_OXYGEN;
	config->elements_atoms = calloc(2,sizeof(int));
	config->elements_atoms[0] = 2;
	config->elements_atoms[1] = 1;
	config->elements_length = 2;
	testConfig(config,"Test Bethe on own composition of water by atoms and own I value",1);
	free(config);


	if(success)
	{
		printf("All test passed\n");
	}
	return 0;
}

int testConfig(dedx_config * config, char * text, float energy)
{
	int err = 0;
	float stp;
	char err_str[100];
	dedx_workspace *ws = dedx_allocate_workspace(1,&err);


	dedx_load_config(ws,config,&err);
	
	if(err != 0)
	{
		success = 0;
		dedx_get_error_code(err_str,err);
		printf("%s : %s\n",text,err_str);
		return err;
	}
	
	stp = dedx_get_stp(ws,config,energy,&err);
	if(err != 0)
	{
		success = 0;
		dedx_get_error_code(err_str,err);
		printf("%s : %s\n",text,err_str);
		return err;
	}
	if(only_error == 0)
		printf("%s : stp %f\n",text,stp);
	dedx_free_workspace(ws,&err);
 	return 0;	
}
