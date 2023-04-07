/// @file
///
/// @brief Basic mathematics functions
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///         Rkd - Kirill Dmitriev

#ifndef _KLBC_MATH_HPP_
#define _KLBC_MATH_HPP_

// Use system matematical functions
#include <math.h>
#include <float.h>

INTEGRA_NAMESPACE_START

class Serializer;

#ifdef EPSILON
#undef EPSILON
#endif

#ifdef EPSILON_DIV_10
#undef EPSILON_DIV_10  // Use EPSILON / 10.0
#endif

#ifdef EPSILON_POW_2
#undef EPSILON_POW_2
#endif

#ifdef PI
#undef PI
#endif

#ifdef REV_PI
#undef REV_PI
#endif

#ifdef PI2
#undef PI2
#endif

#ifdef HALFPI
#undef HALFPI
#endif

#ifdef SQRT2
#undef SQRT2
#endif

#ifdef SQRT3
#undef SQRT3
#endif

#ifdef REV_BYTE
#undef REV_BYTE
#endif

// -----------------------------------------------------------------
// Global constants
// -----------------------------------------------------------------

// Type independent tolerance

/// Accuracy for the @b geometric calculations
#define EPSILON                0.0001

/// Accuracy for the @b geometric calculations to the power 2
#define EPSILON_POW_2          0.00000001

// Mathematical constants

/// PI value
#define PI                     3.1415926535897932

/// Reverse value of PI
#define REV_PI                 0.3183098861837907   //  1.0 / PI

/// Doubled PI value
#define PI2                    6.2831853071795865   //  2 * PI
#define HALFPI                 1.5707963267948966192313216916398
/// Square root of 2
#define SQRT2                  1.4142135623730950

/// Square root of 3
#define SQRT3                  1.7320508075688773

// Subsidary constants

/// Reverse value of 255
#define REV_BYTE               0.0039215686274510   //  1 / 255


// -----------------------------------------------------------------
// Global functions
// -----------------------------------------------------------------

/// Get number of digits in integer.
inline int NumLen(INT64 a);

/// Round to the nearest integer.
inline int Round(double a);
/// Round to the given precision.
inline double PRound(double a, int prec);

/// Conversion from degrees to radians
inline double Rad(double a);

/// Conversion from radians to degrees
inline double Deg(double a);

/// Sign of value
inline int Sign(double a);

/// Square root of value
inline double Sqrt(double a);

/// Cube root value (for any domain)
inline double Cbrt(double a);

/// The natural logarithm of value
inline double Log(double a);

/// The base 10 logarithm of value
inline double Log10(double a);

/// The base 2 logarithm of value
inline double Log2(double a);

/// Value x to the power y
inline double Pow(double x, double y);

/// Sine.
inline double Sin(double v);
/// Cosine.
inline double Cos(double v);
/// Tangent.
inline double Tan(double v);

/// Arcsine.
inline double ASin(double x);
/// Arccosine
inline double ACos(double x);
/// Arctangent.
inline double ATan(double x);
/// Arctangent of y/x in the range –PI to PI radians.
inline double ATan2(double y, double x);

/// Exponent.
inline double Exp(double x);


// -----------------------------------------------------------------
// Global template functions
// -----------------------------------------------------------------

/// Absolute value for different types T
template <class T>
inline T Abs(const T &a);

/// Square a value of type T
template <class T>
inline T Sqr(const T &a);

/// Cube of a value of type T
template <class T>
inline T Cube(const T &a);

/// Clip a value of given variable by given range for different types T
template <class T>
inline void Clip(T &v, const double a_min, const double a_max);

/// Clip a value of given variable to given minimum.
template <class T>
inline bool ClipLower(T &v, double a_min);

/// Clip a value of given variable to given maximum.
template <class T>
inline bool ClipHigher(T &v, double a_max);

/// Clip value to given range.
template <class T>
inline T ValToRange(const T &v, const T &vmin, const T &vmax);

/// Clip a value of given variable by given range for different types T
template <class T>
inline T Clipped(const T &v, const double a_min, const double a_max);

/// Check that value is in given range.
template <class T>
inline bool InRange(const T &v, const double a_min, const double a_max);

/// Check that value is a valid float.
template <class T>
inline bool FloatIsOK(const T val)
  {
  return (_finite(val) != 0);
  }

/// Comparison of float / double values with a tolerance.
template<class T>
class Math
  {

  public:

    /// @name Constants
    //@{
    /// Default tolerance (type dependent)
    static const double TOLERANCE;
    /// Minimal positive values for float and double types (rounded up)
    static const double MIN_VALUE;
    /// Maximum values for float and double types (rounded down)
    static const double MAX_VALUE;
    /// Minimum positive floating point number x such that 1.0 + x != 1.0
    static const double EPS_VALUE;
    //@}


    /// @name Comparisons of the number.
    //@{
    /// Comparison of the number with given tolerance.
    inline static bool AboutZero(const double v, const double tolerance);
    /// Comparison of the number with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const double v);
    /// Comparison of the number with predefined tolerance ::EPSILON.
    inline static bool NearZero(const double v);
    //@}

    /// @name Comparisons of the numbers.
    //@{
    /// Comparison of the numbers with given tolerance.
    inline static bool AboutEqual(const double v1, const double v2,
                       const double tolerance);
    /// Comparison of the numbers with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const double v1, const double v2);
    /// Comparison of the numbers with predefined tolerance ::EPSILON.
    inline static bool NearEqual(const double v1, const double v2);
    //@}

    /// @name Determine sign of value with some tolerance around zero.
    //@{
    /// Determine value sign with specified tolerance.
    inline static int SignAbout(const double v, const double tolerance);
    /// Determine value sign with some predefined tolerance Math<T>::TOLERANCE.
    inline static int SignAbout(const double v);
    /// Determine value sign with some predefined tolerance ::EPSILON.
    inline static int SignNear(const double v);
    //@}

  };  // class Math<T>

/// Simple bounding box in 1D space.
template <class T>
class BBox1
  {

  public:

    /// Minimal value
    T vmin,
    /// Maximal value
      vmax;

  public:

    /// @name Constructors
    //@{
    /// Default constructor, without initialization.
    inline BBox1();

    /// Constructor by the one value.
    inline BBox1(const double point);

    /// Constructor by the two values.
    inline BBox1(const double point1, const double point2);

    //@}

    /// @name Assignments
    //@{

    /// Assignment operator.
    inline BBox1<T> & operator = (const BBox1<T> &box);

    //@}

    /// @name Comparisons
    //@{

    /// Comparison of the two bounding boxes for equality.
    inline bool operator == (const BBox1<T> &box) const;

    /// Comparison of the two bounding boxes for non-equality.
    inline bool operator != (const BBox1<T> &box) const;

    //@}

    /// @name Test for validaty
    //@{

    /// Check that box is not empty.
    inline bool NotEmpty() const;

    /// Check that box contains the one point only.
    inline bool IsDot() const;

    //@}

    /// @name Inclusion to box and intersection with box
    //@{

    /// Check that bounding box includes the given point.
    inline bool Includes(const double point) const;

    /// Check that bounding box includes the given box.
    inline bool Includes(const BBox1<T> &box) const;

    /// Check that bounding box intersects the given box.
    inline bool Intersects(const BBox1<T> &box) const;

    //@}

    /// @name Serialization
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing bounding boxes.
    //@{

    /// Serialize the box (no default is provided).
    void Serialize(Serializer &inout, const char *tag);

    /// Lower level serialization.
    void Value(Serializer &inout);

    //@}

    /// @name Miscelaneous methods.
    //@{

    /// Extension of the box for the inclusion of the given point.
    inline void Include(const double point);

    /// Extension of the box for the inclusion of the given box.
    inline void Include(const BBox1<T> &box);

    /// Intersection with given box.
    inline void Intersect(const BBox1<T> &box);

    /// Translation of the box by given vector.
    inline void Translate(const double vct);

    /// Creation of the box by the translation of this box.
    inline BBox1<T> Translated(const double vct) const;

    /// Diagonal of the box.
    inline double Diag() const;

    /// Center of the box.
    inline double Center() const;

    //@}

  };  // class BBox1<T>

// -----------------------------------------------------------------
// Predefined instances of template classes
// -----------------------------------------------------------------

/// Predefined instance with elements of double type.
typedef Math<double> MathD;
/// Predefined instance with elements of float type.
typedef Math<float>  MathF;
/// Predefined instance with elements of int type.
typedef Math<int>  MathI;
/// Predefined instance with elements of WORD type.
typedef Math<WORD>  MathW;

/// Predefined instance of 1D bounding box with elements of int type.
typedef BBox1<int>    BBox1i;
/// Predefined instance of 1D bounding box with elements of float type.
typedef BBox1<float>  BBox1f;
/// Predefined instance of 1D bounding box with elements of double type.
typedef BBox1<double> BBox1d;


//////////////////////////////////////////////////////////////////////////////
/// Get number of digits in integer.
/// @param[in] a - Integer number.
/// @return Number of digits in @b a. Negative sign, if any, is not counted
///         (absolute value is considered).
inline int NumLen(INT64 a)
  {
  a = abs(a);
  if (a == 0)
    return 1;
  return (int)Log10((double)a) + 1;
  }

///////////////////////////////////////////////////////////////////////////////
/// Round to the nearest integer.
/// @param[in] a - Double number.
/// @return Nearest integer number.
int Round(double a)
  {
  return (int)((a) > 0.0 ? (a) + 0.5 : (a) - 0.5);
  }

///////////////////////////////////////////////////////////////////////////////
/// Round to the given precision.
/// @param[in] a - Double number.
/// @param[in] prec - Precision - number of sensible digits.
/// @return Double number rounded with given precision.
double PRound(double a, int prec)
  {
  if (a == 0)
    return 0;
  double p = prec - 1 - floor(Log10(fabs(a)));
  double p10 = Pow(10, p);
  return floor(a * p10 + 0.5 ) / p10;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion from degrees to radians.
/// @param a - Value in degrees.
/// @return Value in radians.
double Rad(double a)
  {
  return a * PI / 180.0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion from radians to degrees.
/// @param a - Value in radians.
/// @return Value in degrees.
double Deg(double a)
  {
  return a * 180.0 / PI;
  }

///////////////////////////////////////////////////////////////////////////////
/// Sign of value.
/// @param a - Value
/// @return - Sign of the given value:
/// - +1 if a > 0;
/// - -1 if a < 0;
/// -  0 if a = 0.
int Sign(double a)
  {
  return (a > 0.0) ? 1 : ((a == 0.0) ? 0 : -1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Square root of value.
/// @param a - Non-negative value.
/// @note Value of parameter may not be less than 0. Debug version asserts it.
/// @return Square root of value.
double Sqrt(double a)
  {
  Assert(a >= 0);  
  // To avoid -1.#IND000000000000 in case small negative values due 
  // restricted accuracy of floating point calculations
  a = (a > 0.0) ? a : 0.0;
  return sqrt(a);
  }


///////////////////////////////////////////////////////////////////////////////
/// Cube root of value for any domain.
/// @param a - Arbitrary value.
/// @return Cube root of value.
double Cbrt(double a)
  {
  static const double THIRD = 1.0 / 3.0;
  if (a > 0)
    return Pow(a, THIRD);
  if (a < 0)
    return -Pow(-a, THIRD);
  return 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// The natural logarithm of value.
/// @param a - Positive value.
/// @note Value of parameter must be positive and less than some predefined
/// maximal value MathD::MAX_VALUE.
/// Debug version asserts it.
/// @return Value of natural logarithm.
double Log(double a)
  {
  Assert((0.0 < a) && (a < MathD::MAX_VALUE));  // domain area
  return log(a);
  }

///////////////////////////////////////////////////////////////////////////////
/// The base 10 logarithm of value.
/// @param a - Positive value.
/// @note Value of parameter must be positive and less than some predefined
/// maximal value MathD::MAX_VALUE.
/// Debug version asserts it.
/// @return Value of logarithm with base 10.
double Log10(double a)
  {
  Assert((0.0 < a) && (a < MathD::MAX_VALUE));  // domain area
  return log10(a);
  }

///////////////////////////////////////////////////////////////////////////////
/// The base 2 logarithm of value.
/// @param a - Positive value.
/// @note Value of parameter must be positive and less than some predefined
/// maximal value MathD::MAX_VALUE.
/// Debug version asserts it.
/// @return Value of logarithm with base 2.
double Log2(double a)
  {
  static const double c = 1 / log(2.0);
  Assert((0.0 < a) && (a < MathD::MAX_VALUE));  // domain area
  return c * log(a);
  }

///////////////////////////////////////////////////////////////////////////////
/// Value @a x to the power @a y.
/// @note Value @a x must be less than some predefined maximal value
/// MathF::MAX_VALUE. Absolute value of @a y must be less than some predefined
/// maximal value MathF::MAX_VALUE. Debug version asserts it.
/// @return Value @a x to the power @a y.
double Pow(double x, double y)
  {
  // "Domain area is restricted to positive x!" (previous note)
  // Who invented such restriction for mathematics? See help for pow
  // function, it does not contain such restriction. 0 and negative
  // values of x are allowed for some values of y.
  Assert(x < MathF::MAX_VALUE);
  Assert(Abs(y) < MathF::MAX_VALUE);
  return pow(x, y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sine of the specified value.
/// @param v - Arbitrary value.
/// @return Sine of given value.
double Sin(double v)
  {
  return sin(v);
  }

///////////////////////////////////////////////////////////////////////////////
/// Cosine of the specified value
/// @param v - Arbitrary value.
/// @return Cosine of given value.
double Cos(double v)
  {
  return cos(v);
  }

///////////////////////////////////////////////////////////////////////////////
/// Tangent of the specified value.
/// @param v - Arbitrary value.
/// @return Tangent of given value.
double Tan(double v)
  {
  return tan(v);
  }

///////////////////////////////////////////////////////////////////////////////
/// Arcsine.
///
/// @param[in] x - Value in range [-1; 1].
/// @note Value of parameter may not be less than -1 or more than +1. Debug
/// version asserts it.
/// @return The asin function returns the arcsine of x in the range –PI/2 to PI/2 radians.
///
double ASin(double x)
  {
  Assert((-1.0 <= x) && (x <= 1.0));
  return asin(x);
  }  // ASin()

///////////////////////////////////////////////////////////////////////////////
/// Arccosine.
///
/// @param[in] x - Value in range [-1; 1].
/// @note Value of parameter may not be less than -1 or more than +1. Debug
/// version asserts it.
/// @return The acos function returns the arccosine of x in the range 0 to PI radians.
///
double ACos(double x)
  {
  Assert((-1.0 <= x) && (x <= 1.0));
  return acos(x);
  }  // Acos()

///////////////////////////////////////////////////////////////////////////////
/// Arctangent.
/// @param x - Arbitrary value.
/// @return The atan function returns the arctangent of x in the range of –PI/2 to PI/2 radians.
double ATan(double x)
  {
  return atan(x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Arctangent of y/x in the range –PI to PI radians.
/// @param x, y - Arbitrary values.
/// @return Arctangent of y/x in the range –PI to PI radians.
double ATan2(double y, double x)
  {
  return atan2(y, x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Exponent.
/// @param x - Arbitrary value.
/// @return Exponent of given value.
double Exp(double x)
  {
  return exp(x);
  }

// -----------------------------------------------------------------
// Global template functions
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Absolute value for different types T.
///
/// @note Type T is numerical: int, float, double.
/// @param a - Arbitrary value.
/// @return Absolute value:
/// - a if a >= 0;
/// - -a if a < 0.
template <class T>
T Abs(const T &a)
  {
  return ((a) >= (T)0) ? (a) : -(a);
  }

///////////////////////////////////////////////////////////////////////////////
/// Square a value of type T
///
/// @note Type T is numerical: int, float, double.
/// @param a - Arbitrary value.
/// @return Square of value.
template <class T>
T Sqr(const T &a)
  {
  return a * a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Cube of a value of type T
///
/// @note Type T is numerical: int, float, double.
/// @param a - Arbitrary value.
/// @return Cube of a value.
template <class T>
T Cube(const T &a)
  {
  return a * a * a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip a value of given variable by given range for different types T
///
/// If parameter @a v of the function less than minimal value @a a_min then
/// parameter @a v is replaced by the @a a_min, if parameter @a v of the
/// function more than maximal value @a a_max then parameter @a v is replaced
/// by the @a a_max, parameter @a v is not changed in other cases.
///
/// @note Type T is numerical: int, float, double.
/// @param[in, out] v - Arbitrary value (clipped on output).
/// @param[in] a_min - Minimal value of range.
/// @param[in] a_max - Maximal value of range.
/// @return None (first parameter is affected)
template <class T>
void Clip(T &v, const double a_min, const double a_max)
  {
  if (v < (T)a_min)
    v = (T)a_min;
  if (v > (T)a_max)
    v = (T)a_max;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip a value of given variable to given minimum.
/// @param[in, out] v - Arbitrary value (clipped on output).
/// @param[in] a_min - Minimal value of range.
/// @return True, if clipped, false otherwise.
template <class T>
bool ClipLower(T &v, double a_min)
  {
  if (v < (T)a_min)
    {
    v = (T)a_min;
    return true;
    }
  return false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip a value of given variable to given maximum.
/// @param[in, out] v - Arbitrary value (clipped on output).
/// @param[in] a_max  Maximal value of range.
/// @return True, if clipped, false otherwise.
template <class T>
bool ClipHigher(T &v, double a_max)
  {
  if (v > (T)a_max)
    {
    v = (T)a_max;
    return true;
    }
  return false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip value to given range.
///
/// If parameter @a v of the function less than minimal value @a a_min then
/// parameter @a v is replaced by the @a a_min, if parameter @a v of the
/// function more than maximal value @a a_max then parameter @a v is replaced
/// by the @a a_max, parameter @a v is not changed in other cases.
///
/// @note Type T should provide comparision methods.
/// @param[in, out] v Arbitrary value (clipped on output).
/// @param[in] a_min Minimal value of range.
/// @param[in] a_max Maximal value of range.
/// @return None (first parameter is affected)
template <class T>
inline T ValToRange(const T &v, const T &vmin, const T &vmax)
  {
  if (v < vmin)
    return vmin;
  if (v > vmax)
    return vmax;
  return v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip a value of given variable by given range for different types T
///
/// @note Type T is numerical: int, float, double.
/// @param[in] v - Arbitrary value.
/// @param[in] a_min - Minimal value of range.
/// @param[in] a_max - Maximal value of range.
/// @return Clipped value:
/// - minimal value of the range - if parameter @a v of the function less than
/// minimal value @a a_min,
/// - maximal value of the range - if parameter @a v of the function more than
/// maximal value @a a_max,
/// - parameter @a v - in other cases.
template <class T>
T Clipped(const T &v, const double a_min, const double a_max)
  {
  if (v < (T)a_min)
    return (T)a_min;
  if (v > (T)a_max)
    return (T)a_max;
  return v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that value is in given range.
///
/// @note Type T is numerical: int, float, double.
/// @param[in] v - Arbitrary value.
/// @param[in] a_min - Minimal value of range.
/// @param[in] a_max - Maximal value of range.
/// @return true - if first parameter of the function 'v' satisfies the
/// following condition: a_min <= v <= a_max, false - otherwise.
template <class T>
bool InRange(const T &v, const double a_min, const double a_max)
  {
  return (v >= a_min && v <= a_max);
  }

//////////////////////////////////////////////////////////////////////////////
/// Round the number - delete decimals lower then given level.
/// @param[in, out] v The number.
/// @param level A given level, for example - 0.00001.
template <class T>
void Round(T &v, double level)
  {
  Assert(level != 0);
  if (v >= 0)
    v = T(floor(v / level + 0.5) * level);
  else
    v = T(ceil(v / level - 0.5) * level);
  }

// -----------------------------------------------------------------
// class Math methods
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the number with given tolerance.
///
/// @param v - Given number.
/// @param tolerance - Given tolerance.
/// @return true - if absolute value of given number is less or equal than
/// given tolerance, false - otherwise.
template <class T>
bool Math<T>::AboutZero(const double v, const double tolerance)
  {
  return ((-tolerance <= v) && (v <= tolerance));
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the number with predefined tolerance Math<T>::TOLERANCE.
///
/// Value of the predefined tolerance is dependent from T type.
/// @param v - Given number.
/// @return true - if absolute value of the given number is less of equal than
/// given Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool Math<T>::AboutZero(const double v)
  {
  return AboutZero(v, TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the number with predefined tolerance ::EPSILON.
///
/// Value of the ::EPSILON is independent from T type.
/// @param v - Given number.
/// @return true - if absolute value of the given number is less or equal than
/// ::EPSILON, false - otherwise.
template <class T>
bool Math<T>::NearZero(const double v)
  {
  return AboutZero(v, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the numbers with given tolerance.
///
/// @param v1 - First number.
/// @param v2 - Second number.
/// @param tolerance - Given tolerance.
/// @return true -if absolute value of the difference between given numbers
/// is less or equal than tolerance, false otherwise.
template <class T>
bool Math<T>::AboutEqual(const double v1, const double v2,
                            const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

/////////////////////////////////////////////////////////////////////////////
/// Comparison of the numbers with predefined tolerance Math<T>::TOLERANCE.
///
/// Value of the predefined tolerance is dependent from T type.
/// @param v1 - First number.
/// @param v2 - Second number.
/// @return true -if absolute value of the difference between given numbers
/// is less or equal than Math<T>::TOLERANCE, false otherwise.
template <class T>
bool Math<T>::AboutEqual(const double v1, const double v2)
  {
  return AboutZero(v1 - v2, TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the numbers with predefined tolerance ::EPSILON.
///
/// Value of the predefined tolerance ::EPSILON is independent from T type.
/// @param v1 - First number.
/// @param v2 - Second number.
/// @return true -if absolute value of the difference between given numbers
/// is less or equal than ::EPSILON, false - otherwise.
template <class T>
bool Math<T>::NearEqual(const double v1, const double v2)
  {
  return AboutZero(v1 - v2, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Determine value sign with specified tolerance.
///
/// @param v - Arbitrary value.
/// @param tolerance - Value of the tolerance.
/// @return Sign of the value:
/// - +1 - if value >= tolerance;
/// - -1 - if value <= -tolerance;
/// -  0 - otherwise.
template <class T>
int Math<T>::SignAbout(const double v, const double tolerance)
  {
  if (v >= tolerance)
    return 1;
  if (v <= -tolerance)
    return -1;
  return 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Determine value sign with some predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Arbitrary value.
/// @return Sign of the value:
/// - +1 - if value >= Math<T>::TOLERANCE;
/// - -1 - if value <= -(Math<T>::TOLERANCE);
/// -  0 - otherwise.
template <class T>
int Math<T>::SignAbout(const double v)
  {
  return SignAbout(v, TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Determine value sign with some predefined tolerance ::EPSILON.
///
/// Value of the predefined ::EPSILON is independent from T type.
/// @param v - Arbitrary value.
/// @return Sign of the value:
/// - +1 - if value >= ::EPSILON;
/// - -1 - if value <= -::EPSILON;
/// -  0 - otherwise.
template <class T>
int Math<T>::SignNear(const double v)
  {
  return SignAbout(v, EPSILON);
  }

// -----------------------------------------------------------------
// class BBox1 methods
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, without initialization.
template <class T>
BBox1<T>::BBox1()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by the one value.
///
/// Minimal @a vmin and maximal @a vmax boundaries of a box are initialized by
/// the same point (scalar value).
/// @param point - 1D point cf. value
template <class T>
BBox1<T>::BBox1(const double point)
  {
  vmin = vmax = (T)point;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by the two values.
///
/// Minimal @a vmin and maximal @a vmax boundaries of a box are initialized by
/// the given points.
/// @param point1 - 1D point (value) defines minimal boundary
/// @param point2 - 1D point (value) defines maximal boundary
template <class T>
BBox1<T>::BBox1(const double point1, const double point2)
  {
  vmin = (T)point1;
  vmax = (T)point2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Parameters of this bounding box (upper @a vmax and lower @a vmin boundary)
/// are replaced by the parameters of the given box.
///
/// @param box - Given box.
/// @return Reference to this box (updated).
template <class T>
BBox1<T> & BBox1<T>::operator = (const BBox1<T> &box)
  {
  vmin = box.vmin;
  vmax = box.vmax; return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for equality.
///
/// This box is equal to given box if boundaries (points @a vmin, @a vmax) of
/// this box are equal to boundaries of the given box.
///
/// @param box - Given box.
/// @return true - if this box is equal to given box, false - otherwise.
template <class T>
bool BBox1<T>::operator == (const BBox1<T> &box) const
  {
  return (vmin == box.vmin) && (vmax == box.vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for non-equality.
///
/// This box is not equal to given box if some boundary of this box (point
/// @a vmin or @a vmax or both points simultaneously) is not equal to
/// corresponding boundary of the given box.
///
/// @param box - Given box.
/// @return true - if this box unequal to given box, false - otherwise.
template <class T>
bool BBox1<T>::operator != (const BBox1<T> &box) const
  {
  return (vmin != box.vmin) || (vmax != box.vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box is not empty.
///
/// Bounding box (segment on number line) is not empty if there are points
/// (even if one point) inside box. In this case maximal boundary @a vmax of a
/// box is not less than minimal boundary @a vmin.
///
/// @return true - if box is not empty, false - otherwise.
template <class T>
bool BBox1<T>::NotEmpty() const
  {
  return (vmin <= vmax) ? true : false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box contains the one point only.
///
/// Bounding box (segment on number line) contains the one point only if
/// maximal boundary @a vmax of the box is equal to the minimal boundary
/// @a vmin of the box.
/// In this case segment on number line transforms to dot: @a vmin = @a vmax.
///
/// @return true if bounding box is dot, false - otherwise.
template <class T>
bool BBox1<T>::IsDot() const
  {
  return vmin == vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given point.
///
/// This bounding box (segment on number line) includes the given point
/// (numerical value in most cases) if given point is inside of this box cf.
/// given point less or equal than maximal boundary @a vmax @b and more or
/// equal than minimal boundary @a vmin of this box: @a vmin <= @a x <= @a vmax.
///
/// @param point - Given point (double value).
/// @return true - if given point is inside of this box, false - otherwise.
template <class T>
bool BBox1<T>::Includes(const double point) const
  {
  return (vmin <= point && point <= vmax) ? true : false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given box.
///
/// This bounding box (segment on number line) includes the given box if
/// both boundaries (maximal @a vmax and minimal @a vmin) of the given box are
/// inside of this box (see method Includes() for the point).
///
/// @param box - Given box (segment on number line).
/// @return true - if given box is inside of this box, false - otherwise.
template <class T>
bool BBox1<T>::Includes(const BBox1<T> &box) const
  {
  return (vmin <= box.vmin && box.vmax <= vmax) ? true : false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box intersects the given box.
///
/// This bounding box (segment on number line) intersects the given box if
/// both boxes (this and given) contain the common points. In opposite case the
/// minimal boundary (@a vmin) of the one box more than maximal boundary
/// (@a vmax)of the other box.
///
/// @param box - Given box (segment on number line).
/// @return true - if this box intersects given box, false - otherwise.
template <class T>
bool BBox1<T>::Intersects(const BBox1<T> &box) const
  {
  return (vmin <= box.vmax && box.vmin <= vmax) ? true : false;
  }

///////////////////////////////////////////////////////////////////////////////
/// Extension of the box for the inclusion of the given point.
///
/// This bounding box (segment on number line) is not changed if given point
/// is inside of this box. Otherwise this box is extended to include
/// the given point. Resulted bounding box is minimal box which includes the
/// initial this box and given point.
///
/// @param point - Given point (double value).
/// @return none (this box is affected).
template <class T>
void BBox1<T>::Include(const double point)
  {
  if (point < vmin)
    vmin = (T)point;
  if (vmax < point)
    vmax = (T)point;
  }

///////////////////////////////////////////////////////////////////////////////
/// Extension of the box for the inclusion of the given box.
///
/// If given box has external points with respect to this box then this box
/// will be extended to include the given box fully. Resulted bounding box is
/// minimal box which includes the initial this box and given box.
///
/// @param box - Given box (segment on number line).
/// @return none (this box is affected).
template <class T>
void BBox1<T>::Include(const BBox1<T> &box)
  {
  if (vmin > box.vmin)
    vmin = box.vmin;
  if (vmax < box.vmax)
    vmax = box.vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Intersection with given box.
///
/// If this bounding box (segment on number line) is intersected by the given
/// box (see method Intersects()) then result of the intersection (bounding box
/// cf. segment on number line) is placed into this box. If this bounding box
/// is not intersected with given box then this box is not changed.
///
/// @param box - Given box (segment on number line).
/// @return none (this box is affected).
template <class T>
void BBox1<T>::Intersect(const BBox1<T> &box)
  {
  if (vmin < box.vmin)
    vmin = box.vmin;
  if (vmax > box.vmax)
    vmax = box.vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Translation of the box by given vector.
///
/// This bounding box (segment on number line) is modified by the addition of
/// the given vector (double value in this case) to the minimal (@a vmin) and
/// maximal (@a vmax) boundaries of this box. In fact it is shift of the
/// segment on number line, where size of the shift is given vector (double
/// value).
///
/// @note Parameter of this method (double value) is named as vector by analogy
/// with 2D and 3D spaces.
///
/// @param vct - Given vector (double value).
/// @return none (this box is affected).
template <class T>
void BBox1<T>::Translate(const double vct)
  {
  vmin = (T)(vmin + vct);
  vmax = (T)(vmax + vct);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the box by the translation of this box.
///
/// New bounding box (range in 1D space) is created  by the addition of the
/// given vector (double value) to the minimal (@a vmin) and maximal (@a vmax)
/// boundaries of this box. In fact it is shift of the segment on number line,
/// where size of the shift is given vector (double value).
///
/// @note Parameter of this method (double value) is named as vector by analogy
/// with 2D and 3D spaces.
///
/// @param vct Given vector (double value)
/// @return Created bounding box.
template <class T>
BBox1<T> BBox1<T>::Translated(const double vct) const
  {
  return BBox1<T>(vmin + vct, vmax + vct);
  }

///////////////////////////////////////////////////////////////////////////////
/// Diagonal of the box.
///
/// Diagonal of the bounding box (segment on number line) is length of the box
/// cf. distance between maximal (@a vmax) and minimal (@a vmin) boundaries of
/// the box.
///
/// @return Length of this box.
template <class T>
double BBox1<T>::Diag() const
  {
  return (double)vmax - (double)vmin;
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the box.
///
/// Center of the bounding box (segment on number line) is arithmetic mean of
/// the minimal (@a vmin) and maximal (@a vmax) boundaries of the box.
///
/// @return Value of the box center.
template <class T>
double BBox1<T>::Center() const
  {
  return ((double)vmax + (double)vmin) / 2;
  }

INTEGRA_NAMESPACE_END
#endif

