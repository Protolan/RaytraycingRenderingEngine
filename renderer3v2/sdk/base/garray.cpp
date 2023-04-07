/// @internal
/// @file
///
/// @brief Definition of GArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class GArray base/garray.hpp

Dynamic array elements of an arbitrary type, which may be compared for equality.

This derived class provides the Find() method and array comparison.

@param[in] T - A type of the elements stored in the array.
In addition to the TArray<T> requirements, the element type T must provide
"operator ==()".

@sa @ref base_mainpage
**/
