#pragma once

#include <dedx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Accepted relative tolerance for stopping power comparisons (0.5%)
static const float DEDX_ERR_ACCEPT = 5e-3f;

static inline int check_result(float result, float expected) {
    return fabsf(result - expected) / expected > DEDX_ERR_ACCEPT;
}

static inline int check_config_stp(dedx_config *cfg, float energy, float expected, const char *label) {
    int err = 0;
    const int program = cfg->program;
    const int ion = cfg->ion;
    const int target = cfg->target;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);

    dedx_load_config(ws, cfg, &err);
    if (err != 0) {
        fprintf(stderr, "FAIL load: %s program=%d ion=%d target=%d err=%d\n", label, program, ion, target, err);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    float result = (float) dedx_get_stp(ws, cfg, energy, &err);
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    if (check_result(result, expected)) {
        fprintf(stderr,
                "FAIL stp: %s program=%d ion=%d target=%d E=%.3e MeV/u: got %.5e expected %.5e\n",
                label,
                program,
                ion,
                target,
                energy,
                result,
                expected);
        return 1;
    }
    return 0;
}

// Check stopping power for a simple program/ion/target/energy combination.
// Returns 0 on pass, 1 on fail.
static inline int check_stp(int program, int ion, int target, float energy, float expected) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;
    return check_config_stp(cfg, energy, expected, "simple");
}
