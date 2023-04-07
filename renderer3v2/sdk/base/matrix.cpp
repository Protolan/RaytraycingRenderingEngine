/// @internal
/// @file
///
/// @brief Definition of TMatrix template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TMatrix base/matrix.hpp

Two-dimensional matrix of arbitrary elements with basic operations.

The template class TMatrix represents a (large) 2D matrix of objects of
arbitrary nature. For small matrices, the classes TMatrix3x3 are recommended.

The size of the matrix can change dynamically, optionally retaining data.
Insertion operators are supported. Resizing and insertion methods
change addresses of elements so the user must not rely on addresses of
array elements after those operations.

The element in the i-th row j-th
column can be addressed as either A(i,j) or A[i][j] and occupies
the position 'i * ncols + j' in the linear array. A[i] gives a pointer
to the array starting with the i-th row.

Internally TMatrix is a linear row wrapped array; so cycling within
a row is much faster then cycling over rows. Conversion to a C-style
linear row-wrapped array is provided, so TMatrix can be used with
legacy algebra routines.

Unlike TArray, there is no "block size" once insertion of an element
all the same requires addition of a row or a column, that is a large block
of memory.

@param[in] T - A type of the elements stored in the matrix.
The type must provide a default constructor and a copy constructor.
Elements are destroyed by calling the destructor of type T if any.

@sa @ref base_mainpage
**/
