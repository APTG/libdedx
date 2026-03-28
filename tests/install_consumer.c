#include <dedx.h>
#include <dedx_tools.h>
#include <dedx_wrappers.h>
#include <stdio.h>

int main(void) {
    int err = 0;
    int major = 0;
    int minor = 0;
    int patch = 0;
    float stp;

    dedx_get_version(&major, &minor, &patch);
    stp = dedx_get_simple_stp(DEDX_PROTON, DEDX_WATER, 100.0f, &err);
    if (err != 0) {
        fprintf(stderr, "dedx_get_simple_stp failed: err=%d\n", err);
        return 1;
    }
    if (stp <= 0.0f) {
        fprintf(stderr, "unexpected stopping power: %f\n", stp);
        return 1;
    }

    printf("libdedx %d.%d.%d OK, stp=%f\n", major, minor, patch, stp);
    return 0;
}
