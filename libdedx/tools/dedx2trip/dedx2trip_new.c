/*
 * Creates TRiP98 readable stopping power tables
 * for the ions wanted by TRiP
 * with water as target.
 *
 * using: libdEdx version 1.2.1
 *
 * author: Ricky Teiwes
 * e-mail: teiwes@phys.au.dk
 */
#include "dedx2trip_new.h"

#define STRSIZE 127

// main method
int main(int argc, char *argv[]){
	int vmajor, vminor, vpatch, vsvn; // integers to hold the version numbers
	int prog_select, target_select; // Program, Target: target is faked to water
	char progname[STRSIZE]; // name of the program to be used
	char filename[STRSIZE]; // filename of output file
	char comment[STRSIZE]; // comment, gets written to the output file
	int err = 0; // integer holding error state
	int j,i; // ints for loops

	dedx_workspace *ws; // create a libdedx workspace
	dedx_config *cfg[TRIP_IONS+1]; // cfg struct

	// check the input
	if (argc != 2) {
		printf("Usage:\n");
		printf("%s program ICRU_ID\n", argv[0]);
		printf("Use -1 for either program.\n\n");
		printf("(C) 2012 by Ricky Teiwes <teiwes@phys.au.dk>.\n");
		//printf("This is part of SHIELD-HITxxA\n");
		exit(-1);
	}

	sprintf(filename,"output.dedx");
	FILE * output = fopen(filename,"w+"); // open output file

	prog_select = atoi(argv[1]); // ascii to integer
	target_select = 276; // Water ICRU_ID

	/*
	 * Check for inconsistencies
	 */
	if (prog_select > 3) {
		printf("No such program: %i\n", prog_select);
		prog_select = -1;  /* print list and exit */
	}

	if (prog_select < 1) {
		printf("Available programs:\n");
		printf("  1 PSTAR + MSTAR\n");
		printf("  2 ICRU \n");
		printf("  3 Bethe-Bloch with Linhard-Scharff.\n");
		exit(-1);
	}


	/* Allocate memory for workspace and configuration */
	ws = dedx_allocate_workspace(TRIP_IONS+1, &err); // returns struct of type dedx_workspace
	if (err != 0) {
		printf("Error during init!");
	}
	for (i = 0; i<=TRIP_IONS; i++) {
		cfg[i] = calloc(1, sizeof(dedx_config));
	}
	/* set targets and ions for all configs */
	for (i=0; i <= TRIP_IONS; i++) {
		cfg[i]-> target = target_select;
		//TODO:  modified from here on
		cfg[i]->rho = 1.0;
		cfg[i]->i_value = 80.0;
		cfg[i]->elements_id = calloc(2, sizeof(int));
		cfg[i]->elements_id[0] = DEDX_HYDROGEN;
		cfg[i]->elements_id[1] = DEDX_OXYGEN;
		cfg[i]->elements_atoms = calloc(2,sizeof(int));
		cfg[i]->elements_atoms[0] = 2;
		cfg[i]->elements_atoms[1] = 1;
		cfg[i]->elements_length = 2;
		//
		cfg[i]-> ion = Z[i];
	}
	// TODO: set the last placeholder to MSTAR
	cfg[TRIP_IONS]-> program = DEDX_MSTAR;

	/* PSTAR and MSTAR */
	if (prog_select == 1) {
		sprintf(progname,"PSTAR for Z=1 and MSTAR for Z > 1");
		// the three first are H as projectile
		// set to use PSTAR, as MSTAR has no values
		cfg[0] -> program = DEDX_PSTAR;
		cfg[1] -> program = DEDX_PSTAR;
		cfg[2] -> program = DEDX_PSTAR;
		// Set the rest to MSTAR
		for (i=3; i < TRIP_IONS; i++) {
			cfg[i]-> program = DEDX_MSTAR;
		}
	}
	/* ICRU49 + 73 */
	if (prog_select == 2) {
		sprintf(progname,"ICRU 49 and ICRU73 + MSTAR for He-4 > 250 MeV");
		cfg[0] -> program = DEDX_PSTAR;
		cfg[1] -> program = DEDX_PSTAR;
		cfg[2] -> program = DEDX_PSTAR;
		for (i=3; i < TRIP_IONS; i++) {
			cfg[i]-> program =DEDX_ICRU;
		}
	}
	/*  BETHE_EXT00 */
	if (prog_select == 3) {
		sprintf(progname,"BETHE _EXT00 using mean exitation potential I = %.2feV",cfg[TRIP_IONS]->i_value);
		for (i=0; i < TRIP_IONS; i++) {
			cfg[i] -> program = DEDX_BETHE_EXT00;
		}

	}

	/* Populate all tables and check for errors */
	for (i=0; i < TRIP_IONS; i++) { // last config was already loaded
		dedx_load_config(ws, cfg[i], &err);
		if (err != 0) {
			printf("Bummer when loading ion %s with prog %i in material %i: %i", elements[i],cfg[i]->program, cfg[i]->target,err);
			exit(-1);
		}
	}

	/* Set loaded program: has to be done after table population because of I_value */
	if (prog_select == 1) sprintf(progname,"PSTAR for Z=1 and MSTAR for Z > 1");
	if (prog_select == 2) sprintf(progname,"ICRU 49 and ICRU73 + MSTAR for He-4 > 250 MeV");
	if (prog_select == 3) sprintf(progname,"BETHE _EXT00 using mean exitation potential I = %.2feV",cfg[0]->i_value);


	/* Create Header */
	fprintf(output, "!filetype    dEdx\n");
	fprintf(output, "!fileversion\t19980515\n");
	time_t time1;
	time(&time1);
	fprintf(output,"!filedate\t%s", ctime(&time1));
	fprintf(output, "!material    H2O\n");
	fprintf(output, "!composition    H2O\n");
	fprintf(output, "!density    1\n");
	fprintf(output, "#\n");
	sprintf(comment,progname);
	fprintf(output, "# comment: %s\n", comment);
	fprintf(output, "#\n");

	/* Calculation of table */
	dedx_get_version(&vmajor,&vminor,&vpatch,&vsvn);
	printf("* Stopping power tables calculated with libdEdx, version %i.%i.%i-svn%i\n",vmajor,vminor,vpatch,vsvn);
	printf("Preparing stopping power table for %s using %s.\n",dedx_get_material_name(target_select), progname);
	float ste; // float to store the stopping power
	for(j= 0; j < TRIP_IONS; j++)// the number of projectiles Check the number! Boundary value analysis CORRECT!!
	{
		printf("Calculating for ion: %s\n", elements[j]);

		fprintf(output, "!projectile   %s\n", elements[j]);
		fprintf(output, "# E/(MeV/u) dE/dx(MeVcm**2/g) Range(g/cm**2)(optional)\n");
		fprintf(output, "!dedx\n");

		for(i = 0; i < TRIP_ETABLE; i++) // the number of energy entries
		{
			fprintf(output,"        %4.2f  ",trip_energy_table[i]);
			// HELIUM if ASTAR is used!
			if(Z[j] == 2) {
				if (trip_energy_table[i] > 250) {
					ste = dedx_get_stp(ws, cfg[TRIP_IONS], trip_energy_table[i], &err);
				}
				else {
					ste = dedx_get_stp(ws, cfg[j], trip_energy_table[i], &err);
				}
			}
			else{
				ste = dedx_get_stp(ws, cfg[j], trip_energy_table[i], &err);
			}
			fprintf(output,"%1.3E  ",ste);
			float range;
			float eps = 0.0001;
			float acc = 0.0001;
			float a = 0.026; // lower bound i.e. T_f final kinetic energy

			// HELIUM if ASTAR is used, also use the respective configuration
			if (Z[j] == 2) {
				if (trip_energy_table[i] > 250) {
					range = masses[j] * get_range(ws, cfg[j], &err, a, 250, acc, eps) + masses[j] * get_range(ws, cfg[TRIP_IONS], &err, 250, trip_energy_table[i], acc, eps);
				}
				else {
					range = masses[j] * get_range(ws, cfg[j], &err, a, trip_energy_table[i], acc, eps);
				}
			} else {
				range = masses[j] * get_range(ws, cfg[j], &err, a, trip_energy_table[i], acc, eps);
			}

			fprintf(output,"%1.3E", range);
			fprintf(output,"\n");
		}
	}
	// close the output file
	fclose(output);

	// clean up libdedx, to reallocate the memory
	dedx_free_workspace(ws, &err);
	// TODO: free cfg's

	printf("Wrote %s.\n", filename);
	return EXIT_SUCCESS;
}




