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
#include "dedx_split.h"

#include <stdlib.h>

char **dedx_internal_split(char *string, char split_character, unsigned int *items, unsigned int str_length) {
    int i = 0;
    int j = 0;
    char **words = (char **) malloc(10 * sizeof(char *));
    *items = 0;
    char word[20] = "";

    if (words == NULL)
        return NULL;

    for (i = 0; i < 10; i++) {
        words[i] = malloc(20 * sizeof(char));
        if (words[i] == NULL) {
            dedx_internal_free_split_temp(words, i);
            return NULL;
        }
    }

    for (i = 0; i < str_length; i++) {

        if (string[i] == split_character) {

            for (j = 0; j < 20; j++) {
                words[*items][j] = word[j];
                word[j] = '\0';
            }
            (*items)++;
            j = 0;
        } else if (string[i] == '\0') {
            break;
        } else {
            word[j++] = string[i];
        }
    }

    for (j = 0; j < 20; j++) {
        words[*items][j] = word[j];
        word[j] = '\0';
    }
    (*items)++;
    return words;
}

void dedx_internal_free_split_temp(char **temp, unsigned int count) {
    unsigned int i = 0;
    if (temp == NULL)
        return;
    for (i = 0; i < count; i++)
        free(temp[i]);
    free((void *) temp);
}
