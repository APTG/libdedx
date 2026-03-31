#include <dedx_error.h>
#include <string.h>

#include "dedx_mpaul.h"
#include "test_helpers.h"

static int faili(const char *label, int got, int expected) {
    fprintf(stderr, "FAIL %s: got %d expected %d\n", label, got, expected);
    return 1;
}

static int failf(const char *label, double got, double expected) {
    fprintf(stderr, "FAIL %s: got %.8g expected %.8g\n", label, got, expected);
    return 1;
}

static int failmsg(const char *label, const char *message) {
    fprintf(stderr, "FAIL %s: %s\n", label, message);
    return 1;
}

static int check_string_present(const char *label, const char *value) {
    if (value == NULL || value[0] == '\0')
        return failmsg(label, "missing string");
    return 0;
}

static int check_close(const char *label, double got, double expected, double rel) {
    double scale = fabs(expected) > 1e-12 ? fabs(expected) : 1.0;

    if (fabs(got - expected) > rel * scale)
        return failf(label, got, expected);
    return 0;
}

int main(void) {
    int failures = 0;
    int err = 0;
    int major = -1;
    int minor = -1;
    int patch = -1;
    const int *programs;
    const int *materials;
    const int *ions;
    float composition[20][2];
    unsigned int comp_len = 0;
    float i_value;
    float simple_stp;
    dedx_workspace *ws;
    dedx_config *cfg;
    float direct_stp;
    float coef_c;
    float coef_d_low_z;
    float coef_g;
    float coef_h_low_z;
    float coef_h;

    dedx_get_version(&major, &minor, &patch);
    if (major < 0 || minor < 0 || patch < 0)
        failures += failmsg("version", "negative version component");

    failures += check_string_present("program name", dedx_get_program_name(DEDX_PSTAR));
    failures += check_string_present("program version", dedx_get_program_version(DEDX_PSTAR));
    failures += check_string_present("material name", dedx_get_material_name(DEDX_WATER));
    failures += check_string_present("ion name", dedx_get_ion_name(DEDX_PROTON));

    programs = dedx_get_program_list();
    if (programs == NULL || programs[0] < 0)
        failures += failmsg("program list", "missing program list");

    materials = dedx_get_material_list(DEDX_PSTAR);
    if (materials == NULL || materials[0] < 0)
        failures += failmsg("material list", "missing material list");

    ions = dedx_get_ion_list(DEDX_DEFAULT);
    if (ions == NULL || ions[0] != 1 || ions[111] != 112 || ions[112] != -1)
        failures += failmsg("default ion list", "unexpected ion list contents");

    dedx_get_composition(DEDX_WATER, composition, &comp_len, &err);
    if (err != DEDX_OK) {
        failures += faili("water composition err", err, DEDX_OK);
    } else if (comp_len == 0) {
        failures += failmsg("water composition", "empty composition");
    }

    err = 0;
    i_value = dedx_get_i_value(DEDX_WATER, &err);
    if (err != DEDX_OK) {
        failures += faili("water i-value err", err, DEDX_OK);
    } else if (i_value <= 0.0f) {
        failures += failmsg("water i-value", "non-positive I value");
    }

    err = 0;
    simple_stp = dedx_get_simple_stp(DEDX_PROTON, DEDX_WATER, 100.0f, &err);
    if (err != DEDX_OK) {
        failures += faili("simple stp err", err, DEDX_OK);
    } else if (simple_stp <= 0.0f) {
        failures += failmsg("simple stp", "non-positive stopping power");
    }

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    if (ws == NULL || cfg == NULL || err != DEDX_OK) {
        fprintf(stderr, "FAIL setup direct load: err=%d\n", err);
        dedx_free_config(cfg, &err);
        if (ws != NULL)
            dedx_free_workspace(ws, &err);
        return failures + 1;
    }

    cfg->program = DEDX_ICRU;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_WATER;
    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        failures += faili("direct load err", err, DEDX_OK);
    } else {
        direct_stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        if (err != DEDX_OK) {
            failures += faili("direct stp err", err, DEDX_OK);
        } else {
            failures += check_close("simple/direct stp", simple_stp, direct_stp, 1e-5);
        }
    }
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    coef_c = dedx_internal_calculate_mspaul_coef('c', DEDX_CARBON, DEDX_HYDROGEN, 10.0f);
    coef_d_low_z = dedx_internal_calculate_mspaul_coef('d', DEDX_CARBON, DEDX_HYDROGEN, 10.0f);
    failures += check_close("mspaul d->c low-z", coef_d_low_z, coef_c, 1e-6);

    coef_g = dedx_internal_calculate_mspaul_coef('g', DEDX_CARBON, DEDX_HYDROGEN, 10.0f);
    coef_h_low_z = dedx_internal_calculate_mspaul_coef('h', DEDX_CARBON, DEDX_HYDROGEN, 10.0f);
    failures += check_close("mspaul h->g low-z", coef_h_low_z, coef_g, 1e-6);

    coef_h = dedx_internal_calculate_mspaul_coef('h', DEDX_CARBON, DEDX_WATER, 10.0f);
    if (coef_h <= 0.0f)
        failures += failmsg("mspaul h water", "non-positive coefficient");

    if (dedx_internal_calculate_mspaul_coef('c', DEDX_HELIUM, DEDX_WATER, 10.0f) != 1.0f)
        failures += failmsg("mspaul helium", "helium fast-path is not unity");

    return failures;
}
