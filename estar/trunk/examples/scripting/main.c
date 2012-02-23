#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "invoke_func.h"

const int read_c = 255;

int parser(FILE * in, FILE * out);
int clean(FILE *in, FILE *out);

int main(int argv, char * argc[])
{
	char * input_file;
	char * output_file;
	if(argv < 2)
	{
		printf("Input file should be specified\n");
		return -1;
	}
	input_file = argc[1];
	if(argv == 3)
		output_file = argc[2];
	else
		output_file = "libdedx_out";
	FILE *in = fopen(input_file,"r");
	FILE *temp = fopen("lib.temp","w+");
	FILE *out = fopen(output_file,"w+");
	if(in == NULL)
	{
		printf("Input file does not exist\n");
		return -1;
	}

	clean(in,temp);
	fclose(temp);
	fclose(in);


	//Open clean file
	in = fopen("lib.temp","r");
	parser(in,out);	
	fclose(in);
	fclose(out);
	return 0;
}
int parser(FILE *in, FILE *out)
{
	build_functions();
	set_output(out);
 	char line[read_c];
	int i;
	char cmd[20];
	fgets(line,read_c,in);
	while(!feof(in))
	{
		i = 0;
		while(line[i] != '(' && line[i] != ' ')
		{
			cmd[i] = line[i];
			cmd[i+1] = '\0';
			i++; 
		}
		invoke_function(cmd,&line[i]);
		fgets(line,read_c,in);
	}	
	return 0;
}
/*Remove Comments, lineshift, tabs, white spaces, */
int clean(FILE *in, FILE *out)
{
	char line[read_c];
	char output[read_c];
	int i = 0;
	int j = 0;
	char comment = 0;
	char string = 0;
	char space = 0;
	fgets(line,read_c,in);
	while(!feof(in))
	{
		i = 0;
		while(line[i] != '\0')
		{
			if(comment == 0)
			{ 
				if(line[i] == '\n' || line[i] == '\r')
					break;
				if(line[i] == '\t')
				{
					i++; 
					continue;
				}
				if(strncmp(&line[i],"/*",2) == 0)
				{
					comment = 1;
					i++;
					continue;
				}
				if(strncmp(&line[i],"//",2) == 0)
					break;
				if(string != 0)
				{
					if(line[i] == '"')
						string = 0;
				}
				else
				{
					if(line[i] == '"')
						string = 1;
					if(line[i] == ' ')
					{
						if(space == 0 && j != 0)
							space = 1;
						else
						{
							i++;
							continue;
						}
					}
				}
				output[j++] = line[i];
				if(line[i] == ';')
				{
					output[j] = '\0';
					fprintf(out,"%s\n",output);
					output[0] = '\0';
					j = 0;
					space = 0;
				}
			}
			else
			{
				if(strncmp(&line[i],"*/",2) == 0)
				{
					comment = 0;
					i++;
				}
			}
			i++;	
		}
		fgets(line,read_c,in);
	}
	return 0;
}

