#ifndef DEDX_H_INCLUDED
#define DEDX_H_INCLUDED

#define _DEDX_MAXELEMENTS 150

/* available data tables */
enum {DEDX_ASTAR=1, DEDX_PSTAR, DEDX_ESTAR,
      DEDX_MSTAR, DEDX_ICRU73_OLD, DEDX_ICRU73, DEDX_ICRU49, _DEDX_0008, 
      DEDX_ICRU, DEDX_DEFAULT=100, DEDX_BETHE_EXT00};

enum {DEDX_DEFAULT_STATE=0,DEDX_GAS,DEDX_CONDENSED};

enum {DEDX_MSTAR_MODE_A='a',DEDX_MSTAR_MODE_B='b',DEDX_MSTAR_MODE_G='g',
      DEDX_MSTAR_MODE_H='h',DEDX_MSTAR_MODE_C='c',DEDX_MSTAR_MODE_D='d',
      DEDX_MSTAR_MODE_DEFAULT='b' /* as recommended by Helmut Paul */
};

enum {DEDX_HYDROGEN=1, DEDX_HELIUM, DEDX_LITHIUM, DEDX_BERYLLIUM, DEDX_BORON,
      DEDX_CARBON, DEDX_GRAPHITE=906, DEDX_NITROGEN=7, DEDX_OXYGEN,
      DEDX_FLUORINE, DEDX_NEON, DEDX_SODIUM, DEDX_MAGNESIUM,
      DEDX_ALUMINUM, DEDX_SILICON, DEDX_PHOSPHORUS, DEDX_SULFUR,
      DEDX_CHLORINE, DEDX_ARGON, DEDX_POTASSIUM, DEDX_CALCIUM, DEDX_SCANDIUM,
      DEDX_TITeANIUM, DEDX_VANADIUM, DEDX_CHROMIUM, DEDX_MANGANESE, DEDX_IRON,
      DEDX_COBALT, DEDX_NICKEL, DEDX_COPPER, DEDX_ZINC, DEDX_GALLIUM,
      DEDX_GERMANIUM, DEDX_ARSENIC, DEDX_SELENIUM, DEDX_BROMINE, DEDX_KRYPTON,
      DEDX_RUBIDIUM, DEDX_STRONTIUM, DEDX_YTTRIUM, DEDX_ZIRCONIUM, DEDX_NIOBIUM,
      DEDX_MOLYBDENUM, DEDX_TECHNETIUM, DEDX_RUTHENIUM, DEDX_RHODIUM,
      DEDX_PALLADIUM, DEDX_SILVER, DEDX_CADMIUM, DEDX_INDIUM, DEDX_TIN,
      DEDX_ANTIMONY, DEDX_TELLURIUM, DEDX_IODINE, DEDX_XENON, DEDX_CESIUM,
      DEDX_BARIUM, DEDX_LANTHANUM, DEDX_CERIUM, DEDX_PRASEODYMIUM,
      DEDX_NEODYMIUM, DEDX_PROMETHIUM, DEDX_SAMARIUM, DEDX_EUROPIUM,
      DEDX_GADOLINIUM, DEDX_TERBIUM, DEDX_DYSPROSIUM, DEDX_HOLMIUM,
      DEDX_ERBIUM, DEDX_THULIUM, DEDX_YTTERBIUM, DEDX_LUTETIUM, DEDX_HAFNIUM,
      DEDX_TANTALUM, DEDX_TUNGSTEN, DEDX_RHENIUM, DEDX_OSMIUM, DEDX_IRIDIUM,
      DEDX_PLATINUM, DEDX_GOLD, DEDX_MERCURY, DEDX_THALLIUM, DEDX_LEAD,
      DEDX_BISMUTH, DEDX_POLONIUM, DEDX_ASTATINE, DEDX_RADON, DEDX_FRANCIUM,
      DEDX_RADIUM, DEDX_ACTINIUM, DEDX_THORIUM, DEDX_PROTACTINIUM,
      DEDX_URANIUM, DEDX_NEPTUNIUM, DEDX_PLUTONIUM, DEDX_AMERICIUM,
      DEDX_CURIUM, DEDX_BERKELIUM, DEDX_CALIFORNIUM,
      DEDX_A150_TISSUE_EQUIVALENT_PLASTIC, DEDX_ACETONE, DEDX_ACETYLENE,
      DEDX_ADENINE, DEDX_ADIPOSE_TISSUE_ICRP, DEDX_AIR_DRY_NEAR_SEA_LEVEL,
      DEDX_ALANINE, DEDX_ALUMINUMOXIDE, DEDX_AMBER, DEDX_AMMONIA, DEDX_ANILINE,
      DEDX_ANTHRACENE, DEDX_B100, DEDX_BAKELITE, DEDX_BARIUM_FLUORIDE,
      DEDX_BARIUM_SULFATE, DEDX_BENZENE, DEDX_BERYLLIUM_OXIDE,
      DEDX_BISMUTH_GERMANIUM_OXIDE, DEDX_BLOOD_ICRP, DEDX_BONE_COMPACT_ICRU,
      DEDX_BONE_CORTICAL_ICRP, DEDX_BORON_CARBIDE, DEDX_BORON_OXIDE,
      DEDX_BRAIN_ICRP, DEDX_BUTANE, DEDX_N_BUTYLALCOHOL,
      DEDX_C552_AIR_EQUIVALENT_PLASTIC, DEDX_CADMIUM_TELLURIDE,
      DEDX_CADMIUM_TUNGSTATE, DEDX_CALCIUM_CARBONATE, DEDX_CALCIUM_FLUORIDE,
      DEDX_CALCIUM_OXIDE, DEDX_CALCIUM_SULFATE, DEDX_CALCIUM_TUNGSTATE,
      DEDX_CARBON_DIOXIDE, DEDX_CARBON_TETRACHLORIDE,
      DEDX_CELLULOSE_ACETATE_CELLOPHANE, DEDX_CELLULOSE_ACETATE_BUTYRATE,
      DEDX_CELLULOSE_NITRATE, DEDX_CERIC_SULFATE_DOSIMETER_SOLUTION,
      DEDX_CESIUM_FLUORIDE, DEDX_CESIUM_IODIDE, DEDX_CHLORO_BENZENE,
      DEDX_CHLOROFORM, DEDX_CONCRETE_PORTLAND, DEDX_CYCLOHEXANE,
      DEDX_DICHLOROBENZENE, DEDX_DICHLORODIETHYL_ETHER, DEDX_DICHLOROETHANE,
      DEDX_DIETHYLETHER, DEDX_N_N_DIMETHYL_FORMAMIDE, DEDX_DIMETHYL_SULFOXIDE,
      DEDX_ETHANE, DEDX_ETHYL_ALCOHOL, DEDX_ETHYL_CELLULOSE, DEDX_ETHYLENE,
      DEDX_EYE_LENS_ICRP, DEDX_FERRIC_OXIDE, DEDX_FERRO_BORIDE,
      DEDX_FERROUS_OXIDE, DEDX_FERROUS_SULFATE_DOSIMETER_SOLUTION,
      DEDX_FREON_12, DEDX_FREON_12B2, DEDX_FREON_13, DEDX_FREON_13B1,
      DEDX_FREON_13I1, DEDX_GADOLINIUM_OXYSULFIDE, DEDX_GALLIUM_ARSENIDE,
      DEDX_GEL_IN_PHOTOGRAPHIC_EMULSION, 
      DEDX_GLASS_PYREX, DEDX_GLASS_LEAD, DEDX_GLASS_PLATE, /* 169,170,171 */
      DEDX_GLUCOSE, DEDX_GLUTAMINE, DEDX_GLYCEROL,
      DEDX_GUANINE, DEDX_GYPSUM_PLASTER_OF_PARIS, DEDX_N_HEPTANE, DEDX_N_HEXANE,
      DEDX_KAPTON_POLYIMIDE_FILM, DEDX_LANTHANUM_OXYBROMIDE,
      DEDX_LANTHANUM_OXYSULFIDE, DEDX_LEAD_OXIDE, DEDX_LITHIUM_AMIDE,
      DEDX_LITHIUM_CARBONATE, DEDX_LITHIUM_FLUORIDE, DEDX_LITHIUM_HYDRIDE,
      DEDX_LITHIUM_IODIDE, DEDX_LITHIUM_OXIDE, DEDX_LITHIUM_TETRABORATE,
      DEDX_LUNG_ICRP, DEDX_M3_WAX, DEDX_MAGNESIUM_CARBONATE,
      DEDX_MAGNESIUM_FLUORIDE, DEDX_MAGNESIUM_OXIDE, DEDX_MAGNESIUM_TETRABORATE,
      DEDX_MERCURIC_IODIDE, DEDX_METHANE, DEDX_METHANOL, DEDX_MIX_D_WAX,
      DEDX_MS20_TISSUE_SUBSTITUTE, DEDX_MUSCLE_SKELETAL, DEDX_MUSCLE_STRIATED,
      DEDX_MUSCLE_EQUIVALENT_LIQUID_WITH_SUCROSE,
      DEDX_MUSCLE_EQUIVALENT_LIQUID_WITHOUT_SUCROSE, DEDX_NAPHTHALENE,
      DEDX_NITROBENZENE, DEDX_NITROUS_OXIDE, DEDX_NYLON_DUPONT_ELVAMIDE_8062,
      DEDX_NYLON_TYPE_6_AND_6_6, DEDX_NYLON_TYPE_6_10,
      DEDX_NYLON_TYPE_11_RILSAN, DEDX_OCTANE_LIQUID, DEDX_PARAFFIN_WAX,
      DEDX_N_PENTANE, DEDX_PHOTOGRAPHIC_EMULSION,
      DEDX_PLASTIC_SCINTILLATOR_VINYLTOLUENE_BASED, DEDX_PLUTONIUM_DIOXIDE,
      DEDX_POLYACRYLONITRILE, DEDX_POLYCARBONATE_MAKROLON_LEXAN,
      DEDX_POLYCHLOROSTYRENE, DEDX_POLYETHYLENE, DEDX_MYLAR,
      DEDX_LUCITE_PERSPEX_PMMA, DEDX_POLYOXYMETHYLENE,
      DEDX_POLYPROPYLENE, DEDX_POLYSTYRENE, DEDX_POLYTETRAFLUOROETHYLENE,
      DEDX_POLYTRIFLUOROCHLOROETHYLENE, DEDX_POLYVINYL_ACETATE,
      DEDX_POLYVINYL_ALCOHOL, DEDX_POLYVINYL_BUTYRAL, DEDX_POLYVINYL_CHLORIDE,
      DEDX_POLYVINYLIDENE_CHLORIDE_SARAN, DEDX_POLYVINYLIDENE_FLUORIDE,
      DEDX_POLYVINYL_PYRROLIDONE, DEDX_POTASSIUM_IODIDE, DEDX_POTASSIUM_OXIDE,
      DEDX_PROPANE, DEDX_PROPANE_LIQUID, DEDX_N_PROPYL_ALCOHOL, DEDX_PYRIDINE,
      DEDX_RUBBER_BUTYL, DEDX_RUBBER_NATURAL, DEDX_RUBBER_NEOPRENE,
      DEDX_SILICON_DIOXIDE, DEDX_SILVER_BROMIDE, DEDX_SILVER_CHLORIDE,
      DEDX_SILVER_HALIDES_IN_PHOTOGRAPHIC_EMULSION, DEDX_SILVER_IODIDE,
      DEDX_SKIN_ICRP, DEDX_SODIUM_CARBONATE, DEDX_SODIUM_IODIDE,
      DEDX_SODIUM_MONOXIDE, DEDX_SODIUM_NITRATE, DEDX_STILBENE,
      DEDX_SUCROSE, DEDX_TERPHENYL, DEDX_TESTES_ICRP,
      DEDX_TETRACHLOROETHYLENE, DEDX_THALLIUM_CHLORIDE, DEDX_TISSUE_SOFT_ICRP,
      DEDX_TISSUE_SOFT_ICRU_FOUR_COMPONENT,
      DEDX_TISSUE_EQUIVALENT_GAS_METHANE_BASED,
      DEDX_TISSUE_EQUIVALENT_GAS_PROPANE_BASED, DEDX_TITANIUM_DIOXIDE,
      DEDX_TOLUENE, DEDX_TRICHLOROETHYLENE, DEDX_TRIETHYL_PHOSPHATE,
      DEDX_TUNGSTEN_HEXAFLUORIDE, DEDX_URANIUM_DICARBIDE,
      DEDX_URANIUM_MONOCARBIDE, DEDX_URANIUM_OXIDE, DEDX_UREA, DEDX_VALINE,
      DEDX_VITON_FLUOROELASTOMER, DEDX_WATER_LIQUID, DEDX_WATER_VAPOR,
      DEDX_XYLENE};

#define DEDX_PROTON     1
#define DEDX_ELECTRON   1001
#define DEDX_POSITRON   1002
#define DEDX_PIMINUS    1003
#define DEDX_PIPLUS     1004
#define DEDX_PIZERO     1005
#define DEDX_ANTIPROTON 1006

/* aliases */
#define DEDX_WATER    DEDX_WATER_LIQUID
#define DEDX_AIR      DEDX_AIR_DRY_NEAR_SEA_LEVEL
#define DEDX_PMMA     DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_PERSPEX  DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_LUCITE   DEDX_LUCITE_PERSPEX_PMMA
#define DEDX_TEFLON   DEDX_POLYTETRAFLUOROETHYLENE
#define DEDX_CONCRETE DEDX_CONCRETE_PORTLAND
#define DEDX_CAESIUM  DEDX_CESIUM


void dedx_get_error_code(char *err_str, int err);
const char * dedx_get_program_name(int program);
const char * dedx_get_program_version(int program);
const char * dedx_get_material_name(int material);
const char * dedx_get_ion_name(int ion);
void dedx_get_version(int *major, int *minor, int *patch, int *svn);
void dedx_get_composition(int target, float composition[][2], 
	unsigned int * comp_len, int *err);
float dedx_get_i_value(int target, int *err);

const int * dedx_get_program_list(void);
const int * dedx_get_material_list(int program);
const int * dedx_get_ion_list(int program);
float dedx_get_min_energy(int program, int ion);
float dedx_get_max_energy(int program, int ion);
typedef struct
{
	int cache;
	int hits;
	int miss;
} _dedx_lookup_accelerator;


typedef struct
{
	float a;
	float b;
	float c;
	float d;
	float x;
} _dedx_spline_base;

typedef struct
{
	_dedx_spline_base base[_DEDX_MAXELEMENTS];
	int n;
	int prog;
	int target;
	int ion;
	int datapoints;
	_dedx_lookup_accelerator acc;
} _dedx_lookup_data;


typedef struct
{	_dedx_lookup_data ** loaded_data;
	int datasets;
	int active_datasets;
	
} dedx_workspace;


dedx_workspace * dedx_allocate_workspace(unsigned int count, int *err);
void dedx_free_workspace(dedx_workspace * workspace, int *err);


//Experimental New API
typedef struct
{
  int cfg_id;
  int program;
  int target;            /* target can either be an element or a compound */
  int ion;               /* id number of projectile */
  int ion_a;             /* nucleon number of projectile */
  int bragg_used;        /* is 1 if braggs additivity rule was applied */
  int compound_state;    /* DEDX_DEFAULT=0,  DEDX_GAS DEDX_CONDENSED ... */
  
unsigned int elements_length;   /* elements_length  --- number of unique elements in comp. */
  unsigned int loaded;
  int * elements_id;     /* elements_id      --- Z of each element */
  int * elements_atoms;  /* elements_atoms   --- number of atoms per comp. unit */
  char mstar_mode;       /* DEDX_MSTAR_MODE_DEFAULT, _A, _B, _C ... */
  float i_value;         /* i_value   --- mean excitation potential of target  */
  float _temp_i_value; /*used intern for passing i_value to bethe function*/
  float rho;             /* density in g/cm^3 */
  float * elements_mass_fraction;     /* mass_fraction of each element */
  float * elements_i_value;           /* i_value of each element */
  const char * target_name;
  const char * ion_name;
  const char * program_name;
} dedx_config;


void dedx_load_config(dedx_workspace *ws, 
		dedx_config * config, int *err);
float dedx_get_stp(dedx_workspace * ws, 
		dedx_config * config, float energy, int * err);
float dedx_get_simple_stp(int ion, int target, float energy, int *err);
void dedx_free_config(dedx_config * config, int *err);
/*
   dedx_config must be specified BEFORE calling dedx_load_config()

cfg_id: configuration id, which is set by dedx_config. Don't touch.
program: must have
target: if NOT specified, then specify element_id
ion: must have
compound_state: if DEDX_DEFAULT (0), then this val is set to 
normal state of aggregation
elements_length: must be specified if target is undefined (DEDX_UNDEFINED)
 *elements_id: must be specified if target is undefined
 *elements_atoms: must be specified if target AND element_mass_fraction 
 are undefined
mstar_mode: if DEDX_DEFAULT, then 'b' method of MSTAR is used, which was recommended by H. Paul
i_value: if DEDX_DEFAULT, then ICRU I values are used for target compound, 
if target is set, or if target is 0, then it is calculated from 
the individual i-values set in elements_i_value.
rho: density of material, used by BETHE-type algorithms (DEDX_BETHE_EXT00)
elements_mass_fraction: must be specified if target=0 and elements_atoms = 0
elements_i_value: if target is 0, then individual I-values of elements can 
be specified here.

Once dedx_load_config() was invoked, dedx_config is out of scope, 
any changes to this structure will have no effect, except for cfg_id.
The user can still use it to check applied I-values, element compositions 
and states, etc.
 */

#endif // DEDX_H_INCLUDED
