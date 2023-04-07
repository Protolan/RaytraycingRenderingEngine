/// @file
///
/// @brief Class for Sun location calculations.
///
/// @author  Piv - Ildar Valiev, KC3H, '06.01.27
///
/// Copyright &copy; INTEGRA, Inc., 2006.

#ifndef _KC3H_SUNLOC_HPP
#define _KC3H_SUNLOC_HPP


#include <math/matrix3.hpp>
#include <math/vect2.hpp>
#include <math/vect3.hpp>



INTEGRA_NAMESPACE_START

/// Class for Sun location.
class SunLoc
  {
  public:

    /// Calculate sun direction in global CS from sun location.
    Vect3d SunLoc2Dir(const Vect2d &sun_loc) const;
    /// Get Zenith direction.
    inline const Vect3d &GetZenith() const;
    /// Get South direction.
    inline Vect3d GetSouth() const;
    /// Rotation matrix, from local CS to global.
    inline const Matrix3d &GetOrientRM() const;

    /// @name Static functions.
    //@{
    /// @brief Create orientation rotation matrix from the given
    /// zenith and south directions.
    static void CreateOrientMatrix(const Vect3d &zenith, const Vect3d &south,
                                   Matrix3d &orient);
    /// Correct south direction to be orthogonal to Zenith.
    static void CorrectSouth(const Vect3d &zenith, Vect3d &south);

    /// Calculate sun location from sun direction in global CS
    static void Dir2SunLoc(const Vect3d &sun_dir, const Matrix3d orient,
                              Vect2d &sun_loc);
    /// Calculate sun location from sun direction
    static void Dir2SunLoc(const Vect3d &sun_dir, const Vect3d &zenith,
                          const Vect3d &south, Vect2d &sun_loc);
    /// Calculate sun direction in global CS from sun location.
    static void SunLoc2Dir(const Vect2d &sun_loc,
                  const Vect3d &zenith, const Vect3d &south, Vect3d &sun_dir);
    /// Calculate sun direction in global CS from sun location
    static void SunLoc2Dir(const Vect2d &sun_loc, const Matrix3d &orient_rm,
                           Vect3d &sun_dir);
    /// Conversion of direction in cartesian CS to sun altitude and azimuth
    static void Cartesian2SunLoc(const Vect3d &cart, Vect2d &sun_loc);
    /// Conversion of sun altitude and azimuth to direction in cartesian CS
    static void SunLoc2Cartesian(const Vect2d &sun_loc, Vect3d &cart);
    /// Calculate sun location from the the light rotation matrix.
    static void RM2SunLoc(const Matrix3d &light_rm, const Matrix3d &rm,
                          Vect2d &sun_loc);
    /// Calculate  light rotation matrix from the sun location.
    static void SunLoc2RM(const Vect2d &sun_loc, const Matrix3d &orient_rm,
                       Matrix3d &light_rm);

    //@}
    /// Light emission direction in light CS.
    inline static Vect3d LightOwnDir();

  private:
    /// Orientation rotation matrix, from local CS to global:
    /// m_orient_rm[0] - east
    /// m_orient_rm[1] - north
    /// m_orient_rm[2] - zenith
    Matrix3d m_orient_rm;
    /// Sun location: altitide and azimuth.
    /// Sun altitude grows from 0 at horizon line to PI / 2 at zenith
    /// Sun azimuth is counted from north to east
    Vect2d m_sun_loc;

  };


//////////////////////////////////////////////////////////////////////////////
/// Get Zenith direction.
///
/// @return Zenith direction.
const Vect3d &SunLoc::GetZenith() const
  {
  return m_orient_rm.r2;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get South direction.
///
/// @return South direction.
Vect3d SunLoc::GetSouth() const
  {
  return -m_orient_rm.r1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Rotation matrix, from local CS to global.
///
/// Local CS is Coordinate System related with sky directions:
/// East   - along X axis,
/// North  - along Y axis,
/// Zenith - along Z axis.
/// Global CS is CS of scene root node.
/// @return Rotation matrix, from local CS to global.
const Matrix3d &SunLoc::GetOrientRM() const
  {
  return m_orient_rm;
  }

//////////////////////////////////////////////////////////////////////////////
/// Light emission direction in light CS.
///
/// Direction is the same in both Left and  Right CS: -Z is understandable for
/// Right CS (the same as for camera), but not understandable for Left CS. But
/// it was so in Fly. The most right solution - point real light direction
/// directly in light classes (where it have sence). Definitions are required
/// only for Gonio Lights for conversion of spheric coordinates to decart.
/// @return 3D vector {0, 0, -1}.
Vect3d SunLoc::LightOwnDir()
  {
  return Vect3d(0, 0, -1);
  }

INTEGRA_NAMESPACE_END



#endif  // #ifndef _KC3H_SUNLOC_HPP
