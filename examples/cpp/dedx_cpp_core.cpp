// Core (stateful) libdedx API consumed from C++.
//
// Builds a proton-in-water stopping-power / CSDA-range table using the RAII
// helpers, std::vector and exception-based error handling. Demonstrates that
// dedx.h and dedx_tools.h link cleanly into a C++ program now that the public
// headers carry extern "C" guards.
#include <cstdlib>
#include <dedx.h>
#include <dedx_tools.h>
#include <iomanip>
#include <iostream>
#include <vector>

#include "dedx_raii.hpp"

int main() {
    try {
        auto ws = dedx::make_workspace(1);

        auto cfg = dedx::make_config();
        cfg->program = DEDX_PSTAR;
        cfg->ion = DEDX_PROTON;
        cfg->ion_a = 1; // nucleon number, required by dedx_get_csda
        cfg->target = DEDX_WATER;

        int err = DEDX_OK;
        dedx_load_config(ws.get(), cfg.get(), &err);
        dedx::check(err, "dedx_load_config");

        const std::vector<float> energies = {1.0f, 5.0f, 10.0f, 50.0f, 100.0f, 250.0f};

        std::cout << "Proton stopping power & CSDA range in liquid water (PSTAR)\n";
        std::cout << "  E [MeV/nucl]   dE/dx [MeV cm^2/g]   CSDA [g/cm^2]\n";
        std::cout << std::fixed << std::setprecision(4);

        for (const float e : energies) {
            const float stp = dedx_get_stp(ws.get(), cfg.get(), e, &err);
            dedx::check(err, "dedx_get_stp");

            const double csda = dedx_get_csda(ws.get(), cfg.get(), e, &err);
            dedx::check(err, "dedx_get_csda");

            std::cout << std::setw(14) << e << std::setw(21) << stp << std::setw(16) << csda << '\n';
        }

        return EXIT_SUCCESS;
    } catch (const dedx::error &ex) {
        std::cerr << "libdedx error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
