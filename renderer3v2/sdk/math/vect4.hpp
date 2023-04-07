/// @file
///
/// @brief Definition of template class for 4D vectors.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Rkd - Kirill Dmitriev

#ifndef _KLBC_VECT4_HPP_
#define _KLBC_VECT4_HPP_

#include "math.hpp"
#include "vect3.hpp"

INTEGRA_NAMESPACE_START

//  forward references
template <class T> class BVect4;
template <class T> class TMatrix4;

/// @brief Class BVect4 is set of the four elements of arbitrary type.
/// Arithmetic operations are not provided by this class.
template <class T>
class BVect4
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector element
    T x,
    /// Second vector element
      y,
    /// Third vector element
      z,
    /// Fourth vector element
      w;

  public:
    /// @name Constructors:
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline BVect4();
    /// Constructor from the given scalar value.
    inline BVect4(T a);
    /// Constructor from the 3D vector and given value.
    inline BVect4(const BVect3<T> &v, T a);
    /// Constructor from the data of the arbitrary type.
    inline BVect4(const T &x0, const T &y0, const T &z0, const T &w0);
    /// Copy constructor.
    inline BVect4(const BVect4<T> &u);
    //@}

    /// @name Element access.
    //@{
    /// @brief Access to i-th element of the vector: x (i = 0), y (i = 1),
    /// z (i = 2), w (i = 3).
    inline T & operator [](int i);
    /// @brief Access to i-th element of the vector: x (i = 0), y (i = 1),
    /// z (i = 2), w (i = 3).
    inline const T &operator [](int i) const;
    //@}

    /// @name Assignments.
    //@{
    /// Assignment operator.
    inline BVect4<T> & operator = (const BVect4<T> &u);
    //@}

    /// @name  Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const BVect4<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const BVect4<T> &u) const;
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Conversion of the vector to the array of four elements.
    inline operator T *();
    /// Conversion of the vector to the array of four elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of four elements as 4D vector.
    inline static const BVect4<T> &Cast(const T *u);
    /// Treatment of the array of four elements as 4D vector.
    inline static BVect4<T> &Cast(T *u);
    //@}

    /// @name Other methods.
    //@{
    /// Get 3D vector part of this vector.
    inline const BVect3<T> &GetVect3() const;
    /// Set 3D vector part of this vector.
    inline void SetVect3(const BVect3<T> &v);
    //@}

    /// Value of maximal element of the vector.
    inline T MaxElement() const;

    /// @name Number of the vector elements.
    //@{
    /// Number of the vector elements.
    inline static int NComponents();
    //@}
  };  //  class BVect4<T>


/// 4D vector of arbitrary type.
template <class T>
class TVect4
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector element
    T x,
    /// Second vector element
      y,
    /// Third vector element
      z,
    /// Fourth vector element
      w;

  public:
    /// @name Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TVect4();
    /// Constructor from given value.
    inline TVect4(const double v);
    /// Constructor from given elements.
    inline TVect4(const double x0, const double y0,
                  const double z0, const double w0 = 1);
    /// Constructor from the two 3D points.
    inline TVect4(const TPoint3<T> &from, const TPoint3<T> &to);
    /// Copy constructor.
    inline TVect4(const TVect4<T> &u);
    /// Initialization of the 4D vector by the 3D vector and given value.
    inline TVect4(const TVect3<T> &u, T w0 = 1);
    //@}

    /// @name Element access.
    //@{
    /// @brief Access to the i-th element of the vector: x (i = 0), y (i = 1),
    /// z (i = 2), w (i = 3).
    inline T & operator [](int i);
    /// @brief Access to the i-th element of the vector: x (i = 0), y (i = 1),
    /// z (i = 2), w (i = 3).
    inline T   operator [](int i) const;
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const TVect4<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const TVect4<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool LessOrEqual(const TVect4<T> &u) const;
    /// Lexical comparison.
    inline bool operator < (const TVect4<T> &u) const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given vector to this vector.
    inline TVect4<T> & operator = (const TVect4<T> &u);

    // Vector-to-vector operators.
    /// Element-by-element addition of the given vector to this vector.
    inline TVect4<T> & operator += (const TVect4<T> &u);
    /// Element-by-element subtraction of the given vector from this vector.
    inline TVect4<T> & operator -= (const TVect4<T> &u);
    /// Element-by-element multiplication of this vector by given vector.
    inline TVect4<T> & operator *= (const TVect4<T> &u);
    /// Element-by-element division of this vector by given vector.
    inline TVect4<T> & operator /= (const TVect4<T> &u);

    // Vector-to-scalar operators.
    /// Addition of the given scalar to elements of this vector.
    inline TVect4<T> & operator += (const double d);
    /// Subtraction of the given scalar from elements of this vector.
    inline TVect4<T> & operator -= (const double d);
    /// Multiplication of this vector by given scalar.
    inline TVect4<T> & operator *= (const double d);
    /// Vector by scalar division.
    inline TVect4<T> & operator /= (const double d);
    /// Addition with a weight.
    inline TVect4<T> &AddWithWeight(const TVect4<T> &u, const double w);
    //@}

    /// @name Unary operators.
    //@{
    /// Negation of this vector.
    inline void Negate();
    /// Creation of the opposite vector.
    inline TVect4<T> operator -() const;
    //@}

    /// @name Binary operators.
    //@{
    // Vector-to-vector operators.
    /// @brief Vector creation by the element-by-element addition of the given
    /// vector to this vector.
    inline TVect4<T> operator + (const TVect4<T> &u) const;
    /// @brief Vector creation by the element-by-element subtraction a given
    /// vector from this vector.
    inline TVect4<T> operator - (const TVect4<T> &u) const;
    /// @brief Vector creation by the element-by-element multiplication a given
    /// vector to this vector.
    inline TVect4<T> operator * (const TVect4<T> &u) const;
    /// @brief Vector creation by the element-by-element division of this
    /// vector by given vector.
    inline TVect4<T> operator / (const TVect4<T> &u) const;

    // Vector-to-scalar operators.
    /// @brief Vector creation by the addition a scalar value to elements of
    /// this vector.
    inline TVect4<T> operator + (const double d) const;
    /// @brief Vector creation by the subtraction a scalar value from elements
    /// of this vector.
    inline TVect4<T> operator - (const double d) const;
    /// @brief Vector creation by the multiplication a scalar value to elements
    /// of this vector.
    inline TVect4<T> operator * (const double d) const;
    /// @brief Vector creation by the division elements of this vector by the
    /// scalar value.
    inline TVect4<T> operator / (const double d) const;

    // Vector-to-matrix operators.
    /// Multiplication of this vector by the given matrix.
    TVect4<T> operator * (const TMatrix4<T> &v) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 4D vectors.
    //@{
    /// Serialize the vector using given default.
    void Serialize(Serializer &inout, const char *tag, const TVect4<T> &dval = 0);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Other methods.
    //@{
    /// Clipping elements of the vector.
    inline void Clip(const double vmin, const double vmax);
    /// Clipping elements lower than given minimum.
    inline void ClipLower(double vmin);
    /// Clipping elements higher than given maximum.
    inline void ClipHigher(double vmax);
    /// Clip vector elements to given range.
    inline TVect4<T> ValToRange(const T vmin, const T vmax) const;
    /// Check of the elements of this vector - are they in given range.
    inline bool InRange(double vmin, double vmax) const;
    /// Index of maximal element (by absolute value).
    inline int MaxElementIndex() const;
    /// Absolute value of maximal element (by absolute value) of the vector.
    inline T MaxElement() const;
    /// Set of the vector elements to the given values.
    inline void Set(double x0, double y0, double z0, double w0);
    /// Set of the vector elements to the same value.
    inline void Set(double a);
    /// Sum of the elements of the vector.
    inline T Sum() const;
    /// Projection of this vector to given vector.
    inline TVect4<T> Project(const TVect4<T> &u) const;
    /// Normalize vector.
    inline TVect4<T> &Normalize();
    /// Test of vector normalization.
    inline bool Normalized() const;
    /// Length of the vector and normalization of the vector.
    inline double ModNormalize();
    /// Calculate the sum of the vector components and divide to this sum.
    inline double SumNormalize();
    /// @brief Calculate value of the maximal element of the vector and normalize
    /// the vector to the maximal value.
    inline double MaxNormalize();
    /// Vector orientation to given vector.
    inline void Orient(const TVect4<T> &v);
    /// Check that all elements of vector are valid floats.
    bool IsOK() const;
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Conversion of the vector to array of the four elements.
    inline operator T *();
    /// Conversion of the vector to array of the four elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of the four elements as a constant vector.
    inline static const TVect4<T> &Cast(const T *u);
    /// Treatment of the array of the four elements as a vector.
    inline static TVect4<T> &Cast(T *u);
    //@}

    /// @name Other methods.
    //@{
    /// Get 3D vector part of this vector.
    inline const TVect3<T> &GetVect3() const;
    /// Set 3D vector part of this vector.
    inline void SetVect3(const TVect3<T> &v);
    //@}

    /// @name Number of the elements of the vector.
    //@{
    /// Number of the elements of the vector.
    static int NComponents();
    //@}
  };  // class TVect4<T>

// -----------------------------------------------------------------
// Related template functions
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// @brief Vector creation by the multiplication a scalar value to elements of
/// given vector.
///
/// Function creates a new 4D vector. Elements of a new vector are the products
/// of the corresponding elements of the given vector and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 4D vector.
/// @return New 4D vector (product of vector and scalar)
template <class T>
inline TVect4<T> operator * (const double d, const TVect4<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Dot product of the 3D components of the given vectors.
///
/// Dot product of the two 4D vectors is calculated as Dot product of the 3D
/// components (first three coordinates) of these vectors (see  function
/// DotProd() for the 3D vectors)). Fourth elements of the vectors are not used.
///
/// @param a - First 4D vector - multiplier.
/// @param b - Second 4D vector - multiplier.
template <class T>
inline double DotProd(const TVect4<T> &a, const TVect4<T> &b)
  {
  return (double)a.x * (double)b.x +
         (double)a.y * (double)b.y +
         (double)a.z * (double)b.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Dot product of the all four elements of the vectors.
///
/// Dot product is sum of the products of the corresponding elements of the
/// vectors - multipliers.
///
/// @param a - First vector multiplier
/// @param b - Second vector multiplier
template <class T>
inline double DotProd4(const TVect4<T> &a, const TVect4<T> &b)
  {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cross product.
template <class T>
inline TVect4<T> CrossProd(const TVect4<T> &a, const TVect4<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Squared length of the given vector in 3D space.
///
/// Length of the vector is Dot product (see function
/// DotProd(const TVect4<T> &a, const TVect4<T> &b)), where multipliers are
/// equal to the given vector. Dot product is calculated in 3D sense cf. fourth
/// coordinate of the vector is not used.
///
/// @param u - Given 4D vector.
/// @return Length of vector in second power.
template <class T>
inline double SqrLength(const TVect4<T> &u)
  {
  return DotProd(u, u);
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of the vector in 3D space.
///
/// Length of the vector is square root of the Dot product (see function
/// DotProd(const TVect4<T> &a, const TVect4<T> &b)), where multipliers are
/// equal to the given vector. Dot product is calculated in 3D sense cf.
/// fourth coordinate of the vector is not used.
///
/// @param u - 4D vector.
/// @return Length of vector.
template <class T>
inline double Length(const TVect4<T> &u)
  {
  return Sqrt(SqrLength(u));
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cosine of angle between two given vectors.
template <class T>
inline double Cos(const TVect4<T> &a, const TVect4<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// @brief Sine of angle between two given vectors.
template <class T>
inline double Sin(const TVect4<T> &a, const TVect4<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with integer elements.
///
/// @param u - 4D vector with elements of the arbitrary type.
/// @return Vector with integer elements.
template <class T>
inline TVect4<int> ConvI(const TVect4<T> &u)
  {
  return TVect4<int>(u.x, u.y, u.z, u.w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with float elements.
///
/// @param u - 4D vector with elements of the arbitrary type.
/// @return Vector with float elements.
template <class T>
inline TVect4<float> ConvF(const TVect4<T> &u)
  {
  return TVect4<float>(u.x, u.y, u.z, u.w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with double elements.
///
/// @param u - 4D vector with elements of the arbitrary type.
/// @return Vector with double elements.
template <class T>
inline TVect4<double> ConvD(const TVect4<T> &u)
  {
  return TVect4<double>(u.x, u.y, u.z, u.w);
  }

/// Comparison of 4D vectors with a tolerance.
template <class T>
class Math4 : public Math<T>
  {
  public:
    /// @name Comparison of the vector with tolerance.
    //@{
    /// Comparison of the vector with given tolerance.
    inline static bool AboutZero(const TVect4<T> &v, const double tolerance);
    /// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TVect4<T> &v);
    //@}

    /// @name Comparison of the vectors(points) with tolerance.
    //@{
    /// Comparison of the vectors with given tolerance.
    inline static bool AboutEqual(const TVect4<T> &v1, const TVect4<T> &v2, const double tolerance);
    /// Comparison of the vectors with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TVect4<T> &v1, const TVect4<T> &v2);
    //@}

  };  // class MAth4<T>

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 4D vector with elements of BYTE type.
typedef BVect4<BYTE>      Set4b;
/// Predefined instance of 4D vector with elements of short type.
//typedef BVect4<short>     Set4s;
/// Predefined instance of 4D vector with elements of int type.
typedef BVect4<int>       Set4i;
/// Predefined instance of 4D vector with elements of unsigned type.
typedef BVect4<unsigned>  Set4u;
/// Predefined instance of 4D vector with elements of float type.
typedef BVect4<float>     Set4f;

/// Predefined instance of 4D vector with elements of short type.
//typedef TVect4<short>     Vect4s;
/// Predefined instance of 4D vector with elements of int type.
typedef TVect4<int>       Vect4i;
/// Predefined instance of 4D vector with elements of float type.
typedef TVect4<float>     Vect4f;
/// Predefined instance of 4D vector with elements of double type.
typedef TVect4<double>    Vect4d;

/// Predefined instance of 4D vector with elements of WORD type.
typedef TVect4<WORD>      Color4w;
/// Predefined instance of 4D vector with elements of short type.
//typedef TVect4<short>     Color4s;
/// Predefined instance of 4D vector with elements of int type.
typedef TVect4<int>       Color4i;
/// Predefined instance of 4D vector with elements of float type.
typedef TVect4<float>     Color4f;
/// Predefined instance of 4D vector with elements of float type.
typedef TVect4<float>     RGBAColor;
/// Predefined instance of 4D vector with elements of double type.
typedef TVect4<double>    Color4d;

// ---------------------------------------------------------------------------
//                        IMPLEMENTATION PART
// ---------------------------------------------------------------------------

/////////////////////////// BVect4<T> class //////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
BVect4<T>::BVect4()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given scalar value.
template <class T>
BVect4<T>::BVect4(T a)
  {
  x = a;
  y = a;
  z = a;
  w = a;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the 3D vector and given value.
///
/// The first tree elements of this vector are initialized by the elements of
/// the given 3D vector, fourth component is defined by the given value.
///
/// @param v - Given 3D vector
/// @param a - Given value
template <class T>
BVect4<T>::BVect4(const BVect3<T> &v, T a)
  {
  x = v.x;
  y = v.y;
  z = v.z;
  w = a;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given elements.
///
/// Elements of the vector are initialized by the parameters of the method
/// according to its order.
/// @param x0 - Value for the first vector element.
/// @param y0 - Value for the second vector element.
/// @param z0 - Value for the third vector element.
/// @param w0 - Value for the fourth vector element.
template <class T>
BVect4<T>::BVect4(const T &x0, const T &y0, const T &z0, const T &w0)
  {
  x = x0;
  y = y0;
  z = z0;
  w = w0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - Source 4D vector.
template <class T>
BVect4<T>::BVect4(const BVect4<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  w = u.w;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2),
/// w (i = 3).
///
/// @note i must be in range 0..3. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to i-th element of this vector.
template <class T>
T &BVect4<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 4));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2),
/// w (i = 3).
///
/// @note i must be in range 0..3. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Constant reference to the i-th element.
template <class T>
const T &BVect4<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 4));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Number of the vector elements.
///
/// @return 4.
template <class T>
int BVect4<T>::NComponents()
  {
  return 4;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get 3D vector part of this vector.
///
/// New 3D vector is not created (data is not changed). First three coordinates
/// of this 4D vector are interpreted as coordinates of the 3D vector.
///
/// @return Constant reference to 3D vector.
template <class T>
const BVect3<T> &BVect4<T>::GetVect3() const
  {
  return BVect3<T>::Cast(&x);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set 3D vector part of this vector.
/// @param v Constant reference to 3D vector.
template <class T>
void BVect4<T>::SetVect3(const BVect3<T> &v)
  {
  x = v.x;
  y = v.y;
  z = v.z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 4D vector.
/// @return Reference to this vector (updated).
template <class T>
BVect4<T> &BVect4<T>::operator = (const BVect4<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  w = u.w;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 4D vector.
/// @return true - if all elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool BVect4<T>::operator == (const BVect4<T> &u) const
  {
  return (x == u.x) && (y == u.y) && (z == u.z) && (w == u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 4D vector.
/// @return true - if some element (or several alements together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool BVect4<T>::operator != (const BVect4<T> &u) const
  {
  return (x != u.x) || (y != u.y) || (z != u.z) || (w != u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of four elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Pointer to the first element of array.
template <class T>
BVect4<T>::operator T *()
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of four elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Pointer to the first element of array.
template <class T>
BVect4<T>::operator const T *() const
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of four elements as 4D vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Constant reference to the 4D vector.
template <class T>
const BVect4<T> &BVect4<T>::Cast(const T *u)
  {
  return *(const BVect4<T> *)u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of four elements as 4D vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Reference to the 4D vector.
template <class T>
BVect4<T> &BVect4<T>::Cast(T *u)
  {
  return *(BVect4<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Value of maximal element of the vector.
///
/// @return Value of maximal element of the vector.
template <class T>
T BVect4<T>::MaxElement() const
  {
  return Max(Max(x, y), Max(z, w));
  }



/////////////////////////// TVect4<T> class //////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
TVect4<T>::TVect4()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from given value.
///
/// The vector elements are initialized by given value.
/// @param v - Given value.
template <class T>
TVect4<T>::TVect4(const double v)
  {
  x = y = z = w = (T)v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from given elements.
///
/// The elements of the vector are initialized by the method parameters
/// according to its order.
///
/// @param x0 - Value of the first vector element.
/// @param y0 - Value of the second vector element.
/// @param z0 - Value of the third vector element.
/// @param w0 - Value of the fourth vector element.
template <class T>
TVect4<T>::TVect4(const double x0, const double y0,
                  const double z0, const double w0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  w = (T)w0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the two 3D points.
///
/// Vector is initialized by the 3D vector (see constructor of the
/// initialization of the 4D vector by the 3D vector) created by the given 3D
/// points. The first point is begin of the 3D vector, the second point is end
/// of the 3D vector.
///
/// @param from - First 3D point.
/// @param to   - Second 3D point.
template <class T>
TVect4<T>::TVect4(const TPoint3<T> &from, const TPoint3<T> &to)
  {
  *this = to - from;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - Source 4D vector.
template <class T>
TVect4<T>::TVect4(const TVect4<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  w = u.w;
  }

///////////////////////////////////////////////////////////////////////////////
/// Initialization of the 4D vector by the 3D vector and given value.
///
/// The first tree elements of this vector are initialized by the elements of
/// the given 3D vector, fourth element is initialized by the given value.
///
/// @param u  - Given 3D vector
/// @param w0 - Value of the fourth element of this vector.
template <class T>
TVect4<T>::TVect4(const TVect3<T> &u, T w0)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  w = w0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2),
/// w (i = 3).
///
/// @note i must be in range 0..3. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to the element of this vector.
template <class T>
T &TVect4<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 4));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2),
/// w (i = 3).
///
/// @note i must be in range 0..3. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to the element of this vector.
template <class T>
T TVect4<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 4));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get 3D vector part of this vector.
///
/// New 3D vector is not created (data is not changed). First three coordinates
/// of this 4D vector are interpreted as coordinates of the 3D vector.
///
/// @return Constant reference to 3D vector.
template <class T>
const TVect3<T> &TVect4<T>::GetVect3() const
  {
  return TVect3<T>::Cast(&x);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set 3D vector part of this vector.
/// @param v Constant reference to 3D vector.
template <class T>
void TVect4<T>::SetVect3(const TVect3<T> &v)
  {
  x = v.x;
  y = v.y;
  z = v.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Number of the elements of the vector.
///
/// @return 4
template <class T>
int TVect4<T>::NComponents()
  {
  return 4;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 4D vector.
/// @return true - if all four elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool TVect4<T>::operator == (const TVect4<T> &u) const
  {
  return (x == u.x) && (y == u.y) && (z == u.z) && (w == u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 4D vector.
/// @return true - if some element (or several elements together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool TVect4<T>::operator != (const TVect4<T> &u) const
  {
  return (x != u.x) || (y != u.y) || (z != u.z) || (w != u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 4D vector.
/// @return true - if all four elements of this vector are less or equal than
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool TVect4<T>::LessOrEqual(const TVect4<T> &u) const
  {
  return (x <= u.x) && (y <= u.y) && (z <= u.z) && (w <= u.w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Lexical comparison.
///
/// Comparison of two vectors. This method is different from other comparison
/// methods as it forms a lexicographical vectors order during sorting. It
/// means that it returns true immediately if first component of this vector is
/// less than first component of the given vector, or false if it is greater. If
/// first elements are equal, it compares second one and so on.
///
/// @param u - Given 4D vector.
/// @return true - if this vector less than given vector in lexical sense,
/// false - otherwise.
template <class T>
bool TVect4<T>::operator < (const TVect4<T> &u) const
  {
  if (x < u.x)
    return true;
  if (x > u.x)
    return false;
  if (y < u.y)
    return true;
  if (y > u.y)
    return false;
  if (z < u.z)
    return true;
  if (z > u.z)
    return false;
  return w < u.w;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment of the given vector to this vector.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 4D vector.
/// @return Reference to this vector (updated).
template <class T>
TVect4<T> &TVect4<T>::operator = (const TVect4<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  w = u.w;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element addition of the given vector to this vector.
///
/// Elements of this vector are replaced by the sums of corresponding elements
/// of this vector and given vector.
/// @param u - Given 4D vector.
/// @return Reference to this vector (the sum of two vectors).
template <class T>
TVect4<T> &TVect4<T>::operator += (const TVect4<T> &u)
  {
  x += u.x;
  y += u.y;
  z += u.z;
  w += u.w;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element subtraction of the given vector from this vector.
///
/// Elements of this vector are replaced by the differences between
/// corresponding elements of this vector and given vector.
///
/// @param u - Given 4D vector.
/// @return Reference to this vector (the difference between two vectors).
template <class T>
TVect4<T> &TVect4<T>::operator -= (const TVect4<T> &u)
  {
  x -= u.x;
  y -= u.y;
  z -= u.z;
  w -= u.w;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element multiplication of this vector by given vector.
///
/// Elements of this vector are replaced by the products of corresponding
/// elements of this vector and given vector.
/// @param u - Given 4D vector.
/// @return Reference to this vector (product of two vectors).
template <class T>
TVect4<T> &TVect4<T>::operator *= (const TVect4<T> &u)
  {
  x *= u.x;
  y *= u.y;
  z *= u.z;
  w *= u.w;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element division of this vector by given vector.
///
/// Elements of this vector are replaced by the results of division of
/// corresponding elements of this vector by elements of the given vector.
/// @note Given vector may not have zero elements. Debug version ASSERTs
/// about this.
///
/// @param u - Given 4D vector.
/// @return Reference to this vector (result of division of two vectors).
template <class T>
TVect4<T> &TVect4<T>::operator /= (const TVect4<T> &u)
  {
  Assert((u.x != 0) && (u.y != 0) && (u.z != 0) && (u.w != 0));
  x /= u.x;
  y /= u.y;
  z /= u.z;
  w /= u.w;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of the given scalar to elements of this vector.
///
/// Elements of this vector are replaced by the sums of these elements with
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated).
template <class T>
TVect4<T> &TVect4<T>::operator += (const double d)
  {
  x = (T)(x + d);
  y = (T)(y + d);
  z = (T)(z + d);
  w = (T)(w + d);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given scalar from elements of this vector.
///
/// Elements of this vector are replaced by the differences between these
/// elements and given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated).
template <class T>
TVect4<T> &TVect4<T>::operator -= (const double d)
  {
  x = (T)(x - d);
  y = (T)(y - d);
  z = (T)(z - d);
  w = (T)(w - d);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this vector by given scalar.
///
/// Elements of this vector are replaced by the products of these elements and
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated)
template <class T>
TVect4<T> &TVect4<T>::operator *= (const double d)
  {
  x = (T)(x * d);
  y = (T)(y * d);
  z = (T)(z * d);
  w = (T)(w * d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector by scalar division.
///
/// Elements of this vector are replaced by the results of division of
/// these elements by given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated).
template <class T>
TVect4<T> &TVect4<T>::operator /= (const double d)
  {
  Assert(d != 0.0);
  x = (T)(x / d);
  y = (T)(y / d);
  z = (T)(z / d);
  w = (T)(w / d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition with a weight.
///
/// Elements of this vector are replaced by the sums of these elements with
/// products of the given vector elements and weight value.
///
/// @param u - Given 4D vector.
/// @param wt - Weight value.
/// @return  Reference to this vector (updated).
template <class T>
TVect4<T> &TVect4<T>::AddWithWeight(const TVect4<T> &u, const double wt)
  {
  x = (T)(x + u.x * wt);
  y = (T)(y + u.y * wt);
  z = (T)(z + u.z * wt);
  w = (T)(w + u.w * wt);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Negation of this vector.
///
/// The signs of the all elements of this vector are changed to opposite.
///
/// @return None (this vector is affected).
template <class T>
void TVect4<T>::Negate()
  {
  x = -x;
  y = -y;
  z = -z;
  w = -w;
  }

//////////////////////////////////////////////////////////////////////////////
/// Creation of the opposite vector.
///
/// Method creates a new 4D vector. Elements of a new vector are negative with
/// respect to the elements of this vector.
///
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator -() const
  {
  return TVect4<T>(-x, -y, -z, -w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element addition of the given vector to
/// this vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given vector.
/// @param u - Given 4D vector.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator + (const TVect4<T> &u) const
  {
  return TVect4<T>(x + u.x, y + u.y, z + u.z, w + u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element subtraction a given vector from
/// this vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given vector.
/// @param u - Given 4D vector.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator - (const TVect4<T> &u) const
  {
  return TVect4<T>(x - u.x, y - u.y, z - u.z, w - u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element multiplication a given vector to
/// this vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given vector.
/// @param u - Given 4D vector.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator * (const TVect4<T> &u) const
  {
  return TVect4<T>(x * u.x, y * u.y, z * u.z, w * u.w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element division of this vector by given
/// vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the elements of the
/// given vector.
/// @note Given vector may not have zero elements. Debug version ASSERTs
/// about this.
/// @param u - Given 4D vector.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator / (const TVect4<T> &u) const
  {
  Assert((u.x != 0) && (u.y != 0) && (u.z != 0) && (u.w != 0));
  return TVect4<T>(x / u.x, y / u.y, z / u.z, w / u.w);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the addition a scalar value to elements of this vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator + (const double d) const
  {
  return TVect4<T>(x + d, y + d, z + d, w + d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the subtraction a scalar value from elements of  this
/// vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator - (const double d) const
  {
  return TVect4<T>(x - d, y - d, z - d, w - d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the multiplication a scalar value to elements of  this
/// vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator * (const double d) const
  {
  return TVect4<T>(x * d, y * d, z * d, w * d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the division elements of this vector by the scalar
/// value.
///
/// Method creates a new 4D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
/// @param d - Given scalar.
/// @return New 4D vector.
template <class T>
TVect4<T> TVect4<T>::operator / (const double d) const
  {
  Assert(d != 0);
  return TVect4<T>(x / d, y / d, z / d, w / d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements of the vector.
///
/// Vector elements clipped by the given values in the following sense.
/// Parameters of the method define lower and upper boundary of the elements.
/// If first element of this vector more than upper boundary then first element
/// is replaced by the upper boundary. If first element of this vector less
/// than lower boundary then first element is replaced by the lower boundary.
/// The rest elements of this vector may be modified analogously.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin - lower bound of the vector elements.
/// @param vmax - upper bound of the vector elements.
/// @return None (only this vector is affected).
template <class T>
void TVect4<T>::Clip(const double vmin, const double vmax)
  {
  Assert(vmax >= vmin);
  Integra::Clip(x, vmin, vmax);
  Integra::Clip(y, vmin, vmax);
  Integra::Clip(z, vmin, vmax);
  Integra::Clip(w, vmin, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements lower than given minimum.
///
/// @param vmin - Lower bound of the vector elements.
template <class T>
void TVect4<T>::ClipLower(double vmin)
  {
  Integra::ClipLower(x, vmin);
  Integra::ClipLower(y, vmin);
  Integra::ClipLower(z, vmin);
  Integra::ClipLower(w, vmin);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements higher than given maximum.
///
/// @param vmax - Top bound of the vector elements.
template <class T>
void TVect4<T>::ClipHigher(double vmax)
  {
  Integra::ClipHigher(x, vmax);
  Integra::ClipHigher(y, vmax);
  Integra::ClipHigher(z, vmax);
  Integra::ClipHigher(w, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip vector elements to given range.
///
/// Vector elements are clipped to given range.
/// Cliped vector is returned.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin Lower bound of the vector elements.
/// @param vmax Upper bound of the vector elements.
/// @return Clipped vector.
///
template <class T>
TVect4<T> TVect4<T>::ValToRange(const T vmin, const T vmax) const
  {
  Assert(vmax >= vmin);
  return TVect4<T>(
    Integra::ValToRange(x, vmin, vmax),
    Integra::ValToRange(y, vmin, vmax),
    Integra::ValToRange(z, vmin, vmax),
    Integra::ValToRange(w, vmin, vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check of the elements of this vector - are they in given range.
///
/// Parameters of the method define the lower and upper boundaries of the some
/// range. Method returns true if all four coordinates of this vector are
/// satisfied of the conditions:
/// - coordinates of this vector less or equal than upper boundary @b and
/// - coordinates of this vector more or equal than lower boundary.
///
/// @param vmin - Minimal value of the range (lower bound).
/// @param vmax - Maximal value of the range (upper bound).
/// @return true if coordinates of this vector in specified range,
/// false - otherwise.
template <class T>
bool TVect4<T>::InRange(double vmin, double vmax) const
  {
  Assert(vmax >= vmin);
  return (x >= vmin && x <= vmax &&
          y >= vmin && y <= vmax &&
          z >= vmin && z <= vmax &&
          w >= vmin && w <= vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Index of maximal element (by absolute value).
///
/// @return 0 - if first element of this vector is maximal by absolute value,
/// 1 - if second element is maximal by absolute value, 2 - if third element
/// is maximal by absolute value, 3 - otherwise.
template <class T>
int TVect4<T>::MaxElementIndex() const
  {
  if ((Abs(x) >= Abs(y)) && (Abs(x) >= Abs(z)) && (Abs(x) >= Abs(w)))
    return 0;
  if ((Abs(y) >= Abs(z)) && (Abs(y) >= Abs(w)))
    return 1;
  if (Abs(z) >= Abs(w))
    return 2;
  return 3;
  }

///////////////////////////////////////////////////////////////////////////////
/// Absolute value of maximal element (by absolute value) of the vector.
///
/// @return Absolute value of maximal element (by absolute value) of the vector.
template <class T>
T TVect4<T>::MaxElement() const
  {
  return Max( Max3(Abs(x), Abs(y), Abs(z)), Abs(w));
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the given values.
///
/// Elements of this vector are replaced by the corresponding parameters
/// (double values) of the method.
///
/// @param x0 - Value for the first vector element.
/// @param y0 - Value for the second vector element.
/// @param z0 - Value for the third vector element.
/// @param w0 - Value for the fourth vector element.
/// @return None (this vector is affected).
template <class T>
void TVect4<T>::Set(double x0, double y0, double z0, double w0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  w = (T)w0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the same value.
///
/// The all elements of this vector are replaced by the parameter (double value)
/// of the method.
/// @param a - New value for the all elements of this vector.
/// @return None (this vector is affected).
template <class T>
void TVect4<T>::Set(double a)
  {
  x = y = z = w = (T)a;
  }

//////////////////////////////////////////////////////////////////////////////
/// Sum of the elements of the vector.
///
/// @return Value of the sum of the elements of this vector
template <class T>
T TVect4<T>::Sum() const
  {
  return (T)(x + y + z + w);
  }

///////////////////////////////////////////////////////////////////////////////
/// Projection of this vector to given vector.
///
/// Method creates a new 4D vector. Direction of new vector is equal to
/// direction of the given vector (cf. signs of the vector elements are not
/// changed) - if Dot product (see function
/// DotProd(const TVect4<T> &a, const TVect4<T> &b)) of this vector and given
/// vector is positive value. Direction of new vector is opposite to direction
/// of the given vector - if Dot product of this vector and given vector is
/// negative value. The Length of the new vector (see function
/// Length(const TVect4<T> &u)) is product of the length of this vector and
/// cosin (see function Cos(const TVect4<T> &a, const TVect4<T> &b)) of acute
/// angle between 3D components of this and given  vectors.
///
/// @note Given vector may not be equal to zero vector. Debug version ASSERTs it.
///
/// @param u - Given vector (projective axis)
/// @return New 4D vector (projection)
template <class T>
TVect4<T> TVect4<T>::Project(const TVect4<T> &u) const
  {
  double sqr_len = DotProd(u, u);
  Assert(sqr_len > 0.0);;
  return u * DotProd((*this), u) / sqr_len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Normalize vector.
///
/// Method normalizes this vector and returns itself. 3D component of the
/// normalized vector has the same direction as 3D component of the initial
/// vector (before normalization) but Length (see function
/// Length(const TVect4<T> &u)) of the normalized vector is  equal to 1.
///
/// @note Zero vectors are not allowed, DEBUG version ASSERTs it.
/// @return Reference to this vector (normalized).
template <class T>
TVect4<T> &TVect4<T>::Normalize()
  {
  double len = Length(*this);
  Assert(len != 0.0);
  x = (T)(x / len);
  y = (T)(y / len);
  z = (T)(z / len);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of the vector and normalization of the vector.
///
/// Method calculates Length of this vector (see function
/// Length(const TVect4<T> &u) - length of the 4D vector is defined by the
/// first three elements only) and if length of
/// the vector is not zero then normalizes this vector (divides the first three
/// elements of the vector by its length). 3D component of the normalized
/// vector has the same direction as 3D component of initial vector (before
/// normalization) but length of the normalized vector is equal to 1.
///
/// @note Zero vectors are allowed.
/// @return Length of this vector before normalization.
template <class T>
double TVect4<T>::ModNormalize()
  {
  double len = Length(*this);
  if (len > 0.0)
    {
    x = (T)(x / len);
    y = (T)(y / len);
    z = (T)(z / len);
    }
  return len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculate the sum of the vector components and divide to this sum.  Hardly it is required.
///
/// Method calculates sum of this vector components and if sum is not
/// zero then divides this vector ti the sum. Normalized vector has the same direction
/// as initial vector (before normalization) but sum of the normalized
/// vector components is equal to 1.
///
/// @note Zero vectors are allowed.
/// @return Sum of this vector components before normalization.
///
template <class T>
double TVect4<T>::SumNormalize()
  {
  double sum = Sum();
  if (sum != 0.0)
    {
    x = (T)(x / sum);
    y = (T)(y / sum);
    z = (T)(z / sum);
    }
  return sum;
  }

///////////////////////////////////////////////////////////////////////////////
/// Value of the maximal element of the vector and normalization of the vector
/// to the maximal value.
///
/// Method searchs absolute value of maximal element (by absolute value) of
/// this vector (see method MaxElement()) and if found value is not zero then
/// normalizes this vector to this value cf. divides the first three elements
/// of this vector by this value. The 3D component of the resulted vector has
/// the same direction as 3D component of the initial vector (before
/// normalization) but the all elements of this vector are less or equal than 1.
///
/// @note Zero vectors are allowed, in this case the first three elements of
/// vector are set to 1.
/// @return Absolute value of the maximal element (by absolute value) before
/// normalization.
template <class T>
double TVect4<T>::MaxNormalize()
  {
  double len = MaxElement();
  if (len != 0.0)
    {
    x = (T)(x / len);
    y = (T)(y / len);
    z = (T)(z / len);
    }
  else
    {
    x = 1;
    y = 1;
    z = 1;
    }
  return len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Test of vector normalization.
///
/// Vector is normalized if its Length (see function
/// Length(const TVect4<T> &u) - length of the 4D vector is defined by the
/// first three elements only) is equal to 1.
/// Method calculates length of this vector and compares this length with 1.
///
/// @return true - if this vector is normalized, false - otherwise.
template <class T>
bool TVect4<T>::Normalized() const
  {
  return MathF::AboutEqual(Length(*this), 1.0);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector orientation to given vector.
///
/// Method calculates the Dot product (see function
/// DotProd(const TVect4<T> &a, const TVect4<T> &b) - three elements of the
/// vectors are used) of this vector with given vector and if calculated value
/// is negative then changes the signs of the all four elements of this vector
/// to opposite.
///
/// @param v - Given 4D vector.
/// @return None (this vector may be affected).
template <class T>
void TVect4<T>::Orient(const TVect4<T> &v)
  {
  if (DotProd(*this, v) < 0)
    Negate();
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to array of the four elements.
///
/// Usage of this operator as
/// @code
/// (T *)u
/// @endcode
/// returns pointer to the first element of the array. T is type of the
/// elements of this array.
///
/// @return  Pointer to the first element of array.
template <class T>
TVect4<T>::operator T *()
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to array of the four elements.
///
/// Usage of this operator as
/// @code
/// (T *)u
/// @endcode
/// returns pointer to the first element of the array. T is type of the
/// elements of this array.
///
/// @return  Constant pointer to the first element of array.
template <class T>
TVect4<T>::operator const T *() const
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the four elements as a constant vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 4D constant vector.
template <class T>
const TVect4<T> &TVect4<T>::Cast(const T *u)
  {
  return *(const TVect4<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the four elements as a vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 4D vector.
template <class T>
TVect4<T> &TVect4<T>::Cast(T *u)
  {
  return *(TVect4<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Cross product of the two given vectors (the first tree elements of the
/// vectors are used only).
///
/// Cross product is defined for the 3D space in the following way.
/// If first vector (multiplier) has coordinates {x1; y1; z1}, second vector
/// (multiplier) has coordinates {x2; y2; z2} then cross product is vector with
/// coordinates:
///
/// {y1 * z2 - y2* z1; x2 * z1 - x1 * z2; x1 * y2 - x2 * y1}.
///
/// This vector is orthogonal to both given vectors. Triple of the vectors
/// first multiplier, second multiplier and result vector has a 'Right hand'
/// orientation. Length of the cross product is equal of the area of the
/// parallelogram constructed by the vectors - multipliers.
///
/// Cross product of the two 4D vectors is 4D vector. The 3D component of this
/// vector (first three coordinates) is defined by the cross product of the
/// 3D components of these vectors. The fourth element of the cross product
/// vector is 1.
///
/// @param a - First 4D vector.
/// @param b - Second 4D vector.
template <class T>
inline TVect4<T> CrossProd(const TVect4<T> &a, const TVect4<T> &b)
  {
  return TVect4<T>(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Cosine of the angle between two given vectors (the first tree elements of
/// the vector are used only).
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
/// @param a - First 4D vector.
/// @param b - Second 4D vector.
/// @return Value of Cos
template <class T>
inline double Cos(const TVect4<T> &a, const TVect4<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(DotProd(a, b) / Sqrt(SqrLength(a) * SqrLength(b)), -1, 1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sine of the angle between two given vectors (the first tree elements of the
/// vector are used only).
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
///
/// @param a - First 4D vector.
/// @param b - Second 4D vector.
/// @return Value of Sine.
template <class T>
inline double Sin(const TVect4<T> &a, const TVect4<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(Sqrt(SqrLength(CrossProd(a, b)) /
                      (SqrLength(a) * SqrLength(b))),
                 0, 1);
  }

// -------------------------------------------------------------
// Implementation of Math4<T> methods
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with given tolerance.
///
/// @param v - Given 4D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than given tolerance, false - otherwise.
///
template <class T>
inline bool Math4<T>::AboutZero(const TVect4<T> &v, const double tolerance)
  {
  return Math<T>::AboutZero(v.x, tolerance) &&
         Math<T>::AboutZero(v.y, tolerance) &&
         Math<T>::AboutZero(v.z, tolerance) &&
         Math<T>::AboutZero(v.w, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Given 4D vector.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than tolerance Math<T>::TOLERANCE, false - otherwise.
///
template <class T>
inline bool Math4<T>::AboutZero(const TVect4<T> &v)
  {
  return AboutZero(v, Math4<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with given tolerance.
///
/// Method calculates difference between given vectors and result (4D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 4D vector.
/// @param v2 - Second 4D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than given tolerance, false -
/// otherwise.
///
template <class T>
inline bool Math4<T>::AboutEqual(const TVect4<T> &v1, const TVect4<T> &v2,
                                 const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given vectors and result (4D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 4D vector.
/// @param v2 - Second 4D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
///
template <class T>
inline bool Math4<T>::AboutEqual(const TVect4<T> &v1, const TVect4<T> &v2)
  {
  return AboutZero(v1 - v2, Math4<T>::TOLERANCE);
  }

INTEGRA_NAMESPACE_END
#endif
