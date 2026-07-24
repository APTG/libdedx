#include <dedx.h>
#include <dedx_error.h>
#include <dedx_tools.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Regression tests for dedx_get_inverse_stp() and dedx_get_max_stp()
 * (issue #121). The previous implementation used find_min() over a hardcoded
 * x in [0.01, 10] to locate the energy of maximum stopping power, which does
 * not match the real tabulated energy range; it also mapped side < 0 (never
 * 0 or 1) to the low-energy branch, so side == 0 and side == 1 were
 * indistinguishable. Both bugs made dedx_get_inverse_stp() fail with
 * DEDX_ERR_ENERGY_OUT_OF_RANGE for ordinary proton/water queries.
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

    /* --- Below the ascending branch's floor: only the descending branch can
     * reach this STP, exactly the scenario that made the old find_min()-based
     * code return DEDX_ERR_ENERGY_OUT_OF_RANGE for proton/water. Both sides
     * must now fall back to the same (only reachable) descending-branch
     * energy, with no error and a positive result. */
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

    /* --- Interior peak in the STP curve, STP reachable on both branches:
     * side=0 (low energy / ascending) and side=1 (high energy / descending)
     * must select distinct branches and each round-trip back to the
     * requested STP. */
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

    /* --- Out-of-range STP values (above the maximum stopping power, or
     * below the STP at max energy) must return an error rather than looping
     * to a bogus or negative energy. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);

        err = 0;
        double e_too_high = dedx_get_inverse_stp(ws, cfg, 2000.0f, 0, &err);
        expect_int("stp above peak err", err, DEDX_ERR_ENERGY_OUT_OF_RANGE);
        expect_near("stp above peak sentinel", e_too_high, -1.0, 1e-9);

        err = 0;
        double e_too_low = dedx_get_inverse_stp(ws, cfg, 0.5f, 0, &err);
        expect_int("stp below max-energy floor err", err, DEDX_ERR_ENERGY_OUT_OF_RANGE);
        expect_near("stp below floor sentinel", e_too_low, -1.0, 1e-9);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
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

    /* --- dedx_get_max_stp() reports the true maximum of the curve. */
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = make_config(DEDX_PSTAR, DEDX_PROTON, 1, DEDX_WATER);
        err = 0;
        double peak = dedx_get_max_stp(ws, cfg, &err);
        expect_int("max stp err", err, DEDX_OK);

        /* Brute-force reference: sample far more densely and take the max. */
        float emin = dedx_get_min_energy(cfg->program, cfg->ion);
        float emax = dedx_get_max_energy(cfg->program, cfg->ion);
        double log_emin = log((double) emin);
        double log_emax = log((double) emax);
        double reference_peak = 0.0;
        const int n = 2000;
        for (int i = 0; i < n; i++) {
            float e = (float) exp(log_emin + (log_emax - log_emin) * i / (n - 1));
            int sample_err = 0;
            double s = dedx_get_stp(ws, cfg, e, &sample_err);
            if (sample_err == 0 && s > reference_peak)
                reference_peak = s;
        }
        expect_near("max stp matches brute-force reference", peak, reference_peak, 5e-2);
        expect_true("max stp is a real maximum", peak > 0.0);

        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    return failures;
}
