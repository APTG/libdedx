#include <dedx.h>
#include <dedx_tools.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int failf(const char *label, double got, double expected) {
    fprintf(stderr, "FAIL %s: got %.8g expected %.8g\n", label, got, expected);
    return 1;
}

static int faili(const char *label, int got, int expected) {
    if (got == expected)
        return 0;
    fprintf(stderr, "FAIL %s: got %d expected %d\n", label, got, expected);
    return 1;
}

static int approx(double a, double b, double rel) {
    double scale = fabs(b) > 1e-12 ? fabs(b) : 1.0;
    return fabs(a - b) <= rel * scale;
}

int main(void) {
    int failures = 0;
    int err = 0;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    const float energy = 100.0f;
    double csda;
    double inverse_csda;
    double stp;
    double bragg_peak;
    float old_values[2] = {1.0f, 2.0f};
    float new_values[2] = {0.0f, 0.0f};

    if (ws == NULL || cfg == NULL || err != DEDX_OK) {
        fprintf(stderr, "setup failed: err=%d\n", err);
        dedx_free_config(cfg, &err);
        if (ws != NULL)
            dedx_free_workspace(ws, &err);
        return 1;
    }

    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    cfg->ion_a = 1;

    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "dedx_load_config failed: err=%d\n", err);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    csda = dedx_get_csda(ws, cfg, energy, &err);
    if (err != DEDX_OK) {
        failures += faili("dedx_get_csda err", err, DEDX_OK);
    }

    inverse_csda = dedx_get_inverse_csda(ws, cfg, (float) csda, &err);
    if (err != DEDX_OK) {
        failures += faili("dedx_get_inverse_csda err", err, DEDX_OK);
    } else if (!approx(inverse_csda, energy, 1e-3)) {
        failures += failf("inverse csda energy", inverse_csda, energy);
    }

    stp = dedx_get_stp(ws, cfg, energy, &err);
    if (err != DEDX_OK) {
        failures += faili("dedx_get_stp err", err, DEDX_OK);
    } else if (!(stp > 0.0)) {
        failures += failf("dedx_get_stp value", stp, 1.0);
    }

    /* The Bragg-peak stopping power is the maximum of the curve, so it must
       exceed the stopping power at 100 MeV (far up the high-energy branch). */
    bragg_peak = dedx_get_bragg_peak_stp(ws, cfg, &err);
    if (err != DEDX_OK) {
        failures += faili("dedx_get_bragg_peak_stp err", err, DEDX_OK);
    } else if (!(bragg_peak > stp)) {
        failures += failf("dedx_get_bragg_peak_stp value", bragg_peak, stp);
    }

    /* Without a nucleon number the Bragg-peak helper must refuse to run. */
    cfg->ion_a = 0;
    err = DEDX_OK;
    bragg_peak = dedx_get_bragg_peak_stp(ws, cfg, &err);
    failures += faili("dedx_get_bragg_peak_stp ion_a guard", err, DEDX_ERR_ION_A_REQUIRED);
    cfg->ion_a = 1;

    err = convert_units(DEDX_MEVCM2G, DEDX_KEVUM, DEDX_WATER, 2, old_values, new_values);
    if (err != DEDX_OK) {
        failures += faili("convert_units err", err, DEDX_OK);
    } else {
        if (!approx(new_values[0], 0.1, 1e-6))
            failures += failf("convert_units first", new_values[0], 0.1);
        if (!approx(new_values[1], 0.2, 1e-6))
            failures += failf("convert_units second", new_values[1], 0.2);
    }

    old_values[0] = 1.0f;
    old_values[1] = 2.0f;
    new_values[0] = 0.0f;
    new_values[1] = 0.0f;
    err = convert_units(DEDX_KEVUM, DEDX_MEVCM2G, DEDX_WATER, 2, old_values, new_values);
    if (err != DEDX_OK) {
        failures += faili("convert_units reverse err", err, DEDX_OK);
    } else {
        if (!approx(new_values[0], 10.0, 1e-6))
            failures += failf("convert_units reverse first", new_values[0], 10.0);
        if (!approx(new_values[1], 20.0, 1e-6))
            failures += failf("convert_units reverse second", new_values[1], 20.0);
    }

    /* The Bragg-peak helper loads an unloaded configuration automatically, as
       documented; verify that documented behavior on a fresh config. */
    {
        int err2 = 0;
        dedx_config *cfg2 = calloc(1, sizeof(dedx_config));
        dedx_workspace *ws2 = dedx_allocate_workspace(1, &err2);
        if (cfg2 != NULL && ws2 != NULL && err2 == DEDX_OK) {
            cfg2->program = DEDX_PSTAR;
            cfg2->ion = DEDX_PROTON;
            cfg2->target = DEDX_WATER;
            cfg2->ion_a = 1;
            /* deliberately not loaded; the helper must load it itself */
            bragg_peak = dedx_get_bragg_peak_stp(ws2, cfg2, &err2);
            if (err2 != DEDX_OK)
                failures += faili("bragg auto-load err", err2, DEDX_OK);
            else if (!(bragg_peak > 0.0))
                failures += failf("bragg auto-load value", bragg_peak, 1.0);
        } else {
            failures += faili("bragg auto-load setup", err2, DEDX_OK);
        }
        dedx_free_config(cfg2, &err2);
        dedx_free_workspace(ws2, &err2);
    }

    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);
    return failures;
}
