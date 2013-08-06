/*
 * range.h
 *
 *  Created on: Jun 4, 2011
 *      Author: ricky
 */

#ifndef RANGE_H_
#define RANGE_H_

#include <dedx.h>
#include "adapt.h"

#define real float

float get_range(dedx_workspace *ws, dedx_config *cfg, int *err, float a, float b, float acc, float eps);

#endif /* RANGE_H_ */
