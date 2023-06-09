/// @file
///
/// @brief Niederreiter Random number generator class header file
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004.
///
/// @internal
/// @author Rkd - Kirill Dmitriev

#ifndef _RMAQ_NRND_HPP_
#define _RMAQ_NRND_HPP_

#include <base/time.hpp>

#include "rnd.hpp"

INTEGRA_NAMESPACE_START

// ----------------------------------------------------
//                   Niederreiter class
// ----------------------------------------------------

/// Class for Niederreiter Random number generator
class Niederreiter : public Rnd
  {
  /// Number of bits actually used in sample
  static const int NIEDERREITER_NBTS;

  /// Number of dimensions of Niederreiter sequence (any reasonable number)
  static const int NIEDERREITER_NDMS = 26;

  /// 1 / 2^63
  static const double NIEDERREITER_INVV;

  public:

    /// @name Constructor, destructor.
    //@{

    /// Default constructor
    Niederreiter(INT64 sample = 1, unsigned iseed = Time::Get());

    /// Destructor
    ~Niederreiter();

    //@}

    /// @name Services.
    //@{

    /// Current quasi-random vector component - value in [0, 1).
    virtual inline double DRnd();
    /// Random number in segment [0, 1).
    virtual inline double DRndP();

    /// Pointer to current quasi-random vector component.
    virtual inline double *DRndPtr();
    /// ReNormalize
    virtual inline void ReNorm(double &prob, double pp, double pp_prev);

    /// Go to next quasi-random vector.
    inline Niederreiter &operator ++();

    /// Current quasi-random vector index.
    inline const INT64 &Sample() const;

    /// Set current quasi-random vector index
    inline Niederreiter &SetSample(const INT64 &sample);

    /// Current quasi-random vector dimension index.
    inline int Dimension() const;

    /// Set quasi-random vector dimention index
    inline bool SetDimension(int dindex);

    //@}

  private:

    /// Generate double value, corresponding to current 'sample' and 'dindex'
    void GenerateDValue();

    /// Current quasi-random vector index
    INT64 sample;
    /// Current quasi-random vector dimension index
    int dindex;
    /// Current quasi-random value in [0, 1) interval
    double dvalue;
    /// Cache used to accelerate computations
    INT64 cache[NIEDERREITER_NDMS][2];

  /// Generate table of irreducable polynomials
  static void GeneratePolynomials();
  /// Generate cj values
  static void GenerateCJ();
  /// Irreducable polynomials (used for creation of cj values)
  static int *polynomials;
  /// cj values used to generate quasi-random values (shared by all generators)
  static INT64 **cj;
  /// Number of the instances
  static int instances;
  };

///////////////////////////////////////////////////////////////////////////////
/// Current quasi-random vector component - value in [0, 1).
///
/// Method returns current quasi-random vector component and immediately
/// generates the next one to be returned at the next DRnd() call. Note that
/// if no more components are left (internal constant NIEDERREITER_NDMS = 26
/// is exceeded), than pseudo-random value, generated by base Rnd class is
/// returned.
///
/// @return Quasi-random value from [0, 1) interval
double Niederreiter::DRnd()
  {
  double result = dvalue;
  GenerateDValue();
  return result;
  }

///////////////////////////////////////////////////////////////////////////////
/// Random number in segment [0, 1).
///
/// For Niederreiter this function is equal to *DRndPtr() - does not generate
/// new random number. Is necessary in The RDISurfAttr::ChooseSurfEvent,
/// where new random number was generated for Rnd and was not for Niederreiter.
///
/// @return Pseudo-random number in segment [0, 1).
double Niederreiter::DRndP()
  {
  return dvalue;
  }  // END OF DRnd FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Pointer to current quasi-random vector component.
///
/// Method returns pointer to current quasi-random vector component. this
/// provides a possibility to change this value, which can be useful to sample
/// several descrete events by single quasi-random value.
/// @if INTEGRA_INTERNAL
/// (See rmaq05ds.Rkd for more details)
/// @endif
///
/// This is highly recommended that changes to pointed value still leave it
/// in [0, 1) interval.
///
/// @return Pointer to value in [0, 1) interval.
double *Niederreiter::DRndPtr()
  {
  Assert(0.0 <= dvalue && dvalue < 1.0);
  return &dvalue;
  }

///////////////////////////////////////////////////////////////////////////////
/// Re-normalize.
/// @param prob    Initial probability.
/// @param pp      Current probability.
/// @param pp_prev Previous probability.
void Niederreiter::ReNorm(double &prob, double pp, double pp_prev)
  {
  prob = (prob - pp_prev) / (pp - pp_prev);
  dvalue = prob;
  }

///////////////////////////////////////////////////////////////////////////////
/// Go to next quasi-random vector.
///
/// @return Reference to this class with new quasi-random vector
Niederreiter &Niederreiter::operator ++()
  {
  ++sample;
  dindex = -1;
  GenerateDValue();
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Current quasi-random vector index.
///
/// @return Constant reference to current quasi-random vector index.
const INT64 &Niederreiter::Sample() const
  {
  return sample;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set current quasi-random vector index.
///
/// @param sample0 - New value for the current quasi-random vector index
/// @return Reference to this class with new current quasi-random vector index.
Niederreiter &Niederreiter::SetSample(const INT64 &sample0)
  {
  sample = sample0;
  dindex = -1;
  GenerateDValue();
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Current quasi-random vector dimention index.
///
/// @return Current quasi-random vector dimention index.
int Niederreiter::Dimension() const
  {
  return dindex;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set current quasi-random vector dimention index.
///
/// @param dindex0- New value of current quasi-random vector dimention index.
/// @return true - if specified index less or equal of the predefined internal
/// constant NIEDERREITER_NDMS = 26, false - otherwise (method Rnd::DRnd()
/// will be used instead of Niederreiter::DRnd()).
bool Niederreiter::SetDimension(int dindex0)
  {
  dindex = dindex0 - 1;
  GenerateDValue();
  return dindex < NIEDERREITER_NDMS;
  }

INTEGRA_NAMESPACE_END
#endif
