/// @internal
/// @file
///
/// @brief  Definition of tolerance from Math class
///
/// The file is intended to provide commonly used mathematical functions and
/// constants.
///
/// @author Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov<BR>
///         Pnd - Nikolay Derjabin<BR>
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///

#include <integra.hpp>

#include <base/serializer.hpp>

#include "math.hpp"

INTEGRA_NAMESPACE_START

//-----------------------------------------------------------
//                 STATIC VARIABLES
//-----------------------------------------------------------

template<> const double Math<float>::MAX_VALUE = 3.40e+38;
template<> const double Math<double>::MAX_VALUE = 1.79e+308;
template<> const double Math<int>::MAX_VALUE = 2147483647;
template<> const double Math<WORD>::MAX_VALUE = 65535;

template<> const double Math<float>::MIN_VALUE = 1.18e-38;
template<> const double Math<double>::MIN_VALUE = 2.26e-308;
template<> const double Math<int>::MIN_VALUE = 1;

template<> const double Math<float>:: EPS_VALUE = 1.192092896e-7;
template<> const double Math<double>::EPS_VALUE = 2.22044604925031300e-016;
template<> const double Math<int>::EPS_VALUE = 0;
template<> const double Math<WORD>:: EPS_VALUE = 0;
template<> const double Math<DWORD>:: EPS_VALUE = 0;

template<> const double Math<float>::TOLERANCE  = 1.0e-05f;
template<> const double Math<double>::TOLERANCE = MathF::EPS_VALUE;
template<> const double Math<int>::TOLERANCE = 0;

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

// MathF has been already implicitly instantiated in math.hpp
// LIB_TEMPLATE_INSTANCE(Math, float)
LIB_TEMPLATE_INSTANCE(Math, double)

LIB_TEMPLATE_INSTANCE(BBox1, short)
LIB_TEMPLATE_INSTANCE(BBox1, int)
LIB_TEMPLATE_INSTANCE(BBox1, float)
LIB_TEMPLATE_INSTANCE(BBox1, double)

/**

@class Math math/math.hpp

Comparison of float / double values with a tolerance.

This template class is used for precision related staff. MathF and MathD
are predefined instances of the template class  for float and double
values, respectively.

The all methods of the Math class may be divided on the two parts:
- Methods of "About" family: AboutZero(), AboutEqual(), SignAbout(). These
methods provide comparisons with arbitrary tolerance or with predefined
tolerance Math<T>::TOLERANCE. These methods may be used in mathematical
calculations where high precision is necessary. In this case precision is
dependent on type of the used variables.
- Methods of "Near" family: NearZero(), NearEqual(), SignNear(). These methods
provide comparisons with predefined tolerance ::EPSILON. These methods may be
used in geometrical calculations where numerical values may be treated as
coordinates of the points on the number line. In this case precision is not
dependent on type of the used variables.

The full set of the predefined constants of the Math class is presented in
following table.

<TABLE>
<TR>
<TD><CENTER>Constant</TD>
<TD>double</TD>
<TD>float</TD>
<TD>int</TD>
<TD>WORD</TD>
<TD>DWORD</CENTER></TD>
</TR>

<TR>
<TD><CENTER>MAX_VALUE</TD>
<TD>1.79e+308</TD>
<TD>3.40e+38</TD>
<TD>2147483647</TD>
<TD>-</TD>
<TD>-</CENTER></TD>
</TR>

<TR>
<TD><CENTER>MIN_VALUE</TD>
<TD>2.26e-308</TD>
<TD>1.18e-38</TD>
<TD>1</TD>
<TD>-</TD>
<TD>-</CENTER></TD>
</TR>

<TR>
<TD><CENTER>EPS_VALUE</TD>
<TD>2.22044604925031300e-016</TD>
<TD>1.192092896e-7</TD>
<TD>-</TD>
<TD>0</TD>
<TD>0</CENTER></TD>
</TR>

<TR>
<TD><CENTER>TOLERANCE</TD>
<TD>1.192092896e-7</TD>
<TD>1.0E-05f</TD>
<TD>-</TD>
<TD>-</TD>
<TD>-</CENTER></TD>
</TR>

</TABLE>


@param T - some numerical type - float or double in most cases. Usage integer
type T is possible but not reasonable since integer values allow strict
comparisons.

*/

/**
@class BBox1 math/math.hpp

Simple bounding box in 1D space.

Template class BBox1<T> implements simple bounding box determined by minimal
and maximal points having coordinates of type T. In simplest case (type T
is float or double) it is numerical range.

BBox1i, BBox1f, BBox1d are predefined instances of the template classes for
integer, float, double values, respectively. BBox1 classes have two public
variables:

@code
T vmin, vmax;
@endcode

In geometric sense it is closed segment on the number line. This segment is
defined by the two opposite points:
- vmin - 1D point defines lower boundary of the box. Coordinate of this point
is the minimal value for the coordinates of the internal points of the box.
- vmax - 1D point defines upper boundary of the box. Coordinate of this point
is the maximal value for the coordinates of the internal points of the box.

@image html bbox_1d.gif

The main features of the bounding boxes are:
- Check of the inclusion of the points or bounding boxes;
- Extension for the inclusion points or bounding boxes;
- Intersections with other boxes.

The boundary points of the bounding box are treated as @b internal points of
the box in the all class methods.

@param T - element type such as int, float, double.
*/

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
template <class T>
void BBox1<T>::Value(Serializer &inout)
  {
  inout.Value(vmin);
  inout.Value(vmax);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the box (no default)
///
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
template <class T>
void BBox1<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  Value(inout);
  inout.EndChunk();
  }


INTEGRA_NAMESPACE_END
