#include <stdio.h>
#include <dedx.h>
#include "dedx_wrappers.h"

int main() {
    const int * programs = dedx_get_program_list();
    const int materials[] = {DEDX_WATER_LIQUID, DEDX_SILICON, DEDX_LITHIUM_FLUORIDE, DEDX_GOLD, DEDX_COPPER, DEDX_LUCITE_PERSPEX_PMMA, -1};
    const int ions[] = {DEDX_HYDROGEN, DEDX_HELIUM, DEDX_OXYGEN, DEDX_CARBON, DEDX_URANIUM, DEDX_IRON, -1};

    FILE * file = fopen("result.txt", "a");
    fprintf(file, "Program, Material, Ion, Energy, Stopping Power \n");
    for(int i = 0; programs[i] != -1; i++) {
        const char * prog_name = dedx_get_program_name(programs[i]);

        for(int j =0; materials[j] != -1; j++){
            const char * material_name = dedx_get_material_name(materials[j]);

            for(int k =0; ions[k] != -1; k++){
                const char * ion_name = dedx_get_ion_name(ions[k]);

                int no_of_samples = dedx_get_stp_table_size(programs[i], ions[k], materials[j]);
                if(no_of_samples != -1) {
                    float *default_energies = calloc(sizeof(float), no_of_samples);
                    float *default_stps = calloc(sizeof(float), no_of_samples);

                    dedx_fill_default_energy_stp_table(programs[i], ions[k], materials[j], default_energies, default_stps);

                    for(int i = 0; i < no_of_samples; i++){
                        fprintf(file, "%s, %s, %s, %f, %f \n", prog_name, material_name, ion_name, default_energies[i], default_stps[i]);
                    }

                    free(default_energies);
                    free(default_stps);
                }
            }
        }
    }
    fclose(file);

    return 0;
}