/// @internal
/// @file
///
/// @brief Definition of template class for 3D bounding sphere
///
/// @author Rkd - Kirill Dmitriev <BR>
///         Ovn - Vadim Novoselsky
///
/// Copyright &copy; INTEGRA, Inc., 2001.

#include <integra.hpp>

#include "bsphere3.hpp"

INTEGRA_NAMESPACE_START

/**
@class BSphere3 math/bsphere3.hpp

Simple bounding sphere.

Template class BSphere3<T>  implements simple bounding sphere determined 
by center of sphere and it's radius of type T. BSphere3f and BSphere3d are
predefined instances of the template class for float and double values, 
respectively. BSphere3<T> class has two public members: 

@code
T         radius;  // radius of the sphere
TPoint3<T> center; // center of the sphere
@endcode

The main sphere feature is expansion to include the external points and other
spheres.


@param T - some REAL type (float, double) 
 
*/

//-----------------------------------------------------------
//                    Instantiations         
//-----------------------------------------------------------

// ----------------------------------------------------------
//                   BSphere3 METHODS
// ----------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Sphere expansion for the inclusion of the given point.
///
/// If given point is internal with respect to this sphere then this sphere is
/// not changed. If given point is external with respect to this sphere 
/// (distance from point to sphere center more than sphere radius) then this 
/// sphere is extended to include the given point. Resulted sphere is @b minimal 
/// sphere containing both initial sphere and given point. 
///
/// Let given point is external with respect to this sphere. There are two 
/// points of the sphere with minimal (near point) and maximal (far point) 
/// distances to given point. Segment between far point and given point defines
/// diameter of the resulted sphere. 
///
/// @param point - Given point.
/// @return None (only this sphere is affected).
template <class T>
void BSphere3<T>::Include(const TPoint3<T> &point)
  {
  double length;
  TVect3<T> u(point, center);

  //  point is inside of sphere
  if ((length = SqrLength(u)) <= Sqr(radius))
    return;
  length = Sqrt(length);

  //  new radius of sphere
  radius = (T)(0.5 * (radius + length));

  //  new center of sphere
  center = point + u * (radius / length);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sphere expansion for the inclusion of the given sphere.
///
/// If the all points of the given sphere are internal with respect to this 
/// sphere then this sphere is not changed.
/// If given sphere has external points with respect to this sphere (sum of 
/// the distance between centers and given sphere radius more than this sphere 
/// radius) then this sphere is extended to include the given sphere fully.
/// Resulted sphere is @b minimal sphere containing both initial sphere and 
/// given sphere. 
///
/// Let the given sphere has the external points with respect to this sphere.
/// There is one point of the given sphere with maximal distance to this sphere.
/// In other side, there is one point of this sphere with maximal distance to 
/// given sphere. The segment between these points defines the diameter of 
/// the resulted sphere.
///
/// @param bsphere - Given sphere.
/// @return None (only this sphere is affected).
template <class T>
void BSphere3<T>::Include(const BSphere3<T> &bsphere)
  {
  double length;
  TVect3<T> u(center, bsphere.center);

  //  bsphere is inside of sphere
  if ((length = Length(u)) + bsphere.radius > radius)
    {
    //  sphere is inside of bsphere
    if (length + radius <= bsphere.radius)
      *this = bsphere;
    else
      {
      //  new radius of sphere
      radius = (T)(0.5 * (radius + bsphere.radius + length));

      //  new center of sphere
      center = bsphere.center + u * ((bsphere.radius - radius) / length);
      }
    }
  }

LIB_TEMPLATE_INSTANCE(BSphere3, float)
LIB_TEMPLATE_INSTANCE(BSphere3, double)

INTEGRA_NAMESPACE_END
