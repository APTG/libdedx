#ifndef DEDX_CONST_H
#define DEDX_CONST_H

/**
 * @file dedx_const.h
 * @brief Shared physical constants used by libdedx internals.
 *
 * These are compile-time constants in the legacy units expected by the
 * original stopping-power implementations bundled in libdedx.
 */

/** Avogadro constant in mol^-1. */
#define DEDX_N_AVO 6.0221367

/** Atomic mass unit in MeV/c^2. */
#define DEDX_AMU 931.49432

/** Pi. */
#define DEDX_PI 3.14159265358979323846

/** Electron rest mass in MeV/c^2. */
#define DEDX_EMASS 0.51099906

/** Elementary charge scaling used by the legacy formulas. */
#define DEDX_ECHARGE 1.60217733

/** e^2 / (4 pi epsilon_0) in eV*Angstrom units used by the Bethe code. */
#define DEDX_ECHARGE2 14.399652

#endif // DEDX_CONST_H
