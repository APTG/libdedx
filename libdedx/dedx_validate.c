#include "dedx_validate.h"
#include "dedx_file_access.h"
#include <math.h>
#include "dedx_periodic_table.h"


int _dedx_set_names(dedx_config * config, int *err)
{
	if(config->target != 0)
		config->target_name = dedx_get_material_name(config->target);
	
	config->ion_name = dedx_get_ion_name(config->ion);
	config->program_name = dedx_get_program_name(config->program);
	return 0;
}
int _dedx_validate_rho(dedx_config * config, int *err)
{
	if(config->rho <= 0.0 && config->target != 0)	
	{
		config->rho = _dedx_read_density(config->target,err); 
	}
	else if(config->rho <= 0.0 && config->target == 0 && config->program >= 100)
	{
		*err = 208;
	}
	return 0;
}
int _dedx_evaluate_i_pot(dedx_config * config, int *err)
{
	int i;
	if(config->elements_i_value == NULL && config->target != 0)
	{
		if(config->i_value == 0.0)
		{
			config->i_value = _dedx_get_i_value(config->target,config->compound_state,err);
		}
		if(*err != 0)
			return -1;
	}
	else if(config->i_value == 0.0 && config->target == 0 && config->elements_i_value == NULL)
	{
		config->elements_i_value = calloc(config->elements_length,sizeof(float));
		for(i = 0; i < config->elements_length; i++)
		{
			config->elements_i_value[i] = _dedx_get_i_value(config->elements_id[i],config->compound_state,err);
		}
	}
	if(config->elements_id != NULL && config->elements_i_value == NULL)
	{
		_dedx_calculate_element_i_pot(config,err);
	}
	if(config->elements_i_value != NULL && config->i_value == 0.0)
	{
		float charge_avg = 0.0;
		for(i = 0; i < config->elements_length; i++)
		{
			config->i_value += config->elements_mass_fraction[i]*log(config->elements_i_value[i])*config->elements_id[i]/_dedx_get_atom_mass(config->elements_id[i],err);
			charge_avg += config->elements_mass_fraction[i]*config->elements_id[i]/_dedx_get_atom_mass(config->elements_id[i],err);
		}
		config->i_value = exp(config->i_value/charge_avg);
	}	
	return 0;
}
int _dedx_evaluate_compound(dedx_config * config,int *err)
{	
	int i = 0;
	if(config->target > 0 && config->target <= 99)
	{
		return 0;
	}
	config->bragg_used = 1;
	//_evaluate_compound_state_mstar(config,err);	
	if(config->elements_id == NULL)
	{
		unsigned int compos_len;
		float composition[20][2];
		_dedx_get_composition(config->target, composition, &compos_len, err);
		if(*err != 0)
			return -1;
		if(compos_len == 0)
		{
			*err = 201;
			return -1;
		}
		config->elements_id = (int *)malloc(sizeof(int)*compos_len);
		config->elements_mass_fraction = (float *)malloc(sizeof(float)*compos_len);
		for(i= 0; i < compos_len; i++)
		{
			config->elements_id[i] = (int)composition[i][0];
			config->elements_mass_fraction[i] = composition[i][1];
		}
		config->elements_length = compos_len;
	}
	else if(config->elements_mass_fraction == NULL && config->elements_atoms != NULL)
	{
		int length = config->elements_length;
		int * compos = config->elements_atoms;
		float * density = malloc(sizeof(float)*length);
		float * weight = malloc(sizeof(float)*length);
		float f, sum = 0;
		for(i = 0; i < length; i++)
		{
			f = _dedx_get_atom_mass(config->elements_id[i],err);
			if(*err != 0)
			{
				free(density);
				free(weight);
				return -1;
			}
			density[i] = f;
			sum += compos[i]*f;
		}

		for(i = 0; i < length; i++)
		{
			weight[i] = compos[i]*density[i]/sum;
		}
		free(density);
		config->elements_mass_fraction = weight;
	}
	else
	{
		return -1;
	}
	return 0;
}
int _dedx_validate_config(dedx_config * config,int *err)
{
	_dedx_validate_rho(config,err);
	if(*err != 0)
		return -1;
	if(config->program >= 100)
	{
		//Order is important
		_dedx_evaluate_compound(config,err);
		_dedx_validate_state(config,err);
		_dedx_evaluate_i_pot(config,err);	
	}
	if(config->target == 0 && config->elements_id != NULL)
	{
		_dedx_evaluate_compound(config,err);
	}
	return 0;
}
int _dedx_validate_state(dedx_config *config,int *err)
{
	if(config->compound_state == DEDX_DEFAULT_STATE)
	{
		if(_dedx_target_is_gas(config->target,err))
		{
			config->compound_state = DEDX_GAS;		
		}	
		else
		{
			config->compound_state = DEDX_CONDENSED;
		}	
	}
	return 0;
}
int _dedx_calculate_element_i_pot(dedx_config * config,int *err)
{
	int i;
	float charge_avg = 0;
	float avg_pot = 0;
	float log_x,i_pot_x;
	int target;
	for(i = 0; i < config->elements_length; i++)
	{
		target = config->elements_id[i];
		charge_avg += config->elements_mass_fraction[i]*target/_dedx_get_atom_mass(target,err);
		avg_pot += config->elements_mass_fraction[i]*target/_dedx_get_atom_mass(target,err)*log(_dedx_get_i_value(target,config->compound_state,err));
	}
	log_x = log(config->i_value);
	log_x -= avg_pot/charge_avg;
	i_pot_x = exp(log_x);
	config->elements_i_value = (float *)malloc(sizeof(float)*config->elements_length);
	for(i = 0; i < config->elements_length; i++)
	{
		config->elements_i_value[i] = _dedx_get_i_value(config->elements_id[i],config->compound_state,err)*i_pot_x;
	}
	return 0;

}
