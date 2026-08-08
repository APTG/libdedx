#include "dedx_periodic_table.h"

/* dedx_amu[]/dedx_nucl[] are indexed by id - 1, so id must also be bounded below:
 * a caller-supplied id <= 0 (e.g. an unchecked custom-compound elements_id[] entry)
 * would otherwise index the tables out of bounds. */
float dedx_internal_get_atom_charge(int id, int *err) {
    *err = DEDX_OK;
    if (id >= 1 && id < 113) {
        return id;
    }
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}

float dedx_internal_get_atom_mass(int id, int *err) {
    *err = DEDX_OK;
    if (id >= 1 && id < 113)
        return dedx_amu[id - 1];
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}

int dedx_internal_get_nucleon(int id, int *err) {
    *err = DEDX_OK;
    if (id >= 1 && id < 113)
        return dedx_nucl[id - 1];
    *err = DEDX_ERR_NOT_AN_ELEMENT;
    return -1;
}
