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
    }

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

    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);
    return failures;
}
