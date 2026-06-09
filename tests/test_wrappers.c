#include <dedx.h>
#include <dedx_wrappers.h>
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
    const float energy = 100.0f;
    double range = 0.0;
    float stp_high = 0.0f;
    double recovered;
    double peak;

    /* Reference CSDA range at 100 MeV/nucl for protons in water. */
    err = dedx_get_csda_range_table(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, 1, &energy, &range);
    failures += faili("csda_range_table err", err, DEDX_OK);

    /* Inverse CSDA flat wrapper should recover the original energy. */
    err = 0;
    recovered = dedx_get_inverse_csda_simple(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, range, &err);
    if (err != DEDX_OK) {
        failures += faili("inverse_csda_simple err", err, DEDX_OK);
    } else if (!approx(recovered, energy, 1e-3)) {
        failures += failf("inverse_csda_simple energy", recovered, energy);
    }

    /* Reference stopping power at 100 MeV/nucl (falling, high-energy branch). */
    err = 0;
    stp_high = dedx_get_simple_stp_for_program(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, energy, &err);
    failures += faili("simple_stp err", err, DEDX_OK);

    /* Inverse STP flat wrapper on the high-energy branch recovers the energy. */
    err = 0;
    recovered = dedx_get_inverse_stp_simple(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, stp_high, 1, &err);
    if (err != DEDX_OK) {
        failures += faili("inverse_stp_simple err", err, DEDX_OK);
    } else if (!approx(recovered, energy, 1e-2)) {
        failures += failf("inverse_stp_simple energy", recovered, energy);
    }

    /* The low-energy branch (side < 0) yields the rising-edge solution, which
       lies below the high-energy solution for the same stopping power. */
    err = 0;
    {
        double low_branch = dedx_get_inverse_stp_simple(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, stp_high, -1, &err);
        if (err != DEDX_OK) {
            failures += faili("inverse_stp_simple low-branch err", err, DEDX_OK);
        } else if (!(low_branch > 0.0 && low_branch < recovered)) {
            failures += failf("inverse_stp_simple low-branch energy", low_branch, recovered);
        }
    }

    /* Bragg peak stopping power must exceed the stopping power at 100 MeV. */
    err = 0;
    peak = dedx_get_bragg_peak_stp_simple(DEDX_PSTAR, DEDX_PROTON, DEDX_WATER, &err);
    if (err != DEDX_OK) {
        failures += faili("bragg_peak_stp_simple err", err, DEDX_OK);
    } else if (!(peak > stp_high)) {
        failures += failf("bragg_peak_stp_simple value", peak, stp_high);
    }

    /* Error path: an unsupported program/ion combination (PSTAR is
       proton-only) must report DEDX_ERR_ION_NOT_SUPPORTED and return -1 from
       every flat wrapper, exercising their allocation-cleanup branches. */
    err = 0;
    recovered = dedx_get_inverse_csda_simple(DEDX_PSTAR, DEDX_CARBON, DEDX_WATER, 1.0, &err);
    failures += faili("inverse_csda_simple unsupported err", err, DEDX_ERR_ION_NOT_SUPPORTED);
    if (!approx(recovered, -1.0, 1e-9))
        failures += failf("inverse_csda_simple unsupported return", recovered, -1.0);

    err = 0;
    recovered = dedx_get_inverse_stp_simple(DEDX_PSTAR, DEDX_CARBON, DEDX_WATER, 10.0, 1, &err);
    failures += faili("inverse_stp_simple unsupported err", err, DEDX_ERR_ION_NOT_SUPPORTED);
    if (!approx(recovered, -1.0, 1e-9))
        failures += failf("inverse_stp_simple unsupported return", recovered, -1.0);

    err = 0;
    recovered = dedx_get_bragg_peak_stp_simple(DEDX_PSTAR, DEDX_CARBON, DEDX_WATER, &err);
    failures += faili("bragg_peak_stp_simple unsupported err", err, DEDX_ERR_ION_NOT_SUPPORTED);
    if (!approx(recovered, -1.0, 1e-9))
        failures += failf("bragg_peak_stp_simple unsupported return", recovered, -1.0);

    return failures;
}
