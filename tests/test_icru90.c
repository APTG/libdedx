#include "test_helpers.h"

// Generic ICRU should use ICRU90 carbon data where available.

int main(void) {
    int failures = 0;

    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_AIR, 5.000000e-04f, 3.091000e+02f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_WATER, 5.000000e-04f, 3.776000e+02f);
    failures += check_stp(DEDX_ICRU, DEDX_CARBON, DEDX_GRAPHITE, 5.000000e-04f, 6.836000e+02f);

    return failures;
}
