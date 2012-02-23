#ifndef DEDX_STOPPING_DATA_H_INCLUDED
#define DEDX_STOPPING_DATA_H_INCLUDED

#include "dedx.h"

typedef struct
{
    int target;
    int ion;
    unsigned int length;
    float data[_DEDX_MAXELEMENTS];
} stopping_data;

#endif // DEDX_STOPPING_DATA_H_INCLUDED
