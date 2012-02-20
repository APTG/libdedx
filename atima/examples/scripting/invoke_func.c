#include "invoke_func.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct func_struct func_item;
typedef struct data_struct data_item;

func_item * func_head = NULL;
data_item * data_head = NULL;
FILE *fp_out;

int write_line(char * str);

int get_stp(data_item * config, data_item * energy, float * stp);

int config(char * params);
int clone(char * params);
int set(char * params);
int energy(char * params);
int print(char * params);

int print_list(char * text, int argv, char ** args);
int print_values(char * text, int items_n, data_item *** items);
int replace_signs(char * str);


int set_config(dedx_config *config, char * params);
int set_config_value(dedx_config * config, char * name, char * value);
int set_config_list(dedx_config * config, char * name, char * list);

int set_energy(float ** energy,int * n,char * params);


int clone_config(data_item * a, data_item *b);
int clone_energy(data_item * a, data_item *b);
int clone_dedx_config(dedx_config * a, dedx_config * b);

data_item * create_data_item(char * name);
data_item * get_data_item(char * name);
int get_data_type(data_item * item);
char * get_variable_name(char * params, int * offset);

void add_function(char * cmd, int (*func_p)(char * params));
int set_output(FILE * out)
{
	fp_out = out;	
	return 0;
}
int write_line(char * str)
{
	fprintf(fp_out,"%s",str);
	return 0;
}

int get_stp(data_item * config, data_item * energy, float * stp)
{
	int err = 0;
	int i;
	clone_dedx_config(config->config_temp,config->config);
	if(config->ws != NULL)
		dedx_free_workspace(config->ws,&err);
	config->ws = dedx_allocate_workspace(1,&err);
	dedx_load_config2(config->ws,config->config_temp,&err);
	if(err != 0)
		return -1;
	for(i = 0; i < energy->e_length; i++)
	{
		stp[i] = dedx_get_stp(config->ws,0,energy->energy[i],&err);
	}
	
	return 0;
}
data_item * create_data_item(char * name)
{
	data_item * curr = get_data_item(name);
	if(curr != NULL)
	{
		fprintf(stderr,"Variable already defined");
		return NULL;
	}
	curr = (data_item *)malloc(sizeof(data_item));
	strcpy(curr->name,name);
	curr->next = data_head;
	data_head = curr;
	return curr;
	
}
data_item * get_data_item(char * name)
{
	data_item * curr = data_head;
	while(curr != NULL)
	{
		if(strcmp(curr->name,name) == 0)
		{
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}
int get_data_type(data_item * item)
{
	return item->type;
}
char * get_variable_name(char * params, int * offset)
{
	char * name = (char *)calloc(20,sizeof(char));
	int i = 0;
	int j = 0;
	while(params[i] != '=' && params[i] != ';' && params[i] != '.')
	{
		if(params[i] != ' ')
			name[j++] = params[i]; 
		i++; 
	}
	*offset = i+1;
 	return name;		
}
int build_functions()
{
	add_function("CONFIG",&config);
	add_function("CLONE",&clone);
	add_function("SET",&set);
	add_function("ENERGY",&energy);
	add_function("PRINT",&print);
	return 0;
}
int invoke_function(char * cmd, char * params)
{
	func_item * curr = func_head;
	do
	{
		if(strcmp(curr->cmd,cmd) == 0)
		{
			curr->func_p(params);
			break;
		}
		curr = curr->next;
	}
	while(curr != NULL);
	return -1;	
}
void add_function(char * cmd, int (*func_p)(char * params))
{
	func_item * curr = (func_item *)malloc(sizeof(func_item));
	strcpy(curr->cmd,cmd);
	curr->func_p = func_p;
	curr->next = func_head;
	func_head = curr;
}
int clone(char * params)
{
	char a[20];
	char b[20];
	char shift = 0;
	int i = 0;
	int j = 0;
	data_item * item_a;
	data_item * item_b;
	while(params[i] != ';' && params[i] != ')')
	{
		if(params[i] != ' ' && params[i] != '(')
		{
			if(params[i] == ',')
			{
				a[j] = '\0';
				j = 0;
				shift = 1;
			}
			else if(shift == 0)
			{
				a[j++] = params[i];
			}
			else
			{
				b[j++] = params[i];
			}
		}
i++;
	}
	b[j] = '\0';
	item_a = get_data_item(a);
	item_b = get_data_item(b);
	if(item_a == NULL || item_b == NULL)
	{
		fprintf(stderr,"Both Variables should be defined before use clone");
		return -1;
	}
	if(get_data_type(item_a) != get_data_type(item_b))
	{
		fprintf(stderr,"Both variables should be same type with clone");
		return -1;
	}
	switch(get_data_type(item_a))
	{
		case DEDX_SCRIPT_CONFIG:
			clone_config(item_a,item_b);
			break;	
		case DEDX_SCRIPT_ENERGY:
			clone_energy(item_a,item_b);
			break;
	}	
	return 0;
}
int clone_config(data_item * a, data_item *b)
{
	memcpy(a->config,b->config,sizeof(dedx_config));
	dedx_config * a_conf = a->config;
	dedx_config * b_conf = b->config;
	clone_dedx_config(a_conf,b_conf);
	return 0;
}
int clone_dedx_config(dedx_config * a_conf, dedx_config * b_conf)
{
	memcpy(a_conf,b_conf,sizeof(dedx_config));
	int length = b_conf->elements_length;
	if(b_conf->elements_id != NULL)
	{
		a_conf->elements_id = (int *)malloc(sizeof(int)*length);
		memcpy(a_conf->elements_id,b_conf->elements_id,sizeof(int)*length);
	}	
	if(b_conf->elements_atoms != NULL)
	{
		a_conf->elements_atoms = (int *)malloc(sizeof(int)*length);
		memcpy(a_conf->elements_atoms,b_conf->elements_atoms,sizeof(int)*length);
	}
	if(b_conf->elements_mass_fraction != NULL)
	{
		a_conf->elements_mass_fraction = (float *)malloc(sizeof(float)*length);
		memcpy(a_conf->elements_mass_fraction,b_conf->elements_mass_fraction,sizeof(float)*length);
	}
	if(b_conf->elements_i_value != NULL)
	{
		a_conf->elements_i_value = (float *)malloc(sizeof(float)*length);
		memcpy(a_conf->elements_i_value,b_conf->elements_i_value,sizeof(float)*length);
	}
	return 0;

}
int clone_energy(data_item * a, data_item * b)
{
	a->energy = malloc(sizeof(float)*b->e_length);
	b->e_length = a->e_length;
	memcpy(a->energy,b->energy,sizeof(float)*b->e_length);
	return 0;
}
int set(char * params)
{	
	int i = 0;
	int offset = 0;
	int offset2 = 0;
	char * name = get_variable_name(params,&offset);
	char * variable;
	char values[256];
	data_item * item = get_data_item(name);
	if(item == NULL)
	{
		fprintf(stderr,"Varibles does not exist");
		return -1;
	}
	if(params[offset-1] == '.')
	{
		variable = get_variable_name(&params[offset],&offset2);
		offset += offset2;
		char * tmp = &params[offset];
		while(tmp[i] != ';')
		{
			values[i] = tmp[i];
			i++;
		}
		values[i] = '\0';
		set_config_value(item->config,variable,values);
	}
	else
	{
		set_config(item->config,&params[offset]);
	}
	return 0;
}
int energy(char * params)
{
	int offset = 0;
	char * name = get_variable_name(params,&offset);
	data_item * item = create_data_item(name);
	if(item == NULL)
		return -1;
	item->type = DEDX_SCRIPT_ENERGY;
	if(params[offset-1] == ';')
		return 0;
	set_energy(&(item->energy),&(item->e_length),&params[offset]);
	return 0;
}
int set_energy(float ** energy,int * n, char * params)
{
	int i;
	int j = 0;
	int items = 0;
	i = 1;	
	if(params[0] == '{')
	{
		char list[100][10];
		while(params[i] != '}')
		{
			if(params[i] == ',')
			{
				list[items][j] = '\0';
				items++;
				j = 0;
			} 
			else
			{
				list[items][j++] = params[i];
			}
			i++;
		}
		if(list[items][0] != '\0')
		{
			list[items][j] = '\0';
			items++;
		}
		*energy = (float *)malloc(sizeof(float)*items);
		for(i = 0; i < items; i++)
		{
			(*energy)[i] = atof(list[i]);
		}
	}
	*n = items;
	return 0;
}
int print(char * params)
{
	int offset = 0;
	int i;
	int j;
	int items = 0;
	char text[80];
	char pure_text = 1;
	char ** list = (char **)calloc(10,sizeof(char *));
	for(i = 0; i < 10; i++)
		list[i] = (char *)calloc(10,sizeof(char));
	i = 2;
	j = 0;
	while(params[i] != '"')
	{
		if(params[i] == '%')
			pure_text = 0;
		text[j++] = params[i++];
	}
	offset = i+1;
	text[j] = '\0';
	replace_signs(text);
	if(pure_text == 0)
	{
		if(params[offset] != ',')
			return -1;
		i = offset + 1;
		j = 0;
		while(params[i] != ';')
		{
			if(params[i] == ',')
			{
				list[items][j] = '\0';
				j = 0;
				items++;
			}	
			else
			{
				list[items][j++] = params[i];
			}
			i++;
		}
		if(list[items][0] != '\0')
		{
			list[items][j-1] = '\0';
			items++;
		}
		print_list(text,items,list);
	}
	else
		write_line(text);
	return 0;
}
int replace_signs(char * str)
{
	int i = 0;
	int j = 0;
	while(str[i] != '\0')
	{
		if(str[j] == '\\')
		{
			switch(str[j+1])
			{
				case 'n':
					str[i] = '\n';
					break;
				case 'r':
					str[i] = '\r';
					break;
				case 't':
					str[i] = '\t';
					break;
			}
			j++;
		}
		else
		{
			str[i] = str[j]; 
		}
		i++;
		j++;
	}
	return 0;
}
int print_list(char * text, int argv, char ** args)
{
	int i,j,k;

	data_item *** items = (data_item ***)malloc(sizeof(data_item **)*argv);
	for(i = 0; i < argv; i++)
		items[i] = (data_item **)malloc(sizeof(data_item *)*2);
	char a[20];
	char b[20];

	char * tmp;
	int argument;
	for(k = 0; k < argv; k++)
	{
		tmp = args[k];
		argument = 0;
		i = 0;
		j = 0;
		while(tmp[i] != '\0')
		{
			if(argument == 0 && tmp[i] != '(')
				a[j++] = tmp[i];
			else if(tmp[i] == ')')
			{
				break;
			}
			else if(tmp[i] == '(')
			{
				argument = 1;
				a[j] = '\0';
				j = 0;
			}
			else
			{
				b[j++] = tmp[i];
			}
			i++;
		}
		if(argument == 0)
			a[j] = '\0';
		b[j] = '\0';
		items[k][0] = get_data_item(a);
		if(argument != 0)
		{
			items[k][0] = get_data_item(b);
			items[k][1] = get_data_item(a);
		}
		else
		{
			items[k][0] = get_data_item(a);
		}
		if(items[k][0]->type != DEDX_SCRIPT_ENERGY)
			return -1;
	}
	print_values(text,argv,items);	
	return 0;
}
int print_values(char * text, int items_n, data_item *** items)
{
	int i,j,k;
	int dim = 0;
	int tmp_dim = 0;
	float ** data;
	char output[100];
	char tmp[10];
	for(i = 0; i < items_n; i++)
	{
		tmp_dim = items[i][0]->e_length;
		if(i != 0 && tmp_dim != dim)
		{
			fprintf(stderr,"Print out arguments should have same dimension");
			return -1;
		}
		else
		{
			dim = tmp_dim;
		}
	}	
	data = (float **)malloc(sizeof(float *)*items_n);
	for(i = 0; i < items_n; i++)
	{
		if(items[i][1] == NULL)
		{
			data[i] = items[i][0]->energy;

		}
		else
		{
			data[i] = (float *)malloc(sizeof(float)*items[i][0]->e_length);
			get_stp(items[i][1],items[i][0],data[i]);
		}
	}
	for(i = 0; i < dim; i++)
	{
		j = 0;
		k = 0;
		int l;
		int num;

		while(text[j] != '\0')
		{
			if(text[j] == '%')
			{
				num = atoi(&(text[j+1]));
				sprintf(tmp,"%6.4E%c",data[num][i],'\0');
				l = 0;
				while(tmp[l] != '\0')
				{	
					output[k++] = tmp[l++];
				}		
				j++;
			}
			else
			{	
				output[k++] = text[j];
			}
			j++;
		}
		write_line(output);		
	}	
	return 0;
}
int config(char * params)
{
	int offset = 0;
	char * name;
	name = get_variable_name(params,&offset);
	data_item * item = create_data_item(name);
	if(item == NULL)
		return -1;
	item->config = calloc(1,sizeof(dedx_config));
	item->config_temp = calloc(1,sizeof(dedx_config));
	item->type = DEDX_SCRIPT_CONFIG;

	if(params[offset-1] == ';')
		return 0;
	else if(params[offset-1] == '=')
	{
		set_config(item->config,&params[offset]);
	}
	return 0;
}
int set_config_value(dedx_config * config, char * name, char * value)
{
	if(strcmp("prog",name) == 0)
	{
		config->prog = atoi(value);
	}
	else if(strcmp("ion",name) == 0)
	{
		config->ion = atoi(value);
	}
	else if(strcmp("target",name) == 0)
	{
		config->target = atoi(value);
	}
	else if(strcmp("i_value",name) == 0)
	{
		config->target = atof(value);
	}
	else if(strcmp("compound_state",name) == 0)
	{
		config->compound_state = atoi(value);
	}
	else if(strcmp("mstar_mode",name) == 0)
	{
		config->mstar_mode = atoi(value);
	}
	return 0;	
}
int set_config_list(dedx_config * config, char * name, char * list)
{
	char params[10][20];
	int i = 0;
	int j = 0;
	int items = 0;
	float * f_temp;
	int * i_temp;
	while(list[i] != '\0')
	{
		if(list[i] == ',')
		{
			params[items][j] = '\0';
			items++;
			j = 0;
		}
		else
		{
			params[items][j++] = list[i];
		}
		i++;
	}
	if(params[items][0] != '\0')
		items++;
	if(strcmp("elements_id",name) == 0)
	{
		config->elements_id = malloc(sizeof(int)*items);
		i_temp = config->elements_id;
	}
	else if(strcmp("elements_atoms",name) == 0)
	{
		config->elements_atoms = malloc(sizeof(int)*items);
		i_temp = config->elements_atoms;
	}
	else if(strcmp("elements_mass_fraction",name) == 0)
	{
		config->elements_mass_fraction = malloc(sizeof(float)*items);
		f_temp = config->elements_mass_fraction;
	}
	else if(strcmp("elements_i_value",name) == 0)
	{
		config->elements_i_value = malloc(sizeof(float)*items);
		f_temp = config->elements_i_value;
	}
	else 
		return -1;
	for(i = 0; i < items; i++)
	{
		if(i_temp != NULL)
		{
			i_temp[i] = atoi(params[i]);
		}
		else
		{
			f_temp[i] = atof(params[i]);
		}
	}
	config->elements_length = items;
	return 0;
}
int set_config(dedx_config *config, char * params)
{
	int i = 1;
	int j = 0;
	int variable = 1;
	int list = 0;
	int is_list = 0;
	char name[20];
	char value[20];
	if(params[0] != '{')
		return -1;
	while(params[i] != ';')
	{
		if(variable)
		{
			if(params[i] == '=')
			{
				variable = 0;
				name[j] = '\0';
				j = 0;
			}	
			else
			{
				name[j++] = params[i];
			}
		}
		else
		{
			if(params[i] == '{')
			{
				list = 1;
			}
			else if(list == 1 && params[i] == '}')
			{
				list = 0;
				is_list = 1;	
			}
			else if(list == 0 && (params[i] == ',' || params[i] == '}'))
			{
				value[j] = '\0';
				variable = 1;
				list = 0;
				j = 0;
				//execute
				if(is_list == 1)
					set_config_list(config,name,value);
				else
					set_config_value(config,name,value);
				is_list = 0;
			}
			else
			{
				value[j++] = params[i];
			}
		}
		i++;
	}	
	return 0;
}
