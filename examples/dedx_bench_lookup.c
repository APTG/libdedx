#include <dedx.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double now_seconds(void) {
    return (double) clock() / (double) CLOCKS_PER_SEC;
}

static uint32_t lcg_next(uint32_t *state) {
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

static int parse_int_arg(const char *text, int fallback) {
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (end == text || *end != '\0' || value <= 0) {
        return fallback;
    }
    return (int) value;
}

static void configure_default(dedx_config *cfg, int program, int ion, int target, int interpolation_mode) {
    cfg->program = program;
    cfg->ion = ion;
    cfg->target = target;
    cfg->interpolation_mode = interpolation_mode;
}

static int run_benchmark(dedx_workspace *ws,
                         dedx_config *cfg,
                         int iterations,
                         float min_energy,
                         float max_energy,
                         const char *label) {
    double elapsed;
    double log_min_energy = log((double) min_energy);
    double log_span = log((double) max_energy) - log_min_energy;
    uint32_t state = 0x12345678u;
    volatile float sink = 0.0f;
    int err = DEDX_OK;
    int i;
    double t0;

    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL load %s err=%d\n", label, err);
        return 1;
    }

    t0 = now_seconds();
    for (i = 0; i < iterations; i++) {
        const double u = ((double) (lcg_next(&state) >> 8) + 0.5) / 16777216.0;
        const float energy = (float) exp(log_min_energy + u * log_span);
        sink += dedx_get_stp(ws, cfg, energy, &err);
        if (err != DEDX_OK) {
            fprintf(stderr, "FAIL lookup %s iter=%d err=%d\n", label, i, err);
            return 1;
        }
    }
    elapsed = now_seconds() - t0;

    printf("%s\n", label);
    printf("  iterations     : %d\n", iterations);
    printf("  elapsed_s      : %.6f\n", elapsed);
    printf("  ns_per_lookup  : %.2f\n", elapsed * 1e9 / (double) iterations);
    printf("  lookups_per_s  : %.0f\n", (double) iterations / elapsed);
    printf("  checksum       : %.8e\n", sink);

    return 0;
}

int main(int argc, char **argv) {
    const int iterations = (argc > 1) ? parse_int_arg(argv[1], 1000000) : 1000000;
    const int program = (argc > 2) ? parse_int_arg(argv[2], DEDX_PSTAR) : DEDX_PSTAR;
    const int ion = (argc > 3) ? parse_int_arg(argv[3], DEDX_PROTON) : DEDX_PROTON;
    const int target = (argc > 4) ? parse_int_arg(argv[4], DEDX_WATER) : DEDX_WATER;
    const float min_energy = dedx_get_min_energy(program, ion);
    const float max_energy = dedx_get_max_energy(program, ion);
    dedx_workspace *ws = NULL;
    dedx_config *cfg_log = NULL;
    dedx_config *cfg_linear = NULL;
    int err = DEDX_OK;
    int cleanup_err = DEDX_OK;
    int failures = 0;

    if (min_energy <= 0.0f || max_energy <= min_energy) {
        fprintf(stderr,
                "FAIL invalid energy range for program=%d ion=%d: min=%.8g max=%.8g\n",
                program,
                ion,
                min_energy,
                max_energy);
        return 1;
    }

    ws = dedx_allocate_workspace(2, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL allocate workspace err=%d\n", err);
        return 1;
    }

    cfg_log = calloc(1, sizeof(dedx_config));
    cfg_linear = calloc(1, sizeof(dedx_config));
    if (cfg_log == NULL || cfg_linear == NULL) {
        fprintf(stderr, "FAIL allocate config\n");
        free(cfg_log);
        free(cfg_linear);
        dedx_free_workspace(ws, &cleanup_err);
        return 1;
    }

    configure_default(cfg_log, program, ion, target, DEDX_INTERPOLATION_LOG_LOG);
    configure_default(cfg_linear, program, ion, target, DEDX_INTERPOLATION_LINEAR);

    printf("Benchmarking libdedx lookups\n");
    printf("  program        : %d (%s)\n", program, dedx_get_program_name(program));
    printf("  ion            : %d (%s)\n", ion, dedx_get_ion_name(ion));
    printf("  target         : %d (%s)\n", target, dedx_get_material_name(target));
    printf("  energy_range   : [%.8g, %.8g] MeV/u\n", min_energy, max_energy);

    failures += run_benchmark(ws, cfg_log, iterations, min_energy, max_energy, "mode=log-log");
    failures += run_benchmark(ws, cfg_linear, iterations, min_energy, max_energy, "mode=linear");

    dedx_free_config(cfg_log, &cleanup_err);
    dedx_free_config(cfg_linear, &cleanup_err);
    dedx_free_workspace(ws, &cleanup_err);

    return failures;
}
