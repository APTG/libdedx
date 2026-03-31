/*
    This file is part of libdedx.

    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dedx_file_access.h"

/* Resolve the data directory once per process.
 * Prefers DEDX_DATA_PATH_LOCAL (build tree) if it contains the sentinel file,
 * otherwise falls back to DEDX_DATA_PATH (install prefix).
 * Result is cached in a static buffer; never changes after the first call. */
static const char *_dedx_get_data_path(void) {
    static char resolved[DEDX_PATH_SIZE];
    static int done = 0;
    if (!done) {
        struct stat st;
        char probe[DEDX_PATH_SIZE];
        strcpy(probe, DEDX_DATA_PATH_LOCAL);
        strcat(probe, "icru_pstarEng.dat");
        if (stat(probe, &st) == 0)
            strcpy(resolved, DEDX_DATA_PATH_LOCAL);
        else
            strcpy(resolved, DEDX_DATA_PATH);
        done = 1;
    }
    return resolved;
}

void _dedx_convert_to_binary(char *path, char *output, int *err) {
    *err = 0;
    FILE *fp = fopen(path, "r");
    FILE *out = fopen(output, "wb+");
    char line[100];
    int datalines = 0;
    float data[_DEDX_MAXELEMENTS];
    char **temp = NULL;
    stopping_data container;
    if (fp == NULL || out == NULL) {
        if (out == NULL)
            *err = 6;
        else
            *err = 8;
        return;
    }
    while (!feof(fp)) {
        if (fgets(line, 100, fp) == NULL) {
        }
        if (line[0] == '#') {

            unsigned int length = 0;
            memset(&data, 0, _DEDX_MAXELEMENTS);
            temp = _dedx_split(line, ':', &length, 100);
            temp[0][0] = ' ';
            int i = 0;
            datalines = atoi(temp[2]);
            while (i++ < datalines && !feof(fp)) {
                if (fgets(line, 100, fp) == NULL) {
                }
                data[i - 1] = atof(line);
            }
            container.target = atoi(temp[0]);
            container.ion = atoi(temp[1]);
            container.length = datalines;
            memcpy(&container.data, &data, _DEDX_MAXELEMENTS * sizeof(float));
            fwrite(&container, sizeof(container), 1, out);
        }
    }
    free(temp);
    fclose(fp);
    fclose(out);
}

void _dedx_read_binary_data(stopping_data *data, int prog, int ion, int target, int *err) {
    *err = 0;
    const char *folder = _dedx_get_data_path();
    char path[DEDX_PATH_SIZE];
    path[0] = '\0';
    strcat(path, folder);
    strcat(path, _dedx_get_program_file(prog));
    strcat(path, ".bin");

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        char input_path[DEDX_PATH_SIZE];
        input_path[0] = '\0';
        strcat(input_path, folder);
        strcat(input_path, _dedx_get_program_file(prog));
        strcat(input_path, ".dat");
        _dedx_convert_to_binary(input_path, path, err);
        if (*err != 0) {
            *err = 4;
            return;
        }
        fp = fopen(path, "rb");
        if (fp == NULL) {
            *err = 4;
            return;
        }
    }
    stopping_data dat;
    *err = 202;
    while (fread(&dat, sizeof(stopping_data), 1, fp) == 1) {
        if (dat.target == target && dat.ion == ion) {
            memcpy(data, &dat, sizeof(stopping_data));
            *err = 0;
        }
    }
    fclose(fp);
}

void _dedx_convert_energy_binary(char *path, char *output, int *err) {
    *err = 0;
    FILE *fp = fopen(path, "r");
    // TODO: Next line wont work after installation, unless user is root.
    FILE *out = fopen(output, "wb+");
    char line[100];
    int datalines;
    int i;
    float data[_DEDX_MAXELEMENTS];
    if (fp == NULL || out == NULL) {

        if (out == NULL)
            *err = 6;
        else
            *err = 7;
        return;
    }

    if (fgets(line, 100, fp) == NULL) {
    }
    datalines = atoi(line);

    for (i = 0; i < datalines; i++) {
        if (fgets(line, 100, fp) != NULL) {
        };
        data[i] = atof(line);
    }
    fwrite(&data, sizeof(data), 1, out);
    fclose(fp);
    fclose(out);
}

void _dedx_read_energy_data(float *energy, int prog, int *err) {
    *err = 0;
    const char *folder = _dedx_get_data_path();
    char path[DEDX_PATH_SIZE];
    path[0] = '\0';
    strcat(path, folder);
    strcat(path, _dedx_get_energy_file(prog));
    strcat(path, ".bin");

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        char input_path[DEDX_PATH_SIZE];
        input_path[0] = '\0';
        strcat(input_path, folder);
        strcat(input_path, _dedx_get_energy_file(prog));
        strcat(input_path, ".dat");
        _dedx_convert_energy_binary(input_path, path, err);
        if (*err != 0) {
            *err = 5;
            return;
        }
        fp = fopen(path, "rb");
        if (fp == NULL) {
            *err = 5;
            return;
        }
    }
    if (fread(energy, sizeof(float) * _DEDX_MAXELEMENTS, 1, fp) == 0) {
    }
    fclose(fp);
}

float _dedx_read_effective_charge(int id, int *err) {
    *err = 0;
    float charge = -1;
    if (id < 99) {
        charge = (float) id;
        return charge;
    }
    const char *folder = _dedx_get_data_path();
    char line[100];
    char **temp = NULL;
    unsigned int items = 0;
    char file[] = "effective_charge.dat";
    char path[DEDX_PATH_SIZE];
    strcpy(path, folder);
    strcat(path, file);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        *err = 3;
        return 0;
    }

    while (!feof(fp)) {

        if (fgets(line, 100, fp) != NULL) {
        }
        temp = _dedx_split(line, '\t', &items, 100);
        if (atoi(temp[0]) == id) {
            charge = atof(temp[1]);
            _dedx_free_split_temp(temp);
            break;
        }
        _dedx_free_split_temp(temp);
    }
    fclose(fp);
    return charge;
}

size_t _dedx_target_is_gas(int target, int *err) {
    *err = 0;
    size_t is_gas = 0;
    char str[100];
    const char *folder = _dedx_get_data_path();
    char file[] = "gas_states.dat";
    char path[DEDX_PATH_SIZE];
    strcpy(path, folder);
    strcat(path, file);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        *err = 2;
        return 0;
    }

    while (!feof(fp)) {
        if (fgets(str, 100, fp) == NULL) {
        }
        if (atoi(str) == target)
            is_gas = 1;
    }
    fclose(fp);
    return is_gas;
}

float _dedx_read_density(int id, int *err) {
    float density = 1.0;
    char str[100];
    char path[DEDX_PATH_SIZE];
    char file[] = "compos.txt";
    unsigned int items = 0;
    char **temp;
    FILE *fp;

    *err = 0;
    const char *folder = _dedx_get_data_path();
    strcpy(path, folder);
    strcat(path, file);
    fp = fopen(path, "r");

    if (fp == NULL) {
        *err = 1;
        return 0;
    }

    while (!feof(fp)) {

        if (fgets(str, 100, fp) == NULL) {
        }

        temp = _dedx_split(str, '\t', &items, 100);

        if (atoi(temp[0]) == id) {
            density = atof(temp[1]);
            _dedx_free_split_temp(temp);
            break;
        }
        _dedx_free_split_temp(temp);
    }
    fclose(fp);
    if (density == 0.0) {
        *err = 201;
    }
    return density;
}

float _dedx_get_i_value(int target, int state, int *err) {
    float pot = 0.0;
    char file[] = "compos.txt";
    char path[DEDX_PATH_SIZE];
    const char *folder = _dedx_get_data_path();
    char str[100];
    char **temp;
    unsigned int items = 0;

    strcpy(path, folder);
    strcat(path, file);

    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        *err = 1;
        return 0;
    }
    while (!feof(fp)) {
        if (fgets(str, 100, fp) == NULL) {
        }
        temp = _dedx_split(str, '\t', &items, 100);
        if (atoi(temp[0]) == target) {
            if (items == 4) {
                if (state == atoi(temp[3])) {
                    pot = atof(temp[2]);
                }
            } else {
                pot = atof(temp[2]);
                if (state == 2 && _dedx_target_is_gas(target, err) == 0 && target <= 99)
                    pot = pot * 1.13;
            }
        }
        _dedx_free_split_temp(temp);
    }
    fclose(fp);
    if (pot == 0.0) {
        *err = 201;
    }
    return pot;
}

void _dedx_get_composition(int target, float composition[][2], unsigned int *length, int *err) {
    *err = 0;
    char file[] = "composition";
    char path[DEDX_PATH_SIZE];
    char str[100];
    *length = 0;
    float f_temp;
    FILE *fp;
    unsigned int items;
    char **temp;

    const char *folder = _dedx_get_data_path();
    strcpy(path, folder);
    strcat(path, file);

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = 10;
        return;
    }
    while (!feof(fp)) {
        if (fgets(str, 100, fp) != NULL)
            if (str[0] == '#') {
                if (atoi(&str[1]) == target) {
                    while (!feof(fp)) {
                        if (fgets(str, 100, fp) != NULL) {
                        }
                        f_temp = atof(str);
                        if (f_temp == 0.0)
                            break;
                        temp = _dedx_split(str, ':', &items, 100);
                        composition[(*length)][0] = atof(temp[0]);
                        composition[(*length)++][1] = atof(temp[1]);
                        _dedx_free_split_temp(temp);
                    }

                    break;
                }
            }
    }
    fclose(fp);
}

float *_dedx_get_atima_data(int target, int *err) {
    *err = 0;
    char file[] = "atima_compos";
    char path[DEDX_PATH_SIZE];
    const char *folder = _dedx_get_data_path();
    char str[100];
    float *compos;

    FILE *fp;
    unsigned int items;
    char **temp;

    strcpy(path, folder);
    strcat(path, file);

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = 10;
        return NULL;
    }
    compos = (float *) malloc(sizeof(float) * 4);
    while (!feof(fp) && fgets(str, 100, fp) != NULL) {
        temp = _dedx_split(str, '\t', &items, 100);
        if (atoi(temp[0]) == target) {
            compos = (float *) malloc(sizeof(float) * 4);
            compos[0] = atof(temp[1]);
            compos[1] = atof(temp[2]);
            compos[2] = atof(temp[3]);
            compos[3] = atof(temp[4]);
        }
        _dedx_free_split_temp(temp);
    }
    fclose(fp);
    return compos;
}
