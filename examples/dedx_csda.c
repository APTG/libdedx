#include <dedx.h>
#include <dedx_tools.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int err = 0;
    float energy = 100.0f; /* MeV/nucl */
    double csda;
    dedx_workspace *ws;
    dedx_config *cfg;

    ws = dedx_allocate_workspace(1, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "failed to allocate workspace: err=%d\n", err);
        return 1;
    }

    cfg = (dedx_config *) calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->ion_a = 1; /* nucleon number — required by dedx_get_csda */
    cfg->target = DEDX_WATER;

    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "failed to load config: err=%d\n", err);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    csda = dedx_get_csda(ws, cfg, energy, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "dedx_get_csda failed: err=%d\n", err);
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
        return 1;
    }

    printf("CSDA range at %g MeV/nucl: %6.3E cm^2/g\n", energy, csda);

    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);
    return 0;
}
