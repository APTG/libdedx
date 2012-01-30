#include "dedx_calculate_charge.h"
#include "dedx_periodic_table.h"
float _dedx_get_avg_charge(float composition[][2], int length)
{
    int i = 0;
    int err = 0;
    float charge = 0.0;
    float nucleon = 0.0;
    for(i = 0; i < length; i++)
    {
        nucleon += composition[i][1]/_dedx_get_atom_mass((int)composition[i][0],&err);
    }
    for(i = 0; i < length; i++)
    {
        charge += composition[i][1]/_dedx_get_atom_mass((int)composition[i][0],&err)/nucleon*_dedx_get_atom_charge((int)composition[i][0],&err);
    }
    return charge;
}
float _dedx_get_weight_charge(float composition[][2], int length)
{
    int err = 0;
    float charge = 0.0;
    int i = 0;
    for(i = 0; i < length; i++)
    {
        charge += composition[i][1]*_dedx_get_atom_charge((int)composition[i][0],&err);
    }
    return charge;
}
