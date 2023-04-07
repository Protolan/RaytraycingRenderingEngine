/// @file
/// @brief  Describes uni- and bi- variate tabulated function on mesh of
///         arbitrary type, equiped with math operations
///
/// @author Sergey Ershov, KLBM, '03.04.25.
///
/// Copyright &copy; INTEGRA, Inc., 2003.

#ifndef _KLBM_MTTABFUNC_HPP
#define _KLBM_MTTABFUNC_HPP

#include <base/arrays.hpp>
#include <base/matrix.hpp>

#include "mesh.hpp"
#include "meshcell.hpp"
#include "ttabfunc.hpp"

#ifdef T
#undef T
#endif

#ifdef X
#undef X
#endif

#ifdef Y
#undef Y
#endif

#ifdef Z
#undef Z
#endif

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
/// @class MTabFunc1D math/mtabfunc.hpp
///
/// @brief Typified univariate tabulated function f(x), equiped with some math
///        operations. The x is represented by "double" and "f" takes value
///        of any type T which supports basic arithmetic used
///        for interpolation and also supports "<" etc
///
///        The tabulated function is thus represented by mesh {x[i]}
///        and function values at its knots {f(x[i])}
///        Interpolation is linear between mesh knots
///
///        This class is optimised for performance and does not use any virtual
///        functions. The best performance is achieved with uniform mesh.
///
/// @param T      - type of value of function f(x)
///        MESH1D - mesh in "x"; it can be ANY of 1D meshes, e.g.
///                 uniform or indexed or general (based on arrays
///                 and binary search)
///
/// @todo      Interpolation procedure uses binary "+" etc; it also
///            returns function VALUE and thus creates and copies
///            TEMPORARY objects. So despite it is possible to use
///            this class for any type, it will not be very efficient
///            if "T" is dynamic array, or matrix or any other large
///            object. This class is best for "T" like double, Vect3d,
///            and so on.
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH1D> class MTabFunc1D :
                                                 public TTabFunc1D<T, MESH1D>
  {
  public:
    /// Copies source object into <B>*this</B>
    MTabFunc1D& operator = (const MTabFunc1D &from)
      {
      TTabFunc1D<T, MESH1D>::operator=(from);
      return *this;
      }

    /// @brief Makes function constant so that f(x)=value for any "x";
    /// removes data table from memory
    MTabFunc1D& operator = (const T &value)
      {
      TTabFunc1D<T, MESH1D>::operator=(value);
      return *this;
      }

    /// finds smallest value of the function on the mesh
    inline T Min() const;

    /// finds greatest value of the function on the mesh
    inline T Max() const;

    /// finds both bounds in one call; works faster
    inline void MinMax(T &low, T &upper) const;
  }; // end class 'MTabFunc1D()'

//////////////////////////////////////////////////////////////////////////////
/// @class MTabFunc2D math/mtabfunc.hpp
///
/// @brief Typified bivariate tabulated function f(x,y) equiped with math
///        operations. The x and y are represented by "double" and "f" takes
///        value of any type @b T which supports basic arithmetic used for
///        interpolation and also supports "<" etc
///
///        The tabulated function is thus represented by RECTANGULAR mesh
///        {x[i],y[j]} and function values at its knots {f(x[i],y[j])  }
///        Interpolation is bi-linear inside mesh cell
///
///        This class is optimised for performance and does not use any virtual
///        functions. The best performance is achieved with uniform mesh.
///
/// @param T      - type of value of function f(x)
///        MESH2D - mesh in "(x,y)"; it can be ANY of 2D meshes, namely
///                 Mesh2D<XMESH1D, YMESH1D> where meshes in "x"
///                 and "y" are independent and may be of any allowed
///                 type: uniform or indexed or general (based on arrays
///                 and binary search)
///
/// @todo      Interpolation procedure uses binary "+" etc; it also
///            returns function VALUE and thus creates and copies
///            TEMPORARY objects. So despite it is possible to use
///            this class for any type, it will not be very efficient
///            if "T" is dynamic array, or matrix or any other large
///            object. This class is best for "T" like double, Vect3d,
///            and so on.
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH2D> class MTabFunc2D :
                                             public TTabFunc2D<T, MESH2D>
  {
  public:
    /// Copies source object into <B>*this</B>
    MTabFunc2D& operator = (const MTabFunc2D &from)
      {
      TTabFunc2D<T, MESH2D>::operator=(from);
      return *this;
      }

    /// @brief Makes function constant so that f(x,y)=value for any (x,y);
    /// removes data table from memory
    MTabFunc2D& operator = (const T &value)
      {
      TTabFunc2D<T, MESH2D>::operator=(value);
      return *this;
      }

    /// finds smallest value of the function on the mesh
    inline T Min() const;

    /// finds greatest value of the function on the mesh
    inline T Max() const;

    /// finds both bounds in one call; works faster
    inline void MinMax(T &low, T &upper) const;
  }; // end class 'TTabFunc2D'

// ---------------------------------------------------------------------------
//                       METHODS OF CLASS MTabFunc1D
// ---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// @brief Calculates and returns lower bound of function which for our linear
///        interpolation is obtained as the smallest of tabulated values
/// @return Lower bound of function on its mesh
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH1D> T
MTabFunc1D<T, MESH1D>::Min() const
  {
  if (this->IsConstant())
    return (*this)(0);

  T low = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (v < low)
      low = v;
    }
  return low;
  } // end function "Min()"


//////////////////////////////////////////////////////////////////////////////
/// @brief  Calculates and returns upper bound of function which for our linear
///         interpolation is obtained as the greatest of tabulated values
/// @return Upper bound of function on its mesh
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH1D> T
MTabFunc1D<T, MESH1D>::Max() const
  {
  if (this->IsConstant())
    return (*this)(0);

  T upper = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (upper < v)
      upper = v;
    }
  return upper;
  } // end function "Max()"


//////////////////////////////////////////////////////////////////////////////
/// @brief Calculates and returns bounds of function which for linear
///        interpolation is obtained as the smallest and greatest of
///        tabulated values
///
/// @param low   - low bound
/// @param upper - upper bound
/// @note This version works faster than two
/// successive calls to Min() and Max()
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH1D> void
MTabFunc1D<T, MESH1D>::MinMax(T& low, T &upper) const
  {
  if (this->IsConstant())
    {
    low   = (*this)(0);
    upper = (*this)(0);
    return;
    }

  low   = this->Value(0);
  upper = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (v < low)
      low = v;
    else if (upper < v)
      upper = v;
    }
  } // end function "MinMax()"

// ---------------------------------------------------------------------------
//                       METHODS OF CLASS MTabFunc2D
// ---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// @brief Calculates and returns lower bound of function which for our linear
///        interpolation is obtained as the smallest of tabulated values
/// @return Lower bound of function on its mesh
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH2D> T
MTabFunc2D<T, MESH2D>::Min() const
  {
  if (this->IsConstant())
    return (*this)(0, 0);

  // we use linearly wrapped array because access to it is faster
  T low = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (v < low)
      low = v;
    }
  return low;
  } // end function "Min()"


//////////////////////////////////////////////////////////////////////////////
/// @brief  Calculates and returns upper bound of function which for our linear
///         interpolation is obtained as the greatest of tabulated values
/// @return Upper bound of function on its mesh
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH2D> T
MTabFunc2D<T, MESH2D>::Max() const
  {
  if (this->IsConstant())
    return (*this)(0, 0);

  // we use linearly wrapped array because access to it is faster
  T upper = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (upper < v)
      upper = v;
    }
  return upper;
  } // end function "Max()"

//////////////////////////////////////////////////////////////////////////////
/// @brief Calculates and returns bounds of function which for linear
///        interpolation is obtained as the smallest and greatest of
///        tabulated values
///
/// @param low   - low bound
/// @param upper - upper bound
/// @note This version works faster than two successive calls to Min() and Max()
//////////////////////////////////////////////////////////////////////////////
template <class T, class MESH2D> void
MTabFunc2D<T, MESH2D>::MinMax(T& low, T &upper) const
  {
  if (this->IsConstant())
  {
    low   = (*this)(0, 0);
    upper = (*this)(0, 0);
    return;
    }

  // we use linearly wrapped array because access to it is faster
  low   = this->Value(0);
  upper = this->Value(0);
  for (int i = 1; i < this->Resolution(); i++)
    {
    const T &v = this->Value(i);
    if (v < low)
      low = v;
    else if (upper < v)
      upper = v;
    }
  } // end function "MinMax()"

INTEGRA_NAMESPACE_END
#endif // #ifndef _KLBM_MTTABFUNC_HPP
// END FILE
