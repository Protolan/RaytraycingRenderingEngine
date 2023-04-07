/// @file
///
/// @brief Definition of template class for 3D bounding sphere
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Rkd - Kirill Dmitriev <BR>
///         Ovn - Vadim Novoselsky

#ifndef _RKD_BSPHERE3_HPP_
#define _RKD_BSPHERE3_HPP_

#include "math.hpp"
#include "vect3.hpp"

INTEGRA_NAMESPACE_START

/// Simple bounding sphere.
template <class T>
class BSphere3
  {
  public:

    /// @name  Constructors
    //@{
    
    /// Default constructor.
    inline BSphere3();
    
    /// Copy constructor.
    inline BSphere3(const BSphere3 &bsphere);
    
    /// Constructor from center sphere and radius.
    inline BSphere3(const TPoint3<T> &center, const double radius = 0);
    
    /// Construct sphere, bounding given box.
    inline BSphere3(const BBox3<T> &bbox);
    
    //@}

    /// @name Sphere expansion.
    //@{

    /// Sphere expansion for the inclusion of the given point.
    void Include(const TPoint3<T> &point);
    
    /// Sphere expansion for the inclusion of the given sphere.
    void Include(const BSphere3<T> &bsphere);
    
    //@}

  public:
    /// Radius of the sphere
    T          radius;
    /// Center of the sphere
    TPoint3<T> center;
  };

//  predefined types

/// Predefined instance of bounding sphere with float components.
typedef BSphere3<float>  BSphere3f;

/// Predefined instance of bounding sphere with double components.
typedef BSphere3<double> BSphere3d;

///////////////////////////////////////////////////////////////////////////////
/// Convert given sphere to the same sphere having float element type.
/// @param u - Given sphere with elements of the orbitrary type.
/// @return  Sphere with float elements.
template <class T>
inline BSphere3f ConvF(const BSphere3<T> &u)
  {
  return BSphere3f(ConvF(u.center), u.radius);
  }

///////////////////////////////////////////////////////////////////////////////
/// Convert given sphere to the same sphere having double element type.
/// @param u - Given sphere with elements of the orbitrary type.
/// @return  Sphere with double elements.
template <class T>
inline BSphere3d ConvD(const BSphere3<T> &u)
  {
  return BSphere3d(ConvD(u.center), u.radius);
  }

// -----------------------------------------------------------------
//                Definition of BSphere3 methods
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Constructs sphere without parameters initialization.
template <class T>
BSphere3<T>::BSphere3()
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// Constructs sphere from given sphere.
/// @param bsphere - Given sphere.
template <class T>
BSphere3<T>::BSphere3(const BSphere3 &bsphere)
  {
  center = bsphere.center; radius = bsphere.radius;
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from center sphere and radius.
///
/// Center and radius members are initialized by defined values.
/// @param center - 3D point of the sphere center
/// @param radius - Value of the sphere radius
template <class T>
BSphere3<T>::BSphere3(const TPoint3<T> &center, const double radius)
  {
  this->center = center;
  this->radius = (T)radius;
  }

///////////////////////////////////////////////////////////////////////////////
/// Construct sphere, bounding given box.
///
/// Sphere, fully bounding given box is constructed. Center sphere defined by 
/// the center of given box. Radius sphere is half of the box diagonal length.
/// @param bbox - Given box
template <class T>
BSphere3<T>::BSphere3(const BBox3<T> &bbox)
  {
  center = bbox.Center();
  radius = (T)(0.5 * Length(bbox.Diag()));
  }

INTEGRA_NAMESPACE_END
#endif
