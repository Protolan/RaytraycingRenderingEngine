/// @file
///
/// @brief Definition of template class for transormation matrices 4x3
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>

#ifndef _KLBC_MATRIX43_HPP_
#define _KLBC_MATRIX43_HPP_

#include "matrix3.hpp"

INTEGRA_NAMESPACE_START

// -----------------------------------------------------------------
//                Template class TMatrix43
// -----------------------------------------------------------------

/// Matrix 4x3 of the arbitrary type.
template <class T>
class TMatrix43
  {
  public:

    /// Transformation types.
    enum TransformationType
      {
      /// General transformation (rotation and translation are present).
      GENERAL_TRANSFORM,
      /// It is translation matrix, rotation part is identity matrix.
      TRANSLATION_TRANSFORM,
      /// It is identity matrix, transformation is absent.
      IDENTITY_TRANSFORM,
      /// It is a matrix without rotation.
      SCALE_TRANSLATION_TRANSFORM
      };

    /// @name  Constructors.
    //@{
    /// Default constructor, no initialization of elements is performed.
    inline TMatrix43();
    /// Constructor from the given value.
    inline explicit TMatrix43(const double val);
    /// Constructor from the given values.
    inline TMatrix43(const double a, const double b, const double c);
    /// Constructor from the given 3x3 matrix and 3D vector.
    inline TMatrix43(const TMatrix3<T> &rm, const TVect3<T> &tr);
    /// Constructor from 4 3D vectors, first 3 present rotation matrix, the last - translation.
    inline TMatrix43(const TVect3<T> &x_vect, const TVect3<T> &y_vect, const TVect3<T> &z_vect, const TVect3<T> &tr);
    //  Default copy constructor is used
    //@}

    /// @name Element access.
    //@{
    /// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
    inline TVect3<T> &operator [] (int i);
    /// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
    inline const TVect3<T> &operator [] (int i) const;
    /// Access to rotation part of this matrix.
    inline const TMatrix3<T> &Matrix3() const;
    /// Access to rotation part of this matrix.
    inline TMatrix3<T> &Matrix3();
    /// Replacement of the rotation part of this matrix by the given matrix.
    inline void SetMatrix3(const TMatrix3<T> &mat);
    /// Get the translation part of this matrix.
    inline const TVect3<T> &GetTrans() const;
    /// Set translation part of this matrix by the given vector.
    inline void SetTrans(const TVect3<T> &transl);
    /// Set translation part of this matrix by the given point.
    inline void SetTrans(const TPoint3<T> &transl);
    //@}

    /// @name Strict comparison.
    //@{
    /// Comparison of this matrix with given matrix (equality).
    inline bool operator == (const TMatrix43<T> &u) const;
    /// Comparison of this matrix with given matrix (inequality).
    inline bool operator != (const TMatrix43<T> &u) const;
    //@}

    /// @name Comparison with tolerance.
    //@{
    /// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
    inline static bool AboutEqual(const TMatrix43<T> &a, const TMatrix43<T> &b);
    /// @brief Comparison of the matrices with given tolerance
    inline static bool AboutEqual(const TMatrix43<T> &a, const TMatrix43<T> &b, const double tolerance);
    //@}

    /// @name Arithmetic operators.
    //@{
    /// Addition of the matrices.
    inline TMatrix43<T> operator + (const TMatrix43<T> &u) const;
    /// Subtraction of the matrices.
    inline TMatrix43<T> operator - (const TMatrix43<T> &u) const;
    /// Multiplication of this matrix by given scalar.
    inline TMatrix43<T> operator * (const double d) const;
    /// Composition of transformations.
    inline TMatrix43<T> operator * (const TMatrix43<T> &u) const;
    /// Matrix by scalar division.
    inline TMatrix43<T> operator / (const double d) const;
    //@}

    /// @name Transformation of points and vectors.
    //@{
    /// Transformation of the vector.
    inline void VectorTransform(TVect3<T> &u) const;
    /// Transformation of the vector.
    inline TVect3<T> VectorTransformed(const TVect3<T> &u) const;
    /// Scale the vector.
    inline TVect3<T> VectorScaled(const TVect3<T> &u) const;
    /// Create vector by back transformation of the given vector.
    inline TVect3<T> VectorBackRotTransformed(const TVect3<T> &u) const;
    /// Create point by back transformation of the given point.
    inline TPoint3<T> PointBackRotTransformed(const TPoint3<T> &p) const;
    /// Transformation of the point.
    inline void PointTransform(TPoint3<T> &p) const;
    /// Transformation of the point.
    inline TPoint3<T> PointTransformed(const TPoint3<T> &p) const;
    /// Scale and translation of the point.
    inline TPoint3<T> PointScaledAndTranslated(const TPoint3<T> &p) const;
    /// Translation of the point.
    inline TPoint3<T> PointTranslated(const TPoint3<T> &p) const;
    /// Transform bounding box.
    void BBoxTransform(BBox3<T> &bbox) const;
    //@}

    /// @name Identity matrix.
    //@{
    /// Set matrix to identity matrix.
    inline void Identity();
    /// Check that this matrix is identity matrix.
    inline bool IsIdentity() const;
    /// Check that this matrix is close to identity matrix.
    inline bool AboutIdentity(const double tolerance = Math<T>::TOLERANCE) const;
    /// Check that this transformation matrix is translation matrix only, without rotation.
    inline bool IsOnlyTranslation() const;
    /// Check that this transformation matrix is scale matrix only, possibly with translation.
    inline bool IsOnlyScaleAndTranslation() const;
    /// Get type of transformation of this matrix.
    inline TransformationType GetTransformationType() const;
    //@}

    /// @name Assignment operators.
    //@{
    /// Assignment of the given matrix to this matrix.
    inline TMatrix43<T> & operator = (const TMatrix43<T> &u);
    /// Addition of the given matrix to this matrix.
    inline TMatrix43<T> & operator += (const TMatrix43<T> &u);
    /// Subtraction of the given matrix from this matrix.
    inline TMatrix43<T> & operator -= (const TMatrix43<T> &u);
    /// Multiplication of this matrix by given scalar.
    inline TMatrix43<T> & operator *= (const double d);
    /// Composition of transformations.
    inline TMatrix43<T> & operator *= (const TMatrix43<T> &u);
    /// Matrix by scalar division.
    inline TMatrix43<T> & operator /= (const double d);
    //@}

    /// @name Inverse matrix.
    //@{
    /// Get inversed transformation.
    inline TMatrix43<T> Inversed() const;
    /// Inverse transformation for rotation matrix.
    inline void InverseRot();
    /// Return inversed transformation for rotation matrix.
    inline TMatrix43<T> InversedForRotMat() const;
    //@}

    /// @name  Rotation, translation, scale matrices.
    /// One should take into account that rotation matrices are created so that
    /// rotation of vector (point) corresponds to the following order of
    /// multiplication: Vector * RotationMatrix.
    //@{
    /// Matrix of rotation around X axis at a given angle.
    inline TMatrix43<T> &RotationX(double angle);
    /// Matrix of rotation around Y axis at a given angle.
    inline TMatrix43<T> &RotationY(double angle);
    /// Matrix of rotation around Z axis at a given angle.
    inline TMatrix43<T> &RotationZ(double angle);
    /// Matrix of rotation around given axis at a given angle.
    inline TMatrix43<T> &RotationAxis(const TVect3<T> &u, double angle);
    /// Matrix of rotation to given axes.
    inline TMatrix43<T> &GenRotation(const TVect3<T> &ux, const TVect3<T> &uy, const TVect3<T> &uz);
    /// Matrix of translation.
    inline TMatrix43<T> &Translation(const TVect3<T> &u);
    /// Scaling matrix.
    inline TMatrix43<T> &Scale(const double x, const double y, const double z);
    /// View matrix.
    TMatrix43<T> &ViewMatrix(const TVect3<T> &obs, const TVect3<T> &view, const TVect3<T> &up);
    //@}

    /// @name Serialization.
    /// Serialization allows to read / write, or load / save data in easy way.
    /// These methods provide reading and writing 4x3 matrices.
    //@{
    /// Serialize the matrix using identity one as default.
    void Serialize(Serializer &inout, const char *tag);
    /// Lower level serialization.
    void Value(Serializer &inout);
    //@}

    /// @name Predefined identity matrix.
    //@{
    /// Predefined identity matrix.
    static const TMatrix43<T> &E();
    //@}

    /// Convert given transformation matrix to separate elements of Euler Transform.
    void ConvertToEulerTransformValues(Vect3d &translation, 
      double &azimuth, double &tilt, double &rotation, Vect3d &scale) const;

  private:
    /// @name Members: matrix 4x3.
    //@{
    /// The first 3 rows (rotation) as matrix 3x3.
    TMatrix3<T> m;
    /// 4-th rows (translation) as 3D vector.
    TVect3<T> v;
    //@}
  };  // class TMatrix3<T

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 4x3 matrix with elements of float type.
typedef TMatrix43<float>  Matrix43f;
/// Predefined instance of 4x3 matrix with elements of float type.
typedef TMatrix43<double> Matrix43d;

//////////////////////////////////////////////////////////////////////////////
/// Default constructor, no initialization of elements is performed.
template <class T>
TMatrix43<T>::TMatrix43()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given value.
///
/// Diagonal elements of the rotation part of this matrix (first three
/// rows) are initialized by the same value @a val. The rest elements of the
/// matrix are initialized by the 0. Initialized matrix is scaling matrix.
/// @param val - Given value.
template <class T>
TMatrix43<T>::TMatrix43(const double val) 
  : 
  m(val), 
  v(0)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
///
/// Diagonal elements of the rotation part of this matrix (first three
/// rows) are initialized by the given values according to its order. The
/// rest elements of the matrix are initialized by 0. Initialized matrix
/// is scaling matrix with given scale factors for the coordinate axes.
///
/// @param a - Value for the (0, 0) element.
/// @param b - Value for the (1, 1) element.
/// @param c - Value for the (2, 2) element.
template <class T>
TMatrix43<T>::TMatrix43(const double a, const double b, const double c)
  : 
  m(a, b, c), 
  v(0)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given 3x3 matrix and 3D vector.
///
/// The rotation part of this matrix (first three rows) is initialized
/// by the given 3x3 matrix. The translation part of this matrix (fourth
/// row) is initialized by the given 3D vector.
///
/// @param[in] rm - Matrix 3x3 for the initialization of rotation part.
/// @param[in] tr - 3D Vector for initialization of translation part.
template <class T>
TMatrix43<T>::TMatrix43(const TMatrix3<T> &rm, const TVect3<T> &tr)
  : 
  m(rm), 
  v(tr)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from 4 3D vectors, first 3 present rotation matrix, the last - translation.
/// @param[in] x_vect - 3D Vector for initialization of rotation part.
/// @param[in] y_vect - 3D Vector for initialization of rotation part.
/// @param[in] z_vect - 3D Vector for initialization of rotation part.
/// @param[in] tr - 3D Vector for initialization of translation part.
template <class T>
TMatrix43<T>::TMatrix43(const TVect3<T> &x_vect, const TVect3<T> &y_vect, const TVect3<T> &z_vect, const TVect3<T> &tr)
  : 
  m(x_vect, y_vect, z_vect), 
  v(tr)
  {
  }
  
//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
///
/// @note i must be in range 0..3. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2, 3).
/// @return Reference to the 3D vector - i-th row of the matrix.
template <class T>
TVect3<T> &TMatrix43<T>::operator [] (int i)
  {
  Assert(i >= 0 && i < 4);;
  return (i < 3) ? m[i] : v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the i-th row of the matrix (i = 0, 1, 2, 3).
///
/// @note i must be in range 0..3. Debug version asserts it.
/// @note The result may be used as an l-value.
/// @param i - Index of the row (0, 1, 2, 3).
/// @return Constant reference to the 3D vector - i-th row of the matrix.
template <class T>
const TVect3<T> &TMatrix43<T>::operator [] (int i) const
  {
  Assert(i >= 0 && i < 4);;
  return (i < 3) ? m[i] : v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to rotation part of this matrix.
///
/// Rotation part of this matrix is 3x3 matrix - the first three rows
/// of this matrix.
/// @note The result may be used as an l-value.
///
/// @return Constant reference to matrix 3x3.
template <class T>
const TMatrix3<T> &TMatrix43<T>::Matrix3() const
  {
  return m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to rotation part of this matrix.
///
/// Rotation part of this matrix is 3x3 matrix - the first three rows
/// of this matrix.
/// @note The result may be used as an l-value.
///
/// @return Reference to matrix 3x3.
template <class T>
TMatrix3<T> &TMatrix43<T>::Matrix3()
  {
  return m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Replacement of the rotation part of this matrix by the given matrix.
///
/// Rotation part of this matrix (first three rows) is replaced by the
/// given matrix.
///
/// @param mat - 3x3 matrix.
/// @return None (rotation part of this matrix is modified).
template <class T>
void TMatrix43<T>::SetMatrix3(const TMatrix3<T> &mat)
  {
  m = mat;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the translation part of this matrix.
///
/// Translation part of this matrix is 3D vector - fourth row of this
/// matrix.
/// @note The result may be used as an l-value.
///
/// @return Constant reference to 3D vector.
template <class T>
const TVect3<T> &TMatrix43<T>::GetTrans() const
  {
  return v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set translation part of this matrix by the given vector.
///
/// Translation part of this matrix (fourth row) is replaced by the
/// given vector.
///
/// @param transl - Given 3D vector.
/// @return None (translation part of this matrix is modified).
template <class T>
void TMatrix43<T>::SetTrans(const TVect3<T> &transl)
  {
  v = transl;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set translation part of this matrix by given point.
///
/// Translation part of this matrix (fourth row) is replaced by the
/// given vector.
///
/// @param transl - Given 3D vector.
/// @return None (translation part of this matrix is modified).
template <class T>
void TMatrix43<T>::SetTrans(const TPoint3<T> &transl)
  {
  v = transl.Vector();
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (equality).
///
/// The two matrices are equal if all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 4x3 matrix.
/// @return true - if this matrix is equal to given matrix, false -
/// otherwise.
template <class T>
bool TMatrix43<T>::operator == (const TMatrix43<T> &u) const
  {
  return (m == u.m) && (v == u.v);
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of this matrix with given matrix (inequality).
///
/// The two matrices are equal if all elements of the one matrix are equal
/// (strictly) to the corresponding elements of the other matrix, otherwise
/// matrices are not equal.
///
/// @param u - Given 4x3 matrix.
/// @return true - if this matrix is not equal to given matrix, false -
/// otherwise.
template <class T>
bool TMatrix43<T>::operator != (const TMatrix43<T> &u) const
  {
  return (m != u.m) || (v != u.v);
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with predefined tolerance Math<T>::TOLERANCE.
///
/// Method compares rotation parts (first three rows) of the given matrices
/// (see method TMatrix3<T>::AboutEqual()) and separately translation parts
/// (fourth rows) of the matrices see method Math3<T>::AboutEqual()).
///
/// @param a - First matrix 4x3.
/// @param b - Second matrix 4x3.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than tolerance
/// Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool TMatrix43<T>::AboutEqual(const TMatrix43<T> &a, const TMatrix43<T> &b)
  { return (TMatrix3<T>::AboutEqual(a.Matrix3(), b.Matrix3()) &&
           Math3<T>::AboutEqual(a.GetTrans(), b.GetTrans()));
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of the matrices with given tolerance.
///
/// Method compares rotation parts (first three rows) of the given matrices
/// (see method TMatrix3<T>::AboutEqual()) and separately translation parts
/// (fourth rows) of the matrices see method Math3<T>::AboutEqual()).
///
/// @param a - First matrix 4x3.
/// @param b - Second matrix 4x3.
/// @return true - if all differences between corresponding elements of the
/// given matrices are less or equal (by absolute value) than tolerance
/// Math<T>::TOLERANCE, false - otherwise.
template <class T>
bool TMatrix43<T>::AboutEqual(const TMatrix43<T> &a, const TMatrix43<T> &b,
                              const double tolerance)
  { return (TMatrix3<T>::AboutEqual(a.Matrix3(), b.Matrix3(), tolerance) &&
           Math3<T>::AboutEqual(a.GetTrans(), b.GetTrans(), tolerance));
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of the matrices.
///
/// Method creates a new matrix 4x3. Elements of the new matrix are the sums
/// of the elements of this matrix with corresponding elements of the given
/// matrix.
///
/// @param u - Given matrix 4x3.
/// @return New matrix 4x3 (sum of the matrices).
template <class T>
TMatrix43<T> TMatrix43<T>::operator + (const TMatrix43<T> &u) const
  {
  TMatrix43<T> res;
  res.m = m + u.m;
  res.v = v + u.v;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction of the matrices.
///
/// Method creates a new matrix 4x3. Elements of the new matrix are the
/// differences between elements of this matrix (minuend) and corresponding
/// elements of the given matrix (subtrahend).
///
/// @param u - Given 4x3 matrix (subtrahend).
/// @return New matrix 4x3 (difference between two matrices).
template <class T>
TMatrix43<T> TMatrix43<T>::operator - (const TMatrix43<T> &u) const
  {
  TMatrix43<T> res;
  res.m = m - u.m;
  res.v = v - u.v;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given scalar.
///
/// Method creates a new matrix 4x3. Elements of the new matrix are the products
/// of the elements of this matrix and given scalar.
///
/// @param d - Given scalar (multiplier).
/// @return New 4x3 matrix (product).
template <class T>
TMatrix43<T> TMatrix43<T>::operator * (const double d) const
  {
  TMatrix43<T> res;
  res.m = m * d;
  res.v = v * d;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transformation of the vector.
///
/// Method modifies a given 3D vector by the following transformations:
/// - Given vector is rotated by the rotation part (first three rows) of
/// this matrix (multiplication of the 3x3 matrix by the vector of vector from
/// the left).
///
/// @param u - Given 3D vector.
/// @return None (only given vector is affected).
template <class T>
void TMatrix43<T>::VectorTransform(TVect3<T> &u) const
  {
  u = u * m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transformation of the vector.
///
/// Method creates a new 3D vector. New 3D vector is result of multiplication
/// of the rotation part (3x3 matrix - first three rows) of this matrix by the
/// given vector from the left. If rotation part of this matrix is rotation
/// matrix (it is wrong in common case) then resulted vector is rotated with
/// respect to given vector.
///
/// @param u - Given 3D vector.
/// @return Transformed 3D vector.
template <class T>
TVect3<T> TMatrix43<T>::VectorTransformed(const TVect3<T> &u) const
  {
  return u * m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Scale the vector.
///
/// Method creates a new 3D vector. New 3D vector is result of multiplication
/// of the scale part (3x3 matrix.
///
/// @param u - Given 3D vector.
/// @return Scaled 3D vector.
template <class T>
TVect3<T> TMatrix43<T>::VectorScaled(const TVect3<T> &u) const
  {
  return m.VectorScaled(u);
  }

//////////////////////////////////////////////////////////////////////////////
/// Create vector by back transformation of the given vector.
///
/// Method creates a new 3D vector. New 3D vector is result of multiplication
/// of the rotation part (3x3 matrix - first three rows) of this matrix by the
/// given vector from the right. If rotation part of this matrix is rotation
/// matrix (it is wrong in common case) then resulted vector is back rotated
/// with respect to given vector (see method VectorTransformed()).
///
/// @attention This method can be applied only for matrices
/// known to be rotation matrices (without scale). For efficiency reason,
/// method does not perform matrix inverse. It multiplies vector by matrix
/// in reversed order - such operation is valid only for rotation matrices.
///
/// @param u - Given 3D vector.
/// @return Back transformed 3D vector.
template <class T>
TVect3<T> TMatrix43<T>::VectorBackRotTransformed(const TVect3<T> &u) const
  {
  return m * u;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create point by back transformation of the given point.
///
/// Method creates a new 3D point from given point by the following
/// transformations:
/// - Given point is shifted by the translation part (fourth row) of this matrix
/// in the back direction (subtraction of the 3D vector from given point).
/// - Resulted point is back rotated by the rotation part (first three rows) of
/// this matrix (multiplication of the 3x3 matrix by the vector of point from
/// the right, see method TMatrix3<T>::BackRotated()).
///
/// @attention This method can be applied only for matrices
/// known to be rotation matrices (without scale). For efficiency reason,
/// method does not perform matrix inverse. It multiplies point by matrix
/// in reversed order - such operation is valid only for rotation matrices.
/// @param p - Given 3D point.
/// @return Back transformed 3D point.
template <class T>
TPoint3<T> TMatrix43<T>::PointBackRotTransformed(const TPoint3<T> &p) const
  {
  return (m.BackRotated(p - v));
  }

//////////////////////////////////////////////////////////////////////////////
/// Transformation of the point.
///
/// Method modifies a given 3D point by the following transformations:
/// - Given point is rotated by the rotation part (first three rows) of
/// this matrix (multiplication of the 3x3 matrix by the vector of point from
/// the left).
/// - Resulted point is shifted by the translation part (fourth row) of this
/// matrix (addition of the 3D vector to given point).
///
/// @param p - Given 3D point.
/// @return None (only given point is affected).
template <class T>
void TMatrix43<T>::PointTransform(TPoint3<T> &p) const
  {
  p.Vector() = p.Vector() * m + v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transformation of the point.
///
/// Method creates a new 3D point from given point by the following
/// transformations:
/// - Given point is rotated by the rotation part (first three rows) of
/// this matrix (multiplication of the 3x3 matrix by the vector of point from
/// the left).
/// - Resulted point is shifted by the translation part (fourth row) of this
/// matrix (addition of the 3D vector to given point).
///
/// @param p - Given 3D point.
/// @return New 3D point (transformed with respect to given).
template <class T>
TPoint3<T> TMatrix43<T>::PointTransformed(const TPoint3<T> &p) const
  {
  return  (p.Vector() * m + v).Point();
  }

//////////////////////////////////////////////////////////////////////////////
/// Scale and translation of the point.
///
/// It is supposed that matrix contain the scale and translation
/// members only (see IsOnlyScaleAndTranslation() method)
/// Method creates a new 3D point from given point by the following
/// transformations:
/// - Given point is scaled by the using the scale part of
/// this matrix .
/// - Resulted point is shifted by the translation part (fourth row) of this
/// matrix (addition of the 3D vector to given point).
///
/// @param p - Given 3D point.
/// @return New 3D point (transformed with respect to given).
template <class T>
TPoint3<T> TMatrix43<T>::PointScaledAndTranslated(const TPoint3<T> &p) const
  {
  return TPoint3<T>(p.x * m[0][0] + v.x, p.y * m[1][1] + v.y,
                    p.z * m[2][2] + v.z);
  }



//////////////////////////////////////////////////////////////////////////////
/// Translation of the point.
///
/// Method creates a new 3D point by the move of the given point by the
/// translation part (fourth row) of this matrix.  Moved point is sum of
/// the given 3D point and 3D vector of translation part of this matrix.
///
/// @param p - Given 3D point.
/// @return New 3D point (translated with respect to given).
template <class T>
TPoint3<T> TMatrix43<T>::PointTranslated(const TPoint3<T> &p) const
  {
  return  p + v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Composition of transformations.
///
/// Method creates a new 4x3 matrix by the following way:
/// - Rotation part (first three rows) of the new matrix is product of the
/// rotation part of this matrix (first multiplier) and rotation part of the
/// given matrix (second multiplier).
/// - Translation part (fourth row) of the new matrix is sum of the two
/// components:
///   - translation part of this matrix (3D vector) rotated by the given matrix
///     (vector multiplies matrix from the left);
///   - translation part of given matrix.
///
/// Transformation of the point by the resulted matrix is equal to the
/// combination of the two transformations:
/// - this matrix transforms the given point;
/// - given matrix transforms the resulted (in previous step) point.
///
/// @param u - Given 4x3 matrix.
/// @return New 4x3 matrix (combination of transformations).
template <class T>
TMatrix43<T> TMatrix43<T>::operator * (const TMatrix43<T> &u) const
  {
  TMatrix43<T> res;
  // product of rotation parts
  res.m = m * u.m;

  // product of translation part of this matrix and rotation part of u
  // addition of translation part of u matrix to
  // translation part of res matrix
  res.v = v * u.m + u.v;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix by scalar division.
///
/// Method creates a new 4x3 matrix. Elements of the new matrix are the results
/// of the division of the elements of this matrix by the given scalar.
///
/// @note Given scalar may not be equal to zero. Debug version asserts about
/// this.
/// @param d - Given scalar (divisor).
/// @return New matrix 4x3.
template <class T>
TMatrix43<T> TMatrix43<T>::operator / (const double d) const
  {
  TMatrix43<T> res;
  Assert(d != 0);
  res.m = m / d;
  res.v = v / d;
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set matrix to identity matrix.
///
/// Method sets rotation part (first three rows) of this this matrix to
/// identity matrix (see method TMatrix3<T>::Identity()) and translation part
/// (fourth row) to zero vector.
///
/// @return None (this matrix is affected only).
template <class T>
void TMatrix43<T>::Identity()
  {
  m.Identity();
  v = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that this matrix is identity matrix.
///
/// @return true if rotation part of this matrix (first three rows) is identity
/// matrix (see method TMatrix3<T>::Identity()) @b and translation part (fourth
/// row) is zero vector, false - otherwise.
template <class T>
bool TMatrix43<T>::IsIdentity() const
  {
  return (m == TMatrix3<T>::E()) && (v == TVect3<T>(0));
  }

#if !defined(_WIN32) || (_MSC_VER >= 1400) || defined(DOXYGEN)
//////////////////////////////////////////////////////////////////////////////
/// Check that this matrix is close to identity matrix.
///
/// @return true if rotation part of this matrix is about identity
/// matrix (see method TMatrix3<T>::AboutIdentity()) @b and translation part (fourth
/// row) is zero vector, false - otherwise.
template <class T>
bool TMatrix43<T>::AboutIdentity(const double tolerance) const
  {
  return TMatrix3<T>::AboutIdentity(m, tolerance) && v == TVect3<T>(0); 
  }
#endif

//////////////////////////////////////////////////////////////////////////////
/// Check that this transformation matrix is translation matrix only, without
/// rotation.
///
/// @return true - if rotation part (first three rows) of this matrix is
/// identity matrix (see method TMatrix3<T>::Identity()), false - otherwise.
template <class T>
bool TMatrix43<T>::IsOnlyTranslation() const
  {
  return (m == TMatrix3<T>::E());
  }

/// Check that this transformation matrix is scale matrix only,
/// possibly with translation.
template <class T>
bool TMatrix43<T>::IsOnlyScaleAndTranslation() const
  {
  return (m[0][1] == 0 && m[0][2] == 0 &&
          m[1][0] == 0 && m[1][2] == 0 &&
          m[2][0] == 0 && m[2][1] == 0);
  }


//////////////////////////////////////////////////////////////////////////////
/// Type of transformation of this matrix.
///
/// @return Type of the transformation.
template <class T> typename TMatrix43<T>::TransformationType
TMatrix43<T>::GetTransformationType() const
  {
  return IsIdentity() ? IDENTITY_TRANSFORM :
         IsOnlyTranslation() ? TRANSLATION_TRANSFORM : 
         IsOnlyScaleAndTranslation() ? SCALE_TRANSLATION_TRANSFORM : 
         GENERAL_TRANSFORM;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the elements of the given matrix.
///
/// @param u - Given matrix 4x3.
/// @return Reference to this matrix (updated).
template <class T>
TMatrix43<T> &TMatrix43<T>::operator = (const TMatrix43<T> &u)
  {
  m = u.m;
  v = u.v;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of the given matrix to this matrix.
///
/// Elements of this matrix are replaced by the sums of corresponding elements
/// of this matrix and given matrix.
///
/// @param u - Given matrix 4x3.
/// @return Reference to this matrix (sum of two matrices).
template <class T>
TMatrix43<T> & TMatrix43<T>::operator += (const TMatrix43<T> &u)
  {
  m += u.m;
  v += u.v;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Subtraction of the given matrix from this matrix.
///
/// Elements of this matrix are replaced by the differences between
/// corresponding elements of this matrix and given matrix.
///
/// @param u - Given matrix 4x3 (subtrahend).
/// @return Reference to this matrix (difference between two matrices).
template <class T>
TMatrix43<T> & TMatrix43<T>::operator -= (const TMatrix43<T> &u)
  {
  m -= u.m;
  v -= u.v;
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
template <class T>
TMatrix43<T> & TMatrix43<T>::operator *= (const double d)
  {
  m *= d;
  v *= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Composition of transformations.
///
/// Method modifies this matrix by the following way:
/// - Rotation part (first three rows) of the modified matrix is products of
/// the rotation part of this matrix (first multiplier) and rotation part of
/// the given matrix (second multiplier).
/// - Translation part (fourth row) of the modified matrix is sum of the two
/// components:
///   - translation part of this matrix (3D vector) rotated by the given matrix
///     (vector multiplies matrix from the left);
///   - translation part of given matrix.
///
/// Transformation of the point by the resulted matrix is equal to the
/// combination of the two transformations:
/// - this matrix (before modification) transforms the given point;
/// - given matrix transforms the resulted (in previous step) point.
///
/// @param u - Given 4x3 matrix.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> & TMatrix43<T>::operator *= (const TMatrix43<T> &u)
  {
  v = v * u.m + u.v;
  m *= u.m;
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
template <class T>
TMatrix43<T> & TMatrix43<T>::operator /= (const double d)
  {
  Assert(d != 0);
  m /= d;
  v /= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get inversed transformation.
///
/// Method creates a new 4x3 matrix. The rotation part (first three rows) of
/// the new matrix is inverse with respect to rotation part of this
/// matrix (see method TMatrix3<T>::Inversed()).
/// Translation part of the new matrix (3D vector of fourth row) is created by
/// the following way:
/// - rotation part of the new matrix rotates the translation part (3D vector)
/// of this matrix (vector multiplies matrix from the left);
/// - resulted vector is converted to opposite by the negation of the all vector
/// elements.
///
/// Resulted point after two transformations (see methods PointTransform()
/// or PointTransformed()) of this matrix and inverse matrix (one by one) is
/// equal to initial point.
///
/// @note Determinant of the rotation part  of this matrix must not be equal
/// to zero. Debug version asserts about this.
///
/// @return New transformation matrix 4x3.
template <class T>
TMatrix43<T> TMatrix43<T>::Inversed() const
  {
  TMatrix43<T> res;
  res.m = this->m.Inversed();
  res.v = -(this->v * res.m);
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Inverse transformation for rotation matrix.
///
/// This method is analog of the method Inversed() but unlike of mentioned
/// method the new transformation matrix is not created - this matrix is
/// modified. Moreover it is assumed that rotation part (first three rows) of
/// this matrix is rotation matrix. Because for efficiency reason, method does
/// not perform matrix inverse, matrix transposition is used instead. This
/// approach is valid for the rotation matrix only and wrong in common case.
///
/// @return None (only this matrix is affected).
template <class T>
void TMatrix43<T>::InverseRot()
  {
  m.Transpose();
  v = -(v * m);
  }

//////////////////////////////////////////////////////////////////////////////
/// Return inversed transformation for rotation matrix.
///
/// It is assumed that rotation part of
/// this matrix is rotation matrix. Because for efficiency reason, method does
/// not perform matrix inverse, matrix transposition is used instead. This
/// approach is valid for the rotation matrix only and wrong in common case.
///
/// @return Inversed transformation.
template <class T>
TMatrix43<T> TMatrix43<T>::InversedForRotMat() const
  {
  TMatrix43<T> res;
  res.m = m.Transposed();
  res.v = -(v * res.m);

  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around X axis at a given angle.
///
/// Method modifies rotation part (first three rows) of this matrix to matrix
/// of rotation (see method TMatrix3<T>::RotationX()) and translation part
/// (fourth row) to zero vector. Result of multiplication of this matrix with
/// the some vector <b> from the left </b> is vector which is rotated with
/// respect to initial vector around X axis at given angle counterclockwise
/// (view direction along -X axis).
///
/// @image html rotate_x.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::RotationX(double angle)
  {
  m.RotationX(angle);  // rotation part
  v = 0;               // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around Y axis at a given angle.
///
/// Method modifies rotation part (first three rows) of this matrix to matrix
/// of rotation (see method TMatrix3<T>::RotationY()) and translation part
/// (fourth row) to zero vector.
/// Result of multiplication of this matrix with the some vector <b> from the
/// left </b> is vector which is rotated with respect to initial vector around
/// Y axis at given angle counterclockwise (view direction along -Y axis).
///
/// @image html rotate_y.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::RotationY(double angle)
  {
  m.RotationY(angle);  // rotation part
  v = 0;               // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around Z axis at a given angle.
///
/// Method modifies rotation part (first three rows) of this matrix to matrix
/// of rotation (see method TMatrix3<T>::RotationZ()) and translation part
/// (fourth row) to zero vector.
/// Result of multiplication of this matrix with the some vector <b> from the
/// left </b> is vector which is rotated with respect to initial vector around
/// Z axis at given angle counterclockwise (view direction along -Z axis).
///
/// @image html rotate_z.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::RotationZ(double angle)
  {
  m.RotationZ(angle);  // rotation part
  v = 0;               // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around given axis at a given angle.
///
/// Method modifies the rotation part (first three rows) of this matrix to
/// matrix of rotation (see method TMatrix3<T>::RotationAxis()) and translation
/// part (fourth row) to zero vector. Result of multiplication of this matrix
/// with the some vector <b> from the left </b> is vector which is rotated with
/// respect to initial vector around given axis (rotation axis is defined by
/// the given vector) at given angle counterclockwise (view direction is
/// opposite to given axis).
///
/// @note Length of axis (3D vector) may not be less than tolerance
/// Math<T>::TOLERANCE. Debug version asserts about this.
///
/// @param u - Given 3D vector (rotation axis).
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::RotationAxis(const TVect3<T> &u, double angle)
  {
  m.RotationAxis(u, angle);  // rotation part
  v = 0;                     // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation to given axes.
///
/// Method modifies rotation part (first tree rows) of this matrix to rotation
/// matrix (see method TMatrix3<T>::GenRotation()) and translation part (fourth
/// row) of this matrix to zero vector. Resulted matrix rotates X axis to first
/// given vector, Y axis to second given vector, Z axis to third given vector.
/// Given vectors define first three rows of the modified matrix.
///
/// @note Given vectors should be orthogonal unit vectors.
///
/// @param ux - First given vector - new X axis.
/// @param uy - Second given vector - new Y axis.
/// @param uz - Third given vector - new Z axis.
///
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::GenRotation(
    const TVect3<T> &ux, const TVect3<T> &uy, const TVect3<T> &uz)
  {
  m.GenRotation(ux, uy, uz);  // rotation part
  v = 0;                      // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Matrix of translation.
///
/// Method modifies rotation part of this matrix (first three rows) to
/// identity matrix and replaces translation part (fourth row) by the given
/// vector.
///
/// @param u - Given 3D vector for the translation part of this matrix.
/// @return Reference to this matrix (modified)
template <class T>
TMatrix43<T> &TMatrix43<T>::Translation(const TVect3<T> &u)
  {
  m.Identity();
  v = u;  // translation part
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Scaling matrix.
///
/// Method modifies rotation part of this matrix (first three rows) to diagonal
/// matrix with diagonal elements defined by the parameters of the method and
/// modifies translation part (fourth row) to zero vector.
///
/// Multiplication of this matrix with some vector scales coordinates
/// of the vector.
///
/// @param x - Scale factor for X, (0, 0) element.
/// @param y - Scale factor for Y, (1, 1) element.
/// @param z - Scale factor for Z, (2, 2) element.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::Scale(const double x, const double y, const double z)
  {
  m.Scale(x, y, z);  // rotation part
  v = 0;             // translation part
  return *this;
  }

// ---------------------------------------------------------------------------
//                    TMatrix43 SERVICE DEFINITIONS
// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of the given matrix by the given scalar.
///
/// Function creates a new matrix. Elements of the new matrix are the products
/// of the elements of the given matrix and given scalar.
///
/// @param d - Given scalar.
/// @param u - Given 4x3 matrix.
/// @return New 4x3 matrix.
template <class T>
inline TMatrix43<T> operator * (const double d, const TMatrix43<T> &u)
  {
  return u * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Transformation of the point.
///
/// Operator creates a new 3D point from given point by the following
/// transformations of the given matrix:
/// - Given point is rotated by the rotation part (first three rows) of the
/// given matrix: multiplication of the 3x3 matrix by the vector of point from
/// the left.
/// - Resulted point is shifted by the translation part (fourth row) of the
/// given matrix: addition of the 3D vector to given point.
///
/// This operator is analog of the methods PointTransformed(), PointTransform().
///
/// @param u - Given 3D point.
/// @param a - Given 4x3 matrix
/// @return New 3D point (transformed with respect to given).
template <class T>
inline TPoint3<T> operator * (const TPoint3<T> &u, const TMatrix43<T> &a)
  {
  return (u.Vector() * a.Matrix3() + a.GetTrans()).Point();
  }

///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with float elements.
///
/// @param v - 4x3 matrix with elements of the arbitrary type.
/// @return 4x3 matrix with float elements.
template <class T>
inline TMatrix43<float> ConvF(const TMatrix43<T> &v)
  {
  TMatrix43<float> res;

  // converts the rows as TVect3
  res[0] = ConvF(v[0]);
  res[1] = ConvF(v[1]);
  res[2] = ConvF(v[2]);
  res[3] = ConvF(v[3]);
  return res;
  }


///////////////////////////////////////////////////////////////////////////////
/// Conversion of the matrix with elements of the arbitrary type to the matrix
/// with double elements.
///
/// @param v - 4x3 matrix with elements of the arbitrary type.
/// @return 4x3 matrix with double elements.
template <class T>
inline TMatrix43<double> ConvD(const TMatrix43<T> &v)
  {
  TMatrix43<double> res;

  // converts the rows as TVect3
  res[0] = ConvD(v[0]);
  res[1] = ConvD(v[1]);
  res[2] = ConvD(v[2]);
  res[3] = ConvD(v[3]);
  return res;
  }

INTEGRA_NAMESPACE_END
#endif // _KLBC_MATRIX43_HPP_
