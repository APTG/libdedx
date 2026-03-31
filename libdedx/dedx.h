#ifndef DEDX_H_INCLUDED
#define DEDX_H_INCLUDED

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
 *   float stp = dedx_get_stp(ws, cfg, 100.0f, &err);  // MeV cm²/g at 100 MeV/u
 *   dedx_free_config(cfg, &err);
 *   dedx_free_workspace(ws, &err);
 * @endcode
 */

#define _DEDX_MAXELEMENTS 150

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
    DEDX_MSTAR_MODE_A = 'a',
    DEDX_MSTAR_MODE_B = 'b',
    DEDX_MSTAR_MODE_G = 'g',
    DEDX_MSTAR_MODE_H = 'h',
    DEDX_MSTAR_MODE_C = 'c',
    DEDX_MSTAR_MODE_D = 'd',
    DEDX_MSTAR_MODE_DEFAULT = 'b' /**< Recommended by Helmut Paul */
};

/** @} */

/**
 * @defgroup ions_and_materials Ion and material identifiers
 * @brief Identifiers for projectile ions and target materials.
 *
 * Elemental ions (Z=1–98) correspond to their atomic number.
 * Compound materials follow sequentially after the elements.
 * Both share a single enum to preserve the numeric ID mapping.
 * @{
 */
enum {
    /* --- Elemental ions (Z = atomic number) --- */
    DEDX_HYDROGEN = 1,
    DEDX_HELIUM,
    DEDX_LITHIUM,
    DEDX_BERYLLIUM,
    DEDX_BORON,
    DEDX_CARBON,
    DEDX_GRAPHITE = 906,
    DEDX_NITROGEN = 7,
    DEDX_OXYGEN,
    DEDX_FLUORINE,
    DEDX_NEON,
    DEDX_SODIUM,
    DEDX_MAGNESIUM,
    DEDX_ALUMINUM,
    DEDX_SILICON,
    DEDX_PHOSPHORUS,
    DEDX_SULFUR,
    DEDX_CHLORINE,
    DEDX_ARGON,
    DEDX_POTASSIUM,
    DEDX_CALCIUM,
    DEDX_SCANDIUM,
    DEDX_TITeANIUM,
    DEDX_VANADIUM,
    DEDX_CHROMIUM,
    DEDX_MANGANESE,
    DEDX_IRON,
    DEDX_COBALT,
    DEDX_NICKEL,
    DEDX_COPPER,
    DEDX_ZINC,
    DEDX_GALLIUM,
    DEDX_GERMANIUM,
    DEDX_ARSENIC,
    DEDX_SELENIUM,
    DEDX_BROMINE,
    DEDX_KRYPTON,
    DEDX_RUBIDIUM,
    DEDX_STRONTIUM,
    DEDX_YTTRIUM,
    DEDX_ZIRCONIUM,
    DEDX_NIOBIUM,
    DEDX_MOLYBDENUM,
    DEDX_TECHNETIUM,
    DEDX_RUTHENIUM,
    DEDX_RHODIUM,
    DEDX_PALLADIUM,
    DEDX_SILVER,
    DEDX_CADMIUM,
    DEDX_INDIUM,
    DEDX_TIN,
    DEDX_ANTIMONY,
    DEDX_TELLURIUM,
    DEDX_IODINE,
    DEDX_XENON,
    DEDX_CESIUM,
    DEDX_BARIUM,
    DEDX_LANTHANUM,
    DEDX_CERIUM,
    DEDX_PRASEODYMIUM,
    DEDX_NEODYMIUM,
    DEDX_PROMETHIUM,
    DEDX_SAMARIUM,
    DEDX_EUROPIUM,
    DEDX_GADOLINIUM,
    DEDX_TERBIUM,
    DEDX_DYSPROSIUM,
    DEDX_HOLMIUM,
    DEDX_ERBIUM,
    DEDX_THULIUM,
    DEDX_YTTERBIUM,
    DEDX_LUTETIUM,
    DEDX_HAFNIUM,
    DEDX_TANTALUM,
    DEDX_TUNGSTEN,
    DEDX_RHENIUM,
    DEDX_OSMIUM,
    DEDX_IRIDIUM,
    DEDX_PLATINUM,
    DEDX_GOLD,
    DEDX_MERCURY,
    DEDX_THALLIUM,
    DEDX_LEAD,
    DEDX_BISMUTH,
    DEDX_POLONIUM,
    DEDX_ASTATINE,
    DEDX_RADON,
    DEDX_FRANCIUM,
    DEDX_RADIUM,
    DEDX_ACTINIUM,
    DEDX_THORIUM,
    DEDX_PROTACTINIUM,
    DEDX_URANIUM,
    DEDX_NEPTUNIUM,
    DEDX_PLUTONIUM,
    DEDX_AMERICIUM,
    DEDX_CURIUM,
    DEDX_BERKELIUM,
    DEDX_CALIFORNIUM,

    /* --- Compound and mixture targets (NIST material database) --- */
    DEDX_A150_TISSUE_EQUIVALENT_PLASTIC,
    DEDX_ACETONE,
    DEDX_ACETYLENE,
    DEDX_ADENINE,
    DEDX_ADIPOSE_TISSUE_ICRP,
    DEDX_AIR_DRY_NEAR_SEA_LEVEL,
    DEDX_ALANINE,
    DEDX_ALUMINUMOXIDE,
    DEDX_AMBER,
    DEDX_AMMONIA,
    DEDX_ANILINE,
    DEDX_ANTHRACENE,
    DEDX_B100,
    DEDX_BAKELITE,
    DEDX_BARIUM_FLUORIDE,
    DEDX_BARIUM_SULFATE,
    DEDX_BENZENE,
    DEDX_BERYLLIUM_OXIDE,
    DEDX_BISMUTH_GERMANIUM_OXIDE,
    DEDX_BLOOD_ICRP,
    DEDX_BONE_COMPACT_ICRU,
    DEDX_BONE_CORTICAL_ICRP,
    DEDX_BORON_CARBIDE,
    DEDX_BORON_OXIDE,
    DEDX_BRAIN_ICRP,
    DEDX_BUTANE,
    DEDX_N_BUTYLALCOHOL,
    DEDX_C552_AIR_EQUIVALENT_PLASTIC,
    DEDX_CADMIUM_TELLURIDE,
    DEDX_CADMIUM_TUNGSTATE,
    DEDX_CALCIUM_CARBONATE,
    DEDX_CALCIUM_FLUORIDE,
    DEDX_CALCIUM_OXIDE,
    DEDX_CALCIUM_SULFATE,
    DEDX_CALCIUM_TUNGSTATE,
    DEDX_CARBON_DIOXIDE,
    DEDX_CARBON_TETRACHLORIDE,
    DEDX_CELLULOSE_ACETATE_CELLOPHANE,
    DEDX_CELLULOSE_ACETATE_BUTYRATE,
    DEDX_CELLULOSE_NITRATE,
    DEDX_CERIC_SULFATE_DOSIMETER_SOLUTION,
    DEDX_CESIUM_FLUORIDE,
    DEDX_CESIUM_IODIDE,
    DEDX_CHLORO_BENZENE,
    DEDX_CHLOROFORM,
    DEDX_CONCRETE_PORTLAND,
    DEDX_CYCLOHEXANE,
    DEDX_DICHLOROBENZENE,
    DEDX_DICHLORODIETHYL_ETHER,
    DEDX_DICHLOROETHANE,
    DEDX_DIETHYLETHER,
    DEDX_N_N_DIMETHYL_FORMAMIDE,
    DEDX_DIMETHYL_SULFOXIDE,
    DEDX_ETHANE,
    DEDX_ETHYL_ALCOHOL,
    DEDX_ETHYL_CELLULOSE,
    DEDX_ETHYLENE,
    DEDX_EYE_LENS_ICRP,
    DEDX_FERRIC_OXIDE,
    DEDX_FERRO_BORIDE,
    DEDX_FERROUS_OXIDE,
    DEDX_FERROUS_SULFATE_DOSIMETER_SOLUTION,
    DEDX_FREON_12,
    DEDX_FREON_12B2,
    DEDX_FREON_13,
    DEDX_FREON_13B1,
    DEDX_FREON_13I1,
    DEDX_GADOLINIUM_OXYSULFIDE,
    DEDX_GALLIUM_ARSENIDE,
    DEDX_GEL_IN_PHOTOGRAPHIC_EMULSION,
    DEDX_GLASS_PYREX,
    DEDX_GLASS_LEAD,
    DEDX_GLASS_PLATE, /* 169,170,171 */
    DEDX_GLUCOSE,
    DEDX_GLUTAMINE,
    DEDX_GLYCEROL,
    DEDX_GUANINE,
    DEDX_GYPSUM_PLASTER_OF_PARIS,
    DEDX_N_HEPTANE,
    DEDX_N_HEXANE,
    DEDX_KAPTON_POLYIMIDE_FILM,
    DEDX_LANTHANUM_OXYBROMIDE,
    DEDX_LANTHANUM_OXYSULFIDE,
    DEDX_LEAD_OXIDE,
    DEDX_LITHIUM_AMIDE,
    DEDX_LITHIUM_CARBONATE,
    DEDX_LITHIUM_FLUORIDE,
    DEDX_LITHIUM_HYDRIDE,
    DEDX_LITHIUM_IODIDE,
    DEDX_LITHIUM_OXIDE,
    DEDX_LITHIUM_TETRABORATE,
    DEDX_LUNG_ICRP,
    DEDX_M3_WAX,
    DEDX_MAGNESIUM_CARBONATE,
    DEDX_MAGNESIUM_FLUORIDE,
    DEDX_MAGNESIUM_OXIDE,
    DEDX_MAGNESIUM_TETRABORATE,
    DEDX_MERCURIC_IODIDE,
    DEDX_METHANE,
    DEDX_METHANOL,
    DEDX_MIX_D_WAX,
    DEDX_MS20_TISSUE_SUBSTITUTE,
    DEDX_MUSCLE_SKELETAL,
    DEDX_MUSCLE_STRIATED,
    DEDX_MUSCLE_EQUIVALENT_LIQUID_WITH_SUCROSE,
    DEDX_MUSCLE_EQUIVALENT_LIQUID_WITHOUT_SUCROSE,
    DEDX_NAPHTHALENE,
    DEDX_NITROBENZENE,
    DEDX_NITROUS_OXIDE,
    DEDX_NYLON_DUPONT_ELVAMIDE_8062,
    DEDX_NYLON_TYPE_6_AND_6_6,
    DEDX_NYLON_TYPE_6_10,
    DEDX_NYLON_TYPE_11_RILSAN,
    DEDX_OCTANE_LIQUID,
    DEDX_PARAFFIN_WAX,
    DEDX_N_PENTANE,
    DEDX_PHOTOGRAPHIC_EMULSION,
    DEDX_PLASTIC_SCINTILLATOR_VINYLTOLUENE_BASED,
    DEDX_PLUTONIUM_DIOXIDE,
    DEDX_POLYACRYLONITRILE,
    DEDX_POLYCARBONATE_MAKROLON_LEXAN,
    DEDX_POLYCHLOROSTYRENE,
    DEDX_POLYETHYLENE,
    DEDX_MYLAR,
    DEDX_LUCITE_PERSPEX_PMMA,
    DEDX_POLYOXYMETHYLENE,
    DEDX_POLYPROPYLENE,
    DEDX_POLYSTYRENE,
    DEDX_POLYTETRAFLUOROETHYLENE,
    DEDX_POLYTRIFLUOROCHLOROETHYLENE,
    DEDX_POLYVINYL_ACETATE,
    DEDX_POLYVINYL_ALCOHOL,
    DEDX_POLYVINYL_BUTYRAL,
    DEDX_POLYVINYL_CHLORIDE,
    DEDX_POLYVINYLIDENE_CHLORIDE_SARAN,
    DEDX_POLYVINYLIDENE_FLUORIDE,
    DEDX_POLYVINYL_PYRROLIDONE,
    DEDX_POTASSIUM_IODIDE,
    DEDX_POTASSIUM_OXIDE,
    DEDX_PROPANE,
    DEDX_PROPANE_LIQUID,
    DEDX_N_PROPYL_ALCOHOL,
    DEDX_PYRIDINE,
    DEDX_RUBBER_BUTYL,
    DEDX_RUBBER_NATURAL,
    DEDX_RUBBER_NEOPRENE,
    DEDX_SILICON_DIOXIDE,
    DEDX_SILVER_BROMIDE,
    DEDX_SILVER_CHLORIDE,
    DEDX_SILVER_HALIDES_IN_PHOTOGRAPHIC_EMULSION,
    DEDX_SILVER_IODIDE,
    DEDX_SKIN_ICRP,
    DEDX_SODIUM_CARBONATE,
    DEDX_SODIUM_IODIDE,
    DEDX_SODIUM_MONOXIDE,
    DEDX_SODIUM_NITRATE,
    DEDX_STILBENE,
    DEDX_SUCROSE,
    DEDX_TERPHENYL,
    DEDX_TESTES_ICRP,
    DEDX_TETRACHLOROETHYLENE,
    DEDX_THALLIUM_CHLORIDE,
    DEDX_TISSUE_SOFT_ICRP,
    DEDX_TISSUE_SOFT_ICRU_FOUR_COMPONENT,
    DEDX_TISSUE_EQUIVALENT_GAS_METHANE_BASED,
    DEDX_TISSUE_EQUIVALENT_GAS_PROPANE_BASED,
    DEDX_TITANIUM_DIOXIDE,
    DEDX_TOLUENE,
    DEDX_TRICHLOROETHYLENE,
    DEDX_TRIETHYL_PHOSPHATE,
    DEDX_TUNGSTEN_HEXAFLUORIDE,
    DEDX_URANIUM_DICARBIDE,
    DEDX_URANIUM_MONOCARBIDE,
    DEDX_URANIUM_OXIDE,
    DEDX_UREA,
    DEDX_VALINE,
    DEDX_VITON_FLUOROELASTOMER,
    DEDX_WATER_LIQUID,
    DEDX_WATER_VAPOR,
    DEDX_XYLENE
};

/** @} */ /* ions_and_materials */

/** @defgroup special_ions Special particle identifiers
 *  @brief Identifiers for particles that do not map directly to atomic number.
 *  @{
 */
#define DEDX_PROTON 1        /**< Proton (Z=1, same as DEDX_HYDROGEN) */
#define DEDX_ELECTRON 1001   /**< Electron */
#define DEDX_POSITRON 1002   /**< Positron */
#define DEDX_PIMINUS 1003    /**< Pi minus */
#define DEDX_PIPLUS 1004     /**< Pi plus */
#define DEDX_PIZERO 1005     /**< Pi zero */
#define DEDX_ANTIPROTON 1006 /**< Antiproton */
/** @} */

/** @defgroup aliases Common material aliases
 *  @{
 */
#define DEDX_WATER DEDX_WATER_LIQUID
#define DEDX_AIR DEDX_AIR_DRY_NEAR_SEA_LEVEL
#define DEDX_PMMA DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_PERSPEX DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_LUCITE DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_TEFLON DEDX_POLYTETRAFLUOROETHYLENE
#define DEDX_CONCRETE DEDX_CONCRETE_PORTLAND
#define DEDX_CAESIUM DEDX_CESIUM
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
 *  @return Minimum energy in MeV/u.
 */
float dedx_get_min_energy(int program, int ion);

/** @brief Return the maximum valid energy for a program/ion combination.
 *  @param[in] program  Program identifier.
 *  @param[in] ion      Ion identifier.
 *  @return Maximum energy in MeV/u.
 */
float dedx_get_max_energy(int program, int ion);

/** @cond INTERNAL */
typedef struct {
    int cache;
    int hits;
    int miss;
} _dedx_lookup_accelerator;

typedef struct {
    float a;
    float b;
    float c;
    float d;
    float x;
} _dedx_spline_base;

typedef struct {
    _dedx_spline_base base[_DEDX_MAXELEMENTS];
    int n;
    int prog;
    int target;
    int ion;
    int datapoints;
    _dedx_lookup_accelerator acc;
} _dedx_lookup_data;

/** @endcond */

/**
 * @brief Workspace holding preloaded stopping power datasets.
 *
 * Allocate with dedx_allocate_workspace() and free with dedx_free_workspace().
 * A workspace can hold multiple loaded configurations simultaneously.
 */
typedef struct {
    _dedx_lookup_data **loaded_data; /**< Array of loaded datasets */
    int datasets;                    /**< Total number of dataset slots */
    int active_datasets;             /**< Number of currently loaded datasets */
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
 */
void dedx_load_config(dedx_workspace *ws, dedx_config *config, int *err);

/** @brief Evaluate the stopping power at a given energy.
 *
 *  @param[in]  ws      Workspace with a loaded configuration.
 *  @param[in]  config  Loaded configuration (cfg_id must be valid).
 *  @param[in]  energy  Kinetic energy in MeV/u.
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
 *  @param[in]  energy  Kinetic energy in MeV/u.
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

#endif // DEDX_H_INCLUDED
