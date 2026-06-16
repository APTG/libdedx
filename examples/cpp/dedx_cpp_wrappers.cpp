// Stateless wrapper + tools libdedx APIs consumed from C++.
//
// Reads the program's default tabulated grid, evaluates a custom energy grid
// in one batched call, and converts mass stopping power to linear stopping
// power — all backed by std::vector. Exercises dedx_wrappers.h and
// dedx_tools.h from a C++ translation unit.
#include <cstddef>
#include <cstdlib>
#include <dedx.h>
#include <dedx_tools.h>
#include <dedx_wrappers.h>
#include <iomanip>
#include <iostream>
#include <vector>

#include "dedx_raii.hpp"

int main() {
    try {
        constexpr int program = DEDX_PSTAR;
        constexpr int ion = DEDX_PROTON;
        constexpr int target = DEDX_WATER_LIQUID;

        // 1) Default tabulated grid shipped with the program.
        const int n = dedx_get_stp_table_size(program, ion, target);
        if (n <= 0) {
            std::cerr << "no tabulated data for this program/ion/target\n";
            return EXIT_FAILURE;
        }

        std::vector<float> grid_e(static_cast<std::size_t>(n));
        std::vector<float> grid_stp(static_cast<std::size_t>(n));
        // Returns 0 on success / negative on error; the point count is `n`.
        if (dedx_fill_default_energy_stp_table(program, ion, target, grid_e.data(), grid_stp.data()) < 0) {
            std::cerr << "dedx_fill_default_energy_stp_table failed\n";
            return EXIT_FAILURE;
        }
        std::cout << "Default PSTAR grid: " << n << " points, " << grid_e.front() << " - " << grid_e.back()
                  << " MeV/nucl\n";

        // 2) Evaluate an arbitrary energy grid in a single batched call.
        const std::vector<float> energies = {2.0f, 20.0f, 200.0f};
        std::vector<float> stps(energies.size());
        dedx::check(
            dedx_get_stp_table(program, ion, target, static_cast<int>(energies.size()), energies.data(), stps.data()),
            "dedx_get_stp_table");

        // 3) Convert mass stopping power [MeV cm^2/g] to linear units. For
        //    liquid water (rho = 1 g/cm^3) the MeV/cm value coincides
        //    numerically with the mass value, so keV/um is shown too to make
        //    the unit change visible.
        const auto convert = [&](int to_unit) {
            std::vector<float> out(stps.size());
            dedx::check(
                convert_units(DEDX_MEVCM2G, to_unit, target, static_cast<int>(stps.size()), stps.data(), out.data()),
                "convert_units");
            return out;
        };
        const std::vector<float> stps_mevcm = convert(DEDX_MEVCM);
        const std::vector<float> stps_kevum = convert(DEDX_KEVUM);

        std::cout << "  E [MeV/nucl]   dE/dx [MeV cm^2/g]   dE/dx [MeV/cm]   dE/dx [keV/um]\n";
        std::cout << std::fixed << std::setprecision(4);
        for (std::size_t i = 0; i < energies.size(); ++i) {
            std::cout << std::setw(14) << energies[i] << std::setw(21) << stps[i] << std::setw(16) << stps_mevcm[i]
                      << std::setw(16) << stps_kevum[i] << '\n';
        }

        return EXIT_SUCCESS;
    } catch (const dedx::error &ex) {
        std::cerr << "libdedx error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
