/*
 * adapt.h
 *
 *  Created on: Jun 4, 2011
 *      Author: ricky
 */

#ifndef ADAPT_H_
#define ADAPT_H_

#define real float
#include <math.h>
#include <stdio.h>

real adapt24(real (*func)(real),real a,real b,real f2,real f3,real acc,real eps,real *error);

real adapt(real (*func)(real),real a,real b,real acc,real eps,real *error);

#endif /* ADAPT_H_ */
