#ifndef DEDX_BETHE_H
#define DEDX_BETHE_H

#include "dedx_bethe_struct.h"

/** @brief Evaluate Bethe stopping power for one energy using cached work buffers.
 *  @param[in,out] ws      Cached Bethe/Gauss-search workspace.
 *  @param[in]     energy  Projectile energy in MeV/nucl (MeV per nucleon).
 *  @param[in]     PZ      Projectile charge.
 *  @param[in]     PA      Projectile mass number.
 *  @param[in]     TZ      Target atomic number.
 *  @param[in]     TA      Target mass number.
 *  @param[in]     rho     Target density in g/cm^3.
 *  @param[in]     Io_Pot  Mean excitation potential in eV.
 *  @return Stopping power in MeV cm^2/g.
 */
float dedx_internal_calculate_bethe_energy(
    dedx_internal_bethe_workspace *ws, float energy, float PZ, float PA, float TZ, float TA, float rho, float Io_Pot);

#endif // DEDX_BETHE_H
