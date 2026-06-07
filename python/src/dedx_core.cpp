// nanobind binding for libdedx — the low-level ``libdedx._core`` extension.
//
// This module statically links the libdedx C library and exposes the full
// public C API: the workspace/config object model, custom compounds, stopping
// power / CSDA range / inverse lookups, unit conversion, composition and
// I-value accessors, energy bounds, and the program/ion/material lists & names.
//
// numpy arrays are accepted (and returned) for the vectorised entry points so
// callers get array in / array out without manual buffer juggling.

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/vector.h>

extern "C" {
#include "dedx.h"
#include "dedx_periodic_table.h"
#include "dedx_tools.h"
#include "dedx_wrappers.h"
}

namespace nb = nanobind;
using namespace nb::literals;

namespace {

// Raise a Python exception carrying the libdedx error string for a non-zero code.
void check(int err) {
    if (err != 0) {
        char buf[256];
        dedx_get_error_code(buf, err);
        throw std::runtime_error("libdedx error " + std::to_string(err) + ": " + buf);
    }
}

// Read a -1 terminated static list returned by the C API into a vector.
std::vector<int> read_terminated_list(const int *list) {
    std::vector<int> out;
    if (list == nullptr)
        return out;
    for (int i = 0; list[i] != -1; ++i)
        out.push_back(list[i]);
    return out;
}

// 1-D contiguous float64 input array (numpy's default dtype).
using FloatArrayIn = nb::ndarray<const double, nb::ndim<1>, nb::c_contig, nb::device::cpu>;

std::vector<float> to_float_vector(const FloatArrayIn &arr) {
    size_t n = arr.shape(0);
    const double *src = arr.data();
    std::vector<float> out(n);
    for (size_t i = 0; i < n; ++i)
        out[i] = static_cast<float>(src[i]);
    return out;
}

// Build an owning 1-D numpy float64 array from a std::vector<double>.
nb::ndarray<nb::numpy, double> make_numpy(std::vector<double> &&values) {
    auto *data = new std::vector<double>(std::move(values));
    nb::capsule owner(data, [](void *p) noexcept { delete static_cast<std::vector<double> *>(p); });
    size_t n = data->size();
    return nb::ndarray<nb::numpy, double>(data->data(), {n}, owner);
}

// ---------------------------------------------------------------------------
// Config: owns a heap dedx_config. The C library frees the element arrays and
// the struct itself via dedx_free_config(), so every pointer we install must be
// malloc()'d and ownership handed to the library.
// ---------------------------------------------------------------------------
class Config {
  public:
    Config() {
        cfg_ = static_cast<dedx_config *>(std::calloc(1, sizeof(dedx_config)));
        if (cfg_ == nullptr)
            throw std::bad_alloc();
        cfg_->interpolation_mode = DEDX_INTERPOLATION_DEFAULT;
        cfg_->mstar_mode = DEDX_MSTAR_MODE_DEFAULT;
    }

    ~Config() {
        int err = 0;
        dedx_free_config(cfg_, &err);
    }

    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;

    dedx_config *raw() { return cfg_; }

    // --- scalar accessors -------------------------------------------------
    int program() const { return cfg_->program; }
    void set_program(int v) { cfg_->program = v; }
    int target() const { return cfg_->target; }
    void set_target(int v) { cfg_->target = v; }
    int ion() const { return cfg_->ion; }
    void set_ion(int v) { cfg_->ion = v; }
    int compound_state() const { return cfg_->compound_state; }
    void set_compound_state(int v) { cfg_->compound_state = v; }
    int interpolation_mode() const { return cfg_->interpolation_mode; }
    void set_interpolation_mode(int v) { cfg_->interpolation_mode = v; }
    int mstar_mode() const { return static_cast<unsigned char>(cfg_->mstar_mode); }
    void set_mstar_mode(int v) { cfg_->mstar_mode = static_cast<char>(v); }
    float i_value() const { return cfg_->i_value; }
    void set_i_value(float v) { cfg_->i_value = v; }
    float rho() const { return cfg_->rho; }
    void set_rho(float v) { cfg_->rho = v; }

    // --- resolved fields -------------------------------------------------
    int cfg_id() const { return cfg_->cfg_id; }
    int ion_a() const { return cfg_->ion_a; }
    // Nucleon number; normally filled by load(), but writable to support
    // isotopes or ions for which the default cannot be resolved.
    void set_ion_a(int v) { cfg_->ion_a = v; }
    bool bragg_used() const { return cfg_->bragg_used != 0; }
    bool loaded() const { return cfg_->loaded != 0; }

    nb::object target_name() const { return name_or_none(cfg_->target_name); }
    nb::object ion_name() const { return name_or_none(cfg_->ion_name); }
    nb::object program_name() const { return name_or_none(cfg_->program_name); }

    // --- custom compound element arrays ----------------------------------
    std::vector<int> elements_id() const {
        return copy_ints(cfg_->elements_id, cfg_->elements_length);
    }
    void set_elements_id(const std::vector<int> &v) {
        replace_ints(&cfg_->elements_id, v);
        cfg_->elements_length = static_cast<unsigned int>(v.size());
    }
    std::vector<int> elements_atoms() const {
        return copy_ints(cfg_->elements_atoms, cfg_->elements_length);
    }
    void set_elements_atoms(const std::vector<int> &v) {
        check_length(v.size());
        replace_ints(&cfg_->elements_atoms, v);
    }
    std::vector<float> elements_mass_fraction() const {
        return copy_floats(cfg_->elements_mass_fraction, cfg_->elements_length);
    }
    void set_elements_mass_fraction(const std::vector<float> &v) {
        check_length(v.size());
        replace_floats(&cfg_->elements_mass_fraction, v);
    }
    std::vector<float> elements_i_value() const {
        return copy_floats(cfg_->elements_i_value, cfg_->elements_length);
    }
    void set_elements_i_value(const std::vector<float> &v) {
        check_length(v.size());
        replace_floats(&cfg_->elements_i_value, v);
    }

  private:
    dedx_config *cfg_;

    static nb::object name_or_none(const char *s) {
        if (s == nullptr)
            return nb::none();
        return nb::cast(std::string(s));
    }
    static std::vector<int> copy_ints(const int *p, unsigned int n) {
        std::vector<int> out;
        if (p != nullptr)
            out.assign(p, p + n);
        return out;
    }
    static std::vector<float> copy_floats(const float *p, unsigned int n) {
        std::vector<float> out;
        if (p != nullptr)
            out.assign(p, p + n);
        return out;
    }
    void check_length(size_t n) const {
        if (cfg_->elements_length != 0 && n != cfg_->elements_length)
            throw std::invalid_argument("element array length must match elements_id length");
    }
    static void replace_ints(int **slot, const std::vector<int> &v) {
        std::free(*slot);
        *slot = nullptr;
        if (!v.empty()) {
            *slot = static_cast<int *>(std::malloc(sizeof(int) * v.size()));
            if (*slot == nullptr)
                throw std::bad_alloc();
            std::memcpy(*slot, v.data(), sizeof(int) * v.size());
        }
    }
    static void replace_floats(float **slot, const std::vector<float> &v) {
        std::free(*slot);
        *slot = nullptr;
        if (!v.empty()) {
            *slot = static_cast<float *>(std::malloc(sizeof(float) * v.size()));
            if (*slot == nullptr)
                throw std::bad_alloc();
            std::memcpy(*slot, v.data(), sizeof(float) * v.size());
        }
    }
};

// ---------------------------------------------------------------------------
// Workspace: owns a dedx_workspace and drives evaluation against a Config.
// ---------------------------------------------------------------------------
class Workspace {
  public:
    explicit Workspace(unsigned int count) {
        int err = 0;
        ws_ = dedx_allocate_workspace(count, &err);
        check(err);
        if (ws_ == nullptr)
            throw std::runtime_error("failed to allocate libdedx workspace");
    }

    ~Workspace() {
        int err = 0;
        dedx_free_workspace(ws_, &err);
    }

    Workspace(const Workspace &) = delete;
    Workspace &operator=(const Workspace &) = delete;

    int load(Config &config) {
        int err = 0;
        int id = dedx_load_config(ws_, config.raw(), &err);
        check(err);
        // load_config does not populate ion_a (only the C convenience wrappers
        // do), yet csda()/inverse_*() require it. Fill it here so the object
        // model "just works"; ignore failures for ions without nucleon data.
        if (config.raw()->ion_a <= 0) {
            int nerr = 0;
            int a = dedx_internal_get_nucleon(config.raw()->ion, &nerr);
            if (nerr == 0)
                config.raw()->ion_a = a;
        }
        return id;
    }

    float stp(Config &config, float energy) {
        int err = 0;
        float v = dedx_get_stp(ws_, config.raw(), energy, &err);
        check(err);
        return v;
    }

    double csda(Config &config, float energy) {
        int err = 0;
        double v = dedx_get_csda(ws_, config.raw(), energy, &err);
        check(err);
        return v;
    }

    double inverse_stp(Config &config, float stopping_power, int side) {
        int err = 0;
        double v = dedx_get_inverse_stp(ws_, config.raw(), stopping_power, side, &err);
        check(err);
        return v;
    }

    double inverse_csda(Config &config, float range) {
        int err = 0;
        double v = dedx_get_inverse_csda(ws_, config.raw(), range, &err);
        check(err);
        return v;
    }

    int datasets() const { return ws_->datasets; }
    int active_datasets() const { return ws_->active_datasets; }

  private:
    dedx_workspace *ws_;
};

// ---------------------------------------------------------------------------
// Free functions
// ---------------------------------------------------------------------------
std::tuple<int, int, int> version() {
    int major = 0, minor = 0, patch = 0;
    dedx_get_version(&major, &minor, &patch);
    return {major, minor, patch};
}

std::string version_string() { return dedx_get_version_string(); }

std::string error_string(int err) {
    char buf[256];
    dedx_get_error_code(buf, err);
    return std::string(buf);
}

std::string program_name(int program) { return dedx_get_program_name(program); }
std::string program_version(int program) { return dedx_get_program_version(program); }
std::string material_name(int material) { return dedx_get_material_name(material); }
std::string ion_name(int ion) { return dedx_get_ion_name(ion); }

std::vector<int> program_list() { return read_terminated_list(dedx_get_program_list()); }
std::vector<int> material_list(int program) { return read_terminated_list(dedx_get_material_list(program)); }
std::vector<int> ion_list(int program) { return read_terminated_list(dedx_get_ion_list(program)); }

float min_energy(int program, int ion) { return dedx_get_min_energy(program, ion); }
float max_energy(int program, int ion) { return dedx_get_max_energy(program, ion); }

float i_value(int target) {
    int err = 0;
    float v = dedx_get_i_value(target, &err);
    check(err);
    return v;
}

// Returns an (N, 2) numpy array with [atomic_number, mass_fraction] per row.
nb::ndarray<nb::numpy, double> composition(int target) {
    float buf[64][2];
    unsigned int len = 0;
    int err = 0;
    dedx_get_composition(target, buf, &len, &err);
    check(err);
    auto *data = new std::vector<double>(static_cast<size_t>(len) * 2);
    for (unsigned int i = 0; i < len; ++i) {
        (*data)[i * 2 + 0] = buf[i][0];
        (*data)[i * 2 + 1] = buf[i][1];
    }
    nb::capsule owner(data, [](void *p) noexcept { delete static_cast<std::vector<double> *>(p); });
    return nb::ndarray<nb::numpy, double>(data->data(), {static_cast<size_t>(len), 2}, owner);
}

float simple_stp(int ion, int target, float energy) {
    int err = 0;
    float v = dedx_get_simple_stp(ion, target, energy, &err);
    check(err);
    return v;
}

float simple_stp_for_program(int program, int ion, int target, float energy) {
    int err = 0;
    float v = dedx_get_simple_stp_for_program(program, ion, target, energy, &err);
    check(err);
    return v;
}

int stp_table_size(int program, int ion, int target) {
    return dedx_get_stp_table_size(program, ion, target);
}

nb::ndarray<nb::numpy, double> stp_table(int program, int ion, int target, const FloatArrayIn &energies) {
    std::vector<float> e = to_float_vector(energies);
    std::vector<float> s(e.size());
    int ret = dedx_get_stp_table(program, ion, target, static_cast<int>(e.size()), e.data(), s.data());
    check(ret);
    std::vector<double> out(s.begin(), s.end());
    return make_numpy(std::move(out));
}

nb::ndarray<nb::numpy, double> csda_range_table(int program, int ion, int target, const FloatArrayIn &energies) {
    std::vector<float> e = to_float_vector(energies);
    std::vector<double> r(e.size());
    int ret = dedx_get_csda_range_table(program, ion, target, static_cast<int>(e.size()), e.data(), r.data());
    check(ret);
    return make_numpy(std::move(r));
}

std::tuple<nb::ndarray<nb::numpy, double>, nb::ndarray<nb::numpy, double>>
default_energy_stp_table(int program, int ion, int target) {
    int n = dedx_get_stp_table_size(program, ion, target);
    if (n < 0)
        check(n);
    if (n == 0)
        throw std::runtime_error("no tabulated data for the requested program/ion/target");
    std::vector<float> e(n), s(n);
    int ret = dedx_fill_default_energy_stp_table(program, ion, target, e.data(), s.data());
    if (ret < 0)
        check(ret);
    std::vector<double> ed(e.begin(), e.end());
    std::vector<double> sd(s.begin(), s.end());
    return {make_numpy(std::move(ed)), make_numpy(std::move(sd))};
}

nb::ndarray<nb::numpy, double>
convert_units_py(int old_unit, int new_unit, int material, const FloatArrayIn &values) {
    std::vector<float> in = to_float_vector(values);
    // The C convert_units() short-circuits when the units match and leaves the
    // output untouched, so handle that case here to return the values unchanged.
    if (old_unit == new_unit) {
        std::vector<double> res(in.begin(), in.end());
        return make_numpy(std::move(res));
    }
    std::vector<float> out(in.size());
    int ret = convert_units(old_unit, new_unit, material, static_cast<int>(in.size()), in.data(), out.data());
    check(ret);
    std::vector<double> res(out.begin(), out.end());
    return make_numpy(std::move(res));
}

}  // namespace

NB_MODULE(_core, m) {
    m.doc() = "Low-level nanobind binding for the libdedx stopping-power C library.";

    // ---- enums / identifiers -------------------------------------------
    // Programs
    m.attr("ASTAR") = (int)DEDX_ASTAR;
    m.attr("PSTAR") = (int)DEDX_PSTAR;
    m.attr("ESTAR") = (int)DEDX_ESTAR;
    m.attr("MSTAR") = (int)DEDX_MSTAR;
    m.attr("ICRU73_OLD") = (int)DEDX_ICRU73_OLD;
    m.attr("ICRU73") = (int)DEDX_ICRU73;
    m.attr("ICRU49") = (int)DEDX_ICRU49;
    m.attr("ICRU") = (int)DEDX_ICRU;
    m.attr("DEFAULT") = (int)DEDX_DEFAULT;
    m.attr("BETHE_EXT00") = (int)DEDX_BETHE_EXT00;

    // Aggregate states
    m.attr("DEFAULT_STATE") = (int)DEDX_DEFAULT_STATE;
    m.attr("GAS") = (int)DEDX_GAS;
    m.attr("CONDENSED") = (int)DEDX_CONDENSED;

    // MSTAR modes
    m.attr("MSTAR_MODE_A") = (int)DEDX_MSTAR_MODE_A;
    m.attr("MSTAR_MODE_B") = (int)DEDX_MSTAR_MODE_B;
    m.attr("MSTAR_MODE_G") = (int)DEDX_MSTAR_MODE_G;
    m.attr("MSTAR_MODE_H") = (int)DEDX_MSTAR_MODE_H;
    m.attr("MSTAR_MODE_C") = (int)DEDX_MSTAR_MODE_C;
    m.attr("MSTAR_MODE_D") = (int)DEDX_MSTAR_MODE_D;
    m.attr("MSTAR_MODE_DEFAULT") = (int)DEDX_MSTAR_MODE_DEFAULT;

    // Interpolation modes
    m.attr("INTERPOLATION_LOG_LOG") = (int)DEDX_INTERPOLATION_LOG_LOG;
    m.attr("INTERPOLATION_LINEAR") = (int)DEDX_INTERPOLATION_LINEAR;
    m.attr("INTERPOLATION_DEFAULT") = (int)DEDX_INTERPOLATION_DEFAULT;

    // Stopping-power units
    m.attr("MEVCM2G") = (int)DEDX_MEVCM2G;
    m.attr("MEVCM") = (int)DEDX_MEVCM;
    m.attr("KEVUM") = (int)DEDX_KEVUM;

    // A few common ions / materials for convenience.
    m.attr("HYDROGEN") = (int)DEDX_HYDROGEN;
    m.attr("PROTON") = (int)DEDX_PROTON;
    m.attr("HELIUM") = (int)DEDX_HELIUM;
    m.attr("CARBON") = (int)DEDX_CARBON;
    m.attr("WATER") = (int)DEDX_WATER;
    m.attr("WATER_LIQUID") = (int)DEDX_WATER_LIQUID;
    m.attr("WATER_VAPOR") = (int)DEDX_WATER_VAPOR;
    m.attr("AIR") = (int)DEDX_AIR;

    // ---- Config --------------------------------------------------------
    nb::class_<Config>(m, "Config", "Stopping-power calculation configuration.")
        .def(nb::init<>())
        .def_prop_rw("program", &Config::program, &Config::set_program)
        .def_prop_rw("target", &Config::target, &Config::set_target)
        .def_prop_rw("ion", &Config::ion, &Config::set_ion)
        .def_prop_rw("compound_state", &Config::compound_state, &Config::set_compound_state)
        .def_prop_rw("interpolation_mode", &Config::interpolation_mode, &Config::set_interpolation_mode)
        .def_prop_rw("mstar_mode", &Config::mstar_mode, &Config::set_mstar_mode)
        .def_prop_rw("i_value", &Config::i_value, &Config::set_i_value)
        .def_prop_rw("rho", &Config::rho, &Config::set_rho)
        .def_prop_rw("elements_id", &Config::elements_id, &Config::set_elements_id)
        .def_prop_rw("elements_atoms", &Config::elements_atoms, &Config::set_elements_atoms)
        .def_prop_rw("elements_mass_fraction", &Config::elements_mass_fraction, &Config::set_elements_mass_fraction)
        .def_prop_rw("elements_i_value", &Config::elements_i_value, &Config::set_elements_i_value)
        .def_prop_ro("cfg_id", &Config::cfg_id)
        .def_prop_rw("ion_a", &Config::ion_a, &Config::set_ion_a)
        .def_prop_ro("bragg_used", &Config::bragg_used)
        .def_prop_ro("loaded", &Config::loaded)
        .def_prop_ro("target_name", &Config::target_name)
        .def_prop_ro("ion_name", &Config::ion_name)
        .def_prop_ro("program_name", &Config::program_name);

    // ---- Workspace -----------------------------------------------------
    nb::class_<Workspace>(m, "Workspace", "Workspace holding preloaded stopping-power datasets.")
        .def(nb::init<unsigned int>(), "count"_a = 1)
        .def("load", &Workspace::load, "config"_a,
             "Load a configuration; returns the dataset id and populates resolved fields.")
        .def("stp", &Workspace::stp, "config"_a, "energy"_a,
             "Mass stopping power (MeV cm2/g) at the given energy (MeV/nucl).")
        .def("csda", &Workspace::csda, "config"_a, "energy"_a,
             "CSDA range (g/cm2) at the given energy (MeV/nucl). Requires a prior load().")
        .def("inverse_stp", &Workspace::inverse_stp, "config"_a, "stp"_a, "side"_a,
             "Energy (MeV/nucl) for a stopping power; side<0 low-energy branch, side>=0 high-energy branch.")
        .def("inverse_csda", &Workspace::inverse_csda, "config"_a, "range"_a,
             "Energy (MeV/nucl) for a CSDA range (g/cm2). Requires a prior load().")
        .def_prop_ro("datasets", &Workspace::datasets)
        .def_prop_ro("active_datasets", &Workspace::active_datasets);

    // ---- module-level functions ----------------------------------------
    m.def("version", &version, "Return the (major, minor, patch) library version.");
    m.def("version_string", &version_string, "Return the full library version string.");
    m.def("error_string", &error_string, "err"_a, "Human-readable description of an error code.");
    m.def("program_name", &program_name, "program"_a);
    m.def("program_version", &program_version, "program"_a);
    m.def("material_name", &material_name, "material"_a);
    m.def("ion_name", &ion_name, "ion"_a);
    m.def("program_list", &program_list);
    m.def("material_list", &material_list, "program"_a);
    m.def("ion_list", &ion_list, "program"_a);
    m.def("min_energy", &min_energy, "program"_a, "ion"_a);
    m.def("max_energy", &max_energy, "program"_a, "ion"_a);
    m.def("i_value", &i_value, "target"_a, "Mean excitation potential (eV) of a material.");
    m.def("composition", &composition, "target"_a,
          "Elemental composition as an (N, 2) array of [Z, mass_fraction].");
    m.def("simple_stp", &simple_stp, "ion"_a, "target"_a, "energy"_a);
    m.def("simple_stp_for_program", &simple_stp_for_program, "program"_a, "ion"_a, "target"_a, "energy"_a);
    m.def("stp_table_size", &stp_table_size, "program"_a, "ion"_a, "target"_a);
    m.def("stp_table", &stp_table, "program"_a, "ion"_a, "target"_a, "energies"_a);
    m.def("csda_range_table", &csda_range_table, "program"_a, "ion"_a, "target"_a, "energies"_a);
    m.def("default_energy_stp_table", &default_energy_stp_table, "program"_a, "ion"_a, "target"_a,
          "Return (energies, stps) for the built-in tabulated data points.");
    m.def("convert_units", &convert_units_py, "old_unit"_a, "new_unit"_a, "material"_a, "values"_a,
          "Convert an array of stopping-power values between unit systems.");
}
