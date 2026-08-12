#include "dedx_lookup_data.h"
#include "test_helpers.h"

/*
 * Regression test raised in review of issue #149's Phase 2 PR: load_bethe_2()'s
 * memset(data, 0, sizeof(*data)) (added for A1, to stop the tail of the 150-float
 * data->data[] array from carrying uninitialized heap/stack past data->length) also
 * zeroes data->target and data->ion -- and neither was ever repopulated afterward, so
 * load_data() persisted 0/0 into ws->loaded_data[]->ion/target for every Bethe-formula
 * dataset instead of the actual ion/target that was loaded. Nothing in the public API
 * currently reads that stored metadata back out, so this had no observable effect on
 * computed stopping powers -- but it is exactly the kind of silently-wrong internal
 * state this audit is about, so it is now set explicitly, matching what
 * read_embedded_stopping_data() already does for the tabulated-report path. */
static int test_bethe_records_ion_and_target(void) {
    int failures = 0;
    int err = 0;
    dedx_workspace *ws = dedx_allocate_workspace(1, &err);
    dedx_config *cfg = calloc(1, sizeof(dedx_config));

    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_CARBON;
    cfg->rho = 2.0f;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    if (err != DEDX_OK) {
        fprintf(stderr, "FAIL bethe ion/target load: err=%d\n", err);
        failures++;
    } else {
        dedx_internal_lookup_data *loaded = ws->loaded_data[cfg->cfg_id];
        if (loaded->ion != DEDX_PROTON || loaded->target != DEDX_CARBON) {
            fprintf(stderr,
                    "FAIL bethe ion/target: got ion=%d target=%d, expected ion=%d target=%d\n",
                    loaded->ion,
                    loaded->target,
                    DEDX_PROTON,
                    DEDX_CARBON);
            failures++;
        }
    }
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

static dedx_config *make_bethe_default_config(int ion, int target) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = ion;
    cfg->target = target;

    switch (target) {
    case DEDX_WATER:
        cfg->elements_i_value = calloc(2, sizeof(float));
        cfg->elements_i_value[0] = 21.8f;
        cfg->elements_i_value[1] = 106.0f;
        cfg->rho = 1.000f;
        break;
    case DEDX_PMMA:
        cfg->elements_i_value = calloc(3, sizeof(float));
        cfg->elements_i_value[0] = 21.8f;
        cfg->elements_i_value[1] = 81.0f;
        cfg->elements_i_value[2] = 106.0f;
        cfg->elements_length = 3;
        cfg->rho = 1.190f;
        break;
    case DEDX_ALANINE:
        cfg->elements_i_value = calloc(4, sizeof(float));
        cfg->elements_i_value[0] = 21.8f;
        cfg->elements_i_value[1] = 81.0f;
        cfg->elements_i_value[2] = 82.0f;
        cfg->elements_i_value[3] = 106.0f;
        cfg->elements_length = 4;
        cfg->rho = 1.230f;
        break;
    }

    return cfg;
}

static dedx_config *make_bethe_new_i_config(int ion, int target) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = ion;
    cfg->target = target;

    switch (target) {
    case DEDX_WATER:
        cfg->elements_id = calloc(2, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(2, sizeof(int));
        cfg->elements_atoms[0] = 2;
        cfg->elements_atoms[1] = 1;
        cfg->elements_length = 2;
        cfg->elements_i_value = calloc(2, sizeof(float));
        cfg->elements_i_value[0] = 19.2f;
        cfg->elements_i_value[1] = 106.0f;
        cfg->rho = 1.000f;
        break;
    case DEDX_PMMA:
        cfg->elements_id = calloc(3, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_CARBON;
        cfg->elements_id[2] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(3, sizeof(int));
        cfg->elements_atoms[0] = 8;
        cfg->elements_atoms[1] = 5;
        cfg->elements_atoms[2] = 2;
        cfg->elements_i_value = calloc(3, sizeof(float));
        cfg->elements_i_value[0] = 19.2f;
        cfg->elements_i_value[1] = 81.0f;
        cfg->elements_i_value[2] = 106.0f;
        cfg->elements_length = 3;
        cfg->rho = 1.190f;
        break;
    case DEDX_ALANINE:
        cfg->elements_id = calloc(4, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_CARBON;
        cfg->elements_id[2] = DEDX_NITROGEN;
        cfg->elements_id[3] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(4, sizeof(int));
        cfg->elements_atoms[0] = 7;
        cfg->elements_atoms[1] = 3;
        cfg->elements_atoms[2] = 1;
        cfg->elements_atoms[3] = 2;
        cfg->elements_i_value = calloc(4, sizeof(float));
        cfg->elements_i_value[0] = 19.2f;
        cfg->elements_i_value[1] = 78.0f;
        cfg->elements_i_value[2] = 82.0f;
        cfg->elements_i_value[3] = 95.0f;
        cfg->elements_length = 4;
        cfg->rho = 1.230f;
        break;
    }

    return cfg;
}

static dedx_config *make_bethe_null_i_config(int ion, int target) {
    dedx_config *cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = ion;
    cfg->target = 0;

    switch (target) {
    case DEDX_WATER:
        cfg->elements_id = calloc(2, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(2, sizeof(int));
        cfg->elements_atoms[0] = 2;
        cfg->elements_atoms[1] = 1;
        cfg->elements_length = 2;
        cfg->rho = 1.000f;
        break;
    case DEDX_PMMA:
        cfg->elements_id = calloc(3, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_CARBON;
        cfg->elements_id[2] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(3, sizeof(int));
        cfg->elements_atoms[0] = 8;
        cfg->elements_atoms[1] = 5;
        cfg->elements_atoms[2] = 2;
        cfg->elements_length = 3;
        cfg->rho = 1.190f;
        break;
    case DEDX_ALANINE:
        cfg->elements_id = calloc(4, sizeof(int));
        cfg->elements_id[0] = DEDX_HYDROGEN;
        cfg->elements_id[1] = DEDX_CARBON;
        cfg->elements_id[2] = DEDX_NITROGEN;
        cfg->elements_id[3] = DEDX_OXYGEN;
        cfg->elements_atoms = calloc(4, sizeof(int));
        cfg->elements_atoms[0] = 7;
        cfg->elements_atoms[1] = 3;
        cfg->elements_atoms[2] = 1;
        cfg->elements_atoms[3] = 2;
        cfg->elements_length = 4;
        cfg->rho = 1.230f;
        break;
    }

    return cfg;
}

int main(void) {
    int failures = 0;
    const float energy_grid[] = {0.07f, 1.0f, 10.0f, 78.0f, 1000.0f};

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_WATER), energy_grid[0], 7.094e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_WATER), energy_grid[1], 2.672e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_WATER), energy_grid[2], 4.577e1f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_WATER), energy_grid[3], 8.791e0f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_WATER), energy_grid[4], 2.197e0f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_PMMA), energy_grid[0], 7.144e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_PMMA), energy_grid[1], 2.649e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_PMMA), energy_grid[2], 4.500e1f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_PMMA), energy_grid[3], 8.619e0f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_PROTON, DEDX_PMMA), energy_grid[4], 2.150e0f, "bethe-default");

    failures += check_config_stp(
        make_bethe_default_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[0], 7.028e2f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[1], 2.634e2f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[2], 4.484e1f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[3], 8.592e0f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[4], 2.145e0f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_WATER), energy_grid[0], 1.987e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_WATER), energy_grid[1], 1.048e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_WATER), energy_grid[2], 1.831e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_WATER), energy_grid[3], 3.517e1f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_WATER), energy_grid[4], 8.787e0f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[0], 1.964e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[1], 1.039e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[2], 1.800e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[3], 3.447e1f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[4], 8.599e0f, "bethe-default");

    failures += check_config_stp(
        make_bethe_default_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[0], 1.935e3f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[1], 1.033e3f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[2], 1.793e2f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[3], 3.437e1f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[4], 8.580e0f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_WATER), energy_grid[0], 5.109e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_WATER), energy_grid[1], 7.380e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_WATER), energy_grid[2], 1.635e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_WATER), energy_grid[3], 3.165e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_WATER), energy_grid[4], 7.908e1f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_PMMA), energy_grid[0], 5.010e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_PMMA), energy_grid[1], 7.317e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_PMMA), energy_grid[2], 1.608e3f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_PMMA), energy_grid[3], 3.103e2f, "bethe-default");
    failures +=
        check_config_stp(make_bethe_default_config(DEDX_CARBON, DEDX_PMMA), energy_grid[4], 7.739e1f, "bethe-default");

    failures += check_config_stp(
        make_bethe_default_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[0], 4.944e3f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[1], 7.269e3f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[2], 1.601e3f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[3], 3.093e2f, "bethe-default");
    failures += check_config_stp(
        make_bethe_default_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[4], 7.722e1f, "bethe-default");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[0], 7.336e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[1], 2.692e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[2], 4.597e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[3], 8.821e0f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[4], 2.202e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[0], 7.318e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[1], 2.663e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[2], 4.515e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[3], 8.640e0f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[4], 2.153e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[0], 7.523e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[1], 2.688e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[2], 4.538e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[3], 8.670e0f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[4], 2.162e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[0], 2.053e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[1], 1.056e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[2], 1.839e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[3], 3.529e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[4], 8.808e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[0], 2.012e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[1], 1.045e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[2], 1.806e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[3], 3.456e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[4], 8.614e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[0], 2.055e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[1], 1.055e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[2], 1.815e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[3], 3.468e1f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[4], 8.648e0f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[0], 5.226e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[1], 7.439e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[2], 1.642e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[3], 3.175e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[4], 7.927e1f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[0], 5.097e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[1], 7.359e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[2], 1.613e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[3], 3.110e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[4], 7.752e1f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[0], 5.143e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[1], 7.418e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[2], 1.621e3f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[3], 3.121e2f, "bethe-new-i");
    failures +=
        check_config_stp(make_bethe_new_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[4], 7.783e1f, "bethe-new-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[0], 7.096e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[1], 2.673e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[2], 4.578e+01f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[3], 8.810e+00f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_WATER), energy_grid[4], 2.201e+00f, "bethe-null-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[0], 7.146e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[1], 2.650e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[2], 4.482e+01f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[3], 8.576e+00f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_PROTON, DEDX_PMMA), energy_grid[4], 2.146e+00f, "bethe-null-i");

    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[0], 7.030e+02f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[1], 2.635e+02f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[2], 4.500e+01f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[3], 8.598e+00f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_PROTON, DEDX_ALANINE), energy_grid[4], 2.148e+00f, "bethe-null-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[0], 1.988e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[1], 1.049e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[2], 1.831e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[3], 3.524e+01f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_WATER), energy_grid[4], 8.803e+00f, "bethe-null-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[0], 1.965e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[1], 1.039e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[2], 1.793e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[3], 3.430e+01f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_HELIUM, DEDX_PMMA), energy_grid[4], 8.582e+00f, "bethe-null-i");

    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[0], 1.936e+03f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[1], 1.034e+03f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[2], 1.800e+02f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[3], 3.439e+01f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_HELIUM, DEDX_ALANINE), energy_grid[4], 8.593e+00f, "bethe-null-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[0], 5.110e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[1], 7.382e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[2], 1.635e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[3], 3.172e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_WATER), energy_grid[4], 7.923e+01f, "bethe-null-i");

    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[0], 5.012e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[1], 7.319e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[2], 1.601e+03f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[3], 3.087e+02f, "bethe-null-i");
    failures +=
        check_config_stp(make_bethe_null_i_config(DEDX_CARBON, DEDX_PMMA), energy_grid[4], 7.724e+01f, "bethe-null-i");

    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[0], 4.945e+03f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[1], 7.271e+03f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[2], 1.607e+03f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[3], 3.095e+02f, "bethe-null-i");
    failures += check_config_stp(
        make_bethe_null_i_config(DEDX_CARBON, DEDX_ALANINE), energy_grid[4], 7.733e+01f, "bethe-null-i");

    failures += test_bethe_records_ion_and_target();

    return failures;
}
