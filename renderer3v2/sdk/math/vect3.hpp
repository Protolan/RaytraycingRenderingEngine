/// @file
///
/// @brief Definition of template class for 3D vectors and points
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>

#ifndef _KLBC_VECT3_HPP_
#define _KLBC_VECT3_HPP_

#include <base/base.hpp>

#include "math.hpp"

INTEGRA_NAMESPACE_START

// Forward references
template <class T> class BBox3;
template <class T> class BVect3;
template <class T> class Math3;
template <class T> class TMatrix3;
template <class T> class TMatrix43;
template <class T> class TPoint3;
template <class T> class TVect3;

/// CIE sum vector, set during setting input CIEConv for ColorModel.
extern double CIE_SUM_VECT[3]; 


/// @brief Class BVect3 is set of the three elements of arbitrary type.
/// Arithmetic operations are not provided by this class.
template <class T>
class BVect3
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector element
    T x,
    /// Second vector component
      y,
    /// Third vector component
      z;

  public:
    /// @name Constructors.
    //@{
    /// Default constructor, no initialization.
    inline BVect3();
    /// Constructor from the value.
    inline BVect3(const T &v);
    /// Constructor from the given elements.
    inline BVect3(const T &x0, const T &y0, const T &z0);
    /// Copy constructor.
    inline BVect3(const BVect3<T> &u);
    //@}

    /// @name Element access.
    //@{
    /// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
    inline T & operator [](int i);
    /// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
    inline const T &operator [](int i) const;
    //@}

    /// @name Assignment.
    //@{
    /// Assignment operator.
    inline BVect3<T> & operator = (const BVect3<T> &u);
    /// Set the vector elements to the given values.
    inline void Set(const T &x0, const T &y0, const T &z0);
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const BVect3<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const BVect3<T> &u) const;
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Conversion of the vector to the array of three elements.
    inline operator T *();
    /// Conversion of the vector to the array of three elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of three elements as a vector.
    inline static const BVect3<T> &Cast(const T *u);
    /// Treatment of the array of three elements as a vector.
    inline static BVect3<T> &Cast(T *u);
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 3D sets.
    //@{
    /// @brief Serialize the vector using given default.
    void Serialize(Serializer &inout, const char *tag, const BVect3<T> &dval = 0);
    /// @brief Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// Value of maximal element of the vector.
    inline T MaxElement() const;

    /// @name Number of the vector elements.
    //@{
    /// Number of the vector elements.
    static int NComponents();
    //@}
  };  // class BVect3<T>


/// 3D vector of arbitrary type.
template <class T>
class TVect3
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector component
    T x,
    /// Second vector component
      y,
    /// Third vector component
      z;

  public:

    /// @name Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TVect3();
    /// Constructor from given value.
    inline TVect3(const T &v);
    /// Constructor from given elements.
    inline TVect3(const double x0, const double y0, const double z0);
    /// Constructor from two points.
    inline TVect3(const TPoint3<T> &from, const TPoint3<T> &to);
    /// Copy constructor.
    inline TVect3(const TVect3<T> &u);
    /// Constructor from the Set3b.
    inline TVect3(const BVect3<BYTE> &u);
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
    inline T & operator [](int i);
    /// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
    inline const T &operator [](int i) const;
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const TVect3<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const TVect3<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool LessOrEqual(const TVect3<T> &u) const;
    /// Comparison by lexicographical order.
    inline bool operator < (const TVect3<T> &u) const;
    /// Checks - is some component which is greater than 0.
    inline bool IsPositiveComp() const;
    /// Comparison of the vectors with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2);
    /// Comparison of the vectors with given tolerance.
    inline static bool AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2,
                       const double tolerance);
    /// Check - are all coordinates equal to 0.
    inline bool IsZero() const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given vector to this vector.
    inline TVect3<T> & operator = (const TVect3<T> &u);

    // Vector-to-vector operators.

    /// Element-by-element addition of the given vector to this vector.
    inline TVect3<T> & operator += (const TVect3<T> &u);
    /// Element-by-element subtraction of the given vector from this vector.
    inline TVect3<T> & operator -= (const TVect3<T> &u);
    /// Element-by-element multiplication of this vector by given vector.
    inline TVect3<T> & operator *= (const TVect3<T> &u);
    /// Element-by-element division of this vector by given vector.
    inline TVect3<T> & operator /= (const TVect3<T> &u);

    // Vector-to-scalar operators.

    /// Addition of the given scalar to elements of this vector.
    inline TVect3<T> & operator += (const double d);
    /// Subtraction of the given scalar from elements of this vector.
    inline TVect3<T> & operator -= (const double d);
    /// Multiplication of this vector by given scalar.
    inline TVect3<T> & operator *= (const double d);
    /// Vector by scalar division.
    inline TVect3<T> & operator /= (const double d);
    /// Addition with a weight.
    inline TVect3<T> &AddWithWeight(const TVect3<T> &u, const double w);

    // Vector-to-matrix operators.

    /// Multiplication of this vector by the given matrix.
    TVect3<T> operator * (const TMatrix3<T> &v) const;
    //@}

    /// @name Unary operators.
    //@{
    /// Negation of this vector.
    inline void Negate();
    /// Creation of the opposite vector.
    inline TVect3<T> operator -() const;
    //@}

    /// @name Binary operators.
    //@{
    // Vector-to-vector operators.

    /// @brief Vector creation by the element-by-element addition of the given
    /// vector to this vector.
    inline TVect3<T> operator + (const TVect3<T> &u) const;
    /// @brief Vector creation by the element-by-element subtraction a given
    /// vector from this vector.
    inline TVect3<T> operator - (const TVect3<T> &u) const;
    /// @brief Vector creation by the element-by-element multiplication a given
    /// vector to this vector.
    inline TVect3<T> operator * (const TVect3<T> &u) const;
    /// @brief Vector creation by the element-by-element division of this
    /// vector by given vector.
    inline TVect3<T> operator / (const TVect3<T> &u) const;
    // Vector-to-scalar operators

    /// @brief Vector creation by addition a scalar value to elements of
    /// this vector.
    inline TVect3<T> operator + (const double d) const;
    /// @brief Vector creation by subtraction a scalar value from elements
    /// of  this vector.
    inline TVect3<T> operator - (const double d) const;
    /// @brief Vector creation by the multiplication a scalar value to elements
    /// of this vector.
    inline TVect3<T> operator * (const double d) const;
    /// @brief Vector creation by the division elements of this vector by the
    /// scalar value.
    inline TVect3<T> operator / (const double d) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 3D vectors.
    //@{
    /// Serialize the vector using given default.
    void Serialize(Serializer &inout, const char *tag, const TVect3<T> &dval = 0);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Other methods.
    //@{
    /// Clipping elements of the vector.
    inline void Clip(double vmin, double vmax);
    /// Clipping elements lower than given minimum.
    inline bool ClipLower(double vmin);
    /// Clipping elements higher than given maximum.
    inline bool ClipHigher(double vmax);
    /// Clip vector elements to given range.
    inline TVect3<T> ValToRange(const T &vmin, const T &vmax) const;
    /// Check of the elements of this vector - are they in given range.
    inline bool InRange(double vmin, double vmax) const;
    /// Round the vector - delete decimals lower then given precision.
    void Round(double level = Math<float>::TOLERANCE);
    /// Check that all elements of vector are valid floats.
    inline bool IsOK() const;
    /// Index of maximal element (by absolute value).
    inline int MaxElementIndex() const;
    /// Index of minimal element (by absolute value).
    inline int MinElementIndex() const;
    /// Absolute value of maximal element (by absolute value) of the vector.
    inline T MaxElement() const;
    /// Value of maximal element of the vector.
    inline T MaxVal() const;
    /// Value of minimal element of the vector.
    inline T MinVal() const;
    /// Set of the vector elements to the given values.
    inline void Set(double x0, double y0, double z0);
    /// Set of the vector elements to the same value.
    inline void Set(double a);
    /// Squared length of this vector.
    inline double SqrLength() const;
    /// Length of this vector.
    inline double Length() const;
    /// Sum of the elements of the vector.
    inline T Sum() const;

    /// Y (CIE) component of color. Don't use it in the modern applications - potentially 2 different color spaces are possible! 
    inline double CIESum() const;
    /// CIE normalize - CIE sum is equal to 1 after normalization. Don't use it in the modern applications!
    inline double CIENormalize();
    /// Projection of this vector to given vector.
    inline TVect3<T> Project(const TVect3<T> &u) const;
    /// Any unit orthogonal vector to given vector.
    TVect3<T> AnyOrthogonal() const;
    /// Normalize vector.
    inline TVect3<T> &Normalize();
    /// Test of vector normalization.
    inline bool Normalized() const;
    /// Length of the vector and normalization of the vector.
    inline double ModNormalize();
    /// Calculate the sum of the vector components and divide to this sum.
    inline double SumNormalize();
    /// @brief Calculate value of the maximal element of the vector and normalize
    /// the vector by the maximal value.
    inline double MaxNormalize();
    /// Vector orientation to given vector.
    inline void Orient(const TVect3<T> &v);
    /// Provides axis with given index from 0 to 5 (x, y, z, -x, -y, -z).
    static TVect3<T> Axis(int axis_ind);
    /// Sort vertices in ascending order.
    inline void Sort();
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Conversion of the vector to array of three elements.
    inline operator T *();
    /// Conversion of the vector to array of three elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of the three elements as a constant vector.
    inline static const TVect3<T> &Cast(const T *u);
    /// Treatment of the array of the three elements as a vector.
    inline static TVect3<T> &Cast(T *u);
    //@}

    /// @name Vector to point conversion.
    /// Reference, or value of received point. Reference is always returned
    /// except the cases when this vector has 'const' scope. In this case,
    /// compiler will substitute first function (that is only value will be
    /// returned).
    //@{
    /// Vector conversion to point.
    inline const TPoint3<T> &Point() const;
    /// Vector conversion to point.
    inline TPoint3<T> &Point();
    //@}

    /// @name Number of the elements of the vector.
    //@{
    /// Number of the elements of the vector.
    static int NComponents();
    //@}

    /// Create coordinate system (matrix 3x3) from one axis.
    static void CreateCoordSystem(const TVect3<T> &v1, TVect3<T> &v2, TVect3<T> &v3);
  };  // class TVect3<T>

// -----------------------------------------------------------------
// Related template functions
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Create coordinate system (matrix 3x3) from one axis.
/// @param[in] v1 - First axis vector.
/// @param[out] v2 - Second axis vector.
/// @param[out] v3 - Third axis vector.
template <class T>
inline void TVect3<T>::CreateCoordSystem(const TVect3<T> &v1,
                                         TVect3<T> &v2, TVect3<T> &v3)
  {
  if (Abs(v1.x) > Abs(v1.y))
    {
    double invLen = 1.0 / Sqrt(v1.x * v1.x + v1.z * v1.z);
    v2 = TVect3<T>(-v1.z * invLen, 0.0, v1.x * invLen);
    }
  else
    {
    double invLen = 1.0 / Sqrt(v1.y * v1.y + v1.z * v1.z);
    v2 = TVect3<T>(0.0, v1.z * invLen, -v1.y * invLen);
    }
  v3 = CrossProd(v1, v2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given vectors and result (3D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 3D vector.
/// @param v2 - Second 3D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
inline bool TVect3<T>::AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2)
  {
  return Math3<T>::AboutZero(v1 - v2, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with given tolerance.
///
/// Method calculates difference between given vectors and result (3D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 3D vector.
/// @param v2 - Second 3D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than given tolerance, false -
/// otherwise.
template <class T>
inline bool TVect3<T>::AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2,
                                 const double tolerance)
  {
  return Math3<T>::AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Vector creation by the multiplication a scalar value to elements of
/// given vector.
///
/// Function creates a new 3D vector. Elements of a new vector are the products
/// of the corresponding elements of the given vector and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 3D vector.
/// @return New 3D vector (product of vector and scalar)
template <class T>
inline TVect3<T> operator * (const double d, const TVect3<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Dot product of two given vectors.
///
/// Dot product is sum of the products of the corresponding elements of the
/// vectors - multipliers. In other side value of the DOT product is product of
/// the three multipliers:
/// - length of the first vector;
/// - length of the second vector;
/// - cosin of angle between vectors.
///
/// @param a - First 3D vector - multiplier.
/// @param b - Second 3D vector - multiplier.
/// @return Value of Dot product.
template <class T>
inline double DotProd(const TVect3<T> &a, const TVect3<T> &b)
  {
  return (double)a.x * (double)b.x +
         (double)a.y * (double)b.y +
         (double)a.z * (double)b.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cross product.
template <class T>
inline TVect3<T> CrossProd(const TVect3<T> &a, const TVect3<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Compose a vector from maximal components of 2 vectors.
/// @param[in] a - First 3D vector.
/// @param[in] b - Second 3D vector.
/// @return Vector consisting from maximal components of 2 vectors.
template <class T>
inline TVect3<T> Max(const TVect3<T> &a, const TVect3<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Squared length of the given vector.
///
/// Squared length of the vector is sum of the squared coordinates.
/// @param u - Given 3D vector.
/// @return Length of vector in second power.
template <class T>
inline double SqrLength(const TVect3<T> &u)
  {
  return DotProd(u, u);
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of the given vector.
///
/// Length of the vector is square root of the sum of the squared coordinates.
/// @param u - Given 3D vector.
/// @return Length of vector.
template <class T>
inline double Length(const TVect3<T> &u)
  {
  return Sqrt(SqrLength(u));
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cosine of angle between two given vectors.
template <class T>
inline double Cos(const TVect3<T> &a, const TVect3<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// @brief Sine of angle between two given vectors.
template <class T>
inline double Sin(const TVect3<T> &a, const TVect3<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with integer elements.
///
/// @param u - 3D vector with elements of the arbitrary type.
/// @return Vector with integer elements.
template <class T>
inline TVect3<int> ConvI(const TVect3<T> &u)
  {
  return TVect3<int>(u.x, u.y, u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with float elements.
///
/// @param u - 3D vector with elements of the arbitrary type.
/// @return Vector with float elements.
template <class T>
inline TVect3<float> ConvF(const TVect3<T> &u)
  {
  return TVect3<float>(u.x, u.y, u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with double elements.
///
/// @param u - 3D vector with elements of the arbitrary type.
/// @return Vector with double elements.
template <class T>
inline TVect3<double> ConvD(const TVect3<T> &u)
  {
  return TVect3<double>(u.x, u.y, u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with elements of the WORD type.
///
/// @param u - 3D vector with elements of the arbitrary type.
/// @return Vector with elements of the WORD type.
template <class T>
inline TVect3<WORD> ConvW(const TVect3<T> &u)
  {
  return TVect3<WORD>((WORD)u.x, (WORD)u.y, (WORD)u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with elements of the BYTE type.
///
/// @param u - 3D vector with elements of the arbitrary type.
/// @return Vector with elements of the WORD type.
template <class T>
inline BVect3<BYTE> ConvB(const TVect3<T> &u)
  {
  return BVect3<BYTE>((BYTE)u.x, (BYTE)u.y, (BYTE)u.z);
  }

/// 3D point of arbitrary type.
template <class T>
class TPoint3
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First point coordinate
    T x,
    /// Second point coordinate
      y,
    /// Third point coordinate
      z;

  public:
    /// @name Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TPoint3();
    /// Constructor from the scalar.
    inline TPoint3(const double v);
    /// Constructor from the three scalars.
    inline TPoint3(const double x0, const double y0, const double z0);
    /// Copy constructor.
    inline TPoint3(const TPoint3<T> &u);
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th element of the point: x (i = 0), y (i = 1), z (i = 2).
    inline T & operator [](int i);
    /// Access to the i-th element of the point: x (i = 0), y (i = 1), z (i = 2).
    inline const T &operator [](int i) const;
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this point with given point.
    inline bool operator == (const TPoint3<T> &u) const;
    /// Element-by-element comparison of this point with given point.
    inline bool operator != (const TPoint3<T> &u) const;
    /// Element-by-element comparison of this point with given point.
    inline bool LessOrEqual(const TPoint3<T> &u) const;
    /// Comparison by lexicographical order.
    inline bool operator < (const TPoint3<T> &u) const;
    /// Comparison of the points with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2);
    /// Comparison of the points with given tolerance.
    inline static bool AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2,
                       const double tolerance);
    /// Check that all elements of point are valid floats.
    inline bool IsOK() const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given point to this point.
    inline TPoint3<T> & operator = (const TPoint3<T> &u);
    //@}

    /// @name Point-to-point operators.
    //@{
    /// Addition of the given point to this point.
    inline TPoint3<T> & operator += (const TPoint3<T> &u);
    //@}

    /// @name Point-to-vector operators.
    //@{
    /// Addition of the given vector to this point.
    inline TPoint3<T> & operator += (const TVect3<T> &u);
    /// Subtraction of the given vector from this point.
    inline TPoint3<T> & operator -= (const TVect3<T> &u);
    //@}

    /// @name Point-to-scalar operators.
    //@{
    /// Addition of the given scalar to elements of this point.
    inline TPoint3<T> & operator += (const double d);
    /// Subtraction of the given scalar from elements of this point.
    inline TPoint3<T> & operator -= (const double d);
    /// Multiplication of this point by given scalar.
    inline TPoint3<T> & operator *= (const double d);
    /// Point by scalar division.
    inline TPoint3<T> & operator /= (const double d);
    /// Addition with a weight.
    inline TPoint3<T> &AddWithWeight(const TVect3<T> &u, const double w);
    //@}

    /// @name Unary operators.
    //@{
    /// Negation of this point.
    inline void Negate();
    /// Creation of the opposite point.
    inline TPoint3<T> operator -() const;
    //@}

    /// @name Binary operators.
    //@{
    /// @name Point-to-vector(point) operators.
    //@{
    /// Creation of the point by addition of the given point to point.
    inline TPoint3<T> operator + (const TPoint3<T> &u) const;
    /// Creation of the point by addition of the given vector to point.
    inline TPoint3<T> operator + (const TVect3<T> &u) const;
    /// Creation of the point by subtraction of the given vector from point.
    inline TPoint3<T> operator - (const TVect3<T> &u) const;
    /// @brief Creation of the vector by subtraction of the given point
    /// from this point.
    inline TVect3<T> operator - (const TPoint3<T> &p) const;
    //@}

    /// @name Point-to-scalar operators.
    //@{
    /// Point creation by addition a scalar value to elements of this point.
    inline TPoint3<T> operator + (const double d) const;
    /// @brief Point creation by subtraction a scalar value from elements
    /// of  this point.
    inline TPoint3<T> operator - (const double d) const;
    /// @brief Point creation by the multiplication a scalar value to elements
    /// of  this point.
    inline TPoint3<T> operator * (const double d) const;
    /// @brief Point creation by the division of the elements of this point by
    /// the scalar value.
    inline TPoint3<T> operator / (const double d) const;
    //@}
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 3D points.
    //@{
    /// Serialize the point using given default.
    void Serialize(Serializer &inout, const char *tag, const TPoint3<T> &dval = 0);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Other methods.
    //@{
    /// Clipping elements of the point.
    inline void Clip(double vmin, double vmax);
    /// Clipping elements lower than given minimum.
    inline bool ClipLower(double vmin);
    /// Clip point elements to given range.
    inline TPoint3<T> ValToRange(const T &vmin, const T &vmax) const;
    /// Set of the point elements to the given values.
    inline void Set(double x0, double y0, double z0);
    /// Set of the point elements to the same value.
    inline void Set(double a);
    //@}

    /// @name Conversion of point to the array.
    //@{
    /// Point conversion to the array of the three elements.
    inline operator T *();
    /// Point conversion to the array of the three elements.
    inline operator const T *() const;
    //@}

    /// @name Array to point conversion.
    /// Treat an array of three elements as a point.  Reference, or constant
    /// reference to received point is returned. Reference is always returned
    /// except the cases when the array has 'const' scope. In this case, compiler
    /// will substitute first function (that is constant reference will be
    /// returned).
    //@{
    /// Treatment of array of three elements as a constant point.
    inline static const TPoint3<T> &Cast(const T *u);
    /// Treatment of array of three elements as a point.
    inline static TPoint3<T> &Cast(T *u);
    //@}

    /// @name Point to vector conversion.
    /// Convert point to vector. Reference, or constant reference to received
    /// vector is returned. Reference is always returned except the cases when
    /// this point has 'const' scope. In this case, compiler will substitute
    /// first function (that is constant reference will be returned).
    //@{
    /// Treatment of the point as constant vector.
    inline const TVect3<T> &Vector() const;
    /// Treatment of the point as vector.
    inline TVect3<T> &Vector();
    //@}

    /// @name Number of the elements of the point.
    //@{
    /// Number of the elements of the point.
    static int NComponents();
    //@}
  };  // class TPoint3<T>

// -----------------------------------------------------------------
// Related template functions
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given points and result (3D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 3D point.
/// @param v2 - Second 3D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
inline bool TPoint3<T>::AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2)
  {
  return Math3<T>::AboutZero(v1 - v2, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with given tolerance.
///
/// Method calculates difference between given points and result (3D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 3D point.
/// @param v2 - Second 3D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than given tolerance, false -
/// otherwise.
template <class T>
inline bool TPoint3<T>::AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2,
                                 const double tolerance)
  {
  return Math3<T>::AboutZero(v1 - v2, tolerance);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of vector are valid floats.
template <class T>
bool TPoint3<T>::IsOK() const
  {
  return (FloatIsOK(x) && FloatIsOK(y) && FloatIsOK(z));
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Point creation by the multiplication a scalar value to elements of
/// given point.
///
/// Function creates a new 3D point. Elements of a new point are the products of
/// corresponding elements of the given point and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 3D point.
/// @return New 3D point (product of point and scalar)
template <class T>
inline TPoint3<T> operator * (const double d, const TPoint3<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Squared distance between two 3D points.
///
/// @param a - First 3D point.
/// @param b - Second 3D point.
/// @return Distance between points in second power.
template <class T>
inline double SqrDist(const TPoint3<T> &a, const TPoint3<T> &b)
  {
  return SqrLength(a - b);
  }

///////////////////////////////////////////////////////////////////////////////
/// Distance between two 3D points.
///
/// @param a - First 3D point.
/// @param b - Second 3D point.
/// @return Distance between points.
template <class T>
inline double Dist(const TPoint3<T> &a, const TPoint3<T> &b)
  {
  return Length(a - b);
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Center of the segment defined by the two given points.
template <class T>
inline TPoint3<T> Center(const TPoint3<T> &a, const TPoint3<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// @brief Center of the triangle.
template <class T>
inline TPoint3<T> Center(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c);
///////////////////////////////////////////////////////////////////////////////
/// @brief Area of the triangle.
template <class T>
inline double TrgArea(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c);

///////////////////////////////////////////////////////////////////////////////
/// Angle of the triangle vertex.
template <class T>
inline double VertAngle(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c);

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with integer elements.
///
/// @param u - 3D point with elements of the arbitrary type.
/// @return Point with integer elements.
template <class T>
inline TPoint3<int> ConvI(const TPoint3<T> &u)
  {
  return TPoint3<int>(u.x, u.y, u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with float elements.
///
/// @param u - 3D point with elements of the arbitrary type.
/// @return Point with float elements.
template <class T>
inline TPoint3<float> ConvF(const TPoint3<T> &u)
  {
  return TPoint3<float>(u.x, u.y, u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with double elements.
///
/// @param u - 3D point with elements of the arbitrary type.
/// @return Point with double elements.
template <class T>
inline TPoint3<double> ConvD(const TPoint3<T> &u)
  {
  return TPoint3<double>(u.x, u.y, u.z);
  }


/// @brief Simple bounding 3D box. In fact it is rectangular parallelepiped
///        with sides parallel to the coordinate planes.
template <class T>
class BBox3
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:

    /// Point with minimal coordinates
    TPoint3<T> vmin,
    /// Point with maximal coordinates
               vmax;

  public:

    /// @name Constructors
    //@{
    /// Default constructor without initialization.
    inline BBox3();
    /// Constructor by one point.
    inline BBox3(const TPoint3<T> &point);
    /// Constructor by the two points.
    inline BBox3(const TPoint3<T> &point1, const TPoint3<T> &point2);
    /// Copy constructor.
    inline BBox3(const BBox3<T> &box);
    //@}

    /// @name Assignment.
    //@{
    /// Constructor from one point.
    inline void Init(const TPoint3<T> &point);
    /// Constructor from two points.
    inline void Init(const TPoint3<T> &min, const TPoint3<T> &max);
    /// Assignment operator.
    inline BBox3<T> & operator = (const BBox3<T> &box);
    //@}

    /// Multiplication of this box to given scalar.
    BBox3<T> & operator *= (const double d);
    /// Division of this box by given scalar.
    BBox3<T> & operator /= (const double d);

    /// @name Comparison operators and methods.
    //@{
    /// Comparison of the two bounding boxes for equality.
    inline bool operator == (const BBox3<T> &box) const;
    /// Comparison of the two bounding boxes for non-equality.
    inline bool operator != (const BBox3<T> &box) const;
    //@}

    /// @name Test for validaty.
    //@{
    /// Set empty BBox.
    inline void SetEmpty();
    /// Check that box is not empty.
    inline bool NotEmpty() const;
    /// Check that box is empty.
    inline bool IsEmpty() const;
    /// Check that box contains one  point only.
    inline bool IsDot() const;
    //@}

    /// @name Inclusion to box and intersection with box.
    //@{
    /// Check that bounding box includes the given point.
    inline bool Includes(const TPoint3<T> &point) const;
    /// Check that bounding box includes the given box.
    inline bool Includes(const BBox3<T> &box) const;
    /// Check that bounding box intersects the given box.
    inline bool Intersects(const BBox3<T> &box) const;
    /// Check that the ray intersects the given box.
    int Intersects(const TPoint3<T> &point, const TVect3<T> &direction, T &distance, int side) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 3D boxes.
    //@{
    /// Serialize the box (no default is provided).
    void Serialize(Serializer &inout, const char *tag);

    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Miscellaneous methods.
    //@{
    /// Extension of the box by inclusion of the given point.
    inline void Include(const TPoint3<T> &point);
    /// Extension of the box by inclusion of the given box.
    inline void Include(const BBox3<T> &box);
    /// Intersection with given box.
    inline void Intersect(const BBox3<T> &box);
    /// Translation of the box by the given vector.
    inline void Translate(const TVect3<T> &vct);
    /// Creation of the box by the translation of this box.
    inline BBox3<T> Translated(const TVect3<T> &vct) const;
    /// Creation of the box by the transformation of this box.
    BBox3<T> Transformed(const TMatrix43<T> &tm) const;
    /// Diagonal of the box.
    inline TVect3<T> Diag() const;
    /// Center of the box.
    inline TPoint3<T> Center() const;
    /// Volume of the box.
    inline double Volume() const;
    /// Get the vertex of box, with given index.
    TPoint3<T> GetVertex(int ind) const;
    //@}

  public:
    /// Get CVV (canonical view volume) cube
    static const BBox3<T> &CVV();
  };  // class BBox3<T>

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the bounding box with data of the arbitrary type to
/// bounding box with float data.
///
/// @param u - 3D box with data of the arbitrary type.
/// @return Bounding box with float data.
template <class T>
inline BBox3<float> ConvF(const BBox3<T> &u)
  {
  return BBox3<float>(ConvF(u.vmin), ConvF(u.vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the bounding box with data of the arbitrary type to
/// bounding box with double data.
///
/// @param u - 3D box with data of the arbitrary type.
/// @return Bounding box with double data.
template <class T>
inline BBox3<double> ConvD(const BBox3<T> &u)
  {
  return BBox3<double>(ConvD(u.vmin), ConvD(u.vmax));
  }

/// Comparison of 3D vectors and points with a tolerance.
template <class T>
class Math3 : public Math<T>
  {
  public:

    /// @name Comparison of the vector(point) with tolerance.
    //@{

    /// Comparison of the vector with given tolerance.
    inline static bool AboutZero(const TVect3<T> &v, const double tolerance);

    /// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TVect3<T> &v);

    /// Comparison of the vector with predefined tolerance ::EPSILON.
    inline static bool NearZero(const TVect3<T> &v);

    /// Comparison of the point with given tolerance.
    inline static bool AboutZero(const TPoint3<T> &v, const double tolerance);

    /// Comparison of the point with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TPoint3<T> &v);

    /// Comparison of the point with predefined tolerance ::EPSILON.
    inline static bool NearZero(const TPoint3<T> &v);

    //@}

    /// @name Comparison of the vectors(points) with tolerance.
    //@{

    /// Comparison of the vectors with given tolerance.
    inline static bool AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2,
                       const double tolerance);

    /// Comparison of the vectors with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2);

    /// Comparison of the vectors with predefined tolerance ::EPSILON.
    inline static bool NearEqual(const TVect3<T> &v1, const TVect3<T> &v2);

    /// Comparison of the points with given tolerance.
    inline static bool AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2,
                       const double tolerance);

    /// Comparison of the points with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2);

    /// Comparison of the points with predefined tolerance ::EPSILON.
    inline static bool NearEqual(const TPoint3<T> &v1, const TPoint3<T> &v2);

    //@}

  };  // class MAth3<T>


// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 3D vector with elements of BYTE type.
typedef BVect3<BYTE>      Set3b;
/// Predefined instance of 3D vector with elements of double type.
typedef BVect3<double>    Set3d;
/// Predefined instance of 3D vector with elements of float type.
typedef BVect3<float>     Set3f;
/// Predefined instance of 3D vector with elements of short type.
//typedef BVect3<short>     Set3s;
/// Predefined instance of 3D vector with elements of int type.
typedef BVect3<int>       Set3i;
/// Predefined instance of 3D vector with elements of unsigned type.
typedef BVect3<unsigned>  Set3u;

/// Predefined instance of 3D vector with elements of short type.
typedef TVect3<short>     Vect3s;
/// Predefined instance of 3D vector with elements of int type.
typedef TVect3<int>       Vect3i;
/// Predefined instance of 3D vector with elements of float type.
typedef TVect3<float>     Vect3f;
/// Predefined instance of 3D vector with elements of double type.
typedef TVect3<double>    Vect3d;
/// Predefined instance of 3D vector with elements of unsigned type.
typedef TVect3<unsigned>  Vect3u;

/// Predefined instance of 3D vector with elements of WORD type.
typedef TVect3<WORD>      Color3w;
/// Predefined instance of 3D vector with elements of short type.
typedef TVect3<short>     Color3s;
/// Predefined instance of 3D vector with elements of int type.
typedef TVect3<int>       Color3i;
/// Predefined instance of 3D vector with elements of float type.
typedef TVect3<float>     Color3f;
/// Predefined instance of 3D vector with elements of float type.
typedef TVect3<float>     RGBColor;
/// Predefined instance of 3D vector with elements of double type.
typedef TVect3<double>    Color3d;

/// Predefined instance of 3D point with elements of int type.
typedef TPoint3<int>      Point3i;
/// Predefined instance of 3D point with elements of float type.
typedef TPoint3<float>    Point3f;
/// Predefined instance of 3D point with elements of double type.
typedef TPoint3<double>   Point3d;

/// Predefined instance of 3D bounding box with elements of int type.
typedef BBox3<int>        BBox3i;
/// Predefined instance of 3D bounding box with elements of float type.
typedef BBox3<float>      BBox3f;
/// Predefined instance of 3D bounding box with elements of double type.
typedef BBox3<double>     BBox3d;

/// Predefined instance of float type.
typedef Math3<float>      Math3f;
/// Predefined instance of double type.
typedef Math3<double>     Math3d;

/// DistinctColorGenerator for given color index.
Set3b DistinctColorGenerator(double seed, int col_ind, double v);

// =============================================================
// IMPLEMENTATION PART
// =============================================================

// -------------------------------------------------------------
// BVect3<T> methods and related functions
// -------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization.
template <class T>
BVect3<T>::BVect3()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the value.
///
/// The all three elements of the vector are initialized by the given value.
/// @param v - Given value.
template <class T>
BVect3<T>::BVect3(const T &v)
  {
  x = v;
  y = v;
  z = v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given elements.
///
/// Elements of the vector are initialized by the parameters of the method
/// according to its order.
/// @param x0 - Value for the first vector element.
/// @param y0 - Value for the second vector element.
/// @param z0 - Value for the third vector element.
template <class T>
BVect3<T>::BVect3(const T &x0, const T &y0, const T &z0)
  {
  x = x0;
  y = y0;
  z = z0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param u - Source 3D vector.
template <class T>
BVect3<T>::BVect3(const BVect3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to i-th element of this vector.
template <class T>
T & BVect3<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value
/// @param i - Index of the vector element.
/// @return Constant reference to i-th element of this vector.
template <class T>
const T &BVect3<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Number of the vector elements.
///
/// @return 3.
template <class T>
int BVect3<T>::NComponents()
  {
  return 3;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 3D vector.
/// @return Reference to this vector (updated).
template <class T>
BVect3<T> & BVect3<T>::operator = (const BVect3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the vector elements to the given values.
/// @param x0 - Given X value.
/// @param y0 - Given Y value.
/// @param z0 - Given Z value.
template <class T>
void BVect3<T>::Set(const T &x0, const T &y0, const T &z0)
  {
  x = x0;
  y = y0;
  z = z0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if all elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool BVect3<T>::operator == (const BVect3<T> &u) const
  {
  return (x == u.x) && (y == u.y) && (z == u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if some element (or several alements together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool BVect3<T>::operator != (const BVect3<T> &u) const
  {
  return (x != u.x) || (y != u.y) || (z != u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of three elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Pointer to the first element of array.
template <class T>
BVect3<T>::operator T *()
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of three elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Constant pointer to the first element of array.
template <class T>
BVect3<T>::operator const T *() const
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of three elements as a vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Constant reference to the 3D vector.
template <class T>
const BVect3<T> &BVect3<T>::Cast(const T *u)
  {
  return *(const BVect3<T> *)u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of three elements as a vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Reference to the 3D vector.
template <class T>
BVect3<T> &BVect3<T>::Cast(T *u)
  {
  return *(BVect3<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Value of maximal element of the vector.
///
/// @return Value of maximal element of the vector.
template <class T>
T BVect3<T>::MaxElement() const
  {
  return Max3(x, y, z);
  }

// -------------------------------------------------------------
// TVect3<T> methods and related functions
// -------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
TVect3<T>::TVect3()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from given value.
///
/// The vector elements are initialized by given value.
/// @param v - Given value.
template <class T>
TVect3<T>::TVect3(const T &v)
  {
  x = v;
  y = v;
  z = v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from given elements.
///
/// The elements of the vector are initialized by the method parameters
/// according to its order.
/// @param x0 - Value of the first vector element
/// @param y0 - Value of the second vector element
/// @param z0 - Value of the third vector element
template <class T>
TVect3<T>::TVect3(const double x0, const double y0, const double z0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from two points.
///
/// Vector is initialized by the two points. The first point is begin of the
/// vector, the second point is end of the vector.
///
/// @param from - 3D point - begin of the vector.
/// @param to   - 3D point - end of the vector.
template <class T>
TVect3<T>::TVect3(const TPoint3<T> &from, const TPoint3<T> &to)
  {
  *this = to - from;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - Source 3D vector.
template <class T>
TVect3<T>::TVect3(const TVect3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the Set3b.
/// @param u - Source 3D vector.
template <class T>
TVect3<T>::TVect3(const BVect3<BYTE> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to the element of this vector.
template <class T>
T & TVect3<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Constant reference to the element of this vector.
template <class T>
const T &TVect3<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if all three elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool TVect3<T>::operator == (const TVect3<T> &u) const
  {
  return (x == u.x) && (y == u.y) && (z == u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if some element (or several elements together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool TVect3<T>::operator != (const TVect3<T> &u) const
  {
  return (x != u.x) || (y != u.y) || (z != u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 3D vector.
/// @return true - if all three elements of this vector are less or equal than
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool TVect3<T>::LessOrEqual(const TVect3<T> &u) const
  {
#if 0
  return (x <= u.x) && (y <= u.y) && (z <= u.z);
#else
  return (z <= u.z) && (y <= u.y) && (x <= u.x);
#endif
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison by lexicographical order.
///
/// Comparison of two vectors. This method is different from other comparison
/// methods as it forms a lexicographical vectors order during sorting. It
/// means that it returns true immediately if first component of this vector is
/// less than first component of the given vector, or false if it is greater. If
/// first elements are equal, it compares second one and so on.
///
/// @param u - Given 3D vector.
/// @return true - if this vector less than given vector in lexical sense,
/// false - otherwise.
template <class T>
bool TVect3<T>::operator < (const TVect3<T> &u) const
  {
#if 0
  if (x < u.x)
    return true;
  if (x > u.x)
    return false;
  if (y < u.y)
    return true;
  if (y > u.y)
    return false;
  return z < u.z;
#else
  if (z < u.z)
    return true;
  if (z > u.z)
    return false;
  if (y < u.y)
    return true;
  if (y > u.y)
    return false;
  return x < u.x;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Checks - is some component which is greater than 0.
/// @return true is some component is greater than 0.
template <class T>
bool TVect3<T>::IsPositiveComp() const
  {
  return (x > 0.0 || y > 0.0 || z > 0.0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check - are all coordinates equal to 0.
template <class T>
bool TVect3<T>::IsZero() const
  {
  return x == 0 && y == 0 && z == 0;
  }


//////////////////////////////////////////////////////////////////////////////
/// Assignment of the given vector to this vector.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 3D vector.
/// @return Reference to this vector (updated).
template <class T>
TVect3<T> & TVect3<T>::operator = (const TVect3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element addition of the given vector to this vector.
///
/// Elements of this vector are replaced by the sums of corresponding elements
/// of this vector and given vector.
/// @param u - Given 3D vector.
/// @return Reference to this vector (the sum of two vectors).
template <class T>
TVect3<T> & TVect3<T>::operator += (const TVect3<T> &u)
  {
  x += u.x;
  y += u.y;
  z += u.z;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element subtraction of the given vector from this vector.
///
/// Elements of this vector are replaced by the differences between
/// corresponding elements of this vector and given vector.
///
/// @param u - Given 3D vector.
/// @return Reference to this vector (the difference between two vectors).
template <class T>
TVect3<T> & TVect3<T>::operator -= (const TVect3<T> &u)
  {
  x -= u.x;
  y -= u.y;
  z -= u.z;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element multiplication of this vector by given vector.
///
/// Elements of this vector are replaced by the products of corresponding
/// elements of this vector and given vector.
/// @param u - Given 3D vector.
/// @return Reference to this vector (product of two vectors).
template <class T>
TVect3<T> & TVect3<T>::operator *= (const TVect3<T> &u)
  {
  x *= u.x;
  y *= u.y;
  z *= u.z;
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
/// @param u - Given 3D vector.
/// @return Reference to this vector (result of division of two vectors).
template <class T>
TVect3<T> &TVect3<T>::operator /= (const TVect3<T> &u)
  {
  Assert((u.x != 0) && (u.y != 0) && (u.z != 0));
  x /= u.x;
  y /= u.y;
  z /= u.z;
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
TVect3<T> & TVect3<T>::operator += (const double d)
  {
  x = (T)(x + d);
  y = (T)(y + d);
  z = (T)(z + d);
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
TVect3<T> & TVect3<T>::operator -= (const double d)
  {
  x = (T)(x - d);
  y = (T)(y - d);
  z = (T)(z - d);
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
TVect3<T> & TVect3<T>::operator *= (const double d)
  {
  x = (T)(x * d);
  y = (T)(y * d);
  z = (T)(z * d);
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
TVect3<T> &TVect3<T>::operator /= (const double d)
  {
  Assert(d != 0.0);
  x = (T)(x / d);
  y = (T)(y / d);
  z = (T)(z / d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition with a weight.
///
/// Elements of this vector are replaced by the sums of these elements with
/// products of the given vector elements and weight value.
///
/// @param u - Given 3D vector.
/// @param w - Weight value.
/// @return  Reference to this vector (updated).
template <class T>
TVect3<T> &TVect3<T>::AddWithWeight(const TVect3<T> &u, const double w)
  {
  x = (T)(x + u.x * w);
  y = (T)(y + u.y * w);
  z = (T)(z + u.z * w);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Negation of this vector.
///
/// The signs of the all elements of this vector are changed to opposite. As a
/// result the direction of this vector is changed to opposite.
/// @return None (this vector is affected).
template <class T>
void TVect3<T>::Negate()
  {
  x = -x;
  y = -y;
  z = -z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Creation of the opposite vector.
///
/// Method creates a new 3D vector. Elements of a new vector are negative with
/// respect to the elements of this vector. As a result the length of the new
/// vector is equal of the length of this vector but direction is opposite with
/// respect to this vector.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator -() const
  {
  return TVect3<T>(-x, -y, -z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element addition of the given vector to
/// this vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given vector.
/// @param u - Given 3D vector.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator + (const TVect3<T> &u) const
  {
  return TVect3<T>(x + u.x, y + u.y, z + u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element subtraction a given vector from
/// this vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given vector.
/// @param u - Given 3D vector.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator - (const TVect3<T> &u) const
  {
  return TVect3<T>(x - u.x, y - u.y, z - u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element multiplication a given vector to
/// this vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given vector.
/// @param u - Given 3D vector.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator * (const TVect3<T> &u) const
  {
  return TVect3<T>(x * u.x, y * u.y, z * u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element division of this vector by given
/// vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the elements of the given
/// vector.
/// @note Given vector may not have zero elements. Debug version ASSERTs
/// about this.
/// @param u - Given 3D vector.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator / (const TVect3<T> &u) const
  {
  Assert((u.x != 0) && (u.y != 0) && (u.z != 0));
  return TVect3<T>(x / u.x, y / u.y, z / u.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by addition a scalar value to elements of this vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator + (const double d) const
  {
  return TVect3<T>((T)(x + d), (T)(y + d), (T)(z + d));
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by subtraction a scalar value from elements of  this
/// vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator - (const double d) const
  {
  return TVect3<T>((T)(x - d), (T)(y - d), (T)(z - d));
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the multiplication a scalar value to elements of  this
/// vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator * (const double d) const
  {
  return TVect3<T>((T)(x * d), (T)(y * d), (T)(z * d));
  }

//////////////////////////////////////////////////////////////////////////////
/// Vector creation by the division elements of this vector by the scalar
/// value.
///
/// Method creates a new 3D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
/// @param d - Given scalar.
/// @return New 3D vector.
template <class T>
TVect3<T> TVect3<T>::operator / (const double d) const
  {
  Assert(d != 0);
  return (*this) * (1/d);
  }

//////////////////////////////////////////////////////////////////////////////
/// Number of the elements of the vector.
///
/// @return 3
template <class T>
int TVect3<T>::NComponents()
  {
  return 3;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements of the vector.
///
/// Vector elements clipped by the given values in the following sense.
/// Parameters of the method define lower and upper boundary of the elements.
/// If first element of this vector more than upper boundary then first element
/// is replaced by the upper boundary. If first element of this vector less
/// than lower boundary then first element is replaced by the lower boundary.
/// Second and third elements of this vector may be modified analogously.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin - Lower bound of the vector elements.
/// @param vmax - Upper bound of the vector elements.
/// @return None (only this vector is affected).
template <class T>
void TVect3<T>::Clip(double vmin, double vmax)
  {
  Assert(vmax >= vmin);
  Integra::Clip(x, vmin, vmax);
  Integra::Clip(y, vmin, vmax);
  Integra::Clip(z, vmin, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements lower than given minimum.
///
/// @param vmin  Lower bound of the vector elements.
/// @return True, if clipped, false otherwise.
template <class T>
bool TVect3<T>::ClipLower(double vmin)
  {
  bool ret = false;
  if (Integra::ClipLower(x, vmin))
    ret = true;
  if (Integra::ClipLower(y, vmin))
    ret = true;
  if (Integra::ClipLower(z, vmin))
    ret = true;
  return ret;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements higher than given maximum.
///
/// @param vmax  Top bound of the vector elements.
/// @return True, if clipped, false otherwise.
template <class T>
bool TVect3<T>::ClipHigher(double vmax)
  {
  bool ret = false;
  if (Integra::ClipHigher(x, vmax))
    ret = true;
  if (Integra::ClipHigher(y, vmax))
    ret = true;
  if (Integra::ClipHigher(z, vmax))
    ret = true;
  return ret;
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
TVect3<T> TVect3<T>::ValToRange(const T &vmin, const T &vmax) const
  {
  Assert(vmax >= vmin);
  return TVect3<T>(
    Integra::ValToRange(x, vmin, vmax),
    Integra::ValToRange(y, vmin, vmax),
    Integra::ValToRange(z, vmin, vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check of the elements of this vector - are they in given range.
///
/// Parameters of the method define the lower and upper boundaries of the some
/// range. Method returns true if all three coordinates of this vector are
/// satisfied of the conditions:
/// - coordinates of this vector less or equal than upper boundary @b and
/// - coordinates of this vector more or equal than lower boundary.
///
/// @param vmin - Minimal value of the range (lower bound).
/// @param vmax - Maximal value of the range (upper bound).
/// @return true if coordinates of this vector in specified range,
/// false - otherwise.
template <class T>
bool TVect3<T>::InRange(double vmin, double vmax) const
  {
  Assert(vmax >= vmin);
  return (x >= vmin && x <= vmax &&
          y >= vmin && y <= vmax &&
          z >= vmin && z <= vmax);
  }


//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of vector are valid floats.
template <class T>
bool TVect3<T>::IsOK() const
  {
  return (FloatIsOK(x) && FloatIsOK(y) && FloatIsOK(z));
  }

///////////////////////////////////////////////////////////////////////////////
/// Index of maximal element (by absolute value).
///
/// @return 0 - if first element of this vector is maximal by absolute value,
/// 1 - if second element is maximal by absolute value, 2 - otherwise (cf.
/// third element is maximal by absolute value).
template <class T>
int TVect3<T>::MaxElementIndex() const
  {
  if ((Abs(x) >= Abs(y)) && (Abs(x) >= Abs(z)))
    return 0;
  if (Abs(y) >= Abs(z))
    return 1;
  return 2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Index of minimal element (by absolute value).
///
/// @return 0 - if first element of this vector is minimal by absolute value,
/// 1 - if second element is minimal by absolute value, 2 - otherwise (cf.
/// third element is minimal by absolute value).
template <class T>
int TVect3<T>::MinElementIndex() const
  {
  if ((Abs(x) <= Abs(y)) && (Abs(x) <= Abs(z)))
    return 0;
  if (Abs(y) <= Abs(z))
    return 1;
  return 2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Absolute value of maximal element (by absolute value) of the vector.
///
/// @return Absolute value of maximal element (by absolute value) of the vector.
template <class T>
T TVect3<T>::MaxElement() const
  {
  return Max3(Abs(x), Abs(y), Abs(z));
  }

///////////////////////////////////////////////////////////////////////////////
/// Value of maximal element of the vector.
///
/// @return Value of maximal element of the vector.
template <class T>
T TVect3<T>::MaxVal() const
  {
  return Max3(x, y, z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Value of minimal element of the vector.
///
/// @return Value of minimal element of the vector.
template <class T>
T TVect3<T>::MinVal() const
  {
  return Min3(x, y, z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the given values.
///
/// Elements of this vector are replaced by the corresponding parameters
/// (double values) of the method.
/// @param x0 - New value for the first element of this vector.
/// @param y0 - New value for the second element of this vector.
/// @param z0 - New Value for the third element of this vector.
/// @return None (this vector is affected).
template <class T>
void TVect3<T>::Set(double x0, double y0, double z0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the same value.
///
/// The all elements of this vector are replaced by the parameter (double value)
/// of the method.
/// @param a - New value for the all elements of this vector.
/// @return None (this vector is affected).
template <class T>
void TVect3<T>::Set(double a)
  {
  x = y = z = (T)a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Squared length of this vector.
///
/// Squared length of the vector is sum of the squared coordinates.
/// @return Length of vector in second power.
template <class T>
double TVect3<T>::SqrLength() const
  {
  return x * x + y * y + z * z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of this vector.
///
/// Length of the vector is square root of the sum of the squared coordinates.
/// @return Length of vector.
template <class T>
double TVect3<T>::Length() const
  {
  return Sqrt(SqrLength());
  }

//////////////////////////////////////////////////////////////////////////////
/// Sum of the elements of the vector.
///
/// @return Value of the sum of the elements of this vector
template <class T>
T TVect3<T>::Sum() const
  {
  return (T)(x + y + z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Y (CIE) component of color.
///
/// Vector elements are treated as color components.
/// @return Value of the Y (CIE) component of color.
template <class T>
double TVect3<T>::CIESum() const
  {
  return x * CIE_SUM_VECT[0] + y * CIE_SUM_VECT[1] + z * CIE_SUM_VECT[2];
  }

///////////////////////////////////////////////////////////////////////////////
/// Projection of this vector to given vector.
///
/// Method creates a new 3D vector. Direction of new vector is equal to
/// direction of the given vector - if Dot product (see method
/// DotProd(const TVect3<T> &a, const TVect3<T> &b)) of this vector and
/// given vector is positive value and direction of new vector is opposite to
/// direction of the given vector - if  Dot product of this vector and
/// given vector is negative value. The length of the new vector is product of
/// the length of this vector and cosin of acute angle between this and given
/// vectors.
///
/// @image html project_3d.gif
///
/// @note Given vector may not be equal to zero vector. Debug version ASSERTs it.
///
/// @param u - Given vector (projective axis)
/// @return New 3D vector (projection)
template <class T>
TVect3<T> TVect3<T>::Project(const TVect3<T> &u) const
  {
  double sqr_len = DotProd(u, u);
  Assert(sqr_len > 0.0);
  return u * DotProd((*this), u) / sqr_len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Dot product of two given vectors, double and float.
///
/// @param a - First 3D vector - multiplier.
/// @param b - Second 3D vector - multiplier.
/// @return Value of Dot product.
inline double DotProdDF(const Vect3d &a, const Vect3f &b)
  {
  return a.x * b.x + a.y * b.y + a.z * b.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Normalize vector.
///
/// Method normalizes this vector and returns itself. Normalized vector has the
/// same direction as initial vector (before normalization) but length of the
/// normalized vector is equal to 1.
///
/// @note Zero vectors are not allowed, DEBUG version ASSERTs it.
/// @return Reference to this vector (normalized).
template <class T>
TVect3<T> & TVect3<T>::Normalize()
  {
  double len = Length();
  Assert(len != 0.0);
  x = (T)(x / len);
  y = (T)(y / len);
  z = (T)(z / len);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of the vector and normalization of the vector.
///
/// Method calculates length of this vector and if length of the vector is not
/// zero then normalizes this vector. Normalized vector has the same direction
/// as initial vector (before normalization) but length of the normalized
/// vector is equal to 1.
///
/// @note Zero vectors are allowed.
/// @return Length of this vector before normalization.
template <class T>
double TVect3<T>::ModNormalize()
  {
  double len = Length();
  if (len > 0.0)
    {
    x = (T)(x / len);
    y = (T)(y / len);
    z = (T)(z / len);
    }
  return len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculate the sum of the vector components and divide to this sum.
///
/// Method calculates sum of this vector components and if sum is not
/// zero then divides this vector ti the sum. Normalized vector has the same direction
/// as initial vector (before normalization) but sum of the normalized
/// vector components is equal to 1.
///
/// @note Zero vectors are allowed. In this case all 3 components are set to 1 /3. 
/// @return Sum of this vector components before normalization.
template <class T>
double TVect3<T>::SumNormalize()
  {
  double sum = Sum();
  if (sum != 0.0)
    {
    x = (T)(x / sum);
    y = (T)(y / sum);
    z = (T)(z / sum);
    }
  else
    x = y = z = (T)(1.0 / 3.0);
  return sum;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculate value of the maximal element of the vector and normalize vector
/// by the maximal value.
///
/// Method searchs absolute value of maximal element (by absolute value) of
/// this vector (see method MaxElement()) and if found value is not zero then
/// normalizes this vector by this value cf. divides the all elements of this
/// vector by this value. Resulted vector has the same direction as initial
/// vector (before normalization) but the all elements of this vector are
/// less or equal than 1.
///
/// @note Zero vectors are allowed, in this case all components of vector
/// are set to 1.
/// @return Absolute value of the maximal element (by absolute value) before
/// normalization.
template <class T>
double TVect3<T>::MaxNormalize()
  {
  double max = MaxElement();
  if (max != 0.0)
    {
    x = (T)(x / max);
    y = (T)(y / max);
    z = (T)(z / max);
    }
  else
    {
    x = 1;
    y = 1;
    z = 1;
    }
  return max;
  }

///////////////////////////////////////////////////////////////////////////////
/// CIE normalize - CIE sum is equal to 1 after normalization.
///
/// Method calculates CIE sum of this vector and if sum is not
/// zero then divides this vector to the sum.
///
/// @note Zero vectors are allowed.
/// @return CIE sum of this vector before normalization.
template <class T>
double TVect3<T>::CIENormalize()
  {
  double sum = CIESum();
  if (sum > 0.0)
    {
    x = (T)(x / sum);
    y = (T)(y / sum);
    z = (T)(z / sum);
    }
  return sum;
  }


///////////////////////////////////////////////////////////////////////////////
/// Test of vector normalization.
///
/// Vector is normalized if it's length is equal to 1. Method calculates
/// length of this vector and compares this length with 1.
///
/// @return true - if this vector is normalized, false - otherwise.
template <class T>
bool TVect3<T>::Normalized() const
  {
  return MathF::AboutEqual(Length(), 1.0);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector orientation to given vector.
///
/// Method calculates the Dot product (see function
/// DotProd(const TVect3<T> &a, const TVect3<T> &b)) of this vector with given
/// vector and if calculated value is negative then changes direction of this
/// vector to opposite (see method Negate()).
///
/// @param v - Given 3D vector.
/// @return None (this vector may be affected).
template <class T>
void TVect3<T>::Orient(const TVect3<T> &v)
  {
  if (DotProd(*this, v) < 0)
    Negate();
  }

//////////////////////////////////////////////////////////////////////////////
/// Sort vertices in ascending order.
template <class T>
void TVect3<T>::Sort()
  {
  if (x > y)
    Swap(x, y);
  if (y > z)
    Swap(y, z);
  if (x > y)
    Swap(x, y);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to array of three elements.
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
TVect3<T>::operator T *()
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to array of three elements.
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
TVect3<T>::operator const T *() const
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the three elements as a constant vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D constant vector.
template <class T>
const TVect3<T> &TVect3<T>::Cast(const T *u)
  {
  return *(const TVect3<T> *)u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the three elements as a vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D vector.
template <class T>
TVect3<T> &TVect3<T>::Cast(T *u)
  {
  return *(TVect3<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to point.
///
/// New point is not created (data is not changed), but contents of this
/// vector may be used as contents of the returned point.
///
/// @return Constant reference to 3D point.
template <class T>
const TPoint3<T> &TVect3<T>::Point() const
  {
  return TPoint3<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to point.
///
/// New point is not created (data is not changed), but contents of this
/// vector may be used as contents of the returned point.
///
/// @return Reference to 3D point.
template <class T>
TPoint3<T> &TVect3<T>::Point()
  {
  return TPoint3<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Cross product.
///
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
/// @image html cross_product.gif
///
/// @param[in] a - First 3D vector.
/// @param[in] b - Second 3D vector.
/// @return Cross product.
template <class T>
inline TVect3<T> CrossProd(const TVect3<T> &a, const TVect3<T> &b)
  {
  return TVect3<T>(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
  }
  
///////////////////////////////////////////////////////////////////////////////
/// Compose a vector from maximal components of 2 vectors.
/// @param[in] a - First 3D vector.
/// @param[in] b - Second 3D vector.
/// @return Vector consisting from maximal components of 2 vectors.
template <class T>
inline TVect3<T> Max(const TVect3<T> &a, const TVect3<T> &b)
  {
  return TVect3<T>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z));
  }
  
///////////////////////////////////////////////////////////////////////////////
/// Cosine of angle between two given vectors.
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
/// @param a - First 3D vector.
/// @param b - Second 3D vector.
/// @return Value of Cosine
template <class T>
inline double Cos(const TVect3<T> &a, const TVect3<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(DotProd(a, b) / Sqrt(SqrLength(a) * SqrLength(b)), -1, 1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sine of angle between two given vectors.
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
/// @param a - First 3D vector
/// @param b - Second 3D vector
/// @return Value of Sine
template <class T>
inline double Sin(const TVect3<T> &a, const TVect3<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(Sqrt(SqrLength(CrossProd(a, b)) /
                      (SqrLength(a) * SqrLength(b))),
                 0, 1);
  }

// -------------------------------------------------------------
// TPoint3<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
TPoint3<T>::TPoint3()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the scalar.
///
/// The point elements are initialized by given scalar.
/// @param v - Given scalar.
template <class T>
TPoint3<T>::TPoint3(const double v)
  {
  x = y = z = (T)v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the three scalars.
///
/// The point elements are initialized by @a x0, @a y0, @a z0 respectively.
///
/// @param x0 - Value for the first element of the point.
/// @param y0 - Value for the second element of the point.
/// @param z0 - Value for the third element of the point.
template <class T>
TPoint3<T>::TPoint3(const double x0, const double y0, const double z0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - 3D point - source for the initialization.
template <class T>
TPoint3<T>::TPoint3(const TPoint3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the point: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the point element.
/// @return Reference to the element of this point.
template <class T>
T & TPoint3<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the point: x (i = 0), y (i = 1), z (i = 2).
///
/// @note i must be in range 0..2. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the point element.
/// @return Constant reference to the element of this point.
template <class T>
const T &TPoint3<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 3));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Number of the elements of the point.
///
/// @return 3.
template <class T>
int TPoint3<T>::NComponents()
  {
  return 3;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 3D point.
/// @return true - if all three elements of this point are equal to the
/// corresponding elements of the given point, false - otherwise.
template <class T>
bool TPoint3<T>::operator == (const TPoint3<T> &u) const
  {
  return (x == u.x) && (y == u.y) && (z == u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 3D point.
/// @return true - if some element (or several elements together) of this
/// point is not equal to the corresponding element of the given point,
/// false - otherwise.
template <class T>
bool TPoint3<T>::operator != (const TPoint3<T> &u) const
  {
  return (x != u.x) || (y != u.y) || (z != u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 3D point.
/// @return true - if all three elements of this point are less or equal than
/// corresponding elements of the given point, false - otherwise.
template <class T>
bool TPoint3<T>::LessOrEqual(const TPoint3<T> &u) const
  {
#if 0
  return (x <= u.x) && (y <= u.y) && (z <= u.z);
#else
  return (z <= u.z) && (y <= u.y) && (x <= u.x);
#endif
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison by lexicographical order.
///
/// Comparison of two points. This method is different from other comparison
/// methods as it forms a lexicographical points order during sorting. It
/// means that it returns true immediately if first component of this point is
/// less than first component of the given point, or false if it is greater. If
/// first elements are equal, it compares second one and so on.
///
/// @param u - Given 3D point.
/// @return true - if this point less than given point in lexical sense,
/// false - otherwise.
template <class T>
bool TPoint3<T>::operator < (const TPoint3<T> &u) const
  {
#if 0
  if (x < u.x)
    return true;
  if (x > u.x)
    return false;
  if (y < u.y)
    return true;
  if (y > u.y)
    return false;
  return z < u.z;
#else
  if (z < u.z)
    return true;
  if (z > u.z)
    return false;
  if (y < u.y)
    return true;
  if (y > u.y)
    return false;
  return x < u.x;
#endif
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment of the given point to this point.
///
/// Elements of this point are replaced by the elements of the given point.
/// @param u - Given 3D point.
/// @return Reference to this point (updated).
template <class T>
TPoint3<T> & TPoint3<T>::operator = (const TPoint3<T> &u)
  {
  x = u.x;
  y = u.y;
  z = u.z;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given point to this point.
///
/// Elements of this point are replaced by the sums of corresponding elements
/// of this point and given point. In geometric sense this action is
/// summation of this point with the given point.
/// @param u - Given 3D point.
/// @return Reference to this point (the sum of the points).
template <class T>
TPoint3<T> & TPoint3<T>::operator += (const TPoint3<T> &u)
  {
  x += u.x;
  y += u.y;
  z += u.z;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given vector to this point.
///
/// Elements of this point are replaced by the sums of corresponding elements
/// of this point and given vector. In geometric sense this action is
/// translation of this point along of the given vector.
/// @param u - Given 3D vector.
/// @return Reference to this point (the sum of the point and vector).
template <class T>
TPoint3<T> & TPoint3<T>::operator += (const TVect3<T> &u)
  {
  x += u.x;
  y += u.y;
  z += u.z;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given vector from this point.
///
/// Elements of this point are replaced by the differences between
/// corresponding elements of this point and given vector. In geometric sense
/// this action is translation of this point along of the given vector in
/// opposite direction.
///
/// @param u - Given 3D vector.
/// @return Reference to this point (difference between point and vector).
template <class T>
TPoint3<T> & TPoint3<T>::operator -= (const TVect3<T> &u)
  {
  x -= u.x;
  y -= u.y;
  z -= u.z;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given scalar to elements of this point.
///
/// Elements of this point are replaced by the sums of these elements with
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this point (updated).
template <class T>
TPoint3<T> & TPoint3<T>::operator += (const double d)
  {
  x = (T)(x + d);
  y = (T)(y + d);
  z = (T)(z + d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given scalar from elements of this point.
///
/// Elements of this point are replaced by the differences between these
/// elements and given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this point (updated).
template <class T>
TPoint3<T> & TPoint3<T>::operator -= (const double d)
  {
  x = (T)(x - d);
  y = (T)(y - d);
  z = (T)(z - d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this point by given scalar.
///
/// Elements of this point are replaced by the products of these elements and
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this point (updated).
template <class T>
TPoint3<T> & TPoint3<T>::operator *= (const double d)
  {
  x = (T)(x * d);
  y = (T)(y * d);
  z = (T)(z * d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Point by scalar division.
///
/// Elements of this point are replaced by the results of division of
/// these elements by given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
///
/// @param d - Given scalar.
/// @return Reference to this point (updated).
template <class T>
TPoint3<T> &TPoint3<T>::operator /= (const double d)
  {
  Assert(d != 0.0);
  x = (T)(x / d);
  y = (T)(y / d);
  z = (T)(z / d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition with a weight.
///
/// Elements of this point are replaced by the sums of these elements with
/// products of the given vector elements and weight value.
///
/// @param u - Given 3D vector.
/// @param w - Weight value.
/// @return  Reference to this point (updated).
template <class T>
TPoint3<T> &TPoint3<T>::AddWithWeight(const TVect3<T> &u,
                                             const double w)
  {
  x = (T)(x + u.x * w);
  y = (T)(y + u.y * w);
  z = (T)(z + u.z * w);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Negation of this point.
///
/// The signs of the all elements of this point are changed to opposite. As a
/// result the new position of this point is symmetric to old position with
/// respect to center (cf. point {0;0;0}) of coordinate system.
/// @return None (this point is affected).
template <class T>
void TPoint3<T>::Negate()
  {
  x = -x;
  y = -y;
  z = -z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the opposite point.
///
/// Method creates a new 3D point. Elements of a new point are negative with
/// respect to elements of this point. As a result the new point is symmetric
/// to this point with respect to center (cf. point {0;0;0}) of coordinate
/// system.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator -() const
  {
  return TPoint3<T>(-x, -y, -z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by addition of the given point to point.
/// @param u Given 3D point.
/// @return New 3D point
template <class T>
TPoint3<T> TPoint3<T>::operator + (const TPoint3<T> &u) const
  {
  return TPoint3<T>(x + u.x, y + u.y, z + u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by addition of the given vector to point.
///
/// Method creates a new 3D point. Elements of a new point are the sums of
/// corresponding elements of this point and given vector. In geometric sense
/// this action is translation of this point along of the given vector.
///
/// @param u - Given 3D vector.
/// @return New 3D point
template <class T>
TPoint3<T> TPoint3<T>::operator + (const TVect3<T> &u) const
  {
  return TPoint3<T>(x + u.x, y + u.y, z + u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by subtraction of the given vector from point.
///
/// Method creates a new 3D point. Elements of a new point are the differences
/// between corresponding elements of this point and given vector. In geometric
/// sense this action is translation of this point along of the given vector in
/// the opposite (with respect to vector direction) direction.
///
/// @param u - Given 3D vector.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator - (const TVect3<T> &u) const
  {
  return TPoint3<T>(x - u.x, y - u.y, z - u.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the vector by subtraction of the given point from this
/// point.
///
/// Method creates a new 3D vector. Elements of a new vector are the differences
/// between corresponding elements of this point and given point. In geometric
/// sense the given point is begin of the new vector, this point is end of the
/// new vector.
///
/// @param p - Given 3D point.
/// @return New 3D vector.
template <class T>
TVect3<T> TPoint3<T>::operator - (const TPoint3<T> &p) const
  {
  return TVect3<T>(x - p.x, y - p.y, z - p.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by addition a scalar value to elements of this point.
///
/// Method creates a new 3D point. Elements of a new point are the sums of the
/// corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator + (const double d) const
  {
  return TPoint3<T>(x + d, y + d, z + d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by subtraction a scalar value from elements of  this
/// point.
///
/// Method creates a new 3D point. Elements of a new point are the differences
/// between corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator - (const double d) const
  {
  return TPoint3<T>(x - d, y - d, z - d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by the multiplication a scalar value to elements of  this
/// point.
///
/// Method creates a new 3D point. Elements of a new point are the products of
/// corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator * (const double d) const
  {
  return TPoint3<T>(x * d, y * d, z * d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by the division of the elements of this point by the scalar
/// value.
///
/// Method creates a new 3D point. Elements of a new point are the results of
/// the division corresponding elements of this point by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
/// @param d - Given scalar.
/// @return New 3D point.
template <class T>
TPoint3<T> TPoint3<T>::operator / (const double d) const
  {
  Assert(d != 0);
  return TPoint3<T>(x / d, y / d, z / d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements of the point.
///
/// Point elements clipped by the given values in the following sense.
/// Parameters of the method define lower and upper boundary of the elements.
/// If first element of this point more than upper boundary then first element
/// is replaced by the upper boundary. If first element of this point less
/// than lower boundary then first element is replaced by the lower boundary.
/// Second and third elements of this point may be modified by this method
/// analogously.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin - lower bound of the point elements.
/// @param vmax - upper bound of the point elements.
/// @return None (only this point is affected).
template <class T>
void TPoint3<T>::Clip(double vmin, double vmax)
  {
  Assert(vmax >= vmin);
  Integra::Clip(x, vmin, vmax);
  Integra::Clip(y, vmin, vmax);
  Integra::Clip(z, vmin, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements lower than given minimum.
///
/// @param vmin - Lower bound of the vector elements.
template <class T>
bool TPoint3<T>::ClipLower(double vmin)
  {
  bool ret = false;
  if (Integra::ClipLower(x, vmin))
    ret = true;
  if (Integra::ClipLower(y, vmin))
    ret = true;
  if (Integra::ClipLower(z, vmin))
    ret = true;
  return ret;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip point elements to given range.
///
/// Point elements are clipped to given range.
/// Cliped point is returned.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin Lower bound of the point elements.
/// @param vmax Upper bound of the point elements.
/// @return Clipped point.
///
template <class T>
TPoint3<T> TPoint3<T>::ValToRange(const T &vmin, const T &vmax) const
  {
  Assert(vmax >= vmin);
  return TPoint3<T>(
    Integra::ValToRange(x, vmin, vmax),
    Integra::ValToRange(y, vmin, vmax),
    Integra::ValToRange(z, vmin, vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the point elements to the given values.
///
/// Elements of this point are replaced by the corresponding parameters (double
/// values) of the method.
/// @param x0 - New value for the first element of this point.
/// @param y0 - New value for the second element of this point.
/// @param z0 - New value for the third element of this point.
/// @return None (this point is affected).
template <class T>
void TPoint3<T>::Set(double x0, double y0, double z0)
  {
  x = (T)x0;
  y = (T)y0;
  z = (T)z0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the point elements to the same value.
///
/// The all elements of this point are replaced by the parameter (double value)
/// of the method.
/// @param a - New value for the all point elements.
/// @return None (this point is affected).
template <class T>
void TPoint3<T>::Set(double a)
  {
  x = y = z = (T)a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Point conversion to the array of the three elements.
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
TPoint3<T>::operator T *()
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Point conversion to the array of the three elements.
///
/// Usage of this operator as
/// @code
/// (T *)u
/// @endcode
/// returns constant pointer to the first element of the array. T is type of
/// the elements of this array.
///
/// @return A constant pointer to the first element of array.
template <class T>
TPoint3<T>::operator const T *() const
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of three elements as a constant point.
///
/// New point is not created (data is not changed), but contents of the array
/// may be used as elements of the returned point.
///
/// @return Reference to the 3D constant point.
template <class T>
const TPoint3<T> &TPoint3<T>::Cast(const T *u)
  {
  return *(const TPoint3<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of three elements as a point.
///
/// New point is not created (data is not changed), but contents of the array
/// may be used as elements of the returned point.
///
/// @return Reference to the 3D point.
template <class T>
TPoint3<T> &TPoint3<T>::Cast(T *u)
  {
  return *(TPoint3<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the point as constant vector.
///
/// New vector is not created (data is not changed), but elements of the point
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D constant vector.
template <class T>
const TVect3<T> &TPoint3<T>::Vector() const
  {
  return TVect3<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the point as vector.
///
/// New vector is not created (data is not changed), but elements of the point
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D vector.
template <class T>
TVect3<T> &TPoint3<T>::Vector()
  {
  return TVect3<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the segment defined by the two given points.
///
/// Center of the segment is point. Elements of this point are the average
/// values of the corresponding elements of the two points in the begin and end
/// of the segment.
///
/// @param a - First 3D point (begin of the segment).
/// @param b - Second 3D point (end of the segment).
/// @return Center of the segment.
template <class T>
inline TPoint3<T> Center(const TPoint3<T> &a, const TPoint3<T> &b)
  {
  return ((a.Vector() + b.Vector()) / 2.0).Point();
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the triangle defined by the three points.
///
/// Center of the triangle is point. Elements of this point are the average
/// values of the corresponding elements of the three points - vertices of the
/// triangle.
///
/// @param a - First 3D point (first vertex of the triangle).
/// @param b - Second 3D point (second vertex of the triangle).
/// @param c - Third  3D point (third vertex of the triangle).
/// @return Center of the triangle.
template <class T>
inline TPoint3<T> Center(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c)
  {
  return ((a.Vector() + b.Vector() + c.Vector()) / 3.0).Point();
  }

///////////////////////////////////////////////////////////////////////////////
/// Area of the triangle.
///
/// Triangle is defined by the given three points - parameters of the method.
/// Area of the triangle is half of the length of the cross product vector.
/// (see function CrossProd()). Vectors - multipliers are the triangle sides
/// with common begin - any triangle vertex.
///
/// @param a - First 3D point (first vertex of the triangle).
/// @param b - Second 3D point (second vertex of the triangle).
/// @param c - Third  3D point (third vertex of the triangle).
/// @return Area of the triangle.
template <class T>
inline double TrgArea(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c)
  {
  return Length(CrossProd(a - b, a - c)) / 2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Angle of the triangle vertex.
///
/// Triangle is defined by the given three points, the first point is a
/// queried vertex.
///
/// @param a - First 3D point (first vertex of the triangle).
/// @param b - Second 3D point (second vertex of the triangle).
/// @param c - Third  3D point (third vertex of the triangle).
/// @return Angle of the vertex.
template <class T>
inline double VertAngle(const TPoint3<T> &a, const TPoint3<T> &b,
                         const TPoint3<T> &c)
  {
  TVect3<T> v1 = b - a;
  TVect3<T> v2 = c - a;
  double sin_ang = Length(CrossProd(v1, v2));
  double cos_ang = DotProd(v1, v2);
  return atan2(sin_ang, cos_ang);
  }

///////////////////////////////////////////////////////////////////////////////
/// Barycentric coordinates of the point in triangle. 
/// This version calculates one negative bary coord for point out of triangle.
///
/// Triangle is defined by the given three points - the first three parameters
/// of the method. Let (p1, p2, p3) is triangle defined by the vertices
/// p1, p2, p3 and @b p is arbitrary point. Barycentric coordinates of the
/// point @b p are the areas of the triangles (p, p2, p3), (p, p3, p1),
/// (p, p1, p2) normalized by the area of the triangle (p1, p2, p3).
/// Instead of areas of triangles - DotProds of CrossProds are used.
///
/// @image html bary_coord.gif
///
/// @param p1 - First 3D point (first vertex of the triangle).
/// @param p2 - Second 3D point (second vertex of the triangle).
/// @param p3 - Third  3D point (third vertex of the triangle).
/// @param p  - Given point
/// @return  Vector of the barycentric coordinates of given point.
template <class T>
inline Vect3d BaryCoords(const TPoint3<T> &p1, const TPoint3<T> &p2,
                         const TPoint3<T> &p3, const TPoint3<T> &p)
  {
  TVect3<T> cp0 = CrossProd(p2 - p1, p3 - p1); // Whole triangle.
  TVect3<T> cp1 = CrossProd(p2 - p, p3 - p); // Triangle opposite to p1.
  TVect3<T> cp2 = CrossProd(p3 - p, p1 - p); // Triangle opposite to p2.
  TVect3<T> cp3 = CrossProd(p1 - p, p2 - p); // Triangle opposite to p3.
  double dp0 = DotProd(cp0, cp0); 
  Vect3d bary(DotProd(cp1, cp0) / dp0, DotProd(cp2, cp0) / dp0, DotProd(cp3, cp0) / dp0);

  return bary;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the vector by subtraction of 2 points of different types.
///
/// Method creates a new 3D vector. Elements of a new vector are the differences
/// between corresponding elements of this point and given point. In geometric
/// sense the given point is begin of the new vector, this point is end of the
/// new vector.
///
/// @param p - Given 3D point.
/// @return New 3D vector.
inline Vect3d operator - (const Point3d &p1, const Point3f &p2) 
  {
  return Vect3d(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
  }

// -------------------------------------------------------------
// BBox3<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor without initialization.
template <class T>
BBox3<T>::BBox3()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by one point.
///
/// Lower (@a vmin) and upper (@a vmax) boundaries of the box are initialized
/// by the same 3D  point. In result the box is one point only (see method
/// IsDot()).
/// @param point - 3D point.
template <class T>
BBox3<T>::BBox3(const TPoint3<T> &point)
  {
  vmin = vmax = point;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by the two points.
///
/// Lower (@a vmin) and upper (@a vmax) boundaries of a box are initialized by
/// the  given points.
/// @param point1 - 3D point defines lower boundary.
/// @param point2 - 3D point defines upper boundary.
template <class T>
BBox3<T>::BBox3(const TPoint3<T> &point1, const TPoint3<T> &point2)
  {
  vmin = point1; 
  vmax = point2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param box - 3D bounding box - source for this box creation.
template <class T>
BBox3<T>::BBox3(const BBox3<T> &box)
  {
  vmin = box.vmin;
  vmax = box.vmax;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from one point.
/// @param point Lower and upper boundary.
template <class T>
void BBox3<T>::Init(const TPoint3<T> &point)
  {
  vmin = vmax = point;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from two points.
/// @param min Lower boundary.
/// @param max Upper boundary.
template <class T>
void BBox3<T>::Init(const TPoint3<T> &min, const TPoint3<T> &max)
  {
  vmin = min;
  vmax = max;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Parameters of this bounding box (upper @a vmax and lower @a vmin boundary)
/// are replaced by the parameters of the given box.
///
/// @param box - Given 3D bounding box.
/// @return Reference to this box (updated).
template <class T>
BBox3<T> & BBox3<T>::operator = (const BBox3<T> &box)
  {
  vmin = box.vmin;
  vmax = box.vmax;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for equality.
///
/// This box is equal to given box if boundaries (points @a vmin, @a vmax) of
/// this box are equal to boundaries of the given box.
/// @param box - Given 3D box.
/// @return true - if this bounding box is equal to given box, false -
/// otherwise.
template <class T>
bool BBox3<T>::operator == (const BBox3<T> &box) const
  {
  return (vmin == box.vmin) && (vmax == box.vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for non-equality.
///
/// This box is not equal to given box if some boundary of this box (point
/// @a vmin or @a vmax or both points simultaneously) is not equal to
/// corresponding boundary of the given box.
/// @param box - Given 3D box.
/// @return true - if this bounding box is not equal to given box, false -
/// otherwise.
template <class T>
bool BBox3<T>::operator != (const BBox3<T> &box) const
  {
  return (vmin != box.vmin) || (vmax != box.vmax);
  }


///////////////////////////////////////////////////////////////////////////////
/// Check that box is not empty.
///
/// Bounding box is not empty if there are points (even if one point) inside
/// box. In this case lower boundary (@a vmin) of a box is less or equal than
/// upper boundary (@a vmax) cf. box is not empty if coordinates
/// of the @a vmin point are less or equal than coordinates of @a vmax point.
///
/// @return true - if box is not empty, false - otherwise.
template <class T>
bool BBox3<T>::NotEmpty() const
  {
  return (vmin.x <= vmax.x) && (vmin.y <= vmax.y) && (vmin.z <= vmax.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box is empty.
///
/// Bounding box is empty if there are no points inside box.
///
/// @return true - if box is empty, false - otherwise.
template <class T>
bool BBox3<T>::IsEmpty() const
  {
  return (vmin.x > vmax.x) || (vmin.y > vmax.y) || (vmin.z > vmax.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Set empty BBox.
template <class T>
void BBox3<T>::SetEmpty()
  {
  vmin.x = vmin.y = vmin.z = 1;
  vmax.x = vmax.y = vmax.z = -1;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box contains one  point only.
///
/// Bounding box contains one  point only if upper boundary (@a vmax) of the
/// box is equal to the lower boundary (@a vmin) of the box. Therefore box is
/// one point if coordinates of the @a vmin point are equal to the coordinates
/// of @a vmax point. In this case the rectangular parallelepiped is degenerate
/// cf. it is dot: @a vmin = @a vmax.
///
/// @return true - if bounding box is dot, false - otherwise.
template <class T>
bool BBox3<T>::IsDot() const
  {
  return vmin == vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given point.
///
/// This bounding box includes the given point if coordinates of the given
/// point (x, y, z) is in ranges defined by the lower (@a vmin) and upper
/// (@a vmax) boundaries of this box cf. coordinates of the point are less or
/// equal than coordinates of the point @a vmax @b and more or equal than
/// coordinates of the point @a vmin:
/// - @a vmin.x <= @a x <= @a vmax.x;
/// - @a vmin.y <= @a y <= @a vmax.y.
/// - @a vmin.z <= @a z <= @a vmax.z.
///
/// @param point - Given 3D point.
/// @return true - if given point is inside of this box, false - otherwise.
template <class T>
bool BBox3<T>::Includes(const TPoint3<T> &point) const
  {
  return (vmin.LessOrEqual(point)) && (point.LessOrEqual(vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given box.
///
/// This bounding box includes the given box if this box includes the both
/// boundaries (@a vmin and @a vmax) of the given box (see method Includes()
/// for the point).
///
/// @param box - Given 3D box (rectangular parallelepiped).
/// @return true - if given box is inside of this box, false - otherwise.
template <class T>
bool BBox3<T>::Includes(const BBox3<T> &box) const
  {
  return (vmin.LessOrEqual(box.vmin)) && (box.vmax.LessOrEqual(vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box intersects the given box.
///
/// This bounding box intersects the given box if both boxes (this and given)
/// contain the common points.
///
/// @param box - Given 3D box (rectangular parallelepiped).
/// @return true - if this box intersects the given box, false - otherwise.
template <class T>
bool BBox3<T>::Intersects(const BBox3<T> &box) const
  {
  return (vmin.LessOrEqual(box.vmax)) && (box.vmin.LessOrEqual(vmax));
  }


///////////////////////////////////////////////////////////////////////////////
/// Extension of the box by inclusion of the given point.
///
/// This bounding box is not changed if given point is inside of this box (see
/// method Includes()). Otherwise this box is extended to include the given
/// point. Resulted bounding box is minimal box which includes the initial this
/// box and given point.
///
/// @param point - Given 3D point.
/// @return None (this box may be affected).
template <class T>
void BBox3<T>::Include(const TPoint3<T> &point)
  {
  if (point.x < vmin.x)
    vmin.x = point.x;
  if (vmax.x < point.x)
    vmax.x = point.x;
  if (point.y < vmin.y)
    vmin.y = point.y;
  if (vmax.y < point.y)
    vmax.y = point.y;
  if (point.z < vmin.z)
    vmin.z = point.z;
  if (vmax.z < point.z)
    vmax.z = point.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Extension of the box by inclusion of the given box.
///
/// This bounding box is not changed if both boundaries (points @a vmin and
/// @a vmax) of the given box are inside of this box (see  method Includes()).
/// Otherwise this box is extended to include the given box.
/// Resulted bounding box is minimal box which includes the initial this box and
/// given box.
///
/// As this box as included one may be empty.
///
/// @param box - Given 3D box.
/// @return None (this box may be affected).
template <class T>
void BBox3<T>::Include(const BBox3<T> &box)
  {
  if (box.IsEmpty())
    return;
  if (this->IsEmpty())
    {
    (*this) = box;
    return;
    }
  if (vmin.x > box.vmin.x)
    vmin.x = box.vmin.x;
  if (vmax.x < box.vmax.x)
    vmax.x = box.vmax.x;
  if (vmin.y > box.vmin.y)
    vmin.y = box.vmin.y;
  if (vmax.y < box.vmax.y)
    vmax.y = box.vmax.y;
  if (vmin.z > box.vmin.z)
    vmin.z = box.vmin.z;
  if (vmax.z < box.vmax.z)
    vmax.z = box.vmax.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Intersection with given box.
///
/// If this bounding box is intersected by the given box then result of the
/// intersection (new bounding box) is placed into this box. If this bounding
/// box is not intersected by the given box (see method Intersects()) then this
/// box is not changed.
///
/// @param box - Given 3D box.
/// @return none (this box may be affected).
template <class T>
void BBox3<T>::Intersect(const BBox3<T> &box)
  {
  if (vmin.x < box.vmin.x)
    vmin.x = box.vmin.x;
  if (vmax.x > box.vmax.x)
    vmax.x = box.vmax.x;
  if (vmin.y < box.vmin.y)
    vmin.y = box.vmin.y;
  if (vmax.y > box.vmax.y)
    vmax.y = box.vmax.y;
  if (vmin.z < box.vmin.z)
    vmin.z = box.vmin.z;
  if (vmax.z > box.vmax.z)
    vmax.z = box.vmax.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Translation of the box by the given vector.
///
/// This bounding box is modified by addition of the  given 3D vector to
/// the lower and upper boundaries (3D points @a vmin and @a vmax with minimal
/// and maximal coordinates) of this box. In fact it is shift of the rectangular
/// parallelepiped in 3D space along of the given vector.
///
/// @param vct - Given 3D vector.
/// @return None (this box is affected).
template <class T>
void BBox3<T>::Translate(const TVect3<T> &vct)
  {
  vmin += vct; vmax += vct;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the box by the translation of this box.
///
/// New bounding box is created  by addition of the given vector to the
/// lower and upper boundaries (3D points @a vmin and @a vmax with minimal and
/// maximal coordinates) of this box. In fact it is shift of the rectangular
/// parallelepiped in 3D space along of the given vector.
///
/// @param vct - Given 3D vector.
/// @return Created bounding box.
template <class T>
BBox3<T> BBox3<T>::Translated(const TVect3<T> &vct) const
  {
  return BBox3<T>(vmin + vct, vmax + vct);
  }

///////////////////////////////////////////////////////////////////////////////
/// Diagonal of the box.
///
/// Diagonal is vector created by the two points. Begin of this vector is point
/// with minimal coordinates of the box @a vmin. End of this vector is point
/// with maximal coordinates of the box @a vmax.
///
/// @return 3D vector of the diagonal.
template <class T>
TVect3<T> BBox3<T>::Diag() const
  {
  return vmax - vmin;
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the box.
///
/// Center of the box is 3D point. Coordinates of this point are the average
/// values of the boundaries of the box (3D points @a vmin and @a vmax with
/// minimal and maximal coordinates).
///
/// @return 3D point of the box center.
template <class T>
TPoint3<T> BBox3<T>::Center() const
  {
  return Integra::Center(vmax, vmin);
  }

///////////////////////////////////////////////////////////////////////////////
/// Volume of the box.
///
/// Volume of the box is product of the all box sizes (coordinates of the
/// diagonal (3D vector) of the box (see method Diag()).
///
/// @return Value of the box volume.
template <class T>
double BBox3<T>::Volume() const
  {
  return (vmax.x - vmin.x) * (vmax.y - vmin.y) * (vmax.z - vmin.z);
  }

// -------------------------------------------------------------
// Implementation of Math3<T> methods
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with given tolerance.
///
/// @param v - Given 3D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than given tolerance, false - otherwise.
template <class T>
inline bool Math3<T>::AboutZero(const TVect3<T> &v, const double tolerance)
  {
  return Math<T>::AboutZero(v.x, tolerance) &&
         Math<T>::AboutZero(v.y, tolerance) &&
         Math<T>::AboutZero(v.z, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Given 3D vector.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than tolerance Math<T>::TOLERANCE, false - otherwise.
template <class T>
inline bool Math3<T>::AboutZero(const TVect3<T> &v)
  {
  return AboutZero(v, Math3<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with predefined tolerance ::EPSILON.
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v - Given 3D vector.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than tolerance ::EPSILON, false - otherwise.
template <class T>
inline bool Math3<T>::NearZero(const TVect3<T> &v)
  {
  return AboutZero(v, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with given tolerance.
///
/// @param v - Given 3D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than given tolerance, false - otherwise.
template <class T>
inline bool Math3<T>::AboutZero(const TPoint3<T> &v, const double tolerance)
  {
  return Math<T>::AboutZero(v.x, tolerance) &&
         Math<T>::AboutZero(v.y, tolerance) &&
         Math<T>::AboutZero(v.z, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Given 3D point.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than tolerance Math<T>::TOLERANCE, false - otherwise.
template <class T>
inline bool Math3<T>::AboutZero(const TPoint3<T> &v)
  {
  return AboutZero(v, Math3<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with predefined tolerance ::EPSILON.
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v - Given 3D point.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than tolerance ::EPSILON, false - otherwise.
template <class T>
inline bool Math3<T>::NearZero(const TPoint3<T> &v)
  {
  return AboutZero(v, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with given tolerance.
///
/// Method calculates difference between given vectors and result (3D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 3D vector.
/// @param v2 - Second 3D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than given tolerance, false -
/// otherwise.
template <class T>
inline bool Math3<T>::AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2,
                                 const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given vectors and result (3D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 3D vector.
/// @param v2 - Second 3D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
inline bool Math3<T>::AboutEqual(const TVect3<T> &v1, const TVect3<T> &v2)
  {
  return AboutZero(v1 - v2, Math3<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance ::EPSILON.
///
/// Method calculates difference between given vectors and result (3D vector)
/// compares with tolerance ::EPSILON (see method NearZero()).
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v1 - First 3D vector.
/// @param v2 - Second 3D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance ::EPSILON,
/// false - otherwise.
template <class T>
inline bool Math3<T>::NearEqual(const TVect3<T> &v1, const TVect3<T> &v2)
  {
  return AboutZero(v1 - v2, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with given tolerance.
///
/// Method calculates difference between given points and result (3D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 3D point.
/// @param v2 - Second 3D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than given tolerance, false -
/// otherwise.
template <class T>
inline bool Math3<T>::AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2,
                                 const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given points and result (3D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 3D point.
/// @param v2 - Second 3D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
inline bool Math3<T>::AboutEqual(const TPoint3<T> &v1, const TPoint3<T> &v2)
  {
  return AboutZero(v1 - v2, Math3<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance ::EPSILON.
///
/// Method calculates difference between given points and result (3D vector)
/// compares with tolerance ::EPSILON (see method NearZero()).
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v1 - First 3D point.
/// @param v2 - Second 3D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance ::EPSILON,
/// false - otherwise.
template <class T>
inline bool Math3<T>::NearEqual(const TPoint3<T> &v1, const TPoint3<T> &v2)
  {
  return AboutZero(v1 - v2, EPSILON);
  }

INTEGRA_NAMESPACE_END
#endif

