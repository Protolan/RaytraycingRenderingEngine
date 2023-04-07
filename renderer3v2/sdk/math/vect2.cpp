/// @internal
/// @file
///
/// @brief  Methods of class for 2D vectors and 
///         predefined instances for some types
///
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///

#include <integra.hpp>

#include <base/serializer.hpp>

#include "matrix2.hpp"
#include "vect2.hpp"

INTEGRA_NAMESPACE_START

/**
@class BVect2 math/vect2.hpp

Class BVect2 is set of the two elements of arbitrary type. Arithmetic 
operations are not provided by this class.

Template class BVect2<T> implements simplest 2D vectors with elements of type T.
This class is similar to the class TVect2 but have no elements arithmetic. 
Set2b, Set2s, Set2i, Set2u, Set2f and Set2d are predefined instances of the 
template class for vectors with byte, short, int, unsigned, float and double 
elements, respectively.

Conceptually 2D vector consists of two elements: x and y. Underlying 
implementation of 2D vector is

@code
public:
  T x, y;
@endcode

i.e. user has possibility of direct access to the vector elements.

The user may create own instances of the template class if needed. Type T must 
support assignment and comparison operators (including comparison with zero).

@param T - template type of set elements, 
           must support '=' and '==' operators
*/

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the vector serialization (as array of the two elements)
/// @param inout - Serialization medium
template <class T>
void BVect2<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the vector using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - Vector by the default
template <class T>
void BVect2<T>::Serialize(Serializer &inout, const char *tag, const BVect2<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }


/**
@class TVect2 math/vect2.hpp

2D vector of arbitrary type. 

Template class TVect2<T> implements 2D vectors with elements of type T. Vect2s, 
Vect2i, Vect2f and Vect2d are predefined instances of the template class for 
vectors with short, integer, float and double elements, respectively.

Conceptually 2D vector consists of two elements: x and  y. Underlying 
implementation of 2D vector is

@code
public:
  T x, y;
@endcode

i.e. user has possibility of direct access to the vector elements.

The user may create own instances of the template class if needed. Type T must 
support assignment and usual arithmetic and comparison operators (including 
comparison with zero).

<B>Efficiency consideration:</B>
- Full efficiency of vector operations is achieved only if full optimization is 
used.
- For optimization, it is recommended to use methods, working with references 
instead of methods, accepting (or returning) parameters by value. 

For example, if @a a and @a b are vectors, it is better to use:
@code
a += b;   // faster operation (no copying is performed)
@endcode
instead of
@code
a = a + b; // slower operation (result of 'a + b' is copied into 'a')
@endcode

@param T - template type of vector elements, 
           (some SIGNED integral type, or a real type)
*/

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the vector serialization (as array of the two elements)
/// @param inout - Serialization medium
template <class T>
void TVect2<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the vector using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - Vector by the default
template <class T>
void TVect2<T>::Serialize(Serializer &inout, const char *tag, const TVect2<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this vector by the given matrix.
///
/// Method creates a new 2D vector. Elements of a new vector are the Dot 
/// products (see function DotProd()) of the columns of given matrix and this 
/// vector cf. vector is multiplied by the matrix from the right. 
///
/// @param v - Given matrix (multiplier).
/// @return New 2D vector (product).
///
template <class T>
TVect2<T> TVect2<T>::operator * (const TMatrix2<T> &v) const
  {
  TVect2<T> res;
  res[0] = (T)DotProd(*this, v.GetCol(0));
  res[1] = (T)DotProd(*this, v.GetCol(1));
  return res;
  }

/**
@class TPoint2 math/vect2.hpp

2D point of arbitrary type. 

Template class TPoint2<T> implements 2D points with elements of type T. Point2i, 
Point2f, Point2d and Point2l are predefined instances of the template class for 
points with integer, float, double and long elements, respectively.

Conceptually 2D point consists of two elements: x, y. Underlying implementation 
of 2D point is

@code
public:
  T x, y;
@endcode

i.e. user has possibility of direct access to the point elements.

The user may create own instances of the template class if needed. Type T must 
support assignment and usual arithmetic and comparison operators (including 
comparison with zero).

<B>Efficiency consideration:</B>
- Full efficiency of point operations is achieved only if full optimization 
is used.
- For optimization, it is recommended to use methods, working with references 
instead of methods, accepting (or returning) parameters by value. 

For example, if @a a is point and @a b is vector, it is better to use
@code
a += b;     // faster operation (no copying is performed)
@endcode
instead of
@code
a = a + b; // slower operation (result of 'a + b' is copied into 'a')
@endcode

@param T - template type of the point coordinates  (some SIGNED integral type, 
or a real type).

*/

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the point serialization (as array of the two elements)
/// @param inout - Serialization medium
template <class T>
void TPoint2<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 2);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the point using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - Point by the default
template <class T>
void TPoint2<T>::Serialize(Serializer &inout, const char *tag, const TPoint2<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

/**
@class BBox2 math/vect2.hpp

Simple bounding 2D box. 

Template class BBox2<T> implements simple bounding box determined by minimal 
and maximal points having coordinates of type T. BBox2i, BBox2f, BBox2d are 
predefined instances of the template classes  for boxes in 2D of integer, float, 
double values respectively. BBox2<T> class have two public variables:

@code
public:
  Point2<T> vmin, vmax;
@endcode

In geometric sense it is rectangle in 2D space with sides parallel to the 
coordinate axes. This rectangle is defined by the two opposite points: left 
lower vertex and right upper vertex:  
- @a vmin - 2D point defines lower boundary of the box. Coordinates of this 
point are the minimal values for the coordinates of the internal points of the 
box.
- @a vmax - 2D point defines upper boundary of the box. Coordinates of this 
point are the maximal values for the coordinates of the internal points of the 
box.

@image html bbox_2d.gif

The main features of the bounding boxes are:
- Check of the inclusion of the points or bounding boxes;
- Extension for the inclusion points or bounding boxes;
- Intersections with other boxes.

The boundary points of the bounding box are treated as internal points of the
box in the all class methods. 

@param T - element type such as int, float, double. 

Usage of TPoint2<T> and TVect2<T> classes is assumed.

*/

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
template <class T>
void BBox2<T>::Value(Serializer &inout)
  {
  vmin.Value(inout);
  vmax.Value(inout);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the box (no default)
///
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
template <class T>
void BBox2<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  Value(inout);
  inout.EndChunk();
  }

/**
@class Math2 math/vect2.hpp

Comparison of 2D vectors and points with a tolerance

This class is intended to provide a comparison of 2D vectors and points 
with a tolerance. Math2f and Math2d are predefined instances of the 
template class for float and double values, respectively.

The all methods of the class Math2 may be divided to the two parts:
- Methods of "About" family: AboutZero(), AboutEqual(). These methods provide 
comparisons with arbitrary tolerance or with predefined tolerance 
Math<T>::TOLERANCE. These methods may be used in mathematical calculations 
where high precision is necessary. In this case precision is dependent on type 
of the used variables.
- Methods of "Near" family: NearZero(), NearEqual(). These methods provide 
comparisons with predefined tolerance ::EPSILON. These methods may be used in 
geometrical calculations where numerical values may be treated as coordinates 
of the points/vectors on the numerical plane. In this case precision is not 
dependent on type of the used variables.

@param T - some REAL type (float, double) 

*/

//-----------------------------------------------------------
//        Instantiations         
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(BVect2, BYTE)
LIB_TEMPLATE_INSTANCE(BVect2, short)
LIB_TEMPLATE_INSTANCE(BVect2, int)
LIB_TEMPLATE_INSTANCE(BVect2, unsigned)

LIB_TEMPLATE_INSTANCE(TVect2, int)
LIB_TEMPLATE_INSTANCE(TVect2, float)
LIB_TEMPLATE_INSTANCE(TVect2, double)

LIB_TEMPLATE_INSTANCE(TPoint2, int)
LIB_TEMPLATE_INSTANCE(TPoint2, double)
LIB_TEMPLATE_INSTANCE(TPoint2, float)

LIB_TEMPLATE_INSTANCE(BBox2, int)
LIB_TEMPLATE_INSTANCE(BBox2, float)
LIB_TEMPLATE_INSTANCE(BBox2, double)

LIB_TEMPLATE_INSTANCE(Math2, float)
LIB_TEMPLATE_INSTANCE(Math2, double)

INTEGRA_NAMESPACE_END
