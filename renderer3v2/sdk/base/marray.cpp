/// @internal
/// @file
///
/// @brief Definition of MArray template class
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @author Pnd - Nikolay Derjabin<BR>
///         Rkd - Kirill Dmitriev<BR>
///         Ose - Sergey Ershov
///

/**
@class MArray base/marray.hpp

Dynamic array of elements of an arbitrary type,
for which '==' and '<' operations are applicable.
MArray class provides array sort and fast binary search.

Special class MArray was elaborated for the processing (sorting, binary search)
arrays with elements, for which '==' and '<' operations are applicable.
Moreover this class provides sorting arrays with arbitrary elements (without
'==' and '<' operators). In this case the special functions less() may be used
instead of comparison operators.

@note MArray class adds no own member variables.

@param T - Base element type, must provide proper "==" and "<" operators
(obligatory for the binary search).

*/

