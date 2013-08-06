/*
 * range.c
 *
 *  Created on: Jun 4, 2011
 *      Author: Ricky Teiwes
 */

#include "range.h"

dedx_workspace globws;
dedx_config globcfg;
int globerr;

float f(float x) {
	float ste;
	static int i = 0;
	i++;
	//printf("fofofo... %i globcfg:%i x:%f\n", i, globcfg, x);
	ste = 1 / dedx_get_stp(&globws, &globcfg, x, &globerr) ; // it should not be x, but the energy
	//printf("fofofo... %i globerr:%i x:%f\n", i, globerr, x);
	return ste;//
}

float get_range(dedx_workspace * ws, dedx_config * cfg, int *err, float a, float b, float acc, float eps){

	float integral;
	real error = 0;

	globws = *ws;
	globcfg = *cfg;
	globerr = *err;
	integral = adapt(&f, a, b, acc, eps, &error);

	return integral;
}
