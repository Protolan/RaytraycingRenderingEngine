/// @file
///
/// @brief Basic physical constants.
///
/// Copyright &copy; INTEGRA, Inc., 2009.
///
/// @internal
/// @author Pbb - Boris Barladian<BR>

#ifndef _KISG_PHYS_CONST_HPP_
#define _KISG_PHYS_CONST_HPP_

INTEGRA_NAMESPACE_START

/// Basic physical constants.
class PhysConst
  {
  public:
    /// @name Constants
    //@{
    /// Watt to lumen coefficient on 555nm (lm/W).
    static const double WT2LUM;
    /// Watt to lumen coefficient averaged on 380-780nm range (lm/W).
    static const double AVEWT2LUM;
    /// @brief Watt to lumen coefficient averaged on 380-780nm range (lm/W)
    /// divided by integral = (r + g + b) * 1/3 = 1/3.
    static const double AVEWT2LUM3;
    // Planck constant 6.626068 * 10-34 m2 kg / sec
    static const double PLANCK;
    // Light speed 299,792,458 (exact)
    static const double LIGHTSPEED;
    // Boltzmann's constant   1.3806503 * 10-23 m2 kg sec-2 K-1
    static const double BOLTZMANN;
    //@}
  };  // class PhysConst>


INTEGRA_NAMESPACE_END
#endif

