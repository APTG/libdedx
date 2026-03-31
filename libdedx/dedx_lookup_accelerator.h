#ifndef DEDX_LOOKUP_ACCELERATOR_H_INCLUDED
#define DEDX_LOOKUP_ACCELERATOR_H_INCLUDED

/** @brief Cached interval index for repeated spline evaluations.
 *
 *  The spline evaluator stores the last successful interval in @c cache so
 *  nearby lookups can skip the binary search. The hit/miss counters are
 *  currently informational only.
 */
typedef struct {
    int cache;
    int hits;
    int miss;
} _dedx_lookup_accelerator;

#endif // DEDX_LOOKUP_ACCELERATOR_H_INCLUDED
