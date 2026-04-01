#ifndef DEDX_BETHE_STRUCT_H
#define DEDX_BETHE_STRUCT_H

/** @brief Cached Bethe input parameters for one target/projectile combination. */
typedef struct {
    double TZ0;
    double TA0;
    double potentiale;
    double rho;
    double PZ0;
    double PA0;
} dedx_internal_bethe_model;

/** @brief Scratch values used while stitching the low-energy extension. */
typedef struct {
    double e_min;
    double e_max;
    double epsilon;
    double h;
    double e_zero;
    double e_extr;
    double f_extr;
    double e_sewn;
    double f_sewn;
} dedx_internal_bethe_gold;

/** @brief Heap-owned Bethe work buffers cached across repeated evaluations. */
typedef struct {
    dedx_internal_bethe_model *bet;
    dedx_internal_bethe_gold *gold;
} dedx_internal_bethe_workspace;

#endif // DEDX_BETHE_STRUCT_H
