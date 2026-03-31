#include "dedx_periodic_table.h"

float _dedx_get_atom_charge(int id, int *err) {
    if (id < 113) {
        return id;
    }
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}

float _dedx_get_atom_mass(int id, int *err) {
    *err = DEDX_OK;
    if (id < 113)
        return dedx_amu[id - 1];
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}

int _dedx_get_nucleon(int id, int *err) {
    *err = DEDX_OK;
    if (id < 113)
        return dedx_nucl[id - 1];
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}
