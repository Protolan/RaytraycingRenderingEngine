/// @internal
/// @file
///
/// @brief  Methods of class for matrices 4x3 and instances for double
//          and float types
///
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///


#include <integra.hpp>

#include <base/serializer.hpp>

#include "matrix43.hpp"

INTEGRA_NAMESPACE_START

/**
@class TMatrix43 math/matrix43.hpp

Matrix 4x3 of the arbitrary type.

The template class TMatrix43<T>  is used to store 4x3 matrices of template
type: T. Matrix43f and Matrix43d are predefined instances of the template
class for matrices with float and double elements, respectively.

Usually the matrices 4x3 are used for transformation points, vectors in 3D
space by rotation, translation and scale. This class provides the correspondent
methods for creation of such matrices.

User should take into account that transformation of vector (point) corresponds
to the following order <B>Vector * Matrix43</B>. It is recommended to use
special methods for transformation:  PointTransform() and
PointBackRotTransformed(), VectorTransformed() and VectorBackRotTransformed().

TMatrix43 class provides special operator '*' for the combination of the
two 4x3 matrices. This operator is realized (it is not real multiplication) so
that combination 1_Matrix43 * 2_Matrix43 corresponds to the transformation of
vector (point) by 1_Matrix43 at first and by 2_Matrix43 at last (evidently for
the square matrices).

The user may create own instances of the template class if needed.
Type T must support assignment and usual arithmetic and comparison
operators (including comparison with zero).

Conceptually 4x3 matrix consists of 3x3 matrix - first three rows and 3D vector -
fourth row. Underlying implementation of 3x3 matrix is

@code
  private:
    TMatrix3<T> m;
    TVect3<T> v;
@endcode

3x3 matrix in the first three rows are used as otation/scale part; the last
4-th row is used as translation part of 3D transformations.

@param T - template type of the matrix elements,
*/

// -----------------------------------------------------------------------
//             Methods of definition TMatrix43 class
// -----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// Access to predefined identity matrix.
///
/// Method provides access to predefined identity matrix. Diagonal elements of
/// the rotation part (first three rows) of this matrix are 1 and the rest
/// elements are zero.
///
/// @return Reference to predefined identity matrix
template <class T>
const TMatrix43<T> &TMatrix43<T>::E()
  {
  static const TMatrix43<T> e(1.0);
  return e;
  }

//////////////////////////////////////////////////////////////////////////////
/// View matrix.
///
/// Method modifies this matrix to a view matrix to rotate and translate the
/// world given that the viewer is located at @a obs (first parameter), looking
/// into @a view (second parameter), and the up vector is @a up (third
/// parameter).
///
/// The transformation changes things around so the observer is at the origin
/// and looking down the -Z axis.
///
/// @param obs - Position of observer (viewer).
/// @param view - Direction of the view.
/// @param up - Up vector.
/// @return Reference to this matrix (modified).
template <class T>
TMatrix43<T> &TMatrix43<T>::ViewMatrix(
    const TVect3<T> &obs, const TVect3<T> &view, const TVect3<T> &up)
  {
  TVect3<T> f = view / Length(view);
  f.Negate();

  TVect3<T> s = CrossProd(up, f);
  s.Normalize();

  TVect3<T> u = CrossProd(f, s);

  m.SetCol(0, s);
  m.SetCol(1, u);
  m.SetCol(2, f);
  v.x = -(T)DotProd(obs, s);
  v.y = -(T)DotProd(obs, u);
  v.z = -(T)DotProd(obs, f);

  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transformation of the bounding box.
///
/// The all vertices of the given bounding box are transformed by this
/// transformation matrix (see method PointTransform()). Bounding box is
/// created anew by the inclusion of the all transformed vertices.
///
/// @note Linear sizes (lengths of the ribs) of the resulted box may be
/// changed with respect to initial sizes.
///
/// @param bbox - Given bounding box.
/// @return None (only given bounding box is affected)
template <class T>
void TMatrix43<T>::BBoxTransform(BBox3<T> &bbox) const
  {
  TPoint3<T> p1(bbox.vmin.x, bbox.vmin.y, bbox.vmin.z);
  TPoint3<T> p2(bbox.vmax.x, bbox.vmin.y, bbox.vmin.z);
  TPoint3<T> p3(bbox.vmin.x, bbox.vmax.y, bbox.vmin.z);
  TPoint3<T> p4(bbox.vmax.x, bbox.vmax.y, bbox.vmin.z);

  TPoint3<T> p5(bbox.vmin.x, bbox.vmin.y, bbox.vmax.z);
  TPoint3<T> p6(bbox.vmax.x, bbox.vmin.y, bbox.vmax.z);
  TPoint3<T> p7(bbox.vmin.x, bbox.vmax.y, bbox.vmax.z);
  TPoint3<T> p8(bbox.vmax.x, bbox.vmax.y, bbox.vmax.z);

  PointTransform(p1);
  PointTransform(p2);
  PointTransform(p3);
  PointTransform(p4);
  PointTransform(p5);
  PointTransform(p6);
  PointTransform(p7);
  PointTransform(p8);

  bbox.vmin = bbox.vmax = p1;
  bbox.Include(p2);
  bbox.Include(p3);
  bbox.Include(p4);
  bbox.Include(p5);
  bbox.Include(p6);
  bbox.Include(p7);
  bbox.Include(p8);
  };

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
template <class T>
void TMatrix43<T>::Value(Serializer &inout)
  {
  inout.Value(&m[0][0], 12);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the matrix using identity one as default
///
/// @param inout - Serialization medium
/// @param tag   - Name of the serialized object
template <class T>
void TMatrix43<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  TMatrix43<T> dval;
  dval.Identity();
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

  
//////////////////////////////////////////////////////////////////////////////
/// Convert given transformation matrix to separate elements of Euler Transform.
///
/// @param[in] tmatrix Rotation matrix.
/// @param[out] translation Translation vector.
/// @param[out] azimuth Azimuth angle.
/// @param[out] tilt Tilt angle.
/// @param[out] rotation Rotation angle.
/// @param[out] scale Scale vector.
template <class T>
void TMatrix43<T>::ConvertToEulerTransformValues(Vect3d &translation, 
    double &azimuth, double &tilt, double &rotation, Vect3d &scale) const
  {
  Matrix3d rm = ConvD(m);
  scale = rm.Normalize();
  double rm_det = rm.Det();
  if (rm_det < 0)
    {
    scale[0] = -scale[0];
    rm[0].Negate();
    rm_det = -rm_det;
    }
  Assert(MathF::AboutEqual(rm_det, 1.0));
  double alpha, beta, gamma;
  rm.RotationToEulerForLight(alpha, beta, gamma);
  if (Math<double>::NearZero(alpha))
    alpha = 0.0;
  if (Math<double>::NearZero(beta))
    beta = 0.0;
  if (Math<double>::NearZero(gamma))
    gamma = 0.0;
  translation = ConvD(v);
  azimuth = alpha;
  // to 0  - PI2 range
  if (azimuth < 0)
    azimuth += PI2;
  
  tilt = beta;
  rotation = gamma;
  }

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(TMatrix43, double)
LIB_TEMPLATE_INSTANCE(TMatrix43, float)

INTEGRA_NAMESPACE_END
