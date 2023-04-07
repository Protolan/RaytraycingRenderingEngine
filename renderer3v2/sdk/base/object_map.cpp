/// @file
///
/// @brief ObjectMap class definition.
///
/// Copyright &copy; INTEGRA, Inc., 2014.
///
/// @internal
/// @author Pnd - Nick Deryabin, '14.04.07

/**

@class ObjectMap base/object_map.hpp

Fast map of objects to indices.

The class provides a limited STL-based implementation of a map.
Public intrface of the class has no STL specifics.

Usage:

@code
ObjectMap<Plug> obj_map;
...
Plug *obj1 = ...;
obj_map.AddObj(obj1, 1);
Plug *obj2 = ...;
obj_map.AddObj(obj2, 2);
...
int index;
bool found = obj_map.Find(obj, &index);
@endcode

**/

