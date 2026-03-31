#ifndef DEDX_STOPPING_DATA_H
#define DEDX_STOPPING_DATA_H

#include "dedx_elements.h"

typedef struct {
    int target;
    int ion;
    unsigned int length;
    float data[DEDX_MAX_ELEMENTS];
} stopping_data;

#endif // DEDX_STOPPING_DATA_H
