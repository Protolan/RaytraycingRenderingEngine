/// @file
///
/// @brief Declaration of GArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '01.04.24

#ifndef _KLBC_GARRAY_HPP_
#define _KLBC_GARRAY_HPP_

#include "arrays.hpp"

INTEGRA_NAMESPACE_START

/// @brief Dynamic array of elements of an arbitrary type, which may
/// be compared for equality.
template <class T>
class GArray : public TArray<T>
  {
  public:
    /// @name Constructors
    //@{
    /// Default constructor.
    explicit GArray(int the_block_size = TArray<T>::DEF_BLOCK_SIZE);
    /// Constructor from the given values.
    inline GArray(const T *val, int length,
                  int the_block_size = TArray<T>::DEF_BLOCK_SIZE);
    /// Copy constructor.
    GArray(const GArray<T> &sour);
    //@}

  public:
    /// @name Comparison
    //@{
    /// Comparison of arrays.
    bool operator ==(const GArray<T> &src) const;
    //@}

  public:
    /// @name Search
    //@{
    /// Find an element.
    bool Find(const T &elem, int *pos = NULL) const;
    //@}
  };  // class GArray

//////////////////////////////////////////////////////////////////////////////
// Methods of the class GArray

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Initialization by default: the area of the array is set to NULL,
/// the size and the length are set to zero, the block size is set to the
/// parameter.
/// @param[in] the_block_size - A size of the memory block, in elements;
/// must be > 0, debug version asserts it.
template <class T>
GArray<T>::GArray(int the_block_size) : TArray<T>(the_block_size)
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
/// @param[in] val The given data array.
/// @param length Length of the given data array.
/// @param[in] the_block_size  Size of the memory block, in elements;
/// @note Is array succesfully constructed or not - can be controlled
/// via the Length() of the array, which is 0, if data allocation failed.
template <class T>
GArray<T>::GArray(const T *val, int length, int the_block_size)
  : TArray<T>(val, length, the_block_size)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] sour - A source object of the class.
template <class T>
GArray<T>::GArray(const GArray<T> &sour) : TArray<T>(sour)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Arrays comparison.
///
/// The method compares two arrays on the per-element basis.
/// @param[in] src An array to compare with.
/// @return @b true, if all the elements are correspondently equal;
/// @b false otherwise.
template <class T>
bool GArray<T>::operator ==(const GArray<T> &src) const
  {
  if (this->count != src.count)
    return false;
  for (int i = 0; i < this->count; i++)
    if (!(this->data[i] == src.data[i]))
      return false;
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find an element.
///
/// The method searches for the element in the array.
/// @param[in] elem - An element to find.
/// @param[out] pos - The position of the found element; pass NULL if not
/// necessary.
/// @return @b YES, if the element was found, or @b NO otherwise.
///  @note If @b YES, and @b pos is not NULL, *pos is set to the
/// (zero-based) position of the found element.
template <class T>
bool GArray<T>::Find(const T &elem, int *pos) const
  {
  int len = this->Length();
  for (int i = 0; i < len; i++)
    {
    if (elem == this->operator[](i))
      {
      if (pos != NULL)
        *pos = i;
      return YES;
      }
    }
  return NO;
  }

INTEGRA_NAMESPACE_END
#endif
