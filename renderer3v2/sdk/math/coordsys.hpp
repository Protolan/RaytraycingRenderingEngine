/// @file
///
/// @brief Coordinate system definition
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004.
///
/// @internal
/// @author Piv - Ildar Valiev<BR>
///         Fgk - Gennady Kireyko

#ifndef _KILR_COORDSYS_HPP_
#define _KILR_COORDSYS_HPP_

#include <base/arrays.hpp>
#include <math/vect3.hpp>
#include <math/matrix43.hpp>

INTEGRA_NAMESPACE_START

/// Coordinate system definition and conversion.
class CoordSys
  {
  public:
    /// @name Coordinate system (CS) conversion for some classes
    //@{
    /// CS conversion for vectors with float coordinates.
    inline static void Conv(Vect3f &v);
    /// CS conversion for points with float coordinates.
    inline static void Conv(Point3f &v);
    /// CS conversion for points with float coordinates.
    inline static void Conv(Point3d &v);
    /// CS conversion for vectors with double coordinates.
    inline static void Conv(Vect3d &v);

    /// CS conversion for array of 3D points.
    static void Conv(TArray<Point3f> &vert);
    /// CS conversion for array of 3D points.
    static void Conv(TArray<Point3d> &vert);
    /// CS conversion for array of 3D vectors.
    static void Conv(TArray<Vect3f> &vect);
    /// CS conversion for 4x3 matrices
    inline static void Conv(Matrix43d &tm);
    /// CS conversion for 3x3 matrices.
    inline static void Conv(Matrix3d &rm);

    /// CS conversion for light node TM.
    static void ConvForLight(Matrix43d &tm, bool compens);
    /// CS conversion for Camera node TM.
    static void ConvForCamera(Matrix43d &tm);
    //@}

    /// @name Default Up (Zenith), Front, Right and South vectors.
    //@{
    /// Default Up (Zenith) vector.
    inline static Vect3d UpDir();
    /// Default Front vector.
    inline static Vect3d FrontDir();
    /// Default Right vector.
    inline static Vect3d RightDir();
    /// Default South vector.
    inline static Vect3d SouthDir();
    //@}

    /// @name Default orthogonal view matrices.
    /// These matrices are defined by the Camera orientation with respect
    /// to coordinate system. Method TMatrix3<T>::GenRotation() provides common
    /// way of these matrices creation. Multiplication of such matrix by the
    /// point from the right returns point position in the coordinate system
    /// of the Camera.
    //@{
    /// Front view matrix for orthogonal camera.
    inline static Matrix3d FrontView();
    /// Left view matrix for orthogonal camera.
    inline static Matrix3d LeftView();
    /// Back view matrix for orthogonal camera.
    inline static Matrix3d BackView();
    /// Right view matrix for orthogonal camera.
    inline static Matrix3d RightView();
    /// Top view matrix for orthogonal camera.
    inline static Matrix3d TopView();
    /// Bottom view matrix for orthogonal camera.
    inline static Matrix3d BottomView();
    //@}

  private:
    static Matrix43d conv_tm; // is used for conversion of matrices
  };
// END OF class 'CoordSys'

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for vectors with float coordinates.
///
/// Conversion realized by the swapping 2nd and 3th vector coordinates.
/// @param[in, out] v - 3D vector.
void CoordSys::Conv(Vect3f &v)
  {
  Swap(v.y, v.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for points with float coordinates.
///
/// Conversion realized by the swapping 2nd and 3th point coordinates.
/// @param[in, out] v - 3D point.
void CoordSys::Conv(Point3f &v)
  {
  Swap(v.y, v.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for points with float coordinates.
///
/// Conversion realized by the swapping 2nd and 3th point coordinates.
/// @param[in, out] v - 3D point.
void CoordSys::Conv(Point3d &v)
  {
  Swap(v.y, v.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for vectors with double coordinates.
///
/// Conversion realized by the swapping 2nd and 3th vector coordinates.
/// @param[in, out] v - 3D vector.
void CoordSys::Conv(Vect3d &v)
  {
  Swap(v.y, v.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for 4x3 matrices.
///
/// Conversion realized by the multiplication to the left and to the right
/// of the special conversion matrix (see operator @a * for the 4x3 matrices
/// (class TMatrix43<T>).
///
/// @param[in, out] tm - Transformation matrix.
void CoordSys::Conv(Matrix43d &tm)
  {
  tm = conv_tm * tm * conv_tm;
  }

//////////////////////////////////////////////////////////////////////////////
/// CS conversion for 3x3 matrices.
///
/// Conversion realized by the multiplication to the left and to the right
/// of the rotation part of the special conversion matrix.
/// @param[in, out] rm - Rotation matrix.
void CoordSys::Conv(Matrix3d &rm)
  {
  rm = conv_tm.Matrix3() * rm * conv_tm.Matrix3();
  }

//////////////////////////////////////////////////////////////////////////////
/// Default Up (Zenith) vector.
/// @return Vector {0, 0, 1}.
Vect3d CoordSys::UpDir()
  {
  return Vect3d(0, 0, 1);
  }

//////////////////////////////////////////////////////////////////////////////
/// Default Front vector.
/// @return Vector {0, 1, 0}.
Vect3d CoordSys::FrontDir()
  {
  return Vect3d(0, 1, 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Default Right vector.
/// @return Vector {1, 0, 0}.
Vect3d CoordSys::RightDir()
  {
  return Vect3d(1, 0, 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// @brief Default South vector
/// @return Vector {1, 0, 0} if constant OLD_NORTH is defined, or {0, -1; 0}.
Vect3d CoordSys::SouthDir()
   {
#ifdef OLD_NORTH
   return Vect3d(1, 0, 0);
#else
   return Vect3d(0, -1, 0);
#endif
   }

//////////////////////////////////////////////////////////////////////////////
/// Front view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD>1</TD><TD>0</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>0</TD><TD>1</TD></TR>
/// <TR><TD>0</TD><TD>-1</TD><TD>0</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to front view.
///
/// @image html camera_front.gif
///
/// @return 3x3 rotation matrix of the front view.
Matrix3d CoordSys::FrontView()
  {
  return Matrix3d(Vect3d(1, 0, 0), Vect3d(0, 0, 1), Vect3d(0, -1, 0));
  }

//////////////////////////////////////////////////////////////////////////////
/// Left view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD> 0</TD><TD>-1</TD><TD>0</TD></TR>
/// <TR><TD> 0</TD><TD> 0</TD><TD>1</TD></TR>
/// <TR><TD>-1</TD><TD> 0</TD><TD>0</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to left view.
///
/// @image html camera_left.gif
///
/// @return 3x3 rotation matrix of the left view
Matrix3d CoordSys::LeftView()
  {
  return Matrix3d(Vect3d(0, -1, 0), Vect3d(0, 0, 1), Vect3d(-1, 0, 0));
  }

//////////////////////////////////////////////////////////////////////////////
/// Back view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD>-1</TD><TD>0</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>0</TD><TD>1</TD></TR>
/// <TR><TD>0</TD><TD>1</TD><TD>0</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to back view.
///
/// @image html camera_back.gif
///
/// @return 3x3 rotation matrix of the back view.
Matrix3d CoordSys::BackView()
  {
  return Matrix3d(Vect3d(-1, 0, 0), Vect3d(0, 0, 1), Vect3d(0, 1, 0));
  }

//////////////////////////////////////////////////////////////////////////////
/// Right view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD>0</TD><TD>1</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>0</TD><TD>1</TD></TR>
/// <TR><TD>1</TD><TD>0</TD><TD>0</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to right view.
///
/// @image html camera_right.gif
///
/// @return 3x3 rotation matrix of the right view
Matrix3d CoordSys::RightView()
  {
  return Matrix3d(Vect3d(0, 1, 0), Vect3d(0, 0, 1), Vect3d(1, 0, 0));
  }

//////////////////////////////////////////////////////////////////////////////
/// Top view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD>1</TD><TD>0</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>1</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>0</TD><TD>1</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to top view.
///
/// @image html camera_top.gif
///
/// @return 3x3 rotation matrix of the top view.
Matrix3d CoordSys::TopView()
  {
  return Matrix3d(Vect3d(1, 0, 0), Vect3d(0, 1, 0), Vect3d(0, 0, 1));
  }

//////////////////////////////////////////////////////////////////////////////
/// Bottom view matrix for orthogonal camera.
///
/// <TABLE>
/// <TR><TD>1</TD><TD>0</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>-1</TD><TD>0</TD></TR>
/// <TR><TD>0</TD><TD>0</TD><TD>-1</TD></TR>
/// </TABLE>
/// Rows of this matrix define orientation of the coordinate axes of the
/// Camera coordinate system. Camera is oriented to bottom view.
///
/// @image html camera_bottom.gif
///
/// @return 3x3 rotation matrix of the bottom view.
Matrix3d CoordSys::BottomView()
  {
  return Matrix3d(Vect3d(1, 0, 0), Vect3d(0, -1, 0), Vect3d(0, 0, -1));
  }

INTEGRA_NAMESPACE_END
#endif // #ifndef _KILR_COORDSYS_HPP_

