#include <dedx.h>
#include <dedx_error.h>
#include <dedx_tools.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Regression tests for dedx_get_inverse_stp(), dedx_get_max_stp(), and
 * dedx_get_min_stp() (issue #121).
 *
 * The stopping-power-vs-energy curve is not simply unimodal: PSTAR/ICRU
 * proton tables extend to 10 GeV/nucleon, far enough to show the curve rise
 * to the Bragg peak (~0.08 MeV), fall through a minimum-ionizing point
 * (~3000-4000 MeV), and rise again (relativistic rise / Fermi plateau) up to
 * the table's max energy. A requested STP in that high-energy dip can be
 * reachable at two distinct energies, on top of the usual two solutions
 * around the Bragg peak. dedx_get_inverse_stp() finds every monotonic run in
 * the exact tabulated knots (no arbitrary sampling density) and returns the
 * lowest-energy (side=0) or highest-energy (side=1) reachable solution.
 */

static int failures = 0;

static void expect_int(const char *label, long got, long expected) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %ld expected %ld\n", label, got, expected);
        failures++;
    }
}

static void expect_near(const char *label, double got, double expected, double rel) {
    double scale = fabs(expected) > 1e-12 ? fabs(expected) : 1.0;
    if (fabs(got - expected) > rel * scale) {
        fprintf(stderr, "FAIL %s: got %.8g expected %.8g\n", label, got, expected);
        failures++;
    }
}

static void expect_true(const char *label, int condition) {
    if (!condition) {
        fprintf(stderr, "FAIL %s\n", label);
        failures++;
    }
}

static dedx_config *make_config(int program, int ion, int ion_a, int target) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;
    cfg->ion_a = ion_a;
    return cfg;
}

int main(void) {
    int err = 0;

    /* --- STP below the Bragg peak's ascending floor: only the post-peak
     * descending run reaches it, so side=0 and side=1 must agree on the
     * same (only reachable) energy, with no error. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        expect_int("workspace alloc err", err, DEDX_OK);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);
        err = 0;
        double e0 = dedx_get_inverse_stp(ws, cfg, 100.0f, 0, &err);
        expect_int("low-floor stp side=0 err", err, DEDX_OK);
        expect_true("low-floor stp side=0 positive", e0 > 0.0);

        err = 0;
        double e1 = dedx_get_inverse_stp(ws, cfg, 100.0f, 1, &err);
        expect_int("low-floor stp side=1 err", err, DEDX_OK);
        expect_near("low-floor stp side=0/1 agree", e0, e1, 1e-3);

        int verify_err = 0;
        double stp_at_e0 = dedx_get_stp(ws, cfg, (float) e0, &verify_err);
        expect_int("low-floor roundtrip err", verify_err, DEDX_OK);
        expect_near("low-floor roundtrip stp", stp_at_e0, 100.0, 1e-2);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- Around the Bragg peak, STP reachable on both the ascending
     * (pre-peak) and descending (post-peak) runs: side=0 and side=1 must
     * select distinct energies and each round-trip back to the requested
     * STP. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);
        const float target_stp = 500.0f;

        err = 0;
        double e_low = dedx_get_inverse_stp(ws, cfg, target_stp, 0, &err);
        expect_int("branch stp side=0 err", err, DEDX_OK);

        err = 0;
        double e_high = dedx_get_inverse_stp(ws, cfg, target_stp, 1, &err);
        expect_int("branch stp side=1 err", err, DEDX_OK);

        expect_true("side=0 selects the lower energy", e_low < e_high);

        int verify_err = 0;
        double stp_low = dedx_get_stp(ws, cfg, (float) e_low, &verify_err);
        expect_int("side=0 roundtrip err", verify_err, DEDX_OK);
        expect_near("side=0 roundtrip stp", stp_low, target_stp, 1e-2);

        verify_err = 0;
        double stp_high = dedx_get_stp(ws, cfg, (float) e_high, &verify_err);
        expect_int("side=1 roundtrip err", verify_err, DEDX_OK);
        expect_near("side=1 roundtrip stp", stp_high, target_stp, 1e-2);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- Relativistic-rise dip around the minimum-ionizing point (~3000-4000
     * MeV for proton/water PSTAR): a target STP in this dip is reachable at
     * two widely separated energies, one on each side of the minimum. This
     * is exactly the region the old find_min()-based implementation got
     * wrong -- it assumed the whole post-peak range was a single descending
     * branch and rejected every STP here as out of range. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);
        const float target_stp = 2.05f;

        err = 0;
        double e_low = dedx_get_inverse_stp(ws, cfg, target_stp, 0, &err);
        expect_int("dip stp side=0 err", err, DEDX_OK);

        err = 0;
        double e_high = dedx_get_inverse_stp(ws, cfg, target_stp, 1, &err);
        expect_int("dip stp side=1 err", err, DEDX_OK);

        expect_true("dip side=0 selects the lower energy", e_low < e_high);
        expect_true("dip side=0 energy is in the pre-minimum range", e_low > 100.0 && e_low < 3000.0);
        expect_true("dip side=1 energy is in the post-minimum range", e_high > 3000.0 && e_high < 10000.0);

        int verify_err = 0;
        double stp_low = dedx_get_stp(ws, cfg, (float) e_low, &verify_err);
        expect_int("dip side=0 roundtrip err", verify_err, DEDX_OK);
        expect_near("dip side=0 roundtrip stp", stp_low, target_stp, 1e-2);

        verify_err = 0;
        double stp_high = dedx_get_stp(ws, cfg, (float) e_high, &verify_err);
        expect_int("dip side=1 roundtrip err", verify_err, DEDX_OK);
        expect_near("dip side=1 roundtrip stp", stp_high, target_stp, 1e-2);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- Out-of-range STP values (above the global maximum, or below the
     * global minimum) must return an error rather than looping to a bogus or
     * negative energy. The bounds are derived from dedx_get_max_stp()/
     * dedx_get_min_stp() themselves rather than hardcoded constants, so this
     * test stays valid if the underlying tabulated data is ever updated. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);

        err = 0;
        double max_stp = dedx_get_max_stp(ws, cfg, &err);
        expect_int("out-of-range setup: max stp err", err, DEDX_OK);
        err = 0;
        double min_stp = dedx_get_min_stp(ws, cfg, &err);
        expect_int("out-of-range setup: min stp err", err, DEDX_OK);

        float stp_above_max = (float) (max_stp * 2.0);
        float stp_below_min = (float) (min_stp * 0.5);

        err = 0;
        double e_too_high = dedx_get_inverse_stp(ws, cfg, stp_above_max, 0, &err);
        expect_int("stp above global max err", err, DEDX_ERR_ENERGY_OUT_OF_RANGE);
        expect_near("stp above global max sentinel", e_too_high, -1.0, 1e-9);

        err = 0;
        double e_too_low = dedx_get_inverse_stp(ws, cfg, stp_below_min, 0, &err);
        expect_int("stp below global min err", err, DEDX_ERR_ENERGY_OUT_OF_RANGE);
        expect_near("stp below global min sentinel", e_too_low, -1.0, 1e-9);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- A config that fails to load (here: a Bethe-type program with a
     * custom target and no density set, DEDX_ERR_RHO_REQUIRED -- the same
     * failure test_error_codes.c uses) must propagate that error cleanly out
     * of all three functions that route through get_loaded_dataset(), not
     * crash or silently return a bogus value. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_BETHE_EXT00, DEDX_PROTON, 1, 0);

        err = 0;
        double e = dedx_get_inverse_stp(ws, cfg, 100.0f, 0, &err);
        expect_true("unloadable config: inverse_stp err set", err != DEDX_OK);
        expect_near("unloadable config: inverse_stp sentinel", e, -1.0, 1e-9);

        err = 0;
        double max_stp = dedx_get_max_stp(ws, cfg, &err);
        expect_true("unloadable config: max_stp err set", err != DEDX_OK);
        expect_near("unloadable config: max_stp sentinel", max_stp, -1.0, 1e-9);

        err = 0;
        double min_stp = dedx_get_min_stp(ws, cfg, &err);
        expect_true("unloadable config: min_stp err set", err != DEDX_OK);
        expect_near("unloadable config: min_stp sentinel", min_stp, -1.0, 1e-9);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- A config already loaded into a workspace that was since freed and
     * replaced (config->loaded == 1, but config->cfg_id is not a valid slot
     * in the new workspace) must be transparently reloaded into the new
     * workspace rather than erroring out -- get_loaded_dataset() detects the
     * stale cfg_id and reloads instead of trusting the loaded flag alone. */
    {
        dedx_workspace *ws1 = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);

        err = 0;
        double max_stp_ws1 = dedx_get_max_stp(ws1, cfg, &err);
        expect_int("stale workspace: first load err", err, DEDX_OK);
        expect_true("stale workspace: cfg marked loaded", cfg->loaded != 0);

        dedx_free_workspace(ws1, &err);

        dedx_workspace *ws2 = dedx_allocate_workspace(1, &err);
        err = 0;
        double max_stp_ws2 = dedx_get_max_stp(ws2, cfg, &err);
        expect_int("stale workspace: reload into ws2 err", err, DEDX_OK);
        expect_near("stale workspace: same result after reload", max_stp_ws2, max_stp_ws1, 1e-9);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws2, &err);
    }

    /* --- ion_a <= 0 is rejected up front, same contract as before. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 0, DEDX_WATER);
        err = 0;
        double e = dedx_get_inverse_stp(ws, cfg, 100.0f, 0, &err);
        expect_int("ion_a required err", err, DEDX_ERR_ION_A_REQUIRED);
        expect_near("ion_a required sentinel", e, -1.0, 1e-9);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    /* --- dedx_get_max_stp() / dedx_get_min_stp() report the true extrema of
     * the curve: the Bragg peak and the minimum-ionizing point. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);

        err = 0;
        double max_stp = dedx_get_max_stp(ws, cfg, &err);
        expect_int("max stp err", err, DEDX_OK);

        err = 0;
        double min_stp = dedx_get_min_stp(ws, cfg, &err);
        expect_int("min stp err", err, DEDX_OK);

        expect_true("min stp is less than max stp", min_stp < max_stp);
        expect_true("min stp is positive", min_stp > 0.0);

        /* Brute-force reference: sample far more densely than the table
         * itself and take the min/max, as an independent cross-check. */
        float emin = dedx_get_min_energy(cfg->program, cfg->ion);
        float emax = dedx_get_max_energy(cfg->program, cfg->ion);
        double log_emin = log((double) emin);
        double log_emax = log((double) emax);
        double reference_max = 0.0;
        double reference_min = HUGE_VAL;
        const int n = 5000;
        for (int i = 0; i < n; i++) {
            float e = (float) exp(log_emin + (log_emax - log_emin) * i / (n - 1));
            int sample_err = 0;
            double s = dedx_get_stp(ws, cfg, e, &sample_err);
            if (sample_err != 0)
                continue;
            if (s > reference_max)
                reference_max = s;
            if (s < reference_min)
                reference_min = s;
        }
        expect_near("max stp matches brute-force reference", max_stp, reference_max, 5e-2);
        expect_near("min stp matches brute-force reference", min_stp, reference_min, 5e-2);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    return failures;
}
