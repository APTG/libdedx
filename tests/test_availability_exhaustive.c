#include <math.h>
#include <string.h>

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
 * Phase 2 fixed A1, A2, A3, A5, A6, A7 and A8, and (in review) B2-B4 below. Load
 * failures are asserted *per error code*, not as one aggregate ceiling: an aggregate
 * budget can absorb a brand-new failure mode as long as some other one improved by at
 * least as much in the same run, which is exactly the kind of silent trade a ratchet
 * is supposed to catch. TOTAL_COMBINATIONS is still asserted separately, with
 * equality, so a change in what dedx_get_material_list_for_ion() advertises is always
 * visible here too.
 *
 * History, verified at each step by diffing this sweep's per-(program,ion) output
 * against the previous build and tracing every failure by error code -- not assumed:
 *
 *   - A2's off-by-one fix (material_id_supported()'s element/compound boundary, id 99
 *     = A150 tissue-equivalent plastic) stops routing compound id 99 through the
 *     elemental embedded-table lookup for ASTAR/PSTAR/MSTAR/ICRU73_OLD/ICRU73/ICRU49/
 *     ICRU. TOTAL_COMBINATIONS: 101957 -> 101886 (-71 = 1+1+17+16+16+2+18, one fewer
 *     per (program,ion) across those programs' ion lists).
 *   - A1 rejects DEDX_AUTO compounds whose constituents resolve onto mismatched
 *     energy grids (DEDX_ERR_INCONSISTENT_ENERGY_GRID) instead of silently mixing or
 *     truncating them -- previously counted under bound_mismatches/dead_configs
 *     instead, or not counted as broken at all. A5 fixes 183 of the original 407
 *     FERROUSOXIDE (id 159) failures (adds its density row, only requires rho where a
 *     program actually reads it); the remaining 224 (DEDX_ERR_TARGET_NOT_FOUND, for
 *     DEDX_DEFAULT/DEDX_BETHE_EXT00 only, which read a compound's *own* I-value
 *     directly rather than Bragg-averaging it, and no authoritative FERROUSOXIDE
 *     I-value exists to fill that row with -- see data/README.md) were, at that point,
 *     a deliberate, documented, *open* gap on the load_failures side. At that
 *     intermediate point load_failures read 407 -> 1470 (224 remaining + 1246 new
 *     DEDX_ERR_INCONSISTENT_ENERGY_GRID) and bound_mismatches/dead_configs dropped
 *     1568 -> 480 and 174 -> 0.
 *   - In review of this PR, material_id_supported() was taught both of the
 *     constraints above instead of only reproducing dedx_load_config()'s answer after
 *     the fact (finding B4): it now predicts a DEDX_AUTO grid-tier mismatch the same
 *     way find_data() resolves it (dedx_embedded_resolve_program() per constituent),
 *     and it now checks I-value availability for program >= DEDX_DEFAULT the same way
 *     it already checked density, closing the FERROUSOXIDE gap above to zero *on the
 *     advertising side* without fabricating the I-value itself -- the material simply
 *     stops being advertised for the two programs that need a number nobody can
 *     verify. This is what actually brought load_failures down, not a raised ceiling:
 *     TOTAL_COMBINATIONS 101886 -> 100222 (-1664, the compounds/programs that would
 *     have failed and are no longer advertised), load_failures 1470 -> 108,
 *     bound_mismatches 480 -> 470.
 *   - The 108 that remained were DEDX_MSTAR only, all DEDX_ERR_ION_NOT_SUPPORTED_MSTAR:
 *     ions 12-15 combined with a gas target hit an "illegal mode" branch in
 *     dedx_mpaul.c that predates this whole PR (reproduced identically against
 *     `main`, e.g. DEDX_MSTAR + ion 12 + DEDX_ARGON) -- previously silent (a
 *     nonsense stopping power with err == DEDX_OK, also found in review, finding B2),
 *     now a clean, correctly-reported error.
 *   - A second review follow-up closed that gap too: material_id_supported() and
 *     element_supported_for_ion() now take an `mstar_state` parameter, not a new
 *     public API surface but the "hardcoded assumption about the default mode"
 *     option floated above -- find_data() itself already assumes mstar_mode 'b' when
 *     a caller leaves it unset, so probing dedx_internal_calculate_mspaul_coef()
 *     under that same default is exactly what dedx_load_config() would actually do.
 *     For a *compound* target, that probe has to use the compound's own resolved
 *     gas/condensed state, not each constituent's own -- load_compound() resolves
 *     config->compound_state once, from the compound's own id, before its
 *     constituent loop runs (see issue #149 finding A3), so a per-constituent guess
 *     would both under- and over-advertise relative to what dedx_load_config() does.
 *     TOTAL_COMBINATIONS: 100222 -> 100114 (-108, the MSTAR ion/gas-target
 *     combinations that would have failed and are no longer advertised),
 *     load_failures 108 -> 0. LOAD_FAILURE_BASELINES is now empty: every load failure
 *     this sweep used to hit has either been fixed outright or stopped being
 *     advertised, so dedx_load_config() is now expected to succeed for every
 *     combination this sweep is asked to check.
 */

#define TOTAL_COMBINATIONS 100114

/* Every (error code, count) pair load failures are currently expected to fall into.
 * Equality per code, not a ceiling: a future fix should remove an entry (or lower its
 * count) as its root cause is addressed, and any load failure whose code isn't listed
 * here at all is an unconditional hard failure below, not something a budget can
 * quietly absorb. */
typedef struct {
    int code;
    long baseline;
} error_baseline;

/* Empty: as of the MSTAR-availability follow-up below, every load failure this sweep
 * used to hit has a root cause that's either fixed outright or -- for the two open,
 * documented physics-data gaps (FERROUSOXIDE's I-value, the ICRU73 Na-in-Ar zero) --
 * no longer advertised as available in the first place, so this sweep now expects
 * dedx_load_config() to succeed for every combination it's asked to check. A future
 * regression that adds even one new load failure is therefore always a hard FAIL via
 * load_failures_unexpected below (any error code, since none has an entry here to
 * match against), not something a baseline entry could absorb. */
static const error_baseline LOAD_FAILURE_BASELINES[] = {};

#define BASELINE_BOUND_MISMATCHES 470
#define BASELINE_DEAD_CONFIGS 0

/* Number of energies sampled (log-spaced) across each combination's advertised
 * [min, max] range for the "dead at every energy" check. */
#define SAMPLE_COUNT 9

/* One counter per entry in LOAD_FAILURE_BASELINES, in the same order, plus a catch-all
 * for any error code not listed there. */
typedef struct {
    long total;
    long load_failures;
    long load_failures_by_code[sizeof(LOAD_FAILURE_BASELINES) / sizeof(LOAD_FAILURE_BASELINES[0])];
    long load_failures_unexpected;
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
        size_t code_i;
        int matched = 0;

        stats->load_failures++;
        for (code_i = 0; code_i < sizeof(LOAD_FAILURE_BASELINES) / sizeof(LOAD_FAILURE_BASELINES[0]); code_i++) {
            if (LOAD_FAILURE_BASELINES[code_i].code == err) {
                stats->load_failures_by_code[code_i]++;
                matched = 1;
                break;
            }
        }
        if (!matched) {
            stats->load_failures_unexpected++;
            if (stats->load_failures_unexpected <= 5) {
                fprintf(stderr,
                        "FAIL sweep_one: unexpected error code %d for program=%d ion=%d target=%d "
                        "(not in LOAD_FAILURE_BASELINES)\n",
                        err,
                        program,
                        ion,
                        target);
            }
        }
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
        /* Progress! Not a failure, but flag it so whoever fixed part of the remaining
         * gap remembers to tighten the baseline in this file as part of that change. */
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
    sweep_stats stats;
    int failures = 0;
    int p;
    size_t code_i;

    memset(&stats, 0, sizeof(stats));

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
    for (code_i = 0; code_i < sizeof(LOAD_FAILURE_BASELINES) / sizeof(LOAD_FAILURE_BASELINES[0]); code_i++) {
        printf(
            "  load_failures[err=%d]=%ld\n", LOAD_FAILURE_BASELINES[code_i].code, stats.load_failures_by_code[code_i]);
    }
    if (stats.load_failures_unexpected > 0) {
        printf("  load_failures[unexpected]=%ld\n", stats.load_failures_unexpected);
    }

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

    /* Any load failure whose error code isn't in LOAD_FAILURE_BASELINES at all is a
     * brand-new failure mode -- always a hard failure, never just a NOTE, regardless
     * of how the aggregate count moves. */
    if (stats.load_failures_unexpected > 0) {
        fprintf(stderr,
                "FAIL load_failures_unexpected: %ld load failure(s) used an error code not listed in "
                "LOAD_FAILURE_BASELINES -- see the FAIL lines above for examples; add a traced entry "
                "instead of ignoring it\n",
                stats.load_failures_unexpected);
        failures++;
    }
    for (code_i = 0; code_i < sizeof(LOAD_FAILURE_BASELINES) / sizeof(LOAD_FAILURE_BASELINES[0]); code_i++) {
        char label[64];
        snprintf(label, sizeof(label), "load_failures[err=%d]", LOAD_FAILURE_BASELINES[code_i].code);
        failures += check_baseline(stats.load_failures_by_code[code_i], LOAD_FAILURE_BASELINES[code_i].baseline, label);
    }
    failures += check_baseline(stats.bound_mismatches, BASELINE_BOUND_MISMATCHES, "bound_mismatches");
    failures += check_baseline(stats.dead_configs, BASELINE_DEAD_CONFIGS, "dead_configs");

    return failures;
}
