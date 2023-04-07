/// @internal
/// @file
///
/// @brief Definition of TStack template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TStack base/stack.hpp

Dynamic stack of elements of an arbitrary type.

The template class TStack provides objects keeping in a LIFO (Last input -
First output) stack of elements of generic type T. The number of elements
of the TStack can vary.

@param[in] T - The type of elements, which will be stored in the stack.
The type must provide a default constructor and a copy constructor.

The constructor of type T (default or explicit) is called to create
stack elements.
Elements are destroyed by calling the destructor of type T if any.

The user may not rely on addresses of stack elements as they can be
reallocated in memory by some TStack's methods.

Memory is (re)allocated by blocks - a minimal number of new
placeholder elements are added when the stack area is expanded.

@sa @ref base_mainpage

@internal
The current implementation of TStack object is based on TArray class.
**/
