#include "dedx_periodic_table.h"
float _dedx_get_atom_charge(int id, int *err)
{
    if(id < 113)
    {
      return id;
    }
    *err = 204;
    return -1;
}
float _dedx_get_atom_mass(int id, int *err)
{
    *err = 0;
    if(id < 113)
      return dedx_amu[id-1];
    *err = 204;
    return -1;
}
