/// @file
///
/// @brief Definition of template class for transormation matrices 4x4
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Enk - Nikolay Kirilov<BR>
///         Vai - Alexey Ignatenko

#ifndef _ELGO_MATRIX4_HPP_
#define _ELGO_MATRIX4_HPP_

#include "matrix43.hpp"
#include "vect4.hpp"

INTEGRA_NAMESPACE_START

// -----------------------------------------------------------------
//                Template class TMatrix4
// -----------------------------------------------------------------

/// Matrix 4x4 of the arbitrary type.
template <class T>
class TMatrix4
  {
  public:
    /// @name  Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TMatrix4();
    /// Constructor from the given value.
    inline TMatrix4(const double a);
    /// Constructor from the given values.
    inline TMatrix4(const double a, const double b,
                    const double c, const double d);
    /// Constructor from the given vectors.
    inline TMatrix4(const TVect4<T> &x, const TVect4<T> &y,
                    const TVect4<T> &z, const TVect4<T> &w);
    ///  Copy constructor.
    inline TMatrix4(const TMatrix4<T> &mat);
    /// Constructor from the given 4x3 matrix.
    explicit inline TMatrix4(const TMatrix43<T> &mat);
    /// Constructor from the given 3x3 matrix.
    explicit inline TMatrix4(const TMatrix3<T> &mat);
    //@}

    /// @name  Element access.
    //@{
    /// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
    inline TVect4<T> & operator [] (int i);
    /// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
    inline const TVect4<T> &operator [] (int i) const;
    /// @brief Set of the j-th (j = 0, 1, 2, 3) column of the matrix to the
    /// given 4D vector.
    inline void SetCol(int j, const TVect4<T> &u);
    /// Getting of the j-th (j = 0, 1, 2, 3) column of this matrix.
    inline TVect4<T> GetCol(int j) const;
    //@}

    /// @name  Comparison operators and methods.
    //@{
    /// Comparison of this matrix with given matrix (equality).
    inline bool operator == (const TMatrix4<T> &u) const;
    /// Comparison of this matrix with given matrix (inequality).
    inline bool operator != (const TMatrix4<T> &u) const;
    //@}

    /// @name Arithmetic operators.
    //@{
    /// Addition of the matrices.
    inline TMatrix4<T> operator + (const TMatrix4<T> &v) const;
    /// Subtraction of the matrices.
    inline TMatrix4<T> operator - (const TMatrix4<T> &v) const;
    /// Multiplication of this matrix by given scalar.
    inline TMatrix4<T> operator * (const double d) const;
    /// Multiplication of this matrix by the given vector.
    inline TVect4<T> operator * (const TVect4<T> &u) const;
    /// Multiplication of this matrix by given matrix.
    TMatrix4<T> operator * (const TMatrix4<T> &v) const;
    /// Matrix by scalar division.
    inline TMatrix4<T> operator / (const double d) const;
    //@}

    /// @name  Assignment operators.
    //@{
    /// Assignment of the given matrix to this matrix.
    inline TMatrix4<T> & operator = (const TMatrix4<T> &u);
    /// Addition of the given matrix to this matrix.
    inline TMatrix4<T> & operator += (const TMatrix4<T> &u);
    /// Subtraction of the given matrix from this matrix.
    inline TMatrix4<T> & operator -= (const TMatrix4<T> &u);
    /// Multiplication of this matrix by given matrix.
    TMatrix4<T> & operator *= (const TMatrix4<T> &u);
    /// Multiplication of this matrix by given scalar.
    inline TMatrix4<T> & operator *= (const double d);
    /// Matrix by scalar division.
    inline TMatrix4<T> & operator /= (const double d);
    //@}

    /// @name Comparison with tolerance.
    //@{
    /// Comparison with given tolerance.
    inline static bool AboutZero(const TMatrix4<T> &u, const double tolerance);
    /// Comparison with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TMatrix4<T> &u);
    /// Comparison of the matrices with given tolerance.
    inline static bool AboutEqual(const TMatrix4<T> &a, const TMatrix4<T> &b,
                                  const double tolerance);
    /// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TMatrix4<T> &a,
                                  const TMatrix4<T> &b);
    //@}

    /// @name Other methods.
    //@{
    /// Creation of the 4x3 matrix from 4x4 matrix.
    inline TMatrix43<T> Matrix43() const;
    /// Transpose this matrix.
    inline void Transpose();
    /// Get transposed matrix.
    TMatrix4<T> Transposed() const;
    /// Set of the matrix to the identity matrix.
    inline void Identity();
    /// @brief Normalize rows of this matrix, returns new math vector 
    /// consisting of lengths of the rows before normalization.
    Vect4d Normalize();
    /// Scaling matrix.
    inline TMatrix4<T> &Scale(const double a, const double b,
                              const double c, const double d);
    /// Get inversed matrix.
    TMatrix4<T> Inversed() const;
    /// Determinant of this matrix.
    double Det() const;
    /// Check that all elements of matrix are valid floats.
    bool IsOK() const;
    //@}

    /// @name Projective matrices.
    //@{
    /// @brief Perspective projection matrix (truncated pyramid is defined by
    /// the view angle).
    void Perspective(double fovy, double aspect, double z_near, double z_far);
    /// @brief Perspective projection matrix (truncated pyramid is defined by
    /// the view angle and far clipping plane at the infinity).
    void InfinitePerspective(double fovy, double aspect, double z_near);
    /// @brief Perspective projection matrix (truncated pyramid is defined by
    /// the two poins).
    void Frustum(double left, double right, double bottom, double top,
                 double z_near, double z_far);
    /// Perspective projection matrix for infinte far plane.
    void InfiniteFrustum(double left, double right, double bottom, double top,
                         double z_near);
    /// Orthographic projection matrix.
    void Ortho(double left, double right, double bottom, double top,
               double z_near, double z_far);
    /// Orthographic projection matrix for far clipping plane at the infinity.
    void InfiniteOrtho(double left, double right, double bottom, double top,
               double z_near, double z_far);
    //@}

    /// @name Transformation point and bounding box.
    //@{
    /// Transformation of the point.
    TVect4<T> PointTransform(const TPoint3<T> &p) const;
    /// Check inclusion to CVV point after transformation
    bool Includes(const TPoint3<T> &point) const;
    /// Check intersection with CVV after transformation
    bool Visible(const BBox3<T> &bbox) const;
    //@}

  public:
    // 4x4 matrix is 4 rows of 4D vectors

    /// First row of the matrix
    TVect4<T> r0,
    /// Second row of the matrix
              r1,
    /// Third row of the matrix
              r2,
    /// Fourth row of the matrix
              r3;
  };

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 4x4 matrix with elements of float type.
typedef TMatrix4<float>   Matrix4f;
/// Predefined instance of 4x4 matrix with elements of float type.
typedef TMatrix4<double>  Matrix4d;

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
///
template <class T>
TMatrix4<T>::TMatrix4()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given value.
///
/// Diagonal elements of the matrix are initialized by the same value @a a. The
/// rest elements of the matrix are initialized by the 0.
///
/// @param a - Given value.
///
template <class T>
TMatrix4<T>::TMatrix4(const double a)
  {
  r0[0] = r1[1] = r2[2] = r3[3] = (T)a;
          r0[1] = r0[2] = r0[3] = 0.0;
  r1[0]         = r1[2] = r1[3] = 0.0;
  r2[0] = r2[1]         = r2[3] = 0.0;
  r3[0] = r3[1] = r3[2]         = 0.0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
///
/// Diagonal elements of the matrix are initialized by the given values
/// according to its order. The rest elements of the matrix are initialized
/// by 0.
///
/// @param a - Value for the (0, 0) element.
/// @param b - Value for the (1, 1) element.
/// @param c - Value for the (2, 2) element.
/// @param d - Value for the (3, 3) element.
///
template <class T>
TMatrix4<T>::TMatrix4(const double a, const double b,
                      const double c, const double d)
  {
  r0[0] = (T)a;  r1[1] = (T)b;  r2[2] = (T)c;  r3[3] = (T)d;
          r0[1] = r0[2] = r0[3] = 0.0;
  r1[0]         = r1[2] = r1[3] = 0.0;
  r2[0] = r2[1]         = r2[3] = 0.0;
  r3[0] = r3[1] = r3[2]         = 0.0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given vectors.
///
/// Rows of the matrix are initialized by the elements of the given vectors
/// according to its order.
///
/// @param x - 4D vector for the first row.
/// @param y - 4D vector for the second row.
/// @param z - 4D vector for the third row.
/// @param w - 4D vector for the fourth row.
///
template <class T>
TMatrix4<T>::TMatrix4(const TVect4<T> &x, const TVect4<T> &y,
                      const TVect4<T> &z, const TVect4<T> &w)
  {
  r0 = x;
  r1 = y;
  r2 = z;
  r3 = w;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param mat - Source 4x4 matrix.
///
template <class T>
TMatrix4<T>::TMatrix4(const TMatrix4<T> &mat)
  {
  r0 = mat.r0;
  r1 = mat.r1;
  r2 = mat.r2;
  r3 = mat.r3;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given 4x3 matrix.
///
/// The first three columns of this matrix are initialized by the corresponding
/// columns of the given matrix. The rest fourth column is initialized by the
/// vector (0, 0, 0, 1).
///
/// @param mat - 4x3 matrix for the first three columns.
///
template <class T>
TMatrix4<T>::TMatrix4(const TMatrix43<T> &mat)
  {
  r0[0] = mat[0][0]; r0[1] = mat[0][1]; r0[2] = mat[0][2]; r0[3] = 0;
  r1[0] = mat[1][0]; r1[1] = mat[1][1]; r1[2] = mat[1][2]; r1[3] = 0;
  r2[0] = mat[2][0]; r2[1] = mat[2][1]; r2[2] = mat[2][2]; r2[3] = 0;
  r3[0] = mat[3][0]; r3[1] = mat[3][1]; r3[2] = mat[3][2]; r3[3] = 1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given 3x3 matrix.
///
/// Elements of the first three rows and columns are initialized by the
/// corresponding elements of the given 3x3 matrix. The last fourth column and
/// fourth row are initialized by the vector (0, 0, 0, 1).
///
/// @param mat - 3x3 matrix for the elements of the first three rows and
/// columns.
///
template <class T>
TMatrix4<T>::TMatrix4(const TMatrix3<T> &mat)
  {
  r0[0] = mat[0][0]; r0[1] = mat[0][1]; r0[2] = mat[0][2]; r0[3] = 0;
  r1[0] = mat[1][0]; r1[1] = mat[1][1]; r1[2] = mat[1][2]; r1[3] = 0;
  r2[0] = mat[2][0]; r2[1] = mat[2][1]; r2[2] = mat[2][2]; r2[3] = 0;
  r3[0] = 0;         r3[1] = 0;         r3[2] = 0;         r3[3] = 1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
///
/// @note i must be in range 0..3. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2, 3).
/// @return Reference to the 4D vector - i-th row of the matrix.
///
template <class T>
TVect4<T> &TMatrix4<T>::operator [] (int i)
  {
  Assert(i >= 0 && i < 4);;
  return (&r0)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
///
/// @note i must be in range 0..3. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2, 3).
/// @return Reference to the 4D vector - i-th row of the matrix.
///
template <class T>
const TVect4<T> &TMatrix4<T>::operator [] (int i) const
  {
  Assert(i >= 0 && i < 4);
  return (&r0)[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the j-th (j = 0, 1, 2, 3) column of the matrix to the given 4D
/// vector.
///
/// Elements of the matrix in the j-th (j = 0, 1, 2, 3) column are replaced by
/// the corresponding elements of the given 4D vector.
///
/// @note j must be 0, 1, 2 or 3. Debug version asserts this.
/// @param j - Index of the column (0, 1, 2 or 3)
/// @param u - Given 4D vector.
/// @return None (this matrix is updated)
///
template <class T>
void TMatrix4<T>::SetCol(int j, const TVect4<T> &u)
  {
  Assert(j >= 0 && j < 4);
  r0[j] = u[0];
  r1[j] = u[1];
  r2[j] = u[2];
  r3[j] = u[3];
  }

//////////////////////////////////////////////////////////////////////////////
/// Getting of the j-th (j = 0, 1, 2, 3) column of this matrix.
///
/// Method creates a new 4D vector from j-th (j = 0, 1, 2 or 3) column of this
/// matrix.
///
/// @note j must be 0, 1, 2 or 3. Debug version asserts this.
/// @param j - Index of the column (0, 1, 2 or 3).
/// @return New 4D vector (j-th column).
///
template <class T>
TVect4<T> TMatrix4<T>::GetCol(int j) const
  {
  Assert(j >= 0 && j < 4);
  TVect4<T> u;
  u[0] = r0[j];
  u[1] = r1[j];
  u[2] = r2[j];
  u[3] = r3[j];
  return u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (equality).
///
/// The two matrices are equal if all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 4x4 matrix.
/// @return true - if this matrix is equal to given matrix, false - otherwise.
///
template <class T>
bool TMatrix4<T>::operator == (const TMatrix4<T> &u) const
  {
  return (r0 == u.r0) && (r1 == u.r1) && (r2 == u.r2) && (r3 == u.r3);
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (inequality).
///
/// The two matrices are equal if the all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 4x4 matrix.
/// @return true - if this matrix is not equal to given matrix, false -
/// otherwise.
///
template <class T>
bool TMatrix4<T>::operator != (const TMatrix4<T> &u) const
  {
  return (r0 != u.r0) || (r1 != u.r1) || (r2 != u.r2) || (r3 != u.r3);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with given tolerance.
///
/// @param u - Given matrix 4x4.
/// @param tolerance - Given tolerance
/// @return true if - all elements of the given matrix less or equal (by
/// absolute value) than value of given tolerance, false - otherwise.
///
template <class T>
bool TMatrix4<T>::AboutZero(const TMatrix4<T> &u, const double tolerance)
  {
  return (Math4<T>::AboutZero(u[0], tolerance) &&
          Math4<T>::AboutZero(u[1], tolerance) &&
          Math4<T>::AboutZero(u[2], tolerance) &&
          Math4<T>::AboutZero(u[3], tolerance));
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with predefined tolerance Math<T>::TOLERANCE.
///
/// @note Value of Math<T>::TOLERANCE is dependent from type T.
/// @param u - Given matrix 4x4.
/// @return true if all elements of the given matrix less or equal (by absolute
/// value) than predefined tolerance Math<T>::TOLERANCE.
///
template <class T>
bool TMatrix4<T>::AboutZero(const TMatrix4<T> &u)
  {
  return AboutZero(u, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with given tolerance.
///
/// Method calculates difference between given matrices (creates new temporary
/// 4x4 matrix) and compares the all elements of the created matrix with given
/// tolerance.
///
/// @param a - First matrix 4x4.
/// @param b - Second matrix 4x4.
/// @param tolerance - Given tolerance.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than given tolerance,
/// false - otherwise.
///
template <class T>
bool TMatrix4<T>::AboutEqual(const TMatrix4<T> &a, const TMatrix4<T> &b,
                              const double tolerance)
  {
  return AboutZero(a - b, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given matrices (creates new temporary
/// 4x4 matrix) and compares the all elements of the created matrix with
/// tolerance Math<T>::TOLERANCE.
///
/// @param a - First matrix 4x4.
/// @param b - Second matrix 4x4.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than tolerance
/// Math<T>::TOLERANCE, false - otherwise.
///
template <class T>
bool TMatrix4<T>::AboutEqual(const TMatrix4<T> &a,
                             const TMatrix4<T> &b)
  {
  return AboutZero(a - b, Math<T>::TOLERANCE);
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of the matrices.
///
/// Method creates a new matrix 4x4. Elements of the new matrix are the sums
/// of the elements of this matrix with corresponding elements of the given
/// matrix.
///
/// @param v - Given matrix 4x4.
/// @return New matrix 4x4 (sum of the matrices).
///
template <class T>
TMatrix4<T> TMatrix4<T>::operator + (const TMatrix4<T> &v) const
  {
  TMatrix4<T> res;
  res.r0 = r0 + v.r0;
  res.r1 = r1 + v.r1;
  res.r2 = r2 + v.r2;
  res.r3 = r3 + v.r3;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction of the matrices.
///
/// Method creates a new matrix 4x4. Elements of the new matrix are the
/// differences between elements of this matrix (minuend) and corresponding
/// elements of the given matrix (subtrahend).
///
/// @param v - Given matrix (subtrahend)
/// @return New matrix 4x4 (difference between two matrices)
///
template <class T>
TMatrix4<T> TMatrix4<T>::operator - (const TMatrix4<T> &v) const
  {
  TMatrix4<T> res;
  res.r0 = r0 - v.r0;
  res.r1 = r1 - v.r1;
  res.r2 = r2 - v.r2;
  res.r3 = r3 - v.r3;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Method creates a new matrix 4x4. Elements of the new matrix are the
/// products of the elements of this matrix and given scalar.
///
/// @param d - Given scalar (multiplier).
/// @return New 4x4 matrix (product).
///
template <class T>
TMatrix4<T> TMatrix4<T>::operator * (const double d) const
  {
  TMatrix4<T> res;
  res.r0 = r0 * d;
  res.r1 = r1 * d;
  res.r2 = r2 * d;
  res.r3 = r3 * d;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by the given vector.
///
/// Method creates a new 4D vector. Elements of a new vector are the Dot
/// products (see function DotProd4()) of the rows of this matrix and given
/// vector cf. matrix is multiplied by the vector from the right.
///
/// @param u - Given vector (multiplier)
/// @return New 4D vector (product).
///
template <class T>
TVect4<T> TMatrix4<T>::operator * (const TVect4<T> &u) const
  {
  TVect4<T> res;
  res.x = (T)DotProd4(r0, u);
  res.y = (T)DotProd4(r1, u);
  res.z = (T)DotProd4(r2, u);
  res.w = (T)DotProd4(r3, u);
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix by scalar division.
///
/// Method creates a new 4x4 matrix. Elements of the new matrix are the results
/// of the division of the elements of this matrix by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version asserts about
/// this.
/// @param d - Given scalar (divisor).
/// @return New matrix 4x4.
///
template <class T>
TMatrix4<T> TMatrix4<T>::operator / (const double d) const
  {
  TMatrix4<T> res;
  Assert(d != 0);
  res.r0 = r0 / d;
  res.r1 = r1 / d;
  res.r2 = r2 / d;
  res.r3 = r3 / d;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the elements of the given matrix.
///
/// @param u - Given matrix 4x4.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix4<T> & TMatrix4<T>::operator = (const TMatrix4<T> &u)
  {
  r0 = u.r0;
  r1 = u.r1;
  r2 = u.r2;
  r3 = u.r3;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the sums of corresponding elements
/// of this matrix and given matrix.
///
/// @param u - Given matrix 4x4.
/// @return Reference to this matrix (the sum of two matrices).
///
template <class T>
TMatrix4<T> &TMatrix4<T>::operator += (const TMatrix4<T> &u)
  {
  r0 += u.r0;
  r1 += u.r1;
  r2 += u.r2;
  r3 += u.r3;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given matrix from this matrix.
///
/// Elements of this matrix are replaced by the differences between
/// corresponding elements of this matrix and given matrix.
///
/// @param u - Given matrix (subtrahend)
/// @return Reference to this matrix (difference between two matrices)
///
template <class T>
TMatrix4<T> &TMatrix4<T>::operator -= (const TMatrix4<T> &u)
  {
  r0 -= u.r0;
  r1 -= u.r1;
  r2 -= u.r2;
  r3 -= u.r3;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Elements of this matrix are replaced by the Dot products (see function
/// DotProd4()) of the rows of this matrix and columns of the given matrix. Let i -
/// index of the row (i = 0, 1, 2, 3), j - index of the column (j = 0, 1, 2, 3)
/// then new element (i, j) of this matrix is Dot product of the i-th row of
/// this matrix and j-th column of the given matrix.
///
/// @param u - Given matrix 4x4 (multiplier).
/// @return Reference to this matrix (updated)
///
template <class T>
TMatrix4<T> &TMatrix4<T>::operator *= (const TMatrix4<T> &u)
  {
  *this = (*this) * u;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Elements of this matrix are replaced by the products of these elements and
/// given scalar.
///
/// @param d - Given scalar.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix4<T> &TMatrix4<T>::operator *= (const double d)
  {
  r0 *= d;
  r1 *= d;
  r2 *= d;
  r3 *= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
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
TMatrix4<T> &TMatrix4<T>::operator /= (const double d)
  {
  Assert(d != 0);
  r0 /= d;
  r1 /= d;
  r2 /= d;
  r3 /= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transpose this matrix.
///
/// Method modifies this matrix by the swapping between rows and columns cf.
/// rows of the modified matrix are the columns of the initial matrix and
/// vice versa.
///
/// @return None (this matrix is mofified).
///
template <class T>
void TMatrix4<T>::Transpose()
  {
  Swap(r0[1], r1[0]);
  Swap(r0[2], r2[0]);
  Swap(r1[2], r2[1]);

  Swap(r0[3], r3[0]);
  Swap(r1[3], r3[1]);
  Swap(r2[3], r3[2]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set of the matrix to the identity matrix.
///
/// The diagonal elements of this matrix are set to 1.0. The rest elements
/// of this matrix are set to 0.
///
/// @return None (this matrix is affected only).
///
template <class T>
void TMatrix4<T>::Identity()
  {
  r0[0] = r1[1] = r2[2] = r3[3] = 1;
          r0[1] = r0[2] = r0[3] = 0.0;
  r1[0]         = r1[2] = r1[3] = 0.0;
  r2[0] = r2[1]         = r2[3] = 0.0;
  r3[0] = r3[1] = r3[2]         = 0.0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Scaling matrix.
///
/// Scaling matrix is diagonal matrix. Diagonal elements of the matrix are the
/// scale factors. The rest elements of the matrix are zeros.
///
/// Multiplication of this matrix with some vector scales coordinates
/// of the vector.
///
/// @param a - Scale factors for X.
/// @param b - Scale factors for Y.
/// @param c - Scale factors for Z.
/// @param d - Scale factors for W (fourth element of 4D vector).
///
template <class T>
TMatrix4<T> &TMatrix4<T>::Scale(const double a, const double b,
                             const double c, const double d)
  {
  r0[0] = (T)a;  r1[1] = (T)b;  r2[2] = (T)c;  r3[3] = (T)d;
          r0[1] = r0[2] = r0[3] = 0.0;
  r1[0]         = r1[2] = r1[3] = 0.0;
  r2[0] = r2[1] =         r2[3] = 0.0;
  r3[0] = r3[1] = r3[2]         = 0.0;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Creation of the 4x3 matrix from 4x4 matrix.
///
/// Method creates a new 4x3 matrix from the first three columns of this matrix.
/// @return New 4x3 matrix.
///
template <class T>
inline TMatrix43<T> TMatrix4<T>::Matrix43() const
  {
  TMatrix43<T> m;

  m[0][0] = r0.x; m[0][1] = r0.y; m[0][2] = r0.z;
  m[1][0] = r1.x; m[1][1] = r1.y; m[1][2] = r1.z;
  m[2][0] = r2.x; m[2][1] = r2.y; m[2][2] = r2.z;
  m[3][0] = r3.x; m[3][1] = r3.y; m[3][2] = r3.z;

  return m;
  }

// ---------------------------------------------------------------------------
//                   TMatrix4 SERVICE DEFINITIONS
// ---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of the given matrix by the given scalar.
///
/// Function creates a new matrix. Elements of the new matrix are the products
/// of the elements of the given matrix and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 4x4 matrix.
/// @return New 4x4 matrix.
///
template <class T>
TMatrix4<T> operator * (const double d, const TMatrix4<T> &u)
  {
  return u * d;
  }

//////////////////////////////////////////////////////////////////////////////
/// @brief Multiplication of the given matrix by the given vector.
///
/// Function creates a new 4D vector. Elements of a new vector are the Dot
/// products (see function DotProd4()) of the columns of the given matrix and
/// given vector cf. matrix is multiplied by the vector from the left.
///
/// @param u - Given 4D vector (multiplier).
/// @param a - Given 4x4 matrix (multiplier).
/// @return New 4D vector (product).
///
template <class T>
TVect4<T> operator * (const TVect4<T> &u, const TMatrix4<T> &a)
  {
  TVect4<T> res;
  res.x = u.x * a.r0.x + u.y * a.r1.x + u.z * a.r2.x + u.w * a.r3.x;
  res.y = u.x * a.r0.y + u.y * a.r1.y + u.z * a.r2.y + u.w * a.r3.y;
  res.z = u.x * a.r0.z + u.y * a.r1.z + u.z * a.r2.z + u.w * a.r3.z;
  res.w = u.x * a.r0.w + u.y * a.r1.w + u.z * a.r2.w + u.w * a.r3.w;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with float elements.
///
/// @param v - 4x4 matrix with elements of the arbitrary type.
/// @return 4x4 matrix with float elements.
///
template <class T>
TMatrix4<float> ConvF(const TMatrix4<T> &v)
  {
  return TMatrix4<float>(ConvF(v[0]), ConvF(v[1]), ConvF(v[2]));
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with double elements.
///
/// @param v - 4x4 matrix with elements of the arbitrary type.
/// @return 4x4 matrix with double elements.
///
template <class T>
TMatrix4<double> ConvD(const TMatrix4<T> &v)
  {
  return TMatrix4<double>(ConvD(v[0]), ConvD(v[1]), ConvD(v[2]));
  }

INTEGRA_NAMESPACE_END
#endif // _KLBC_MATRIX4_HPP_

