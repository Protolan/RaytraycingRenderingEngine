/// @file
///
/// @brief Random number generator class header file
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Pls Lev Shapiro<BR>
///         Pnd Nickolay Derjabin<BR>
///         Ose Sergey Ershov<BR>

#ifndef _KSML_RND_HPP_
#define _KSML_RND_HPP_

#include <base/arrays.hpp>
#include <base/base.hpp>
#include <base/time.hpp>

#include "vect2.hpp"

INTEGRA_NAMESPACE_START

/// Class for random number generator.
class Rnd
  {
  public:

    /// Maximal string length in the method RndStore
    enum
      {
      STRING_LEN = 30
      };

    /// @name Constructors, destructor is default
    //@{
    /// Constructor with initializing class data fields by the input string.
    Rnd(const char *str);
    /// @brief Constructor with initializing class data fields
    /// by the given value (current time by default).
    Rnd(unsigned iseed = Time::Get());
    /// Copy constructor.
    Rnd(const Rnd &u);
    //@}

    /// @name Services.
    //@{
    /// Pseudo-random number in segment [0, 1).
    virtual inline double DRnd();
    /// Random number in segment [0, 1).
    virtual inline double DRndP();
    /// Pseudo-random number in the arbitrary segment.
    inline double DRnd(double a, double b);
    /// Pointer to current pseudo-random number.
    virtual inline double *DRndPtr();
    /// ReNormalize
    virtual inline void ReNorm(double &prob, double pp, double pp_prev);
    /// Pseudo-random number in integer range [0, n - 1].
    inline int IRnd(int n);
    /// Save the class data in the string.
    void RndStore(char *buf);
    /// Picks normal deviate with zero mean and unit variance.
    double Gauss();
    /// @brief Pseudo-random point in the cell of the sphere with uniform
    /// probability density.
    inline Point2d SphrUnif(const double phi[2], const double cos_theta[2]);
    /// @brief Pseudo-random point in the cell of the sphere with bilinear
    /// probability density.
    inline Point2d SphrBiLin(const double phi[2], const double theta[2],
                             const double cos_theta[2], const double f[2][2]);
    /// @brief Pseudo-random value in the segment of the sphere with bilinear
    /// probability density (function is dependent from one polar coordinate only).
    inline double SphrLin(const double theta[2],
                          const double cos_theta[2],
                          const double f[2]);
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const Rnd &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const Rnd &u) const;
    //@}
    /// Create seeds for independent Rnds.
    static void CreateSeedsMultiRnd(int num, TArray<unsigned> &rnd_seeds);

  private:
    enum
      {
      MOD1  = 30269,
      MOD2  = 30307,
      MOD3  = 30323,
      MULT1 = 171,
      MULT2 = 172,
      MULT3 = 170
      };

    /// Actualize 'dvalue' by pseudo-random number in acc. to m1-m3.
    inline void Gen();
    /// Checks if new random seed is unique in random seeds array.
    static bool IsRndSeedUnique(unsigned seed, int last, const TArray<unsigned> &rnd_seeds);

  private:
    /// Current pseudo-random value (likely in [0, 1) interval)
    double dvalue;
    /// First data field
    long m1;
    /// Second data field
    long m2;
    /// Third data field
    long m3;
    /// Spare normal deviate generated and rememberd by Gauss() on previous call
    double gauss_prev;
    /// Whether "gauss_prev" exists
    bool   has_gauss_prev;
  }; // end class "Rnd"


///////////////////////////////////////////////////////////////////////////////
/// Actualize internal parameter 'dvalue' by pseudo-random number in acc.
/// to m1-m3
///
/// @note Pseudo-random number in segment [0, 1)
/// @return None (this class is affected only)
void Rnd::Gen()
  {
  dvalue = (double)m1 / MOD1 + (double)m2 / MOD2 + (double)m3 / MOD3;
  dvalue -= (int)dvalue;  // fractional part of 'dvalue'
  Assert(0.0 <= dvalue && dvalue < 1.0);
  }  // END OF Gen() FUNCTION


///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random number in segment [0, 1).
///
/// Method generates pseudo-random number in segment [0, 1).
///
/// @return Pseudo-random number in segment [0, 1).
double Rnd::DRnd()
  {
  // generate random number
  m1 = (m1 * MULT1) % MOD1;
  m2 = (m2 * MULT2) % MOD2;
  m3 = (m3 * MULT3) % MOD3;
  double result = dvalue;
  Gen();  // actualize 'dvalue' by pseudo-random number in acc. to m1-m3
  return result;
  }  // END OF DRnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Random number in segment [0, 1).
///
/// For Rnd this function is equal to DRnd - generates new random number.
/// Is necessary in The RDISurfAttr::ChooseSurfEvent,
/// where new random number was generated for Rnd and was not for Niederreiter.
///
/// @return Pseudo-random number in segment [0, 1).
double Rnd::DRndP()
  {
  return DRnd();
  }  // END OF DRnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random number in the arbitrary segment.
///
/// Method generates pseudo-random number in the segment [a, b), if a < b, and
/// in  segment (b, a] if a > b.
///
/// @param  a - Begin of the segment,
/// @param  b - End of the segment.
/// @return Pseudo-random number in given segment.
double Rnd::DRnd(double a, double b)
  {
  return (a + (b - a) * DRnd());
  }  // END OF DRnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Pointer to current pseudo-random number.
///
/// Method returns pointer to current pseudo-random number calculated after
/// DRnd() and DRnd(double a, double b) calls (the next call of these
/// methods changes contents of this pointer).
///
/// This provides a possibility to  change this value arbitrarily, which can be
/// useful to sample several descrete events by single pseudo-random value.
///
/// @if INTEGRA_INTERNAL
/// (see rmaq05ds.Rkd for more details)
/// @endif
/// This is highly recommended that changes to pointed value still leave it
/// in [0, 1) interval.
///
/// @return Pointer to value in [0, 1)
double *Rnd::DRndPtr()
  {
  Assert(0.0 <= dvalue && dvalue < 1.0);
  return &dvalue;
  }

///////////////////////////////////////////////////////////////////////////////
/// Re-normalize.
///
/// This function does nothing for Rnd - it is introduced for the sake
/// of avoiding clones of SurfAttrs functions.
/// @param prob    Initial probability.
/// @param pp      Current probability.
/// @param pp_prev Previous probability.
void Rnd::ReNorm(double &prob, double pp, double pp_prev)
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random number in integer range [0, n - 1].
///
/// @param n - Upper bound defines range: 0, 1, ... n - 1.
///
/// @return Pseudo-random number in range [0, n - 1].
int Rnd::IRnd(int n)
  {
  Assert(n > 0);

  // scale [0, 1] to 0....n-1
  int nx = (int)(n * DRnd());
  Assert(nx < n);
  return (nx == n) ? 0 : nx;
  } // END OF IRnd FUNCTION


///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random point in the cell of the sphere with uniform probability
/// density.
///
/// Generates a deviate (phi, theta) uniformly distributed over a sphere cell.
/// "Uniformly" means that "the number of points" @a dN per solid angle is
/// constant, that is
///
/// <CENTER> @a dN = const * @a sin(theta) * @a d(theta) * @a d(phi) </CENTER>
///
/// The deviate may be generated both an a whole sphere or in a cell.
///
/// @param phi - Array of two values, cell bounds in azimuth in ANY order
/// (i.e. the second element may be smaller).
///
/// @param cos_theta -  Array of two values, cell bounds in polar angle
/// (through its cosine which is posible because in [0, PI] cosine is
/// @b monotone) in @b any order (i.e. second element may be smaller or greater
/// than first).
///
/// @return Pseudo - random in polar coordinates (@a phi, @a theta):
/// - Min(@a phi[0], @a phi[1]) <= @a phi <= Max(@a phi[0], @a phi[1]);
/// - Min(@a theta[0], @a theta[1]) <= @a theta <= Max(@a theta[0], @a theta[1]).
Point2d Rnd::SphrUnif(const double phi[2], const double cos_theta[2])
  {
  Point2d p;
  p[0] = phi[0] + (phi[1] - phi[0]) * DRnd(); // phi
  p[1] = cos_theta[0] + (cos_theta[1] - cos_theta[0]) * DRnd(); // cos(theta)
  p[1] = ACos(p[1]);
  return p;
  } // end function 'SphrUnif()'

///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random point in the cell of the sphere with bilinear probability
/// density.
///
/// Generates a deviate (@a phi, @a theta) distributed over a sphere cell with
/// bilinear density, that is, "the number of points" @a dN is
///
/// <CENTER>
/// @a dN = const * @a f(@a phi, @a theta) * @a sin(theta) * @a d(theta) * @a d(phi)
/// </CENTER>
///
/// where @a f(@a phi,@a theta) is bilinear interpolation between the values at
/// cell vertices.
///
/// @param phi - Array of two values are the cell bounds in azimuth; in any
/// order (i.e. the second element may be smaller).
///
/// @param theta - Array of two values are the cell bounds in polar
/// angle, in any order (i.e. second element may be smaller or greater than
/// first).
///
/// @param cos_theta - Array of two values are the cosine of cell bounds in
/// polar angle, i.e. {@a cos(theta[0]); @a cos(theta[1])}; in any order
/// (i.e. second element may be smaller or greater than first). Using cosine
/// instead of  angles is possible because in [0,PI] cosine is monotone but
/// this works faster.
///
/// @param f - Four values: @a f[i][j] is density at vertex
/// (@a phi[i], @a theta[j]).
///
/// @return Pseudo - random point in polar coordinates (@a phi, @a theta):
/// - Min(@a phi[0], @a phi[1]) <= @a phi <= Max(@a phi[0], @a phi[1]);
/// - Min(@a theta[0], @a theta[1]) <= @a theta <= Max(@a theta[0], @a theta[1]).
Point2d Rnd::SphrBiLin(const double phi[2], const double theta[2],
                       const double cos_theta[2], const double f[2][2])
  {
  double fmax = Max(f[0][0], Max3(f[0][1], f[1][0], f[1][1]));
  Assert(fmax > 0);

  double sp = 1 / (phi[1]   - phi[0]);
  double st = 1 / (theta[1] - theta[0]);

  // we shall use rejection method. First we generate uniform deviate
  // and then "compare" its density with teh target one
  Point2d p;
  for ( ; ; ) // iteration loop of rejection method
    {
    p = SphrUnif(phi, cos_theta); // uniform deviate
    if (fmax <= 0)
      return p;

    // target density at this point:
    double cp = sp * (p[0] - phi[0]);
    double ct = st * (p[1] - theta[0]);
    double fp = (1 - cp) * ((1 - ct) * f[0][0] + ct * f[0][1])
                + cp  * ((1 - ct) * f[1][0] + ct * f[1][1]);

    // accept the point with probability f/fmax
    // else reject it and generate new one
    if (fp >= fmax * DRnd())
      break;
    }
  return p;
  } // end function 'SphrBiLin()'

///////////////////////////////////////////////////////////////////////////////
/// Pseudo-random value in the segment of the sphere with bilinear probability
/// density (function is dependent from one polar coordinate only).
///
/// Generates a deviate @a theta distributed over a sphere cell with bilinear
/// density, that is, "the number of points" @a dN is
///
/// <CENTER>
/// @a dN = const * @a f(theta) * @a sin(theta) * @a d(theta) * @a d(phi)
/// </CENTER>
///
/// where @a f(theta) is linear in @a theta.
///
/// @param theta - Array of two values are the cell bounds in polar
/// angle, in any order (i.e. second element may be smaller or greater than
/// first).
///
/// @param cos_theta - Array of two values are the cosine of cell bounds in
/// polar angle, i.e. {@a cos(theta[0]); @a cos(theta[1])}; in any order
/// (i.e. second element may be smaller or greater than first). Using cosine
/// instead of angles is possible because in [0,PI] cosine is monotone but this
/// works faster.
///
/// @param f - Array of two values: @a f[j] is density at vertex @a theta[j]. cf.
/// density is independent from azimuth.
/// @return Pseudo-random value @a theta:
/// - Min(@a theta[0], @a theta[1]) <= @a theta <= Max(@a theta[0], @a theta[1]).
double Rnd::SphrLin(const double theta[2],
                    const double cos_theta[2], const double f[2])
  {
  double fmax = Max(f[0], f[1]);
  double st = 1 / (theta[1] - theta[0]);

  Assert(fmax > 0);

  // we shall use rejection method. First we generate uniform deviate
  // and then "compare" its density with teh target one
  double t;
  for ( ; ; ) // iteration loop of rejection method
    {
    // uniform deviate
    t = cos_theta[0] + (cos_theta[1] - cos_theta[0]) * DRnd(); // cos(theta)
    t = ACos(t); // theta

    // target density at this point:
    double ct = st * (t - theta[0]);
    double fp = (1 - ct) * f[0] + ct * f[1];

    // accept the point with probability f/fmax
    // else reject it and generate new one
    if (fp >= fmax * DRnd())
      break;
    }
  return t;
  } // end function 'SphrLin()'


//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if all elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
bool Rnd::operator == (const Rnd &u) const
  {
  return (dvalue == u.dvalue) && (m1 == u.m1) && (m2 == u.m2) && (m3 == u.m3) &&
         (gauss_prev == u.gauss_prev) && (has_gauss_prev == u.has_gauss_prev);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if some element (or several alements together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
bool Rnd::operator != (const Rnd &u) const
  {
  return (dvalue != u.dvalue) || (m1 != u.m1) || (m2 != u.m2) || (m3 != u.m3) ||
         (gauss_prev != u.gauss_prev) || (has_gauss_prev != u.has_gauss_prev);
  }

INTEGRA_NAMESPACE_END
#endif // _KSML_RND_HPP_

