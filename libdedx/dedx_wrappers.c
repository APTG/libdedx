void dedx_get_program_list(int *program_list) {
    /* returns a list of available programs, terminated with -1 */
    int i = 0;
    while(dedx_available_programs[i] != -1){
        program_list[i] = dedx_available_programs[i];
        i++;
    }
    program_list[i] = -1;
}

void dedx_get_material_list(int program, int *material_list) {
    /* returns a list of available materials, terminated with -1 */
    int i = 0;
    if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT) {
        while(dedx_program_available_materials[program][i] != -1){
            material_list[i] = dedx_program_available_materials[0][i];
            i++;
        }
    }
    else {
        while(dedx_program_available_materials[program][i] != -1){
            material_list[i] = dedx_program_available_materials[program][i];
            i++;
        }
    }
    material_list[i] = -1;
}
void dedx_get_ion_list(int program, int * ion_list) {
    int i = 0;
    /* returns a list of available ions, terminated with -1 */
    if (program == DEDX_BETHE_EXT00 || program == DEDX_DEFAULT) {  /* any ion, no restrictions */
        for (i=0;i<112;i++)
            ion_list[i]=i+1;
        ion_list[112]=-1; // stopper
    }
    else {
        while(dedx_program_available_ions[program][i] != -1){
            ion_list[i] = dedx_program_available_ions[program][i];
            i++;
        }
        ion_list[i] = -1;
    }
}


//energy - in MeV/nucleon, unless  program is ESTAR, then MeV
//stp - in MeVcm2/g
int dedx_get_stp_table(const int program, const int ion, const int target, const int no_of_points, const float *energies, float *stps) {
    int err = 0;
    dedx_config *config = (dedx_config *)calloc(1,sizeof(dedx_config));
    config->target = target;
    config->ion = ion;
    config->program = program;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    if (err != 0) return err;
    dedx_load_config(ws,config, &err);

    for(int i = 0 ; i < no_of_points; i++){
        stps[i] =  dedx_get_stp(ws, config, energies[i], &err);
    }
    dedx_free_config(config, &err);
    dedx_free_workspace(ws, &err);

    return err;
}

float dedx_get_simple_stp_for_program(const int program, const int ion, const int target, float energy, int *err) {
    float stp;
    dedx_config *config = (dedx_config *)calloc(1,sizeof(dedx_config));
    config->target = target;
    config->ion = ion;
    config->program = program;
    dedx_workspace *ws = dedx_allocate_workspace(1,err);
    if(*err != 0)
        return 0;
    dedx_load_config(ws,config,err);

    stp = dedx_get_stp(ws,config,energy,err);
    if(*err != 0)
        return 0;
    dedx_free_config(config,err);
    dedx_free_workspace(ws,err);
    return stp;
}