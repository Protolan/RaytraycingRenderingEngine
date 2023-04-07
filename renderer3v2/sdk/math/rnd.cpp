/// @internal
/// @file
///
/// @brief Functions of the Rnd class for calculation
///        of the random number
///
/// @author Pls Lev Shapiro<BR>
///         Pnd Nickolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.


#include <integra.hpp>

#include "rnd.hpp"

INTEGRA_NAMESPACE_START

// **********************************************************
//                  STATIC VARIABLES
// **********************************************************
// CONSTANTS

/// Number of handler fields
static const int NUMBER_OF_HANDLER_FIELDS = 3;


/**
@class Rnd math/rnd.hpp

Class for random number generator.

The Rnd class provides generation of the pseudo-random values. It may be real 
values on the unit segment or arbitrary segment, integer values of the integral
sequence, random points on the sphere (in polar coordinates) with various types 
of probability density.

The Rnd class is used in the following way. In the first step, the object of 
the Rnd class is created by the some unsigned parameter. Value of this 
parameter predetermines the sequence of pseudo-random values which may be 
generated. In the next steps the method Rnd::DRnd() is called for the 
pseudo-random numbers generation. This method (on the each call) returns the 
some pseudo-random value.

@note User should take into account that unsigned parameter of the Rnd 
constructor predetermines the @b full random sequence. For the prevention of 
the repeated sequence (with the same random values) generation the current 
time is used as parameter of the constructor by the default.

*/

///////////////////////////////////////////////////////////////////////////////
/// Constructor with initializing class data fields by the given value (current 
/// time by default).
///
/// This constructor creates Rnd object, initializes all internal parameters
/// according the given value.
/// @note Unsigned parameter predetermines the @b full random sequence.
///
/// @param iseed - Value for the Rnd object initialization. 
Rnd::Rnd(unsigned iseed)
  {
  // Filling of the RNG fields
  m1 = iseed % (MOD1 - 1) + 1;
  m2 = iseed % (MOD2 - 1) + 1;
  m3 = iseed % (MOD3 - 1) + 1;

  DRnd();  // actualize 'dvalue'
  gauss_prev = 0;
  has_gauss_prev = false;
  } // END OF Rnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Constructor with initializing class data fields by the input string.
///
/// This constructor creates Rnd object according parameters stored in
/// parameter str.
///
/// @param str - In this parameter the whole data necessary for Rnd class 
/// creation is stored. This string must be stored previously by the method 
/// RndStore()
Rnd::Rnd(const char *str)
  {
  // read values from string; if number of tokens in the string does
  // not equal number of handler fields (3) then RND parameters
  // set as default
  if (sscanf(str, "%lx %lx %lx", &m1, &m2, &m3) != NUMBER_OF_HANDLER_FIELDS)
    {
    Assert(false);
    m1 = m2 = m3 = 1L;
    }

  Gen();  // actualize 'dvalue' by pseudo-random number in acc. to m1-m3
  gauss_prev = 0;
  has_gauss_prev = false;
  } // END OF Rnd FUNCTION

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param u - Random generator.
/// @note Copy contructor should provide the equality for operations:
/// {u.RndStore(str); Rnd a(str);}  <=>  {Rnd a(u)}
Rnd::Rnd(const Rnd &u)
  {
  m1 = u.m1;
  m2 = u.m2;
  m3 = u.m3;
  Gen();
  gauss_prev = 0;
  has_gauss_prev = false;
  } // END OF Rnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Picks normal deviate with zero mean and unit variance.
///
/// Generates a normal deviate with zero mean and unit variance using 
/// Box Miller algorithm: 
/// - pick two uniform deviates @a x and @a y, then
/// - calculate polar coordinates (@a r, @a phi) of the point (@a x, @a y) so 
/// that:
///   - @a v1 = exp(-@a r^2 / 2), 
///   - @a v2 = @a phi / @a 2pi and 
/// - finally obtain two normal deviates @a x and @a y with zero mean and unit 
/// variance 
///   - @a x = @a r * @a cos(phi), 
///   - @a y = @a r * @a sin(phi).
///
/// For better performance a trick is used: we pick a point (@a v1, @a v2)
/// distributed uniformly in the unit CIRCLE (not SQUARE!) and then we can
/// calculate @a phi without calls to trig functions.
///
/// @note See Teukolsky et al, "Numerical Recipes in C", for more detail.
///
/// @return Pseudo random deviate on (0,1)
double Rnd::Gauss()
  {
  double v1, v2, rsq, fac;

  if (!has_gauss_prev)
    {
    // We do not have a normal deviate to return. Thus calculate it.

    do // Take two uniform deviates so that (v1, v2) is in unit circle
      {
      v1 = -1 + 2 * DRnd();   // uniform in [-1,1]
      v2 = -1 + 2 * DRnd();   // uniform in [-1,1]
      rsq = v1 * v1 + v2 * v2;
      } while (rsq >= 1 || rsq == 0); // If (v1, v2) is not in unit circle
                                      // try another

    // The 2D point (v1, v2) belongs to unit circle. The 'rsq' is
    // squared radius: rsq=r^2 and thus is UNIFORM deviate in [0,1].
    // The polar coordinates of the point (v1, v2), i.e. 'phi' and 'r',
    // are independent. Here 'phi' is the angle between (v1,v2) and v1-axis.

    fac = Sqrt(-2 * Log(rsq) / rsq);
    // Now
    //
    //  v1 * fac = sqrt(-2log(r^2)) * v1 / sqrt(r^2)
    //           = sqrt(-2log(r^2)) * cos(phi),
    //
    //  v2 * fac = sqrt(-2log(r^2)) * sin(phi),
    //
    //  recalling that r^2 is uniform deviate in [0,1] and that it is
    //  independent of 'phi', we recognise in the above the Box-Miller
    //  transform: two uniform -> two Gaussian which are: v1*fac, v2*fac.
    //  Return the first and store the second (to return next time)
    gauss_prev = v1 * fac;
    has_gauss_prev = true; // Means that we HAVE an extra deviate handy!
    return v2 * fac;
    }
  else
    {
    has_gauss_prev = false; // Spare deviate is exhausted
    return gauss_prev;
    }
  }

///////////////////////////////////////////////////////////////////////////////
/// Save the class data in the string.
///
/// Stores Rnd object current state in string. This method allows to obtain 
/// description of particular  RNG (random number generator) in text form. It 
/// can be useful for storage of the data structure into text file. The  stored 
/// data can be later used (by appropriate constructor Rnd(const char *str). The 
/// clone is exact copy of the RNG at the moment of RndStore() call, it will 
/// generate the same sequence  of random numbers.
///
/// @note Parameter must be pointer to character buffer with enum STRING_LEN 
/// size.
/// @param[out] buf - String with text representation of given RNG data fields.
void Rnd::RndStore(char *buf)
  {
  // save handler fields to string
  sprintf(buf, "%lx %lx %lx", m1, m2, m3);
  Assert(strlen(buf) < STRING_LEN);
  } // END OF RndStore FUNCTION

//////////////////////////////////////////////////////////////////////////////
/// Create seeds for independent Rnds.
/// @param[in] num  Seeds number.
/// @param[out] rnd_seeds  Seeds array.
void Rnd::CreateSeedsMultiRnd(int num, TArray<unsigned> &rnd_seeds)
  {
  // Mask for Rnd seeds
  const static unsigned SEED_MASK = 0x7FFFFFFF;
  rnd_seeds.Allocate(num);
  unsigned rnd_seed = Time::Get();
  for (int itd = 0; itd < num; itd++)
    {
    do
      {
      rnd_seed = (rnd_seed * Time::Get()) ^ Time::Get();
      rnd_seed = rnd_seed & SEED_MASK;
      }
    while (!IsRndSeedUnique(rnd_seed, itd, rnd_seeds));

    rnd_seeds[itd] = rnd_seed;
    } 
  } // CreateSeedsMultiRnd()

//////////////////////////////////////////////////////////////////////////////
/// Checks if new random seed is unique in random seeds array.
/// @param[in] seed Random seed.
/// @param[in] last Index of last element in seeds array.
/// @param[in] rnd_seeds Random seeds array.
/// @return True, if unique, false otherwise.
bool Rnd::IsRndSeedUnique(unsigned seed, int last, const TArray<unsigned> &rnd_seeds)
  {
  for (int ii = 0; ii < last; ii++)
    {
    if (rnd_seeds[ii] == seed)
      return false;
    }

  return true;
  } // IsRndSeedUnique()


INTEGRA_NAMESPACE_END
