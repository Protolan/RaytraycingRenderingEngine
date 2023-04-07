/// @internal
/// @file
///
/// @brief "Overview of Functionality" page.
///
/// @author Fvg - Vjacheslav Glukhov, KLAS, '04.09.19
///
/// Copyright &copy; INTEGRA, Inc., 2004.

//////////////////////////////////////////////////////////////////////////////
/**

@page math_overview Overview of Functionality

Math library is basic library for the most mathematical operations in Integra
SDK, provides platform-independent lower-level commonly used services to 
deal with vectors, points, matrices, arrays, random numbers and etc.

<CENTER><B>Contents of the Mathematical library:</B></CENTER>

  @li <B>Constants and functions.</B><BR>
  - Commonly used mathematical constants and functions are provided 
  via math.hpp header file. 
  - The special functions are elaborated for conversion between Cartesian and 
  polar coordinate systems described in the polar.hpp header file. 
  - The special template classes Math, Math2, Math3 provides comparison between 
  elements (values, vectors, points) of the one-dimensional, two-dimensional 
  and three-dimensional spaces. For the more compount objects (4D vectors,
  matrices) there are own methods of the comparison. For each template class, a 
  number of predefined   template instances (float, double) is available. These classes described in 
  the header files math.hpp, vect2.hpp, vect3.hpp. 

  @li <B>Vectors and points.</B><BR>
  Vectors and points are described by template classes.  There are different 
  template classes dependent on dimension of the vector/point (2D, 3D or 4D), 
  and whether arithmetic operations may be performed on the vectors or not. 
  These classes described in the header files vect2.hpp, vect3.hpp, vect4.hpp 
  for the 2D, 3D and 4D spaces respectively. There are a number predefined 
  template instances for the generally accented types (byte, integer, double
  and etc.). The following table presents the full set of the predefined 
  template instances.

<CENTER>
<TABLE> 
<TR> 
<TD><CENTER>Template<BR>class</CENTER></TD> 
<TD><CENTER>Dim</CENTER></TD> 
<TD><CENTER>Arithmetic</CENTER></TD> 
<TD><CENTER>byte</CENTER></TD> 
<TD><CENTER>short</CENTER></TD> 
<TD><CENTER>int</CENTER></TD> 
<TD><CENTER>unsigned</CENTER></TD> 
<TD><CENTER>float</CENTER></TD>
<TD><CENTER>double</CENTER></TD> 
<TD><CENTER>long</CENTER></TD> 
<TD><CENTER>WORD</CENTER></TD>
</TR>

<TR> 
<TD><CENTER>BVect2</TD> <TD>2</TD> <TD>no</TD> 
<TD>Set2b</TD> <TD>Set2s</TD> <TD>Set2i</TD>
<TD>Set2u</TD> <TD>Set2f</TD> <TD>Set2d</TD> 
<TD>-</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>TVect2</TD> <TD>2</TD> <TD>yes</TD> 
<TD>-</TD> <TD>Vect2s</TD> <TD>Vect2i</TD>
<TD>-</TD> <TD>Vect2f</TD> <TD>Vect2d</TD>
<TD>-</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>TPoint2</TD> <TD>2</TD> <TD>yes</TD> 
<TD>-</TD> <TD>-</TD> <TD>Point2i</TD>
<TD>-</TD> <TD>Point2f</TD> <TD>Point2d</TD>
<TD>Point2l</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>BVect3</TD> <TD>3</TD> <TD>no</TD> 
<TD>Set3b</TD> <TD>Set3s</TD> <TD>Set3i</TD>
<TD>Set3u</TD> <TD>Set3f</TD> <TD>-</TD>
<TD>-</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>TVect3</TD> <TD>3</TD> <TD>yes</TD> 
<TD>-</TD> <TD>Vect3s<BR>Color3s</TD> <TD>Vect3i<BR>Color3i</TD>
<TD>-</TD> <TD>Vect3f<BR>Color3f<BR>RGBColor</TD> <TD>Vect3d<BR>Color3d</TD>
<TD>-</TD> <TD>Color3w</CENTER></TD>
</TR>

<TR> <TD><CENTER>TPoint3</TD> <TD>3</TD> <TD>yes</TD> 
<TD>-</TD> <TD>-</TD> <TD>Point3i</TD>
<TD>-</TD> <TD>Point3f</TD> <TD>Point3d</TD>
<TD>-</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>BVect4</TD> <TD>4</TD> <TD>no</TD> 
<TD>Set4b</TD> <TD>Set4s</TD> <TD>Set4i</TD>
<TD>Set4u</TD> <TD>Set4f</TD> <TD>-</TD>
<TD>-</TD> <TD>-</CENTER></TD>
</TR>

<TR> <TD><CENTER>TVect4</TD> <TD>4</TD> <TD>yes</TD> 
<TD>-</TD> <TD>Vect4s<BR>Color4s</TD> <TD>Vect4i<BR>Color4i</TD>
<TD>-</TD> <TD>Vect4f<BR>Color4f<BR>RGBAColor</TD> 
<TD>Vect4d<BR>Color4d</TD> <TD>-</TD> <TD>Color4w</CENTER></TD>
</TR>

</TABLE>
</CENTER>

  @li <B>Matrices.</B><BR>
  Matrices are provided via template classes.  There are template classes 
  for 2x2, 3x3, 4x3, 4x4 matrices. For each template class, a two predefined 
  template instances for float and double elements are available.
  - TMatrix2. This template class defines 2x2 matrix. Matrix2f and Matrix2d are 
  predefined instances with float and double elements, respectively. Usually 
  the matrices 2x2 are used for transformations of 2D  points and vectors by 
  rotation or scaling. This class is described in matrix2.hpp header file.
  - TMatrix3. This template class defines 3x3 matrix. Matrix3f and Matrix3d are 
  predefined instances with float and double elements, respectively. Usually 
  the matrices 3x3 are used for transformations of 3D  points and vectors by 
  rotation or scaling. This class is described in matrix3.hpp header file.
  - TMatrix43. This template class defines 4x3 matrix (four rows and three 
  columns). Matrix43f and Matrix43d are predefined instances with float and 
  double elements, respectively. Usually the matrices 4x3 are used for 
  transformations of 3D points and vectors by rotation, translation and scaling. 
  This class is described in matrix43.hpp header file.
  - TMatrix4. This template class defines 4x4 matrix. Matrix4f and Matrix4d are 
  predefined instances with float and double elements, respectively. Usually 
  the matrices 4x4 are used for the all transformations of 3D points and 
  vectors in homogeneous coordinates (rotation, translation, scaling, 
  orthogonal and perspective transformations). This class described in 
  matrix4.hpp header file.

  @li <B>Bounding boxes.</B><BR>
  Template classes BBox1, BBox2 and BBox3 implement bounding boxes 
  determined by minimal and maximal points having coordinates of type T. There 
  are predefined instances of the these template classes.
  - BBox1. It is range in 1D (one-dimensional) space (segment). There are a 
  number predefined template instances BBox1i, BBox1f, BBox1d for the following 
  types respectively: int, float, double. This class described in math.hpp header 
  file.
  - BBox2. It is range in 2D (two-dimensional) space (rectangle). There are a 
  number predefined template instances BBox2i, BBox2f, BBox2d for the following 
  types respectively: int, float, double. This class described in vect2.hpp 
  header file.
  - BBox3. It is range in 3D (three-dimensional) space (rectangular 
  parallelepiped). There are a number predefined template instances BBox3i, 
  BBox3f, BBox3d for the following types respectively: int, float, double. This 
  class described in vect3.hpp header file.

  @li <B>Bounding sphere.</B><BR>
  Template class BSphere3  implements simple bounding sphere determined by 
  center of sphere and it's radius of type T. BSphere3f and BSphere3d are 
  predefined instances of the template class for float and double values, 
  respectively. This class described in bsphere3.hpp header file.

  @li <B>Coordinate System.</B><BR>
  All classes of Integra SDK use Right hand coordinate system with Up vector 
  along Z axis. But some aplications use Left hand coordinate system. The 
  special class CoordSys provides conversion geometrical object from
  Left hand coordinate system to Right hand coordinate system. In addition this
  class provides access to the some predefined orientation vectors and matrices 
  of the orthogonal views.

  @li <B>Sorted arrays.</B><BR>
  Template class MArray provides sorting, binary search of the array. Base 
  element type T must provide proper "==" and "<" operators for the binary 
  search. Methods of this class provide sorting of the arrays in arbitrary
  order irrespective of comparison operators. This class described in 
  marray.hpp header file.

  @li <B>Random numbers generators.</B><BR>
  There are two classes Rnd and Niederreiter for the random number generation. 
  The fisrt Rnd class (described in rnd.hpp header file) uses well-known 
  algorithms for the generation of the @b pseudo-random numbers on the segment, 
  @b pseudo-random points on the sphere. It is possible that level of randomness 
  provided by this class is not sufficient for the some special cases. In these
  cases the Niederreiter class (described in nrnd.hpp header file) may be used 
  instead. This class generates @b quasi-random values based on the special more 
  refined algorithm.

  @li <b>Complex Arithmetics</b><br>
  Complex numbers math is implemented by TComplex template class.
  Current implementation supports float and double types for
  template parameter. Performance of all methods provided by this
  class is either equal or greater than corresponding functionality
  available from standard template library (STL).

**/