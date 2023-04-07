/// @internal
/// @file
///
/// @brief  Methods of class for 4D vectors and instances for 
///         integer, float and double types
///
/// @author Mvr - Radostev Vasiliy<BR>
///         Piv - Valiev Ildar
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///

#include <integra.hpp>

#include <base/serializer.hpp>

#include "matrix4.hpp"
#include "matrix43.hpp"
#include "vect4.hpp"

INTEGRA_NAMESPACE_START

/**
@class BVect4 math/vect4.hpp

Class BVect4 is set of the four elements of arbitrary type. Arithmetic 
operations are not provided by this class.

Template class BVect4<T> implements simplest 4D vectors with elements of 
type T. This class is similar to the class TVect4 but have no elements 
arithmetic. Set4b, Set4s, Set4i, Set4u and Set4f are predefined instances 
of the template class for vectors with byte, short, int, unsigned and float
elements, respectively.

Conceptually 4D vector consists of four elements: x,  y, z and w. 
Underlying implementation of 4D vector is
@code
public:
  T x, y, z, w;
@endcode
i.e. user has possibility of direct access to the vector elements.

The user may create own instances of the template class if needed. 
Type T must support assignment and comparison operators (including 
comparison with zero).

@param T - template type of set elements, 
           must support '=' and '==' operators
*/

/**
@class TVect4 math/vect4.hpp

4D vector of arbitrary type. 

Template class TVect4<T> implements 4D vectors with elements of type T. 
Vect4s (Color4s), Vect4i (Color4i), Vect4f (Color4f, RGBAColor), Vect4d 
(Color4d), Colow4w are predefined instances of the template class for vectors 
with short, integer, float, double and WORD elements, respectively.

Conceptually 4D vector consists of four elements: x,  y, z and w. 
Underlying implementation of 4D vector is
@code
public:
  T x, y, z, w;
@endcode
i.e. user has possibility of direct access to the vector elements.

The user may create own instances of the template class if needed. Type 
T must support assignment and usual arithmetic and comparison operators 
(including comparison with zero).

<B>Efficiency consideration:</B>
- Full efficiency of vector operations is achieved only if full 
optimization is used.
- For optimization, it is recommended to use methods, working with 
references instead of methods, accepting (or returning) parameters by 
value. 

For example, if @a a and @a b are vectors, it is better to use
@code
a += b;     // faster operation (no copying is performed)
@endcode
instead of
@code
a = a + b; // slower operation (result of 'a + b' is copied into 'a')
@endcode

@attention In the some functions and methods (TVect4<T>::Project(), 
TVect4<T>::Normalize() and etc.) of this class the first three coordinates of 
4D vector are used only.

@param T - template type of vector elements, 
           (some SIGNED integral type, or a real type)
*/

// -----------------------------------------------------------------------
// Method definitions of TVect4 class
// -----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the vector serialization (as array of the three elements)
/// @param inout - Serialization medium
template <class T>
void TVect4<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 4);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the vector using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - vector by the default
template <class T>
void TVect4<T>::Serialize(Serializer &inout, const char *tag, const TVect4<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that all elements of vector are valid floats.
template <class T>
bool TVect4<T>::IsOK() const
  {
  return (FloatIsOK(x) && FloatIsOK(y) && FloatIsOK(z) && FloatIsOK(w));
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this vector by the given matrix.
///
/// Method creates a new 4D vector. Elements of a new vector are the Dot 
/// products (see function DotProd()) of the columns of given matrix and this 
/// vector cf. vector is multiplied by the matrix from the right. 
///
/// @param v - Given matrix (multiplier).
/// @return New 4D vector (product).
///
template <class T>
TVect4<T> TVect4<T>::operator * (const TMatrix4<T> &v) const
  {
  TVect4<T> res;
  res[0] = (T)DotProd4(*this, v.GetCol(0));
  res[1] = (T)DotProd4(*this, v.GetCol(1));
  res[2] = (T)DotProd4(*this, v.GetCol(2));
  res[3] = (T)DotProd4(*this, v.GetCol(3));
  return res;
  }

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(TVect4, int)
LIB_TEMPLATE_INSTANCE(TVect4, float)
LIB_TEMPLATE_INSTANCE(TVect4, double)

INTEGRA_NAMESPACE_END
