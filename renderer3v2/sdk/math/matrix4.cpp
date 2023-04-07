/// @internal
/// @file
///
/// @brief  Methods of class for matrices 4x4.
///
/// @author Enk - Nikolay Kirilov<BR>
///         Vai - Alexey Ignatenko
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///

#include <integra.hpp>

#include <base/serializer.hpp>

#include "matrix4.hpp"

INTEGRA_NAMESPACE_START

/**
@class TMatrix4 math/matrix4.hpp

Matrix 4x4 of the arbitrary type.

The template class TMatrix4<T>  is used to store 4x4 matrices of template
type: T. Matrix4f and Matrix4d are predefined instances of the template class
for matrices with float and double elements, respectively.

The user may create own instances of the template class if needed. Type T must
support assignment and usual arithmetic and comparison operators (including
comparison with zero).

Conceptually 4x4 matrix consists of four vectors: r0, r1, r2, r3. First vector
is first row of the matrix, second vector is second row of the matrix and so on.
Underlying implementation of 4x4 matrix is

@code
  public:
    TVect4<T> r0, r1, r2, r3;
@endcode

User has possibility of direct access to the rows of the matrix.

The main feature of this class is creation of perspective and orthographic
projection matrices.

@param T - template type of the matrix elements,
*/

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Method creates a new 4x4 matrix. Elements of the new matrix are the Dot
/// products (see function DotProd4()) of the rows of this matrix and columns
/// of the given matrix. Let i - index of the row (i = 0, 1, 2, 3), j - index
/// of the column (j = 0, 1, 2, 3) then element (i, j) of the new matrix is Dot
/// product of the i-th row of this matrix and j-th column of the given matrix.
///
/// @param v - Given matrix 4x4 (multiplier).
/// @return New 4x4 matrix.
///
template <class T>
TMatrix4<T> TMatrix4<T>::operator * (const TMatrix4<T> &v) const
  {
  TMatrix4<T> res;

  res.r0[0] = r0.x * v.r0.x + r0.y * v.r1.x + r0.z * v.r2.x + r0.w * v.r3.x;
  res.r0[1] = r0.x * v.r0.y + r0.y * v.r1.y + r0.z * v.r2.y + r0.w * v.r3.y;
  res.r0[2] = r0.x * v.r0.z + r0.y * v.r1.z + r0.z * v.r2.z + r0.w * v.r3.z;
  res.r0[3] = r0.x * v.r0.w + r0.y * v.r1.w + r0.z * v.r2.w + r0.w * v.r3.w;

  res.r1[0] = r1.x * v.r0.x + r1.y * v.r1.x + r1.z * v.r2.x + r1.w * v.r3.x;
  res.r1[1] = r1.x * v.r0.y + r1.y * v.r1.y + r1.z * v.r2.y + r1.w * v.r3.y;
  res.r1[2] = r1.x * v.r0.z + r1.y * v.r1.z + r1.z * v.r2.z + r1.w * v.r3.z;
  res.r1[3] = r1.x * v.r0.w + r1.y * v.r1.w + r1.z * v.r2.w + r1.w * v.r3.w;

  res.r2[0] = r2.x * v.r0.x + r2.y * v.r1.x + r2.z * v.r2.x + r2.w * v.r3.x;
  res.r2[1] = r2.x * v.r0.y + r2.y * v.r1.y + r2.z * v.r2.y + r2.w * v.r3.y;
  res.r2[2] = r2.x * v.r0.z + r2.y * v.r1.z + r2.z * v.r2.z + r2.w * v.r3.z;
  res.r2[3] = r2.x * v.r0.w + r2.y * v.r1.w + r2.z * v.r2.w + r2.w * v.r3.w;

  res.r3[0] = r3.x * v.r0.x + r3.y * v.r1.x + r3.z * v.r2.x + r3.w * v.r3.x;
  res.r3[1] = r3.x * v.r0.y + r3.y * v.r1.y + r3.z * v.r2.y + r3.w * v.r3.y;
  res.r3[2] = r3.x * v.r0.z + r3.y * v.r1.z + r3.z * v.r2.z + r3.w * v.r3.z;
  res.r3[3] = r3.x * v.r0.w + r3.y * v.r1.w + r3.z * v.r2.w + r3.w * v.r3.w;

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Get transposed matrix.
///
/// Method creates a new 4x4 matrix. New matrix is transposed with respect to
/// this matrix cf. rows of the new matrix are the columns of this matrix and
/// vice versa.
///
/// @return New 4x4 matrix equal to transposed this matrix.
///
template <class T>
TMatrix4<T> TMatrix4<T>::Transposed() const
  {
  TMatrix4<T> res;

  res.SetCol(0, r0);
  res.SetCol(1, r1);
  res.SetCol(2, r2);
  res.SetCol(3, r3);

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Normalize rows of this matrix, returns new math vector 
/// consisting of lengths of the rows before normalization.
///
/// @return 4D scale vector - length of each row (before normalization).
///
template <class T>
Vect4d TMatrix4<T>::Normalize()
  {
  Vect4d scale;

  scale.x = r0.ModNormalize();
  scale.y = r1.ModNormalize();
  scale.z = r2.ModNormalize();
  scale.w = r3.ModNormalize();

  return scale;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get inversed matrix.
///
/// Method creates a new 4x4 matrix inverse to this matrix cf. product
/// of the new matrix with this matrix is identity matrix.
///
/// @note Inverse matrix is calculated by the Cramer's Rule.
///
/// @return New 4x4 matrix equal to inversed this matrix.
///
template <class T>
TMatrix4<T> TMatrix4<T>::Inversed() const
  {
  T tmp[12]; // temp array for pairs
  TMatrix4<T> m;

  // calculate pairs for first 8 elements (cofactors)
  tmp[0]  = r2.z * r3.w;
  tmp[1]  = r3.z * r2.w;
  tmp[2]  = r1.z * r3.w;
  tmp[3]  = r3.z * r1.w;
  tmp[4]  = r1.z * r2.w;
  tmp[5]  = r2.z * r1.w;
  tmp[6]  = r0.z * r3.w;
  tmp[7]  = r3.z * r0.w;
  tmp[8]  = r0.z * r2.w;
  tmp[9]  = r2.z * r0.w;
  tmp[10] = r0.z * r1.w;
  tmp[11] = r1.z * r0.w;

  // calculate first 8 elements (cofactors)
  m.r0.x  = tmp[0] * r1.y + tmp[3] * r2.y + tmp[4]  * r3.y;
  m.r0.x -= tmp[1] * r1.y + tmp[2] * r2.y + tmp[5]  * r3.y;
  m.r0.y  = tmp[1] * r0.y + tmp[6] * r2.y + tmp[9]  * r3.y;
  m.r0.y -= tmp[0] * r0.y + tmp[7] * r2.y + tmp[8]  * r3.y;
  m.r0.z  = tmp[2] * r0.y + tmp[7] * r1.y + tmp[10] * r3.y;
  m.r0.z -= tmp[3] * r0.y + tmp[6] * r1.y + tmp[11] * r3.y;
  m.r0.w  = tmp[5] * r0.y + tmp[8] * r1.y + tmp[11] * r2.y;
  m.r0.w -= tmp[4] * r0.y + tmp[9] * r1.y + tmp[10] * r2.y;
  m.r1.x  = tmp[1] * r1.x + tmp[2] * r2.x + tmp[5]  * r3.x;
  m.r1.x -= tmp[0] * r1.x + tmp[3] * r2.x + tmp[4]  * r3.x;
  m.r1.y  = tmp[0] * r0.x + tmp[7] * r2.x + tmp[8]  * r3.x;
  m.r1.y -= tmp[1] * r0.x + tmp[6] * r2.x + tmp[9]  * r3.x;
  m.r1.z  = tmp[3] * r0.x + tmp[6] * r1.x + tmp[11] * r3.x;
  m.r1.z -= tmp[2] * r0.x + tmp[7] * r1.x + tmp[10] * r3.x;
  m.r1.w  = tmp[4] * r0.x + tmp[9] * r1.x + tmp[10] * r2.x;
  m.r1.w -= tmp[5] * r0.x + tmp[8] * r1.x + tmp[11] * r2.x;

  // calculate pairs for second 8 elements (cofactors)
  tmp[0]  = r2.x * r3.y;
  tmp[1]  = r3.x * r2.y;
  tmp[2]  = r1.x * r3.y;
  tmp[3]  = r3.x * r1.y;
  tmp[4]  = r1.x * r2.y;
  tmp[5]  = r2.x * r1.y;
  tmp[6]  = r0.x * r3.y;
  tmp[7]  = r3.x * r0.y;
  tmp[8]  = r0.x * r2.y;
  tmp[9]  = r2.x * r0.y;
  tmp[10] = r0.x * r1.y;
  tmp[11] = r1.x * r0.y;

  // calculate second 8 elements (cofactors)
  m.r2.x  = tmp[0]  * r1.w + tmp[3]  * r2.w + tmp[4]  * r3.w;
  m.r2.x -= tmp[1]  * r1.w + tmp[2]  * r2.w + tmp[5]  * r3.w;
  m.r2.y  = tmp[1]  * r0.w + tmp[6]  * r2.w + tmp[9]  * r3.w;
  m.r2.y -= tmp[0]  * r0.w + tmp[7]  * r2.w + tmp[8]  * r3.w;
  m.r2.z  = tmp[2]  * r0.w + tmp[7]  * r1.w + tmp[10] * r3.w;
  m.r2.z -= tmp[3]  * r0.w + tmp[6]  * r1.w + tmp[11] * r3.w;
  m.r2.w  = tmp[5]  * r0.w + tmp[8]  * r1.w + tmp[11] * r2.w;
  m.r2.w -= tmp[4]  * r0.w + tmp[9]  * r1.w + tmp[10] * r2.w;
  m.r3.x  = tmp[2]  * r2.z + tmp[5]  * r3.z + tmp[1]  * r1.z;
  m.r3.x -= tmp[4]  * r3.z + tmp[0]  * r1.z + tmp[3]  * r2.z;
  m.r3.y  = tmp[8]  * r3.z + tmp[0]  * r0.z + tmp[7]  * r2.z;
  m.r3.y -= tmp[6]  * r2.z + tmp[9]  * r3.z + tmp[1]  * r0.z;
  m.r3.z  = tmp[6]  * r1.z + tmp[11] * r3.z + tmp[3]  * r0.z;
  m.r3.z -= tmp[10] * r3.z + tmp[2]  * r0.z + tmp[7]  * r1.z;
  m.r3.w  = tmp[10] * r2.z + tmp[4]  * r0.z + tmp[9]  * r1.z;
  m.r3.w -= tmp[8]  * r1.z + tmp[11] * r2.z + tmp[5]  * r0.z;

  // calculate matrix inverse
  T div = (r0.x * m.r0.x + r1.x * m.r0.y + r2.x * m.r0.z + r3.x * m.r0.w);
  Assert(div != 0);
  m *= 1 / div;

  return m;
  }

///////////////////////////////////////////////////////////////////////////////
/// Determinant of this matrix.
///
/// @return Determinant value.
///
template <class T>
double TMatrix4<T>::Det() const
  {
  // Calculate the determinant of the matrix by the first line decomposing
  TMatrix3<T> m0(TVect3<T>(r1[1], r1[2], r1[3]), 
                 TVect3<T>(r2[1], r2[2], r2[3]), 
                 TVect3<T>(r3[1], r3[2], r3[3]));

  TMatrix3<T> m1(TVect3<T>(r1[0], r1[2], r1[3]), 
                 TVect3<T>(r2[0], r2[2], r2[3]), 
                 TVect3<T>(r3[0], r3[2], r3[3]));

  TMatrix3<T> m2(TVect3<T>(r1[0], r1[1], r1[3]), 
                 TVect3<T>(r2[0], r2[1], r2[3]), 
                 TVect3<T>(r3[0], r3[1], r3[3]));

  TMatrix3<T> m3(TVect3<T>(r1[0], r1[1], r1[2]), 
                 TVect3<T>(r2[0], r2[1], r2[2]), 
                 TVect3<T>(r3[0], r3[1], r3[2]));

  return (r0[0] * m0.Det()) - (r0[1] * m1.Det()) + (r0[2] * m2.Det()) - (r0[3] * m3.Det());
  }

//////////////////////////////////////////////////////////////////////////////
/// Perspective projection matrix (truncated pyramid is defined by the view
/// angle).
///
/// This method is analog of the method Frustum() but truncated cone is defined
/// by the view angle. There are following transitional formulas (see method
/// Frustum()):
/// - top = z_near * Tan(foy / 2);
/// - bottom = -top;
/// - right = top * aspect;
/// - left = -right.
///
/// There are direct calculation formulas:
/// - a(0, 0) = f / aspect;
/// - a(1, 1) = f;
/// - a(2, 2) = -(z_far + z_near) / (z_far - z_near);
/// - a(2, 3) = -1;
/// - a(3, 2) = -2 * z_far * z_near / (z_far - z_near),<BR>
/// where f = 1 / Tan(fovy / 2) and the rest elements are zero,
///
/// @note This method is similar with function gluPerspective() of the OpenGL.
/// @param fovy   - angle of visibility along Y axis.
/// @param aspect - ratio of the sides of the visibility rectangle.
/// @param z_near - distance from observer to near clipping plane along Z axis.
/// @param z_far  - distance from observer to far clipping plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::Perspective(double fovy, double aspect,
                              double z_near, double z_far)
  {
  Assert(z_far > z_near);
  T f = static_cast<T>(1 / Tan(fovy / 2));

  r0[0] = static_cast<T>(f / aspect);
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = f;
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = 0;
  r2[1] = 0;
  r2[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  r2[3] = -1;

  r3[0] = 0;
  r3[1] = 0;
  r3[2] = static_cast<T>(-2 * z_far * z_near / (z_far - z_near));
  r3[3] = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Perspective projection matrix (truncated pyramid is defined by the view
/// angle and far clipping plane at the infinity).
///
/// This method is analog of the method Perspective() but coefficients of the
/// pseudodepth function (see method Frustum()) calculated by the following
/// formulas:
/// - a = -1;
/// - b = -2 * z_near;
///
/// There are direct calculation formulas:
/// - a(0, 0) = f / aspect;
/// - a(1, 1) = f;
/// - a(2, 2) = -(1 - Math<T>::TOLERANCE);
/// - a(2, 3) = -1;
/// - a(3, 2) = z_near * (Math<T>::TOLERANCE - 1),<BR>
/// where f = 1 / Tan(fovy / 2) and the rest elements are zero,
///
/// @param fovy   - angle of visibility along Y axis.
/// @param aspect - ratio of the sides of the visibility rectangle.
/// @param z_near - distance from observer to near clipping plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::InfinitePerspective(double fovy, double aspect, double z_near)
  {
  T f = static_cast<T>(1 / Tan(fovy / 2));

  r0[0] = static_cast<T>(f / aspect);
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = f;
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = 0;
  r2[1] = 0;
  //r2[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  // Approximation for z_far >> z_near
  r2[2] = -static_cast<T>(1 + Math<T>::TOLERANCE * 2);
  r2[3] = -1;

  r3[0] = 0;
  r3[1] = 0;
  //r3[2] = static_cast<T>(-2 * z_far * z_near / (z_far - z_near));
  // Approximation for z_far >> z_near
  r3[2] = -static_cast<T>(2 * z_near * (1 + Math<T>::TOLERANCE));
  r3[3] = 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Perspective projection matrix (truncated pyramid is defined by the two
/// points).
///
/// Parameters of the method define truncated pyramid of the visibility. The
/// triple (left, bottom, z_near)  defines point on the near clipping plane
/// (plane perpendicular to z-axis), the triple (right, top, z_far) defines
/// point on the far clipping plane.
///
/// Method modifies this matrix to perspective projection matrix. This matrix
/// provides perspective transformation with scaling and translation of the
/// truncated pyramid to canonical view volume CVV - cube [-1;+1].
///
/// Created matrix corresponding to the following transformations:
/// - Perspective transformation with pseudodepth. Observer is placed in (0,0,0),
/// view direction {0, 0, -1}. Point {x, y, z} is transformed to point:
///   - {-z_near * x / z; -z_near * y / z; -(a * z + b) / z}, where a, b are
/// coefficients of the pseudodepth function:
///     - a = - (z_far + z_near) / (z_far - z_near),
///     - b = - 2 * z_near * z_far / (z_far - z_near).
///
/// - Translation:
///   - -(right + left) / 2 along x-axis;
///   - -(top + bottom) / 2 along y-axis.
/// - Scaling with scale factors:
///   - 2 / (right - left) along X axis;
///   - 2 / (top - bottom) along Y axis.
///
/// There are direct calculation formulas:
/// - a(0, 0) = 2 * z_near / (right - left);
/// - a(1, 1) = 2 * z_near / (top - bottom);
/// - a(2, 0) = (right + left) / (right - left);
/// - a(2, 1) = (top + bottom) / (top - bottom);
/// - a(2, 2) = - (z_far + z_near) / (z_far - z_near);
/// - a(2, 3) = -1;
/// - a(3, 2) = - 2 * z_far * z_near / (z_far - z_near);<BR>
/// the rest elements are zero.
///
/// @note This method is similar with function glFrustum() of the OpenGL.
///
/// @param left   - coordinate of the left clipping plane.
/// @param right  - coordinate of the right clipping plane.
/// @param bottom - coordinate of the bottom clipping plane.
/// @param top    - coordinate of the top clipping plane.
/// @param z_near - distance from observer to near clipping plane along Z axis.
/// @param z_far  - distance from observer to far clipping plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::Frustum(double left, double right, double bottom, double top,
                          double z_near, double z_far)
  {
  Assert(right != left);
  Assert(top != bottom);
  Assert(z_far > z_near);

  r0[0] = static_cast<T>(2 * z_near / (right - left));
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = static_cast<T>(2 * z_near / (top - bottom));
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = static_cast<T>((right + left) / (right - left));
  r2[1] = static_cast<T>((top + bottom) / (top - bottom));
  r2[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  r2[3] = -1;

  r3[0] = 0;
  r3[1] = 0;
  r3[2] = static_cast<T>(-2 * z_far * z_near / (z_far - z_near));
  r3[3] = 0;
  }

///////////////////////////////////////////////////////////////////////////////
/// Perspective projection matrix for infinite far clipping plane
/// (truncated pyramid is defined by the two points).
///
/// Parameters of the method define truncated pyramid of the visibility. The
/// triple (left, bottom, z_near)  defines point on the near clipping plane
/// (plane perpendicular to z-axis), the triple (right, top, z_far) defines
/// point on the far clipping plane.
///
/// Method modifies this matrix to perspective projection matrix. This matrix
/// provides perspective transformation with scaling and translation of the
/// truncated pyramid to canonical view volume CVV - cube [-1;+1].
///
/// Created matrix corresponding to the following transformations:
/// - Perspective transformation with pseudodepth. Observer is placed in (0,0,0),
/// view direction {0, 0, -1}. Point {x, y, x} is transformed to point:
///   - {-z_near * x / z; -z_near * y / z; -(a * z + b) / z}, where a, b are
/// coefficients of the pseudodepth function:
///     - a = - (z_far + z_near) / (z_far - z_near),
///     - b = - 2 * z_near * z_far / (z_far - z_near).
///
/// - Translation:
///   - -(right + left) / 2 along x-axis;
///   - -(top + bottom) / 2 along y-axis.
/// - Scaling with scale factors:
///   - 2 / (right - left) along X axis;
///   - 2 / (top - bottom) along Y axis.
///
/// There are direct calculation formulas:
/// - a(0, 0) = 2 * z_near / (right - left);
/// - a(1, 1) = 2 * z_near / (top - bottom);
/// - a(2, 0) = (right + left) / (right - left);
/// - a(2, 1) = (top + bottom) / (top - bottom);
/// - a(2, 2) = - (z_far + z_near) / (z_far - z_near);
/// - a(2, 3) = -1;
/// - a(3, 2) = - 2 * z_far * z_near / (z_far - z_near);<BR>
/// the rest elements are zero.
///
/// @note This method is similar with function glFrustum() of the OpenGL.
///
/// @param left   - coordinate of the left clipping plane.
/// @param right  - coordinate of the right clipping plane.
/// @param bottom - coordinate of the bottom clipping plane.
/// @param top    - coordinate of the top clipping plane.
/// @param z_near - distance from observer to near clipping plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::InfiniteFrustum(double left, double right, double bottom,
                                  double top, double z_near)
  {
  Assert(right != left);
  Assert(top != bottom);

  r0[0] = static_cast<T>(2 * z_near / (right - left));
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = static_cast<T>(2 * z_near / (top - bottom));
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = static_cast<T>((right + left) / (right - left));
  r2[1] = static_cast<T>((top + bottom) / (top - bottom));
  //r2[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  // Approximation for z_far >> z_near
  r2[2] = -static_cast<T>(1 + Math<T>::TOLERANCE * 2);
  r2[3] = -1;

  r3[0] = 0;
  r3[1] = 0;
  //r3[2] = static_cast<T>(-2 * z_far * z_near / (z_far - z_near));
  // Approximation for z_far >> z_near
  r3[2] = -static_cast<T>(2 * z_near * (1 + Math<T>::TOLERANCE));
  r3[3] = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Orthographic projection matrix.
///
/// Parameters of the method define truncated pyramid of the visibility. The
/// triple (left, bottom, z_near)  defines point on the near clipping plane
/// (plane perpendicular to z-axis), the triple (right, top, z_far) defines
/// point on the far clipping plane.
///
/// Method modifies this matrix to orthographic projection matrix. This matrix
/// provides scaling and translation of the truncated pyramid to canonical view
/// volume CVV - cube [-1;+1].
///
/// Created matrix corresponding to the following transformations:
///
/// - Translation:
///   - -(right + left) / 2 along x-axis;
///   - -(top + bottom) / 2 along y-axis;
///   - -(z_far + z_near) / 2 along z-axis.
/// - Scaling with scale factors:
///   - 2 / (right - left)  along X axis;
///   - 2 / (top - bottom)  along Y axis;
///   - -2 / (z_far - z_near)  along Z axis.
///
/// There are direct calculation formulas:
/// - a(0, 0) = 2 / (right - left);
/// - a(1, 1) = 2 / (top - bottom);
/// - a(2, 2) = -2 / (z_far - z_near);
/// - a(3, 0) = -(right + left) / (right - left);
/// - a(3, 1) = -(top + bottom) / (top - bottom);
/// - a(3, 2) = -(z_far + z_near) / (z_far - z_near);
/// - a(3, 3) = 1;<BR>
/// the rest elements are zero.
///
/// @note This method is similar with function glOrtho() of the OpenGL.
///
/// @param left   - coordinate of the left clipping plane.
/// @param right  - coordinate of the right clipping plane.
/// @param bottom - coordinate of the bottom clipping plane.
/// @param top    - coordinate of the top clipping plane.
/// @param z_near - distance from observer to near plane along Z axis.
/// @param z_far  - distance from viewer to far plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::Ortho(double left, double right, double bottom, double top,
                        double z_near, double z_far)
  {
  Assert(right != left);
  Assert(top != bottom);
  Assert(z_far > z_near);

  r0[0] = static_cast<T>(2 / (right - left));
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = static_cast<T>(2 / (top - bottom));
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = 0;
  r2[1] = 0;
  r2[2] = static_cast<T>(-2 / (z_far - z_near));
  r2[3] = 0;

  r3[0] = static_cast<T>(-(right + left) / (right - left));
  r3[1] = static_cast<T>(-(top + bottom) / (top - bottom));
  r3[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  r3[3] = 1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Orthographic projection matrix for far clipping plane at the infinity
///
///
/// @param left   - coordinate of the left clipping plane.
/// @param right  - coordinate of the right clipping plane.
/// @param bottom - coordinate of the bottom clipping plane.
/// @param top    - coordinate of the top clipping plane.
/// @param z_near - distance from observer to near plane along Z axis.
/// @return None (this matrix is modified).
///
template <class T>
void TMatrix4<T>::InfiniteOrtho(double left, double right,
                                double bottom, double top,
                                double z_near, double z_far)
  {
  Assert(right != left);
  Assert(top != bottom);

  r0[0] = static_cast<T>(2 / (right - left));
  r0[1] = 0;
  r0[2] = 0;
  r0[3] = 0;

  r1[0] = 0;
  r1[1] = static_cast<T>(2 / (top - bottom));
  r1[2] = 0;
  r1[3] = 0;

  r2[0] = 0;
  r2[1] = 0;
  //r2[2] = static_cast<T>(-2 / (z_far - z_near));
  // Approximation for z_far >> z_near
  //r2[2] = static_cast<T>(-2 * MathF::TOLERANCE);
  r2[2] = static_cast<T>(-2 / (z_far * 100 - z_near));

  r2[3] = 0;

  r3[0] = static_cast<T>(-(right + left) / (right - left));
  r3[1] = static_cast<T>(-(top + bottom) / (top - bottom));
  //r3[2] = static_cast<T>(-(z_far + z_near) / (z_far - z_near));
  // Approximation for z_far >> z_near
  //r3[2] = -static_cast<T>(1 + MathF::TOLERANCE * 2);
  r3[2] = static_cast<T>(-(z_far * 100 + z_near) / (z_far * 100 - z_near));
  r3[3] = 1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Apply frustum to point.
/// @param point 3D point.
/// @return true - if there is point after transformation, false - if point
/// infinite.
///
template <class T>
TVect4<T> TMatrix4<T>::PointTransform(const TPoint3<T> &point) const
  {
  TVect4<T> v(point.Vector());
  return v * (*this);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check inclusion point to frustum.
/// @note This matrix is treated as transformation frustum to CVV (cube [-1,1]
/// along all axes.
/// @param point Tested point.
/// @return true - if point after transformation inside of the CVV,
/// false - otherwise.
///
template <class T>
bool TMatrix4<T>::Includes(const TPoint3<T> &point) const
  {
  TVect4<T> v = this->PointTransform(point);
  if (v.w + v.x >= 0 && v.w - v.x >= 0 &&
      v.w + v.y >= 0 && v.w - v.y >= 0 &&
      v.w + v.z >= 0 && v.w - v.z >= 0)
    return true;

  return false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check intersection bbox with frustum.
/// @param bbox Checked box.
/// @return true - checked box intersected with CVV after transformation by
/// this frustum matrix, false - othervise.
/// @note if part of the points is visible and other part is not visible then
/// result is not defined. Method returns true;
///
template <class T>
bool TMatrix4<T>::Visible(const BBox3<T> &bbox) const
  {
  TPoint3<T> p[8];
  p[0] = TPoint3<T>(bbox.vmin.x, bbox.vmin.y, bbox.vmin.z);
  p[1] = TPoint3<T>(bbox.vmax.x, bbox.vmin.y, bbox.vmin.z);
  p[2] = TPoint3<T>(bbox.vmin.x, bbox.vmax.y, bbox.vmin.z);
  p[3] = TPoint3<T>(bbox.vmax.x, bbox.vmax.y, bbox.vmin.z);
  p[4] = TPoint3<T>(bbox.vmin.x, bbox.vmin.y, bbox.vmax.z);
  p[5] = TPoint3<T>(bbox.vmax.x, bbox.vmin.y, bbox.vmax.z);
  p[6] = TPoint3<T>(bbox.vmin.x, bbox.vmax.y, bbox.vmax.z);
  p[7] = TPoint3<T>(bbox.vmax.x, bbox.vmax.y, bbox.vmax.z);

  int n_vis = 0;
  BBox3<T> transf_bbox;
  for (int i = 0; i < 8; i++)
    {
    // Transformed points in 4D
    TVect4<T> v = this->PointTransform(p[i]);
    if (v.w > 0) // point visible
      {
      p[i] = TPoint3<T>(v.x / v.w, v.y / v.w, v.z / v.w);
      if (n_vis > 0)
        {
        transf_bbox.Include(p[i]);
        }
      else
        {
        transf_bbox.vmin = p[i];
        transf_bbox.vmax = p[i];
        }
      n_vis++;
      }
    }

  if (n_vis == 0) // full hidden
    return false;
  if (n_vis == 8) // full visible
#if 0
    return BBox3<T>::CVV().Intersects(transf_bbox);
#else
    {
//    BBox3<T> cvv(TPoint3<T>((T)(-10), (T)(-10), (T)(-1)),
//      TPoint3<T>((T)(+10), (T)(+10), (T)(+1)));
    BBox3<T> cvv(TPoint3<T>((T)(-5), (T)(-5), (T)(-1)),
      TPoint3<T>((T)(+5), (T)(+5), Math<T>::MAX_VALUE));
    return cvv.Intersects(transf_bbox);
    }
#endif
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of matrix are valid floats.
///
template <class T>
bool TMatrix4<T>::IsOK() const
  {
  return (r0.IsOK() && r1.IsOK() && r2.IsOK() && r3.IsOK());
  }

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(TMatrix4, double)
LIB_TEMPLATE_INSTANCE(TMatrix4, float)

INTEGRA_NAMESPACE_END
