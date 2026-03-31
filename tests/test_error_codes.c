#include <dedx.h>
#include <dedx_tools.h>
#include <stdio.h>
#include <stdlib.h>

static int check_err(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got err=%d, expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

int main(void) {
    int failures = 0;
    int err = 0;
    dedx_workspace *ws;
    dedx_config *cfg;

    /* dedx_get_csda requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    /* ion_a intentionally left at 0 */
    err = 0;
    dedx_get_csda(ws, cfg, 1.0f, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "csda without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_inverse_csda requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    err = 0;
    dedx_get_inverse_csda(ws, cfg, 10.0f, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "inverse_csda without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_inverse_stp requires ion_a > 0 */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    err = 0;
    dedx_get_inverse_stp(ws, cfg, 100.0f, -1, &err);
    failures += check_err(err, DEDX_ERR_ION_A_REQUIRED, "inverse_stp without ion_a");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* BETHE with target=0 and no rho: density is required */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_DEFAULT;
    cfg->ion = DEDX_PROTON;
    cfg->target = 0;
    /* rho intentionally left at 0 */
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_RHO_REQUIRED, "BETHE custom target without rho");
    failures += check_err(cfg->loaded, 0, "failed load should not mark config loaded");
    failures += check_err(cfg->cfg_id, -1, "failed load should not assign cfg_id");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* dedx_get_stp must reject invalid dataset ids before dereferencing */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->cfg_id = 0;
    err = 0;
    dedx_get_stp(ws, cfg, 1.0f, &err);
    failures += check_err(err, DEDX_ERR_INVALID_DATASET_ID, "stp with invalid dataset id");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}
