#ifndef DEDX_H
#define DEDX_H

#include "dedx_elements.h"
#include "dedx_error.h"

/**
 * @file dedx.h
 * @brief Public API for libdedx — a stopping power (dE/dx) library.
 *
 * Stopping power is the energy loss of a charged particle per unit path length
 * through a material, expressed in MeV cm² / g (mass stopping power).
 *
 * Typical usage:
 * @code
 *   int err = 0;
 *   dedx_workspace *ws = dedx_allocate_workspace(1, &err);
 *   dedx_config *cfg   = calloc(1, sizeof(dedx_config));
 *   cfg->program = DEDX_PSTAR;
 *   cfg->ion     = DEDX_PROTON;
 *   cfg->target  = DEDX_WATER;
 *   dedx_load_config(ws, cfg, &err);
 *   float stp = dedx_get_stp(ws, cfg, 100.0f, &err);  // MeV cm²/g at 100 MeV/nucl
 *   dedx_free_config(cfg, &err);
 *   dedx_free_workspace(ws, &err);
 * @endcode
 */

/**
 * @defgroup programs Stopping power programs
 * @brief Identifiers for the supported stopping power databases/models.
 * @{
 */
enum {
    DEDX_ASTAR = 1,     /**< NIST ASTAR — alpha particle stopping powers */
    DEDX_PSTAR,         /**< NIST PSTAR — proton stopping powers */
    DEDX_ESTAR,         /**< NIST ESTAR — electron stopping powers (not fully supported) */
    DEDX_MSTAR,         /**< MSTAR — stopping powers for heavier ions */
    DEDX_ICRU73_OLD,    /**< ICRU Report 73 (2005), older parametrization */
    DEDX_ICRU73,        /**< ICRU Report 73 (2005) */
    DEDX_ICRU49,        /**< ICRU Report 49 (1993) — protons and alpha particles */
    _DEDX_0008,         /**< Reserved */
    DEDX_ICRU,          /**< Auto-selects ICRU49 or ICRU73 based on ion type */
    DEDX_DEFAULT = 100, /**< Default program (Bethe formula) */
    DEDX_BETHE_EXT00    /**< Bethe formula with extensions */
};

/** @} */

/**
 * @defgroup states Material aggregate states
 * @{
 */
enum {
    DEDX_DEFAULT_STATE = 0, /**< Default state (normal state of aggregation) */
    DEDX_GAS,               /**< Force gas state */
    DEDX_CONDENSED          /**< Force condensed state */
};

/** @} */

/**
 * @defgroup mstar_modes MSTAR calculation modes
 * @brief Modes for the MSTAR stopping power program.
 *        Mode B is recommended by H. Paul.
 * @{
 */
enum {
    DEDX_MSTAR_MODE_A = 'a',      /**< Automatic state selection; use condensed/gas base mode (`c` or `g`). */
    DEDX_MSTAR_MODE_B = 'b',      /**< Automatic state selection; prefer special state mode (`d` or `h`). */
    DEDX_MSTAR_MODE_G = 'g',      /**< Gaseous target mode. */
    DEDX_MSTAR_MODE_H = 'h',      /**< Special gaseous-target mode for supported ions. */
    DEDX_MSTAR_MODE_C = 'c',      /**< Condensed target mode. */
    DEDX_MSTAR_MODE_D = 'd',      /**< Special condensed-target mode. */
    DEDX_MSTAR_MODE_DEFAULT = 'b' /**< Recommended by Helmut Paul */
};

/** @} */

/**
 * @defgroup interpolation_modes Stopping-power interpolation modes
 * @brief Modes for interpolating tabulated stopping-power data.
 * @{
 */
enum {
    DEDX_INTERPOLATION_LOG_LOG = 0,                         /**< Natural cubic spline of ln(stopping) vs ln(energy). */
    DEDX_INTERPOLATION_LINEAR = 1,                          /**< Natural cubic spline of stopping vs energy. */
    DEDX_INTERPOLATION_DEFAULT = DEDX_INTERPOLATION_LOG_LOG /**< Default for zero-initialized configs. */
};

/** @} */

/** @brief Translate a numeric error code to a human-readable string.
 *  @param[out] err_str  Buffer to receive the error description (caller-allocated).
 *  @param[in]  err      Error code returned by a libdedx function.
 */
void dedx_get_error_code(char *err_str, int err);

/** @brief Return the display name of a stopping power program.
 *  @param[in] program  Program identifier (e.g. DEDX_PSTAR).
 *  @return Pointer to a static string; do not free.
 */
const char *dedx_get_program_name(int program);

/** @brief Return the version string of a stopping power program.
 *  @param[in] program  Program identifier.
 *  @return Pointer to a static string; do not free.
 */
const char *dedx_get_program_version(int program);

/** @brief Return the display name of a target material.
 *  @param[in] material  Material identifier (e.g. DEDX_WATER).
 *  @return Pointer to a static string; do not free.
 */
const char *dedx_get_material_name(int material);

/** @brief Return the display name of an ion.
 *  @param[in] ion  Ion identifier (e.g. DEDX_PROTON).
 *  @return Pointer to a static string; do not free.
 */
const char *dedx_get_ion_name(int ion);

/** @brief Retrieve the library version.
 *  @param[out] major  Major version number.
 *  @param[out] minor  Minor version number.
 *  @param[out] patch  Patch version number.
 */
void dedx_get_version(int *major, int *minor, int *patch);

/** @brief Retrieve the full library version string.
 *  @return Pointer to a static string; do not free.
 */
const char *dedx_get_version_string(void);

/** @brief Retrieve the elemental composition of a compound target.
 *  @param[in]  target       Material identifier.
 *  @param[out] composition  2D array [element][0=Z, 1=mass_fraction].
 *  @param[out] comp_len     Number of elements in the composition.
 *  @param[out] err          Error code; 0 on success.
 */
void dedx_get_composition(int target, float composition[][2], unsigned int *comp_len, int *err);

/** @brief Retrieve the mean excitation potential (I-value) of a material.
 *  @param[in]  target  Material identifier.
 *  @param[out] err     Error code; 0 on success.
 *  @return I-value in eV.
 */
float dedx_get_i_value(int target, int *err);

/** @brief Return a null-terminated list of supported program identifiers.
 *  @return Pointer to a static array terminated by 0; do not free.
 */
const int *dedx_get_program_list(void);

/** @brief Return a null-terminated list of materials supported by a program.
 *  @param[in] program  Program identifier.
 *  @return Pointer to a static array terminated by 0; do not free.
 */
const int *dedx_get_material_list(int program);

/** @brief Return a null-terminated list of ions supported by a program.
 *  @param[in] program  Program identifier.
 *  @return Pointer to a static array terminated by 0; do not free.
 */
const int *dedx_get_ion_list(int program);

/** @brief Return the minimum valid energy for a program/ion combination.
 *  @param[in] program  Program identifier.
 *  @param[in] ion      Ion identifier.
 *  @return Minimum kinetic energy in MeV/nucl (MeV per nucleon).
 */
float dedx_get_min_energy(int program, int ion);

/** @brief Return the maximum valid energy for a program/ion combination.
 *  @param[in] program  Program identifier.
 *  @param[in] ion      Ion identifier.
 *  @return Maximum kinetic energy in MeV/nucl (MeV per nucleon).
 */
float dedx_get_max_energy(int program, int ion);

typedef struct dedx_internal_lookup_data dedx_internal_lookup_data;

/**
 * @brief Workspace holding preloaded stopping power datasets.
 *
 * Allocate with dedx_allocate_workspace() and free with dedx_free_workspace().
 * A workspace can hold multiple loaded configurations simultaneously.
 */
typedef struct {
    dedx_internal_lookup_data **loaded_data; /**< Array of loaded datasets */
    int datasets;                            /**< Total number of dataset slots */
    int active_datasets;                     /**< Number of currently loaded datasets */
} dedx_workspace;

/** @brief Allocate a workspace for loading stopping power data.
 *  @param[in]  count  Maximum number of configurations to hold simultaneously.
 *  @param[out] err    Error code; 0 on success.
 *  @return Pointer to allocated workspace, or NULL on failure.
 */
dedx_workspace *dedx_allocate_workspace(unsigned int count, int *err);

/** @brief Free a workspace and all loaded data.
 *  @param[in]  workspace  Workspace to free.
 *  @param[out] err        Error code; 0 on success.
 */
void dedx_free_workspace(dedx_workspace *workspace, int *err);

/**
 * @brief Configuration for a stopping power calculation.
 *
 * Populate the required fields and pass to dedx_load_config().
 * After loading, the struct reflects the resolved configuration
 * (applied I-values, elemental composition, state, etc.).
 *
 * Required fields before calling dedx_load_config():
 * - @c program
 * - @c ion
 * - @c target (or @c elements_id + @c elements_length for custom compounds)
 *
 * Optional fields:
 * - @c compound_state — defaults to normal state of aggregation
 * - @c mstar_mode     — defaults to DEDX_MSTAR_MODE_DEFAULT
 * - @c interpolation_mode — defaults to DEDX_INTERPOLATION_LOG_LOG
 * - @c i_value        — defaults to tabulated ICRU values
 * - @c rho            — required for Bethe-type programs (DEDX_BETHE_EXT00)
 */
typedef struct {
    int cfg_id;         /**< Configuration ID assigned by dedx_load_config(); do not set. */
    int program;        /**< Stopping power program (e.g. DEDX_PSTAR). Required. */
    int target;         /**< Target material identifier, or 0 for custom compound. */
    int ion;            /**< Projectile ion identifier (e.g. DEDX_PROTON). Required. */
    int ion_a;          /**< Nucleon number of projectile (set automatically). */
    int bragg_used;     /**< 1 if Bragg additivity rule was applied; set by library. */
    int compound_state; /**< Aggregate state: DEDX_DEFAULT, DEDX_GAS, or DEDX_CONDENSED. */

    unsigned int elements_length;  /**< Number of elements in custom compound. */
    unsigned int loaded;           /**< 1 if configuration has been loaded; set by library. */
    int *elements_id;              /**< Atomic numbers (Z) of each element in compound. */
    int *elements_atoms;           /**< Number of atoms per formula unit for each element. */
    char mstar_mode;               /**< MSTAR mode: DEDX_MSTAR_MODE_DEFAULT or A–H. */
    int interpolation_mode;        /**< Interpolation mode: DEDX_INTERPOLATION_LOG_LOG or DEDX_INTERPOLATION_LINEAR. */
    float i_value;                 /**< Mean excitation potential of target in eV. */
    float _temp_i_value;           /**< Internal: temporary I-value for Bethe calculation. */
    float rho;                     /**< Target density in g/cm³ (required for DEDX_BETHE_EXT00). */
    float *elements_mass_fraction; /**< Mass fractions of each element (alternative to atoms). */
    float *elements_i_value;       /**< I-values of individual elements in eV. */
    const char *target_name;       /**< Resolved target name; set by library. */
    const char *ion_name;          /**< Resolved ion name; set by library. */
    const char *program_name;      /**< Resolved program name; set by library. */
} dedx_config;

/** @brief Load a stopping power configuration into the workspace.
 *
 *  Must be called before dedx_get_stp(). After this call, dedx_config
 *  reflects the resolved configuration but changes to the struct have no effect.
 *
 *  @param[in]     ws      Workspace to load into.
 *  @param[in,out] config  Configuration to load; updated with resolved values.
 *  @param[out]    err     Error code; 0 on success.
 *  @return 0 on success, -1 on failure.
 */
int dedx_load_config(dedx_workspace *ws, dedx_config *config, int *err);

/** @brief Evaluate the stopping power at a given energy.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration (cfg_id must be valid).
 *  @param[in]  energy  Kinetic energy in MeV/nucl (MeV per nucleon).
 *  @param[out] err     Error code; 0 on success.
 *  @return Mass stopping power in MeV cm²/g.
 */
float dedx_get_stp(dedx_workspace *ws, dedx_config *config, float energy, int *err);

/** @brief One-call stopping power evaluation using the default program for the ion.
 *
 *  Convenience wrapper: allocates a workspace internally, loads the appropriate
 *  program, evaluates, and frees. Not suitable for repeated calls.
 *
 *  @param[in]  ion     Ion identifier (e.g. DEDX_PROTON).
 *  @param[in]  target  Target material identifier (e.g. DEDX_WATER).
 *  @param[in]  energy  Kinetic energy in MeV/nucl (MeV per nucleon).
 *  @param[out] err     Error code; 0 on success.
 *  @return Mass stopping power in MeV cm²/g.
 */
float dedx_get_simple_stp(int ion, int target, float energy, int *err);

/** @brief Free a dedx_config and any memory allocated inside it.
 *
 *  Always use this instead of plain free() for configs that have been
 *  passed to dedx_load_config(), as the library may allocate internal arrays.
 *
 *  @param[in]  config  Configuration to free.
 *  @param[out] err     Error code; 0 on success.
 */
void dedx_free_config(dedx_config *config, int *err);

#endif // DEDX_H
