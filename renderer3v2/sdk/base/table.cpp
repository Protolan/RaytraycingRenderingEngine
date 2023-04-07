/// @internal
/// @file
///
/// @brief Definition of TTable template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TTable base/table.hpp

Dynamic table of elements of an arbitrary type.

Alternative realization of the array of the pointers
on the arrays with changeable length of the arbitrary
type elements.

@param[in] T Type of the elements which will be stored in the table.
The type must provide default constructor and copy constructor.

This class realizes the following data.
Let we have set of the some cells (for example, cells of
the calculation grid,  voxels etc.). The each cell contains
arbitrary number of the some elements (for example triangles
of some triangulation mesh). One need to add a new element
to the cell  or remove some element from the cell. Empty cells
are possible also. Such data may be represented as array of
the cells where each cell is array of the elements i.e. as
two-dimensional array. But usage of two-dimensional array
leads to great memory de-fragmentation.

Alternative way is
storage of all elements in one main linear array  (array elem)
with two secondary arrays of integers (arrays head and next).

The length of the first array (head) is equal to number of cells.
Each element of this array is equal to
- either position (index) of the first element of this cell
in the main array elem,
- or 0 if this cell is empty.

The length of the second array (next) is equal to the length
of the main array elem. Each element of this array is equal to
- either position of the next element of the current cell in
the main array,
- or 0 if current element is last element,

i.e. if to get position
of the first element of this cell from the first array we can get
position of the next element of this cell from second array and so on.

It is necessary to note that first element of the main array
elem with index 0 is undefined.

@sa @ref base_mainpage
**/
