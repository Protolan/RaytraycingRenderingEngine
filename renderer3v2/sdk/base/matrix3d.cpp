/// @internal
/// @file
///
/// @brief Definition of TMatrix3D template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TMatrix3D base/matrix3d.hpp

Typified three-dimensional matrix A(i,j,k).

This class defines a 3D NxMxK matrix of arbitrary elements, stored
in a "linear" array row by row; the element can be accessed as A(i,j,k)
or A(i,j)[k], with A(i,j) returning the row through a special "linked" array

Provides access to elements, various assignments, allocation of memory, etc.
@param[in] T - Type of elements stored.

@sa @ref base_mainpage
**/
