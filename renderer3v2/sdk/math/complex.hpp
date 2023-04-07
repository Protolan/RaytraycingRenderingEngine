/// @file
///
/// @brief Complex numbers library.
///
/// Copyright &copy; INTEGRA, Inc., 2005.
///
/// @internal
/// @author Yek - Edward Klyshinsky, SSSE, 05.08.09.
///         Ose - Sergey Ershov, KLBL, 05.10.07.
///         Enk - Nickolay Kirilov, KLBL, 05.10.21.

#ifndef _KLBL_COMPLEX_HPP_
#define _KLBL_COMPLEX_HPP_


#include  <math/math.hpp>


INTEGRA_NAMESPACE_START

/// Complex numbers math.
template <class T>
class TComplex
  {
  public:
    /// @name Constructors.
    //@{
    /// Default constructor.
    inline TComplex();
    /// Initialize constructor.
    inline TComplex(T real, T imag);
    /// Copy constructor.
    inline TComplex(const TComplex<T> &mc);
    //@}

  public:
    /// @name Assignment.
    //@{
    /// Assignment with data conversion, if needed
    inline TComplex &operator =(const TComplex<double> &mc);
    /// Assignment with data conversion, if needed
    inline TComplex &operator =(const TComplex<float> &mc);

    /// Real-valued number assignment.
    inline TComplex &operator =(T d);
    //@}

  public:
    /// @name Comparizon.
    //@{
    /// Equality to complex number.
    inline bool operator ==(const TComplex<T> &mc) const;
    /// Equality to real-valued number.
    inline bool operator ==(T d) const;
    /// Inequality to complex number.
    inline bool operator !=(const TComplex<T> &mc) const;
    /// Inequality to real-valued number.
    inline bool operator !=(T d) const;
    //@}

  public:
    /// @name Basic operations.
    //@{
    /// Modulus.
    inline double Abs() const;
    /// Squared modulus.
    inline double Norm() const;
    /// Squared modulus.
    inline double SqrLen() const;
    //@}

  public:
    /// @name Change sign (negate).
    //@{
    /// Negation.
    inline TComplex operator -() const;
    /// Negation.
    inline void Negate();
    /// Negation.
    inline void SetNegate(const TComplex<T> &mc);
    //@}

  public:
    /// @name Arithmetics with assign.
    //@{
    /// Multiply assign complex number.
    inline TComplex &operator *=(const TComplex<T> &mc);
    /// Multiply assign real-valued number.
    inline TComplex &operator *=(double d);
    /// Divide assign complex number.
    inline TComplex &operator /=(const TComplex<T> &mc);
    /// Divide assign real-valued number.
    inline TComplex &operator /=(double d);
    /// Add assign complex number.
    inline TComplex &operator +=(const TComplex<T> &mc);
    /// Add assign real-valued number.
    inline TComplex &operator +=(double d);
    /// Subtract assign complex number.
    inline TComplex &operator -=(const TComplex<T> &mc);
    /// Subtract assign real-valued number.
    inline TComplex &operator -=(double d);
    //@}

  public:
    /// @name These usually help to accelerate code avoiding copying.
    //@{
    /// Addition.
    inline void SetSum(const TComplex<T> &a, const TComplex<T> &b);
    /// Subtraction.
    inline void SetDifference(const TComplex<T> &a, const TComplex<T> &b);
    /// Multiplication.
    inline void SetProduct(const TComplex<T> &a, const TComplex<T> &b);
    /// Division.
    inline void SetRatio(const TComplex<T> &a, const TComplex<T> &b);
    /// Reciprocal.
    inline void Reciprocal(const TComplex<T> &z);
    /// Reciprocal.
    inline void Reciprocal();
    /// Square.
    inline void Sqr(const TComplex<T> &z);
    /// Square.
    inline void Sqr();
    //@}

  public:
    /// @name Complex conjugate.
    //@{
    /// Conjugate.
    inline void Conj(const TComplex<T> &mc);
    /// Conjugate.
    inline void Conj();
    //@}

  public:
    /// @name Transcendent functions, fast variant.
    //@{
    /// Square root.
    inline void Sqrt(const TComplex<T> &z);
    /// Square root.
    inline void Sqrt();
    //@}

  public:
    /// @name Complex number data.
    //@{
    /// Real part.
    T re;
    /// Imaginary part.
    T im;
    //@}

  public:
    /// @name Commonly used constants.
    //@{
    /// Imaginary 1.
    static const TComplex<T> i;
    /// Complex 0.
    static const TComplex<T> ZERO;
    //@}
  };



//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Created object is initialized with (0, 0).
template <class T>
TComplex<T>::TComplex()
: re(0), im(0)
  {
  // no-op
  }

//////////////////////////////////////////////////////////////////////////////
/// Initialize constructor.
///
/// @param[in] real - Real part.
/// @param[in] imag - Imaginary part.
///
/// Created object is initialized with given real and imaginary values.
template <class T>
TComplex<T>::TComplex(T real, T imag)
: re(real), im(imag)
  {
  // no-op
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param[in] mc - Source object.
///
/// Created object is initialized with given complex number.
template <class T>
TComplex<T>::TComplex(const TComplex<T> &mc)
: re(mc.re), im(mc.im)
  {
  // no-op
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment with data conversion, if needed
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Assign given double type component complex number.
template <class T>
TComplex<T> &TComplex<T>::operator =(const TComplex<double> &mc)
  {
  re = (T)(mc.re);
  im = (T)(mc.im);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment with data conversion, if needed
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Assign given float type component complex number.
template <class T>
TComplex<T> &TComplex<T>::operator =(const TComplex<float> &mc)
  {
  re = (T)(mc.re);
  im = (T)(mc.im);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Real-valued number assignment.
///
/// @param[in] d - Source real-valued number.
/// @return This object reference.
///
/// Equates to a real number (sets imaginary part to 0).
template <class T>
TComplex<T> &TComplex<T>::operator =(T d)
  {
  re = d;
  im = 0;
  return *this;
  }


//////////////////////////////////////////////////////////////////////////////
/// Equality to complex number.
///
/// @param[in] mc - Complex number.
/// @return True if numbers are equal.
///
/// Check for equality.
template <class T>
bool TComplex<T>::operator ==(const TComplex<T> &mc) const
  {
  return re == mc.re && im == mc.im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Equality to real-valued number.
///
/// @param[in] d - Template type value.
/// @return True if numbers are equal.
///
/// Check for equality.
template <class T>
bool TComplex<T>::operator ==(T d) const
  {
  return re == d && im == 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Inequality to complex number.
///
/// @param[in] mc - Complex number.
/// @return True if numbers are not equal.
///
/// Check for inequality.
template <class T>
bool TComplex<T>::operator !=(const TComplex<T> &mc) const
  {
  return (re != mc.re) || (im != mc.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Inequality to real-valued number.
///
/// @param[in] d - Template type value.
/// @return True if numbers are not equal.
///
/// Check for inequality.
template <class T>
bool TComplex<T>::operator !=(T d) const
  {
  return re != d || im != 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Modulus.
///
/// @return Modulus of complex number.
///
/// Get complex number's modulus |*this|.
template <class T>
double TComplex<T>::Abs() const
  {
  return Integra::Sqrt(re * re + im * im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Squared modulus.
///
/// @return Squared modulus of complex number.
///
/// Get complex number's squared modulus |*this|^2 = re^2+im^2.
template <class T>
double TComplex<T>::Norm() const
  {
  return re * re + im * im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Squared modulus.
///
/// @return Squared modulus of complex number.
///
/// Get complex number's squared modulus |*this|^2 = re^2+im^2.
template <class T>
double TComplex<T>::SqrLen() const
  {
  return re * re + im * im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Negation.
///
/// @return Negated complex number.
///
/// Get negated complex number.
template <class T>
TComplex<T> TComplex<T>::operator -() const
  {
  return TComplex<T>(-re, -im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Negation.
///
/// Negate this complex number.
template <class T>
void TComplex<T>::Negate()
  {
  re = -re;
  im = -im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Negation.
///
/// @param[in] mc - Source complex number.
///
/// Assign negated value of given complex number.
template <class T>
void TComplex<T>::SetNegate(const TComplex<T> &mc)
  {
  re = -mc.re;
  im = -mc.im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiply assign complex number.
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Multiply and assign given complex number.
template <class T>
TComplex<T> &TComplex<T>::operator *=(const TComplex<T> &mc)
  {
  T tmp;
  tmp = re * mc.re - im * mc.im;
  im  = im * mc.re + re * mc.im;
  re = tmp;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiply assign real-valued number.
///
/// @param[in] d - Source real-valued number.
/// @return This object reference.
///
/// Multiply and assign given real-valued number.
template <class T>
TComplex<T> &TComplex<T>::operator *=(double d)
  {
  re *= (T)d;
  im *= (T)d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Divide assign complex number.
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Divide and assign given complex number.
template <class T>
TComplex<T> &TComplex<T>::operator /=(const TComplex<T> &mc)
  {
  T tmp, a2 = 1 / (mc.re * mc.re + mc.im * mc.im);

  tmp = (re * mc.re + im * mc.im) * a2;
  im  = (im * mc.re - re * mc.im) * a2;
  re = tmp;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Divide assign real-valued number.
///
/// @param[in] d - Source real-valued number.
/// @return This object reference.
///
/// Divide and assign given real-valued number.
template <class T>
TComplex<T> &TComplex<T>::operator /=(double d)
  {
  d = 1 / d;
  re *= (T)d;
  im *= (T)d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add assign complex number.
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Add and assign given complex number.
template <class T>
TComplex<T> &TComplex<T>::operator +=(const TComplex<T> &mc)
  {
  re += mc.re;
  im += mc.im;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add assign real-valued number.
///
/// @param[in] d - Source real-valued number.
/// @return This object reference.
///
/// Add and assign given real-valued number.
template <class T>
TComplex<T> &TComplex<T>::operator +=(double d)
  {
  re += (T)d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtract assign complex number.
///
/// @param[in] mc - Source complex number.
/// @return This object reference.
///
/// Subtract and assign given complex number.
template <class T>
TComplex<T> &TComplex<T>::operator -=(const TComplex<T> &mc)
  {
  re -= mc.re;
  im -= mc.im;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtract assign real-valued number.
///
/// @param[in] d - Source real-valued number.
/// @return This object reference.
///
/// Subtract and assign given real-valued number.
template <class T>
TComplex<T> &TComplex<T>::operator -=(double d)
  {
  re -= (T)d;
  return *this;
  }



//////////////////////////////////////////////////////////////////////////////
/// Addition.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
///
/// *this = a + b.
template <class T>
void TComplex<T>::SetSum(const TComplex<T> &a, const TComplex<T> &b)
  {
  re = a.re + b.re;
  im = a.im + b.im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
///
/// *this = a - b.
template <class T>
void TComplex<T>::SetDifference(const TComplex<T> &a, const TComplex<T> &b)
  {
  re = a.re - b.re;
  im = a.im - b.im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
///
/// *this = a * b.
template <class T>
void TComplex<T>::SetProduct(const TComplex<T> &a, const TComplex<T> &b)
  {
  T tmp;
  tmp = a.re * b.re - a.im * b.im;
  im  = a.im * b.re + a.re * b.im;
  re = tmp;
  }

//////////////////////////////////////////////////////////////////////////////
/// Division.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
///
/// *this = a / b.
template <class T>
void TComplex<T>::SetRatio(const TComplex<T> &a, const TComplex<T> &b)
  {
  T tmp, a2 = 1 / (b.re * b.re + b.im * b.im);

  tmp = (a.re * b.re + a.im * b.im) * a2;
  im  = (b.re * a.im - a.re * b.im) * a2;
  re = tmp;
  }

//////////////////////////////////////////////////////////////////////////////
/// Reciprocal.
///
/// @param[in] z - Source complex number.
///
/// *this = 1 / z.
template <class T>
void TComplex<T>::Reciprocal(const TComplex<T> &z)
  {
  T a2 = 1 / (z.re * z.re + z.im * z.im);

  re =  z.re * a2;
  im = -z.im * a2;
  }

//////////////////////////////////////////////////////////////////////////////
/// Reciprocal.
///
/// *this = 1 / *this.
template <class T>
void TComplex<T>::Reciprocal()
  {
  T a2 = 1 / (re * re + im * im);

  re *=  a2;
  im *= -a2;
  }

//////////////////////////////////////////////////////////////////////////////
/// Square.
///
/// @param[in] z - Source complex number.
///
/// *this = z^2.
template <class T>
void TComplex<T>::Sqr(const TComplex<T> &z)
  {
  T tmp = z.re * z.re - z.im * z.im;
  im = 2 * z.im * z.re;
  re = tmp;
  }

//////////////////////////////////////////////////////////////////////////////
/// Square.
///
/// *this = (*this)^2.
template <class T>
void TComplex<T>::Sqr()
  {
  T real = re;
  re = re * re - im * im;
  im = 2 * im * real;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conjugate.
///
/// @param[in] z - Source complex number.
///
/// *this = Conjugated z.
template <class T>
void TComplex<T>::Conj(const TComplex<T> &z)
  {
  re = z.re;
  im = -z.im;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conjugate.
///
/// This->im = -this.im.
template <class T>
void TComplex<T>::Conj()
  {
  im = -im;
  }


//////////////////////////////////////////////////////////////////////////////
/// Square root.
///
/// @param[in] z - Source complex number.
///
/// *this = Sqrt(z).
template <class T>
void TComplex<T>::Sqrt(const TComplex<T> &z)
  {
  if (z.im == 0.0)
    {
    if (z.re >= 0)
      {
      re = (T)::Sqrt(z.re);
      im = 0;
      }
    else
      {
      im = (T)::Sqrt(-z.re);
      re = 0;
      }
    return;
    }

  double a = ::Sqrt(0.5 * (fabs(z.re) + z.Abs())),
         b = z.im / (2 * a);

  if (z.re >= 0)
    {
    re = (T)a;
    im = (T)b;

    if (im < 0)
      Negate();
    }
  else
    {
    im = (T)a;
    re = (T)b;
    }
  }  // Sqrt()

//////////////////////////////////////////////////////////////////////////////
/// Square root.
///
/// *this = Sqrt(*this).
template <class T>
void TComplex<T>::Sqrt()
  {
  if (im == 0)
    {
    if (re >= 0)
      {
      re = (T)::Sqrt(re);
      im = 0;
      }
    else
      {
      im = (T)::Sqrt(-re);
      re = 0;
      }
    return;
    }

  double a = ::Sqrt(0.5 * (fabs(re) + Abs())),
         b = im / (2 * a);

  if (re >= 0)
    {
    re = (T)a;
    im = (T)b;

    if (im < 0)
      Negate();
    }
  else
    {
    im = (T)a;
    re = (T)b;
    }
  }  // Sqrt()



//////////////////////////////////////////////////////////////////////////////
// overloaded global functions
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/// Modulus.
///
/// @param[in] z - Source complex number.
/// @return Modulus of complex number.
///
/// Get complex number's modulus.
template <class T>
inline double Abs(const TComplex<T> &z)
  {
  return ::Sqrt(z.re * z.re + z.im * z.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Squared modulus.
///
/// @param[in] z - Source complex number.
/// @return Squared modulus of complex number.
///
/// Get complex number's squared modulus |*this|^2 = re^2+im^2.
template <class T>
inline double Norm(const TComplex<T> &z)
  {
  return (double)(z.re * z.re + z.im * z.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Square.
///
/// @param[in] z - Source complex number.
/// @return Complex number squared.
///
/// Get complex number squared.
template <class T>
inline TComplex<T> Sqr(const TComplex<T> &z)
  {
  return TComplex<T>(z.re * z.re - z.im * z.im, 2 * z.im * z.re);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conjugate.
///
/// @param[in] z - Source complex number.
/// @return Conjugated complex number.
///
/// Get conjugated complex number.
template <class T>
inline TComplex<T> Conj(const TComplex<T> &z)
  {
  return TComplex<T>(z.re, -z.im);
  }


//////////////////////////////////////////////////////////////////////////////
/// Multiplication.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
/// @return Product of complex numbers.
///
/// Get product of complex numbers.
template <class T>
inline TComplex<T> operator *(const TComplex<T> &a, const TComplex<T> &b)
  {
  return TComplex<T>(a.re * b.re - a.im * b.im, a.im * b.re + a.re * b.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication.
///
/// @param[in] a - 1st operand.
/// @param[in] d - 2nd operand.
/// @return Product of complex number and real-valued number.
///
/// Get product of complex number and real-valued number.
template <class T>
inline TComplex<T> operator *(const TComplex<T> &a, double d)
  {
  return TComplex<T>(a.re * (T)d, a.im * (T)d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication.
///
/// @param[in] d - 1st operand.
/// @param[in] b - 2nd operand.
/// @return Product of complex number and real-valued number.
///
/// Get product of complex number and real-valued number.
template <class T>
inline TComplex<T> operator *(double d, const TComplex<T> &b)
  {
  return TComplex<T>(b.re * (T)d, b.im * (T)d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Division.
///
/// @param[in] a - numerator.
/// @param[in] b - denominator.
/// @return Ratio of complex numbers a/b.
///
/// Get ratio of complex numbers.
template <class T>
inline TComplex<T> operator /(const TComplex<T> &a, const TComplex<T> &b)
  {
  T a2 = 1 / (b.re * b.re + b.im * b.im);
  return TComplex<T>((a.re * b.re + a.im * b.im) * a2,
                     (b.re * a.im - a.re * b.im) * a2);
  }

//////////////////////////////////////////////////////////////////////////////
/// Division.
///
/// @param[in] a - numerator.
/// @param[in] d - denominator.
/// @return Ratio of complex number and real-valued number a/d.
///
/// Get ratio of complex number and real-valued number.
template <class T>
inline TComplex<T> operator /(const TComplex<T> &a, double d)
  {
  d = 1 / d;
  return TComplex<T>(a.re * (T)d, a.im * (T)d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Division.
///
/// @param[in] d - numerator.
/// @param[in] b - denominator.
/// @return Ratio of complex number and real-valued number d/b.
///
/// Get ratio of complex number and real-valued number.
template <class T>
inline TComplex<T> operator /(double d, const TComplex<T> &b)
  {
  T a2 = (T)d / (b.re * b.re + b.im * b.im);
  return TComplex<T>(b.re * a2, -b.im * a2);
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition.
///
/// @param[in] a - 1st operand.
/// @param[in] b - 2nd operand.
/// @return Sum of complex numbers.
///
/// Get sum of complex numbers.
template <class T>
inline TComplex<T> operator +(const TComplex<T> &a, const TComplex<T> &b)
  {
  return TComplex<T>(a.re + b.re, a.im + b.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition.
///
/// @param[in] a - 1st operand.
/// @param[in] d - 2nd operand.
/// @return Sum of complex number and real-valued number.
///
/// Get sum of complex number and real-valued number.
template <class T>
inline TComplex<T> operator +(const TComplex<T> &a, double d)
  {
  return TComplex<T>(a.re + (T)d, a.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition.
///
/// @param[in] d - 1st operand.
/// @param[in] b - 2nd operand.
/// @return Sum of complex number and real-valued number.
///
/// Get sum of complex number and real-valued number.
template <class T>
inline TComplex<T> operator +(double d, const TComplex<T> &b)
  {
  return TComplex<T>(b.re + (T)d, b.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction.
///
/// @param[in] a - subtrahend.
/// @param[in] b - subtracter.
/// @return Difference of complex numbers a-b.
///
/// Get difference of complex numbers.
template <class T>
inline TComplex<T> operator -(const TComplex<T> &a, const TComplex<T> &b)
  {
  return TComplex<T>(a.re - b.re, a.im - b.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction.
///
/// @param[in] a - subtrahend.
/// @param[in] d - subtracter.
/// @return Difference of complex number and real-valued number a-d.
///
/// Get difference of complex number and real-valued number.
template <class T>
inline TComplex<T> operator -(const TComplex<T> &a, double d)
  {
  return TComplex<T>(a.re - (T)d, a.im);
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction.
///
/// @param[in] d - subtrahend.
/// @param[in] b - subtracter.
/// @return Difference of complex number and real-valued number d-b.
///
/// Get difference of complex number and real-valued number.
template <class T>
inline TComplex<T> operator -(double d, const TComplex<T> &b)
  {
  return TComplex<T>((T)d - b.re, -b.im);
  }


//////////////////////////////////////////////////////////////////////////////
/// Sine.
///
/// @param[in] z - Source complex number.
/// @return Sine of complex number.
///
/// Get sine of complex number.
template <class T>
inline TComplex<T> Sin(const TComplex<T> &z)
  {
  double exp1 = ::Exp(z.im), exp2 = 1 / exp1;

  return TComplex<T>((T)(sin(z.re) * 0.5 * (exp1 + exp2)),
                     (T)(cos(z.re) * 0.5 * (exp1 - exp2)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Arcsine.
///
/// @param[in] z - Source complex number.
/// @return Arcsine of complex number.
///
/// Get arcsine of complex number.
/// @note When imaginary part of input argument is 0, real part
/// must be in [-1, 1] range; debug version asserts this.
template <class T>
inline TComplex<T> ASin(const TComplex<T> &z)
  {
  if (z.im == 0 && fabs(z.re) <= 1)
    return TComplex<T>(::ASin(z.re), 0);

  double tmp = z.im * z.im;
  double a = ::Sqrt((z.re + 1) * (z.re + 1) + tmp),
         b = ::Sqrt((z.re - 1) * (z.re - 1) + tmp);

  tmp = 0.5 * (a + b);
  tmp = ::Log(tmp + ::Sqrt(tmp * tmp - 1));

  return TComplex<T>((T)(::ASin(0.5 * (a - b))),
                     (T)(z.im > 0 ? tmp : -tmp));
  }

//////////////////////////////////////////////////////////////////////////////
/// Cosine.
///
/// @param[in] z - Source complex number.
/// @return Cosine of complex number.
///
/// Get cosine of complex number.
template <class T>
TComplex<T> Cos(const TComplex<T> &z)
  {
  double exp1 = ::Exp(z.im), exp2 = 1 / exp1;

  return TComplex<T>((T)( cos(z.re) * 0.5 * (exp1 + exp2)),
                     (T)(-sin(z.re) * 0.5 * (exp1 - exp2)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Arccosine.
///
/// @param[in] z - Source complex number.
/// @return Arccosine of complex number.
///
/// Get arccosine of complex number.
/// @note When imaginary part of input argument is 0, real part
/// must be in [-1, 1] range; debug version asserts this.
template <class T>
TComplex<T> ACos(const TComplex<T> &z)
  {
  if (z.im == 0 && fabs(z.re) <= 1)
    return TComplex<T>(::ACos(z.re), 0);

  double tmp = z.im * z.im;
  double a = ::Sqrt((z.re + 1) * (z.re + 1) + tmp),
         b = ::Sqrt((z.re - 1) * (z.re - 1) + tmp);

  tmp = 0.5 * (a + b);
  tmp = ::Log(tmp + ::Sqrt(tmp * tmp - 1));

  return TComplex<T>((T)(::ACos(Clipped(0.5 * (a - b), -1.0, 1.0))),
                     (T)(z.im > 0 ? -tmp : tmp));
  }

//////////////////////////////////////////////////////////////////////////////
/// Tangent.
///
/// @param[in] z - Source complex number.
/// @return Tangent of complex number.
///
/// Get tangent of complex number.
template <class T>
TComplex<T> Tan(const TComplex<T> &z)
  {
  double a = z.re + z.re, b = z.im + z.im;
  double exp1 = ::Exp(b), exp2 = 1 / exp1;
  double c = 1 / (cos(a) + (exp1 + exp2) * 0.5);

  return TComplex<T>((T)(sin(a) * c), (T)((exp1 - exp2) * 0.5 * c));
  }

//////////////////////////////////////////////////////////////////////////////
/// Arctangent.
///
/// @param[in] z - Source complex number.
/// @return Arcangent of complex number.
///
/// Get arctangent of complex number.
template <class T>
TComplex<T> ATan(const TComplex<T> &z)
  {
  double a = z.re * z.re + z.im * z.im + 1;
  double atan_arg = (z.re + z.re) / (2 - a),
         pi_arg = PI * 0.5 * (::Sign(a - 2) + 1) * ::Sign(z.re),
         log_arg = (a + z.im + z.im) / (a - z.im - z.im);

  return TComplex<T>((T)(0.5 * (atan(atan_arg) + pi_arg)),
                     (T)(0.25 * ::Log(log_arg)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Logarithm.
///
/// @param[in] z - Source complex number.
/// @return Natural (base @e e) logarithm of complex number.
///
/// Get logarithm of complex number.
template <class T>
TComplex<T> Log(const TComplex<T> &z)
  {
  double r = z.Abs();
  TComplex<T> c((T)Log(r), (T)::ACos(z.re / r));

  if (z.im < 0)
    c.im = 2 * PI - c.im;

  return c;
  }

//////////////////////////////////////////////////////////////////////////////
/// Exponent.
///
/// @param[in] z - Source complex number.
/// @return Exponent of complex number.
///
/// Get exponent of complex number.
template <class T>
TComplex<T> Exp(const TComplex<T> &z)
  {
  double r = ::Exp(z.re);

  return TComplex<T>((T)(r * cos(z.im)), (T)(r * sin(z.im)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Square root.
///
/// @param[in] z - Source complex number.
/// @return Square root of complex number.
///
/// Get square root of complex number.
template <class T>
TComplex<T> Sqrt(const TComplex<T> &z)
  {
  if (z.im == 0.0)
    {
    if (z.re >= 0)
      return TComplex<T>((T)Integra::Sqrt(z.re), 0);
    else
      return TComplex<T>(0, (T)Integra::Sqrt(-z.re));
    }

  double a = Integra::Sqrt(0.5 * (fabs(z.re) + z.Abs())),
         b = z.im / (2 * a);

  if (z.re >= 0)
    {
    if (b < 0)
      return TComplex<T>((T)-a, (T)-b);
    else
      return TComplex<T>((T)a, (T)b);
    }
  else
    return TComplex<T>((T)b, (T)a);
  }

//////////////////////////////////////////////////////////////////////////////
/// Power.
///
/// @param[in] z - Source complex number.
/// @param[in] n - Power.
/// @return Power of complex number.
///
/// Get power of complex number (real-valued power version).
template <class T>
inline TComplex<T> Pow(const TComplex<T> &z, double n)
  {
  double r = z.Abs(), rn = ::Pow(r, n), t;

  t = Integra::ACos(z.re / r);
  if (z.im < 0)
    t = 2 * PI - t;

  return TComplex<T>((T)(rn * cos(n * t)), (T)(rn * sin(n * t)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Power.
///
/// @param[in] z - Source complex number.
/// @param[in] n - Power.
/// @return Power of complex number.
///
/// Get power of complex number (integer power version).
template <class T>
inline TComplex<T> Pow(const TComplex<T> &z, int n)
  {
  TComplex<T> out(z);

  switch (n)
    {
    case 3:
      out *= z;
      // fall-thru
    case 2:
      out *= z;
      // fall-thru
    case 1:
      break;
    case 0:
      out.re = 1;
      out.im = 0;
      break;
    default:
      out = Pow(z, (double)n);
      break;
    }

  return out;
  }

/// Predefined instance of complex number with elements of double type.
typedef TComplex<double> Complexd;
/// Predefined instance of complex number with elements of float type.
typedef TComplex<float>  Complexf;


INTEGRA_NAMESPACE_END

#endif
