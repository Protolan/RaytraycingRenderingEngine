/// @file
///
/// @brief Definition of template class for 2D vectors and points
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///         Mip - Ivan Polykov<BR>

#ifndef _KLBC_VECT2_HPP_
#define _KLBC_VECT2_HPP_

#include <base/base.hpp>

#include "math.hpp"

INTEGRA_NAMESPACE_START

// Forward references
template <class T> class BBox2;
template <class T> class BVect2;
template <class T> class Math2;
template <class T> class TMatrix2;
template <class T> class TPoint2;
template <class T> class TVect2;

/// @brief Class BVect2 is set of the two elements of arbitrary type.
/// Arithmetic operations are not provided by this class.
template <class T>
class BVect2
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector component
    T x;
    /// Second vector component
    T y;

  public:
    /// @name Constructors.
    //@{
    /// Default constructor, no initialization.
    inline BVect2();
    /// Constructor from scalar value.
    inline BVect2(const double v);
    /// Constructor from given elements.
    inline BVect2(const double x0, const double y0);
    /// Copy constructor.
    inline BVect2(const BVect2<T> &u);
    //@}

    /// @name  Access to the vector elements.
    //@{
    /// Access to i-th element of the vector: x (i = 0) or y (i = 1).
    inline T & operator [](int i);
    /// Access to i-th element of the vector: x (i = 0) or y (i = 1).
    inline const T &operator [](int i) const;
    //@}

    /// @name Assignments.
    //@{
    /// Assignment operator.
    inline BVect2<T> & operator = (const BVect2<T> &u);
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const BVect2<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const BVect2<T> &u) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 2D sets.
    //@{
    /// Serialize the vector using given default.
    void Serialize(Serializer &inout, const char *tag, const BVect2<T> &dval = 0);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Conversion of the vector to the array of two elements.
    inline operator T *();
    /// Conversion of the vector to the array of two elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of two elements as a vector.
    inline static const BVect2<T> &Cast(const T *u);
    /// Treatment of the array of two elements as a vector.
    inline static BVect2<T> &Cast(T *u);
    //@}

    /// @name Number of the vector elements.
    //@{
    /// Number of the vector elements.
    static int NComponents();
    //@}
  };  // class BVect2<T>

/// 2D vector of arbitrary type.
template <class T>
class TVect2
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// First vector component
    T x;
    /// Second vector component
    T y;

  public:
    /// @name Constructors.
    //@{
    /// Constuctor by the default, no initialization of elements is performed.
    inline TVect2();
    /// Constructor from scalar.
    inline TVect2(const double v);
    /// Constructor from given components.
    inline TVect2(const double x0, const double y0);
    /// Constructor from two points.
    inline TVect2(const TPoint2<T> &from, const TPoint2<T> &to);
    /// Create vector from  point.
    inline TVect2(const TPoint2<T> &p);
    /// Copy constructor.
    inline TVect2(const TVect2<T> &u);
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th element of the vector: x (i = 0) or y (i = 1).
    inline T & operator [](int i);
    /// Access to the i-th element of the vector: x (i = 0) or y (i = 1).
    inline const T &operator [](int i) const;
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator == (const TVect2<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool operator != (const TVect2<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool LessOrEqual(const TVect2<T> &u) const;
    /// Element-by-element comparison of this vector with given vector.
    inline bool Less(const TVect2<T> &u) const;
    /// @brief Comparison by lexicographical order.
    inline bool operator < (const TVect2<T> &u) const;
    /// Check that all elements of vector are valid floats.
    inline bool IsOK() const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given vector to this vector.
    inline TVect2<T> & operator = (const TVect2<T> &u);

    // Vector-to-vector operators.

    /// Element-by-element vector addition of the given vector to this vector.
    inline TVect2<T> & operator += (const TVect2<T> &u);
    /// Element-by-element vector subtraction of the given vector from this vector.
    inline TVect2<T> & operator -= (const TVect2<T> &u);
    /// Element-by-element vector multiplication of this vector by given vector.
    inline TVect2<T> & operator *= (const TVect2<T> &u);
    /// Element-by-element vector division of this vector by given vector.
    inline TVect2<T> & operator /= (const TVect2<T> &u);
    // Vector-to-scalar operators.

    /// Addition of the given scalar to elements of this vector.
    inline TVect2<T> & operator += (const double d);
    /// Subtraction of the given scalar from elements of this vector.
    inline TVect2<T> & operator -= (const double d);
    /// Multiplication of this vector by given scalar.
    inline TVect2<T> & operator *= (const double d);
    /// Vector by scalar division.
    inline TVect2<T> & operator /= (const double d);
    /// Addition with a weight.
    inline TVect2<T> &AddWithWeight(const TVect2<T> &u, const double w);
    //@}

    /// @name Unary operators.
    //@{
    /// Negation of this vector.
    inline void Negate();
    /// Creation of the opposite vector.
    inline TVect2<T> operator -() const;
    //@}

    /// @name Binary operators.
    //@{
    // Vector-to-vector operators.

    /// @brief Vector creation by the element-by-element addition of the given
    /// vector to this vector.
    inline TVect2<T> operator + (const TVect2<T> &u) const;
    /// @brief Vector creation by the element-by-element subtraction a given
    /// vector from this vector.
    inline TVect2<T> operator - (const TVect2<T> &u) const;
    /// @brief Vector creation by the element-by-element multiplication a given
    /// vector to this vector.
    inline TVect2<T> operator * (const TVect2<T> &u) const;
    /// @brief Vector creation by the element-by-element division of this
    /// vector by given vector.
    inline TVect2<T> operator / (const TVect2<T> &u) const;

    // Vector-to-scalar operators.

    /// @brief Vector createion by addition a scalar value to elements of
    /// this vector.
    inline TVect2<T> operator + (const double d) const;
    /// @brief Vector creation by subtraction a scalar value from elements
    /// of  this vector.
    inline TVect2<T> operator - (const double d) const;
    /// @brief Vector creation by the multiplication a scalar value to elements
    /// of  this vector.
    inline TVect2<T> operator * (const double d) const;
    /// @brief Vector creation by the division elements of this vector by the
    /// scalar value.
    inline TVect2<T> operator / (const double d) const;

    // Vector-to-matrix operators.

    /// Multiplication of this vector by the given matrix.
    TVect2<T> operator * (const TMatrix2<T> &v) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 2D vectors.
    //@{
    /// Serialize the vector using given default.
    void Serialize(Serializer &inout, const char *tag, const TVect2<T> &dval = 0);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Other methods.
    //@{
    /// Clipping elements of the vector.
    inline void Clip(const double vmin, const double vmax);
    /// Clipping elements lower than given minimum.
    inline bool ClipLower(double vmin);
    /// Clipping elements higher than given maximum.
    inline bool ClipHigher(double vmax);
    /// Clip vector elements to given range.
    inline TVect2<T> ValToRange(const T vmin, const T vmax) const;
    /// Check of the elements of this vector - are they in given range.
    inline bool InRange(double vmin, double vmax) const;
    /// Index of maximal element (by absolute value).
    inline int MaxElementIndex() const;
    /// Absolute value of maximal element (by absolute value) of the vector.
    inline T MaxElement() const;
    /// Set of the vector elements to the given values.
    inline void Set(double x0, double y0);
    /// Set of the vector elements to the same value.
    inline void Set(double a);
    /// Squared length of this vector.
    inline double SqrLength() const;
    /// Length of this vector.
    inline double Length() const;
    /// Sum of the elements of the vector.
    inline T Sum() const;
    /// Projection of this vector to given vector.
    inline TVect2<T> Project(const TVect2<T> &u) const;
    /// Any unit orthogonal vector to given vector.
    TVect2<T> AnyOrthogonal() const;
    /// Normalized vector.
    inline TVect2<T> &Normalize();
    /// Normalization of the vector.
    inline double ModNormalize();
    /// Calculate the sum of the vector components and divide to this sum.
    inline double SumNormalize();
    /// @brief Calculate value of the maximal element of the vector and normalize
    /// the vector by the maximal value.
    inline double MaxNormalize();
    /// Test of vector normalization.
    inline bool Normalized() const;
    /// Vector orientation to given vector.
    inline void Orient(const TVect2<T> &v);
    //@}

    /// @name Vector to array conversion.
    //@{
    /// Vector conversion to array of two elements.
    inline operator T *();
    /// Vector conversion to array of two elements.
    inline operator const T *() const;
    //@}

    /// @name Array to vector conversion.
    //@{
    /// Treatment of the array of two elements as a constant vector.
    inline static const TVect2<T> &Cast(const T *u);
    /// Treatment of the array of two elements as a vector.
    inline static TVect2<T> &Cast(T *u);
    //@}

    /// @name Vector to point conversion.
    /// Both methods returns reference, or value of received point. Reference
    /// is always returned except the cases when this vector has 'const' scope.
    /// In this case, compiler will substitute first function (that is only
    /// value will be returned).
    //@{
    /// Vector conversion to point.
    inline const TPoint2<T> &Point() const;
    /// Vector conversion to point.
    inline TPoint2<T> &Point();
    //@}

    /// @name Number of elements of the vector.
    //@{
    /// Number of elements of the vector.
    static int NComponents();
    //@}
  };  // class TVect2<T>

// -----------------------------------------------------------------
// Related template functions
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// @brief Vector creation by the multiplication a scalar value to elements of
/// given vector.
///
/// Function creates a new 2D vector. Elements of a new vector are the products of
/// corresponding elements of the given vector and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 2D vector.
/// @return New 2D vector (product of vector and scalar)
template <class T>
inline TVect2<T> operator * (const double d, const TVect2<T> &u)
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
/// @param a - First 2D vector - multiplier.
/// @param b - Second 2D vector - multiplier.
/// @return Value of Dot product.
template <class T>
inline double DotProd(const TVect2<T> &a, const TVect2<T> &b)
  {
  return (double)a.x * (double)b.x + (double)a.y * (double)b.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cross product.
template <class T>
inline double CrossProd(const TVect2<T> &a, const TVect2<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Squared length of the given vector.
///
/// Squared length of the vector is sum of the squared coordinates.
/// @param u - Given 2D vector.
/// @return Length of vector in second power.
template <class T>
inline double SqrLength(const TVect2<T> &u)
  {
  return DotProd(u, u);
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of the given vector.
///
/// Length of the vector is square root of the sum of the squared coordinates.
/// @param u - Given 2D vector.
/// @return Length of vector.
template <class T>
inline double Length(const TVect2<T> &u)
  {
  return Sqrt(SqrLength(u));
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Cosine of angle between two given vectors.
template <class T>
inline double Cos(const TVect2<T> &a, const TVect2<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// @brief Sine of angle between two given vectors.
template <class T>
inline double Sin(const TVect2<T> &a, const TVect2<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with integer elements.
///
/// @param u - 2D vector with elements of the arbitrary type.
/// @return Vector with integer elements.
template <class T>
inline TVect2<int> ConvI(const TVect2<T> &u)
  {
  return TVect2<int>(u.x, u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with float elements.
///
/// @param u - 2D vector with elements of the arbitrary type.
/// @return Vector with float elements.
template <class T>
inline TVect2<float> ConvF(const TVect2<T> &u)
  {
  return TVect2<float>(u.x, u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector with elements of the arbitrary type to vector
/// with double elements.
///
/// @param u - 2D vector with elements of the arbitrary type.
/// @return Vector with double elements.
template <class T>
inline TVect2<double> ConvD(const TVect2<T> &u)
  {
  return TVect2<double>(u.x, u.y);
  }

/// 2D point of arbitrary type.
template <class T>
class TPoint2
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:

    /// First point coordinate
    T x;
    /// Second point coordinate
    T y;

  public:

    /// @name Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TPoint2();
    /// Constructor from the scalar.
    inline TPoint2(const double v);
    /// Constructor from the two scalars.
    inline TPoint2(const double x0, const double y0);
    /// Constructor from the vector.
    inline TPoint2(const TVect2<T> &v);
    /// Copy constructor.
    inline TPoint2(const TPoint2<T> &u);
    //@}

    /// @name Access to the elements of the point.
    //@{
    /// Access to the i-th element of the point: x (i = 0) or y (i = 1).
    inline T & operator [](int i);
    /// Access to the i-th element of the point: x (i = 0) or y (i = 1).
    inline const T &  operator [](int i) const;
    //@}

    /// @name Comparison operators and methods.
    //@{
    /// Element-by-element comparison of this point with given point.
    inline bool operator == (const TPoint2<T> &u) const;
    /// Element-by-element comparison of this point with given point.
    inline bool operator != (const TPoint2<T> &u) const;
    /// Element-by-element comparison of this point with given point.
    inline bool LessOrEqual(const TPoint2<T> &u) const;
    /// Element-by-element comparison of this point with given point.
    inline bool Less(const TPoint2<T> &u) const;
    /// Comparison by lexicographical order.
    inline bool operator < (const TPoint2<T> &u) const;
    /// Comparison of the points with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2);
    /// Comparison of the points with given tolerance.
    inline static bool AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2,
                       const double tolerance);
    /// Check that all elements of vector are valid floats.
    inline bool IsOK() const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given point to this point.
    inline TPoint2<T> & operator = (const TPoint2<T> &u);
    //@}

    /// @name Point-to-point operators
    //@{
    /// Addition of the given point to this point.
    inline TPoint2<T> & operator += (const TPoint2<T> &u);
    //@}

    /// @name Point-to-vector operators.
    //@{
    /// Addition of the given vector to this point.
    inline TPoint2<T> & operator += (const TVect2<T> &u);
    /// Subtraction of the given vector from this point.
    inline TPoint2<T> & operator -= (const TVect2<T> &u);
    //@}

    /// @name Point-to-scalar operators.
    //@{
    /// Addition of the given scalar to elements of this point.
    inline TPoint2<T> & operator += (const double d);
    /// Subtraction of the given scalar from elements of this point.
    inline TPoint2<T> & operator -= (const double d);
    /// Multiplication of this point by given scalar.
    inline TPoint2<T> & operator *= (const double d);
    /// Point by scalar division.
    inline TPoint2<T> & operator /= (const double d);
    /// Addition with a weight.
    inline TPoint2<T> &AddWithWeight(const TVect2<T> &u, const double w);
    //@}

    /// @name Unary operators.
    //@{
    /// Negation of this point.
    inline void Negate();
    /// Creation of the opposite point.
    inline TPoint2<T> operator -() const;
    //@}

    /// @name Binary operators.
    //@{

    /// @name Point-to-vector(point) operators.
    //@{
    /// Creation of the point by addition of the given point to point.
    inline TPoint2<T> operator + (const TPoint2<T> &u) const;
    /// Creation of the point by addition of the given vector to point.
    inline TPoint2<T> operator + (const TVect2<T> &u) const;
    /// Creation of the point by subtraction of the given vector from point.
    inline TPoint2<T> operator - (const TVect2<T> &u) const;
    /// @brief Creation of the vector by subtraction of the given point
    /// from this point.
    inline TVect2<T> operator - (const TPoint2<T> &p) const;
    //@}

    /// @name Point-to-scalar operators.
    //@{
    /// Point creation by addition a scalar value to elements of this point.
    inline TPoint2<T> operator + (const double d) const;
    /// @brief Point creation by subtraction a scalar value from elements
    /// of this point.
    inline TPoint2<T> operator - (const double d) const;
    /// @brief Point creation by the multiplication a scalar value to elements
    /// of this point.
    inline TPoint2<T> operator * (const double d) const;
    /// @brief Point creation by the division of the elements of this point by
    /// the scalar value.
    inline TPoint2<T> operator / (const double d) const;
    //@}
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 2D points.
    //@{
    /// @brief Serialize the point using given default.
    void Serialize(Serializer &inout, const char *tag, const TPoint2<T> &dval = 0);
    /// @brief Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Other methods.
    //@{
    /// Clipping elements of the point.
    inline void Clip(const double vmin, const double vmax);
    /// Clipping elements lower than given minimum.
    inline void ClipLower(double vmin);
    /// Clip point elements to given range.
    inline TPoint2<T> ValToRange(const T vmin, const T vmax) const;
    /// Set of the point elements to the given values.
    inline void Set(double x0, double y0);
    /// Set of the point elements to the same value.
    inline void Set(double a);
    //@}

    /// @name Point to array conversion.
    //@{
    /// Point conversion to array of two elements.
    inline operator T *();
    /// Point conversion to array of two elements.
    inline operator const T *() const;
    //@}

    /// @name Array to point conversion.
    /// Treat an array of 2 elements as a point.  Reference, or constant
    /// reference to received point is returned. Reference is always returned
    /// except the cases when the array has 'const' scope. In this case, compiler
    /// will substitute first function (that is constant reference will be
    /// returned).
    //@{
    /// Treatment of array of two elements as a constant point.
    inline static const TPoint2<T> &Cast(const T *u);
    /// Treatment of array of two elements as a point.
    inline static TPoint2<T> &Cast(T *u);
    //@}

    /// @name Point to vector conversion.
    /// Conversion point to vector. Reference, or constant reference to
    /// received vector is returned. Reference is always returned except the
    /// cases when this point has 'const' scope. In this case, compiler will
    /// substitute first function (that is constant reference will be returned).
    //@{
    /// Treatment of the point as constant vector.
    inline const TVect2<T> &Vector() const;
    /// Treatment of the point as vector.
    inline TVect2<T> &Vector();
    //@}

    /// @name Number of elements of the point.
    //@{
    /// Number of elements of the point.
    static int NComponents();
    //@}
  };  // class TPoint2<T>

// -----------------------------------------------------------------
// Related template functions
// -----------------------------------------------------------------
// Barycentric coordinates of the point in triangle - are in polar.hpp. 
// Vect3d BaryCoords(const TPoint2<T> &p1, const TPoint2<T> &p2,
//                     const TPoint2<T> &p3, const TPoint2<T> &p)

///////////////////////////////////////////////////////////////////////////////
/// Point creation by the multiplication a scalar value to elements of given point.
///
/// Function creates a new 2D point. Elements of a new point are the products of
/// corresponding elements of the given point and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 2D point.
/// @return New 2D point (product of point and scalar)
template <class T>
inline TPoint2<T> operator * (const double d, const TPoint2<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Squared distance between two 2D points.
///
/// @param a - First 2D point.
/// @param b - Second 2D point.
/// @return Distance between points in second power.
template <class T>
inline double SqrDist(const TPoint2<T> &a, const TPoint2<T> &b)
  {
  return SqrLength(a - b);
  }

///////////////////////////////////////////////////////////////////////////////
/// Distance between two 2D points.
///
/// Calculates as length of the difference (2D vector) between two points.
/// @param a - First 2D point.
/// @param b - Second 2D point.
/// @return Distance between points.
template <class T>
inline double Dist(const TPoint2<T> &a, const TPoint2<T> &b)
  {
  return Length(a - b);
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Center of the segment defined by the two given points.
template <class T>
inline TPoint2<T> Center(const TPoint2<T> &a, const TPoint2<T> &b);

///////////////////////////////////////////////////////////////////////////////
/// @brief Center of the triangle.
template <class T>
inline TPoint2<T> Center(const TPoint2<T> &a, const TPoint2<T> &b,
                         const TPoint2<T> &c);

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with integer elements.
///
/// @param u - 2D point with elements of the arbitrary type.
/// @return Point with integer elements.
template <class T>
inline TPoint2<int> ConvI(const TPoint2<T> &u)
  {
  return TPoint2<int>(u.x, u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with float elements.
///
/// @param u - 2D point with elements of the arbitrary type.
/// @return Point with float elements.
template <class T>
inline TPoint2<float> ConvF(const TPoint2<T> &u)
  {
  return TPoint2<float>(u.x, u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the point with elements of the arbitrary type to point
/// with double elements.
///
/// @param u - 2D point with elements of the arbitrary type.
/// @return Point with double elements.
template <class T>
inline TPoint2<double> ConvD(const TPoint2<T> &u)
  {
  return TPoint2<double>(u.x, u.y);
  }

/// Simple bounding 2D box.
template <class T>
class BBox2
  {
  public:
    /// Provide access to the base type.
    typedef T BaseType;

  public:
    /// Point with minimal coordinates
    TPoint2<T> vmin;
    /// Point with maximal coordinates
    TPoint2<T> vmax;

  public:
    /// @name Constructors.
    //@{
    /// Default constructor, without initialization.
    inline BBox2();
    /// Constructor by the one point.
    inline BBox2(const TPoint2<T> &point);
    /// Constructor by the two points.
    inline BBox2(const TPoint2<T> &point1, const TPoint2<T> &point2);
    /// Constructor from points coordinates.
    inline BBox2(T vmin_x, T vmin_y, T vmax_x, T vmax_y);
    /// Copy constructor.
    inline BBox2(const BBox2<T> &box);
    //@}

    /// @name Assignments.
    //@{
    /// Assignment operator.
    inline BBox2<T> & operator = (const BBox2<T> &box);
    //@}

    /// @name Comparisons.
    //@{
    /// Comparison of the two bounding boxes for equality.
    inline bool operator == (const BBox2<T> &box) const;
    /// Comparison of the two bounding boxes for non-equality.
    inline bool operator != (const BBox2<T> &box) const;
    //@}

    /// @name Test for validaty.
    //@{
    /// Check that box is not empty.
    inline bool NotEmpty() const;
    /// Check that box is empty.
    inline bool IsEmpty() const;
    /// Check that box contains the one point only.
    inline bool IsDot() const;
    //@}

    /// @name Inclusion to box and intersection with box.
    //@{
    /// Check that bounding box includes the given point.
    inline bool Includes(const TPoint2<T> &point) const;
    /// Check that bounding box includes the given box.
    inline bool Includes(const BBox2<T> &box) const;
    /// Check that bounding box intersects the given box.
    inline bool Intersects(const BBox2<T> &box) const;
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 2D boxes.
    //@{
    /// @brief Serialize the box (no default is provided).
    void Serialize(Serializer &inout, const char *tag);
    /// @brief Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Miscellaneous methods.
    //@{
    /// Extension of the box for the inclusion of the given point.
    inline void Include(const TPoint2<T> &point);
    /// Extension of the box for the inclusion of the given box.
    inline void Include(const BBox2<T> &box);
    /// Intersection with given box.
    inline void Intersect(const BBox2<T> &box);
    /// Translation of the box by the given vector.
    inline void Translate(const TVect2<T> &vct);
    /// Creation of the box by the translation of this box.
    inline BBox2<T> Translated(const TVect2<T> &vct) const;
    /// Diagonal of the box.
    inline TVect2<T> Diag() const;
    /// Center of the box.
    inline TPoint2<T> Center() const;
    /// Width of the box.
    inline T Width() const;
    /// Height of the box.
    inline T Height() const;
    /// Area of the box.
    inline T Area() const;
    //@}
  };  // class BBox2<T>

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the bounding box with data of the arbitrary type to
/// bounding box with float data.
///
/// @param u - 2D box with data of the arbitrary type.
/// @return Bounding box with float data.
template <class T>
inline BBox2<float> ConvF(const BBox2<T> &u)
  { return BBox2<float>(ConvF(u.vmin), ConvF(u.vmax)); }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the bounding box with data of the arbitrary type to
/// bounding box with double data.
///
/// @param u - 2D box with data of the arbitrary type.
/// @return Bounding box with double data.
template <class T>
inline BBox2<double> ConvD(const BBox2<T> &u)
  { return BBox2<double>(ConvD(u.vmin), ConvD(u.vmax)); }

// -----------------------------------------------------------------
//               Class Math2
// -----------------------------------------------------------------

/// Comparison of 2D vectors and points with a tolerance.
template <class T>
class Math2 : public Math<T>
  {
  public:
    /// @name Comparison of the vector(point) with tolerance.
    //@{
    /// Comparison of the vector with given tolerance.
    inline static bool AboutZero(const TVect2<T> &v, const double tolerance);
    /// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TVect2<T> &v);
    /// Comparison of the vector with predefined tolerance ::EPSILON.
    inline static bool NearZero(const TVect2<T> &v);
    /// Comparison of the point with given tolerance.
    inline static bool AboutZero(const TPoint2<T> &v, const double tolerance);
    /// Comparison of the point with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TPoint2<T> &v);
    /// Comparison of the point with predefined tolerance ::EPSILON.
    inline static bool NearZero(const TPoint2<T> &v);
    //@}

    /// @name Comparison of the vectors(points) with tolerance.
    //@{
    /// Comparison of the vectors with given tolerance.
    inline static bool AboutEqual(const TVect2<T> &v1, const TVect2<T> &v2,
                       const double tolerance);
    /// Comparison of the vectors with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TVect2<T> &v1, const TVect2<T> &v2);
    /// Comparison of the vectors with predefined tolerance ::EPSILON.
    inline static bool NearEqual(const TVect2<T> &v1, const TVect2<T> &v2);
    /// Comparison of the points with given tolerance.
    inline static bool AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2,
                       const double tolerance);
    /// Comparison of the points with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2);
    /// Comparison of the points with predefined tolerance ::EPSILON.
    inline static bool NearEqual(const TPoint2<T> &v1, const TPoint2<T> &v2);
    //@}
  };  // class Math2<T>

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 2D vector with elements of BYTE type.
typedef BVect2<BYTE>      Set2b;
/// Predefined instance of 2D vector with elements of float type.
typedef BVect2<float>     Set2f;
/// Predefined instance of 2D vector with elements of short type.
typedef BVect2<short>     Set2s;
/// Predefined instance of 2D vector with elements of WORD type.
typedef BVect2<WORD>     Set2w;
/// Predefined instance of 2D vector with elements of int type.
typedef BVect2<int>       Set2i;
/// Predefined instance of 2D vector with elements of unsignet type.
typedef BVect2<unsigned>  Set2u;
/// Predefined instance of 2D vector with elements of double type.
typedef BVect2<double>    Set2d;

/// Predefined instance of 2D vector with elements of WORD type.
typedef TVect2<WORD>     Vect2w;
/// Predefined instance of 2D vector with elements of short type.
typedef TVect2<short>     Vect2s;
/// Predefined instance of 2D vector with elements of int type.
typedef TVect2<int>       Vect2i;
/// Predefined instance of 2D vector with elements of float type.
typedef TVect2<float>     Vect2f;
/// Predefined instance of 2D vector with elements of double type.
typedef TVect2<double>    Vect2d;

/// Predefined instance of 2D point with elements of int type.
typedef TPoint2<int>      Point2i;
/// Predefined instance of 2D point with elements of float type.
typedef TPoint2<float>    Point2f;
/// Predefined instance of 2D point with elements of double type.
typedef TPoint2<double>   Point2d;
/// Predefined instance of 2D point with elements of INT64 type.
typedef TPoint2<INT64>    Point2l;

/// Predefined instance of 2D bounding box with elements of int type.
typedef BBox2<int>        BBox2i;
/// Predefined instance of 2D bounding box with elements of float type.
typedef BBox2<float>      BBox2f;
/// Predefined instance of 2D bounding box with elements of double type.
typedef BBox2<double>     BBox2d;

/// Predefined instance with elements of float type.
typedef Math2<float>      Math2f;
/// Predefined instance with elements of double type.
typedef Math2<double>     Math2d;

// -------------------------------------------------------------
// BVect2<T> methods and related functions
// -------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization.
template <class T>
BVect2<T>::BVect2()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from scalar value.
///
/// Both vector elements are initialized by the given scalar value.
/// @param v - Scalar value.
template <class T>
BVect2<T>::BVect2(const double v)
  {
  x = y = (T)v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from given elements.
///
/// First and second vector elements are initialized by the first and second
/// parameters of the method respectively.
/// @param x0 - Scalar value for the first vector element.
/// @param y0 - Scalar value for the second vector element.
template <class T>
BVect2<T>::BVect2(const double x0, const double y0)
  {
  x = (T)x0;
  y = (T)y0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param u - Source 2D vector.
template <class T>
BVect2<T>::BVect2(const BVect2<T> &u)
  {
  x = u.x;
  y = u.y;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to i-th element of this vector.
template <class T>
T & BVect2<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to i-th element of the vector: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value
/// @param i - Index of the vector element.
/// @return Constant reference to i-th element of this vector.
template <class T>
const T &BVect2<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Number of the vector elements.
///
/// @return 2.
template <class T>
int BVect2<T>::NComponents()
  {
  return 2;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 2D vector.
/// @return Reference to this vector (updated).
template <class T>
BVect2<T> & BVect2<T>::operator = (const BVect2<T> &u)
  {
  x = u.x;
  y = u.y;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if both elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool BVect2<T>::operator == (const BVect2<T> &u) const
  {
  return (x == u.x) && (y == u.y);
  }

//////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if some element (first or second or both together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool BVect2<T>::operator != (const BVect2<T> &u) const
  {
  return (x != u.x) || (y != u.y);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of two elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Pointer to the first element of array.
template <class T>
BVect2<T>::operator T *()
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the vector to the array of two elements.
///
/// Usage of this operator as
/// @code (T *)u @endcode
/// returns pointer to the first element of array.
///
/// @return Constant pointer to the first element of array.
template <class T>
BVect2<T>::operator const T *() const
  {
  return &x;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of two elements as a vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Constant reference to the 2D vector.
template <class T>
const BVect2<T> &BVect2<T>::Cast(const T *u)
  {
  return *(const BVect2<T> *)u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of two elements as a vector.
///
/// New vector is not created, but contents of the array may be used as
/// elements of the returned vector.
/// @param u - Array of the elements of the T type.
/// @return Reference to the 2D vector.
template <class T>
BVect2<T> &BVect2<T>::Cast(T *u)
  {
  return *(BVect2<T> *)u;
  }

// -------------------------------------------------------------
// TVect2<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Constuctor by the default, no initialization of elements is performed.
template <class T>
TVect2<T>::TVect2()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from scalar.
///
/// The vector elements are initialized by given scalar.
/// @param v - Given scalar.
template <class T>
TVect2<T>::TVect2(const double v)
  {
  x = y = (T)v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from given elements.
///
/// The vector elements are initialized by x0, y0 respectively.
/// @param x0 - Value for the first vector element.
/// @param y0 - Value for the second vector element.
template <class T>
TVect2<T>::TVect2(const double x0, const double y0)
  {
  x = (T)x0;
  y = (T)y0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from two points.
///
/// Vector is initialized by the two points. The first point is begin of the
/// vector, the second point is end of the vector.
///
/// @param from - 2D point - begin of the vector.
/// @param to   - 2D point - end of the vector.
template <class T>
TVect2<T>::TVect2(const TPoint2<T> &from, const TPoint2<T> &to)
  {
  *this = to - from;
  }

///////////////////////////////////////////////////////////////////////////////
/// Create vector from  point.
///
/// The vector elements are initialized by the elements of the given point.
/// @param p - 2D point.
template <class T>
TVect2<T>::TVect2(const TPoint2<T> &p)
  {
  x = p.x;
  y = p.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - Source 2D vector.
template <class T>
TVect2<T>::TVect2(const TVect2<T> &u)
  {
  x = u.x;
  y = u.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Reference to the element of this vector.
template <class T>
T & TVect2<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the vector: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the vector element.
/// @return Constant reference to the element of this vector
template <class T>
const T &TVect2<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Number of elements of the vector.
///
/// @return 2.
template <class T>
int TVect2<T>::NComponents()
  {
  return 2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if both elements of this vector are equal to the
/// corresponding elements of the given vector, false - otherwise.
template <class T>
bool TVect2<T>::operator == (const TVect2<T> &u) const
  {
  return (x == u.x) && (y == u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if some element (first or second or both together) of this
/// vector is not equal to the corresponding element of the given vector,
/// false - otherwise.
template <class T>
bool TVect2<T>::operator != (const TVect2<T> &u) const
  {
  return (x != u.x) || (y != u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if both vector elements are less or equal than corresponding
/// elements of the given vector, false - otherwise.
template <class T>
bool TVect2<T>::LessOrEqual(const TVect2<T> &u) const
  {
  return (x <= u.x) && (y <= u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this vector with given vector.
///
/// @param u - Given 2D vector.
/// @return true - if both vector elements are less than corresponding
/// elements of the given vector, false - otherwise.
template <class T>
bool TVect2<T>::Less(const TVect2<T> &u) const
  {
  return (x < u.x) && (y < u.y);
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
/// @param u - Given 2D vector.
/// @return true - if this vector less than given vector in lexical sense,
/// false - otherwise.
template <class T>
bool TVect2<T>::operator < (const TVect2<T> &u) const
  {
  if (x < u.x)
    return true;
  if (x > u.x)
    return false;
  return y < u.y;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of vector are valid floats.
///
/// @return @c true if OK, @c false otherwise.
///
template <class T>
bool TVect2<T>::IsOK() const
  {
  return (FloatIsOK(x) && FloatIsOK(y));
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment of the given vector to this vector.
///
/// Elements of this vector are replaced by the elements of the given vector.
/// @param u - Given 2D vector.
/// @return Reference to this vector (updated).
template <class T>
TVect2<T> & TVect2<T>::operator = (const TVect2<T> &u)
  {
  x = u.x;
  y = u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element addition of the given vector to this vector.
///
/// Elements of this vector are replaced by the sums of corresponding elements
/// of this vector and given vector.
/// @param u - Given 2D vector.
/// @return Reference to this vector (the sum of two vectors).
template <class T>
TVect2<T> & TVect2<T>::operator += (const TVect2<T> &u)
  {
  x += u.x;
  y += u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element subtraction of the given vector from this vector.
///
/// Elements of this vector are replaced by the differences between
/// corresponding elements of this vector and given vector.
///
/// @param u - Given 2D vector.
/// @return Reference to this vector (the difference between two vectors).
template <class T>
TVect2<T> & TVect2<T>::operator -= (const TVect2<T> &u)
  {
  x -= u.x;
  y -= u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element multiplication of this vector by given vector.
///
/// Elements of this vector are replaced by the products of corresponding
/// elements of this vector and given vector.
/// @param u - Given 2D vector.
/// @return Reference to this vector (product of two vectors).
template <class T>
TVect2<T> & TVect2<T>::operator *= (const TVect2<T> &u)
  {
  x *= u.x;
  y *= u.y;
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
/// @param u - Given 2D vector.
/// @return Reference to this vector (result of division of two vectors).
template <class T>
TVect2<T> &TVect2<T>::operator /=(const TVect2<T> &u)
  {
  Assert((u.x != 0) && (u.y != 0));
  x /= u.x;
  y /= u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given scalar to elements of this vector.
///
/// Elements of this vector are replaced by the sums of these elements with
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated).
template <class T>
TVect2<T> & TVect2<T>::operator += (const double d)
  {
  x = (T)(x + d);
  y = (T)(y + d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given scalar from elements of this vector.
///
/// Elements of this vector are replaced by the differences between these
/// elements and given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated).
template <class T>
TVect2<T> & TVect2<T>::operator -= (const double d)
  {
  x = (T)(x - d);
  y = (T)(y - d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this vector by given scalar.
///
/// Elements of this vector are replaced by the products of these elements and
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this vector (updated)
template <class T>
TVect2<T> & TVect2<T>::operator *= (const double d)
  {
  x = (T)(x * d);
  y = (T)(y * d);
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
TVect2<T> &TVect2<T>::operator /= (const double d)
  {
  Assert(d != 0.0);
  x = (T)(x / d);
  y = (T)(y / d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition with a weight.
///
/// Elements of this vector are replaced by the sums of these elements with
/// products of the given vector elements and weight value.
///
/// @param u - Given 2D vector.
/// @param w - Weight value.
/// @return  Reference to this vector (updated).
template <class T>
TVect2<T> &TVect2<T>::AddWithWeight(const TVect2<T> &u, const double w)
  {
  x = (T)(x + u.x * w);
  y = (T)(y + u.y * w);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Negation of this vector.
///
/// The signs of the all elements of this vector are changed to opposite. As a
/// result the direction of this vector is changed to opposite.
/// @return None (this vector is affected).
template <class T>
void TVect2<T>::Negate()
  {
  x = -x;
  y = -y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the opposite vector.
///
/// Method creates a new 2D vector. Elements of a new vector are negative with
/// respect to the elements of this vector. As a result the length of the new
/// vector is equal of the length of this vector but direction is opposite with
/// respect to this vector.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator -() const
  {
  return TVect2<T>(-x, -y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element addition of the given vector to
/// this vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given vector.
/// @param u - Given 2D vector.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator + (const TVect2<T> &u) const
  {
  return TVect2<T>(x + u.x, y + u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element subtraction a given vector from
/// this vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given vector.
/// @param u - Given 2D vector.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator - (const TVect2<T> &u) const
  {
  return TVect2<T>(x - u.x, y - u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element multiplication a given vector to
/// this vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given vector.
/// @param u - Given 2D vector.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator * (const TVect2<T> &u) const
  {
  return TVect2<T>(x * u.x, y * u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the element-by-element division of this vector by given
/// vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the elements of the given
/// vector.
/// @note Given vector may not have zero elements. Debug version ASSERTs
/// about this.
/// @param u - Given 2D vector.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator / (const TVect2<T> &u) const
  {
  Assert((u.x != 0) && (u.y != 0));
  return TVect2<T>(x / u.x, y / u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by addition a scalar value to elements of this vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the sums of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator + (const double d) const
  {
  return TVect2<T>(x + d, y + d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by subtraction a scalar value from elements of  this
/// vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the differences
/// between corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator - (const double d) const
  {
  return TVect2<T>(x - d, y - d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the multiplication a scalar value to elements of  this
/// vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the products of
/// corresponding elements of this vector and given scalar.
/// @param d - Given scalar.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator * (const double d) const
  {
  return TVect2<T>(x * d, y * d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector creation by the division elements of this vector by the scalar
/// value.
///
/// Method creates a new 2D vector. Elements of a new vector are the results of
/// the division corresponding elements of this vector by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
/// @param d - Given scalar.
/// @return New 2D vector.
template <class T>
TVect2<T> TVect2<T>::operator / (const double d) const
  {
  Assert(d != 0);
  return TVect2<T>(x / d, y / d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements of the vector.
///
/// Vector elements clipped by the given values in the following sense.
/// Parameters of the method define lower and upper boundary of the elements.
/// If first element of this vector more than upper boundary then first element
/// is replaced by the upper boundary. If first element of this vector less
/// than lower boundary then first element is replaced by the lower boundary.
/// Second element of this vector may be modified by this method analogously.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin - lower bound of the vector elements.
/// @param vmax - upper bound of the vector elements.
/// @return None (only this vector is affected).
template <class T>
void TVect2<T>::Clip(const double vmin, const double vmax)
  {
  Assert(vmax >= vmin);
  Integra::Clip(x, vmin, vmax);
  Integra::Clip(y, vmin, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements lower than given minimum.
///
/// @param vmin - Lower bound of the vector elements.
/// @return True, if clipped, false otherwise.
template <class T>
bool TVect2<T>::ClipLower(double vmin)
  {
  bool ret = false;
  if (Integra::ClipLower(x, vmin))
    ret = true;
  if (Integra::ClipLower(y, vmin))
    ret = true;
  return ret;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements higher than given maximum.
///
/// @param vmax - Top bound of the vector elements.
/// @return True, if clipped, false otherwise.
template <class T>
bool TVect2<T>::ClipHigher(double vmax)
  {
  bool ret = false;
  if (Integra::ClipHigher(x, vmax))
    ret = true;
  if(Integra::ClipHigher(y, vmax))
    ret = true;
  return ret;
  }

///////////////////////////////////////////////////////////////////////////////
/// Clip vector elements to given range.
///
/// Vector elements are clipped to given range.
/// Clipped vector is returned.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin Lower bound of the vector elements.
/// @param vmax Upper bound of the vector elements.
/// @return Clipped vector.
///
template <class T>
TVect2<T> TVect2<T>::ValToRange(const T vmin, const T vmax) const
  {
  Assert(vmax >= vmin);
  return TVect2<T>(
    Integra::ValToRange(x, vmin, vmax),
    Integra::ValToRange(y, vmin, vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check of the elements of this vector - are they in given range.
///
/// Parameters of the method define the lower and upper boundaries of the some
/// range. Method returns true if all two coordinates of this vector are
/// satisfied of the conditions:
/// - coordinates of this vector less or equal than upper boundary @b and
/// - coordinates of this vector more or equal than lower boundary.
///
/// @param vmin - Minimal value of the range (lower bound).
/// @param vmax - Maximal value of the range (upper bound).
/// @return true if coordinates of this vector in specified range,
/// false - otherwise.
template <class T>
bool TVect2<T>::InRange(double vmin, double vmax) const
  {
  Assert(vmax >= vmin);
  return (x >= vmin && x <= vmax &&
          y >= vmin && y <= vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Index of maximal element (by absolute value).
///
/// @return 0 - if first element of this vector is maximal by absolute value,
/// 1 - otherwise (cf. second element is maximal by absolute value).
template <class T>
int TVect2<T>::MaxElementIndex() const
  {
  return (Abs(x) >= Abs(y)) ? 0 : 1;
  }

///////////////////////////////////////////////////////////////////////////////
/// Absolute value of maximal element (by absolute value) of the vector.
///
/// @return Absolute value of maximal element (by absolute value) of the vector.
template <class T>
T TVect2<T>::MaxElement() const
  {
  return Max(Abs(x), Abs(y));
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the given values.
///
/// Elements of this vector are replaced by the corresponding parameters (double
/// values) of the method.
/// @param x0 - New value for the first element of this vector.
/// @param y0 - New value for the second element of this vector.
/// @return None (this vector is affected).
template <class T>
void TVect2<T>::Set(double x0, double y0)
  {
  x = (T)x0;
  y = (T)y0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the vector elements to the same value.
///
/// The all elements of this vector are replaced by the parameter (double value)
/// of the method.
/// @param a - New value for the both vector elements.
/// @return None (this vector is affected).
template <class T>
void TVect2<T>::Set(double a)
  {
  x = y = (T)a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Squared length of this vector.
///
/// Squared length of the vector is sum of the squared coordinates.
/// @return Length of vector in second power.
template <class T>
double TVect2<T>::SqrLength() const
  {
  return x * x + y * y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Length of this vector.
///
/// Length of the vector is square root of the sum of the squared coordinates.
/// @return Length of vector.
template <class T>
double TVect2<T>::Length() const
  {
  return Sqrt(SqrLength());
  }

///////////////////////////////////////////////////////////////////////////////
/// Sum of the elements of the vector.
///
/// @return Sum of the elements of this vector.
template <class T>
T TVect2<T>::Sum() const
  {
  return (T)(x + y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Projection of this vector to given vector.
///
/// Method creates a new 2D vector. Direction of new vector is equal to
/// direction of the given vector - if Dot product (see function
/// DotProd(const TVect2<T> &a, const TVect2<T> &b)) of this vector and
/// given vector is positive value and direction of new vector is opposite to
/// direction of the given vector - Dot product of this vector and
/// given vector is negative value. The length of the new vector is product of
/// the length of this vector and cosin of acute angle between this and given
/// vectors.
///
/// @image html project_2d.gif
///
/// @note Given vector may not be equal to zero vector. Debug version ASSERTs it.
///
/// @param u - Given vector (projective axis)
/// @return New 2D vector (projection)
template <class T>
TVect2<T> TVect2<T>::Project(const TVect2<T> &u) const
  {
  double sqr_len = DotProd(u, u);
  Assert(sqr_len > 0.0);;
  return u * DotProd((*this), u) / sqr_len;
  }

///////////////////////////////////////////////////////////////////////////////
/// Any unit orthogonal vector to given vector.
///
/// Method creates a unit 2D vector which orthogonal to this vector.
///
/// @note Zero vectors are not allowed, DEBUG version ASSERTs it.
/// @return New unit 2D vector (orthogonal to this vector).
template <class T>
TVect2<T> TVect2<T>::AnyOrthogonal() const
  {
  Assert(Length() != 0.0);
  return TVect2<T>(y, -x) / Length();
  }

///////////////////////////////////////////////////////////////////////////////
/// Normalized vector.
///
/// Method normalizes this vector and returns itself. Normalized vector has the
/// same direction as initial vector (before normalization) but length of the
/// normalized vector is equal to 1.
///
/// @note Zero vectors are not allowed, DEBUG version ASSERTs it.
/// @return Reference to this vector (normalized).
template <class T>
TVect2<T> & TVect2<T>::Normalize()
  {
  double len = Length();
  Assert(len != 0.0);
  x = (T)(x / len);
  y = (T)(y / len);
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
double TVect2<T>::ModNormalize()
  {
  double len = Length();
  if (len > 0.0)
    {
    x = (T)(x / len);
    y = (T)(y / len);
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
/// @note Zero vectors are allowed.
/// @return Sum of this vector components before normalization.
///
template <class T>
double TVect2<T>::SumNormalize()
  {
  double sum = Sum();
  if (sum != 0.0)
    {
    x = (T)(x / sum);
    y = (T)(y / sum);
    }
  else
    x = y = (T)0.5;

  return sum;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculate value of the maximal element of the vector and normalize the vector
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
///
template <class T>
double TVect2<T>::MaxNormalize()
  {
  double max = MaxElement();
  if (max != 0.0)
    {
    x = (T)(x / max);
    y = (T)(y / max);
    }
  else
    {
    x = 1;
    y = 1;
    }
  return max;
  }

///////////////////////////////////////////////////////////////////////////////
/// Test of vector normalization.
///
/// Vector is normalized if it's length is equal to 1. Method calculates
/// length of this vector and compares this length with 1.
///
/// @return true - if this vector is normalized, false - otherwise.
template <class T>
bool TVect2<T>::Normalized() const
  {
  return MathF::AboutEqual(Length(), 1.0);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector orientation to given vector.
///
/// Method calculates the Dot product (see function
/// DotProd(const TVect2<T> &a, const TVect2<T> &b)) of this vector with given
/// vector and if calculated value is negative then changes direction of this
/// vector to opposite.
///
/// @param v - Given 2D vector.
/// @return None (this vector may be affected).
template <class T>
void TVect2<T>::Orient(const TVect2<T> &v)
  {
  if (DotProd(*this, v) < 0)
    Negate();
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to array of two elements.
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
TVect2<T>::operator T *()
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to array of two elements.
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
TVect2<T>::operator const T *() const
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of two elements as a constant vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 2D constant vector.
template <class T>
const TVect2<T> &TVect2<T>::Cast(const T *u)
  {
  return *(const TVect2<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of two elements as a vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 2D vector.
template <class T>
TVect2<T> &TVect2<T>::Cast(T *u)
  {
  return *(TVect2<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to point.
///
/// New point is not created (data is not changed), but contents of this vector
/// may be used as contents of the returned point.
///
/// @return Constant reference to 2D point.
template <class T>
const TPoint2<T> &TVect2<T>::Point() const
  {
  return TPoint2<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Vector conversion to point.
///
/// New point is not created (data is not changed), but contents of this vector
/// may be used as contents of the returned point.
///
/// @return Reference to 2D point.
template <class T>
TPoint2<T> &TVect2<T>::Point()
  {
  return TPoint2<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Cross product.
///
/// Notion <b>cross product</b> is defined for the 3D space. If first vector
/// has coordinates {x1; y1; z1}, second vector has coordinates {x2; y2; z2}
/// then cross product is vector with coordinates
/// {y1 * z2 - y2* z1; x2 * z1 - x1 * z2; x1 * y2 - x2 * y1}.
/// This vector is orthogonal to both given vectors.
///
/// This method calculates the third coordinate of the cross product of the
/// 3D vectors with coordinates {x1; y1; 0} and {x2; y2; 0} where {x1; y1} and
/// {x2; y2} coordinates of the given 2D vectors.
///
/// @param a - First 2D vector.
/// @param b - Second 2D vector.
template <class T>
inline double CrossProd(const TVect2<T> &a, const TVect2<T> &b)
  {
  return a.x * b.y - a.y * b.x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Cosine of angle between two given vectors.
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
/// @param a - First 2D vector.
/// @param b - Second 2D vector.
/// @return Value of Cosine
template <class T>
inline double Cos(const TVect2<T> &a, const TVect2<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(DotProd(a, b) / Sqrt(SqrLength(a) * SqrLength(b)), -1, 1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sine of angle between two given vectors.
///
/// @note Given vectors may not be equal to zero vector. Debug version asserts
/// about this.
/// @param a - First 2D vector
/// @param b - Second 2D vector
/// @return Value of Sine
template <class T>
inline double Sin(const TVect2<T> &a, const TVect2<T> &b)
  {
  Assert((SqrLength(a) != 0) && (SqrLength(b) != 0));
  return Clipped(Abs(CrossProd(a, b)) / Sqrt((SqrLength(a) * SqrLength(b))),
                 0, 1);
  }

// -------------------------------------------------------------
// TPoint2<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
TPoint2<T>::TPoint2()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the scalar.
///
/// The point elements are initialized by given scalar.
/// @param v - Given scalar.
template <class T>
TPoint2<T>::TPoint2(const double v)
  {
  x = y = (T)v;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the two scalars.
///
/// The point elements are initialized by x0, y0 respectively.
///
/// @param x0 - Value for the first point element.
/// @param y0 - Value for the second point element.
template <class T>
TPoint2<T>::TPoint2(const double x0, const double y0)
  {
  x = (T)x0;
  y = (T)y0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the vector.
///
/// The elements of the point are initialized by the elements of the given
/// vector.
///
/// @param v - Given 2D vector.
template <class T>
TPoint2<T>::TPoint2(const TVect2<T> &v)
  {
  x = v.x;
  y = v.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param u - 2D point - source for the initialization.
template <class T>
TPoint2<T>::TPoint2(const TPoint2<T> &u)
  {
  x = u.x;
  y = u.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the point: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the point element.
/// @return Reference to the element of this point.
template <class T>
T & TPoint2<T>::operator [](int i)
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th element of the point: x (i = 0) or y (i = 1).
///
/// @note i must be in range 0..1. Debug version asserts this.
/// @note The result may be used as an l-value.
/// @param i - Index of the point element.
/// @return Constant reference to the element of this point.
template <class T>
const T &TPoint2<T>::operator [](int i) const
  {
  Assert((i >= 0) && (i < 2));
  return (&x)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Number of the elements of the point.
///
/// @return 2.
template <class T>
int TPoint2<T>::NComponents()
  {
  return 2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 2D point.
/// @return true - if both elements of this point are equal to the
/// corresponding elements of the given point, false - otherwise.
template <class T>
bool TPoint2<T>::operator == (const TPoint2<T> &u) const
  {
  return (x == u.x) && (y == u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 2D point.
/// @return true - if some element (first or second or both together) of this
/// point is not equal to the corresponding element of the given point,
/// false - otherwise.
template <class T>
bool TPoint2<T>::operator != (const TPoint2<T> &u) const
  {
  return (x != u.x) || (y != u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 2D point.
/// @return true - if both point elements are less or equal than corresponding
/// elements of the given point, false - otherwise.
template <class T>
bool TPoint2<T>::LessOrEqual(const TPoint2<T> &u) const
  {
  return (x <= u.x) && (y <= u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Element-by-element comparison of this point with given point.
///
/// @param u - Given 2D point.
/// @return true - if both point elements are less than corresponding
/// elements of the given point, false - otherwise.
template <class T>
bool TPoint2<T>::Less(const TPoint2<T> &u) const
  {
  return (x < u.x) && (y < u.y);
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
/// @param u - Given 2D point.
/// @return true - if this point less than given point in lexical sense,
/// false - otherwise.
template <class T>
bool TPoint2<T>::operator < (const TPoint2<T> &u) const
  {
  if (x < u.x)
    return true;
  if (x > u.x)
    return false;
  return y < u.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given points and result (2D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 2D point.
/// @param v2 - Second 2D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
bool TPoint2<T>::AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2)
  {
  return Math2<T>::AboutZero(v1 - v2, Math2<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with given tolerance.
///
/// Method calculates difference between given points and result (2D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 2D point.
/// @param v2 - Second 2D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than given tolerance, false -
/// otherwise.
template <class T>
bool TPoint2<T>::AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2,
                                 const double tolerance)
  {
  return Math2<T>::AboutZero(v1 - v2, tolerance);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of the point are valid floats.
///
/// @return @c true if OK, @c false otherwise.
///
template <class T>
bool TPoint2<T>::IsOK() const
  {
  return (FloatIsOK(x) && FloatIsOK(y));
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment of the given point to this point.
///
/// Elements of this point are replaced by the elements of the given point.
/// @param u - Given 2D point.
/// @return Reference to this point (updated).
template <class T>
TPoint2<T> & TPoint2<T>::operator = (const TPoint2<T> &u)
  {
  x = u.x;
  y = u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given point to this point.
///
/// Elements of this point are replaced by the sums of corresponding elements
/// of this point and given point. In geometric sense this action is
/// summation of this point with the given point.
/// @param u - Given 2D point.
/// @return Reference to this point (the sum of the points).
template <class T>
TPoint2<T> & TPoint2<T>::operator += (const TPoint2<T> &u)
  {
  x += u.x;
  y += u.y;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given vector to this point.
///
/// Elements of this point are replaced by the sums of corresponding elements
/// of this point and given vector. In geometric sense this action is
/// translation of this point along of the given vector.
/// @param u - Given 2D vector.
/// @return Reference to this point (the sum of the point and vector).
template <class T>
TPoint2<T> & TPoint2<T>::operator += (const TVect2<T> &u)
  {
  x += u.x;
  y += u.y;
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
/// @param u - Given 2D vector.
/// @return Reference to this point (difference between point and vector).
template <class T>
TPoint2<T> & TPoint2<T>::operator -= (const TVect2<T> &u)
  {
  x -= u.x;
  y -= u.y;
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
TPoint2<T> & TPoint2<T>::operator += (const double d)
  {
  x = (T)(x + d);
  y = (T)(y + d);
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
TPoint2<T> & TPoint2<T>::operator -= (const double d)
  {
  x = (T)(x - d);
  y = (T)(y - d);
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
TPoint2<T> & TPoint2<T>::operator *= (const double d)
  {
  x = (T)(x * d);
  y = (T)(y * d);
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
TPoint2<T> &TPoint2<T>::operator /= (const double d)
  {
  Assert(d != 0.0);
  x = (T)(x / d);
  y = (T)(y / d);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition with a weight.
///
/// Elements of this point are replaced by the sums of these elements with
/// products of the given vector elements and weight value.
///
/// @param u - Given 2D vector.
/// @param w - Weight value.
/// @return  Reference to this point (updated).
template <class T>
TPoint2<T> &TPoint2<T>::AddWithWeight(const TVect2<T> &u, const double w)
  {
  x = (T)(x + u.x * w);
  y = (T)(y + u.y * w);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Negation of this point.
///
/// The signs of the all elements of this point are changed to opposite. As a
/// result the new position of this point is symmetric to old position with
/// respect to origin (cf. point {0;0}) of coordinate system.
/// @return None (this point is affected).
template <class T>
void TPoint2<T>::Negate()
  {
  x = -x;
  y = -y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the opposite point.
///
/// Method creates a new 2D point. Elements of a new point are negative with
/// respect to elements of this point. As a result the new point is symmetric
/// to this point with respect to center (cf. point {0;0}) of coordinate system.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator -() const
  {
  return TPoint2<T>(-x, -y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by addition of the given point to point.
///
/// Method creates a new 2D point. Elements of a new point are the sums of
/// corresponding elements of this point and given vector. In geometric sense
/// this action is translation of this point along of the given vector.
///
/// @param u - Given 2D vector.
/// @return New 2D point
template <class T>
inline TPoint2<T> TPoint2<T>::operator + (const TPoint2<T> &u) const
  {
  return TPoint2<T>(x + u.x, y + u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by addition of the given vector to point.
///
/// Method creates a new 2D point. Elements of a new point are the sums of
/// corresponding elements of this point and given vector. In geometric sense
/// this action is translation of this point along of the given vector.
///
/// @param u - Given 2D vector.
/// @return New 2D point
template <class T>
inline TPoint2<T> TPoint2<T>::operator + (const TVect2<T> &u) const
  {
  return TPoint2<T>(x + u.x, y + u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by subtraction of the given vector from point.
///
/// Method creates a new 2D point. Elements of a new point are the differences
/// between corresponding elements of this point and given vector. In geometric
/// sense this action is translation of this point along of the given vector in
/// the opposite (with respect to vector direction) direction.
///
/// @param u - Given 2D vector.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator - (const TVect2<T> &u) const
  {
  return TPoint2<T>(x - u.x, y - u.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the vector by subtraction of the given point from this
/// point.
///
/// Method creates a new 2D vector. Elements of a new vector are the differences
/// between corresponding elements of this point and given point. In geometric
/// sense the given point is begin of the new vector, this point is end of the
/// new vector.
///
/// @param p - Given 2D point.
/// @return New 2D vector.
template <class T>
TVect2<T> TPoint2<T>::operator - (const TPoint2<T> &p) const
  {
  return TVect2<T>(x - p.x, y - p.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by addition a scalar value to elements of this point.
///
/// Method creates a new 2D point. Elements of a new point are the sums of the
/// corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator + (const double d) const
  {
  return TPoint2<T>(x + d, y + d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by subtraction a scalar value from elements of  this
/// point.
///
/// Method creates a new 2D point. Elements of a new point are the differences
/// between corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator - (const double d) const
  {
  return TPoint2<T>(x - d, y - d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by the multiplication a scalar value to elements of  this
/// point.
///
/// Method creates a new 2D point. Elements of a new point are the products of
/// corresponding elements of this point and given scalar.
/// @param d - Given scalar.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator * (const double d) const
  {
  return TPoint2<T>(x * d, y * d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Point creation by the division of the elements of this point by the scalar
/// value.
///
/// Method creates a new 2D point. Elements of a new point are the results of
/// the division corresponding elements of this point by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
/// @param d - Given scalar.
/// @return New 2D point.
template <class T>
TPoint2<T> TPoint2<T>::operator / (const double d) const
  {
  Assert(d != 0);
  return TPoint2<T>(x / d, y / d);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements of the point.
///
/// Point elements clipped by the given values in the following sense.
/// Parameters of the method define lower and upper boundary of the elements.
/// If first element of this point more than upper boundary then first element
/// is replaced by the upper boundary. If first element of this point less
/// than lower boundary then first element is replaced by the lower boundary.
/// Second element of this point may be modified by this method analogously.
///
/// @note First parameter can not be larger than second parameter, otherwise
/// the result is undefined. Debug version ASSERTs it.
///
/// @param vmin - lower bound of the point elements.
/// @param vmax - upper bound of the point elements.
/// @return None (only this point is affected).
template <class T>
void TPoint2<T>::Clip(const double vmin, const double vmax)
  {
  Assert(vmax >= vmin);
  Integra::Clip(x, vmin, vmax);
  Integra::Clip(y, vmin, vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Clipping elements lower than given minimum.
///
/// @param vmin - Lower bound of the vector elements.
template <class T>
void TPoint2<T>::ClipLower(double vmin)
  {
  Integra::ClipLower(x, vmin);
  Integra::ClipLower(y, vmin);
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
TPoint2<T> TPoint2<T>::ValToRange(const T vmin, const T vmax) const
  {
  Assert(vmax >= vmin);
  return TPoint2<T>(
    Integra::ValToRange(x, vmin, vmax),
    Integra::ValToRange(y, vmin, vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the point elements to the given values.
///
/// Elements of this point are replaced by the corresponding parameters (double
/// values) of the method.
/// @param x0 - New value for the first element of this point.
/// @param y0 - New value for the second element of this point.
/// @return None (this point is affected).
template <class T>
void TPoint2<T>::Set(double x0, double y0)
  {
  x = (T)x0;
  y = (T)y0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the point elements to the same value.
///
/// The all elements of this point are replaced by the parameter (double value)
/// of the method.
/// @param a - New value for the both point elements.
/// @return None (this point is affected).
template <class T>
void TPoint2<T>::Set(double a)
  {
  x = y = (T)a;
  }

///////////////////////////////////////////////////////////////////////////////
/// Point conversion to array of two elements.
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
TPoint2<T>::operator T *()
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Point conversion to array of two elements.
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
TPoint2<T>::operator const T *() const
  {
  return &x;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of two elements as a constant point.
///
/// New point is not created (data is not changed), but contents of the array
/// may be used as elements of the returned point.
///
/// @return Reference to the 2D constant point.
template <class T>
const TPoint2<T> &TPoint2<T>::Cast(const T *u)
  {
  return *(const TPoint2<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of array of two elements as a point.
///
/// New point is not created (data is not changed), but contents of the array
/// may be used as elements of the returned point.
///
/// @return Reference to the 2D point.
template <class T>
TPoint2<T> &TPoint2<T>::Cast(T *u)
  {
  return *(TPoint2<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the point as constant vector.
///
/// New vector is not created (data is not changed), but elements of the point
/// may be used as elements of the returned vector.
///
/// @return Reference to the 2D constant vector.
template <class T>
const TVect2<T> &TPoint2<T>::Vector() const
  {
  return TVect2<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Treatment of the point as vector.
///
/// New vector is not created (data is not changed), but elements of the point
/// may be used as elements of the returned vector.
///
/// @return Reference to the 2D vector.
template <class T>
TVect2<T> &TPoint2<T>::Vector()
  {
  return TVect2<T>::Cast(&x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the segment defined by the two given points.
///
/// Center of the segment is point. Elements of this point are the average
/// values of the corresponding elements of the two points in the begin and end
/// of the segment.
///
/// @param a - First 2D point (begin of the segment).
/// @param b - Second 2D point (end of the segment).
/// @return Center of the segment.
template <class T>
inline TPoint2<T> Center(const TPoint2<T> &a, const TPoint2<T> &b)
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
/// @param a - First 2D point (first vertex of the triangle).
/// @param b - Second 2D point (second vertex of the triangle).
/// @param c - Third  2D point (third vertex of the triangle).
/// @return Center of the triangle.
template <class T>
inline TPoint2<T> Center(const TPoint2<T> &a, const TPoint2<T> &b,
                         const TPoint2<T> &c)
  {
  return ((a.Vector() + b.Vector() + c.Vector()) / 3.0).Point();
  }


// -------------------------------------------------------------
// BBox2<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, without initialization.
template <class T>
BBox2<T>::BBox2()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by one point.
///
/// Lower (@a vmin) and upper (@a vmax) boundaries of the box are initialized
/// by the same 2D point. In result the box is one point only (see method
/// IsDot()).
/// @param point - 2D point.
template <class T>
BBox2<T>::BBox2(const TPoint2<T> &point)
  {
  vmin = vmax = point;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor by the two points.
///
/// Lower (@a vmin) and upper (@a vmax) boundaries of a box are initialized by
/// the  given points.
///
/// @param point1 - 2D point defines lower boundary.
/// @param point2 - 2D point defines upper boundary.
template <class T>
BBox2<T>::BBox2(const TPoint2<T> &point1, const TPoint2<T> &point2)
  {
  vmin = point1;
  vmax = point2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from points coordinates.
/// @param vmin_x X coordinate of the lower boundary.
/// @param vmin_y Y coordinate of the lower boundary.
/// @param vmax_x X coordinate of the upper boundary.
/// @param vmax_y Y coordinate of the upper boundary.
template <class T>
BBox2<T>::BBox2(T vmin_x, T vmin_y, T vmax_x, T vmax_y)
  {
  vmin.x = vmin_x;
  vmin.y = vmin_y;
  vmax.x = vmax_x;
  vmax.y = vmax_y;
  }



///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param box - 2D bounding box - source for this box creation.
template <class T>
BBox2<T>::BBox2(const BBox2<T> &box)
  {
  vmin = box.vmin;
  vmax = box.vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// Parameters of this bounding box (upper @a vmax and lower @a vmin boundary)
/// are replaced by the parameters of the given box.
///
/// @param box - Given 2D bounding box.
/// @return Reference to this box (updated).
template <class T>
BBox2<T> & BBox2<T>::operator = (const BBox2<T> &box)
  {
  vmin = box.vmin;
  vmax = box.vmax;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for equality.
///
/// This box is equal to given box if boundaries (points @a vmin, @a vmax) of
/// this box are equal to boundaries of the given box.
/// @param box - Given 2D box.
/// @return true - if this bounding box is equal to given box, false -
/// otherwise.
template <class T>
bool BBox2<T>::operator == (const BBox2<T> &box) const
  {
  return (vmin == box.vmin) && (vmax == box.vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the two bounding boxes for non-equality.
///
/// This box is not equal to given box if some boundary of this box (point
/// @a vmin or @a vmax or both points simultaneously) is not equal to
/// corresponding boundary of the given box.
/// @param box - Given 2D box.
/// @return true - if this bounding box is not equal to given box, false -
/// otherwise.
template <class T>
bool BBox2<T>::operator != (const BBox2<T> &box) const
  {
  return (vmin != box.vmin) || (vmax != box.vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box is not empty.
///
/// Bounding box is not empty if there are points (even if one point) inside
/// box. In this case lower boundary (@a vmin) of a box is less or equal than
/// upper boundary (@a vmax). In other words, coordinates of the left lower
/// vertex of the  rectangle are less or equal than coordinates of the right
/// upper vertex of the rectangle.
///
/// @return true - if box is not empty, false - otherwise.
template <class T>
bool BBox2<T>::NotEmpty() const
  {
  return (vmin.x <= vmax.x) && (vmin.y <= vmax.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box is empty.
///
/// Bounding box is empty if there are no points inside box.
///
/// @return true - if box is empty, false - otherwise.
template <class T>
bool BBox2<T>::IsEmpty() const
  {
  return (vmin.x > vmax.x) || (vmin.y > vmax.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that box contains the one point only.
///
/// Bounding box contains the one point only if upper boundary (@a vmax) of
/// the box is equal to the lower boundary (@a vmin) of the box. In other words,
/// left lower vertex of the rectangle is coincide with right upper vertex cf.
/// rectangle is degenerate cf. it is dot: @a vmin = @a vmax.
///
/// @return true - if bounding box is dot, false - otherwise.
template <class T>
bool BBox2<T>::IsDot() const
  {
  return vmin == vmax;
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given point.
///
/// This bounding box includes the given point if coordinates of the given
/// point (x, y) is in ranges defined by the lower (@a vmin) and upper (@a vmax)
/// boundaries of this box:
/// - @a vmin.x <= @a x <= @a vmax.x;
/// - @a vmin.y <= @a y <= @a vmax.y.
///
/// @param point - Given 2D point.
/// @return true - if given point is inside of this box, false - otherwise.
template <class T>
bool BBox2<T>::Includes(const TPoint2<T> &point) const
  {
  return (vmin.LessOrEqual(point)) && (point.LessOrEqual(vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box includes the given box.
///
/// This bounding box includes the given box if both boundaries (@a vmin
/// @a vmax) of the given box are inside of this box (see method Includes() for
/// the point).
///
/// @param box - Given 2D box (rectangle).
/// @return true - if given box is inside of this box, false - otherwise.
template <class T>
bool BBox2<T>::Includes(const BBox2<T> &box) const
  {
  return (vmin.LessOrEqual(box.vmin)) && (box.vmax.LessOrEqual(vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Check that bounding box intersects the given box.
///
/// This bounding box intersects the given box if both boxes (this and given)
/// contain the common points.
///
/// @param box - Given 2D box (rectangle).
/// @return true - if this box intersects the given box, false - otherwise.
template <class T>
bool BBox2<T>::Intersects(const BBox2<T> &box) const
  {
  return (vmin.LessOrEqual(box.vmax)) && (box.vmin.LessOrEqual(vmax));
  }

///////////////////////////////////////////////////////////////////////////////
/// Extension of the box for the inclusion of the given point.
///
/// This bounding box is not changed if given point is inside of this box (see
/// method Includes()). Otherwise this box is extended to include the given
/// point. Resulted bounding box is minimal box which includes the initial this
/// box and given point.
///
/// @param point - Given 2D point.
/// @return None (this box may be affected).
template <class T>
void BBox2<T>::Include(const TPoint2<T> &point)
  {
  if (point.x < vmin.x)
    vmin.x = point.x;
  if (vmax.x < point.x)
    vmax.x = point.x;
  if (point.y < vmin.y)
    vmin.y = point.y;
  if (vmax.y < point.y)
    vmax.y = point.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Extension of the box for the inclusion of the given box.
///
/// This bounding box is not changed if both boundaries (points @a vmax and
/// @a vmin with maximal and minimal coordinates) of the given box are inside
/// of this box (see method Includes()). Otherwise this box is extended to
/// include the given box. Resulted bounding box is minimal box which includes
/// the initial this box and given box.
///
/// @param box - Given 2D box (rectangle).
/// @return None (this box may be affected).
template <class T>
void BBox2<T>::Include(const BBox2<T> &box)
  {
  if (vmin.x > box.vmin.x)
    vmin.x = box.vmin.x;
  if (vmax.x < box.vmax.x)
    vmax.x = box.vmax.x;
  if (vmin.y > box.vmin.y)
    vmin.y = box.vmin.y;
  if (vmax.y < box.vmax.y)
    vmax.y = box.vmax.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Intersection with given box.
///
/// If this bounding box is intersected by the given box then result of the
/// intersection (new bounding box) is placed into this box. If this bounding
/// box is not intersected by the given box then this box is not changed.
///
/// @param box - Given 2D box (rectangle).
/// @return none (this box may be affected).
template <class T>
void BBox2<T>::Intersect(const BBox2<T> &box)
  {
  if (vmin.x < box.vmin.x)
    vmin.x = box.vmin.x;
  if (vmax.x > box.vmax.x)
    vmax.x = box.vmax.x;
  if (vmin.y < box.vmin.y)
    vmin.y = box.vmin.y;
  if (vmax.y > box.vmax.y)
    vmax.y = box.vmax.y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Translation of the box by the given vector.
///
/// This bounding box is modified by addition of the  given 2D vector to
/// the lower (@a vmin) and upper (@a vmax) boundaries of this box. In fact it
/// is shift of the rectangle in 2D space along of the given vector.
///
/// @param vct - Given 2D vector.
/// @return None (this box is affected).
template <class T>
void BBox2<T>::Translate(const TVect2<T> &vct)
  {
  vmin += vct;
  vmax += vct;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the box by the translation of this box.
///
/// New bounding box is created  by addition of the given vector to the
/// lower (@a vmin) and upper (@a vmax) boundaries of this box. In fact it is
/// shift of the rectangle in 2D space along of the given vector.
///
/// @param vct - Given 2D vector.
/// @return Created bounding box.
template <class T>
BBox2<T> BBox2<T>::Translated(const TVect2<T> &vct) const
  {
  return BBox2<T>(vmin + vct, vmax + vct);
  }

///////////////////////////////////////////////////////////////////////////////
/// Diagonal of the box.
///
/// Diagonal is vector created by the two points. Begin of this vector is point
/// with minimal coordinates of the box @a vmin (left lower vertex). End of
/// this vector is point with maximal coordinates of the box @a vmax (right
/// upper vertex).
///
/// @return 2D vector of the diagonal.
template <class T>
TVect2<T> BBox2<T>::Diag() const
  {
  return vmax - vmin;
  }

///////////////////////////////////////////////////////////////////////////////
/// Center of the box.
///
/// Center of the box is 2D point. Coordinates of this point are the average
/// values of the boundaries @a vmin and @a vmax of the box.
///
/// @return 2D point of the box center.
template <class T>
TPoint2<T> BBox2<T>::Center() const
  {
  return Integra::Center(vmax, vmin);
  }

///////////////////////////////////////////////////////////////////////////////
/// Width of the box.
///
/// Width of the box is box size along first coordinate axis.
/// @return Value of the box width.
template <class T>
T BBox2<T>::Width() const
  {
  return (vmax.x - vmin.x);
  }

///////////////////////////////////////////////////////////////////////////////
/// Height of the box.
///
/// Height of the box is box size along second coordinate axis.
/// @return Value of the box height.
template <class T>
T BBox2<T>::Height() const
  {
  return (vmax.y - vmin.y);
  }

///////////////////////////////////////////////////////////////////////////////
/// Area of the box.
///
/// Area of the box is product of the box width and box height.
/// @return Value of the box area.
template <class T>
T BBox2<T>::Area() const
  {
  return Width() * Height();
  }

// -------------------------------------------------------------
// Implementation of Math2<T> methods
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with given tolerance.
///
/// @param v - Given 2D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than given tolerance, false - otherwise.
template <class T>
bool Math2<T>::AboutZero(const TVect2<T> &v, const double tolerance)
  {
  return Math<T>::AboutZero(v.x, tolerance) &&
         Math<T>::AboutZero(v.y, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Given 2D vector.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than tolerance Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool Math2<T>::AboutZero(const TVect2<T> &v)
  {
  return AboutZero(v, Math2<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vector with predefined tolerance ::EPSILON.
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v - Given 2D vector.
/// @return true - if absolute values of the coordinates of the given vector
/// are less or equal than tolerance ::EPSILON, false - otherwise.
template <class T>
bool Math2<T>::NearZero(const TVect2<T> &v)
  {
  return AboutZero(v, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with given tolerance.
///
/// @param v - Given 2D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than given tolerance, false - otherwise.
template <class T>
bool Math2<T>::AboutZero(const TPoint2<T> &v, const double tolerance)
  {
  return Math<T>::AboutZero(v.x, tolerance) &&
         Math<T>::AboutZero(v.y, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with predefined tolerance Math<T>::TOLERANCE.
///
/// @param v - Given 2D point.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than tolerance Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool Math2<T>::AboutZero(const TPoint2<T> &v)
  {
  return AboutZero(v, Math2<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the point with predefined tolerance ::EPSILON.
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v - Given 2D point.
/// @return true - if absolute values of the coordinates of the given point
/// are less or equal than tolerance ::EPSILON, false - otherwise.
template <class T>
bool Math2<T>::NearZero(const TPoint2<T> &v)
  {
  return AboutZero(v, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with given tolerance.
///
/// Method calculates difference between given vectors and result (2D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 2D vector.
/// @param v2 - Second 2D vector.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than given tolerance, false -
/// otherwise.
template <class T>
bool Math2<T>::AboutEqual(const TVect2<T> &v1, const TVect2<T> &v2,
                                 const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given vectors and result (2D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 2D vector.
/// @param v2 - Second 2D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
bool Math2<T>::AboutEqual(const TVect2<T> &v1, const TVect2<T> &v2)
  {
  return AboutZero(v1 - v2, Math2<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the vectors with tolerance ::EPSILON.
///
/// Method calculates difference between given vectors and result (2D vector)
/// compares with tolerance ::EPSILON (see method NearZero()).
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v1 - First 2D vector.
/// @param v2 - Second 2D vector.
/// @return true - if absolute values of the differences of the coordinates
/// of the given vectors are less or equal than tolerance ::EPSILON,
/// false - otherwise.
template <class T>
bool Math2<T>::NearEqual(const TVect2<T> &v1, const TVect2<T> &v2)
  {
  return AboutZero(v1 - v2, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with given tolerance.
///
/// Method calculates difference between given points and result (2D vector)
/// compares with given tolerance (see method AboutZero()).
///
/// @param v1 - First 2D point.
/// @param v2 - Second 2D point.
/// @param tolerance - Given tolerance.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than given tolerance, false -
/// otherwise.
template <class T>
bool Math2<T>::AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2,
                                 const double tolerance)
  {
  return AboutZero(v1 - v2, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given points and result (2D vector)
/// compares with tolerance Math<T>::TOLERANCE (see method AboutZero()).
///
/// @param v1 - First 2D point.
/// @param v2 - Second 2D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance Math<T>::TOLERANCE,
/// false - otherwise.
template <class T>
bool Math2<T>::AboutEqual(const TPoint2<T> &v1, const TPoint2<T> &v2)
  {
  return AboutZero(v1 - v2, Math2<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the points with tolerance ::EPSILON.
///
/// Method calculates difference between given points and result (2D vector)
/// compares with tolerance ::EPSILON (see method NearZero()).
///
/// @note Value of constant ::EPSILON is independent from type T.
/// @param v1 - First 2D point.
/// @param v2 - Second 2D point.
/// @return true - if absolute values of the differences of the coordinates
/// of the given points are less or equal than tolerance ::EPSILON,
/// false - otherwise.
template <class T>
bool Math2<T>::NearEqual(const TPoint2<T> &v1, const TPoint2<T> &v2)
  {
  return AboutZero(v1 - v2, EPSILON);
  }

///////////////////////////////////////////////////////////////////////////////
/// Area of the triangle.
///
/// Triangle is defined by the given three points - parameters of the method.
///
/// @param a - The first vertex of the triangle.
/// @param b - The second vertex of the triangle.
/// @param c - The third vertex of the triangle.
/// @return Area of the triangle.
template <class T>
inline double TrgArea(const TPoint2<T> &a, const TPoint2<T> &b, const TPoint2<T> &c)
  {
  return Abs(0.5 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)));
  }

///////////////////////////////////////////////////////////////////////////////
/// Perimeter of the triangle.
///
/// Triangle is defined by the given three points - parameters of the method.
///
/// @param a - The first vertex of the triangle.
/// @param b - The second vertex of the triangle.
/// @param c - The third vertex of the triangle.
/// @return Perimeter of the triangle.
template <class T>
inline double TrgPerimeter(const TPoint2<T> &a, const TPoint2<T> &b, const TPoint2<T> &c)
  {
  return Length(b - a) + Length(c - b) + Length(a - c);
  }

INTEGRA_NAMESPACE_END
#endif
