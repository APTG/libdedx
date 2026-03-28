#include <dedx.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int program;
    int ion;
    int target;
    const char *label;
} load_case;

static int run_load_case(const load_case *tc) {
    int err = 0;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);
    dedx_config *cfg = calloc(1, sizeof(dedx_config));

    if (err != 0 || ws == NULL || cfg == NULL) {
        fprintf(stderr, "setup failed for %s: err=%d\n", tc->label, err);
        dedx_free_config(cfg, &err);
        if (ws != NULL) {
            dedx_free_workspace(ws, &err);
        }
        return 1;
    }

    cfg->program = tc->program;
    cfg->ion = tc->ion;
    cfg->target = tc->target;

    dedx_load_config(ws, cfg, &err);
    if (err != 0) {
        fprintf(stderr, "dedx_load_config failed for %s: err=%d\n", tc->label, err);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);
    if (err != 0) {
        fprintf(stderr, "cleanup failed for %s: err=%d\n", tc->label, err);
        return 1;
    }

    return 0;
}

int main(void) {
    static const load_case cases[] = {
        {DEDX_ASTAR, DEDX_HELIUM, DEDX_WATER, "ASTAR helium/water"},
        {DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, "PSTAR proton/water"},
        {DEDX_MSTAR, DEDX_LITHIUM, DEDX_WATER, "MSTAR lithium/water"},
        {DEDX_ICRU49, DEDX_PROTON, DEDX_WATER, "ICRU49 proton/water"},
        {DEDX_ICRU49, DEDX_HELIUM, DEDX_WATER, "ICRU49 helium/water"},
        {DEDX_BETHE_EXT00, DEDX_HELIUM, DEDX_WATER, "BETHE helium/water"},
        {DEDX_ICRU73_OLD, DEDX_LITHIUM, DEDX_WATER, "ICRU73_OLD lithium/water"},
        {DEDX_ICRU73, DEDX_LITHIUM, DEDX_WATER, "ICRU73 lithium/water"},
    };

    int failures = 0;
    size_t i;

    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        failures += run_load_case(&cases[i]);
    }

    return failures;
}
