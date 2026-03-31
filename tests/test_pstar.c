#include "test_helpers.h"

// Reference values from the NIST PSTAR database: https://physics.nist.gov/PhysRefData/Star/Text/PSTAR.html

int main(void) {
    int failures = 0;

    // Proton stopping power in water
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 0.07f, 8.183e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 1.0f, 2.606e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 10.0f, 4.564e1f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 78.0f, 8.791e0f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 1000.0f, 2.211e0f);

    // Proton stopping power in PMMA
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_PMMA, 0.07f, 9.319e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_PMMA, 1.0f, 2.530e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_PMMA, 10.0f, 4.450e1f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_PMMA, 78.0f, 8.558e0f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_PMMA, 1000.0f, 2.145e0f);

    // Proton stopping power in alanine
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_ALANINE, 0.07f, 9.324e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_ALANINE, 1.0f, 2.581e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_ALANINE, 10.0f, 4.495e1f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_ALANINE, 78.0f, 8.617e0f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_ALANINE, 1000.0f, 2.148e0f);

    // Proton stopping power in nitrogen (gas target — exercises _dedx_target_is_gas)
    // Reference values from NIST PSTAR database
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_NITROGEN, 0.07f, 7.624e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_NITROGEN, 1.0f, 2.260e2f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_NITROGEN, 10.0f, 4.043e1f);
    failures += check_stp(DEDX_PSTAR, DEDX_PROTON, DEDX_NITROGEN, 100.0f, 6.485e0f);

    return failures;
}
