/// @internal
/// @file
///
/// @brief Definition of TArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class TArray base/arrays.hpp

Dynamic array of elements of an arbitrary type.

The template class TArray provides objects keeping an ordered set
(vectors, lists) of elements of generic type T. The number of
elements of the TArray is variable.

@param[in] T - Type of the elements which will be stored in the array.
The type must provide a default constructor and a copy constructor.

@note The constructor of type T (default or explicit one) is called to create
array elements.  Elements are destroyed by calling the destructor
of type T if any.

The user may not rely on addresses of array elements as they
can be reallocated in memory by certain TArray's methods.

The current implementation of the TArray object keeps a pointer to
dynamic area where all the elements are kept in order. The
length of the array (the current number of elements in the
array) and size of the area (how many elements it may contain
at the moment) are kept as other members. Finally, a block size
is kept for reallocation purposes (minimal number of new
placeholder elements added when the array area is expanded).

@section base_tarray_serialization Serialization of Arrays

An empty array is assumed by default.
To serialize an array, use the following code:

@code
   arr.BegChunk(inout, "MyArray");
   if (arr.Length() > 0)
     // Serialize array elements here using
     // lower level methods of the serializer
   arr.EndChunk(inout);
@endcode

This serializes the array @b arr as a chunk with the name "MyArray"
via the Serializer @b inout. Empty arrays are treated as defaults
and are not written to serialization media.

Example:

This is how an array @b val of type @b Vect3d is serialized in
@b Vect3dArr field object:

@code
void Vect3dArr::Serialize(Serializer &inout)
  {
  val.BegChunk(inout, "Val");
  if (val.Length() > 0)
    inout.Value(&val[0][0], val.Length() * 3);
  val.EndChunk(inout);
  }
@endcode

@sa @ref base_mainpage
**/
