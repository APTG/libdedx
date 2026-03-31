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
} _dedx_bethe_struct;

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
} _dedx_gold_struct;

/** @brief Heap-owned Bethe work buffers cached across repeated evaluations. */
typedef struct {
    _dedx_bethe_struct *bet;
    _dedx_gold_struct *gold;
} _dedx_bethe_coll_struct;

#endif // DEDX_BETHE_STRUCT_H
