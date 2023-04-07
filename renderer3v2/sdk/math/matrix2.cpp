/// @internal
/// @file
///
/// @brief Methods of class for matrices 2x2 and instances for double 
///        and float types
///
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///

#include <integra.hpp>

#include "matrix2.hpp"

#include <base/serializer.hpp>

INTEGRA_NAMESPACE_START

// -----------------------------------------------------------------------
//             Methods of definition TMatrix2 class
// -----------------------------------------------------------------------

/**
@class TMatrix2 math/matrix2.hpp

Matrix 2x2 of the arbitrary type. 

Matrix2f and Matrix2d are predefined instances of the template class 
for matrices with float and double elements, respectively. 

The user may create own instances of the template class if needed. 
Type T must support assignment and usual arithmetic and comparison 
operators (including comparison with zero).

Conceptually 2x2 matrix consists of two vectors: r0 and r1. First vector is
first row of the matrix, second vector is second row of the matrix. Underlying 
implementation of 2x2 matrix is

@code
  public:
    TVect2<T> r0, r1; 
@endcode

User has possibility of direct access to the rows of the matrix.


@param T - template type of the matrix elements, 
*/

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this matrix by given matrix.
///
/// Method creates a new 2x2 matrix. Elements of the new matrix are the Dot 
/// products (see function DotProd()) of the rows of this matrix and columns of 
/// the given matrix. Let i - index of the row (i = 0, 1), j - index of the 
/// column (j = 0, 1) then element (i, j) of the new matrix is Dot product 
/// of the i-th row of this matrix and j-th column of the given matrix.
///
/// @param v - Given matrix 2x2 (multiplier).
/// @return New 2x2 matrix.
///
template <class T>
TMatrix2<T> TMatrix2<T>::operator * (const TMatrix2<T> &v) const
  {
  TMatrix2<T> res;
  res.r0[0] = r0[0] * v.r0[0] + r0[1] * v.r1[0];
  res.r0[1] = r0[0] * v.r0[1] + r0[1] * v.r1[1];
  res.r1[0] = r1[0] * v.r0[0] + r1[1] * v.r1[0];
  res.r1[1] = r1[0] * v.r0[1] + r1[1] * v.r1[1];
  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Get transposed matrix.
///
/// Method creates a new 2x2 matrix. New matrix is transposed with respect to
/// this matrix cf. rows of the new matrix are the columns of this matrix and
/// vice versa.
///
/// @return New 2x2 matrix equal to transposed this matrix.
///
template <class T>
TMatrix2<T> TMatrix2<T>::Transposed() const
  {
  TMatrix2<T> res;

  res.SetCol(0, r0);
  res.SetCol(1, r1);

  return res;
  }

///////////////////////////////////////////////////////////////////////////////
/// Normalize rows of this matrix, returns new math vector 
/// consisting of lengths of the rows before normalization.
///
/// @return 2D scale vector - length of each row (before normalization).
///
template <class T>
Vect2d TMatrix2<T>::Normalize()
  {
  Vect2d scale;

  scale.x = r0.ModNormalize();
  scale.y = r1.ModNormalize();

  return scale;
  }

///////////////////////////////////////////////////////////////////////////////
/// Matrix of rotation at a given angle.
///
/// Method modifies this matrix to matrix of rotation. Result of multiplication 
/// of this matrix with the some given vector <b> from the left </b> is vector 
/// which is rotated with respect to given vector at given angle 
/// counterclockwise.
///
/// @param angle - Rotation angle in radians.
/// @return Reference to this matrix (updated).
///
template <class T>
TMatrix2<T> &TMatrix2<T>::Rotation(double angle)
  {
  T cos_angle = (T)Cos(angle);
  T sin_angle = (T)Sin(angle);

  r0[0] = cos_angle;
  r1[0] = -sin_angle;
  r0[1] = sin_angle;
  r1[1] = cos_angle;

  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
///
template <class T>
void TMatrix2<T>::Value(Serializer &inout)
  {
  inout.Value(&r0[0], 4);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the matrix using identity one as default
///
/// @param inout - Serialization medium
/// @param tag   - Name of the serialized object
///
template <class T>
void TMatrix2<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  TMatrix2<T> dval(1.0);
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
const TMatrix2<T> &TMatrix2<T>::E()
  {
  static const TMatrix2<T> e(1.0);
  return e;
  }


//-----------------------------------------------------------
//        Instantiations         
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(TMatrix2, double)
LIB_TEMPLATE_INSTANCE(TMatrix2, float)

INTEGRA_NAMESPACE_END
