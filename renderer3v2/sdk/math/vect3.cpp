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

#include "matrix3.hpp"
#include "matrix43.hpp"
#include "vect3.hpp"

INTEGRA_NAMESPACE_START

/// CIE sum vector, set during setting input CIEConv for ColorModel. Default value - for Barco monitor.
double CIE_SUM_VECT[3] = {0.25100031242007470, 0.66467979422891876, 0.084319893351006162};

/**
@class BVect3 math/vect3.hpp

Class BVect3<T> is set of the three elements of arbitrary type. Arithmetic
operations are not provided by this class.

Template class BVect3<T> implements simplest 3D vectors with elements of
type T. This class is similar to the class TVect3 but have no elements
arithmetic. Set3b, Set3s, Set3i, Set3u and Set3f are predefined instances of
the template class for vectors with byte, short, int, unsigned and float
elements, respectively.

Conceptually 3D vector consists of three elements: x,  y and z. Underlying
implementation of 3D vector is

@code
public:
  T x, y, z;
@endcode

i.e. user has possibility of direct access to the vector elements.

The user may create own instances of the template class if needed.
Type T must support assignment and comparison operators (including
comparison with zero).

@param T - template type of set elements,
           must support '=' and '==' operators
*/

// -------------------------------------------------------------
// BVect3<T> methods
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the vector serialization (as array of the three elements)
/// @param inout - Serialization medium
template <class T>
void BVect3<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 3);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the vector using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - vector by the default
template <class T>
void BVect3<T>::Serialize(Serializer &inout, const char *tag, const BVect3<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

/**
@class TVect3 math/vect3.hpp

3D vector of arbitrary type.

Template class TVect3<T> implements 3D vectors with elements of type T.
Vect3s (Color3s), Vect3i (Color3i), Vect3f (Color3f, RGBColor), Vect3d (Color3d)
and Color3w are predefined instances of the template class for vectors with
short, integer, float, double and WORD elements, respectively.

Conceptually 3D vector consists of three elements: x,  y and z.
Underlying implementation of 3D vector is
@code
public:
  T x, y, z;
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

@param T - template type of vector elements,
           (some SIGNED integral type, or a real type)
*/

// -----------------------------------------------------------------------
// Method definitions of TVect3 class
// -----------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the vector serialization (as array of the three elements)
/// @param inout - Serialization medium
template <class T>
void TVect3<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 3);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the vector using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - Vector by the default
template <class T>
void TVect3<T>::Serialize(Serializer &inout, const char *tag, const TVect3<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

///////////////////////////////////////////////////////////////////////////////
/// Create any unit orthogonal vector to given vector.
///
/// Dot product (see function DotProd(const TVect3<T> &a, const TVect3<T> &b))
/// between orthogonal vectors is zero. Let {a, b, c} vector coordinates. From
/// equation ax + by + cz = 0, c != 0, possible orthogonal vector
/// {c, c, -(a + b)} will be proper after normalization.
///
/// @note Zero vectors are not allowed, DEBUG version ASSERTs it.
/// @return 3D unit vector, orthogonal to this vector.
template <class T>
TVect3<T> TVect3<T>::AnyOrthogonal() const
  {
  int ind = MaxElementIndex();

  T sum = 0;
  int i;
  for (i = 0; i < 3; i++)
    {
    if (i != ind)
      sum += (*this)[i];
    }

  T cmax = (*this)[ind];

  TVect3<T> res;
  for (i = 0; i < 3; i++)
    {
    if (i == ind)
      res[i] = -sum;
    else
      res[i] = cmax;
    }

  return res.Normalize();
  }

//////////////////////////////////////////////////////////////////////////////
/// Provides axis with given index from 0 to 5 (x, y, z, -x, -y, -z).
/// @param axis_ind Index of the axis.
/// @return Vector of axis.
template <class T>
TVect3<T> TVect3<T>::Axis(int axis_ind)
  {
  switch (axis_ind)
    {
    case 0:
      return TVect3<T>(1, 0, 0);
    case 1:
      return TVect3<T>(0, 1, 0);
    case 2:
      return TVect3<T>(0, 0, 1);
    case 3:
      return TVect3<T>(-1, 0, 0);
    case 4:
      return TVect3<T>(0, -1, 0);
    case 5:
      return TVect3<T>(0, 0, -1);
    default:
      Assert(false);
      return TVect3<T>(0);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Round the vector - delete decimals lower then given level.
/// @param level Given level.
template <class T>
void TVect3<T>::Round(double level)
  {
  Assert(level != 0);
  Integra::Round(x, level);
  Integra::Round(y, level);
  Integra::Round(z, level);
  }

///////////////////////////////////////////////////////////////////////////////
/// Multiplication of this vector by the given matrix.
///
/// Method creates a new 3D vector. Elements of a new vector are the Dot 
/// products (see function DotProd()) of the columns of given matrix and this 
/// vector cf. vector is multiplied by the matrix from the right. 
///
/// @param v - Given matrix (multiplier).
/// @return New 3D vector (product).
///
template <class T>
TVect3<T> TVect3<T>::operator * (const TMatrix3<T> &v) const
  {
  TVect3<T> res;
  res[0] = (T)DotProd(*this, v.GetCol(0));
  res[1] = (T)DotProd(*this, v.GetCol(1));
  res[2] = (T)DotProd(*this, v.GetCol(2));
  return res;
  }

/**
@class TPoint3 math/vect3.hpp

3D point of arbitrary type.

Template class TPoint3<T> implements 3D points with elements of type T.
Point3i, Point3f and Point3d are predefined instances of the template class
for points with integer, float and double elements, respectively.

Conceptually 3D point consists of three elements: x, y, and z.
Underlying implementation of 3D point is
@code
public:
  T x, y, z;
@endcode
i.e. user has possibility of direct access to the point elements.

The user may create own instances of the template class if needed.
Type T must support assignment and usual arithmetic and comparison
operators (including comparison with zero).

<B>Efficiency consideration:</B>
- Full efficiency of point operations is achieved only if full
optimization is used.
- For optimization, it is recommended to use methods, working with
references instead of methods, accepting (or returning) parameters by
value.

For example, if @a a is point and @a b is vector, it is better to use
@code
a += b;     // faster operation (no copying is performed)
@endcode
instead of
@code
a = a + b; // slower operation (result of 'a + b' is copied into 'a')
@endcode

@param T - template type of the point coordinates
           (some SIGNED integral type, or a real type)
*/

// -------------------------------------------------------------
// TPoint3<T> methods
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Lower level of the point serialization (as array of the three elements)
/// @param inout - Serialization medium
template <class T>
void TPoint3<T>::Value(Serializer &inout)
  {
  inout.Value(&x, 3);
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialize the point using given default
/// @param inout - Serialization medium
/// @param tag   - Name of the chunk (parameter name) to use
/// @param dval  - Point by the default
template <class T>
void TPoint3<T>::Serialize(Serializer &inout, const char *tag, const TPoint3<T> &dval)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = dval;
  if (inout.Import() || (*this != dval))
    Value(inout);
  inout.EndChunk();
  }

/**
@class BBox3 math/vect3.hpp

Simple bounding 3D box. In fact it is rectangular parallelepiped with sides
parallel to the coordinate planes.

Template class BBox3<T> implements simple bounding box determined by
minimal and maximal points having coordinates of type T.
BBox3i, BBox3f, BBox3d are predefined instances of the template classes
for boxes in 3D of integer, float, double values respectively.
BBox3<T> class have two public variables:
@code
public:
  Point3<T> vmin, vmax;
@endcode

In geometric sense it is rectangular parallelepiped in 3D space with ribs
parallel to the coordinate axis. This rectangular parallelepiped is defined by
the two opposite points:
- @a vmin - 3D point defines lower boundary of the box. Coordinates of this point
are the minimal values for the coordinates of the internal points of the box.
- @a vmax - 3D point defines upper boundary of the box. Coordinates of this point
are the maximal values for the coordinates of the internal points of the box.

@image html bbox_3d.gif

The main features of the bounding boxes are:
- Check of the inclusion of the points or bounding boxes;
- Extension for the inclusion points or bounding boxes;
- Intersections with other boxes.

The boundary points of the bounding box are treated as internal points of the
box in the all class methods.

@param T - element type such as int, float, double
           Usage of TPoint3<T> and TVect3<T> classes is assumed
*/

// -------------------------------------------------------------
// BBox3<T> methods and related functions
// -------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Lower level serialization
///
/// @param inout - Serialization medium
template <class T>
void BBox3<T>::Value(Serializer &inout)
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
void BBox3<T>::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  Value(inout);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Cretion of the box by the transformation of this box.
///
/// Method transforms the all vertices of this box by the transformation matrix
/// (see TMatrix43 class) and creates new box which includes the all
/// transformed vertices.
///
/// @param tm - Transformation matrix 4x3.
/// @return - Bounding box for the all transformed vertices of this box.
template <class T>
BBox3<T> BBox3<T>::Transformed(const TMatrix43<T> &tm) const
  {
  // consider all 8 vertices
  TPoint3<T> p;

  p.Set(vmin.x, vmin.y, vmin.z);
  tm.PointTransform(p);
  BBox3<T> out(p);

  p.Set(vmax.x, vmin.y, vmin.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmin.x, vmax.y, vmin.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmax.x, vmax.y, vmin.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmin.x, vmin.y, vmax.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmax.x, vmin.y, vmax.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmin.x, vmax.y, vmax.z);
  tm.PointTransform(p);
  out.Include(p);

  p.Set(vmax.x, vmax.y, vmax.z);
  tm.PointTransform(p);
  out.Include(p);

  return out;
  } // Transformed()

//////////////////////////////////////////////////////////////////////////////
/// Get the vertex of box, with given index.
///
/// First 4 vertices form rectangle on vmin.z plane, next - on vmax.z plane.
/// @param ind Index of the vertex, < 8.
/// @return Coordinates of vertex. If index is out of range - center is
///         returned.
template <class T>
TPoint3<T> BBox3<T>::GetVertex(int ind) const
  {
  switch (ind)
    {
    case 0:
      return TPoint3<T>(vmin.x, vmin.y, vmin.z);
    case 1:
      return TPoint3<T>(vmax.x, vmin.y, vmin.z);
    case 2:
      return TPoint3<T>(vmax.x, vmax.y, vmin.z);
    case 3:
      return TPoint3<T>(vmin.x, vmax.y, vmin.z);
    case 4:
      return TPoint3<T>(vmin.x, vmin.y, vmax.z);
    case 5:
      return TPoint3<T>(vmax.x, vmin.y, vmax.z);
    case 6:
      return TPoint3<T>(vmax.x, vmax.y, vmax.z);
    case 7:
      return TPoint3<T>(vmin.x, vmax.y, vmax.z);
    default:
      Assert(false);
      return Center();
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication of this box to given scalar.
/// @param d Given scalar.
/// @return this box after multiplication.
template <class T>
BBox3<T> &BBox3<T>::operator *= (const double d)
  {
  vmin *= d;
  vmax *= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Division of this box by given scalar.
///
/// @param d Given scalar (a positive value).
/// @return this box after multiplication.
///
template <class T>
BBox3<T> &BBox3<T>::operator /= (const double d)
  {
  Assert(d > 0.0);
  vmin /= d;
  vmax /= d;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get CVV (canonical view volume) cube.
///
/// CVV is cube with range [-1; +1] along all coordinate axes.
///
/// @return Reference to predefined bounding box.
template <class T>
const BBox3<T> &BBox3<T>::CVV()
  {
  static const BBox3<T> cvv(TPoint3<T>((T)(-1), (T)(-1), (T)(-1)),
                            TPoint3<T>((T)(+1), (T)(+1), (T)(+1)));
  return cvv;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check that the ray intersects the given box.
///
/// Method find intersection of the ray with the box. Intersection is looking
/// for the specified distance and from specified side (inside, outside or undefined)
///
/// @param[in] point - Ray origin.
/// @param[in] direction - Ray direction.
/// @param[in,out] distance - Maximal allowed distance for search [in] and found distance [out].
/// @param[in] side - Side of the ray intersection (-1 - outside, +1 - inside, 0 - undefined).
/// @return - The search result (-1 found from outside, +1 - fround from inside, 0 - not found).
template <class T>
int BBox3<T>::Intersects(const TPoint3<T> &point, const TVect3<T> &direction, T &distance, int side) const
  {
  int x(0), y(0), z(0), xo(0), yo(0), zo(0), s(0), so(0), i;

  for (i = 0, x = 0, y = 1, z = 2; i < 3; i++, xo = x, yo = y, zo = z, x = yo, y = zo, z = xo)
    {
    T t(-1);

    if (direction[x] > 0)
      {
      if (point[x] <= vmin[x] && side <= 0)
        t = (vmin[x] - point[x]) / direction[x], s = -1;
      else if (point[x] <= vmax[x] && side >= 0)
        t = (vmax[x] - point[x]) / direction[x], s = 1;
      else
        continue;
      }
    else if (direction[x] < 0)
      {
      if (point[x] >= vmax[x] && side <= 0)
        t = (vmax[x] - point[x]) / direction[x], s = -1;
      else if (point[x] >= vmin[x] && side >= 0)
        t = (vmin[x] - point[x]) / direction[x], s = 1;
      else
        continue;
      }
    if (t >= 0 && t < distance)
      {
      T py(point[y] + t * direction[y]), pz(point[z] + t * direction[z]);

      if (py >= vmin[y] && py <= vmax[y] && pz >= vmin[z] && pz <= vmax[z])
        distance = t, so = s;
      }
    }
  return so;
  }

/**
@class Math3 math/vect3.hpp

Comparison of 3D vectors and points with a tolerance

This class is intended to provide a comparison of 3D vectors and points
with a tolerance. Math3f and Math3d are predefined instances of the
template class for float and double values, respectively.

The all methods of the Math3<T> class may be divided to the two parts:
- Methods of "About" family: AboutZero(), AboutEqual(). These methods provide
comparisons with arbitrary tolerance or with predefined tolerance
Math<T>::TOLERANCE. These methods may be used in mathematical calculations
where high precision is necessary. In this case precision is dependent on type
of the used variables.
- Methods of "Near" family: NearZero(), NearEqual(). These method provide
comparisons with predefined tolerance ::EPSILON. These methods may be used in
geometrical calculations where numerical values may be treated as coordinates
of the points/vectors in the 3D numerical space. In this case precision is not
dependent on type of the used variables.

@param T - some REAL type (float, double)
*/

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------


LIB_TEMPLATE_INSTANCE(BVect3, BYTE)
LIB_TEMPLATE_INSTANCE(BVect3, short)
LIB_TEMPLATE_INSTANCE(BVect3, int)
LIB_TEMPLATE_INSTANCE(BVect3, unsigned)

LIB_TEMPLATE_INSTANCE(TVect3, int)
LIB_TEMPLATE_INSTANCE(TVect3, float)
LIB_TEMPLATE_INSTANCE(TVect3, double)

LIB_TEMPLATE_INSTANCE(TPoint3, int)
LIB_TEMPLATE_INSTANCE(TPoint3, double)
LIB_TEMPLATE_INSTANCE(TPoint3, float)

LIB_TEMPLATE_INSTANCE(BBox3, int)
LIB_TEMPLATE_INSTANCE(BBox3, float)
LIB_TEMPLATE_INSTANCE(BBox3, double)

LIB_TEMPLATE_INSTANCE(Math3, float)
LIB_TEMPLATE_INSTANCE(Math3, double)


//////////////////////////////////////////////////////////////////////////////
/// HSV to RGB conversion.
/// @param h Hue in 0 - 1 range.
/// @param s Saturation in 0 - 1 range.
/// @param v Value in 0 - 1 range.
/// @return RGB color in 0 - 1 range.
/// @note See HSV in wikipedia.
Color3d hsv_to_rgb(double h, double s, double v)
  {
  double h6 = h * 6;
  int h_i = (int)(h6);
  double v_min = (1 - s) * v;
  double a = (v - v_min) * (h6 - h_i);
  double v_inc = v_min + a;
  double v_dec = v - a;
  Color3d rgb;
  switch (h_i)
    {
    case 0:
      rgb.x = v;
      rgb.y = v_inc; 
      rgb.z = v_min;
      break;
    case 1:
      rgb.x = v_dec;
      rgb.y = v; 
      rgb.z = v_min;
      break;
    case 2:
      rgb.x = v_min;
      rgb.y = v; 
      rgb.z = v_inc;
      break;
    case 3:
      rgb.x = v_min;
      rgb.y = v_dec; 
      rgb.z = v;
      break;
    case 4:
      rgb.x = v_inc;
      rgb.y = v_min; 
      rgb.z = v;
      break;
    case 5:
      rgb.x = v;
      rgb.y = v_min; 
      rgb.z = v_dec;
      break;
    default:
      Assert(false);
      break;
    }

  return rgb;
  }

//////////////////////////////////////////////////////////////////////////////
/// Distinct Color Generator for given color index.
/// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
/// http://threekings.tk/mirror/ryb_TR.pdf
/// @param seed Random seed to start.
/// @param col_ind Color index.
/// @param v Value in HSV, in  0 - 1 range.
/// @return Color for given index.
Set3b DistinctColorGenerator(double seed, int col_ind, double v)
  {
  static double golden_ratio_conjugate = 0.618033988749895;
  double h = seed; // use random start value

  h += golden_ratio_conjugate * col_ind;
  h = h - (int)h;
  double s = 1 - ((col_ind % 80) / 8) / 10.0;

  Color3d rgb = hsv_to_rgb(h, s, v);
  Set3b col((BYTE)(rgb.x * 255 + 0.5), (BYTE)(rgb.y * 255 + 0.5), (BYTE)(rgb.z * 255 + 0.5));
  return col;
  }


INTEGRA_NAMESPACE_END
