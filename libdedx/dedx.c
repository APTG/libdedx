/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dedx.h"
#include "dedx_mstar.h"
#include "dedx_file.h"
#include "dedx_file_access.h"
#include "dedx_stopping_data.h"
#include "dedx_spline.h"
#include "dedx_lookup_data.h"
#include "dedx_bethe.h"
//#include "dedx_atima.h"
#include "dedx_config.h"
#include "dedx_periodic_table.h"
#include "dedx_program_const.h"
#include "dedx_workspace.h"
#include "dedx_validate.h"
#include <math.h>

int _dedx_find_data(stopping_data * data,dedx_config * config,float * energy, int * err);

int _dedx_check_energy_bounds(_dedx_lookup_data * data, float energy);
int _dedx_load_bethe(stopping_data * data, int ion, int target, float * energy, int * err);
int _dedx_load_bethe2(stopping_data * data, 
		      float PZ, float PA, float TZ, float TA, 
		      float rho, float pot, float * energy, int * err);
int _dedx_load_bethe_config(dedx_workspace * ws, 
			    float PZ, float PA, float TZ, float TA, 
			    float rho, float pot, int * err);
int _calculate_bethe_energi_test(int ion, int target, float pot, int *err);
int _dedx_load_data(dedx_workspace * ws, stopping_data * data, 
		    float * energy, int prog, int * err);

float _dedx_get_min_energy_icru(int ion);
float _dedx_get_max_energy_icru(int ion);

int _dedx_check_ion(int prog, int ion);
int _dedx_find_data(stopping_data * data,dedx_config * config,float * energy, int * err);

int _dedx_load_config_clean(dedx_workspace *ws, dedx_config * config, int *err);
int _dedx_load_compound(dedx_workspace * ws, dedx_config * config, int * err);
int _dedx_load_bethe_2(stopping_data * data, dedx_config * config,float * energy, int * err);
int _dedx_find_bragg_data_2(stopping_data * data, dedx_config *config, float * energy, int *err);
int _dedx_load_atima(stopping_data * data, dedx_config * config, float * energy, int * err);

dedx_workspace * dedx_allocate_workspace(unsigned int count, int *err)
{
    int i = 0;
    *err = 0;

    dedx_workspace * temp = malloc(sizeof(dedx_workspace));
	if(temp == NULL)
	{
		*err = 301;
		return NULL;
	}
    temp->loaded_data = malloc(count*sizeof(_dedx_lookup_data * ));
    for(i = 0; i < count; i++)
    {
        temp->loaded_data[i] = malloc(sizeof(_dedx_lookup_data));
	if(temp->loaded_data[i] == NULL)
	{
		*err = 301;
		return NULL;
	}
    }
    temp->datasets = count;
    temp->active_datasets = 0;
    return temp;
}
void dedx_free_workspace(dedx_workspace * workspace, int *err)
{
	int i = 0;
	*err = 0;

	for(i = 0; i < workspace->datasets; i++)
	{
		free(workspace->loaded_data[i]);
	}
	free(workspace->loaded_data);
	free(workspace);
}
int _dedx_load_data(dedx_workspace * ws, stopping_data * data, float * energy, int prog, int * err)
{
    int active_dataset = ws->active_datasets;
    *err = 0;

    _dedx_calculate_coefficients(ws->loaded_data[active_dataset]->base, energy, data->data, data->length);
    ws->loaded_data[active_dataset]->acc.cache = 0;
    ws->loaded_data[active_dataset]->n = data->length;
    ws->loaded_data[active_dataset]->prog = prog;
    ws->loaded_data[active_dataset]->ion = data->ion;
    ws->loaded_data[active_dataset]->target = data->target;
    ws->loaded_data[active_dataset]->datapoints = data->length;

    //Increment reference Id
    ws->active_datasets++;
    return active_dataset;
}
/*Check the whether the energy are inside the boundery*/
int _dedx_check_energy_bounds(_dedx_lookup_data * data, float energy)
{
	int length = data->datapoints;
	float low = data->base[0].x;
	float high = data->base[length-1].x;
	if(energy < low || energy >high)
	{
		return 101;
	}
	return 0;
}

/*Return an explanation to the error code*/
void dedx_get_error_code(char *err_str, int err) {
	switch(err)
	{
		case 0:
			strcpy(err_str,"No error.");
			break;
		case 1:
			strcpy(err_str,"Composition file compos.txt does not exist.");
			break;
		case 2:
			strcpy(err_str,"MSTAR file mstar_gas_states.dat does not exist.");
			break;
		case 3:
			strcpy(err_str,"MSTAR effective_charge.dat file does not exist.");
			break;
		case 4:
			strcpy(err_str,"Unable to access binary data file.");
			break;
		case 5:
			strcpy(err_str,"Unable to access binary energy file.");
			break;
		case 6:
			strcpy(err_str,"Unable to write to disk.");
			break;
		case 7:
			strcpy(err_str,"Unable to read energy file.");
			break;
		case 8:
			strcpy(err_str,"Unable to read data file.");
			break;
		case 9:
			strcpy(err_str,"Unable to read short_names file.");
			break;
		case 10:
			strcpy(err_str,"Unable to read composition file.");
			break;
		case 11:
			strcpy(err_str,"Unable to read atima composition file.");
			break;
		case 101:
			strcpy(err_str,"Energy out of bounds.");
			break;
		case 201:
			strcpy(err_str,"Target is not in composition file.");
			break;
		case 202:
			strcpy(err_str,"Target and ion combination is not in data file.");
			break;
		case 203:
			strcpy(err_str,"ID does not exist.");
			break;
		case 204:
			strcpy(err_str,"Target is not an atomic element.");
			break;
		case 205:
			strcpy(err_str,"ESTAR is not implemented yet.");
			break;
		case 206:
			strcpy(err_str,"Ion is not supported for MSTAR.");
			break;
		case 207:
			strcpy(err_str,"Ion is not supported for requested table.");
			break; 
		case 208:
			strcpy(err_str,"Rho must be specified in this configuration.");
			break;
		case 209:
			strcpy(err_str,"ion_a must be specified in this configuration.");
			break;
		case 210:
			strcpy(err_str,"I value must be larger than zero.");
			break;
		case 301:
			strcpy(err_str,"Out of memory");
			break;  

		default:
			strcpy(err_str,"No such error code.");
			break;
	}
}

const char * dedx_get_program_name(int program)
{
	return dedx_program_table[program];
}

const char * dedx_get_program_version(int program)
{
	return dedx_program_version_table[program];
}

const char * dedx_get_material_name(int material)
{
	return dedx_material_table[material];
}

const char * dedx_get_ion_name(int ion)
{
	return dedx_ion_table[ion];
}

void dedx_get_version(int *major, int *minor, int *patch, int *svn)
{
	*major = DEDX_VERSION_MAJOR;
	*minor = DEDX_VERSION_MINOR;
	*patch = DEDX_VERSION_PATCH;
	*svn   = DEDX_VERSION_SVN;
	return;
}
void dedx_get_composition(int target, float composition[][2], unsigned int * comp_len, int *err)
{
	_dedx_get_composition(target, composition, comp_len, err);
}
float dedx_get_i_value(int target, int *err)
{
	return _dedx_get_i_value(target,DEDX_GAS,err);
}

const int * dedx_get_program_list(void) {
	/* returns a list of available programs, terminated with -1 */
	return dedx_available_programs;
}
const int * dedx_get_material_list(int program) {
  /* returns a list of available materials, terminated with -1 */
  if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT)
    return dedx_program_available_materials[0];
  else
    return dedx_program_available_materials[program];
}

const int * dedx_get_ion_list(int program) {
	/* returns a list of available ions, terminated with -1 */
  if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT) {  /* any ion, no restrictions */    
    static int temp[113],i; 
    for (i=0;i<112;i++)
      temp[i]=i+1;
    temp[112]=-1; // stopper
    return temp;  // TODO: Hey Jakob, er det her lovligt eller et nyt mem leak?
  }
  else
    return dedx_program_available_ions[program];
}

float dedx_get_min_energy(int program, int ion) {
	float energy_min = 0;

	switch(program) 
	{
		case DEDX_ASTAR:
			energy_min = 0.001 / 4.0;
			break;
		case DEDX_PSTAR:
			energy_min = 0.001;
			break;
		case DEDX_ESTAR:
			energy_min = 0.001;
			break;
		case DEDX_MSTAR:
			energy_min = 0.001;
			break;
		case DEDX_ICRU73_OLD:
			energy_min = _dedx_get_min_energy_icru(ion);
			break;   
		case DEDX_ICRU73:
			energy_min = _dedx_get_min_energy_icru(ion);
			break;
		case DEDX_ICRU49:
			energy_min = _dedx_get_min_energy_icru(ion);
			break;   
		case DEDX_ICRU:
			energy_min = _dedx_get_min_energy_icru(ion);
			break;
		case DEDX_DEFAULT:
			energy_min = 0.001;
			break;   
		case DEDX_BETHE_EXT00:
			energy_min = 0.001;
			break;   
	}  
	return energy_min;
}
float dedx_get_max_energy(int program, int ion) {
	float energy_max = 0;

	switch(program) 
	{
		case DEDX_ASTAR:
			energy_max = 1000.0 / 4.0;
			break;
		case DEDX_PSTAR:
			energy_max = 10000.0;
			break;
		case DEDX_ESTAR:
			energy_max = 10000.0;
			break;
		case DEDX_MSTAR:
			energy_max = 1000.0;
			break;
		case DEDX_ICRU73_OLD:
			energy_max = _dedx_get_max_energy_icru(ion);
			break;   
		case DEDX_ICRU73:
			energy_max = _dedx_get_max_energy_icru(ion);
			break;
		case DEDX_ICRU49:
			energy_max = _dedx_get_max_energy_icru(ion);
			break;   
		case DEDX_ICRU:
			energy_max = _dedx_get_max_energy_icru(ion);
			break;
		case DEDX_DEFAULT:
			energy_max = 1000.0;
			break;
		case DEDX_BETHE_EXT00:
			energy_max = 1000.0;
			break;   
	}  
	return energy_max;
}

float _dedx_get_min_energy_icru(int ion) {
	float _energy_min = 0;
	switch (ion) 
	{
		case 1:
			_energy_min = 0.001;
			break;
		case 2:
			_energy_min = 0.001/4.0;
			break;
		default:
			_energy_min = 0.025;
			break;
	}
	return _energy_min;
}

float _dedx_get_max_energy_icru(int ion) {
	float _energy_max = 0;
	switch (ion) 
	{
		case 1:
			_energy_max = 10000.0;
			break;
		case 2:
			_energy_max = 1000.0/4.0;
			break;
		default:
			_energy_max = 1000.0;
			break;
	}
	return _energy_max;
}

int _dedx_check_ion(int prog, int ion) {
	// checks if ion is availble in program. Returns 0 if yes, else -1.  
	const int *ion_list;
	int i = 0;

	if (prog >= DEDX_DEFAULT) {
		if ((ion < 1) || (ion > 120))
			return 0;
		else
			return 1;
	}

	ion_list = dedx_get_ion_list(prog);
	while (ion_list[i] != -1) {
		if (ion_list[i] == ion)      
			return 1;
		++i;
	}
	return 0;
}



void dedx_load_config(dedx_workspace *ws, dedx_config * config, int *err)
{
	int cfg_id;
	_dedx_validate_config(config,err);
	if(*err != 0)
		return;
	if(config->elements_id != NULL)
		cfg_id = _dedx_load_compound(ws,config,err);
	else
		cfg_id = _dedx_load_config_clean(ws,config,err);
	_dedx_set_names(config,err);
	config->cfg_id = cfg_id;
	config->loaded = 1;
}
int _dedx_load_config_clean(dedx_workspace *ws, dedx_config * config, int *err)
{
	float energy[_DEDX_MAXELEMENTS];
	int cfg;
	int prog = config->program;
	int ion = config->ion;
	int target = config->target;
	config->bragg_used = 0;
	*err = 0;
	stopping_data data;

	//check if ion is available in requested program
	if (!_dedx_check_ion(prog, ion)) {
		*err = 207;
		return -1;
	}
	config->_temp_i_value = config->i_value;
	//Load data
	_dedx_find_data(&data,config,energy,err);

	if(*err != 0)
	{
		//Check whether the error was that the combination wasn't in the data files and target is a compound

		if(*err == 202 && target > 99)
		{
			*err = 0;
			_dedx_evaluate_compound(config,err);
			if(*err != 0)
				return -1;
			if(config->elements_length == 0)
			{
				printf("error \n");
				*err = 201;
				return -1;
			}
			cfg = _dedx_load_compound(ws,config,err);
			if(*err != 0)
				return -1;
		}
		if(*err != 0)
		{
			return -1;
		}
	}
	else
		cfg = _dedx_load_data(ws,&data,energy,prog,err);
	return cfg; 
}

int _dedx_find_data(stopping_data * data,dedx_config * config,float * energy, int * err)
{
	int prog = config->program;
	int target = config->target;
	int ion = config->ion;

	int prog_load = prog;
	int ion_load = ion;
	int target_load = target;
	
	*err = 0;

	if(prog == DEDX_ICRU){
		if(ion == 1){
			prog_load = _DEDX_0008;
		}
		else if(ion == 2){
			prog_load = DEDX_ICRU49;
		}
		else{
			if(!(target == DEDX_WATER || target == DEDX_AIR)){
				prog_load = DEDX_ICRU73_OLD;
			}
			else{
				prog_load = DEDX_ICRU73;
			}
		}
	}
	else if(prog == DEDX_ICRU49){
		if(ion == 1){
			prog_load = _DEDX_0008;	
		}
		else if(ion == 2){
			prog_load = DEDX_ICRU49;
		}
		else{
			*err = 202;		
			return -1;
		}
	}

	//ESTAR not supported
	else if(prog == DEDX_ESTAR){
		*err = 205;
		return -1;
	}
	else if(prog == DEDX_ICRU73 && !(target == 276 || target == 104)){
		prog_load = DEDX_ICRU73_OLD;
	}
	else if(prog == DEDX_MSTAR && ion > 1) //Load ASTAR data, and scale them with the MSTAR method
	{
		ion_load = 2;
	}
	else  if(prog == DEDX_BETHE_EXT00 || prog == DEDX_DEFAULT) //Load bethe data
	{
		prog_load = 101;	
		int prog_temp= config->program;
		_dedx_read_energy_data(energy,prog_load, err);
		config->program = prog_load;
		_dedx_load_bethe_2(data,config, energy, err);
		config->program = prog_temp;
		return 0;
	}
	_dedx_read_binary_data(data, prog_load, ion_load, target_load, err);
	if(*err != 0)
		return -1;
	_dedx_read_energy_data(energy,prog_load, err);
	if(prog == DEDX_MSTAR){
		stopping_data out;
		char mode = 'b';
		if(config->mstar_mode != '\0')
		{
			mode = config->mstar_mode;
		}
		data->ion = config->ion;
		_dedx_convert_energy_to_mstar(data,&out,mode,config,energy);
		memcpy(data,&out,sizeof(stopping_data));
		data->ion = ion;
	}	
	return 0;
}
int _dedx_load_compound(dedx_workspace * ws, dedx_config * config, int * err)
{
	int i = 0;
	int j = 0;
	int length = config->elements_length;
	int * targets = config->elements_id;
	float * weight;
	float energy[_DEDX_MAXELEMENTS];
	float i_value;
	int target;
	stopping_data data;
	stopping_data * compound_data = malloc(sizeof(stopping_data)*length);

	*err = 0;

	if(compound_data == NULL)
	{
		*err = 301;
		return -1;
	}
	weight = config->elements_mass_fraction;
	//Backup i_value and target
	i_value = config->i_value;
	target = config->target;
	for(i = 0; i < length; i++)
	{
		config->target = targets[i];
		if(config->elements_i_value != NULL)
		{
			config->_temp_i_value = config->elements_i_value[i];
			if(config->elements_i_value[i] <= 0.0)
			{
				*err = 210;
				return -1; 
			} 	
		}	
		_dedx_find_data(&compound_data[i],config,energy,err);
		if(*err != 0)
		{
			free(compound_data);
			return -1;
		}
	}
	//Restore I_value and target
	config->i_value = i_value;
	config->target = target;	
	for(j = 0; j < compound_data[0].length; j++)
	{
		data.data[j] = 0.0;
		for(i = 0; i < length; i++)
		{
			data.data[j] += weight[i]*compound_data[i].data[j];
		}
	}
	data.length = compound_data[0].length;
	free(compound_data);	
	return _dedx_load_data(ws,&data,energy,config->program,err);
}

int _dedx_load_bethe_2(stopping_data * data, dedx_config * config, float * energy, int * err)
{
	int i = 0;
	*err = 0;
	if(config->target > 99)
	{
		*err = 202;
		return -1;
	}
	float PZ, PA, TZ, TA, rho, pot;
	PZ = _dedx_get_atom_charge(config->ion,err);
	PA = _dedx_get_atom_mass(config->ion,err);
	TZ = _dedx_get_atom_charge(config->target,err);
	TA = _dedx_get_atom_mass(config->target,err);
	rho = config->rho;
	pot = config->_temp_i_value;
	data->length = 122;
	//Get energy grid.
	_dedx_read_energy_data(energy,DEDX_BETHE_EXT00, err);
	if(*err != 0)
		return 0;
	//Fill the data grid with values.
	_dedx_bethe_coll_struct * bethe = (_dedx_bethe_coll_struct *)calloc(1,sizeof(_dedx_bethe_coll_struct));
	for(i = 0; i < data->length; i++)
	{
		data->data[i] = _dedx_calculate_bethe_energy(bethe,energy[i], PZ, PA, TZ, TA, rho, pot);
	}
	if(bethe->bet != NULL)
		free(bethe->bet);
	if(bethe->gold)
		free(bethe->gold);
	free(bethe);
	return 0;
}
int _dedx_load_atima(stopping_data * data, dedx_config * config, float * energy, int * err)
{
        *err = 0;
	return 0;
}



float dedx_get_stp(dedx_workspace * ws, dedx_config * config, float energy, int * err)
{	
	int id = config->cfg_id;
	*err = 0;
	//Check that the energy is inside the boundery
	if((*err = _dedx_check_energy_bounds(ws->loaded_data[id],energy)) != 0)
		return 0;
	if(id > ws->active_datasets) //Check that the dataset is loaded.
	{
		*err = 203;
		return 0;
	}
	//Evaluating the spline function
	return _dedx_evaluate_spline(ws->loaded_data[id]->base, energy, 
			&(ws->loaded_data[id]->acc), 
			ws->loaded_data[id]->n);
}
void dedx_free_config(dedx_config * config, int *err)
{
	if(config != NULL)
	{
		if(config->elements_id != NULL)
			free(config->elements_id);
		if(config->elements_atoms != NULL)
			free(config->elements_atoms);
		if(config->elements_mass_fraction != NULL)
			free(config->elements_mass_fraction);
		if(config->elements_i_value != NULL)
			free(config->elements_i_value);
		free(config);
	}
	*err = 0;
}

float dedx_get_simple_stp(int ion, int target, float energy, int *err)
{
	dedx_config *config = (dedx_config *)calloc(1,sizeof(dedx_config));
	float stp;
	config->target = target;
	config->ion = ion;
	config->program = DEDX_ICRU;
	dedx_workspace *ws = dedx_allocate_workspace(1,err);
	if(*err != 0)
		return 0;
	dedx_load_config(ws,config,err);
	if(*err != 0)
	{
		dedx_free_config(config,err);
		config = (dedx_config *)calloc(1,sizeof(dedx_config));
		config->ion = ion;
		config->target = target;
		config->program = 100;
		dedx_load_config(ws,config,err);
		if(*err != 0)
			return 0;
	}	
	stp = dedx_get_stp(ws,config,energy,err);
	if(*err != 0)
		return 0;
	dedx_free_config(config,err);
	dedx_free_workspace(ws,err);		
	return stp;
}
