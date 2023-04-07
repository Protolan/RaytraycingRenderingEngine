/// @internal
/// @file
///
/// @brief Definition of GTable template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class GTable base/gtable.hpp

Dynamic table of elements, which may be compared for equality.

This derived class provides the Find() method and table comparison.

@param[in] T - A type of the elements stored in the table.
In addition to the TTable<T> requirements, the type must provide
"operator ==()".

@sa @ref base_mainpage
**/
