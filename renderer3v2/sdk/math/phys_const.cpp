/// @file
///
/// @brief Basic physical constants.
///
/// Copyright &copy; INTEGRA, Inc., 2009.
///
/// @internal
/// @author Pbb - Boris Barladian<BR>


#include <integra.hpp>

#include "phys_const.hpp"

INTEGRA_NAMESPACE_START

/// Watt to lumen coefficient on 555nm (lm/W).
const double PhysConst::WT2LUM = 683.0;
/// Watt to lumen coefficient averaged on 380-780nm range (lm/W).
const double PhysConst::AVEWT2LUM = 179.0;
/// Watt to lumen coefficient averaged on 380-780nm range (lm/W)
/// divided by integral = (r + g + b) * 1/3 = 1/3.
const double PhysConst::AVEWT2LUM3 = PhysConst::AVEWT2LUM * 3.0; // 537
/// Planck constant 6.626068 * 10-34 m2 kg / sec
const double PhysConst::PLANCK = 6.626068e-34;
/// Light speed 299,792,458 (exact)
const double PhysConst::LIGHTSPEED = 299792458;
/// Boltzmann's constant   1.3806503 * 10-23 m2 kg sec-2 K-1
const double PhysConst::BOLTZMANN =  1.3806503e-23;

INTEGRA_NAMESPACE_END

