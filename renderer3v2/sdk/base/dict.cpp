/// @internal
/// @file
///
/// @brief Definition of TDict template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TDict base/dict.hpp

Dynamic dictionary of elements of an arbitrary type.

Template class TDict organizes keeping elements and access to
them by the specified key (sign): dictionary, table, list etc.
Each element of the TDict is pair of  the  KEY (sign of
element) and  VALUE (data of element).

@param[in] KEY Type of the elements to be used as keys.
Type must provide copy constructor and "operator ==()".

@param[in] VALUE Type of the elements to be used as values.
Type must provide copy constructor.

@sa @ref base_mainpage
**/
