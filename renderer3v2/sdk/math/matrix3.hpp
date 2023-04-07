/// @file
///
/// @brief Definition of template class for matrices 3x3
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>

#ifndef _KLBC_MATRIX3_HPP_
#define _KLBC_MATRIX3_HPP_

#include "vect3.hpp"

INTEGRA_NAMESPACE_START

/// Matrix 3x3 of the arbitrary type.
template <class T>
class TMatrix3
  {
  public:
    /// @name  Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TMatrix3();
    /// Constructor from the given value.
    inline TMatrix3(const double a);
    /// Constructor from the given values.
    inline TMatrix3(const double a, const double b, const double c);
    /// Constructor from the given 3D vectors.
    inline TMatrix3(const TVect3<T> &x, const TVect3<T> &y, const TVect3<T> &z);
    /// Copy constructor
    inline TMatrix3(const TMatrix3<T> &mat);
    //@}

    /// @name Array to Matrix conversion.
    //@{
    /// Treatment of the array of the three elements as a constant vector.
    inline static const TMatrix3<T> &Cast(const T *u);
    /// Treatment of the array of the three elements as a vector.
    inline static TMatrix3<T> &Cast(T *u);
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th row of the matrix (i = 0, 1, 2).
    inline TVect3<T> &operator [] (int i);
    /// Access to the i-th row of the matrix (i = 0, 1, 2).
    inline const TVect3<T> &operator [] (int i) const;
    //@}

    /// @name Strict comparison.
    //@{
    /// Comparison of this matrix with given matrix (equality).
    inline bool operator == (const TMatrix3<T> &u) const;
    /// Comparison of this matrix with given matrix (inequality).
    inline bool operator != (const TMatrix3<T> &u) const;
    /// Comparison by lexicographical order.
    inline bool operator < (const TMatrix3<T> &u) const;
    /// Absolute value of maximal element (by absolute value) of the matrix.
    inline T MaxElement() const;
    /// Check that this matrix is identity matrix.
    inline bool IsIdentity() const;
    //@}

    /// @name Comparison with tolerance.
    //@{
    /// Comparison with given tolerance.
    inline static bool AboutZero(const TMatrix3<T> &u, const double tolerance);
    /// Comparison with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutZero(const TMatrix3<T> &u);
    /// Comparison of the matrices with given tolerance.
    inline static bool AboutEqual(const TMatrix3<T> &a, const TMatrix3<T> &b,
                                  const double tolerance);
    /// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TMatrix3<T> &a,
                                  const TMatrix3<T> &b);
    /// Comparison of the given matrix and identity matrix with given tolerance.
    inline static bool AboutIdentity(const TMatrix3<T> &a,
                                     const double tolerance);
    /// @brief Comparison of the given matrix and identity matrix with 
    /// predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutIdentity(const TMatrix3<T> &a);
    /// @brief Comparison of the this matrix and identity matrix with
    /// predefined tolerance Math<T>::TOLERANCE.
    inline bool AboutIdentity() const;
    //@}

    /// @name Arithmetic operators.
    //@{
    /// Addition of the matrices.
    inline TMatrix3<T> operator + (const TMatrix3<T> &v) const;
    /// Subtraction of the matrices.
    inline TMatrix3<T> operator - (const TMatrix3<T> &v) const;
    /// Multiplication of this matrix by given scalar.
    inline TMatrix3<T> operator * (const double d) const;
    /// Multiplication of this matrix by the given vector.
    inline TVect3<T> operator * (const TVect3<T> &u) const;
    /// Multiplication of this matrix by given matrix.
    TMatrix3<T> operator * (const TMatrix3<T> &v) const;
    /// Matrix by scalar division.
    inline TMatrix3<T> operator / (const double d) const;
    //@}

    /// @name Rotation points and vectors.
    //@{
    /// Rotation of the point by the rotation matrix.
    inline TPoint3<T> &Rotate(TPoint3<T> &p) const;
    /// Rotation of the vector by the rotation matrix.
    inline TVect3<T> &Rotate(TVect3<T> &u) const;
    /// Creation of the vector by the rotation of the given vector.
    inline TVect3<T> Rotated(const TVect3<T> &u) const;
    /// Scale the vector.
    inline TVect3<T> VectorScaled(const TVect3<T> &u) const;
    /// Creation of the point by the rotation of the given point.
    inline TPoint3<T> Rotated(const TPoint3<T> &p) const;
    /// Creation of the vector by the back rotation of the given vector.
    inline TVect3<T> BackRotated(const TVect3<T> &u) const;
    /// Creation of the point by the back rotation of the given point.
    inline TPoint3<T> BackRotated(const TPoint3<T> &p) const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given matrix to this matrix.
    inline TMatrix3<T> & operator = (const TMatrix3<T> &u);
    /// Addition of the given matrix to this matrix.
    inline TMatrix3<T> & operator += (const TMatrix3<T> &u);
    /// Subtraction of the given matrix from this matrix.
    inline TMatrix3<T> & operator -= (const TMatrix3<T> &u);
    /// Multiplication of this matrix by given matrix.
    inline TMatrix3<T> & operator *= (const TMatrix3<T> &u);
    /// Multiplication of this matrix by given scalar.
    inline TMatrix3<T> & operator *= (const double d);
    /// Matrix by scalar division.
    inline TMatrix3<T> & operator /= (const double d);
    //@}

    /// @name Scale methods.
    //@{
    /// Get scale of a matrix (lazy form, used in Scene transform classes).
    Vect3d GetScale() const;
    /// Set scale of a matrix (lazy form, used in Scene transform classes).
    void SetScale(const Vect3d &scale);
    //@}

    /// @name Other methods.
    //@{
    /// @brief Set of the j-th (j = 0, 1, 2) column of the matrix to the
    /// given 3D vector.
    inline void SetCol(int j, const TVect3<T> &u);
    /// Getting of the j-th (j = 0, 1, 2) column of this matrix.
    inline TVect3<T> GetCol(int j) const;
    /// Minor of this matrix corresponding to (i-th, j-th) element.
    double Minor(int i, int j) const;
    /// Determinant of this matrix.
    double Det() const;
    /// Get inversed matrix.
    TMatrix3<T> Inversed() const;
    /// Get transposed matrix.
    TMatrix3<T> Transposed() const;
    /// Transpose this matrix.
    inline void Transpose();
    //@}

    /// @name Combination of multiplication with transposition.
    //@{
    /// @brief Product of the matrices with preliminary transposition of the
    /// left multiplier.
    static TMatrix3<T> LeftTransposeMultiply(const TMatrix3<T> &u, const TMatrix3<T> &v);
    /// @brief Product of the matrices with preliminary transposition of the
    /// right multiplier.
    static TMatrix3<T> RightTransposeMultiply(const TMatrix3<T> &u, const TMatrix3<T> &v);
    //@}

    /// @name  Rotation and scale matrices.
    /// One should take into account that rotation matrices are created so that
    /// rotation of vector (point) corresponds to the following order of
    /// multiplication: Vector * RotationMatrix.
    //@{
    /// Set matrix to the identity matrix.
    inline void Identity();
    /// @brief Creation of the rotation matrix from this matrix by the
    /// normalization of the rows.
    Vect3d Normalize();
    /// Matrix of rotation around X axis at a given angle.
    TMatrix3<T> &RotationX(double angle);
    /// Matrix of rotation around Y axis at a given angle.
    TMatrix3<T> &RotationY(double angle);
    /// Matrix of rotation around Z axis at a given angle.
    TMatrix3<T> &RotationZ(double angle);
    /// Matrix of rotation to the given angles around X, Y, Z axes.
    TMatrix3<T> &RotationXYZ(const Vect3d &angles);
    /// @brief Represent rotation given by rotation matrix in the form of
    /// rotation angles around X, Y, Z axes.
    void RotationToXYZAngles(Vect3d &angles) const;

#if 0  // not used
    /// Matrix of rotation to the given angles around Z, Y, X axes.
    TMatrix3<T> &RotationZYX(const Vect3d &angles);
    /// @brief Represent rotation given by rotation matrix in the form of
    /// rotation angles around Z, Y, X axes.
    void RotationToZYXAngles(Vect3d &angles) const;
    /// @brief Represent rotation given by rotation matrix in the form of
    /// rotation angles and scale values for CS axes.
    void MatrixToZYXAnglesScale(Vect3d &angles, Vect3d &scale) const;
#endif

    /// Matrix of rotation around given axis at a given angle.
    TMatrix3<T> &RotationAxis(const TVect3<T> &u, double angle);
    /// Retrieve rotation axis and angle from this matrix of rotation.
    void RotAngAxis(Vect3d &axis, double &angle) const;
    /// Matrix of rotation to the given axes.
    TMatrix3<T> &GenRotation(const TVect3<T> &u, const TVect3<T> &v,
                             const TVect3<T> &w);
    /// Set matrix of rotation from given Euler angles.
    TMatrix3<T> &EulerRotation(double alpha, double beta, double gamma);
    /// Calculation of the Euler angles from rotation matrix.
    void RotationToEuler(double &alpha, double &beta, double &gamma) const;
    /// @brief Matrix of rotation with respect to given Euler angles with
    /// specific orientation of the object (Camera).
    TMatrix3<T> &EulerRotationForCamera(double alpha, double beta, double gamma);
    /// @brief Calculation of the Euler angles from rotation matrix of the
    /// object (Camera) with specific orientation.
    void RotationToEulerForCamera(double &alpha, double &beta, double &gamma) const;
    /// @brief Matrix of rotation with respect to given Euler angles with
    /// specific orientation of the object (light).
    TMatrix3<T> &EulerRotationForLight(double alpha, double beta, double gamma);
    /// @brief Calculation of the Euler angles from rotation matrix of the
    /// object (light) with specific orientation.
    void RotationToEulerForLight(double &alpha, double &beta, double &gamma) const;
    /// Calculate rotation matrix for camera using TBT twist angle.
    TMatrix3<T> &DirTwistRotation(const TVect3<T> &s_dir, const TVect3<T> &d_dir,
                                  const TVect3<T> &s_up, const TVect3<T> &d_up,
                                  const TVect3<T> &add_r_nm, const double &twist);
    void RotationToDirTwist(const TVect3<T> &s_dir, TVect3<T> &d_dir,  
                            const TVect3<T> &s_up, const TVect3<T> &d_up,
                            const TVect3<T> &add_r_nm, double &twist) const;
    /// Matrix of rotation between given vectors.
    TMatrix3<T> &Rotation(const TVect3<T> &source, const TVect3<T> &target);
    /// Matrix of rotation between given vectors.
    TMatrix3<T> &Rotation(const TVect3<T> &source, const TVect3<T> &target,
                          const TVect3<T> &def_rot_dir);
    /// Scaling matrix.
    inline TMatrix3<T> &Scale(const double x, const double y, const double z);
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 3x3 matrices.
    //@{
    /// Serialize the matrix using identity one as default.
    void Serialize(Serializer &inout, const char *tag);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Predefined matrices and vectors.
    //@{
    /// Predefined identity matrix.
    static const TMatrix3<T> &E();
    /// Predefined vector used by twist angle calculations for Lumicept.
    static const TVect3<T> DefCamTwistDir();
    //@}

  private:
    /// Matrix of rotation around given axis at a given angle.
    TMatrix3<T> &RotationAxis(const TVect3<T> &u,
                              double cos_ang, double sin_ang);

  public:
    /// @name Members: 3x3 matrix is implemented as the 3 rows of 3D vectors
    //@{
    /// First row of the matrix
    TVect3<T> r0,
    /// Second row of the matrix
              r1,
    /// Third row of the matrix
              r2;
    //@}
  };

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 3x3 matrix with elements of float type.
typedef TMatrix3<float>   Matrix3f;
/// Predefined instance of 3x3 matrix with elements of float type.
typedef TMatrix3<double>  Matrix3d;

// ----------------------------------------------------------------------------
//                     TMatrix3 SERVICE DEFINITIONS
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
///
template <class T>
TMatrix3<T>::TMatrix3()
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
TMatrix3<T>::TMatrix3(const double a)
  {
  r0[0] = r1[1] = r2[2] = (T)a;
  r0[1] = r0[2] = r1[0] = r1[2] = r2[0] = r2[1] = 0.0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
///
/// Diagonal elements of the matrix are initialized by the given values
/// according to its order. The rest elements of the matrix are initialized by 0.
///
/// @param a - Value for the (0,0) element of the matrix.
/// @param b - Value for the (1,1) element of the matrix.
/// @param c - Value for the (2,2) element of the matrix.
///
template <class T>
TMatrix3<T>::TMatrix3(const double a, const double b, const double c)
  {
  r0[0] = (T)a;
  r1[1] = (T)b;
  r2[2] = (T)c;
  r0[1] = r0[2] = r1[0] = r1[2] = r2[0] = r2[1] = 0.0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given 3D vectors.
///
/// Rows of the matrix are initialized by the given 3D vectors according
/// to its order cf. first row of the matrix is initialized by the first vector,
/// second row is initialized by the second vector and so on.
///
template <class T>
TMatrix3<T>::TMatrix3(const TVect3<T> &x, const TVect3<T> &y, const TVect3<T> &z)
  {
  r0 = x;
  r1 = y;
  r2 = z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the three elements as a constant vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D constant vector.
///
template <class T>
const TMatrix3<T> &TMatrix3<T>::Cast(const T *u)
  {
  return *(const TMatrix3<T> *)u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Treatment of the array of the three elements as a vector.
///
/// New vector is not created (data is not changed), but contents of the array
/// may be used as elements of the returned vector.
///
/// @return Reference to the 3D vector.
///
template <class T>
TMatrix3<T> &TMatrix3<T>::Cast(T *u)
  {
  return *(TMatrix3<T> *)u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param mat - Source 3x3 matrix.
///
template <class T>
TMatrix3<T>::TMatrix3(const TMatrix3<T> &mat)
  {
  r0 = mat.r0;
  r1 = mat.r1;
  r2 = mat.r2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2).
///
/// @note i must be in range 0..2. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2).
/// @return Reference to the 3D vector - i-th row of the matrix.
///
template <class T>
TVect3<T> &TMatrix3<T>::operator [] (int i)
  {
  Assert(i >= 0 && i < 3);;
  return (&r0)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2).
///
/// @note i must be in range 0..2. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2).
/// @return Constant reference to the 3D vector - i-th row of the matrix.
///
template <class T>
const TVect3<T> &TMatrix3<T>::operator [] (int i) const
  {
  Assert(i >= 0 && i < 3);;
  return (&r0)[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Set of the j-th (j = 0, 1, 2) column of the matrix to the given 3D vector.
///
/// Elements of the matrix in the j-th (j = 0, 1, 2) column are replaced by the
/// corresponding elements of the given 3D vector.
///
/// @note j must be 0, 1 or 2. Debug version asserts this.
/// @param j - Index of the column (0, 1 or 2)
/// @param u - Given 3D vector.
/// @return None (this matrix is updated)
///
template <class T>
void TMatrix3<T>::SetCol(int j, const TVect3<T> &u)
  {
  Assert(j >= 0 && j < 3);
  r0[j] = u[0];
  r1[j] = u[1];
  r2[j] = u[2];
  }

///////////////////////////////////////////////////////////////////////////////
/// Getting of the j-th (j = 0, 1, 2) column of this matrix.
///
/// Method creates a new 3D vector from j-th (j = 0, 1 or 2) column of this
/// matrix.
///
/// @note j must be 0, 1 or 2. Debug version asserts this.
/// @param j - Index of the column (0, 1 or 2).
/// @return New 3D vector (j-th column).
///
template <class T>
TVect3<T> TMatrix3<T>::GetCol(int j) const
  {
  Assert(j >= 0 && j < 3);
  TVect3<T> u;
  u[0] = r0[j];
  u[1] = r1[j];
  u[2] = r2[j];
  return u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (equality).
///
/// The two matrices are equal if the all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 3x3 matrix.
/// @return true - if this matrix is equal to given matrix, false - otherwise.
///
template <class T>
bool TMatrix3<T>::operator == (const TMatrix3<T> &u) const
  {
  return (r0 == u.r0) && (r1 == u.r1) && (r2 == u.r2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (inequality).
///
/// The two matrices are equal if the all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 3x3 matrix.
/// @return true - if this matrix is not equal to given matrix, false -
/// otherwise.
///
template <class T>
bool TMatrix3<T>::operator != (const TMatrix3<T> &u) const
  {
  return (r0 != u.r0) || (r1 != u.r1) || (r2 != u.r2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison by lexicographical order.
///
/// Comparison of two matrices. This method is different from other comparison
/// methods as it forms a lexicographical matrices order during sorting. It
/// means that it returns true immediately if first component of this matrix is
/// less than first component of the given matrix, or false if it is greater. If
/// first elements are equal, it compares second one and so on.
///
/// @param u - Given 3D matrix.
/// @return true - if this matrix less than given matrix in lexical sense,
/// false - otherwise.
template <class T>
bool TMatrix3<T>::operator < (const TMatrix3<T> &u) const
  {
  if (r0 < u.r0)
    return true;
  if (u.r0 < r0)
    return false;
  if (r1 < u.r1)
    return true;
  if (u.r1 < r1)
    return false;
  return r2 < u.r2;
  }

///////////////////////////////////////////////////////////////////////////////
/// Absolute value of maximal element (by absolute value) of the matrix.
///
/// @return Absolute value of maximal element (by absolute value) of the matrix.
template <class T>
T TMatrix3<T>::MaxElement() const
  {
  return Max3(r0.MaxElement(), r1.MaxElement(), r2.MaxElement());
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that this matrix is identity matrix.
///
/// @return true if this matrix is identity matrix (see method TMatrix3<T>::Identity()), 
///         false - otherwise.
template <class T>
bool TMatrix3<T>::IsIdentity() const
  {
  return (*this == E());
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with given tolerance.
///
/// @param u - Given matrix 3x3.
/// @param tolerance - Given tolerance
/// @return true if - all elements of the given matrix less or equal (by
/// absolute value) than value of given tolerance, false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutZero(const TMatrix3<T> &u, const double tolerance)
  {
  return (Math3<T>::AboutZero(u[0], tolerance) &&
          Math3<T>::AboutZero(u[1], tolerance) &&
          Math3<T>::AboutZero(u[2], tolerance));
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison with predefined tolerance Math<T>::TOLERANCE.
///
/// @note Value of Math<T>::TOLERANCE is dependent from type T.
/// @param u - Given matrix 3x3.
/// @return true if all elements of the given matrix less or equal (by absolute
/// value)  than predefined tolerance Math<T>::TOLERANCE.
///
template <class T>
bool TMatrix3<T>::AboutZero(const TMatrix3<T> &u)
  {
  return AboutZero(u, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with given tolerance.
///
/// Method calculates difference between given matrices (creates new temporary
/// 3x3 matrix) and compares the all elements of the created matrix with given
/// tolerance.
///
/// @param a - First matrix 3x3.
/// @param b - Second matrix 3x3.
/// @param tolerance - Given tolerance.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than given tolerance,
/// false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutEqual(const TMatrix3<T> &a, const TMatrix3<T> &b,
                              const double tolerance)
  {
  return AboutZero(a - b, tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
///
/// Method calculates difference between given matrices (creates new temporary
/// 3x3 matrix) and compares the all elements of the created matrix with
/// tolerance Math<T>::TOLERANCE.
///
/// @param a - First matrix 3x3.
/// @param b - Second matrix 3x3.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than tolerance
/// Math<T>::TOLERANCE, false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutEqual(const TMatrix3<T> &a,
                             const TMatrix3<T> &b)
  {
  return AboutZero(a - b, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the given matrix and identity matrix with given tolerance.
///
/// Method calculates difference between given matrix and identity matrix
/// (creates new temporary 3x3 matrix) and compares the all elements of the
/// created matrix with given tolerance.
///
/// @param a - Given matrix 3x3.
/// @param tolerance - Given tolerance
/// @return true - if all differences between elements of the given matrix and
/// corresponding elements of the identity matrix are less or equal (by absolute
/// value) than given tolerance, false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutIdentity(const TMatrix3<T> &a,
                                 const double tolerance)
  {
  return AboutZero(a - E(), tolerance);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of the given matrix and identity matrix with predefined tolerance
/// Math<T>::TOLERANCE.
///
/// Method calculates difference between given matrix and identity matrix
/// (creates new temporary 3x3 matrix) and compares the all elements of the
/// created matrix with tolerance Math<T>::TOLERANCE.
///
/// @param a - Given matrix 3x3.
/// @return true - if all differences between elements of the given matrix and
/// corresponding elements of the identity matrix are less or equal (by absolute
/// value) than tolerance Math<T>::TOLERANCE, false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutIdentity(const TMatrix3<T> &a)
  {
  return AboutIdentity(a, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix and identity matrix with predefined tolerance
/// Math<T>::TOLERANCE.
///
/// Method calculates difference between this matrix and identity matrix
/// (creates new temporary 3x3 matrix) and compares the all elements of the
/// created matrix with tolerance Math<T>::TOLERANCE.
///
/// @param a - Given matrix 3x3.
/// @return true - if all differences between elements of the given matrix and
/// corresponding elements of the identity matrix are less or equal (by absolute
/// value) than tolerance Math<T>::TOLERANCE, false - otherwise.
///
template <class T>
bool TMatrix3<T>::AboutIdentity() const
  {
  return AboutIdentity(*this, Math<T>::TOLERANCE);
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the matrices.
///
/// Method creates a new matrix 3x3. Elements of the new matrix are the sums
/// of the elements of this matrix with corresponding elements of the given
/// matrix.
///
/// @param v - Given matrix 3x3.
/// @return New matrix 3x3 (sum of the matrices).
///
template <class T>
TMatrix3<T> TMatrix3<T>::operator + (const TMatrix3<T> &v) const
  {
  TMatrix3<T> res;
  res.r0 = r0 + v.r0;
  res.r1 = r1 + v.r1;
  res.r2 = r2 + v.r2;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the matrices.
///
/// Method creates a new matrix 3x3. Elements of the new matrix are the
/// differences between elements of this matrix (minuend) and corresponding
/// elements of the given matrix (subtrahend).
///
/// @param v - Given matrix (subtrahend).
/// @return New matrix 3x3 (difference between two matrices).
///
template <class T>
TMatrix3<T> TMatrix3<T>::operator - (const TMatrix3<T> &v) const
  {
  TMatrix3<T> res;
  res.r0 = r0 - v.r0;
  res.r1 = r1 - v.r1;
  res.r2 = r2 - v.r2;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Method creates a new matrix 3x3. Elements of the new matrix are the products
/// of the elements of this matrix and given scalar.
///
/// @param d - Given scalar (multiplier).
/// @return New 3x3 matrix (product).
///
template <class T>
TMatrix3<T> TMatrix3<T>::operator * (const double d) const
  {
  TMatrix3<T> res;
  res.r0 = r0 * d;
  res.r1 = r1 * d;
  res.r2 = r2 * d;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by the given vector.
///
/// Method creates a new 3D vector. Elements of a new vector are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the rows of this matrix and given vector cf. matrix is multiplied by the
/// vector from the right. This operator is corresponding to the method
/// BackRotated().
///
/// @param u - Given vector (multiplier)
/// @return New 3D vector (product).
///
template <class T>
TVect3<T> TMatrix3<T>::operator * (const TVect3<T> &u) const
  {
  TVect3<T> res;
  res.x = (T)DotProd(r0, u);
  res.y = (T)DotProd(r1, u);
  res.z = (T)DotProd(r2, u);
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix by scalar division.
///
/// Method creates a new 3x3 matrix. Elements of the new matrix are the results
/// of the division of the elements of this matrix by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version asserts about
/// this.
/// @param d - Given scalar (divisor).
/// @return New matrix 3x3.
///
template <class T>
TMatrix3<T> TMatrix3<T>::operator / (const double d) const
  {
  TMatrix3<T> res;
  Assert(d != 0);
  res.r0 = r0 / d;
  res.r1 = r1 / d;
  res.r2 = r2 / d;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Rotation of the point by the rotation matrix.
///
/// Method modifies the given point. The elements of the given point are
/// replaced by the Dot products (see function
/// DotProd(const TVect3<T> &a, const TVect3<T> &b)) of the columns of
/// this matrix with vector of given point cf. this matrix is multiplied by the
/// vector of given point from the left. If this matrix is rotation matrix (cf.
/// this matrix may be produced by the product of the rotation matrices around
/// coordinate axes) then resulted point is rotated with respect to initial
/// given point.
///
/// @param p - Given 3D point.
/// @return  Reference to given point (rotated).
///
template <class T>
TPoint3<T> &TMatrix3<T>::Rotate(TPoint3<T> &p) const
  {
  p.Vector() = p.Vector() * (*this);
  return p;
  }

///////////////////////////////////////////////////////////////////////////////
/// Rotation of the vector by the rotation matrix.
///
/// Method modifies the given vector. The elements of the given vector are
/// replaced by the Dot products (see function
/// DotProd(const TVect3<T> &a, const TVect3<T> &b)) of the columns of
/// this matrix with given vector cf. this matrix is multiplied by the given
/// vector from the left. If this matrix is rotation matrix (cf. this matrix
/// may be produced by the product of the rotation matrices around coordinate
/// axes) then resulted given vector is rotated with respect to initial given
/// vector.
///
/// @param u - 3D vector
/// @return Reference to given vector (rotated).
///
template <class T>
TVect3<T> &TMatrix3<T>::Rotate(TVect3<T> &u) const
  {
  u = u * (*this);
  return u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the vector by the rotation of the given vector.
///
/// Method creates a new 3D vector. The elements of the new vector are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the columns of this matrix with
/// given vector cf. this matrix is multiplied by the given vector from the left.
/// If this matrix is rotation matrix (cf. this matrix may be produced by the
/// product of the rotation matrices around coordinate axes) then resulted
/// vector is rotated with respect to given vector.
///
/// @param u - Given 3D vector.
/// @return  New 3D vector (rotated with respect to given vector).
///
template <class T>
TVect3<T> TMatrix3<T>::Rotated(const TVect3<T> &u) const
  {
  return u * (*this);
  }

//////////////////////////////////////////////////////////////////////////////
/// Scale the vector.
///
/// Method creates a new 3D vector. New 3D vector is result of multiplication
/// of the scale part matrix.
///
/// @param u - Given 3D vector.
/// @return Scaled 3D vector.
///
template <class T>
TVect3<T> TMatrix3<T>::VectorScaled(const TVect3<T> &u) const
  {
  return TVect3<T>(u.x * r0.x, u.y * r1.y, u.z * r2.z);
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by the rotation of the given point.
///
/// Method creates a new 3D point. The elements of the new point are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the columns of this matrix with vector of given point
/// cf. this matrix is multiplied by the vector of given point from
/// the left. If this matrix is rotation matrix (cf. this matrix may be
/// produced by the product of the rotation matrices around coordinate axes) then
/// resulted point is rotated with respect to given point.
///
/// @param p - Given 3D point.
/// @return  New 3D point (rotated with respect to given point).
///
template <class T>
TPoint3<T> TMatrix3<T>::Rotated(const TPoint3<T> &p) const
  {
  return (p.Vector() * (*this)).Point();
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the vector by the back rotation of the given vector.
///
/// Method creates a new 3D vector. The elements of the new vector are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the rows of this matrix with  given vector cf. this matrix is multiplied
/// by the given vector from the right.
/// If this matrix is rotation matrix (cf. this matrix may be produced
/// by the product of the rotation matrices around coordinate axes) then
/// resulted vector is back rotated in with respect to given vector.
///
/// @param u - Given 3D vector.
/// @return  New 3D vector (back rotated with respect to given vector).
///
template <class T>
TVect3<T> TMatrix3<T>::BackRotated(const TVect3<T> &u) const
  {
  return *this * u;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the point by the back rotation of the given point.
///
/// Method creates the new 3D point. The elements of the new point are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the rows of this matrix with vector of given point cf. this matrix is
/// multiplied by the vector of given point from the right.
/// If this matrix is rotation matrix (cf. this matrix may be produced by the
/// product of the rotation matrices around coordinate axes) then resulted
/// point is back rotated in with respect to given point.
///
/// @param p - Given 3D point.
/// @return  New 3D point (back rotated with respect to given point).
///
template <class T>
TPoint3<T> TMatrix3<T>::BackRotated(const TPoint3<T> &p) const
  {
  return (*this * p.Vector()).Point();
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignment of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the elements of the given matrix.
///
/// @param u - Given matrix 3x3.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::operator = (const TMatrix3<T> &u)
  {
  r0 = u.r0;
  r1 = u.r1;
  r2 = u.r2;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Addition of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the sums of corresponding elements
/// of this matrix and given matrix.
///
/// @param u - Given matrix 3x3.
/// @return Reference to this matrix (sum of two matrices).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::operator += (const TMatrix3<T> &u)
  {
  r0 += u.r0;
  r1 += u.r1;
  r2 += u.r2;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given matrix from this matrix.
///
/// Elements of this matrix are replaced by the differences between
/// corresponding elements of this matrix and given matrix.
///
/// @param u - Given matrix (subtrahend).
/// @return Reference to this matrix (difference between two matrices).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::operator -= (const TMatrix3<T> &u)
  {
  r0 -= u.r0;
  r1 -= u.r1;
  r2 -= u.r2;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Elements of this matrix are replaced by the Dot products
/// (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the rows of this matrix and columns of the given matrix. Let i -
/// index of the row (i = 0, 1, 2), j - index of the column (j = 0, 1, 2) then
/// new element (i, j) of this matrix is Dot product of the i-th row of this
/// matrix and j-th column of the given matrix.
///
/// @param u - Given matrix 3x3 (multiplier).
/// @return Reference to this matrix (updated)
///
template <class T>
TMatrix3<T> &TMatrix3<T>::operator *= (const TMatrix3<T> &u)
  {
  *this = (*this) * u;
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
TMatrix3<T> &TMatrix3<T>::operator *= (const double d)
  {
  r0 *= d;
  r1 *= d;
  r2 *= d;
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
TMatrix3<T> &TMatrix3<T>::operator /= (const double d)
  {
  Assert(d != 0);
  r0 /= d;
  r1 /= d;
  r2 /= d;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Transpose this matrix.
///
/// Method modifies this matrix by the swapping between rows and columns cf.
/// rows of the modified matrix are the columns of the initial matrix and
/// vice versa.
///
/// @return None (this matrix is mofified).
///
template <class T>
void TMatrix3<T>::Transpose()
  {
  Swap(r0[1], r1[0]);
  Swap(r0[2], r2[0]);
  Swap(r1[2], r2[1]);
  }

///////////////////////////////////////////////////////////////////////////////
/// Set matrix to the identity matrix.
///
/// The diagonal elements of this matrix are set to 1.0. The rest elements
/// of this matrix are set to 0.
///
/// @return None (this matrix is affected only).
///
template <class T>
void TMatrix3<T>::Identity()
  {
  r0[0] = r1[1] = r2[2] = 1;
  r0[1] = r0[2] = r1[0] = r1[2] = r2[0] = r2[1] = 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Scaling matrix.
///
/// Scaling matrix is diagonal matrix. Diagonal elements of the matrix are the
/// scale factors. The rest elements of the matrix are zeros.
///
/// Multiplication of this matrix with some vector scales coordinates
/// of the vector.
///
/// @param x - Scale factor for X, (0, 0) element.
/// @param y - Scale factor for Y, (1, 1) element.
/// @param z - Scale factor for Z, (2, 2) element.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::Scale(const double x, const double y, const double z)
  {
  r0[0] = (T)x;
  r1[1] = (T)y;
  r2[2] = (T)z;
  r0[1] = r0[2] = r1[0] = r1[2] = r2[0] = r2[1] = 0;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of the given matrix by the given scalar.
///
/// Function creates a new matrix. Elements of the new matrix are the products
/// of the elements of the given matrix and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 3x3 matrix.
/// @return New 3x3 matrix.
///
template <class T>
TMatrix3<T> operator * (const double d, const TMatrix3<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// @brief Multiplication of the given matrix by the given vector.
///
/// Function creates a new 3D vector. Elements of a new vector are the Dot
/// products (see function DotProd()) of the columns of the given matrix and
/// given vector cf. matrix is multiplied by the vector from the left. This
/// operator is corresponding of the methods Rotated(), Rotation().
///
/// @param u - Given 3D vector (multiplier).
/// @param a - Given 3x3 matrix (multiplier).
/// @return New 3D vector (product).
///
template <class T>
inline TVect3<T> operator * (const TVect3<T> &u, const TMatrix3<T> &a)
  {
  TVect3<T> res;
  res.x = u.x * a.r0.x + u.y * a.r1.x + u.z * a.r2.x;
  res.y = u.x * a.r0.y + u.y * a.r1.y + u.z * a.r2.y;
  res.z = u.x * a.r0.z + u.y * a.r1.z + u.z * a.r2.z;
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with float elements.
///
/// @param v - 3x3 matrix with elements of the arbitrary type.
/// @return 3x3 matrix with float elements.
///
template <class T>
TMatrix3<float> ConvF(const TMatrix3<T> &v)
  {
  return TMatrix3<float>(ConvF(v[0]), ConvF(v[1]), ConvF(v[2]));
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with double elements.
///
/// @param v - 3x3 matrix with elements of the arbitrary type.
/// @return 3x3 matrix with double elements.
///
template <class T>
TMatrix3<double> ConvD(const TMatrix3<T> &v)
  {
  return TMatrix3<double>(ConvD(v[0]), ConvD(v[1]), ConvD(v[2]));
  }

INTEGRA_NAMESPACE_END

#endif // _KLBC_MATRIX3_HPP_

