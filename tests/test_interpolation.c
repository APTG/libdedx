#include <dedx_data_access.h>
#include <dedx_spline.h>
#include <dedx_wrappers.h>
#include <string.h>

#include "test_helpers.h"

static int load_raw_table(int program, int ion, int target, stopping_data *data, float *energies) {
    int err = DEDX_OK;

    dedx_internal_read_binary_data(data, program, ion, target, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL raw-data load: program=%d ion=%d target=%d err=%d\n", program, ion, target, err);
        return 1;
    }

    dedx_internal_read_energy_data(energies, program, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL energy-grid load: program=%d ion=%d target=%d err=%d\n", program, ion, target, err);
        return 1;
    }

    return 0;
}

static double
interpolate_log_log_natural_spline(const float *energies, const float *stopping, unsigned int n, double energy) {
    double log_energy[DEDX_MAX_ELEMENTS];
    double log_stopping[DEDX_MAX_ELEMENTS];
    double h[DEDX_MAX_ELEMENTS];
    double alpha[DEDX_MAX_ELEMENTS];
    double l[DEDX_MAX_ELEMENTS];
    double mu[DEDX_MAX_ELEMENTS];
    double z[DEDX_MAX_ELEMENTS];
    double b[DEDX_MAX_ELEMENTS];
    double c[DEDX_MAX_ELEMENTS];
    double d[DEDX_MAX_ELEMENTS];
    unsigned int i;
    int interval = (int) n - 2;

    for (i = 0; i < n; i++) {
        log_energy[i] = log((double) energies[i]);
        log_stopping[i] = log((double) stopping[i]);
    }

    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;
    l[n - 1] = 1.0;
    z[n - 1] = 0.0;
    c[n - 1] = 0.0;

    for (i = 0; i < n - 1; i++) {
        h[i] = log_energy[i + 1] - log_energy[i];
    }
    for (i = 1; i < n - 1; i++) {
        alpha[i] = 3.0 / h[i] * (log_stopping[i + 1] - log_stopping[i])
                   - 3.0 / h[i - 1] * (log_stopping[i] - log_stopping[i - 1]);
    }
    for (i = 1; i < n - 1; i++) {
        l[i] = 2.0 * (log_energy[i + 1] - log_energy[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    for (i = n - 1; i-- > 0;) {
        c[i] = z[i] - mu[i] * c[i + 1];
        b[i] = (log_stopping[i + 1] - log_stopping[i]) / h[i] - h[i] * (c[i + 1] + 2.0 * c[i]) / 3.0;
        d[i] = (c[i + 1] - c[i]) / (3.0 * h[i]);
    }

    for (i = 0; i < n - 1; i++) {
        if (energies[i] <= energy && energy < energies[i + 1]) {
            interval = (int) i;
            break;
        }
    }

    {
        const double dx = log(energy) - log_energy[interval];
        const double log_value =
            log_stopping[interval] + b[interval] * dx + c[interval] * dx * dx + d[interval] * dx * dx * dx;

        return exp(log_value);
    }
}

static double
interpolate_linear_natural_spline(const float *energies, const float *stopping, unsigned int n, double energy) {
    double h[DEDX_MAX_ELEMENTS];
    double alpha[DEDX_MAX_ELEMENTS];
    double l[DEDX_MAX_ELEMENTS];
    double mu[DEDX_MAX_ELEMENTS];
    double z[DEDX_MAX_ELEMENTS];
    double b[DEDX_MAX_ELEMENTS];
    double c[DEDX_MAX_ELEMENTS];
    double d[DEDX_MAX_ELEMENTS];
    unsigned int i;
    int interval = (int) n - 2;

    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;
    l[n - 1] = 1.0;
    z[n - 1] = 0.0;
    c[n - 1] = 0.0;

    for (i = 0; i < n - 1; i++) {
        h[i] = (double) energies[i + 1] - (double) energies[i];
    }
    for (i = 1; i < n - 1; i++) {
        alpha[i] = 3.0 / h[i] * ((double) stopping[i + 1] - (double) stopping[i])
                   - 3.0 / h[i - 1] * ((double) stopping[i] - (double) stopping[i - 1]);
    }
    for (i = 1; i < n - 1; i++) {
        l[i] = 2.0 * ((double) energies[i + 1] - (double) energies[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }
    for (i = n - 1; i-- > 0;) {
        c[i] = z[i] - mu[i] * c[i + 1];
        b[i] = ((double) stopping[i + 1] - (double) stopping[i]) / h[i] - h[i] * (c[i + 1] + 2.0 * c[i]) / 3.0;
        d[i] = (c[i + 1] - c[i]) / (3.0 * h[i]);
    }

    for (i = 0; i < n - 1; i++) {
        if (energies[i] <= energy && energy < energies[i + 1]) {
            interval = (int) i;
            break;
        }
    }

    {
        const double dx = energy - (double) energies[interval];
        return (double) stopping[interval] + b[interval] * dx + c[interval] * dx * dx + d[interval] * dx * dx * dx;
    }
}

static int check_exact_tabulated_value(
    int api_program, int raw_program, int ion, int target, unsigned int point_index, const char *label) {
    dedx_workspace *ws = NULL;
    dedx_config *cfg = NULL;
    stopping_data raw_data;
    float energies[DEDX_MAX_ELEMENTS];
    int err = DEDX_OK;
    int cleanup_err = DEDX_OK;
    float result;
    float expected;
    float energy;

    memset(&raw_data, 0, sizeof(raw_data));
    if (load_raw_table(raw_program, ion, target, &raw_data, energies) != 0) {
        return 1;
    }
    if (point_index >= raw_data.length) {
        fprintf(stderr, "FAIL test setup: %s point_index=%u length=%u\n", label, point_index, raw_data.length);
        return 1;
    }

    cfg = calloc(1, sizeof(dedx_config));
    if (cfg == NULL) {
        fprintf(stderr, "FAIL alloc config: %s\n", label);
        return 1;
    }
    cfg->program = api_program;
    cfg->ion = ion;
    cfg->target = target;

    ws = dedx_allocate_workspace(1, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL alloc workspace: %s err=%d\n", label, err);
        dedx_free_config(cfg, &cleanup_err);
        return 1;
    }

    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL load config: %s err=%d\n", label, err);
        dedx_free_config(cfg, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return 1;
    }

    energy = energies[point_index];
    expected = raw_data.data[point_index];
    result = dedx_get_stp(ws, cfg, energy, &err);

    dedx_free_config(cfg, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);

    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL evaluate: %s err=%d\n", label, err);
        return 1;
    }
    if (result != expected) {
        fprintf(stderr, "FAIL exact knot: %s E=%.8g got %.8g expected %.8g\n", label, energy, result, expected);
        return 1;
    }
    return 0;
}

static int check_interpolation_mode(
    int api_program, int raw_program, int ion, int target, float energy, int interpolation_mode, const char *label) {
    stopping_data raw_data;
    float energies[DEDX_MAX_ELEMENTS];
    dedx_workspace *ws = NULL;
    dedx_config *cfg = NULL;
    float result;
    float expected;
    int err = DEDX_OK;
    int cleanup_err = DEDX_OK;

    memset(&raw_data, 0, sizeof(raw_data));
    if (load_raw_table(raw_program, ion, target, &raw_data, energies) != 0) {
        return 1;
    }

    if (interpolation_mode == DEDX_INTERPOLATION_LINEAR) {
        expected = (float) interpolate_linear_natural_spline(energies, raw_data.data, raw_data.length, energy);
    } else {
        expected = (float) interpolate_log_log_natural_spline(energies, raw_data.data, raw_data.length, energy);
    }

    cfg = calloc(1, sizeof(dedx_config));
    if (cfg == NULL) {
        fprintf(stderr, "FAIL alloc config: %s\n", label);
        return 1;
    }
    cfg->program = api_program;
    cfg->ion = ion;
    cfg->target = target;
    cfg->interpolation_mode = interpolation_mode;

    ws = dedx_allocate_workspace(1, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL alloc workspace: %s err=%d\n", label, err);
        dedx_free_config(cfg, &cleanup_err);
        return 1;
    }

    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL load config: %s err=%d\n", label, err);
        dedx_free_config(cfg, &cleanup_err);
        dedx_free_workspace(ws, &cleanup_err);
        return 1;
    }

    result = dedx_get_stp(ws, cfg, energy, &err);

    dedx_free_config(cfg, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);

    if (err != DEDX_OK) {
        fprintf(stderr,
                "FAIL interpolation eval: %s program=%d raw_program=%d ion=%d target=%d E=%.8g err=%d\n",
                label,
                api_program,
                raw_program,
                ion,
                target,
                energy,
                err);
        return 1;
    }
    if (fabsf(result - expected) / expected > 1e-4f) {
        fprintf(
            stderr,
            "FAIL interpolation mode: %s program=%d raw_program=%d ion=%d target=%d E=%.8g got %.8g expected %.8g\n",
            label,
            api_program,
            raw_program,
            ion,
            target,
            energy,
            result,
            expected);
        return 1;
    }
    return 0;
}

static int check_internal_two_point_spline_cache(void) {
    dedx_internal_spline_base coef[2];
    dedx_internal_lookup_accelerator acc;
    float energy[2] = {1.0f, 2.0f};
    float stopping[2] = {10.0f, 20.0f};
    float result;

    memset(coef, 0, sizeof(coef));
    memset(&acc, 0, sizeof(acc));

    dedx_internal_calculate_coefficients(coef, energy, stopping, 2, DEDX_INTERPOLATION_LINEAR);

    result = dedx_internal_evaluate_spline(coef, 2.0f, &acc, 2, DEDX_INTERPOLATION_LINEAR);
    if (result != 20.0f) {
        fprintf(stderr, "FAIL internal two-point knot: got %.8g expected 20\n", result);
        return 1;
    }
    if (acc.cache != 0) {
        fprintf(stderr, "FAIL internal two-point cache: got %d expected 0\n", acc.cache);
        return 1;
    }

    result = dedx_internal_evaluate_spline(coef, 1.5f, &acc, 2, DEDX_INTERPOLATION_LINEAR);
    if (fabsf(result - 15.0f) > 1e-6f) {
        fprintf(stderr, "FAIL internal two-point midpoint: got %.8g expected 15\n", result);
        return 1;
    }
    return 0;
}

int main(void) {
    int failures = 0;
    const float icru_old_midpoint = sqrtf(0.03f * 0.04f);

    failures += check_exact_tabulated_value(DEDX_PSTAR, DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 0, "PSTAR first point");
    failures +=
        check_exact_tabulated_value(DEDX_PSTAR, DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 27, "PSTAR interior point");
    failures += check_exact_tabulated_value(DEDX_PSTAR, DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 132, "PSTAR final point");

    failures += check_exact_tabulated_value(
        DEDX_ICRU73, DEDX_ICRU73_OLD, DEDX_ARGON, DEDX_PHOTOGRAPHIC_EMULSION, 1, "ICRU73 old-table point");
    failures += check_interpolation_mode(DEDX_ICRU73,
                                         DEDX_ICRU73_OLD,
                                         DEDX_ARGON,
                                         DEDX_PHOTOGRAPHIC_EMULSION,
                                         icru_old_midpoint,
                                         DEDX_INTERPOLATION_LOG_LOG,
                                         "ICRU73 default log-log");
    failures += check_interpolation_mode(DEDX_ICRU73,
                                         DEDX_ICRU73_OLD,
                                         DEDX_ARGON,
                                         DEDX_PHOTOGRAPHIC_EMULSION,
                                         icru_old_midpoint,
                                         DEDX_INTERPOLATION_LINEAR,
                                         "ICRU73 explicit linear");
    failures += check_internal_two_point_spline_cache();

    return failures;
}
