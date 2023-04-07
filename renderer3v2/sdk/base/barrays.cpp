/// @internal
/// @file
///
/// @brief Definition of BArray, SArray, Array template classes.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @author Eed - Eugene Denisov, '04.12.14

/**
@class BArray base/barrays.hpp

Base class for SArray and Array.

This class provides access to elements with index range check,
query of their number (=array length); copying of arrays etc.
It has @b no mechanism of memory allocation, and memory block
must be provided by derived classes.

Despite operations are <b>not virtual</b>, to avoid performance
penalty this class effectively works as an abstract base.

@param[in] T - The type of array element.

@internal
This class is supposed to become base class of all arrays.
Although so far TArray etc. are not derived from this class because
it is not yet quite settled and some (I hope minor) changes are
anticipated, it is just planned to become the base of all arrays.

@sa @ref base_mainpage
**/

/**
@class SArray base/barrays.hpp

Slice array.

This class represents a "slice" array which "links" to external memory;
it implements BArray and allows creation of objects.
These thus provide access to already allocated memory block as an array.

This class is mainly intended for two purposes:
- Provide access to a subset in other array or matrix, for example,
  the row of a matrix is physically just a (contiguous) segment in
  its memory block and the matrix class can return it "wrapped" into a
  protected slice array.
- Splitting of workspace. Sometimes one needs that a class object do
  some operation as @b const; thus the workspace must be provided
  externally. For different operations it may have different length
  and type but allocating it "specifically" each time may prove
  @b too expensive. Then one may once allocate the "maximal" memory
  block and then "slice" it into workspace arrays as needed using 
  this class.

So the main feature of this class is very fast creation of an object as
contrary to a Plug reference because in retrieving matrix rows we can not
afford even this overhead. As a result, a slice array is less safe than a plug
reference and <B>the caller must care that the source memory block not be
destroyed while its slice is in use</B>. In other words, SArray objects are
for a @b local use; when one needs "long life" objects, a Plug reference must
be preferred as a safer one.

@param[in] T - The type of array element.

@sa @ref base_mainpage
**/

/**
@class Array base/barrays.hpp

Array which allocates an own memory block.

This class implements BArray  and allows to create objects and
resize the array.

So far this class is provided mainly as an example of design.

@param[in] T - The type of array element.

@sa @ref base_mainpage

@internal
This class is temporary and in future must gone with TArray be derived
from BArray.
**/
