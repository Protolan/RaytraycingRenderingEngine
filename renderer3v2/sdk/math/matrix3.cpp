/// @internal
/// @file
///
/// @brief Methods of class for matrices 3x3 and instances for double
///        and float types
///
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///

#include <integra.hpp>

#include <base/serializer.hpp>

#include "matrix3.hpp"

INTEGRA_NAMESPACE_START

/**
@class TMatrix3 math/matrix3.hpp

Matrix 3x3 of the arbitrary type.
This matrix rotates a vector Row by multiplication from right side:
  return u * (*this);


Matrix3f and Matrix3d are predefined instances of the template
class for matrices with float and double elements, respectively.

The user may create own instances of the template class if needed.
Type T must support assignment and usual arithmetic and comparison
operators (including comparison with zero).

Conceptually 3x3 matrix consists of three vectors: r0, r1, r2. First vector is
first row of the matrix, second vector is second row of the matrix and so on.
Underlying implementation of 3x3 matrix is

@code
  public:
    TVect3<T> r0, r1, r2;
@endcode

User has possibility of direct access to the rows of the matrix.

This class provides sufficient number of methods which use 3x3 matrix as
rotation or scaling matrix. The part of these methods creates various rotation
matrices (RotationAxis(), EulerRotation(), etc.) the other part transforms
vectors and points by these matrices (Rotate(), BackRotated(), etc.).

@param T - template type of the matrix elements,
*/


// -----------------------------------------------------------------------
//             Methods of definition TMatrix3 class
// -----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Method creates a new 3x3 matrix. Elements of the new matrix are the
/// Dot products (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// of the rows of this matrix and columns of
/// the given matrix. Let i - index of the row (i = 0, 1, 2), j - index of the
/// column (j = 0, 1, 2) then element (i, j) of the new matrix is
/// Dot product of the i-th row of this matrix and j-th
/// column of the given  matrix.
///
/// @param v - Given matrix 3x3 (multiplier).
/// @return New 3x3 matrix.
///
template <class T>
TMatrix3<T> TMatrix3<T>::operator * (const TMatrix3<T> &v) const
  {
  TMatrix3<T> res;

  res.r0[0] = r0.x * v.r0.x + r0.y * v.r1.x + r0.z * v.r2.x;
  res.r0[1] = r0.x * v.r0.y + r0.y * v.r1.y + r0.z * v.r2.y;
  res.r0[2] = r0.x * v.r0.z + r0.y * v.r1.z + r0.z * v.r2.z;

  res.r1[0] = r1.x * v.r0.x + r1.y * v.r1.x + r1.z * v.r2.x;
  res.r1[1] = r1.x * v.r0.y + r1.y * v.r1.y + r1.z * v.r2.y;
  res.r1[2] = r1.x * v.r0.z + r1.y * v.r1.z + r1.z * v.r2.z;

  res.r2[0] = r2.x * v.r0.x + r2.y * v.r1.x + r2.z * v.r2.x;
  res.r2[1] = r2.x * v.r0.y + r2.y * v.r1.y + r2.z * v.r2.y;
  res.r2[2] = r2.x * v.r0.z + r2.y * v.r1.z + r2.z * v.r2.z;

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Minor of this matrix corresponding to (i-th, j-th) element.
///
/// It is determinant of the matrix 2x2. This 2x2 matrix is formed from this
/// matrix by the exclusion of the i-th row and j-th column.
///
/// @note i and j must be in range 0..2. Debug version asserts this.
/// @param i - index of the row.
/// @param j - index of the column.
/// @return Value of the minor.
///
template <class T>
double TMatrix3<T>::Minor(int i, int j) const
  {
  Assert(i >= 0 && i < 3 && j >= 0 && j < 3);

  // determination of the indexes of minor inside matrix
  // index of first row
  int i1 = (i > 0) ? 0 : 1;

  // index of second row
  int i2 = (i < 2) ? 2 : 1;

  // index of first column
  int j1 = ( j > 0) ? 0 : 1;

  // index of second column
  int j2 = (j < 2) ? 2 : 1;

  // minor as Det of matrix 2x2
  return (*this)[i1][j1] * (*this)[i2][j2] -
         (*this)[i1][j2] * (*this)[i2][j1];
  }

///////////////////////////////////////////////////////////////////////////////
/// Determinant of this matrix.
///
/// @return Determinant value.
///
template <class T>
double TMatrix3<T>::Det() const
  {
  return
       r0[0] * r1[1] * r2[2] -
       r0[0] * r1[2] * r2[1] +
       r0[1] * r1[2] * r2[0] -
       r0[1] * r1[0] * r2[2] +
       r0[2] * r1[0] * r2[1] -
       r0[2] * r1[1] * r2[0];
  }

///////////////////////////////////////////////////////////////////////////////
/// Get inversed matrix.
///
/// Method creates a new 3x3 matrix inverse to this matrix cf. product
/// of the new matrix with this matrix is identity matrix.
///
/// @note Determinant of this matrix must not be equal to zero.
///       Debug version asserts about this.
/// @return New 3x3 matrix (inverse to this matrix).
///
template <class T>
TMatrix3<T> TMatrix3<T>::Inversed() const
  {
  double det = Det();

  Assert(det != 0);

  TMatrix3<T> res;
  const double rdet = 1 / det;

  res[0][0] = (T)((r1[1] * r2[2] - r1[2] * r2[1])  * rdet);
  res[0][1] = (T)(-(r0[1] * r2[2] - r0[2] * r2[1]) * rdet);
  res[0][2] = (T)((r0[1] * r1[2] - r0[2] * r1[1])  * rdet);
  res[1][0] = (T)(-(r1[0] * r2[2] - r1[2] * r2[0]) * rdet);
  res[1][1] = (T)((r0[0] * r2[2] - r0[2] * r2[0])  * rdet);
  res[1][2] = (T)(-(r0[0] * r1[2] - r0[2] * r1[0]) * rdet);
  res[2][0] = (T)((r1[0] * r2[1] - r1[1] * r2[0])  * rdet);
  res[2][1] = (T)(-(r0[0] * r2[1] - r0[1] * r2[0]) * rdet);
  res[2][2] = (T)((r0[0] * r1[1] - r0[1] * r1[0])  * rdet);

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Get transposed matrix.
///
/// Method creates a new 3x3 matrix. New matrix is transposed with respect to
/// this matrix cf. rows of the new matrix are the columns of this matrix and
/// vice versa.
///
/// @return New 3x3 matrix transposed to this matrix.
///
template <class T>
TMatrix3<T> TMatrix3<T>::Transposed() const
  {
  TMatrix3<T> res;

  res.SetCol(0, r0);
  res.SetCol(1, r1);
  res.SetCol(2, r2);

  return res;
  }

// ----------------------------------------------------------------------------
//                        Rotation matrices
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around X axis at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication
/// of this matrix with the some vector <b> from the left </b> is vector
/// which is rotated with respect to initial vector around X axis at given angle
/// counterclockwise (view direction along -X axis).
///
/// @image html rotate_x.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationX(double angle)
  {
  *this = 1;

  T cos_angle = (T)Cos(angle);
  T sin_angle = (T)Sin(angle);

  r1[1] = cos_angle;
  r2[1] = -sin_angle;
  r1[2] = sin_angle;
  r2[2] = cos_angle;

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around Y axis at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication
/// of this matrix with the some vector <b> from the left </b> is vector
/// which is rotated with respect to initial vector around Y axis at given angle
/// counterclockwise (view direction along -Y axis).
///
/// @image html rotate_y.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationY(double angle)
  {
  *this = 1;

  T cos_angle = (T)Cos(angle);
  T sin_angle = (T)Sin(angle);

  r0[0] = cos_angle;
  r2[0] = sin_angle;
  r0[2] = -sin_angle;
  r2[2] = cos_angle;

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around Z axis at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication
/// of this matrix with the some vector <b> from the left </b> is vector
/// which is rotated with respect to initial vector around Z axis at given angle
/// counterclockwise (view direction along -Z axis).
///
/// @image html rotate_z.gif
///
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationZ(double angle)
  {
  *this = 1;

  T cos_angle = (T)Cos(angle);
  T sin_angle = (T)Sin(angle);

  r0[0] = cos_angle;
  r1[0] = -sin_angle;
  r0[1] = sin_angle;
  r1[1] = cos_angle;

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation to the given angles around X, Y, Z axes.
/// Rotation around X axis to X component, then around Y to Y component,
/// then around Z to Z component.
/// @return Reference to this matrix (modified).
///
#if 0
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationXYZ(const Vect3d &angles)
  {
  TMatrix3<T> mx;
  mx.RotationX(angles.x);
  TMatrix3<T> my;
  my.RotationY(angles.y);
  TMatrix3<T> mz;
  mz.RotationZ(angles.z);
  *this = mx * my * mz;

  return *this;
  }
#else

template <class T>
TMatrix3<T> &TMatrix3<T>::RotationXYZ(const Vect3d &angles)
  {
  double cx = Cos(angles.x);
  double sx = Sin(angles.x);
  double cy = Cos(angles.y);
  double sy = Sin(angles.y);
  double cz = Cos(angles.z);
  double sz = Sin(angles.z);

  r0[0] =  (T)(cy * cz);
  r0[1] =  (T)(cy * sz);
  r0[2] = -(T)(sy);

  r1[0] =  (T)(sx * sy * cz - cx * sz);
  r1[1] =  (T)(sx * sy * sz + cx * cz);
  r1[2] =  (T)(sx * cy);

  r2[0] =  (T)(cx * sy * cz + sx * sz);
  r2[1] =  (T)(cx * sy * sz - sx * cz);
  r2[2] =  (T)(cx * cy);

  return *this;
  }
#endif

//////////////////////////////////////////////////////////////////////////////
/// Represent rotation given by rotation matrix in the form of
/// rotation angles around X, Y, Z axes.
///
/// Rotation around X axis to X component, then around Y to Y component,
/// then around Z to Z component.
/// @param[out] angles Rotations around X, Y and Z axes.
/// @note Only for rotation matrices!
///
template <class T>
void TMatrix3<T>::RotationToXYZAngles(Vect3d &angles) const
  {
  double sy = -r0[2];
  Clip(sy, -1.0, 1.0);
  angles.y = ASin(sy); // -PI/2 - +PI/2
  double cy = Cos(angles.y);

  if (cy > Math<T>::TOLERANCE)
    {
    angles.x = ATan2(r1[2], r2[2]); // -PI - +PI
    angles.z = ATan2(r0[1], r0[0]); // -PI - +PI
    }
  else 
    {
    if (sy > 0)
      {
      // zmx = Z - X;
      double sin_zmx = (r2[1] - r1[0]) / (1 + sy);
      double cos_zmx = (r2[0] + r1[1]) / (1 + sy);
      Clip(sin_zmx, -1.0, 1.0);
      Clip(cos_zmx, -1.0, 1.0);
      double zmx = ATan2(sin_zmx, cos_zmx); // -PI - +PI
      // Only Z - X is defined, so one of them can be set arbitrarily.
      angles.x = 0;
      angles.z = zmx;
      }
    else
      {
      // zpx = Z + X;
      double sin_zpx = -(r1[0] + r2[1]) / (1 - sy);
      double cos_zpx = (r1[1] - r2[0]) / (1 - sy); 

      Clip(sin_zpx, -1.0, 1.0);
      Clip(cos_zpx, -1.0, 1.0);
      double zpx = ATan2(sin_zpx, cos_zpx); // -PI - +PI
      // Only Z + X is defined, so one of them can be set arbitrarily.
      angles.x = 0;
      angles.z = zpx;
      }

    TMatrix3<T> check;
    check.RotationXYZ(angles);
    Assert(MathF::AboutEqual(check.r0[0], r0[0]));
    Assert(MathF::AboutEqual(check.r0[1], r0[1]));
    Assert(MathF::AboutEqual(check.r0[2], r0[2]));
    Assert(MathF::AboutEqual(check.r1[0], r1[0]));
    Assert(MathF::AboutEqual(check.r1[1], r1[1]));
    Assert(MathF::AboutEqual(check.r1[2], r1[2]));
    Assert(MathF::AboutEqual(check.r2[0], r2[0]));
    Assert(MathF::AboutEqual(check.r2[1], r2[1]));
    Assert(MathF::AboutEqual(check.r2[2], r2[2]));
    }
  } // TMatrix3<T>::RotationToXYZAngles()

#if 0  // not used
///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation to the given angles around Z, Y, X axes.
/// Rotation around Z axis to Z component, then around Y to Y component,
/// then around X to X component.
/// @return Reference to this matrix (modified).
#if 1
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationZYX(const Vect3d &angles)
  {
  TMatrix3<T> mz;
  mz.RotationZ(angles.z);
  TMatrix3<T> my;
  my.RotationY(angles.y);
  TMatrix3<T> mx;
  mx.RotationX(angles.x);
  *this = mz * my * mx;
  //*this = mx * my * mz;

  return *this;
  }
#else

template <class T>
TMatrix3<T> &TMatrix3<T>::RotationZYX(const Vect3d &angles)
  {
  double cx = Cos(angles.x);
  double sx = Sin(angles.x);
  double cy = Cos(angles.y);
  double sy = Sin(angles.y);
  double cz = Cos(angles.z);
  double sz = Sin(angles.z);

  r0[0] =  (T)(cz * cy);
  r0[1] =  (T)(sz * cx + cz * sy * sx);
  r0[2] =  (T)(sz * sx - cz * sy * cx);

  r1[0] = -(T)(sz * cy);
  r1[1] =  (T)(cz * cx - sz * sy * sx);
  r1[2] =  (T)(cz * sx + sz * sy * cx);

  r2[0] =  (T)(sy);
  r2[1] = -(T)(cy * sx);
  r2[2] =  (T)(cy * cx);

  return *this;
  }
#endif

//////////////////////////////////////////////////////////////////////////////
/// Represent rotation given by rotation matrix in the form of
/// rotation angles around Z, Y, X axes.
///
/// Rotation around Z axis to Z component, then around Y to Y component,
/// then around X to X component.
/// @param[out] angles Rotations around X, Y and Z axes.
/// @note Only for rotation matrices!
///
template <class T>
void TMatrix3<T>::RotationToZYXAngles(Vect3d &angles) const
  {
  double sy = r2[0];
  Clip(sy, -1.0, 1.0);
  angles.y = ASin(sy);
  double cy = Cos(angles.y);
  if (cy > Math<T>::TOLERANCE)
    {
    angles.x = ATan2(-r2[1], r2[2]);
    angles.z = ATan2(-r1[0], r0[0]);
    }
  else
    {
    if (sy > 0)
      {
      // zpx = Z + X;
      double sin_zpx = (r0[1] + r1[2]) / (1 + sy);
      double cos_zpx = (r1[1] - r0[2]) / (1 + sy);
      double zpx = ATan2(sin_zpx, cos_zpx);
      angles.z = zpx;
      angles.x = 0;
      }
    else
      {
      // zmx = Z - X;
      double cos_zmx = (r0[2] + r1[1]) / (1 - sy);
      double sin_zmx = (r0[1] - r1[2]) / (1 - sy);
      double zmx = ATan2(sin_zmx, cos_zmx);
      angles.z = zmx;
      angles.x = 0;
      }
    }

  } // TMatrix3<T>::RotationToZYXAngles()

//////////////////////////////////////////////////////////////////////////////
/// Represent rotation given by rotation matrix in the form of
/// rotation angles and scale values for CS axes.
///
/// @param[out] rotation Rotations around X, Y and Z axes.
/// @param[out] scale Scales along X, Y and Z axes.
/// @note Code from tbtwriter.cpp. It is wrong!!
///
template <class T>
void TMatrix3<T>::MatrixToZYXAnglesScale(Vect3d &angles, Vect3d &scale) const
  {
  Vect3d v;
  TMatrix3<T> r;
  double angle;

  // Initial matrix
  TMatrix3<T> m(*this);

  // There are two cases: if J-vector == X or not
  if (MathD::AboutZero(m[1][1]) && MathD::AboutZero(m[1][2]))
    angles[0] = 0.0;
  else
    {
    v = ConvD(m[1]);
    v[0] = 0;
    v.Normalize();
    // rotate around Ox to put v to TBT Oxy plane
    double angle = ASin(CrossProd(Vect3d(0.0, 0.0, 1.0), v)[0]);
    if (Cos(v, Vect3d(0.0, 0.0, 1.0)) < 0.0)
      angle = PI - angle;
    if (angle < 0.0)
      angle = angle + PI2;
    r.RotationX(-angle);
    m *= r;
    angles[0] = angle;  // Rotation around TBT X axis
    }

  // Now J-vector is in Oxy plane. Rotate around TBT Oz to put it onto Y axis
  v = ConvD(m[1]); // Current J-vector
  v.Normalize();
  // Compare with TBT Oy axis
  angle = ASin(CrossProd(Vect3d(0.0, 0.0, 1.0), v)[1]);
  if (angle < 0)
    angle = angle + 2 * PI;
  r.RotationY(-angle);
  m *= r;
  angles[2] = angle; // Rotation around TBT Z axis
  // Now rotate around TBT Oy to fit all axes
  v = ConvD(m[0]); // Current I-vector
  v.Normalize();
  angle = ASin(CrossProd(Vect3d(1.0, 0.0, 0.0), v)[2]);
  if (Cos(v, Vect3d(1.0, 0.0, 0.0)) < 0.0)
    angle = PI - angle;
  if (angle < 0)
    angle = angle + PI2;

  r.RotationZ(-angle);
  m *= r;
  angles[1] = angle; // Rotation around TBT Y axis

  scale[0] = Length(m[0]);
  scale[1] = Length(m[1]);
  scale[2] = Length(m[2]);
  }  // ::MatrixToZYXAnglesScale()
#endif

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around given axis at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication
/// of this matrix with the some vector <b> from the left </b> is vector
/// which is rotated with respect to initial vector around given axis (rotation
/// axis is defined by the given vector) at given angle counterclockwise (view
/// direction is opposite to given axis).
///
/// @note Length of axis (3D vector) may not be less than tolerance
/// Math<T>::TOLERANCE. Debug version asserts about this.
///
/// @param u0 - Given 3D vector (rotation axis)
/// @param angle - Given angle in radians.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationAxis(const TVect3<T> &u0, double angle)
  {
  double cos_ang = Cos(angle);
  double sin_ang = Sin(angle);

  // Fgk, KLBC01db.Fgk, Normalize axis vector
  TVect3<T> u = u0;
  u.ModNormalize();

  RotationAxis(u, cos_ang, sin_ang);

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation around given axis at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication
/// of this matrix with the some vector <b> from the left </b> is vector
/// which is rotated with respect to initial vector around given axis (rotation
/// axis is defined by the unit vector) at given angle (angle defined by the
/// cosine and sine values) counterclockwise (view direction is opposite to
/// given axis).
///
/// @note Axis defined by the unit vector. Debug version asserts this.
///
/// @param u - 3D unit vector defines rotation axis.
/// @param cos_ang - Cosine of the rotation angle.
/// @param sin_ang - Sine of the rotation angle.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::RotationAxis(const TVect3<T> &u,
                                       double cos_ang, double sin_ang)
  {
  Assert(Abs(Length(u) - 1.0) < Math<T>::TOLERANCE);

  double one_cos = 1.0 - cos_ang;

  r0[0] = (T)(cos_ang + Sqr(u[0]) * (1 - cos_ang));
  r0[1] = (T)(u[0] * u[1] * one_cos + u[2] * sin_ang);
  r0[2] = (T)(u[0] * u[2] * one_cos - u[1] * sin_ang);

  r1[0] = (T)(u[0] * u[1] * one_cos - u[2] * sin_ang);
  r1[1] = (T)(cos_ang + Sqr(u[1]) * (1 - cos_ang)); 
  r1[2] = (T)(u[1] * u[2] * one_cos + u[0] * sin_ang);

  r2[0] = (T)(u[0] * u[2] * one_cos + u[1] * sin_ang);
  r2[1] = (T)(u[1] * u[2] * one_cos - u[0] * sin_ang);
  r2[2] = (T)(cos_ang + Sqr(u[2]) * (1 - cos_ang)); 

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Retrieve rotation axis and angle from this matrix of rotation.
/// http://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
/// (Axis–angle representation).
///
/// @param[out] axis Rotation axis.
/// @param[out] angle Rotation angle.
template <class T>
void TMatrix3<T>::RotAngAxis(Vect3d &axis, double &angle) const
  {
  double cos_val = (r0[0] + r1[1] + r2[2] - 1) / 2;
  Clip(cos_val, -1, 1);
  angle = ACos(cos_val);
  double sin_ang = Sin(angle);
  if (sin_ang == 0)
    return;
  // different sign - due to nonscientific layout of Matrix3.
  axis[0] = (r1[2] - r2[1]) * 0.5 / sin_ang;
  axis[1] = (r2[0] - r0[2]) * 0.5 / sin_ang;
  axis[2] = (r0[1] - r1[0]) * 0.5 / sin_ang;
  axis.ModNormalize();
  // significantly different from 1 for angle about 0. 
  // Assert(Abs(Length(axis) - 1.0) < Math<T>::TOLERANCE); 
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation to given axes.
///
/// Method modifies this matrix to rotation matrix. Resulted matrix rotates
/// X axis to first given vector, Y axis to second given vector, Z axis to
/// third given vector.
///
/// Given vectors define orientation of the coordinate axes of the new
/// coordinate system. Back rotation (vector multiplies matrix from the right)
/// of the point or vector by this matrix defines point position or vector
/// coordinates in the new coordinate system.
///
/// @note Given vectors should be orthogonal unit vectors.
///
/// @param u - First given vector - new X axis.
/// @param v - Second given vector - new Y axis.
/// @param w - Third given vector - new Z axis.
///
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::GenRotation(
    const TVect3<T> &u, const TVect3<T> &v, const TVect3<T> &w)
  {
  r0 = u;
  r1 = v;
  r2 = w;
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Creation of the rotation matrix from this matrix by the normalization
/// of the rows.
///
/// Method modifies this matrix to rotation matrix by the normalization rows
/// of this matrix. In result this matrix becomes pure rotation matrix without
/// scale.
///
/// @return 3D scale vector - length of each row (before normalization).
///
template <class T>
Vect3d TMatrix3<T>::Normalize()
  {
  Vect3d scale;

  scale.x = r0.ModNormalize();
  scale.y = r1.ModNormalize();
  scale.z = r2.ModNormalize();

  return scale;
  }

///////////////////////////////////////////////////////////////////////////////
/// Set matrix of rotation from given Euler angles.
///
/// Method modifies this matrix to rotation matrix. This rotation matrix is
/// defined by the three angles (parameters of the method):
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around Z axis.
///
/// @note Each new rotation is performed around own axes of the object.
///
/// @param alpha  - Value of the alpha angle in radians.
/// @param beta   - Value of the beta angle in radians.
/// @param gamma  - Value of the gamma angle in radians.
/// @return Reference to this matrix (modified).
///
/// @internal IM - from Mathematical Handbook of G. Korn, T. Korn, transpozed
/// due to different layout of matrices in that handbook and this library.
///
template <class T>
TMatrix3<T> &TMatrix3<T>::EulerRotation(double alpha, double beta,
                                        double gamma)
  {
  double cos_a = Cos(alpha);
  double sin_a = Sin(alpha);
  if (MathD::NearZero(sin_a))
    sin_a = 0.0;
  double cos_b = Cos(beta);
  if (MathD::NearZero(cos_b))
    cos_b = 0.0;
  double sin_b = Sin(beta);
  if (MathD::NearZero(sin_b))
    sin_b = 0.0;
  double cos_g = Cos(gamma);
  double sin_g = Sin(gamma);
  if (MathD::NearZero(sin_g))
    sin_g = 0.0;

  r0[0] =  (T)(cos_a * cos_b * cos_g - sin_a * sin_g);
  r1[0] = -(T)(cos_a * cos_b * sin_g + sin_a * cos_g);
  r2[0] =  (T)(cos_a * sin_b);
  r0[1] =  (T)(sin_a * cos_b * cos_g + cos_a * sin_g);
  r1[1] =  (T)(-sin_a * cos_b * sin_g + cos_a * cos_g);
  r2[1] =  (T)(sin_a * sin_b);
  r0[2] = -(T)(sin_b * cos_g);
  r1[2] =  (T)(sin_b * sin_g);
  r2[2] =  (T)(cos_b);

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculation of the Euler angles from rotation matrix.
///
/// This matrix is treated as rotation matrix. In this case Euler angles define
/// order of the rotations around coordinate axes:
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around Z axis.
///
/// Method calculates these Euler angles.
///
/// @note Each new rotation is performed around own axes of the object,
///
/// @param alpha  - Reference to the alpha angle in range  [-PI; +PI].
/// @param beta   - Reference to the beta angle in range  [0; +PI].
/// @param gamma  - Reference to the gamma angle in range [-PI; +PI].
/// @return None.
///
template <class T>
void TMatrix3<T>::RotationToEuler(double &alpha, double &beta, double &gamma) const
  {
  double cos_b = r2[2];
  Clip(cos_b, -1.0, 1.0);
  beta = ACos(cos_b);

  double sin_b = Sqrt(Sqr(r2[0]) + Sqr(r2[1]));
#ifdef DEBUG 
  double det = Det();
  Assert(Math<T>::AboutEqual(Abs(det), 1));  
#endif

  if (sin_b > Math<T>::TOLERANCE)
    {
    //double cos_a = r2[0] / sin_b;
    //double sin_a = r2[1] / sin_b;
    //alpha = ATan2(sin_a, cos_a);
    alpha = ATan2(r2[1], r2[0]);

    //double cos_g = -r0[2] / sin_b;
    //double sin_g = r1[2] / sin_b;
    //gamma = ATan2(sin_g, cos_g);
    gamma = ATan2(r1[2], -r0[2]);
    }
  else
    {
    double cos_a = r0[0] / cos_b;
    double sin_a = r0[1] / cos_b;
    alpha = ATan2(sin_a, cos_a);
    //alpha = ATan2(r0[1], r0[0]);
    gamma = 0;
    }

#ifdef DEBUG
  TMatrix3<T> m;
  m.EulerRotation(alpha, beta, gamma); 
  Assert(AboutEqual(m, *this, MathF::TOLERANCE * 10));
#endif
  } // END OF TMatrix3<T>::RotationToEuler FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Set matrix of rotation from given Euler angles with specific
/// orientation of the object (Camera).
///
/// Method modifies this matrix to rotation matrix of the object (Camera) with
/// specific orientation: view sight along -Z and initial direction of Up (OY)
/// vector along X. This rotation matrix is defined by the three angles
/// (parameters of the method):
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around -Z axis.
///
/// @note Each new rotation is performed around own axes of the object.
///
/// @param alpha  - Value of the alpha angle in radians.
/// @param beta   - Value of the beta angle in radians.
/// @param gamma  - Value of the gamma angle in radians.
/// @return Reference to this matrix (modified).
///
/// @internal In initial orientation Camera CS does not coincide with node CS.
/// This is a result of CS with Up vector along Z. Even with initial
/// orientation defined in this function camera shots scene from Top side,
/// so usually beta is set to 90 to make it horizontal.
///
template <class T>
TMatrix3<T> &TMatrix3<T>::EulerRotationForCamera(double alpha, double beta,
                                        double gamma)
  {
  EulerRotation(alpha + PI, beta, PI / 2 - gamma);

  return *this;
  } // END OF TMatrix3<T>::EulerRotationForCamera FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Calculation of the Euler angles from rotation matrix of the object (Camera)
/// with specific orientation.
///
/// This matrix is treated as rotation matrix of the object (Camera) with
/// specific orientation: view sight along -Z and initial direction of Up (OY)
/// vector along X. In this case Euler angles define order of the rotations
/// around coordinate axes:
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around -Z axis.
///
/// Method calculates these Euler angles.
///
/// @note Each new rotation is performed around own axes of the object.
///
/// @param alpha  - Reference to the alpha angle in range  [-PI; +PI].
/// @param beta   - Reference to the beta angle in range  [0; +PI].
/// @param gamma  - Reference to the gamma angle in range [-PI; +PI].
///
/// @internal In initial orientation Camera CS does not coincide with node CS.
/// This is a result of CS with Up vector along Z. Even with initial
/// orientation defined in this function camera shots scene from Top side,
/// so usually beta is set to 90 to make it horizontal.
///
template <class T>
void TMatrix3<T>::RotationToEulerForCamera(double &alpha, double &beta,
                                           double &gamma) const
  {
  RotationToEuler(alpha, beta, gamma);

  alpha = alpha - PI;
  if (alpha <= -PI)
    alpha += PI * 2;
  gamma =  PI / 2 - gamma;
  if (gamma >= PI)
    gamma -= PI * 2;
  } // END OF TMatrix3<T>::RotationToEulerForCamera FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Set matrix of rotation from given Euler angles with specific
/// orientation of the object (light).
///
/// Method modifies this matrix to rotation matrix of the object with
/// specific orientation: light with emission direction along -Z. This rotation
/// matrix is defined by the three angles (parameters of the method):
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around -Z axis.
///
/// @note Each new rotation is performed around own axes of the object.
///
/// @param alpha  - Value of the alpha angle in radians.
/// @param beta   - Value of the beta angle in radians.
/// @param gamma  - Value of the gamma angle in radians.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::EulerRotationForLight(double alpha, double beta,
                                                double gamma)
  {
  EulerRotation(alpha + PI, beta, PI - gamma);

  return *this;
  } // END OF TMatrix3<T>::EulerRotationForLight FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Calculation of the Euler angles from rotation matrix of the object (light)
/// with specific orientation.
///
/// This matrix is treated as rotation matrix of the object with specific
/// orientation: ight with emission direction along -Z. In this case Euler
/// angles define order of the rotations around coordinate axes:
/// - alpha defines rotation angle around Z axis;
/// - beta defines rotation angle around Y axis;
/// - gamma defines rotation angle around -Z axis.
///
/// Method calculates these Euler angles.
///
/// @note Each new rotation is performed around own axes of the object.
///
/// @param alpha  - Reference to the alpha angle in range  [-PI; +PI].
/// @param beta   - Reference to the beta angle in range  [0; +PI].
/// @param gamma  - Reference to the gamma angle in range [-PI; +PI].
///
template <class T>
void TMatrix3<T>::RotationToEulerForLight(double &alpha, double &beta,
                                           double &gamma) const
  {
  RotationToEuler(alpha, beta, gamma);

  if (!MathD::AboutZero(beta))
    {
    alpha = alpha - PI;
    gamma = PI - gamma;
    }
  else
    {
    // otherwise gamma = PI will be returned
    alpha = alpha + gamma;
    gamma = 0;
    }

  if (alpha <= -PI)
    alpha += PI * 2;
  if (gamma > PI)
    gamma -= PI * 2;
  } // END OF TMatrix3<T>::RotationToEulerForLight FUNCTION

///////////////////////////////////////////////////////////////////////////////
/// Calculate rotation matrix for camera using TBT twist angle.
///
/// Method modifies this matrix to rotation matrix for rotation of arbitrary
/// source Direction vector @a s_dir to destination Direction vector @a d_dir
/// and then rotation around destination to the @a twist angle.
/// Resulted rotation matrix is combination of the three rotations:
/// - 1. Rotation of the @a s_dir vector to @a d_dir vector in the plane
/// defined by these vectors.
/// - 2. Rotation between normals (from 'source' to 'destination'):
///   - 'source' normal is normal to plane defined by the rotated source Up
///     vector and @a d_dir vector;
///   - 'destination' normal is normal to plane defined by the destination Up
///     vector @a d_up and @a d_dir vector .
/// - 3. Rotation around @a d_dir vector to the @a twist angle.
///
/// Normals to planes are calculated as Cross products (see function
/// CrossProd(const TVect3<T> &a, const TVect3<T> &b)) of the vectors which
/// define planes.
///
/// @note Source Up @a s_up and Direction @a s_dir vectors should not coincide.
/// Destination Up @a d_up and Direction @a d_dir vectors - can coincide. In
/// this case additionally pointed reference normal @a add_r_nm is used.
///
/// @param[in] s_dir - Unit vector of the source direction. 
///                    For Lumicept CS - (0,0,-1), for TBT (0,0,1).  
/// @param[out] d_dir - Unit Vector of the destination direction.
/// @param[in] s_up  - Source UP vector (Camera Up vector in local CS of camera). 
///                   (0,1,0) both for Lumicept and TBT.
/// @param[in] d_up  - Destination UP vector (World CS UP vector, from which twist angle is counted). 
///                    (0,0,1) for Lumicept, (0,1,0) for TBT.
/// @param[in] add_r_nm - 3D vector is used if destination direction coincide with source, or with UP direction.
///                     (-1,0,0) for Lumicept, (1,0,0) for TBT.
/// @param[out] twist - Value of the rotation angle. Have different sign in Lumicept and TBT.
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::DirTwistRotation(const TVect3<T> &s_dir, const TVect3<T> &d_dir,  
                                           const TVect3<T> &s_up, const TVect3<T> &d_up,
                                           const TVect3<T> &add_r_nm, const double &twist)
  {
  TMatrix3<T> rot1, rot2, rot3;

  rot1.Rotation(s_dir, d_dir, add_r_nm);

  // rotated source up vector
  TVect3<T> r_s_up(s_up);
  rot1.Rotate(r_s_up);

  // normal of reference destination vertical plane:
  TVect3<T> d_nm = CrossProd(d_dir, d_up);
  if (d_nm.ModNormalize() < MathF::TOLERANCE)
    d_nm = add_r_nm;

  // normal of rotated source vertical plane:
  TVect3<T> s_nm = CrossProd(d_dir, r_s_up);
  double s_nm_len = s_nm.ModNormalize();
  s_nm_len = s_nm_len; // Avoid warning.
  Assert(s_nm_len != 0);

  // rotation from s_nm to d_nm (to 0 twist angle orientation),
  // around d_dir.
  rot2.Rotation(s_nm, d_nm, d_dir);

  // final rotation
  rot3.RotationAxis(d_dir, twist);

  *this = rot1 * rot2 * rot3;

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Calculate destination vector and twist angle from this rotation matrix.
///
/// @param[in] s_dir - Unit vector of the source direction. 
///                    For Lumicept CS - (0,0,-1), for TBT (0,0,1).  
/// @param[out] d_dir - Unit Vector of the destination direction.
/// @param[in] s_up  - Source UP vector (Camera Up vector in local CS of camera). 
///                   (0,1,0) both for Lumicept and TBT.
/// @param[in] d_up  - Destination UP vector (World CS UP vector, from which twist angle is counted). 
///                    (0,0,1) for Lumicept, (0,1,0) for TBT.
/// @param[in] add_r_nm - Direction used if destination direction coincide with source, or with UP direction.
///                     (-1,0,0) for Lumicept, (1,0,0) for TBT.
/// @param[out] twist - Value of the rotation angle. Have different sign in Lumicept and TBT.
template <class T>
void TMatrix3<T>::RotationToDirTwist(const TVect3<T> &s_dir, TVect3<T> &d_dir,  
                                     const TVect3<T> &s_up, const TVect3<T> &d_up,
                                     const TVect3<T> &add_r_nm, double &twist) const
  {
  d_dir = Rotated(s_dir);
  // to compensate possible non-unit scale.
  d_dir.ModNormalize();

  TMatrix3<T> rot1, rot2, rot3;

  rot1.Rotation(s_dir, d_dir, add_r_nm);

  // rotated source up vector
  TVect3<T> r_s_up(s_up);
  rot1.Rotate(r_s_up);

  // normal of reference destination vertical plane:
  TVect3<T> d_nm = CrossProd(d_dir, d_up);
  if (d_nm.ModNormalize() < MathF::TOLERANCE)
    d_nm = add_r_nm;

  // normal of rotated source vertical plane:
  TVect3<T> s_nm = CrossProd(d_dir, r_s_up);
  double s_nm_len = s_nm.ModNormalize();
  s_nm_len = s_nm_len; // avoid warning.
  Assert(s_nm_len != 0);

  // rotation from s_nm to d_nm (to 0 twist angle orientation),
  // around d_dir.
  rot2.Rotation(s_nm, d_nm, d_dir);

  // rot1 * rot2 * rot3 = *this;
  // rot2 * rot3 = rot1.Transposed() * *this;
  rot3 = rot2.Transposed() * rot1.Transposed() * *this;
  // final rotation
  // rot3.RotationAxis(d_dir, twist);

  Vect3d axis;
  double ang;
  rot3.RotAngAxis(axis, ang);
  double dp = DotProd(axis, ConvD(d_dir));
  if (dp < 0)
    ang = -ang;
  twist = ang;
  }

///////////////////////////////////////////////////////////////////////////////
/// Product of the matrices with preliminary transposition of the left
/// multiplier.
///
/// Method creates a new 3x3 matrix by the multiplication of the transposed
/// first given matrix on the second given matrix.
///
/// @param u - First given matrix 3x3 - left multiplier.
/// @param v - Second given matrix 3x3 - right multiplier.
/// @return New matrix 3x3.
///
template <class T>
TMatrix3<T> TMatrix3<T>::LeftTransposeMultiply(const TMatrix3<T> &u,
                                  const TMatrix3<T> &v)
  {
  TMatrix3<T> res;

  // row(transp. u * v) == transp.(col(v) * u)
  for (int i = 0; i < 3 ; i++)
    res[i] = v.GetCol(i) * u;

  res.Transpose();
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Product of the matrices with preliminary transposition of the right
/// multiplier.
///
/// Method creates a new 3x3 matrix by the multiplication of the first given
/// matrix on the transposed second given matrix.
///
/// @param u - First given matrix 3x3 - left multiplier.
/// @param v - Second given matrix 3x3 - right multiplier.
/// @return New matrix 3x3.
///
template <class T>
TMatrix3<T> TMatrix3<T>::RightTransposeMultiply(const TMatrix3<T> &u,
                       const TMatrix3<T> &v)
  {
  TMatrix3<T> res(0.0);

  // col(u * transp. v) == u * row(v)
  for (int i = 0; i < 3 ; i++)
    res.SetCol(i, u * v[i]);

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation between given vectors.
///
/// Method modifies this matrix to rotation matrix. This rotation matrix
/// rotates the first given vector to the second given vector in the plane
/// defined by these vectors. Plane of rotation is not defined if given vectors
/// are the opposite vectors. In this case rotation plane is defined by the
/// normal vector returned by the method TVect3<T>::AnyOrthogonal() for the
/// first given vector.
///
/// @note Given vectors must be unit. Debug version asserts this.
/// @param src - First given 3D vector (source).
/// @param trg - Second given 3D vector (target).
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::Rotation(const TVect3<T> &src, const TVect3<T> &trg)
  {
  Assert(Abs(Length(src) - 1.0) < Math<T>::TOLERANCE);
  Assert(Abs(Length(trg) - 1.0) < Math<T>::TOLERANCE);

  // rotation axis
  TVect3<T> vect = CrossProd(src, trg);

  double sin_d = vect.ModNormalize();
  double cos_d = DotProd(src, trg);

  if (sin_d < MathD::TOLERANCE)
    {
    if (cos_d >= 0.0)
      *this = (T)1.0;
    else
      RotationAxis(src.AnyOrthogonal(), -1, 0);
    }
  else
    RotationAxis(vect, cos_d, sin_d);

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation between given vectors.
///
/// Method modifies this matrix to rotation matrix. This rotation matrix
/// rotates the first given vector to the second given vector in the plane
/// defined by these vectors. Plane of rotation is not defined if given vectors
/// are the opposite vectors. In this case rotation plane is defined by the
/// normal vector - third parameter of the method.
///
/// @note Given vectors must be unit. Debug version asserts this.
/// @param[in] src - First given 3D vector (source).
/// @param[in] trg - Second given 3D vector (target).
/// @param[in] def_rot_dir - Third given 3D vector (used in the case when first
/// given vector is opposite to second given vector).
/// @return Reference to this matrix (modified).
///
template <class T>
TMatrix3<T> &TMatrix3<T>::Rotation(const TVect3<T> &src, const TVect3<T> &trg,
  const TVect3<T> &def_rot_dir)
  {
  Assert(Abs(Length(src) - 1.0) < Math<T>::TOLERANCE);
  Assert(Abs(Length(trg) - 1.0) < Math<T>::TOLERANCE);

  // rotation axis
  TVect3<T> vect = CrossProd(src, trg);

  double sin_d = vect.ModNormalize();
  double cos_d = DotProd(src, trg);

  if (sin_d < MathD::TOLERANCE)
    {
    if (cos_d >= 0.0)
      *this = E();
    else
      RotationAxis(def_rot_dir, cos_d, sin_d);
    }
  else
    RotationAxis(vect, cos_d, sin_d);

  return *this;
  }


//////////////////////////////////////////////////////////////////////////////
/// Get scale of a matrix (lazy form, used in Scene transform classes).
///
/// @return Scale of the transfor.
///
template <class T>
Vect3d TMatrix3<T>::GetScale() const
  {
  Vect3d scale;
  scale.x = r0.Length();
  scale.y = r1.Length();
  scale.z = r2.Length();
  
  return scale;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set scale of a matrix (lazy form, used in Scene transform classes).
///
/// @param[in] scale New Scale for the transform.
///
template <class T>
void TMatrix3<T>::SetScale(const Vect3d &scale)
  {
  Normalize();
  r0 *= scale.x;
  r1 *= scale.y;
  r2 *= scale.z;
  }

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
///
template <class T>
void TMatrix3<T>::Value(Serializer &inout)
  {
  inout.Value(&r0[0], 9);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the matrix using identity one as default
///
/// @param inout - Serialization medium
/// @param tag   - Name of the serialized object
///
template <class T>
void TMatrix3<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  TMatrix3<T> dval;
  dval.Identity();
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to predefined identity matrix.
///
/// Method provides access to predefined identity matrix. Diagonal elements of
/// the rotation part (first three rows) of this matrix are 1 and the rest
/// elements are zero.
///
/// @return Reference to predefined identity matrix
///
template <class T>
const TMatrix3<T> &TMatrix3<T>::E()
  {
  static const TMatrix3<T> e(1.0);
  return e;
  }

//////////////////////////////////////////////////////////////////////////////
/// Predefined vector used by twist angle calculations for Lumicept.
/// This vector is used if UP vector and destination vectors coincide.
/// @return Reference to predefined vector.
template <class T>
const TVect3<T> TMatrix3<T>::DefCamTwistDir()
  {
  return TVect3<T>(1, 0, 0);
  }
  
//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(TMatrix3, double)
LIB_TEMPLATE_INSTANCE(TMatrix3, float)
INTEGRA_NAMESPACE_END
