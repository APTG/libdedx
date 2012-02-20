#include "dedx_math.h"
float _dedx_max(float a, float b)
{
	if(a > b)
		return a;
	return b;
}

float _dedx_min(float a, float b)
{
	if(a < b)
		return a;
	return b;
}
