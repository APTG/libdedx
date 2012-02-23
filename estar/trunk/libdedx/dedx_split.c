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
#include <stdlib.h>
#include "dedx_split.h"
char **_dedx_split(char* string, char splitCaracter, unsigned int * items, unsigned int strLength)
{
    int i = 0;
    int j = 0;
    char **words;
    words = malloc(10*sizeof(char *));
    *items = 0;
    char word[20] = "";

    for(i = 0; i < 10; i++)
    {
        words[i] = (char *)malloc(20*sizeof(char));
    }

    for(i = 0; i < strLength; i++)
    {

        if(string[i] == splitCaracter)
        {

            for(j=0; j < 20; j++)
            {
                words[*items][j] = word[j];
                word[j]='\0';
            }
            (*items)++;
            j = 0;
        }
        else if(string[i] == '\0')
        {
            break;
        }
        else
        {
            word[j++] = string[i];
        }
    }

    for(j=0; j < 20; j++)
    {
        words[*items][j] = word[j];
        word[j]='\0';
    }
    (*items)++;
    return words;
}
void _dedx_free_split_temp(char ** temp)
{
	int i = 0;
	for(i = 0; i < 10; i++)
		free(temp[i]);
	free(temp);
}
