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
 * Result is cached in a static buffer; never changes after the first call.
 *
 * NOTE: not thread-safe. The done/resolved statics are unprotected against
 * concurrent first calls. The library as a whole is not thread-safe; this
 * function should be fixed with call_once() as part of a broader thread-safety
 * effort. See https://github.com/APTG/libdedx/issues (thread-safety issue). */
static const char *_dedx_get_data_path(void) {
    static char resolved[DEDX_PATH_SIZE];
    static int done = 0;
    struct stat st;
    char probe[DEDX_PATH_SIZE];

    if (!done) {
        snprintf(probe, sizeof(probe), "%s%s", DEDX_DATA_PATH_LOCAL, "icru_pstarEng.dat");
        if (DEDX_DATA_PATH_LOCAL[0] != '\0' && stat(probe, &st) == 0)
            snprintf(resolved, sizeof(resolved), "%s", DEDX_DATA_PATH_LOCAL);
        else
            snprintf(resolved, sizeof(resolved), "%s", DEDX_DATA_PATH); /* LCOV_EXCL_LINE */
        done = 1;
    }
    return resolved;
}

void _dedx_convert_to_binary(char *path, char *output, int *err) {
    FILE *fp;
    FILE *out;
    char line[100];
    int datalines = 0;
    int i;
    unsigned int length;
    float data[_DEDX_MAXELEMENTS];
    char **temp = NULL;
    stopping_data container;

    *err = DEDX_OK;
    fp = fopen(path, "r");
    out = fopen(output, "wb+");
    if (fp == NULL || out == NULL) { /* LCOV_EXCL_START */
        if (out == NULL) {
            *err = DEDX_ERR_WRITE_FAILED;
            if (fp != NULL)
                fclose(fp);
        } else {
            *err = DEDX_ERR_NO_DATA_FILE;
            fclose(out);
        }
        return;
    } /* LCOV_EXCL_STOP */
    while (!feof(fp)) {
        if (fgets(line, 100, fp) == NULL) {
        }
        if (line[0] == '#') {
            length = 0;
            i = 0;
            memset(&data, 0, _DEDX_MAXELEMENTS);
            temp = _dedx_split(line, ':', &length, 100);
            temp[0][0] = ' ';
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
    const char *folder;
    char path[DEDX_PATH_SIZE];
    char input_path[DEDX_PATH_SIZE];
    FILE *fp;
    stopping_data dat;

    *err = DEDX_OK;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s.bin", folder, _dedx_get_program_file(prog));

    fp = fopen(path, "rb");
    if (fp == NULL) {
        snprintf(input_path, sizeof(input_path), "%s%s.dat", folder, _dedx_get_program_file(prog));
        _dedx_convert_to_binary(input_path, path, err);
        if (*err != DEDX_OK) {
            *err = DEDX_ERR_NO_BINARY_DATA;
            return;
        }
        fp = fopen(path, "rb");
        if (fp == NULL) {
            *err = DEDX_ERR_NO_BINARY_DATA;
            return;
        }
    }
    *err = DEDX_ERR_COMBINATION_NOT_FOUND;
    while (fread(&dat, sizeof(stopping_data), 1, fp) == 1) {
        if (dat.target == target && dat.ion == ion) {
            memcpy(data, &dat, sizeof(stopping_data));
            *err = DEDX_OK;
        }
    }
    fclose(fp);
}

void _dedx_convert_energy_binary(char *path, char *output, int *err) {
    FILE *fp;
    FILE *out;
    char line[100];
    int datalines;
    int i;
    float data[_DEDX_MAXELEMENTS];

    *err = DEDX_OK;
    fp = fopen(path, "r");
    out = fopen(output, "wb+");
    if (fp == NULL || out == NULL) { /* LCOV_EXCL_START */
        if (out == NULL) {
            *err = DEDX_ERR_WRITE_FAILED;
            if (fp != NULL)
                fclose(fp);
        } else {
            *err = DEDX_ERR_NO_ENERGY_FILE;
            fclose(out);
        }
        return;
    } /* LCOV_EXCL_STOP */

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
    const char *folder;
    char path[DEDX_PATH_SIZE];
    char input_path[DEDX_PATH_SIZE];
    FILE *fp;

    *err = DEDX_OK;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s.bin", folder, _dedx_get_energy_file(prog));

    fp = fopen(path, "rb");
    if (fp == NULL) {
        snprintf(input_path, sizeof(input_path), "%s%s.dat", folder, _dedx_get_energy_file(prog));
        _dedx_convert_energy_binary(input_path, path, err);
        if (*err != DEDX_OK) {
            *err = DEDX_ERR_NO_BINARY_ENERGY;
            return;
        }
        fp = fopen(path, "rb");
        if (fp == NULL) {
            *err = DEDX_ERR_NO_BINARY_ENERGY;
            return;
        }
    }
    if (fread(energy, sizeof(float) * _DEDX_MAXELEMENTS, 1, fp) == 0) {
    }
    fclose(fp);
}

float _dedx_read_effective_charge(int id, int *err) {
    const char *folder;
    char line[100];
    char **temp;
    unsigned int items;
    char path[DEDX_PATH_SIZE];
    FILE *fp;
    float charge = -1;

    *err = DEDX_OK;
    if (id < 99) {
        return (float) id;
    }
    folder = _dedx_get_data_path();
    items = 0;
    temp = NULL;
    snprintf(path, sizeof(path), "%s%s", folder, "effective_charge.dat");

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = DEDX_ERR_NO_CHARGE_FILE;
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
    const char *folder;
    size_t is_gas;
    char str[100];
    char path[DEDX_PATH_SIZE];
    FILE *fp;

    *err = DEDX_OK;
    is_gas = 0;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s", folder, "gas_states.dat");
    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = DEDX_ERR_NO_GAS_FILE;
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
    const char *folder;
    float density;
    char str[100];
    char path[DEDX_PATH_SIZE];
    unsigned int items;
    char **temp;
    FILE *fp;

    *err = DEDX_OK;
    density = 1.0;
    items = 0;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s", folder, "compos.txt");
    fp = fopen(path, "r");

    if (fp == NULL) {
        *err = DEDX_ERR_NO_COMPOS_FILE;
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
        *err = DEDX_ERR_TARGET_NOT_FOUND;
    }
    return density;
}

float _dedx_get_i_value(int target, int state, int *err) {
    const char *folder;
    float pot;
    char path[DEDX_PATH_SIZE];
    char str[100];
    char **temp;
    unsigned int items;
    FILE *fp;

    pot = 0.0;
    items = 0;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s", folder, "compos.txt");

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = DEDX_ERR_NO_COMPOS_FILE;
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
        *err = DEDX_ERR_TARGET_NOT_FOUND;
    }
    return pot;
}

void _dedx_get_composition(int target, float composition[][2], unsigned int *length, int *err) {
    const char *folder;
    char path[DEDX_PATH_SIZE];
    char str[100];
    float f_temp;
    unsigned int items;
    char **temp;
    FILE *fp;

    *err = DEDX_OK;
    *length = 0;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s", folder, "composition");

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = DEDX_ERR_NO_COMPOSITION;
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
    const char *folder;
    char path[DEDX_PATH_SIZE];
    char str[100];
    float *compos;
    unsigned int items;
    char **temp;
    FILE *fp;

    *err = DEDX_OK;
    folder = _dedx_get_data_path();
    snprintf(path, sizeof(path), "%s%s", folder, "atima_compos");

    fp = fopen(path, "r");
    if (fp == NULL) {
        *err = DEDX_ERR_NO_COMPOSITION;
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
