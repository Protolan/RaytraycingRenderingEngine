/// @internal
/// @file
///
/// @brief Definition of TBArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/** 
@class TBArray base/barray.hpp

Dynamic array of elements of an arbitrary type.

The template class TBArray provides objects keeping an ordered set
(vectors, lists) of elements of generic type T. The number of
elements of TBArray is variable.

The main difference between TArray and TBarray is that TBArray
stores data in separate blocks, i.e. memory is allocated by blocks,
as a 2D-array, while TArray use single continuous memory block
(allocated by fixed-size portions).

Use TBArray instead of TArray when memory fragmentation is critical,
i.e. for large arrays.
Usage of TBArray for small arrays is not recommended.
Since TBArray works as a 2D array, it provides a slower access. 

Interface of TBArray is similar to the interface of TArray,
however the sense of parameters is quite different. 

@param[in] T - The type of the elements which will be stored
in the array.  The type must provide a default constructor and a copy 
constructor.

@note The constructor of type T (the default or explicit one) is called to 
create array
elements.  Elements are destroyed by calling the destructor of type T if any.

@sa @ref base_mainpage

@internal
The current implementation of a TBArray object keeps a pointer to
dynamic area where all the elements are kept in order. The
length of the array (the current number of elements in the
array) and size of the area (how many elements it may contain
at the moment) are kept as other members. Finally, a block size
is kept for reallocation purposes (minimal number of new
placeholder elements added when the array area is expanded).
**/
