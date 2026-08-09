#include <dedx_wrappers.h>

#include "test_helpers.h"

/* Regression coverage for issue #51: some ion+material+program combinations were
 * advertised as available (e.g. HYDROGEN + PSTAR + BORON) when the embedded data
 * doesn't actually cover them, and there was no per-ion way to ask which materials
 * a program really supports. These tests lock in:
 *   - the corrected dedx_get_material_list() tables no longer claim elements the
 *     embedded data doesn't have,
 *   - the new dedx_get_material_list_for_ion() / dedx_fill_material_list_for_ion()
 *     give an accurate, ion-specific, decomposition-aware answer,
 *   - DEDX_AUTO falls back to the Bethe-Bloch formula for elements/compounds no
 *     tabulated report covers, while DEDX_ICRU and the other report-specific
 *     programs (PSTAR, ASTAR, ICRU49, ICRU73, ...) still fail those combinations,
 *     as they should -- DEDX_ICRU and DEDX_AUTO are deliberately different models. */

static int contains(const int *list, int value) {
    int i;
    for (i = 0; list[i] != -1; i++) {
        if (list[i] == value)
            return 1;
    }
    return 0;
}

static int check_absent(const int *list, int value, const char *label) {
    if (contains(list, value)) {
        fprintf(stderr, "FAIL %s: value %d unexpectedly present\n", label, value);
        return 1;
    }
    return 0;
}

static int check_present(const int *list, int value, const char *label) {
    if (!contains(list, value)) {
        fprintf(stderr, "FAIL %s: value %d unexpectedly absent\n", label, value);
        return 1;
    }
    return 0;
}

static int check_err(int got, int expected, const char *label) {
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got err=%d, expected %d\n", label, got, expected);
        return 1;
    }
    return 0;
}

/* dedx_get_material_list() rows must no longer claim elements the embedded data
 * doesn't back (issue #51's literal report: PSTAR + Boron). */
static int test_corrected_static_tables(void) {
    int failures = 0;

    failures += check_absent(dedx_get_material_list(DEDX_PSTAR), DEDX_BORON, "PSTAR material list");
    failures += check_absent(dedx_get_material_list(DEDX_ASTAR), DEDX_BORON, "ASTAR material list");
    failures += check_absent(dedx_get_material_list(DEDX_ICRU49), DEDX_BORON, "ICRU49 material list");
    failures += check_absent(dedx_get_material_list(DEDX_ICRU73), DEDX_BORON, "ICRU73 material list");
    failures += check_absent(dedx_get_material_list(DEDX_ICRU73_OLD), DEDX_BORON, "ICRU73_OLD material list");
    failures += check_absent(dedx_get_material_list(DEDX_ICRU), DEDX_BORON, "ICRU material list");

    /* MSTAR genuinely tabulates Boron -- must not be over-corrected. */
    failures += check_present(dedx_get_material_list(DEDX_MSTAR), DEDX_BORON, "MSTAR material list");
    /* Nickel is genuinely tabulated by ICRU73/ICRU73_OLD -- must not be over-corrected. */
    failures += check_present(dedx_get_material_list(DEDX_ICRU73), DEDX_NICKEL, "ICRU73 material list");
    failures += check_present(dedx_get_material_list(DEDX_ICRU73_OLD), DEDX_NICKEL, "ICRU73_OLD material list");

    /* DEDX_AUTO can reach any element via its Bethe-Bloch tier, same as DEDX_DEFAULT
     * and DEDX_BETHE_EXT00. */
    failures += check_present(dedx_get_material_list(DEDX_AUTO), DEDX_BORON, "AUTO material list");

    return failures;
}

/* dedx_load_config() must keep rejecting Boron for report-specific programs,
 * including DEDX_ICRU itself: it is a real, tabulated auto-selecting report, not a
 * Bethe-Bloch hybrid, so a combination no ICRU sub-report covers still fails. */
static int test_report_specific_programs_reject_boron(void) {
    int failures = 0;
    int err;
    dedx_workspace *ws;
    dedx_config *cfg;

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_COMBINATION_NOT_FOUND, "PSTAR+proton+Boron");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_ICRU73;
    cfg->ion = DEDX_LITHIUM;
    cfg->target = DEDX_BORON;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_COMBINATION_NOT_FOUND, "ICRU73+lithium+Boron");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_ICRU;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_COMBINATION_NOT_FOUND, "ICRU+proton+Boron (ICRU must not fall back)");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

/* ...while DEDX_AUTO falls back to the Bethe-Bloch formula for the same element
 * directly, and attempts Bragg-additivity decomposition for a Boron compound (see
 * below for why that particular compound is expected to fail post-#149-A1). */
static int test_auto_bethe_fallback(void) {
    int failures = 0;
    int err;
    dedx_workspace *ws;
    dedx_config *cfg;
    float stp;

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_AUTO;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_OK, "AUTO+proton+Boron load");
    if (err == DEDX_OK) {
        stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        if (err != DEDX_OK || stp <= 0.0f) {
            fprintf(stderr, "FAIL AUTO+proton+Boron stp: err=%d stp=%f\n", err, (double) stp);
            failures++;
        }
    }
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* Boron carbide (B4C) is Bragg-decomposed into boron (not tabulated by any
     * report -- Bethe fallback, 122-point grid) and carbon (tabulated -- PSTAR-family
     * 133-point grid). This is the exact scenario issue #149's finding A1c used as its
     * worked example of "silent range truncation": before that fix, load_compound()
     * summed the two constituents' stopping powers over compound_data[0].length
     * (whichever constituent happened to be first) regardless of whether the other
     * constituent's own grid matched, silently truncating or mixing energy bins.
     * Post-A1, mismatched per-constituent grids are detected and rejected with
     * DEDX_ERR_INCONSISTENT_COMPOUND instead of being served as a wrong or
     * silently-truncated number -- so this load is now expected to fail cleanly. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_AUTO;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON_CARBIDE;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_INCONSISTENT_COMPOUND, "AUTO+proton+BoronCarbide load (mismatched grids)");
    failures += check_err(cfg->bragg_used, 1, "AUTO+proton+BoronCarbide should still attempt Bragg additivity");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* DEDX_ICRU still can't serve the same compound: it does not get the Bethe
     * fallback, unlike DEDX_AUTO. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_ICRU;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON_CARBIDE;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_COMBINATION_NOT_FOUND, "ICRU+proton+BoronCarbide");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    /* PSTAR still can't serve the same compound either: Boron is not in its table
     * and PSTAR does not get the Bethe fallback. */
    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_BORON_CARBIDE;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_ERR_COMBINATION_NOT_FOUND, "PSTAR+proton+BoronCarbide");
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

/*
 * Regression test for issue #149 finding A2: material id 99 (A150 tissue-equivalent
 * plastic) is a compound, one past the last real element id (DEDX_MAX_ELEMENT_ID ==
 * 98), but several `<= 99` / `> 99` off-by-one boundary checks used to treat it as an
 * element instead -- evaluating the Bethe-Bloch formula with the atomic mass/charge of
 * elemental einsteinium (Z=99) rather than decomposing A150's real composition. The
 * issue's own suggested regression test: `bragg_used == 1` and Bethe ~= tabulated
 * within a few % for id 99. Before the fix this was PSTAR=7.3376, BETHE=5.1799 (~29%
 * off, Es-252's Z/A, not A150's); after the fix BETHE tracks PSTAR to well under 1%. */
static int test_a150_boundary(void) {
    int failures = 0;
    int err;
    dedx_workspace *ws;
    dedx_config *cfg;
    float pstar_stp, bethe_stp;

    pstar_stp =
        dedx_get_simple_stp_for_program(DEDX_PSTAR, DEDX_PROTON, DEDX_A150_TISSUE_EQUIVALENT_PLASTIC, 100.0f, &err);
    failures += check_err(err, DEDX_OK, "PSTAR+proton+A150 stp");

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_BETHE_EXT00;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_A150_TISSUE_EQUIVALENT_PLASTIC;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_OK, "BETHE_EXT00+proton+A150 load");
    failures +=
        check_err(cfg->bragg_used, 1, "BETHE_EXT00+proton+A150 should Bragg-decompose, not treat id 99 as Z=99");
    if (err == DEDX_OK) {
        bethe_stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        failures += check_err(err, DEDX_OK, "BETHE_EXT00+proton+A150 stp");
        if (err == DEDX_OK) {
            float rel_diff = fabsf(bethe_stp - pstar_stp) / pstar_stp;
            if (rel_diff > 0.02f) {
                fprintf(stderr,
                        "FAIL A150 boundary: BETHE=%.4f vs PSTAR=%.4f differ by %.2f%% (expected < 2%%)\n",
                        (double) bethe_stp,
                        (double) pstar_stp,
                        (double) (rel_diff * 100.0f));
                failures++;
            }
        }
    }
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

/*
 * Regression test for issue #149 finding A5: dedx_internal_validate_rho() used to
 * hard-fail whenever a target's embedded density row was missing, for *every*
 * program -- even tabulated report lookups that never read config->rho at all. Only
 * one material was affected (FERROUSOXIDE, id 159), but it broke the material for all
 * 407 program/ion pairs that advertised it. The fix adds the missing density row and
 * only requires it for programs that actually evaluate the Bethe-Bloch formula
 * directly on the target (program >= DEDX_DEFAULT); tabulated programs like PSTAR
 * must now load FERROUSOXIDE without the caller ever supplying rho themselves. */
static int test_ferrous_oxide_tabulated_program(void) {
    int failures = 0;
    int err;
    dedx_workspace *ws;
    dedx_config *cfg;
    float stp;

    ws = dedx_allocate_workspace(1, &err);
    cfg = calloc(1, sizeof(dedx_config));
    cfg->program = DEDX_PSTAR;
    cfg->ion = DEDX_PROTON;
    cfg->target = DEDX_FERROUS_OXIDE;
    err = 0;
    dedx_load_config(ws, cfg, &err);
    failures += check_err(err, DEDX_OK, "PSTAR+proton+FerrousOxide load");
    if (err == DEDX_OK) {
        /* Density must have been resolved from the embedded table, not left at 0. */
        if (cfg->rho <= 0.0f) {
            fprintf(stderr, "FAIL PSTAR+proton+FerrousOxide: rho not populated (%f)\n", (double) cfg->rho);
            failures++;
        }
        stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        if (err != DEDX_OK || stp <= 0.0f) {
            fprintf(stderr, "FAIL PSTAR+proton+FerrousOxide stp: err=%d stp=%f\n", err, (double) stp);
            failures++;
        }
    }
    dedx_free_config(cfg, &err);
    dedx_free_workspace(ws, &err);

    return failures;
}

/*
 * Regression test for issue #149 finding A3: load_compound() used to seed each
 * constituent's I-value via the public dedx_get_i_value(), which hardcodes
 * DEDX_GAS -- so config->compound_state was silently ignored for every Bragg-
 * decomposed compound whose elements_i_value wasn't already supplied by the caller
 * (the common case for DEDX_AUTO, which never runs dedx_internal_evaluate_i_pot()
 * since that only happens for program >= DEDX_DEFAULT). Boron is not itself
 * tabulated by any report, so DEDX_AUTO resolves it through the Bethe-Bloch fallback
 * inside load_compound(), where the I-value -- and hence compound_state -- directly
 * affects the computed stopping power. A single-element "compound" isolates
 * load_compound()'s own I-value lookup from Bragg-averaging effects. Before the fix,
 * DEDX_GAS and DEDX_CONDENSED produced the identical (gas-phase) number; they must
 * now differ, since DEDX_CONDENSED applies the solid-state I correction. */
static int test_compound_state_affects_bethe_fallback(void) {
    int failures = 0;
    int err;
    float gas_stp, condensed_stp;

    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = calloc(1, sizeof(dedx_config));
        cfg->program = DEDX_AUTO;
        cfg->ion = DEDX_PROTON;
        cfg->target = 0;
        cfg->compound_state = DEDX_GAS;
        cfg->rho = 2.34f; /* boron's density; required for the Bethe fallback tier */
        cfg->elements_length = 1;
        cfg->elements_id = calloc(1, sizeof(int));
        cfg->elements_id[0] = DEDX_BORON;
        cfg->elements_atoms = calloc(1, sizeof(int));
        cfg->elements_atoms[0] = 1;
        err = 0;
        dedx_load_config(ws, cfg, &err);
        failures += check_err(err, DEDX_OK, "AUTO+proton+gas-boron load");
        gas_stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        failures += check_err(err, DEDX_OK, "AUTO+proton+gas-boron stp");
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }
    {
        dedx_workspace *ws = dedx_allocate_workspace(1, &err);
        dedx_config *cfg = calloc(1, sizeof(dedx_config));
        cfg->program = DEDX_AUTO;
        cfg->ion = DEDX_PROTON;
        cfg->target = 0;
        cfg->compound_state = DEDX_CONDENSED;
        cfg->rho = 2.34f;
        cfg->elements_length = 1;
        cfg->elements_id = calloc(1, sizeof(int));
        cfg->elements_id[0] = DEDX_BORON;
        cfg->elements_atoms = calloc(1, sizeof(int));
        cfg->elements_atoms[0] = 1;
        err = 0;
        dedx_load_config(ws, cfg, &err);
        failures += check_err(err, DEDX_OK, "AUTO+proton+condensed-boron load");
        condensed_stp = dedx_get_stp(ws, cfg, 100.0f, &err);
        failures += check_err(err, DEDX_OK, "AUTO+proton+condensed-boron stp");
        dedx_free_config(cfg, &err);
        dedx_free_workspace(ws, &err);
    }

    if (gas_stp > 0.0f && condensed_stp > 0.0f) {
        float rel_diff = fabsf(condensed_stp - gas_stp) / gas_stp;
        if (rel_diff < 1e-3f) {
            fprintf(stderr,
                    "FAIL A3: gas (%.6f) and condensed (%.6f) boron stp are identical -- "
                    "compound_state is not reaching load_compound()'s I-value lookup\n",
                    (double) gas_stp,
                    (double) condensed_stp);
            failures++;
        }
    }

    return failures;
}

static int test_material_list_for_ion(void) {
    int failures = 0;
    int materials[DEDX_MAX_MATERIAL_LIST + 1];
    unsigned int len;
    int err;

    dedx_get_material_list_for_ion(DEDX_PSTAR, DEDX_PROTON, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_OK, "PSTAR+proton for_ion err");
    materials[len] = -1;
    failures += check_absent(materials, DEDX_BORON, "PSTAR+proton for_ion");
    failures += check_present(materials, DEDX_WATER, "PSTAR+proton for_ion");

    /* MSTAR remaps every ion above helium onto its single reference table; helium
     * itself (ion == 2) already takes that remap path, since MSTAR has no ion == 1
     * entry. MSTAR genuinely tabulates Boron (unlike PSTAR/ASTAR/ICRU49/ICRU73). */
    dedx_get_material_list_for_ion(DEDX_MSTAR, DEDX_HELIUM, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_OK, "MSTAR+helium for_ion err");
    materials[len] = -1;
    failures += check_present(materials, DEDX_BORON, "MSTAR+helium for_ion");
    failures += check_present(materials, DEDX_WATER, "MSTAR+helium for_ion");

    /* ASTAR only supports helium; requesting it with a proton is an ion mismatch,
     * not a material-availability question. */
    dedx_get_material_list_for_ion(DEDX_ASTAR, DEDX_PROTON, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_ERR_ION_NOT_SUPPORTED, "ASTAR+proton for_ion");
    failures += check_err((int) len, 0, "ASTAR+proton for_ion count");

    /* DEDX_ICRU never falls back -- Boron and Boron Carbide stay absent. */
    dedx_get_material_list_for_ion(DEDX_ICRU, DEDX_PROTON, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_OK, "ICRU+proton for_ion err");
    materials[len] = -1;
    failures += check_absent(materials, DEDX_BORON, "ICRU+proton for_ion");
    failures += check_absent(materials, DEDX_BORON_CARBIDE, "ICRU+proton for_ion");

    /* DEDX_AUTO does fall back -- both appear. */
    dedx_get_material_list_for_ion(DEDX_AUTO, DEDX_PROTON, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_OK, "AUTO+proton for_ion err");
    materials[len] = -1;
    failures += check_present(materials, DEDX_BORON, "AUTO+proton for_ion");
    failures += check_present(materials, DEDX_BORON_CARBIDE, "AUTO+proton for_ion");

    dedx_get_material_list_for_ion(DEDX_ICRU73, DEDX_LITHIUM, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_OK, "ICRU73+lithium for_ion err");
    materials[len] = -1;
    failures += check_absent(materials, DEDX_BORON, "ICRU73+lithium for_ion");
    failures += check_absent(materials, DEDX_BORON_CARBIDE, "ICRU73+lithium for_ion");

    dedx_get_material_list_for_ion(DEDX_ESTAR, 1001, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_ERR_ESTAR_NOT_IMPL, "ESTAR+electron for_ion");
    failures += check_err((int) len, 0, "ESTAR+electron for_ion count");

    /* The periodic-table data backing the Bethe evaluation only covers ions up to 112
     * for DEDX_AUTO/DEFAULT/BETHE_EXT00 (see check_ion()); an ion beyond that must be
     * rejected, consistently with what dedx_load_config() would do. */
    dedx_get_material_list_for_ion(DEDX_AUTO, 115, materials, DEDX_MAX_MATERIAL_LIST, &len, &err);
    failures += check_err(err, DEDX_ERR_ION_NOT_SUPPORTED, "AUTO+ion115 for_ion");
    failures += check_err((int) len, 0, "AUTO+ion115 for_ion count");

    return failures;
}

/* The wrapper must agree with the core API and remain -1-terminated. */
static int test_fill_material_list_for_ion(void) {
    int failures = 0;
    int wrapped[DEDX_MAX_MATERIAL_LIST + 1];
    int direct[DEDX_MAX_MATERIAL_LIST];
    unsigned int direct_len;
    int err;
    int i;

    int wrap_err = dedx_fill_material_list_for_ion(DEDX_AUTO, DEDX_PROTON, wrapped);
    dedx_get_material_list_for_ion(DEDX_AUTO, DEDX_PROTON, direct, DEDX_MAX_MATERIAL_LIST, &direct_len, &err);

    failures += check_err(wrap_err, DEDX_OK, "fill_material_list_for_ion err");
    for (i = 0; i < (int) direct_len; i++) {
        if (wrapped[i] != direct[i]) {
            fprintf(stderr, "FAIL fill_material_list_for_ion: entry %d differs (%d vs %d)\n", i, wrapped[i], direct[i]);
            failures++;
            break;
        }
    }
    failures += check_err(wrapped[direct_len], -1, "fill_material_list_for_ion terminator");

    /* Invalid ion/program combination: wrapper reports the error and still
     * produces an empty, -1-terminated list. */
    wrap_err = dedx_fill_material_list_for_ion(DEDX_ASTAR, DEDX_PROTON, wrapped);
    failures += check_err(wrap_err, DEDX_ERR_ION_NOT_SUPPORTED, "fill_material_list_for_ion invalid combo err");
    failures += check_err(wrapped[0], -1, "fill_material_list_for_ion invalid combo terminator");

    return failures;
}

int main(void) {
    int failures = 0;

    failures += test_corrected_static_tables();
    failures += test_report_specific_programs_reject_boron();
    failures += test_auto_bethe_fallback();
    failures += test_a150_boundary();
    failures += test_ferrous_oxide_tabulated_program();
    failures += test_compound_state_affects_bethe_fallback();
    failures += test_material_list_for_ion();
    failures += test_fill_material_list_for_ion();

    if (failures == 0)
        printf("test_material_availability: all checks passed\n");
    return failures;
}
