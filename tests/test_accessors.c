#include <dedx.h>
#include <dedx_error.h>
#include <math.h>
#include <stdio.h>

/* Tests for the public material/ion property accessors:
 *   dedx_get_nucleon_number, dedx_get_atom_mass, dedx_get_density, dedx_is_gas.
 * These promote previously-internal helpers to the public API (issue #119).
 */

static int faili(const char *label, int got, int expected) {
    if (got == expected)
        return 0;
    fprintf(stderr, "FAIL %s: got %d expected %d\n", label, got, expected);
    return 1;
}

static int failmsg(const char *label, const char *message) {
    fprintf(stderr, "FAIL %s: %s\n", label, message);
    return 1;
}

static int check_close(const char *label, double got, double expected, double rel) {
    double scale = fabs(expected) > 1e-12 ? fabs(expected) : 1.0;

    if (fabs(got - expected) > rel * scale) {
        fprintf(stderr, "FAIL %s: got %.8g expected %.8g\n", label, got, expected);
        return 1;
    }
    return 0;
}

int main(void) {
    int failures = 0;
    int err;
    int nucleons;
    float mass;
    float density;
    int gas;

    /* --- nucleon number: valid elements --- */
    err = -1;
    nucleons = dedx_get_nucleon_number(DEDX_HYDROGEN, &err);
    failures += faili("nucleon H err", err, DEDX_OK);
    failures += faili("nucleon H value", nucleons, 1);

    err = -1;
    nucleons = dedx_get_nucleon_number(DEDX_HELIUM, &err);
    failures += faili("nucleon He err", err, DEDX_OK);
    failures += faili("nucleon He value", nucleons, 4);

    err = -1;
    nucleons = dedx_get_nucleon_number(DEDX_CARBON, &err);
    failures += faili("nucleon C err", err, DEDX_OK);
    failures += faili("nucleon C value", nucleons, 12);

    /* --- nucleon number: out-of-range guards (lower bound the internal lacks) --- */
    err = DEDX_OK;
    nucleons = dedx_get_nucleon_number(0, &err);
    failures += faili("nucleon 0 err", err, DEDX_ERR_NOT_AN_ELEMENT);
    failures += faili("nucleon 0 sentinel", nucleons, -1);

    err = DEDX_OK;
    nucleons = dedx_get_nucleon_number(-5, &err);
    failures += faili("nucleon negative err", err, DEDX_ERR_NOT_AN_ELEMENT);
    failures += faili("nucleon negative sentinel", nucleons, -1);

    err = DEDX_OK;
    nucleons = dedx_get_nucleon_number(113, &err);
    failures += faili("nucleon 113 err", err, DEDX_ERR_NOT_AN_ELEMENT);
    failures += faili("nucleon 113 sentinel", nucleons, -1);

    /* --- atom mass: valid elements --- */
    err = -1;
    mass = dedx_get_atom_mass(DEDX_HYDROGEN, &err);
    failures += faili("mass H err", err, DEDX_OK);
    failures += check_close("mass H value", mass, 1.00794, 1e-4);

    err = -1;
    mass = dedx_get_atom_mass(DEDX_CARBON, &err);
    failures += faili("mass C err", err, DEDX_OK);
    failures += check_close("mass C value", mass, 12.0107, 1e-4);

    /* --- atom mass: out-of-range guards --- */
    err = DEDX_OK;
    mass = dedx_get_atom_mass(0, &err);
    failures += faili("mass 0 err", err, DEDX_ERR_NOT_AN_ELEMENT);
    if (mass != -1.0f)
        failures += failmsg("mass 0 sentinel", "expected -1 on out-of-range element");

    err = DEDX_OK;
    mass = dedx_get_atom_mass(200, &err);
    failures += faili("mass 200 err", err, DEDX_ERR_NOT_AN_ELEMENT);
    if (mass != -1.0f)
        failures += failmsg("mass 200 sentinel", "expected -1 on out-of-range element");

    /* --- density: known liquid and gaseous materials --- */
    err = -1;
    density = dedx_get_density(DEDX_WATER, &err);
    failures += faili("density water err", err, DEDX_OK);
    failures += check_close("density water value", density, 1.0, 0.02);

    err = -1;
    density = dedx_get_density(DEDX_AIR, &err);
    failures += faili("density air err", err, DEDX_OK);
    if (density <= 0.0f || density > 0.01f)
        failures += failmsg("density air value", "air density outside plausible gas range");

    /* --- density: unknown material returns a safe sentinel + error --- */
    err = DEDX_OK;
    density = dedx_get_density(9999, &err);
    failures += faili("density unknown err", err, DEDX_ERR_TARGET_NOT_FOUND);
    if (density != 0.0f)
        failures += failmsg("density unknown sentinel", "expected 0 density on unknown material");

    /* --- is_gas: liquid vs gas --- */
    err = -1;
    gas = dedx_is_gas(DEDX_WATER, &err);
    failures += faili("is_gas water err", err, DEDX_OK);
    failures += faili("is_gas water value", gas, 0);

    err = -1;
    gas = dedx_is_gas(DEDX_AIR, &err);
    failures += faili("is_gas air err", err, DEDX_OK);
    failures += faili("is_gas air value", gas, 1);

    /* Unknown target is reported as non-gas (documented behaviour). */
    err = -1;
    gas = dedx_is_gas(9999, &err);
    failures += faili("is_gas unknown value", gas, 0);

    if (failures == 0)
        printf("test_accessors: all checks passed\n");
    return failures;
}
