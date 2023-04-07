/// @file
///
/// @brief Definition of template class for matrices 2x2
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>

#ifndef _KLBC_MATRIX2_HPP_
#define _KLBC_MATRIX2_HPP_

#include <base/base.hpp>

#include "vect2.hpp"

INTEGRA_NAMESPACE_START

/// Matrix 2x2 of the arbitrary type.
template <class T>
class TMatrix2
  {
  public:
    /// @name Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TMatrix2();
    /// Constructor from the given value.
    inline TMatrix2(const double a);
    /// Constructor from the given values.
    inline TMatrix2(const double a, const double b);
    /// Constructor from the given all components.
    inline TMatrix2(double a00, double a01, double a10, double a11);
    /// Constructor from the given 2D vectors.
    inline TMatrix2(const TVect2<T> &x, const TVect2<T> &y);
    /// Copy constructor
    inline TMatrix2(const TMatrix2<T> &mat);
    //@}

    /// @name Strict comparison.
    //@{
    /// Comparison of this matrix with given matrix (equality).
    inline bool operator == (const TMatrix2<T> &u) const;
    /// Comparison of this matrix with given matrix (inequality).
    inline bool operator != (const TMatrix2<T> &u) const;
    //@}

    /// @name Comparison with tolerance.
    //@{
    /// Comparison with given tolerance.
    inline static bool AboutZero(const TMatrix2<T> &u, const double tolerance);
    /// Comparison with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TMatrix2<T> &u);
    /// Comparison of the matrices with given tolerance.
    inline static bool AboutEqual(const TMatrix2<T> &a, const TMatrix2<T> &b, const double tolerance);
    /// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TMatrix2<T> &a, const TMatrix2<T> &b);
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th row of the matrix (i = 0, 1).
    inline TVect2<T> & operator [] (int i);
    /// Access to the i-th row of the matrix (i = 0, 1).
    inline const TVect2<T> &operator [] (int i) const;
    //@}

    /// @name Arithmetic operators.
    //@{
    /// Addition of the matrices.
    inline TMatrix2<T> operator + (const TMatrix2<T> &v) const;
    /// Subtraction of the matrices.
    inline TMatrix2<T> operator - (const TMatrix2<T> &v) const;
    /// Multiplication of this matrix by given scalar.
    inline TMatrix2<T> operator * (const double d) const;
    /// Multiplication of this matrix by the given vector.
    inline TVect2<T> operator * (const TVect2<T> &u) const;
    /// Multiplication of this matrix by given matrix.
    TMatrix2<T> operator * (const TMatrix2<T> &v) const;
    /// Matrix by scalar division.
    inline TMatrix2<T> operator / (const double d) const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given matrix to this matrix.
    inline TMatrix2<T> & operator = (const TMatrix2<T> &u);
    /// Addition of the given matrix to this matrix.
    inline TMatrix2<T> & operator += (const TMatrix2<T> &u);
    /// Subtraction of the given matrix from this matrix.
    inline TMatrix2<T> & operator -= (const TMatrix2<T> &u);
    /// Multiplication of this matrix by given scalar.
    inline TMatrix2<T> & operator *= (const double d);
    /// Multiplication of this matrix by given matrix.
    inline TMatrix2<T> & operator *= (const TMatrix2<T> &u);
    /// Matrix by scalar division.
    inline TMatrix2<T> & operator /= (const double d);
    //@}

    /// @name Other methods.
    //@{
    /// Set of the j-th (j = 0, 1) column of the matrix to the given 2D vector.
    inline void SetCol(int j, const TVect2<T> &u);
    /// Getting of the j-th (j = 0, 1) column of this matrix.
    inline TVect2<T> GetCol(int j) const;
    /// Determinant of this matrix.
    inline double Det() const;
    /// Get inversed matrix.
    inline TMatrix2<T> Inversed() const;
    /// Get transposed matrix.
    TMatrix2<T> Transposed() const;
    /// Transpose this matrix.
    inline void Transpose();
    //@}

    /// @name Rotation and scale matrices.
    //@{
    /// @brief Normalize rows of this matrix, returns new math vector
    /// consisting of lengths of the rows before normalization.
    Vect2d Normalize();
    /// Rotation matrix.
    TMatrix2<T> &Rotation(double angle);
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 2x2 matrices.
    //@{
    /// Serialize the matrix using identity one as default.
    void Serialize(Serializer &inout, const char *tag);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}
    /// @name Predefined identity matrix.
    //@{
    /// Predefined identity matrix.
    static const TMatrix2<T> &E();
    //@}

  public:
    /// Members
    /// 2x2 matrix is implemented as the 2 rows of 2D vectors

    /// First row of the matrix 2x2.
    TVect2<T> r0,
    /// Second row of the matrix 2x2.
              r1;
  };

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 2x2 matrix with elements of float type.
typedef TMatrix2<float>  Matrix2f;
/// Predefined instance of 2x2 matrix with elements of double type.
typedef TMatrix2<double> Matrix2d;

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
///
template <class T>
TMatrix2<T>::TMatrix2()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given value.
///
/// Diagonal elements of the matrix are initialized by the same value @a a. The
/// rest elements of the matrix are initialized by the 0.
/// @param a - Given value.
///
template <class T>
TMatrix2<T>::TMatrix2(const double a)
  {
  r0[0] = r1[1] = (T)a;
  r0[1] = r1[0] = 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
///
/// Diagonal elements of the matrix are initialized by the given values
/// according to its order. The rest elements of the matrix are initialized by 0.
///
/// @param a - Value for the (0,0) element of the matrix.
/// @param b - Value for the (1,1) element of the matrix.
///
template <class T>
TMatrix2<T>::TMatrix2(const double a, const double b)
  {
  r0[0] = (T)a;
  r1[1] = (T)b;
  r0[1] = r1[0] = 0.0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given all components.
///
/// @param[in] a00 Value for the (0,0) element of the matrix.
/// @param[in] a01 Value for the (0,1) element of the matrix.
/// @param[in] a10 Value for the (1,0) element of the matrix.
/// @param[in] a11 Value for the (1,1) element of the matrix.
template <class T>
TMatrix2<T>::TMatrix2(double a00, double a01, double a10, double a11)
  {
  r0[0] = (T)a00;
  r0[1] = (T)a01;
  r1[0] = (T)a10;
  r1[1] = (T)a11;
  }


///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given 2D vectors.
///
/// Rows of the matrix are initialized by the given 2D vectors according
/// to its order cf. first row of the matrix is initialized by the first vector,
/// second row is initialized by the second vector and so on.
///
template <class T>
TMatrix2<T>::TMatrix2(const TVect2<T> &x, const TVect2<T> &y)
  {
  r0 = x;
  r1 = y;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param mat - Source 2x2 matrix.
///
template <class T>
TMatrix2<T>::TMatrix2(const TMatrix2<T> &mat)
  {
  r0 = mat.r0;
  r1 = mat.r1;
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (equality).
///
/// The two matrices are equal if the all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 2x2 matrix.
/// @return true - if this matrix is equal to given matrix, false - otherwise.
///
template <class T>
bool TMatrix2<T>::operator == (const TMatrix2<T> &u) const
  {
  return (r0 == u.r0) && (r1 == u.r1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (inequality).
///
/// The two matrices are equal if the all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 2x2 matrix.
/// @return true - if this matrix is not equal to given matrix, false -
/// otherwise.
///
template <class T>
bool TMatrix2<T>::operator != (const TMatrix2<T> &u) const
  {
  return (r0 != u.r0) || (r1 != u.r1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with given tolerance.
///
/// @param u - Given matrix 2x2.
/// @param tolerance - Given tolerance
/// @return true if - all elements of the given matrix less or equal (by
/// absolute value) than value of given tolerance, false - otherwise.
///
template <class T>
bool TMatrix2<T>::AboutZero(const TMatrix2<T> &u, const double tolerance)
  {
  return (Math2<T>::AboutZero(u[0], tolerance) &&
          Math2<T>::AboutZero(u[1], tolerance));
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with predefined tolerance Math<T>::TOLERANCE.
///
/// @note Value of Math<T>::TOLERANCE is dependent from type T.
/// @param u - Given matrix 2x2.
/// @return true if all elements of the given matrix less or equal (by absolute
/// value)  than predefined tolerance Math<T>::TOLERANCE.
///
template <class T>
bool TMatrix2<T>::AboutZero(const TMatrix2<T> &u)
  {
  return AboutZero(u, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with given tolerance.
///
/// Method calculates difference between given matrices (creates new temporary
/// 2x2 matrix) and compares the all elements of the created matrix with given
/// tolerance.
///
/// @param a - First matrix 2x2.
/// @param b - Second matrix 2x2.
/// @param tolerance - Given tolerance.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than given tolerance,
/// false - otherwise.
///
template <class T>
bool TMatrix2<T>::AboutEqual(const TMatrix2<T> &a, const TMatrix2<T> &b,
                              const double tolerance)
  {
  return AboutZero(a - b, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given matrices (creates new temporary
/// 2x2 matrix) and compares the all elements of the created matrix with
/// tolerance Math<T>::TOLERANCE.
///
/// @param a - First matrix 2x2.
/// @param b - Second matrix 2x2.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than tolerance
/// Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool TMatrix2<T>::AboutEqual(const TMatrix2<T> &a,
                             const TMatrix2<T> &b)
  {
  return AboutZero(a - b, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1).
///
/// @note i must be in range 0..1. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1).
/// @return Reference to the 2D vector - i-th row of the matrix.
///
template <class T>
TVect2<T> & TMatrix2<T>::operator [] (int i)
  {
  Assert(i >= 0 && i < 2);;
  return ((TVect2<T> *)&r0)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1).
///
/// @note i must be in range 0..1. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1).
/// @return Constant reference to the 2D vector - i-th row of the matrix.
///
template <class T>
const TVect2<T> &TMatrix2<T>::operator [] (int i) const
  {
  Assert(i >= 0 && i < 2);;
  return ((const TVect2<T> *)&r0)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the j-th (j = 0, 1) column of the matrix to the given 2D vector.
///
/// Elements of the matrix in the j-th (j = 0, 1) column are replaced by the
/// corresponding elements of the given 2D vector.
///
/// @note j must be 0, 1. Debug version asserts this.
/// @param j - Index of the column (0, 1)
/// @param u - Given 2D vector.
/// @return None (this matrix is updated)
///
template <class T>
void TMatrix2<T>::SetCol(int j, const TVect2<T> &u)
  {
  Assert(j >= 0 && j < 2);
  r0[j] = u[0];
  r1[j] = u[1];
  }

///////////////////////////////////////////////////////////////////////////////
/// Getting of the j-th (j = 0, 1) column of this matrix.
///
/// Method creates a new 2D vector from j-th (j = 0 or 1) column of this
/// matrix.
///
/// @note j must be 0 or 1. Debug version asserts this.
/// @param j - Index of the column (0 or 1).
/// @return New 2D vector (j-th column).
///
template <class T>
TVect2<T> TMatrix2<T>::GetCol(int j) const
  {
  Assert(j >= 0 && j < 2);
  TVect2<T> u;
  u[0] = r0[j];
  u[1] = r1[j];
  return u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the matrices.
///
/// Method creates a new matrix 2x2. Elements of the new matrix are the sums
/// of the elements of this matrix with corresponding elements of the given
/// matrix.
///
/// @param v - Given matrix 2x2.
/// @return New matrix 2x2 (sum of the matrices).
///
template <class T>
TMatrix2<T> TMatrix2<T>::operator + (const TMatrix2<T> &v) const
  {
  TMatrix2<T> res;
  res.r0 = r0 + v.r0;
  res.r1 = r1 + v.r1;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the matrices.
///
/// Method creates a new matrix 2x2. Elements of the new matrix are the
/// differences between elements of this matrix (minuend) and corresponding
/// elements of the given matrix (subtrahend).
///
/// @param v - Given 2x2 matrix (subtrahend)
/// @return New 2x2 matrix (difference between two matrices)
///
template <class T>
TMatrix2<T> TMatrix2<T>::operator - (const TMatrix2<T> &v) const
  {
  TMatrix2<T> res;
  res.r0 = r0 - v.r0;
  res.r1 = r1 - v.r1;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Method creates a new 2x2 matrix. Elements of the new matrix are the products
/// of the elements of this matrix and given scalar.
///
/// @param d - Given scalar (multiplier).
/// @return New 2x2 matrix (product).
///
template <class T>
TMatrix2<T> TMatrix2<T>::operator * (const double d) const
  {
  TMatrix2<T> res;
  res.r0 = r0 * d;
  res.r1 = r1 * d;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by the given vector.
///
/// Method creates a new 2D vector. Elements of a new vector are the Dot
/// products (see function DotProd()) of the rows of this matrix and given
/// vector cf. matrix is multiplied by the vector from the right.
///
/// @param u - Given vector (multiplier).
/// @return New 2D vector (product).
///
template <class T>
TVect2<T> TMatrix2<T>::operator * (const TVect2<T> &u) const
  {
  TVect2<T> res;
  res[0] = (T)DotProd(r0, u);
  res[1] = (T)DotProd(r1, u);
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix by scalar division.
///
/// Method creates a new 2x2 matrix. Elements of the new matrix are the results
/// of the division of the elements of this matrix by the given scalar.
///
/// @note Value d may not be equal to zero.
///       Debug version asserts about this.
/// @param d - Given scalar (divisor).
/// @return New 2x2 matrix.
///
template <class T>
TMatrix2<T> TMatrix2<T>::operator / (const double d) const
  {
  TMatrix2<T> res;
  Assert(d != 0);
  res.r0 = r0 / d;
  res.r1 = r1 / d;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the elements of the given matrix.
///
/// @param u - Given matrix 2x2.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator = (const TMatrix2<T> &u)
  {
  r0 = u.r0;
  r1 = u.r1;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the sums of corresponding elements
/// of this matrix and given matrix.
///
/// @param u - Given matrix 2x2.
/// @return Reference to this matrix (the sum of two matrices).
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator += (const TMatrix2<T> &u)
  {
  r0 += u.r0;
  r1 += u.r1;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given matrix from this matrix.
///
/// Elements of this matrix are replaced by the differences between
/// corresponding elements of this matrix and given matrix.
///
/// @param u - Given matrix 2x2 (subtrahend)
/// @return Reference to this matrix (the difference between two matrices)
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator -= (const TMatrix2<T> &u)
  {
  r0 -= u.r0;
  r1 -= u.r1;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Elements of this matrix are replaced by the products of these elements and
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator *= (const double d)
  {
  r0 *= d;
  r1 *= d;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Elements of this matrix are replaced by the Dot products (see function
/// DotProd()) of the rows of this matrix and columns of the given matrix. Let
/// i - index of the row (i = 0, 1), j - index of the column (j = 0, 1) then
/// new element (i, j) of this matrix is Dot product of the i-th row of this
/// matrix and j-th column of the given matrix.
///
/// @param u - Given matrix 2x2 (multiplier).
/// @return Reference to this matrix (updated)
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator *= (const TMatrix2<T> &u)
  {
  *this = (*this) * u;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix by scalar division.
///
/// Elements of this matrix are replaced by the results of division of
/// these elements by given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version ASSERTs it.
///
/// @param d - Given scalar.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix2<T> &TMatrix2<T>::operator /= (const double d)
  {
  Assert(d != 0);
  r0 /= d;
  r1 /= d;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Determinant of this matrix.
/// @return Value of the determinant.
///
template <class T>
double TMatrix2<T>::Det() const
  {
  return r0[0] * r1[1] - r0[1] * r1[0];
  }

///////////////////////////////////////////////////////////////////////////////
/// Get inversed matrix.
///
/// Method creates a new matrix 2x2 which inverse to this matrix cf. product
/// of the new matrix with this matrix is identity matrix.
///
/// @note Determinant of this matrix must not be equal to zero.
///       Debug version asserts about this.
/// @return New 2x2 matrix (inverse to this matrix).
///
template <class T>
TMatrix2<T> TMatrix2<T>::Inversed() const
  {
  double det = Det();
  Assert(det != 0);
  TMatrix2<T> res;
  const double rdet = 1 / det;
  res[0][0] =  (T)(r1[1] * rdet);
  res[1][0] = (T)(-r1[0] * rdet);
  res[0][1] = (T)(-r0[1] * rdet);
  res[1][1] =  (T)(r0[0] * rdet);
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Transpose this matrix.
///
/// Method modifies this matrix by the swapping between rows and
/// columns cf. rows of the modified matrix are the columns of the initial
/// matrix and vice versa.
///
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix2<T>::Transpose()
  {
  Swap(r0[1], r1[0]);
  }

// ----------------------------------------------------
//             TMatrix2 SERVICE DEFINISIONS
// ----------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiplication of the given matrix by the given scalar.
///
/// Function creates a new matrix. Elements of the new matrix are the products
/// of the elements of the given matrix and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 2x2 matrix.
/// @return New 2x2 matrix.
///
template <class T>
inline TMatrix2<T> operator * (const double d, const TMatrix2<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiplication of the given matrix by the given vector.
///
/// Function creates a new 2D vector. Elements of a new vector are the Dot
/// products (see function DotProd()) of the columns of the given matrix and
/// given vector cf. matrix is multiplied by the vector from the left.
///
/// @param u - Given 2D vector (multiplier).
/// @param a - Given 2x2 matrix (multiplier).
/// @return New 3D vector (product).
///
template <class T>
inline TVect2<T> operator * (const TVect2<T> &u, const TMatrix2<T> &a)
  {
  TVect2<T> res;
  res[0] = u[0] * a[0][0] + u[1] * a[1][0];
  res[1] = u[0] * a[0][1] + u[1] * a[1][1];
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiplication of the given matrix by the given point.
///
/// Function creates a new 2D point. Elements of a new point are the Dot 
/// products (see function DotProd()) of the columns of the given matrix and 
/// given point cf. matrix is multiplied by the vector (point) from the left. 
///
/// @param u - Given 2D point (multiplier).
/// @param a - Given 2x2 matrix (multiplier).
/// @return New 3D point (product).
///
template <class T>
inline TPoint2<T> operator * (const TPoint2<T> &u, const TMatrix2<T> &a)
  {
  TPoint2<T> res;
  res[0] = u[0] * a[0][0] + u[1] * a[1][0];
  res[1] = u[0] * a[0][1] + u[1] * a[1][1];
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with float elements.
///
/// @param v - 2x2 matrix with elements of the arbitrary type.
/// @return 2x2 matrix with float elements.
///
template <class T>
inline TMatrix2<float> ConvF(const TMatrix2<T> &v)
  {
  TMatrix2<float> res;

  // converts the rows as TVect2
  res[0] = ConvF(v[0]);
  res[1] = ConvF(v[1]);

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with double elements.
///
/// @param v - 2x2 matrix with elements of the arbitrary type.
/// @return 2x2 matrix with double elements.
///
template <class T>
inline TMatrix2<double> ConvD(const TMatrix2<T> &v)
  {
  TMatrix2<double> res;

  // converts the rows as TVect2
  res[0] = ConvD(v[0]);
  res[1] = ConvD(v[1]);

  return res;
  }

INTEGRA_NAMESPACE_END

#endif //_KLBC_MATRIX2_HPP_
