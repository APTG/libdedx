#include <dedx.h>
#include <dedx_error.h>
#include <math.h>
#include <stdio.h>

/* Tests for the public material/ion property accessors:
 *   dedx_get_nucleon_number, dedx_get_atom_mass, dedx_get_density, dedx_is_gas.
 * These promote previously-internal helpers to the public API (issue #119).
 *
 * All assertions funnel through the three expect_* helpers below so the only
 * lines that stay uncovered on a passing run are their (shared) failure paths.
 */

static int failures = 0;

static void expect_int(const char *label, long got, long expected) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %ld expected %ld\n", label, got, expected);
        failures++;
    }
}

static void expect_near(const char *label, double got, double expected, double rel) {
    double scale = fabs(expected) > 1e-12 ? fabs(expected) : 1.0;
    if (fabs(got - expected) > rel * scale) {
        fprintf(stderr, "FAIL %s: got %.8g expected %.8g\n", label, got, expected);
        failures++;
    }
}

static void expect_true(const char *label, int condition) {
    if (!condition) {
        fprintf(stderr, "FAIL %s\n", label);
        failures++;
    }
}

int main(void) {
    int err;
    float air;

    /* --- nucleon number: valid elements --- */
    err = -1;
    expect_int("nucleon H value", dedx_get_nucleon_number(DEDX_HYDROGEN, &err), 1);
    expect_int("nucleon H err", err, DEDX_OK);

    err = -1;
    expect_int("nucleon He value", dedx_get_nucleon_number(DEDX_HELIUM, &err), 4);
    expect_int("nucleon He err", err, DEDX_OK);

    err = -1;
    expect_int("nucleon C value", dedx_get_nucleon_number(DEDX_CARBON, &err), 12);
    expect_int("nucleon C err", err, DEDX_OK);

    /* Highest tabulated element (Z=112) must be accepted (upper boundary). */
    err = -1;
    expect_true("nucleon Z=112 value", dedx_get_nucleon_number(112, &err) > 0);
    expect_int("nucleon Z=112 err", err, DEDX_OK);

    /* --- nucleon number: out-of-range guards (lower bound the internal lacks) --- */
    err = DEDX_OK;
    expect_int("nucleon 0 value", dedx_get_nucleon_number(0, &err), -1);
    expect_int("nucleon 0 err", err, DEDX_ERR_NOT_AN_ELEMENT);

    err = DEDX_OK;
    expect_int("nucleon negative value", dedx_get_nucleon_number(-5, &err), -1);
    expect_int("nucleon negative err", err, DEDX_ERR_NOT_AN_ELEMENT);

    err = DEDX_OK;
    expect_int("nucleon 113 value", dedx_get_nucleon_number(113, &err), -1);
    expect_int("nucleon 113 err", err, DEDX_ERR_NOT_AN_ELEMENT);

    /* --- atom mass: valid elements --- */
    err = -1;
    expect_near("mass H value", dedx_get_atom_mass(DEDX_HYDROGEN, &err), 1.00794, 1e-4);
    expect_int("mass H err", err, DEDX_OK);

    err = -1;
    expect_near("mass C value", dedx_get_atom_mass(DEDX_CARBON, &err), 12.0107, 1e-4);
    expect_int("mass C err", err, DEDX_OK);

    err = -1;
    expect_near("mass O value", dedx_get_atom_mass(DEDX_OXYGEN, &err), 15.9994, 1e-4);
    expect_int("mass O err", err, DEDX_OK);

    /* --- atom mass: out-of-range guards --- */
    err = DEDX_OK;
    expect_true("mass 0 sentinel", dedx_get_atom_mass(0, &err) == -1.0f);
    expect_int("mass 0 err", err, DEDX_ERR_NOT_AN_ELEMENT);

    err = DEDX_OK;
    expect_true("mass 200 sentinel", dedx_get_atom_mass(200, &err) == -1.0f);
    expect_int("mass 200 err", err, DEDX_ERR_NOT_AN_ELEMENT);

    /* --- density: known liquid and gaseous materials --- */
    err = -1;
    expect_near("density water value", dedx_get_density(DEDX_WATER, &err), 1.0, 0.02);
    expect_int("density water err", err, DEDX_OK);

    err = -1;
    air = dedx_get_density(DEDX_AIR, &err);
    expect_true("density air range", air > 0.0f && air < 0.01f);
    expect_int("density air err", err, DEDX_OK);

    /* --- density: unknown material returns a safe sentinel + error --- */
    err = DEDX_OK;
    expect_true("density unknown sentinel", dedx_get_density(9999, &err) == 0.0f);
    expect_int("density unknown err", err, DEDX_ERR_TARGET_NOT_FOUND);

    /* --- is_gas: liquid vs gas --- */
    err = -1;
    expect_int("is_gas water value", dedx_is_gas(DEDX_WATER, &err), 0);
    expect_int("is_gas water err", err, DEDX_OK);

    err = -1;
    expect_int("is_gas air value", dedx_is_gas(DEDX_AIR, &err), 1);
    expect_int("is_gas air err", err, DEDX_OK);

    /* Unknown target is reported as non-gas, never an error (documented). */
    err = -1;
    expect_int("is_gas unknown value", dedx_is_gas(9999, &err), 0);
    expect_int("is_gas unknown err", err, DEDX_OK);

    if (failures == 0)
        printf("test_accessors: all checks passed\n");
    return failures;
}
