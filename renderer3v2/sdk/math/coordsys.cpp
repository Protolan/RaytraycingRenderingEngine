/// @internal
/// @file
///
/// @brief CoordSys class implementation
///
/// @author Piv - Ildar Valiev<BR>
///         Fgk - Gennady Kireyko
///
/// Copyright &copy; INTEGRA, Inc., 2002.

#include <integra.hpp>

#include "coordsys.hpp"

INTEGRA_NAMESPACE_START

/**

@class CoordSys math/coordsys.hpp

Coordinate system definition and conversion.

<HR>
@par Coordinate system.

All new classes and applications of Integra use Right hand coordinate system
with Up vector along Z axis. Orientation of proper (Top, Front, Right) views
is shown below:

@image html views.gif

There are some objects in Integra SDK (Camera and Light) which use own
coordinate systems. @anchor Camera Camera coordinate system has View direction
along -Z,  Up direction along Y and Right vector along X.

@image html camera.gif

Emission direction of parallel lights and polar axis for cone and gonio lights
is along -Z. Phi angle of gonios is counted depending on Gonio type. Integra SDK
supports the several types of gonios:

- For Iesna type C: from +X axis in direction natural for right CS and positive
Z axis: Phi angle grows from X to +Y axis.

@image html light_c.gif

- For TBT gonios named as Type_T: from -Y axis in direction natural for left
CS and negative to Z direction of polar axis: Phi angle grows from -Y to +X
axis.

@image html light_t.gif

@par Order of vertices in polygons and triangles.

Surface normals for polygons and triangles are calculated following the next
convention:
- For Right hand CS order of vertices is counterclockwise when the vertices
are viewed (in the local coordinate system of the object) from the opposite
direction as the normal.
- For left hand CS they are clockwise.

@par Sky Light directions.

Zenith vector by default coincides with Up vector of coordinate system (Z axis
in positive direction). South vector coincides with -Y (Y axis in negative
direction). In other words, top view is by default oriented so that North
direction is up.

@par Coordinate system conversion.

Conversion of coordinate system of scenes is performed during import or
loading. Conversion during import is performed individually for each supported
format using either functions of CoordSys class or own functions of
proper importer. For example, VRML format already has right CS, only Up vector
is changed for VRML files with Up vector along Y, or left as is for VRML files
with Up  vectors along Z.

Geometry  for real Left to Right conversion is processed in the following way:
- coordinates of the vectors(points) are converted by swaping Y and Z
components;
- transformation matrices are transformed by multiplying to the same matrix
from both left and right sides: TM' = CM * TM * CM, where CM is special
conversion matrix;
- conversion matrix is proper for conversion of vectors and points- it swaps Y
and Z;
- order of vertices in triangles and polygons is inverted;

Cameras and Lights are transformed in special way. It is not possible to apply
the same conversion as for geometry because their directions in own CS  need
in special conversion (see ConvForCamera(), ConvForLight()):

**/

//////////////////////////////////////////////////////////////////////////////
/// Conversion matrix
Matrix43d CoordSys::conv_tm(Matrix3d(Vect3d(1, 0, 0),
                                     Vect3d(0, 0, 1),
                                     Vect3d(0, 1, 0)),
                                     Vect3d(0));

//////////////////////////////////////////////////////////////////////////////
/// Coordinate system conversion for array of 3D points.
///
/// Conversion realized by the swapping 2nd and 3th point's coordinates for
/// the all points of the array.
/// @param[in, out] vert - Array of the 3D points with float coordinates.
void CoordSys::Conv(TArray<Point3f> &vert)
  {
  for (int i = 0; i < vert.Length(); i++)
    Conv(vert[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Coordinate system conversion for array of 3D points.
///
/// Conversion realized by the swapping 2nd and 3th point's coordinates for
/// the all points of the array.
/// @param[in, out] vert - Array of the 3D points with float coordinates.
void CoordSys::Conv(TArray<Point3d> &vert)
  {
  for (int i = 0; i < vert.Length(); i++)
    Conv(vert[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Coordinate system conversion for array of 3D vectors.
///
/// Conversion realized by the swapping 2nd and 3th vector's coordinates for
/// the all points of the array.
/// @param[in, out] vect - Array of the 3D vectors with float coordinates.
void CoordSys::Conv(TArray<Vect3f> &vect)
  {
  for (int i = 0; i < vect.Length(); i++)
    Conv(vect[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Coordinate system conversion for light node TM.
///
/// @param[in, out] tm - Transformation matrix.
/// @param[in] compens - Sign of the additional transformation to compensate
/// wrong TM created in the some cases.
///
/// @note It can't be performed in common way because in both left and right
/// CS light is rotated from -Z direction to direction of light.
///
/// Conversion is performed in the following way:
/// - Translation part is converted as usually (as 3D vector),
/// - Rotation part:  @a R' = @a NX * @a R * @a conv_tm, where
///   - @a NX - 3x3 matrix negates X component of vectors;
///   - @a R  - 3x3 matrix - rotation part of @a tm;
///   - @a conv_tm - internal conversion matrix.
void CoordSys::ConvForLight(Matrix43d &tm, bool compens)
  {
  Vect3d trans = tm.GetTrans();
  Conv(trans);
  tm.SetTrans(trans);
  Matrix3d nx(Vect3d(-1, 0, 0), Vect3d(0, 1, 0), Vect3d(0, 0, 1));
  tm.SetMatrix3(nx * tm.Matrix3() * conv_tm.Matrix3());

  if (compens)
    {
    // below part is added to compensate wrong TM created by TBTLoader
    // and saved in IOF files before KILR task
    Matrix3d rot;
    rot.RotationZ(-PI / 2);
    tm.SetMatrix3(rot * tm.Matrix3());
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Coordinate system conversion for Camera node TM.
///
/// @param[in, out] tm - Transformation matrix
///
/// @note It can't be performed in common way because camera definition change
/// is not compatible with selected CS conversion.
///
/// Conversion is performed in the following way:
/// - Translation part is converted as usually (as 3D vector),
/// - Rotation part:  @a R' = @a NZ * @a R * @a conv_tm,  where
///   - @a NZ - 3x3 matrix negates Z component of vectors;
///   - @a R  - 3x3 matrix - rotation part of @a tm;
///   - @a conv_tm - internal conversion matrix.
void CoordSys::ConvForCamera(Matrix43d &tm)
  {
  Vect3d trans = tm.GetTrans();
  Conv(trans);
  tm.SetTrans(trans);
  Matrix3d nz(Vect3d(1, 0, 0), Vect3d(0, 1, 0), Vect3d(0, 0, -1));
  tm.SetMatrix3(nz * tm.Matrix3() * conv_tm.Matrix3());
  }

INTEGRA_NAMESPACE_END
