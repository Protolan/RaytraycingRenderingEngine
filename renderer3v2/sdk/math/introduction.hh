/// @internal
/// @file
///
/// @brief "Quick Introduction" page.
///
/// @author Fvg - Viacheslav Glukhov, KLAS, '04.09.19
///
/// Copyright &copy; INTEGRA, Inc., 2004.

//////////////////////////////////////////////////////////////////////////////
/**

@page math_introduction Quick Introduction


<p>
<HR>
@section math_working_with_vectors Working with vectors and points.
<HR>

Classes TVect2, TPoint2, TVect3, TPoint3, TVect4 can be used to operate with 
vectors and points in 2D, 3D and 4D numeric spaces. There are several predefined 
instances for the each class of vectors and points. For example, Vect2d is 
predefined instance of template class TVect2 with double elements, Point3f is
predefined instance of template class TPoint3 with float elements. The full
table of the predefined instances is presented in section @ref math_overview.

Constructors allow to initialize vectors and points by scalar, values of 
elements, by other vector or point of the @b same type. 

@note There are template functions ConvI(const TVect3<T> &u), 
ConvF(const TVect3<T> &u), ConvD(const TVect3<T> &u) for the conversions 
between objects (vectors, points, matrices, bounding boxes, bounding spheres) 
with elements of different types. Example of the conversation functions usage 
see in section @ref math_working_with_bspheres.

@b Example:
@code

  // 2D vectors 'a1' and 'c'  with double components will be created without 
  // initialization.
  Vect2d a1, c;  

  // 3D point 'a2' is initialized  by the value 3.7.
  Point3f a2((float)3.7);

  // 3D vector 'a3' is initialized by the three double components.
  Vect3d a3(3.1, 2.3, 0.0); 

  // 3D point 'a4' is initialized by the point 'a2'
  Point3f a4 = a2;

  // 3D vector 'a5' is initialized by the vector 'a3'.
  Vect3d a5(a3);

  // Vector 'a6' is initialized by the two points 'a2' {3.7, 3.7, 3.7} and 
  // point {1.5, 3.0, 4.0}. Resulted point is {-2.2, -0.7, 0.3}.
  Vect3f a6(a2, Point3f(1.5, 3.0, 4.0));

  // 4D vector 'a7' is initialized by the 3D vector 'a3'. Fourth coordinate of the 
  // vector 'a7' is initialized by 1.0. Resulted vector is {3.1, 2.3, 0.0, 1.0}
  Vect4d a7(a3); 

@endcode

Methods of vector and point classes allow to operate all standard operations of 
vector algebra with elements of the same type, access to elements of the vector 
and point can be done with usage of operator [] but since components of vectors 
and points (x, y for 2D and x, y, z for 3D) are public then direct access to 
the components are more preferable way. 

Arithmetic operations with points are specific and take into account the 
distinctions in notions of the vector and point. In particular the addition of
the two points is absent, but there are operations of vector addition to point 
and vector subtraction from point.

@b Example:
@code
  // Vectors 'a', 'b' are initialized by three components
  Vect3d a(-6.2, -0.1, 0.2), b(2.3, -7.5, 6.3);

  // Vector 'c' is initialized by the sum of vectors 'a' and 'b'.
  // Coordinates of the vector 'c' is {-3.9, -7.6, 6.5}
  Vect3d c = a + b;

  // Point 'pa' is initialized by the three values
  Point3d pa(-1.0, 2.5, 0.6);

  // Point 'pb' is shifted with respect to point 'pa' by the addition of the 
  // vector 'c'. Coordinates of the point 'pb' {-4.9, -5.1, 7.1} 
  Point3d pb = pa + c;

  // Point 'pb' is shifted to point 'pa' by the subtraction of the vector 'c'
  pb -= c;
  
  // Vector 'a1' is initialized by the coordinates of vector 'c' multiplied by 
  // value 3.2. Coordinates of the vector 'a1' is {-12.48, -24.32, 20.8}
  Vect3d a1 = 3.2 * c;

  // All coordinates of the 'a' vector are divided by the coordinates of 
  // vector 'b'. Coordinates of the vector 'a' is ~{-2.7, 0.01, 0.03}
  a /= b;

  // Coordinates of the 'b' vector are the products of the 'c' vector
  // coordinates and 'a1' vector coordinates. Coordinates of the resulted
  // vector 'b' is {48.672, 184.832, 135.2}
  b = c * a1;

  // Only second coordinate of the 'c' vector is changed
  c.y = b.y + a.y;  // More preferable access to vector components

  // The same action as above but via indexes
  c[1] = b[1] + a[1]; 

  // Normalization of the 'c' vector
  c.Normalize();

  // Create 2D vector for the polar coordinates usage
  Vect2d p;

  // Conversion of the unit vector 'c' in Cartesian coordinates to vector in 
  // polar coordinates
  CartesianToPolar(c, p);

  // Conversion of the 'p' vector in polar coordinates to unit vector 'a' 
  // in Cartesian coordinates 
  PolarToCartesian(p, a);
@endcode

Functions and methods Length(const TVect3<T> &u), TVect3<T>::Normalize(), 
Cos(const TVect3<T> &a, const TVect3<T> &b), 
Sin(const TVect3<T> &a, const TVect3<T> &b), 
TVect3<T>::Project(), CrossProd(const TVect3<T> &a, const TVect3<T> &b), 
DotProd(const TVect3<T> &a, const TVect3<T> &b) allow to calculate 
length of vector, normalize the vector to unit length, calculate cos and sin of 
angle between two vectors, calculate projection of vector on other vector, 
calculate "cross" and "dot" products of two vectors.  The next two examples 
illustrate these functions and methods usage.

Let @a a and @a b are arbitrary vectors. The next illustrative example 
calculates rotation from source vector @a a to target vector @a b. Resulted 
rotation is defined by the two components:

- Rotation axis (@a w) - unit vector orthogonal to both vectors @a a and @a b. 
Order of the triple @a a, @a b, @a w has the right hand orientation.
- angle of the rotation counterclockwise (view direction is opposite to vector 
@a w).

@b Example:
@code

  Vect3d  a, b;
  // Initialization of the vectors 'a' and 'b'
  ...

  // Normalization of the vector 'a'
  double len_a = Length(a);
  a /= len_a;
  
  // Normalization of the vector 'b', the same as above but direct method used.
  b.Normalize();
  
  // Rotation axis is calculated by the Cross product. 
  Vect3d w = CrossProd(a, b);

  // Calculation of sine of angle between 'a' and 'b'. This approach uses 
  // property of the Cross product: length of the cross product is area of the 
  // parallelogram defined by the vectors - multipliers. 
  double sin_d = Length(w);

  // Other direct method of the sine calculation
  sin_d = Sin(a, b);

  // Calculation of cosine of angle between 'a' and 'b'
  double cos_d = DotProd(a, b);

  // Other direct method of cosine calculation
  cos_d = Cos(a, b);

  // Rotation angle
  double angle = ATan2(sin_d, cos_d);

@endcode

Let @a p1, @a p2, @a p3 are arbitrary points in the 3D numeric space. The next 
example calculates projection of point @a p3 to line defined by the points
@a p1 and @a p2. Note, this aproach of projection point calculation is 
illustrative only and far from optimal.

@b Example:
@code

  // Vector between points 'p1' and 'p2' is difference between two points
  Vect3d p1_p2 = p2 - p1;

  // Vector between points 'p1' and 'p3' is difference between two points
  Vect3d p1_p3 = p3 - p1;

  // Calculation of vector of projection 'p1_p3' vector to 'p1_p2' vector.
  Vect3d prj_v = p1_p3.Project(p1_p2);

  // Calculation of projection point as sum of the initial point 'p1' and 
  // vector of the projection.
  Point3d prj_p = p1 + prj_v;  

@endcode

The special class CoordSys provides conversion of vectors and points from Left
hand coordinate system to Right hand coordinate system. Conversion is realized
by the swapping Y and Z coordinates.

@b Example:
@code

  // Let 'p' is arbitrary point.
  Point3d p(-3.2, 1.3, 6.7);

  // Point 'p' is converted to point {-3.2, 6.7, 1.3}. This transformation
  // corresponding to conversion of the point from Left hand to Right hand 
  // coordinate system.
  CoordSys::Conv(p);

@endcode

Multiplication operations of vector by the matrix  and matrix by vector are 
available also and are described in the section
@ref math_working_with_matrices.

</p>

<p>
<HR>
@section math_working_with_bboxes Working with bounding boxes.
<HR>

Bounding boxes represent a closed ranges in numeric spaces:
- BBox1 - segment in number line (1D space);
- BBox2 - rectangle (sides are parallel to coordinate axes) in numeric 
plane (2D space);
- BBox3 - rectangular parallelepiped (ribs are parallel to coordinate axes) in 
numeric space (3D space).

Bounding box is defined by the lower and upper boundaries: 
- Public member vmin - point (number in 1D case) with minimal coordinates of 
the box defines lower boundary. 
- Public  member vmax - point with maximal coordinates of the box defines upper 
boundary.

There are six predefined instances for the bounding boxes. BBox1f, BBox1d,
BBox2f, BBox2d, BBox3f, BBox3d are predefined instances of the template classes 
BBox1, BBox2, BBox3 for boxes with float and double elements. 

@b Example:
@code

  // Two points 'a' and 'b' are initialized in 2D space
  Point2f a(3, -4), Point2f b(5, -2);

  // Bounding box 'c' initialized by the two points 'a' - lower boundary and 'b' 
  // - upper boundary cf. bounding box 'c' is set of the points with coordinates
  // {x, y} satisfying to the following conditions:  3 <= x <= 5, -4 <= y <= -2.
  BBox2f c(a, b);

  // It is evident that bounding box 'c' is not empty. 
  // Logical variable 'not_empty' receives 'true' value.
  bool not_empty = c.NotEmpty();

  // Bounding box 'c1' is created without initialization.
  BBox2f c1;  

  // Direct access to the lower and upper  boundaries. 
  c1.vmin = a;
  c1.vmax = b;

  // Now bounding boxes 'c' and 'c1' are equal.
  // Logical variable 'is_equal' receives 'true' value.
  bool is_equal = (c == c1);

  // Bounding box 'c2' is initialized by the one point only.
  BBox2f c2(a);  

  // Lower and upper boundaries of the 'c2' box are equal cf. it is dot.
  // Logical variable is_dot receives 'true'
  bool is_dot = c2.IsDot();

  // Boundaries by x of the 'c1' bounding box are swapped.
  Swap(c1.vmin.x, c1.vmax.x);

  // Now bounding box 'c1' is empty and logical variable 'not_empty' receives
  // 'false' value.
  not_empty = c1.NotEmpty();

@endcode

The main features of the bounding boxes are:
- Check of the inclusion of the points or bounding boxes;
- Extension to include points or bounding boxes;
- Intersections with other boxes.

@b Example:
@code

  // Bounding box 'b0' is initialized by the one point, it is dot.
  BBox3d b0(1.9, 0.0, -0.8);

  // Bounding box 'b0' is extended by the vector {1, 1, 1}. Resulted box is
  // defined by the following boundaries {0.9, -1.0, -1.8} - {2.9, 1.0, 0.2}
  b0.vmin -= Vect3d(1.0);
  b0.vmin += Vect3d(1.0);

  // Parameters of the given box

  // Diagonal {2, 2, 2} in fact it are the sizes of this box
  Vect3d d = b0.Diag();

  // Center {1.9, 0.0, -0.8}
  Point3d c = b0.Center();

  // Volume  8 = 2 * 2 * 2
  double v = b0.Volume();

  // Point 'p' is initialized by the three values
  Point3d p(1.2, -2,3, 0.5);

  // Point 'p' is external with respect to box 'b0' (second and third 
  // coordinates are out of box range). Logical variable 'inside' receives 
  // 'false' value.
  bool inside = b0.Includes(p);

  // Bounding box 'b1' is created by the copy construct
  BBox3d b1(b0);

  // Bounding box 'b0' is extended to include point 'p'. Resulted bounding box
  // has the following boundaries {0.9, -2.3, -1.8} - {2.9, 1.0, 0.5}. 
  b0.Include(p);

  // 'b0' is minimal box which includes initial box and given point.
  // Logical variable 'inside' receives  'true' value in the both cases.
  inside = b0.Includes(p);
  inside = b0.Includes(b1);

  // New bounding box is created by the two points.
  BBox3d b2(Point3d(1.0, -3.0, -0.8), Point3d(4.0, 0.5, 0.0));

  // Check that box 'b2' intersects box 'b1'. It is evident these boxes
  // have common points. Logical variable 'inters' receives  'true' value.
  bool inters = b1.Intersects(b2);

  // Common part of the both boxes 'b1' and 'b2' places to box 'b1'. Now box 
  // 'b1' has the following boundaries {1.0, -1.0, -0.8} - {2.9, 0.5, 0.0}.
  b1.Intersect(b2);  

@endcode

</p>

<p>
<HR>
@section math_working_with_bspheres Working with bounding spheres.
<HR>

Template class BSphere3  implements simple bounding sphere determined by 
center of sphere and it's radius of type T. BSphere3f and BSphere3d are
predefined instances of the template class for float and double values. 
Constructors allow to initialize spheres by the point and radius, by other 
sphere or box of the same type. The special function ConvF(), ConvD() are used
for the conwersion between spheres of the various types.

@b Example:
@code

  // Sphere creation with float components without initialization
  BSphere3f s1;

  // Sphere creation with double components with center in point {1.0, 1.0, 1.0}
  // and radius equal 2
  BSphere3d s2(Point3d(1.0), 2.0);

  // Sphere creation with double components fully bounding box 
  // {-1.0, -1.0, -1.0} - {1.0, 1.0, 1.0}
  // The center of this sphere is center of CS and radius is equal square 
  // root of 3
  BSphere3d s3(BBox3d(Point3d(-1.0), Point3d(1.0)));

  // Sphere creation from above sphere with conversion to float components
  BSphere3f s4(ConvF(s3));
@endcode

The main sphere feature is expansion to include the external points and other
spheres.

@b Example:
@code

  // Point inclusion

  // Create sphere with center is origin of CS and radius is equal 1.0
  BSphere3d s1(Point3d(0.0), 1.0);

  // Point {0.0, 2.0, 0.0} is external to respect of this sphere
  Point3d p(0.0, 2.0, 0.0);

  // Radius of this sphere is calculatedby by formula:
  // radius = (distance_point_center + radius) / 2 = (2 + 1) / 2 = 1.5 
  // and center is calculated by formula:
  // center = (far_point_on_sphere + point) / 2 = 
  // ({0, -1, 0} + {0, 2, 0}) / 2 = {0.0, 0.5, 0.0} 
  // cf. new sphere is minimal sphere which includes the all points of the old 
  // sphere and point 'p' 
  s1.Include(p);

  // Sphere inclusion

  // Create sphere with center is origin of coordinates and radius is equal 2.0
  s1 = BSphere3d(Point3d(0.0), 2.0);

  // Create sphere with center is {3.0, 0.0, 0.0} and radius is equal 2.0
  BSphere3d s2(Point3d(3.0, 0.0, 0.0), 2.0);

  // Radius of the 's1' sphere is calculated by formula:
  // radius = (radius1 + radius2 + distance_centers) / 2 = (2 + 2 + 3) / 2= 3.5 
  // and center is calculated by formula:
  // center = (far_point_sphere1 + far_point_sphere2) / 2 = 
  // ({-2, 0, 0} + {5, 0, 0}) / 2 = {1.5, 0.0, 0.0}
  // cf. new sphere is minimal sphere which includes the all points of the 
  // both spheres  
  s1.Include(s2);

@endcode
 
</p>

<p>
<HR>
@section math_working_with_matrices Working with matrices.
<HR>

Classes TMatrix2, TMatrix3, TMatrix43, TMatrix4 can be used to operate with 
matrices 2x2, 3x3, 4x3 and 4x4. Matrix2f, Matrix2d, Matrix3f, Matrix3d, 
Matrix43f, Matrix43d, Matrix4f, Matrix4d predefined instances of the template 
class for matrices  with float (double) elements. 

Constructors allow to initialize matrix by scalars (diagonal matrix), by 
separate components of the matrix and by other matrix of the same or other type.

@b Example:
@code

  // Matrix 'a2' is initialized to identity matrix
  Matrix2d a2(1.0);

  // Matrix 'b' is initialized by matrix 'a2'
  Matrix2d b = a2;

  // Matrix 'a3' is initialized by the diagonal elements.
  Matrix3d a3(-3.2, 2.5, 1.3);

  // Matrix 'a43' is initialized by the rotation part (first three rows) - 
  // matrix 'a3' and translation part (fourth row) - 3D vector.
  Matrix43d a43(a3, Vect3d(0.5, -1.4, 2.7));
  
  // Matrix 'a4' is initialized by the four rows
  Matrix4d a4(Vect4d(3.7, -8.5, 1.0, 0.0), Vect4d(-5.2, 3.5, 3.1, 0.0), 
              Vect4d(-3.2, 2.5, 1.3, 0.0), Vect4d(0.0, 0.1, 0.0, 1.0));

@endcode

Methods of the square matrix classes (TMatrix2, TMatrix3, TMatrix4) 
allow to operate all standard operations of matrix algebra with elements of the 
same types. In the case of the 4x3 matrix (class TMatrix43) along with 
admissiable operations for the non-square matrix (addition, subtraction, 
operations with scalar) the multiplication (just combination) of the two 
matrices, multiplication matrix and 3D point, inverse matrix are provided also.

@b Example:
@code
  // Let 'a' and 'b' are the arbitrary matrices of Matrix3d type then
  // matrix 'c' is sum of these matrices.
  Matrix3d c = a + b;

  // The all coordinates of matrix 'a1' are the products of the coordinates
  // of the matrix 'c' and value 3.2
  Matrix3d a1 = 3.2 * c;

  double d = 2.1;

  // The all coordinates of matrix 'a' changed by the division on value 2.1
  a /= d;

  // Matrix 'b' is product of the two matrices 'c' and 'a1'.
  b = c * a1;

@endcode

Methods Inverse, Trans, Det, and Minor allow to invert and transpose square 
matrices, calculate determinant and minor. 

@b Example:
@code
  // 3x3 matrix is initialized by the three vectors
  Matrix3d m(Vect3d( 3.0, -1.0, 0.0), Vect3d(-2.0,  1.0, 1.0), Vect3d( 2.0, -1.0, 4.0));

  // Determinant 'det' receives value 5.0
  double det = m.Det();

  // Minor (0, 2) 'min' receives value 0.0 
  double min = m.Minor(0, 2);

  // Matrix 'm1' is transposed with respect to matrix 'm' 
  //   3.0 -2.0  2.0        
  //  -1.0  1.0 -1.0  Resulted  matrix m1 
  //   0.0  1.0  4.0        
  Matrix3d m1 = m.Transposed();

  // Matrix 'm2' is inverse with respect to matrix 'm'
  //  1.0 0.8 -0.2       
  //  2.0 2.4 -0.6    Resulted  matrix m2 
  //  0.0 0.2  0.2       
  Matrix3d m2 = m.Inverse();

@endcode

Special methods of 3x3 and 4x3 matrices allow to create different matrices for
rotation, transfer (for the 4x3 matrices only) and scaling. 

@b Example:
@code

  // Matrix 3x3 'a' is created without initialization
  Matrix3d a;

  // 3D vector 'v' is initialized by the triple of values
  Vect3d v(1.0, 2.0, -1.5);

  // Matrix 'a' is modified to rotation matrix aroung axis (defined by the
  // vector 'v') at  angle 30 degrees
  (void)a.RotationAxis(v, Rad(30.0));

  // Matrix 'a' is modified to rotation matrix of the tree rotations:
  // 30 degree around Z axis;
  // 45 degree around resulted Y axis;
  // 60 degree around resulted Z axis.
  (void)EulerRotation(Rad(30.0), Rad(45.0), Rad(60.0));

  // Matrix 'a' is modified to scaling matrix.
  a.Scale(2.0, 1.5, 2.5);

@endcode

Multiplication operations of matrix by the vector and vector by matrix are 
available also. These operation are realized by the standard rules for the
square matrices. For the 4x3 matrices multiplication of 4x3 matrix by the 3D 
point from the left is realized only in the following way:
- point multiplies the 3x3 matrix of the first three rows from the left;
- resulted point is summed with 3D vector of fourth row.  

This operator is corresponding to the methods PointTransform() and 
PointTransformed(). Multiplications of 4x3 matrices by the 3D point from the 
right or by the vectors are not realized, but class provides the methods 
PointBackRotTransform(), VectorTransform() and etc. which may be used for the 
transformation of points and vectors. 

@b Example:
@code
  // Let 'a' are the 4x3 transformation matrix 
  Matrix43d a;

  // Rotation part of this matrix is rotation matrix around X axis at 30 degree
  a.Matrix3().RotationX(Rad(30.0));

  // Translation part of this matrix is arbitrary vector, for example
  // {1.0, -1.0, 1.0}
  a.GetTrans() = Vect3d(1.0, -1.0, 1.0);

  // Let 'p' is arbitrary point, for example {1.0, 1.0, 1.0}
  Point3d p(1.0);

  // Point 'p1' created by the following actions:
  // - point 'p' is rotated around X axis at angle 30 deg. by the rotation 
  // part of the matrix 'a' 
  // - resulted point is shifted by the translation part of the  matrix 'a'.
  Point3d p1 = p * a;

  // The same as above action 
  Point3d p2 = a.PointTransformed(p);

  // The same as two above actions, but in above actions the new points were
  // created, in this case the initial point 'p' is transformed by the 
  // matrix 'a'. 
  a.PointTransform(p);

@endcode
In result points @a p, @a p1 and @a p2 from this example are the same.

TMatrix43 class provides special operator '*' for the combination of the
two 4x3 matrices. This operator is realized (it is not real multiplication) so 
that combination 1_Matrix43 * 2_Matrix43 corresponds to the transformation of 
vector (point) by 1_Matrix43 at first and by 2_Matrix43 at last (evidently for 
the square matrices).

@b Example:
@code

  // Let 'a' and 'b' are the 4x3 transformation matrices, 'p' is 3D point 
  Matrix43d a, b;
  Point3d p; 

  // Above objects initialized somehow
...

  // Point 'p1' is point 'p' transformed by the matrix 'a' 
  Point3d p1 = p * a;

  // Point 'p2' is point 'p1' transformed by the matrix 'b' 
  Point3d p2 = p1 * b;

  // Matrix 4x3 'c' is combination (see comments before this example) 
  // matrices 'a' and 'b'
  Matrix43d c = a * b;

  // Point 'p3' is point 'p' transformed by the matrix 'c'. 
  Point3d p3 = p * c;
@endcode

Points @a p2 and @a p3 from this example are the same.

Class TMatrix4 provides creation perspective and orthographic projection matrix.
These matrices transform truncate pyramid visibility to canonical view volume 
(cube with sides [-1; 1]). This cube is suitable for the cutting off operations.

@b Example:
@code
  
  // Matrix 'a' is created without initialization 
  Matrix4d a;

  // Modification 'a' matrix to perspective progection matrix with parameters
  // view angle = 90 degree,
  // aspect ratio = 1.0 (size along x = size along y), 
  // distances to near and far clipping planes are 1.0 and 10.0 respectively.
  a.Perspective(Rad(90.0), 1.0, 1.0, 10.0);
  // Calculated matrix is
  //  1 0  0      0
  //  0 1  0      0
  //  0 0 -1.222 -1
  //  0 0 -2.222  0

  // Let 'p1' and 'p2' is arbitrary points
  Point3d p1(5.0, 4.0, -6.0), p2(7.0, 4.0, -6.0);

  // Convert points 'p1' and 'p2' to 4D vectors 'v1' and 'v2' with homogeneous 
  // coordinates. Coordinates of the vectors:
  // 'v1' - {5.0, 4.0, -6.0, 1.0}  'v2' - {7.0, 4.0, -6.0, 1.0}
  Vect4d v1(p1.Vector()), v2(p2.Vector());

  // Vector 'v1' is transformed by the matix 'a' to vector 'v1_t'.
  // Coordinates of the vector 'v1_t' {5, 4, 5.11, 6}. Coordinates
  // of the corresponding 3D point  {5/6, 4/6, 5.11/6}. 
  Vect4d v1_t = v1 * a;

  // Vector 'v2' is transformed by the matix 'a' to vector 'v2_t'.
  // Coordinates of the vector 'v2_t' {7, 4, 5.11, 6}. Coordinates
  // of the corresponding 3D point  {7/6, 4/6, 5.11/6}. 
  Vect4d v2_t = v2 * a;

@endcode

Since the all coordinates of the transformed point @a p1 in range [-1, 1] then
this point is visible.  Since the first coordinate of the transformed point 
@a p2 is out of range [-1, 1] then this point is not visible. Moreover visible 
part of the segment [@a p1, @a p2] may be defined also. It is segment 
[{5/6, 4/6, 5.11/6}, {1, 4/6, 5.11/6}].

The special class CoordSys provides conversion of matrices (3x3, 4x3) from Left
hand coordinate system to Right hand coordinate system. Conversion is realized
by the multiplication of the special matrix @a conv_tm from the left and from 
the rigth.

@b Example:
@code
  
  // Let 'a' is rotation matrix in Left hand coordinate system 
  Matrix3d a;

  // Matrix 'a' is initialized somehow
  ... 

  // Matrix 'b' is initialized by the matrix 'a'
  Matrix3d b(a);

  // This action transforms matrix to corresponding rotation matrix in Right
  // hand coordinate system.
  CoordSys::Conv(a);
  
  // It is rotation part of the special 4x3 matrix of the CoordSys class
  // which used for the conversion.
  Matrix3d cm(Vect3d(1, 0, 0), 
              Vect3d(0, 0, 1), 
              Vect3d(0, 1, 0)),

  // Matrix 'b' is converted by the same way as matrix 'a'
  b = cm * b * cm;

@endcode
Matrices @a a and @a b are equal in this example.

</p>


<p>
<HR>
@section math_working_with_arrays Working with arrays.
<HR>

Special class MArray was elaborated for the processing (sorting, binary search) 
arrays with elements which '==' and '<' operations may be applied to.

Let some class SomeClass of the arbitrary structure is defined. Elements of 
this class may be used as elements of the array, but for the usage MArray class 
the operators '==' and '<' must be defined.

@b Example:
@code
// Definition of the some class
class SomeClass
  {
  ...
  // Obligatory operators
  bool operator == (const SomeClass &b) const;
  bool operator < (const SomeClass &b) const;

  };

  // Create empty array with elements of the SomeClass type
  MArray<SomeClass> a;

  // Initialization of the array somewhere
...

  // Sorting array in ASCENDING order
  a.QSort();

  // Create some element of the SomeClass
  SomeClass c(...// initialization parameters); 

  // Returned index is satisfied the following conditions:
  // a[i - 1] <= c < a[i], or 0 if c < a[0], or n if c > a[n - 1], where
  // n - number of array elements
  int i = a.BinarySearch(c);
@endcode

The important feature of MArray is possibility to use arbitrary function of
the element comparison. The next example sorts array of the SomeClass elements
in DESCENDING order. It allows to use methods of the binary searching in the
array with DESCENDING order of elements.

@b Example:
@code

  // Define the some function 'less'. This function returns true if first
  // parameter less than second parameter
bool less(const SomeClass &a, const SomeClass &b)
  {
  // This function opposite to operator '<' (for the various elements)
  return (b < a);
  }

  // Sorting array in ASCENDING order concerning to function 'less' and
  // in DESCENDING order concerning to operator '<'
  a.QSort(less);

  // Create some element of the SomeClass
  SomeClass c(...// initialization parameters); 

  // Returned index is satisfied the following conditions:
  // a[i] < c <= a[i - 1], or 0 if c > a[0], or n if c <= a[n - 1], where
  // n - number of array elements
  int i = a.BinarySearchRev(c);

@endcode

</p>

<p>
<HR>
@section math_working_with_randomgen Working with random generator.
<HR>

The Rnd class provides generation of the pseudo-random values. 

The Rnd class is used in the following way. In the first step, the object of 
the Rnd class is created by the some unsigned parameter. In the next steps the
method Rnd::DRnd() (on the each call) returns the some pseudo-random value.

@b Example:
@code

  // Create object by the current time (constructor by the default uses
  // current time for the initialization of the random generator)
  Rnd rnd; // It is analog of Rnd rnd(Time::Get());

  for (...// parameters of the iterations)
    {
    // Create pseudo-random value in the segment [0, 1)
    double rnd_val = rnd.DRnd();
    
    // The some actions with pseudo-random value
    ...
  
    }

@endcode

Instead of Rnd the other Niederreiter class (inherits Rnd) may be used for the
@b quasi-random numbers generation. In this case instead of Rnd::DRnd() the 
method Niederreiter::QRnd() may be used.

The next example randomly chooses point on unit sphere uniformly distributed on 
sphere.

@b Example:
@code

  // Quasi-random generator (initialized by the current time by the default)
  Niederreiter rnd;

  // Point is created without initialization.
  Point3d p;

  // z coordinate of the point is randomly choosed in segment [-1, 1). 
  double z = rnd.QRnd() * 2 - 1;

  // z coordinate is sine of theta angle (Latitude)
  double cos_theta = Sqrt(1.0 - z * z);

  // phi value is randomly choosed in segment [0, 2pi) (Longitude) 
  double phi = rnd.QRnd() * PI2;
  
  // Conversion spherical coordinates of the point to cartesian coordinates.
  p.x = cos_theta * cos(phi);
  p.y = cos_theta * sin(phi);
  p.z = z;

@endcode

Another quasi-random generator for low-dimensional integration in 2D is
available with HaltonGen2D class. It is quite effective generator which
allows generation of samples by coherent groups.

The next example quasi- randomly generates points in rectangle uniformly
distributed in each of 5 increments. Points are generated by coherent
groups with 6 samples in each group.

@b Example:
@code

  // Halton quasi-random generator
  HaltonGen2D qrng;
  int span = 2 * 3;  // coherency level: more level -> close coherent points
  int group = 2 * 3; // coherency group: number of coherent points in group
  int len = span * group;  // minimal complete sequence (first increment)
  int prev_len = 0;

  for (int incr = 1; incr <= 5; incr++)
    {
    // specify total length including all increments
    qrng.SetLength(span, len);

    for (int i = prev_len; i < len; i += group)
      for (int j = 0; j < group; j++)
        {
        double qx, qy;
        qrng.GetNext(qy, qx);
        int ix = (int)(qx * image.GetXRes());
        int iy = (int)(qy * image.GetYRes());
        image.Red(iy, ix) = image.Green(iy, ix) = image.Blue(iy, ix) = 255;
        }

    prev_len = len;
    len *= 6; // new increment length is at least 2*3 longer
    } // for each increment

@endcode


</p>

**/