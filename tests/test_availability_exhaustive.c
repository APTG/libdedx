#include <math.h>

#include "test_helpers.h"

/*
 * Regression net for issue #149's findings A1, A2, A4, A5 and A6 (Phase 1, item E2
 * of the plan of action there).
 *
 * This sweeps every (program, ion, material) triple that dedx_get_material_list_for_ion()
 * advertises -- for every program except DEDX_ESTAR, which is unimplemented (see
 * DEDX_ERR_ESTAR_NOT_IMPL) -- and checks three things dedx_get_material_list_for_ion()'s
 * own contract implies should always hold for an advertised combination:
 *
 *   1. dedx_load_config() succeeds.
 *   2. dedx_get_stp() accepts the program/ion pair's own advertised
 *      dedx_get_min_energy()/dedx_get_max_energy() bounds (A4: the bounds are
 *      documented as authoritative in dedx.h but are only "best-effort hints" in
 *      practice -- still tracked here, but A4 itself is out of Phase 2's scope).
 *   3. At least one energy sampled across that range returns a finite, positive
 *      stopping power.
 *
 * Before any of Phase 1/2's fixes, this sweep reproduced the issue's manual audit
 * exactly: 101957 combinations, 407 load failures, 1568 bound mismatches, 174
 * dead-at-every-energy configs -- all 407 load failures and every one of the bound
 * mismatches/dead configs traced back to two root causes: A1's DEDX_AUTO tier-mixing
 * (mismatched per-constituent energy grids) and A5's missing FERROUSOXIDE density row.
 *
 * Phase 2 fixed A1, A2, A3, A5, A6, A7 and A8. Their effect on this sweep, verified by
 * diffing this same sweep's per-(program,ion) output against the pre-Phase-2 build:
 *
 *   - TOTAL_COMBINATIONS: 101957 -> 101886 (-71). A2's off-by-one fix
 *     (material_id_supported()'s element/compound boundary, id 99 = A150 tissue-
 *     equivalent plastic) stops routing compound id 99 through the elemental
 *     embedded-table lookup for ASTAR/PSTAR/MSTAR/ICRU73_OLD/ICRU73/ICRU49/ICRU (the
 *     71 = 1+1+17+16+16+2+18 combinations across their ion lists -- one fewer per
 *     (program,ion) than before). It's now correctly evaluated via composition-based
 *     reachability, which drops it for the ion/program pairs where a constituent
 *     genuinely isn't reachable -- exactly the misclassification A2 set out to fix.
 *   - BASELINE_LOAD_FAILURES: 407 -> 1470. Two components, both traced by error code:
 *       * 224 (DEDX_ERR_TARGET_NOT_FOUND, programs DEDX_DEFAULT/DEDX_BETHE_EXT00 only):
 *         the FERROUSOXIDE (id 159) gap, down from 407. A5 added its density row and
 *         relaxed dedx_internal_validate_rho() to only require rho where it's actually
 *         used, which fixed the 183 tabulated-program combinations. The 224 that
 *         remain are DEDX_DEFAULT/DEDX_BETHE_EXT00, which read a compound target's
 *         *own* I-value directly from embedded metadata rather than Bragg-averaging
 *         it from constituents; no authoritative FERROUSOXIDE I-value exists (see
 *         data/README.md), and A5 deliberately left it unfabricated rather than
 *         invent a number -- so these 224 stay a documented, open gap.
 *       * 1246 (DEDX_ERR_INCONSISTENT_COMPOUND, DEDX_AUTO only, new): compounds whose
 *         constituents resolve onto mismatched energy grids (A1's tier-mixing bug).
 *         Before A1, load_compound() silently summed across the mismatched grids and
 *         dedx_load_config() reported success -- these combinations were counted
 *         instead under bound_mismatches/dead_configs below. A1 now rejects them
 *         instead of serving a wrong or unusable number, which is why they show up
 *         here as load failures rather than a regression.
 *   - BASELINE_BOUND_MISMATCHES: 1568 -> 480, BASELINE_DEAD_CONFIGS: 174 -> 0. Both
 *     drop because A1 catches the same underlying grid-mismatch combinations earlier,
 *     as a clean load failure, instead of letting them load and then fail (or return
 *     nonsense) at the energy-sampling stage.
 *
 * The BASELINE_* constants below still pin these counts so this test is a *ratchet*,
 * not a silent no-op:
 *
 *   - It stays green by asserting "no worse than the known baseline", rather than
 *     asserting "zero failures" -- A4 (advertised-bounds accuracy) and the
 *     FERROUSOXIDE I-value gap remain deliberately open past Phase 2.
 *   - Any future fix must lower the relevant BASELINE_* constant(s) as its root cause
 *     is addressed. Do NOT raise a baseline to make a newly introduced regression
 *     pass -- if this test starts failing because a count went up unexpectedly, that
 *     is a real regression, not a stale baseline. (The load_failures increase in
 *     Phase 2 above was verified, not assumed, before raising this constant.)
 *   - TOTAL_COMBINATIONS is asserted with equality (not a ceiling) so a change in
 *     what dedx_get_material_list_for_ion() advertises -- for better or worse -- is
 *     always visible here, prompting a deliberate update rather than a silent drift.
 */

#define TOTAL_COMBINATIONS 101886
#define BASELINE_LOAD_FAILURES 1470
#define BASELINE_BOUND_MISMATCHES 480
#define BASELINE_DEAD_CONFIGS 0

/* Number of energies sampled (log-spaced) across each combination's advertised
 * [min, max] range for the "dead at every energy" check. */
#define SAMPLE_COUNT 9

typedef struct {
    long total;
    long load_failures;
    long bound_mismatches;
    long dead_configs;
    long alloc_failures;
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
    if (ws == NULL || cfg == NULL) {
        /* Hard test failure, not a swept-combination failure: an OOM here means the
         * sweep itself can no longer be trusted, so report it clearly instead of
         * dereferencing a NULL workspace/config below. dedx_free_config()/
         * dedx_free_workspace() are both NULL-safe, so this cleanup is safe even
         * when only one of the two allocations failed. */
        fprintf(stderr,
                "FAIL sweep_one: allocation failed (workspace=%p config=%p) for program=%d ion=%d target=%d\n",
                (void *) ws,
                (void *) cfg,
                program,
                ion,
                target);
        stats->alloc_failures++;
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return;
    }
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
    sweep_stats stats = {0, 0, 0, 0, 0};
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

    printf("test_availability_exhaustive: total=%ld load_failures=%ld bound_mismatches=%ld dead_configs=%ld "
           "alloc_failures=%ld\n",
           stats.total,
           stats.load_failures,
           stats.bound_mismatches,
           stats.dead_configs,
           stats.alloc_failures);

    /* Unlike the baselines below, any allocation failure is an unconditional hard
     * failure -- there is no acceptable count of "the sweep couldn't get memory". */
    if (stats.alloc_failures > 0) {
        fprintf(
            stderr, "FAIL: %ld combination(s) hit an allocation failure; see FAIL lines above\n", stats.alloc_failures);
        failures++;
    }

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
