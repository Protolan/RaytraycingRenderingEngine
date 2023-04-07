//////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief  A cell of a rectangular 1D - 4D mesh, equppped with ability
///         to interpolate arbitrary function specified at its vertices.
///         The cell is in fact the point where the function is to be
///         interpolated plus indices of the mesh cell in the mesh. The
///         point is specified via normalized offsets from the "lower left"
///         vertex which are just weights for interpolation.
///
/// @author Sergey Ershov, KLBM, '03.04.25.
///
/// Copyright &copy; INTEGRA, Inc., 2003.
//////////////////////////////////////////////////////////////////////////////
#ifndef _KLBM_MESHCELL_HPP
#define _KLBM_MESHCELL_HPP

#include <math/math.hpp>

INTEGRA_NAMESPACE_START

// ----------------------------------------------------------------------------
// TODO: The inner data can be represented
// as MeshCell = Vector<dim, MeshCell1D>
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////
/// @class MeshCell1D math/meshcell.hpp
///
/// @brief Conveys information about position of a point in 1D mesh
///        in the form most optimal for interpolation within mesh cell.
///
/// It describes position of a point @e x on one-dimensional mesh as the
/// index @b i of mesh cell containing the point, and a normalized offset
/// from the left cell end @b c which defines position of the point inside
/// the cell. These parameters are "ready for interpolation" of a linear
/// function inside the cell
///
/// Namely, @e x is between mesh knots x[i], x[i+1] and
/// @e c = (x-x[i])/(x[i+1]-x[i]).
//////////////////////////////////////////////////////////////////////////////
class MeshCell1D
  {
  public:
    /// destructor
    ~MeshCell1D() {  };

    /// @brief constructs non-initialised object: cell with index 0 and point
    /// position 0.0 (= at left cell vertex)
    MeshCell1D() : i(0), c(0) {  };


  public:
    /// zero-based index of the left cell knot in the mesh
    int i;

    /// @brief normalized offset from the left knot, i.e. interpolation weight
    ///  c = (x-x[i])/(x[i+1]-x[i]);
    double c;
  }; // end class MeshCell1D

//////////////////////////////////////////////////////////////////////////////
/// @class MeshCell2D math/meshcell.hpp
///
/// @brief Conveys information about position of a point in 2D mesh
///        in the form most optimal for interpolation within mesh cell.
///
/// It describes position of a point @e x,y on a 2D mesh as the indices
/// <B>(i,j)</B> of mesh cell containing the point, and normalized
/// offsets from the left bottom vertex (i.e. the one with smallest
/// coordinates) of the cell <B>cx,cy</B> which define position of the
/// point inside the cell. These parameters are "ready for interpolation"
/// of a linear function inside the cell
///
/// Namely, @e x is between mesh knots x[i], x[i+1], @e y is between
/// mesh knots y[j], y[j+1], and
/// @e cx = (x-x[i])/(x[i+1]-x[i]).
/// @e cy = (y-y[j])/(y[j+1]-y[j]).
//////////////////////////////////////////////////////////////////////////////
class MeshCell2D
  {
  public:
    /// destructor
    ~MeshCell2D() {  };

    /// constructs non-initialised object: cell with index 0 and point position 0
    MeshCell2D(): i(0), j(0), cx(0), cy(0) {  };

    /// @brief constructs 2D cell from its projections
    /// @param xcell - projection onto x-axis, i.e. mesh line <B>y=const</B>
    /// @param ycell - projection onto y-axis, i.e. mesh line <B>x=const</B>
    MeshCell2D(const MeshCell1D &xcell, const MeshCell1D &ycell)
      {
      i = xcell.i;
      j = ycell.i;
      cx = xcell.c;
      cy = ycell.c;
      }

    /// @brief Access projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto; "0" is for x-axis, "1"
    ///            is for y-axis. DEBUG version asserts that 0<=n<2.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    const MeshCell1D& operator[](int n) const
      {
      ASSERT(n >= 0 && n < 2);
      return ((MeshCell1D *)&i)[n];
      }

    /// @brief Access projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto; "0" is for x-axis, "1"
    ///            is for y-axis. DEBUG version asserts that 0<=n<2.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    MeshCell1D& operator[](int n)
      {
      ASSERT(n >= 0 && n < 2);
      return ((MeshCell1D *)&i)[n];
      }

  public:
    /// zero-based index of mesh interval in x-direction
    int    i;
    /// @brief normalized offset in x-direction i.e. interpolation weight in x
    ///  cx = (x-x[i])/(x[i+1]-x[i]);
    double cx;

    /// zero-based index of mesh interval in y-direction
    int    j;
    /// @brief normalized offset in y-direction i.e. interpolation weight in y
    ///  cy = (y-y[j])/(y[j+1]-y[j]);
    double cy;
  }; // end class MeshCell2D

//////////////////////////////////////////////////////////////////////////////
/// @class MeshCell3D math/meshcell.hpp
///
/// @brief Conveys information about position of a point in 3D mesh
///        in the form most optimal for interpolation within mesh cell.
///
/// It describes position of a point (<I>x,y,z</I>) on a 3D mesh as the
/// indices <B>(i,j,k)</B> of mesh cell containing the point, and
/// normalized offsets from the left bottom vertex (i.e. the one with
/// smallest coordinates) of the cell <B>cx,cy,cz</B> which define
/// position of the point inside the cell. These parameters are "ready
/// for interpolation" of a linear function inside the cell
///
/// Namely, @e x is between mesh knots x[i], x[i+1], @e y is between
/// mesh knots y[j], y[j+1], @e z is between mesh knots z[k], k[k+1], and
/// @e cx = (x-x[i])/(x[i+1]-x[i])
/// @e cy = (y-y[j])/(y[j+1]-y[j])
/// @e cz = (z-z[k])/(z[k+1]-z[k])
//////////////////////////////////////////////////////////////////////////////
class MeshCell3D
  {
  public:
    /// destructor
    ~MeshCell3D(){  };

    /// @brief constructs non-initialised object: cell with index 0 and point
    /// position 0 (at the left bottom vertex)
    MeshCell3D() : i(0), j(0), k(0), cx(0), cy(0), cz(0) {  };

    /// @brief constructs 3D cell from its projections
    /// @param xcell - projection onto x-axis
    /// @param ycell - projection onto y-axis
    /// @param zcell - projection onto z-axis
    MeshCell3D(const MeshCell1D &xcell, const MeshCell1D &ycell,
               const MeshCell1D &zcell)
      : i(xcell.i), j(ycell.i), k(zcell.i),
        cx(xcell.c), cy(ycell.c), cz(zcell.c) { };

    /// @brief Access projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto; "0" is for x-axis,
    ///            "1" is for y-axis, "2" is for z-axis.
    ///            DEBUG version asserts that 0<=n<3.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    const MeshCell1D& operator[](int n) const
      {
      ASSERT(n >= 0 && n < 3);
      return ((MeshCell1D *)&i)[n];
      }

    /// @brief Access 1D projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto; "0" is for x-axis,
    ///            "1" is for y-axis, "2" is for z-axis.
    ///            DEBUG version asserts that 0<=n<3.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    MeshCell1D& operator[](int n)
      {
      ASSERT(n >= 0 && n < 3);
      return ((MeshCell1D *)&i)[n];
      }

    /// @brief Access 1D projections of cell onto coordinate axes
    /// @param n - defines coordinate plane: 0 means (x,y) plane and 1 means
    ///            (y,z) plane. DEBUG version asserts that 0<=n<2.
    /// @return two-dimensional projection
    /// @note implementation is like in like TVect3:
    const MeshCell2D& Get2D(int n) const
      {
      ASSERT(n >= 0 && n < 2);
      return *(MeshCell2D *)(((MeshCell1D *)&i)+n);
      }

    /// @brief Access 2D projections of cell onto coordinate planes
    /// @param n - defines coordinate plane: 0 means (x,y) plane and 1 means
    ///            (y,z) plane. DEBUG version asserts that 0<=n<2.
    /// @return two-dimensional projection
    /// @note implementation is like in like TVect3:
    MeshCell2D& Get2D(int n)
      {
      ASSERT(n >= 0 && n < 2);
      return *(MeshCell2D *)(((MeshCell1D *)&i) +n);
      }

  public:
    /// zero-based index of mesh interval in x-direction
    int    i;
    /// @brief normalized offset in x-direction i.e. interpolation weight in x
    ///  cx = (x-x[i])/(x[i+1]-x[i]);
    double cx;

    /// zero-based index of mesh interval in y-direction
    int    j;
    /// @brief normalized offset in y-direction i.e. interpolation weight in y
    ///  cy = (y-y[j])/(y[j+1]-y[j]);
    double cy;

    /// zero-based index of mesh interval in z-direction
    int    k;
    /// @brief normalized offset in z-direction i.e. interpolation weight in z
    ///  cz = (z-z[k])/(z[k+1]-z[k]);
    double cz;
  }; // end class MeshCell3D




//////////////////////////////////////////////////////////////////////////////
/// @class MeshCell4D math/meshcell.hpp
///
/// @brief Conveys information about position of a point in 4D mesh
///        in the form most optimal for interpolation within mesh cell.
///
///        It describes position of a point (<I>x,y,z,w</I>) on a 4D mesh as
///        the indices <I>(i,j,k,l)</I> of mesh cell containing the point, and
///        normalized offsets from the left bottom vertex (i.e. the one with
///        smallest coordinates) of the cell <B>cx,cy,cz,cw</B> which define
///        position of the point inside the cell. These parameters are "ready
///        for interpolation" of a linear function inside the cell
///
///        Namely, @e x is between mesh knots x[i], x[i+1], @e y is between
///        mesh knots y[j], y[j+1], @e z is between mesh knots z[k], k[k+1],
///        @e w is between mesh knots w[l], k[l+1], and
///        @e cx = (x-x[i])/(x[i+1]-x[i])
///        @e cy = (y-y[j])/(y[j+1]-y[j])
///        @e cz = (z-z[k])/(z[k+1]-z[k])
///        @e cw = (w-w[l])/(w[l+1]-w[l])
//////////////////////////////////////////////////////////////////////////////
class MeshCell4D
  {
  public:
    /// destructor
    ~MeshCell4D() {  };

    /// @brief constructs non-initialised object: cell with index 0 and point
    /// position 0 (at left bottom vertex)
    MeshCell4D() : i(0), j(0), k(0), l(0), cx(0), cy(0), cz(0), cw(0) {  };

    /// @brief constructs 4D cell from its 1D projections
    /// @param xcell - projection onto x-axis
    /// @param ycell - projection onto y-axis
    /// @param zcell - projection onto z-axis
    /// @param wcell - projection onto w-axis
    MeshCell4D(const MeshCell1D &xcell, const MeshCell1D &ycell,
               const MeshCell1D &zcell, const MeshCell1D &wcell)
      : i(xcell.i), j(ycell.i), k(zcell.i), l(wcell.i),
        cx(xcell.c), cy(ycell.c), cz(zcell.c), cw(wcell.c) {};

    /// @brief constructs 4D cell from its 2D projections
    /// @param xycell - projection onto (x,y) plane
    /// @param zwcell - projection onto (z,w) plane
    MeshCell4D(const MeshCell2D &xycell, const MeshCell2D &zwcell)
      : i(xycell.i), j(xycell.j), k(zwcell.i), l(zwcell.j),
        cx(xycell.cx), cy(xycell.cy), cz(zwcell.cx), cw(zwcell.cy) {};


    /// @brief Access 1D projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto;
    ///            "0" is for x-axis, "1" is
    ///            for y-axis, "2" is for z-axis and "3" is for w-axis.
    ///            DEBUG version asserts that 0<=n<4.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    const MeshCell1D& operator[](int n) const
      {
      ASSERT(n >= 0 && n < 4);
      return ((MeshCell1D *)&i)[n];
      }

    /// @brief Access 1D projections of cell onto coordinate axes
    /// @param n - index of axis which we project onto; "0" is for x-axis,
    ///            "1" is for y-axis, "2" is for z-axis and "3" is for w-axis.
    ///            DEBUG version asserts that 0<=n<4.
    /// @return One dimensional projection onto n-th axis
    /// @note implementation is like in like TVect3:
    MeshCell1D& operator[](int n)
      {
      ASSERT(n >= 0 && n < 4);
      return ((MeshCell1D *)&i)[n];
      }

    /// @brief Access 2D projections of cell onto coordinate planes
    /// @param n - defines coordinate plane: 0 means (x,y) plane, 1 means (y,z)
    ///            plane and 3 means (z,w) plane.
    ///            DEBUG version asserts that 0<=n<3.
    /// @return two-dimensional projection
    /// @note implementation is like in like TVect3:
    const MeshCell2D& Get2D(int n) const
      {
      ASSERT(n >=0 && n <3);
      return *(MeshCell2D *)(((MeshCell1D *)&i) + n);
      }

    /// @brief Access 2D projections of cell onto coordinate planes
    /// @param n - defines coordinate plane: 0 means (x,y) plane, 1 means (y,z)
    ///            plane and 3 means (z,w) plane.
    ///            DEBUG version asserts that 0<=n<3.
    /// @return two-dimensional projection
    /// @note implementation is like in like TVect3:
    MeshCell2D& Get2D(int n)
      {
      ASSERT(n >=0 && n <3);
      return *(MeshCell2D *)(((MeshCell1D *)&i) + n);
      }

    /// @brief Access 3D projections of cell onto 3D subspaces
    /// @param n - defines subspace: 0 means (x,y,z), 1 means (y,z,w)
    ///            DEBUG version asserts that 0<=n<2.
    /// @return three-dimensional projection
    /// @note implementation is like in like TVect3:
    const MeshCell3D& Get3D(int n) const
      {
      ASSERT(n >=0 && n <2);
      return *(MeshCell3D *)(((MeshCell1D *)&i) + n);
      }

    /// @brief Access 3D projections of cell onto 3D subspaces
    /// @param n - defines subspace: 0 means (x,y,z), 1 means (y,z,w)
    ///            DEBUG version asserts that 0<=n<2.
    /// @return three-dimensional projection
    /// @note implementation is like in like TVect3:
    MeshCell3D& Get3D(int n)
      {
      ASSERT(n >=0 && n <2);
      return *(MeshCell3D *)(((MeshCell1D *)&i) + n);
      }


  public:
    /// zero-based index of mesh interval in x-direction
    int    i;
    /// @brief normalized offset in x-direction i.e. interpolation weight in x
    ///  cx = (x-x[i])/(x[i+1]-x[i]);
    double cx;

    /// zero-based index of mesh interval in y-direction
    int    j;
    /// @brief normalized offset in y-direction i.e. interpolation weight in y
    ///  cy = (y-y[j])/(y[j+1]-y[j]);
    double cy;

    /// zero-based index of mesh interval in z-direction
    int    k;
    /// @brief normalized offset in z-direction i.e. interpolation weight in z
    ///  cz = (z-z[k])/(z[k+1]-z[k]);
    double cz;

    /// wero-based index of mesh interval in w-direction
    int    l;
    /// @brief normaliwed offset in w-direction i.e. interpolation weight in w
    ///  cw = (w-w[l])/(w[l+1]-w[l]);
    double cw;
  }; // end class MeshCell4D

INTEGRA_NAMESPACE_END
#endif // #define _KLBM_MESHCELL_HPP
// END FILE
