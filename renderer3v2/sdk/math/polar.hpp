/// @file
///
/// @brief Conversions between cartesian (normalized) and polar
/// (theta,phi in radians) coordimate systems.
///
/// Cartesian coordinate system uses right triple of coordinates:
/// - X axis is directed to right;
/// - Y axis is directed from user;
/// - Z axis is directed to top.
///
/// Polar coordinates theta (range is [0, PI]) and phi (range is [-PI, PI])
/// connect with Cartesian coordinates by the following scheme:
/// - (0,     any) in polar corresponds to (0,  0, -1) in Cartesian
/// - (PI/2,    0) in polar corresponds to (1,  0,  0) in Cartesian
/// - (PI/2, PI/2) in polar corresponds to (0, -1,  0) in Cartesian
///
/// @image html polar.gif
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pnd - Nikolay Derjabin<BR>
///         Fgk - Gennady Kireyko<BR>
///         Fvg - Vjacheslav Glukhov

#ifndef _KLBC_POLAR_HPP_
#define _KLBC_POLAR_HPP_

#include "math.hpp"
#include "vect2.hpp"
#include "vect3.hpp"

INTEGRA_NAMESPACE_START

///////////////////////////////////////////////////////////////////////////////
/// Conversions from polar (theta, phi in radians) coordinates
/// to cartesian. Ranges of polar coordinates are not checked.
///
/// @param[in] polar - 2D vector in polar coordinates.
/// @param[out] cart  - 3D vector in cartesian coordinates.
inline void PolarToCartesian(const Vect2d &polar, Vect3d &cart)
  {
  cart.z   = -Cos(polar.x);
  double d = Sqrt(1.0 - cart.z * cart.z);
  cart.x   = Cos(polar.y) * d;
  cart.y   = -Sin(polar.y) * d;
  }

///////////////////////////////////////////////////////////////////////////////
/// Converts cartesian coordinates (normalized) to polar.
/// Normalization of the cartesian vector checked in debug version only.
///
/// @param[in] cart - 3D vector in cartesian coordinates.
/// @param[out] polar - 2D vector in polar coordinates.
///                     polar.x is in the range   0 to PI
///                     polar.y is in the range -PI to PI
inline void CartesianToPolar(const Vect3d &cart, Vect2d &polar)
   {
   Assert(cart.Normalized());
   polar.x = ACos(-cart.z);

   if (MathD::AboutZero(polar.x))
     {
     polar.x = 0.;
     polar.y = 0;
     return;
     }

   if (MathD::AboutEqual(polar.x, PI))
     {
     polar.x = PI;
     polar.y = 0;
     return;
     }

   polar.y = ATan2(-cart.y, cart.x);
   }


///////////////////////////////////////////////////////////////////////////////
/// Barycentric coordinates of the point in triangle. 
/// This version calculates one negative bary coord for point out of triangle.
///
/// Triangle is defined by the given three points - the first three parameters
/// of the method. Let (p1, p2, p3) is triangle defined by the vertices
/// p1, p2, p3 and @b p is arbitrary point. Barycentric coordinates of the
/// point @b p are the areas of the triangles (p, p2, p3), (p, p3, p1),
/// (p, p1, p2) normalized by the area of the triangle (p1, p2, p3).
/// Instead of areas of triangles - DotProds of CrossProds are used.
///
/// @image html bary_coord.gif
///
/// @param[in] p1 - First 2D point (first vertex of the triangle).
/// @param[in] p2 - Second 2D point (second vertex of the triangle).
/// @param[in] p3 - Third  2D point (third vertex of the triangle).
/// @param[in] p  - Given point
/// @return  Vector of the barycentric coordinates of given point.
template <class T>
inline Vect3d BaryCoords(const TPoint2<T> &p1, const TPoint2<T> &p2,
                         const TPoint2<T> &p3, const TPoint2<T> &p)
  {
  double cp0 = CrossProd(p2 - p1, p3 - p1); // Whole triangle.
  double cp1 = CrossProd(p2 - p, p3 - p); // Triangle opposite to p1.
  double cp2 = CrossProd(p3 - p, p1 - p); // Triangle opposite to p2.
  double cp3 = CrossProd(p1 - p, p2 - p); // Triangle opposite to p3.

  Vect3d bary(cp1 / cp0, cp2 / cp0, cp3 / cp0);

  return bary;
  }

///////////////////////////////////////////////////////////////////////////////
/// Project point to triangle sides, if it is out of triangle.
/// @param[in] p1 - First 2D point (first vertex of the triangle).
/// @param[in] p2 - Second 2D point (second vertex of the triangle).
/// @param[in] p3 - Third  2D point (third vertex of the triangle).
/// @param[in,out] p  - Given point.
/// @return Is point changed or not.
template <class T>
inline bool ProjectPointToTriangle(const TPoint2<T> &p1, const TPoint2<T> &p2,
                                   const TPoint2<T> &p3, TPoint2<T> &p)
  {
  double cp0 = CrossProd(p2 - p1, p3 - p1); // Whole triangle.
  bool projected = false;

  double cp1 = CrossProd(p2 - p, p3 - p); // Triangle opposite to p1.
  if (cp1 / cp0 < 0)
    {
    double r = DotProd(p3 - p2, p - p2) / DotProd(p3 - p2, p3 - p2);
    Clip(r, MathF::TOLERANCE, 1 - MathF::TOLERANCE); // If out of triangle side  to closest vertex.
    p = p2 + (p3 - p2) * r; // To fight digital inaccuracy.
    p = p + (p1 - p) * MathF::TOLERANCE;
    projected = true;
    }

  double cp2 = CrossProd(p3 - p, p1 - p); // Triangle opposite to p2.
  if (cp2 / cp0 < 0)
    {
    double r = DotProd(p1 - p3, p - p3) / DotProd(p1 - p3, p1 - p3);
    Clip(r, MathF::TOLERANCE, 1 - MathF::TOLERANCE);
    p = p3 + (p1 - p3) * r;
    p = p + (p2 - p) * MathF::TOLERANCE;
    projected = true;
    }

  double cp3 = CrossProd(p1 - p, p2 - p); // Triangle opposite to p3.
  if (cp3 / cp0 < 0)
    {
    double r = DotProd(p2 - p1, p - p1) / DotProd(p2 - p1, p2 - p1);
    Clip(r, MathF::TOLERANCE, 1 - MathF::TOLERANCE);
    p = p1 + (p2 - p1) * r;
    p = p + (p3 - p) * MathF::TOLERANCE;
    projected = true;
    }
  
  return projected;
  }

INTEGRA_NAMESPACE_END
#endif   // #ifndef _KLBC_POLAR_HPP_

