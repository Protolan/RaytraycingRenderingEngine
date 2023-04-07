/// @file
///
/// @brief Class for Sun location calculations.
///
/// @author  Piv - Ildar Valiev, KC3H, '06.01.27
///
/// Copyright &copy; INTEGRA, Inc., 2006.

#include <integra.hpp>

#include "sunloc.hpp"

INTEGRA_NAMESPACE_START

/**

@class SunLoc math/sunloc.hpp

Class for Sun location calculations.

This class allows mutual conversion of sun directions in  cartesian
CS, sun location defined by altitude and azimuth. Calculations take
into account scene orientation rotation matrix.

Sun  altitude  grows from 0 at horizon line to PI / 2 at zenith Sun
azimuth is counted from the north to east.

**/


//////////////////////////////////////////////////////////////////////////////
/// Create orientation rotation matrix from the given
/// zenith and south directions.
///
/// @param[in] zenith  Direction to zenith.
/// @param[in] south   Direction to south.
/// @param[out] orient Orientation matrix - rotates vectors from local to
///                    global CS.
void SunLoc::CreateOrientMatrix(const Vect3d &zenith,
                                  const Vect3d &south, Matrix3d &orient)
  {
  orient[2] = zenith;
  orient[2].Normalize();
  orient[0] = CrossProd(orient[2], south);      // east
  orient[0].Normalize();
  orient[1] = CrossProd(orient[2], orient[0]);  // north
  }

//////////////////////////////////////////////////////////////////////////////
/// Correct south direction to be orthogonal to Zenith.
///
/// @param[in] zenith  Direction to zenith.
/// @param[in,out] south   Direction to south.
void SunLoc::CorrectSouth(const Vect3d &zenith, Vect3d &south)
  {
  Matrix3d orient;
  orient[2] = zenith;
  orient[2].Normalize();
  orient[0] = CrossProd(orient[2], south);      // east
  orient[0].Normalize();
  orient[1] = CrossProd(orient[2], orient[0]);  // north
  south = -orient[1];
  }

/////////////////////////////////////////////////////////////////////////////
/// Calculate sun location from sun direction
///
/// @param[in] sun_dir  Direction to Sun.
/// @param[in] orient  Orientation matrix - rotates vectors from local to global CS.
/// @param sun_loc Sun location - altitude and azimuth.
void SunLoc::Dir2SunLoc(const Vect3d &sun_dir, const Matrix3d orient,
                              Vect2d &sun_loc)
  {
  Vect3d loc_dir_to_sun = orient.BackRotated(sun_dir);
  Cartesian2SunLoc(loc_dir_to_sun, sun_loc);
  } // End of Dir2SunLoc method

//////////////////////////////////////////////////////////////////////////////
/// Calculate sun location from the light rotation matrix.
///
/// @param[in] light_rm Light rotation matrix.
/// @param[in] orient_rm Orientation matrix.
/// @param[out] sun_loc Sun altitude and azimuth.
void SunLoc::RM2SunLoc(const Matrix3d &light_rm, const Matrix3d &orient_rm,
                       Vect2d &sun_loc)
  {
  Vect3d dir_to_sun = -light_rm.Rotated(LightOwnDir());

  Vect3d loc_dir_to_sun = orient_rm.BackRotated(dir_to_sun);
  Cartesian2SunLoc(loc_dir_to_sun, sun_loc);
  }

//////////////////////////////////////////////////////////////////////////////
/// Calculate  light rotation matrix from the sun location.
///
/// @param[in] sun_loc Sun altitude and azimuth.
/// @param[in] orient_rm Orientation matrix.
/// @param[out] light_rm Light rotation matrix.
void SunLoc::SunLoc2RM(const Vect2d &sun_loc, const Matrix3d &orient_rm,
                       Matrix3d &light_rm)
  {
  Vect3d loc_dir_to_sun;
  SunLoc2Dir(sun_loc, orient_rm, loc_dir_to_sun);
  Matrix3d rm;
  light_rm.Rotation(LightOwnDir(), -loc_dir_to_sun);
  }

/////////////////////////////////////////////////////////////////////////////
/// Calculate sun location from sun direction in global CS
///
/// @param sun_dir  Direction to Sun.
/// @param zenith  Direction to zenith.
/// @param south  Direction to south.
/// @param sun_loc Sun location - altitude and azimuth.
void SunLoc::Dir2SunLoc(const Vect3d &sun_dir, const Vect3d &zenith,
                              const Vect3d &south, Vect2d &sun_loc)
  {
  Matrix3d orient;
  CreateOrientMatrix(zenith, south, orient);
  // Dir2SunLoc(sun_dir, orient, sun_loc);
  Vect3d loc_dir_to_sun = orient.BackRotated(sun_dir);
  Cartesian2SunLoc(loc_dir_to_sun, sun_loc);
  } // End of Dir2SunLoc method


/////////////////////////////////////////////////////////////////////////////
/// Calculate sun direction in global CS from sun location
///
/// @param[in] sun_loc Sun location - altitude and azimuth.
/// @param[in] zenith  Direction to zenith.
/// @param[in] south  Direction to south.
/// @param[out] sun_dir  Direction to Sun.
void SunLoc::SunLoc2Dir(const Vect2d &sun_loc,
                  const Vect3d &zenith, const Vect3d &south, Vect3d &sun_dir)
  {
  SunLoc2Cartesian(sun_loc, sun_dir);
  Matrix3d orient;
  CreateOrientMatrix(zenith, south, orient);
  orient.Rotate(sun_dir);
  } // End of SunLoc2Dir method


/////////////////////////////////////////////////////////////////////////////
/// Calculate sun direction in global CS from sun location
///
/// @param[in] sun_loc Sun location - altitude and azimuth.
/// @param[in] orient_rm The orientation matrix.
/// @param[out] sun_dir  Direction to Sun.
void SunLoc::SunLoc2Dir(const Vect2d &sun_loc,
                  const Matrix3d &orient_rm, Vect3d &sun_dir)
  {
  SunLoc2Cartesian(sun_loc, sun_dir);
  orient_rm.Rotate(sun_dir);
  } // End of SunLoc2Dir method

/////////////////////////////////////////////////////////////////////////////
/// Calculate sun direction in global CS from sun location
///
/// @param[in] sun_loc Sun location - altitude and azimuth.
/// @return Sun direction.
Vect3d SunLoc::SunLoc2Dir(const Vect2d &sun_loc) const
  {
  Vect3d sun_dir;
  SunLoc2Cartesian(sun_loc, sun_dir);
  m_orient_rm.Rotate(sun_dir);

  return sun_dir;
  } // End of SunLoc2Dir method


//////////////////////////////////////////////////////////////////////////////
/// Conversion of direction in cartesian CS to sun altitude and azimuth.
///
/// Sun altitude grows from 0 at horizon line to PI / 2 at zenith
/// Sun azimuth is counted from north to east
///
/// @param cart   Direction in cartesian CS
/// @param sun_loc Sun altitude and azimuth.
void SunLoc::Cartesian2SunLoc(const Vect3d &cart, Vect2d &sun_loc)
  {
  sun_loc.x = ASin(cart.z);

  if (MathD::AboutEqual(sun_loc.x, -PI / 2) ||
      MathD::AboutEqual(sun_loc.x, PI / 2))
    sun_loc.y = 0;
  else
    {
    sun_loc.y = ATan2(cart.x, cart.y);
    if (sun_loc.y < 0.0)
      sun_loc.y += PI2;
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion of sun altitude and azimuth to direction in cartesian CS
///
/// Sun altitude grows from 0 at horizon line to PI / 2 at zenith.
/// Sun azimuth is counted from north to east, from Y to X. North -> Y, East ->X
///
/// @param sun_loc  Sun altitude and azimuth.
/// @param cart     Direction in cartesian CS.
void SunLoc::SunLoc2Cartesian(const Vect2d &sun_loc, Vect3d &cart)
  {
  cart.z   = Sin(sun_loc.x);
  double d = Sqrt(1.0 - Sqr(cart.z));
  cart.x   = Sin(sun_loc.y) * d;
  cart.y   = Cos(sun_loc.y) * d;
  }



INTEGRA_NAMESPACE_END



