#include <math.h>

#include "test_helpers.h"

/*
 * Regression net for issue #149's findings A1, A4, A5 and A6 (Phase 1, item E2 of
 * the plan of action there).
 *
 * This sweeps every (program, ion, material) triple that dedx_get_material_list_for_ion()
 * advertises -- for every program except DEDX_ESTAR, which is unimplemented (see
 * DEDX_ERR_ESTAR_NOT_IMPL) -- and checks three things dedx_get_material_list_for_ion()'s
 * own contract implies should always hold for an advertised combination:
 *
 *   1. dedx_load_config() succeeds (A1, A5: it currently doesn't for 407 of them --
 *      A1's DEDX_AUTO tier-mixing bug and A5's missing ferrous-oxide density row).
 *   2. dedx_get_stp() accepts the program/ion pair's own advertised
 *      dedx_get_min_energy()/dedx_get_max_energy() bounds (A4: the bounds are
 *      documented as authoritative in dedx.h but are only "best-effort hints" in
 *      practice, and are rejected for 1568 combinations).
 *   3. At least one energy sampled across that range returns a finite, positive
 *      stopping power (A1a: 174 DEDX_AUTO configs load with err == DEDX_OK but then
 *      fail at *every* energy, because the spline knots silently end at x == 0).
 *
 * As of this writing (before any of A1/A4/A5/A6 are fixed) this sweep reproduces the
 * exact counts from the issue's manual audit: 101957 combinations swept, 407 load
 * failures, 1568 bound mismatches, 174 dead-at-every-energy configs. The BASELINE_*
 * constants below pin those counts so this test is a *ratchet*, not a silent no-op:
 *
 *   - It stays green today by asserting "no worse than the known-broken baseline",
 *     rather than asserting "zero failures" (which would fail immediately and block
 *     Phase 1, whose job is only to make the failures visible, not fix them yet).
 *   - Phase 2 (A1, A4, A5, A6) must lower the relevant BASELINE_* constant(s) as each
 *     root cause is fixed, down to 0 once all four are done. Do NOT raise a baseline
 *     to make a newly introduced regression pass -- if this test starts failing
 *     because a count went *up*, that is a real regression, not a stale baseline.
 *   - TOTAL_COMBINATIONS is asserted with equality (not a ceiling) so a change in
 *     what dedx_get_material_list_for_ion() advertises -- for better or worse -- is
 *     always visible here, prompting a deliberate update rather than a silent drift.
 */

#define TOTAL_COMBINATIONS 101957
#define BASELINE_LOAD_FAILURES 407
#define BASELINE_BOUND_MISMATCHES 1568
#define BASELINE_DEAD_CONFIGS 174

/* Number of energies sampled (log-spaced) across each combination's advertised
 * [min, max] range for the "dead at every energy" check. */
#define SAMPLE_COUNT 9

typedef struct {
    long total;
    long load_failures;
    long bound_mismatches;
    long dead_configs;
} sweep_stats;

/* Load one (program, ion, target) combination and update stats. Always frees what it
 * allocates; never leaves a dangling workspace/config behind for the next iteration --
 * dedx_load_config() is not idempotent (see #149 C2) and there is no unload function
 * yet, so each combination needs its own fresh workspace. */
static void sweep_one(int program, int ion, int target, sweep_stats *stats) {
    int err = 0;
    dedx_workspace *ws;
    dedx_config *cfg;
    int rc;
    float lo, hi;
    int lo_err = 0, hi_err = 0;
    int ok_samples = 0;
    int i;

    stats->total++;

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;
    err = 0;
    rc = dedx_load_config(ws, cfg, &err);
    if (rc != 0 || err != DEDX_OK) {
        stats->load_failures++;
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return;
    }

    lo = dedx_get_min_energy(program, ion);
    hi = dedx_get_max_energy(program, ion);

    dedx_get_stp(ws, cfg, lo, &lo_err);
    dedx_get_stp(ws, cfg, hi, &hi_err);
    if (lo_err != DEDX_OK || hi_err != DEDX_OK)
        stats->bound_mismatches++;

    for (i = 0; i < SAMPLE_COUNT; i++) {
        float frac = (float) i / (float) (SAMPLE_COUNT - 1);
        float e;
        int e_err = 0;
        float v;

        /* The advertised range spans orders of magnitude (keV to tens of GeV
         * depending on program/ion), so sample log-spaced when both bounds are
         * positive and ordered; fall back to linear spacing for a malformed range
         * rather than dividing by log(0) or log of a negative number. */
        if (lo > 0.0f && hi > lo) {
            float log_lo = logf(lo);
            float log_hi = logf(hi);
            e = expf(log_lo + frac * (log_hi - log_lo));
        } else {
            e = lo + frac * (hi - lo);
        }

        v = dedx_get_stp(ws, cfg, e, &e_err);
        if (e_err == DEDX_OK && isfinite(v) && v > 0.0f)
            ok_samples++;
    }
    if (ok_samples == 0)
        stats->dead_configs++;

    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);
}

static void sweep_program_ion(int program, int ion, sweep_stats *stats) {
    int materials[DEDX_MAX_MATERIAL_LIST];
    unsigned int materials_len = 0;
    int err = 0;
    unsigned int m;

    dedx_get_material_list_for_ion(program, ion, materials, DEDX_MAX_MATERIAL_LIST, &materials_len, &err);
    if (err != DEDX_OK)
        return; /* program/ion combination itself is invalid; nothing to sweep */

    for (m = 0; m < materials_len; m++) {
        sweep_one(program, ion, materials[m], stats);
    }
}

static int check_baseline(long got, long baseline, const char *label) {
    if (got > baseline) {
        fprintf(stderr,
                "FAIL %s: %ld exceeds the known baseline of %ld -- this is a regression, "
                "not a stale baseline; do not silence by raising the constant\n",
                label,
                got,
                baseline);
        return 1;
    }
    if (got < baseline) {
        /* Progress! Not a failure, but flag it so whoever fixed part of A1/A4/A5/A6
         * remembers to tighten the baseline in this file as part of that change. */
        fprintf(stderr,
                "NOTE %s: %ld is below the recorded baseline of %ld -- please lower "
                "BASELINE_* in tests/test_availability_exhaustive.c to match\n",
                label,
                got,
                baseline);
    }
    return 0;
}

int main(void) {
    const int *programs = dedx_get_program_list();
    sweep_stats stats = {0, 0, 0, 0};
    int failures = 0;
    int p;

    for (p = 0; programs[p] != -1; p++) {
        int program = programs[p];
        const int *ions;
        int i;

        if (program == DEDX_ESTAR)
            continue;

        ions = dedx_get_ion_list(program);
        for (i = 0; ions[i] != -1; i++) {
            sweep_program_ion(program, ions[i], &stats);
        }
    }

    printf("test_availability_exhaustive: total=%ld load_failures=%ld bound_mismatches=%ld dead_configs=%ld\n",
           stats.total,
           stats.load_failures,
           stats.bound_mismatches,
           stats.dead_configs);

    if (stats.total != TOTAL_COMBINATIONS) {
        fprintf(stderr,
                "FAIL total combinations: got %ld, expected exactly %d -- "
                "dedx_get_material_list_for_ion()'s advertised set changed; "
                "update TOTAL_COMBINATIONS deliberately after checking why\n",
                stats.total,
                TOTAL_COMBINATIONS);
        failures++;
    }
    failures += check_baseline(stats.load_failures, BASELINE_LOAD_FAILURES, "load_failures");
    failures += check_baseline(stats.bound_mismatches, BASELINE_BOUND_MISMATCHES, "bound_mismatches");
    failures += check_baseline(stats.dead_configs, BASELINE_DEAD_CONFIGS, "dead_configs");

    return failures;
}
