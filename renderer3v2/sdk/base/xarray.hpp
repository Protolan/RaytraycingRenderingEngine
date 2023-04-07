/// @file
///
/// @brief Declaration of XArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2012.
///
/// @internal
/// @author Eed - Eugene Denisov, '12.05.03

#ifndef _KLBC_XARRAY_HPP_
#define _KLBC_XARRAY_HPP_

#include "base.hpp"
#include "iarray.hpp"
#include "serializer.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic large array of elements of an arbitrary type.
template <class T>
class XArray : public IArray
  {
  public:
    /// @name Constants
    //@{
    /// Default block size for the array.
    enum
      {
      /// Default block size for the array.
      DEF_BLOCK_SIZE = 10
      };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    explicit XArray(SIZE_T the_block_size = DEF_BLOCK_SIZE);
    /// Constructor from the given values.
    inline XArray(const T *val, SIZE_T length, SIZE_T the_block_size = DEF_BLOCK_SIZE);
    /// Copy constructor.
    XArray(const XArray<T> &sour);
    /// Destructor.
    inline virtual ~XArray();
    //@}

  public:
    /// @name Access to elements
    //@{
    /// Get a reference to an array element.
    inline T &operator [](SIZE_T pos);
    /// Get a const reference to an array element.
    inline const T &operator [](SIZE_T pos) const;
    /// Get a pointer to the array for reading.
    inline const T *Data() const;
    /// Get a pointer to the array for reading and writing.
    inline T *Data();
    //@}

  public:
    /// @name Length and sizes
    //@{
    /// Get the number of used elements.
    inline SIZE_T Length() const;
    /// Get the size occupied by the array.
    inline SIZE_T Size() const;
    /// Get block size of the array.
    inline SIZE_T BlockSize() const;
    /// Set a new block size.
    inline void SetBlockSize(SIZE_T blsize);
    //@}

  public:
    /// @name Addition of elements
    //@{
    /// Add a new element to the end of the array.
    OKAY Add(const T &elem);
    /// Add a new elements to the end of the array.
    OKAY Append(const T *pelem, SIZE_T len = 1);
    /// Insert a new elements to the specified position.
    OKAY Insert(const T *pelem, SIZE_T pos, SIZE_T len = 1);
    /// Put a new element to the specified position.
    OKAY Put(const T &elem, SIZE_T pos);
    //@}

  public:
    /// @name Removal of elements
    //@{
    /// Exclude a number of elements starting from the specified position.
    void Exclude(SIZE_T pos, SIZE_T len = 1);
    /// Exclude one element at the specified position.
    void Remove(SIZE_T pos);
    //@}

  public:
    /// @name Size and length change
    //@{
    /// Decrease the length of the array.
    inline void Truncate(SIZE_T new_count = 0);
    /// Change the size of the array.
    OKAY Resize(SIZE_T new_size = 0);
    /// Change (expand) length of the array.
    OKAY Allocate(SIZE_T new_len);
    /// Change length of the array.
    OKAY SetLength(SIZE_T new_len);
    /// Change (expand) length of the array.
    OKAY Grow(SIZE_T new_len);
    //@}

  public:
    /// @name Swap arrays
    //@{
    /// Fast swap of arrays.
    static void SwapArrays(XArray<T> &a, XArray<T> &b);
    //@}

  public:
    /// @name Copying, assignment
    //@{
    /// Copy the array.
    OKAY Copy(const XArray<T> &sour);
    /// Permute the array.
    OKAY Permute(const SIZE_T *perm);
    /// Assignment operator.
    XArray<T> &operator =(const XArray<T> &sour);
    /// Set array size to Length(sour) and copy only used array part.
    OKAY SetArray(const XArray<T> &sour);
    /// Set all elements to the same value.
    XArray<T> &operator =(const T &val);
    /// Set all elements to the same value.
    void Set(const T &val);
    /// Set some elements to the same value.
    void Set(const T &val, SIZE_T pos, SIZE_T n);
    /// Append an array to 'this' array.
    inline XArray<T> &operator += (const XArray<T> &sour);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Begin serialization.
    void BegChunk(Serializer &inout, const char *tag);
    /// End serialization.
    inline void EndChunk(Serializer &inout);
    //@}

  private:
    /// @name Private methods
    //@{
    /// Change size of the array.
    OKAY Expand(SIZE_T needed_size);
    //@}

  protected:
    /// @name Protected members
    //@{
    /// Pointer to the array of the elements.
    T *data;
    /// Number of elements in the array.
    SIZE_T count;
    //@}

  private:
    /// @name Private members
    //@{
    /// Size occupied by array (size of allocated area), in elements.
    SIZE_T size;
    /// Number of elements in the memory block.
    SIZE_T block_size;
    //@}
  };  // class XArray


//////////////////////////////////////////////////////////////////////////////
// Methods of the class XArray

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Initialization by default: area of the array is set to NULL,
/// size and length are set to zero, block size is set to the parameter.
/// @param[in] the_block_size - Size of the memory block, in elements;
/// must be > 0, the debug version asserts it.
template <class T>
XArray<T>::XArray(SIZE_T the_block_size)
  {
  Assert(the_block_size < MAX_SIZE_T);
  size = 0;
  count = 0;
  block_size = the_block_size;
  data = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
/// @param[in] val The given data array.
/// @param length Length of the given data array.
/// @param[in] the_block_size  Size of the memory block, in elements;
/// @note Is array successfully constructed or not - can be controlled
/// via the Length() of the array, which is 0, if data allocation failed.
template <class T>
XArray<T>::XArray(const T *val, SIZE_T length, SIZE_T the_block_size)
  {
  Assert(the_block_size < MAX_SIZE_T);
  size = 0;
  count = 0;
  data = NULL;
  block_size = the_block_size;

  // Reallocate memory
  if (Resize(length) != SUCCESS)
    return;

  // Copy data
  count = length;
  for (SIZE_T i = 0; i < count; i++)
    data[i] = val[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] sour - A source object of the class.
template <class T>
XArray<T>::XArray(const XArray<T> &sour)
  {
  size = 0;
  count = 0;
  data = NULL;
  block_size = sour.block_size;  // size of the block by the default
  (void)this->Copy(sour);
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method releases all memory occupied by the array.
template <class T>
XArray<T>::~XArray()
  {
  delete[] data;
  if (data != NULL)
    DelMemStat(size * sizeof(T), data, "XArray");
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a reference to an array element.
///
/// The method returns a reference to the array element located in the
/// zero-based position @a pos.
/// @param[in] pos - An index of the element;
/// must be >= 0 and less than the length of the array, debug version asserts it.
/// @return A reference to the element.
template <class T>
T &XArray<T>::operator [](SIZE_T pos)
  {
  Assert(pos < MAX_SIZE_T && pos < count);
  return data[pos];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a const reference to an array element.
///
/// The method returns a const reference to the array element located
/// in the zero-based position @a pos.
/// @param[in] pos - An index of the element;
/// must be >= 0 and less than the length of the array, debug version asserts it.
/// @return A const reference to the element.
template <class T>
const T &XArray<T>::operator [](SIZE_T pos) const
  {
  Assert(pos < MAX_SIZE_T && pos < count);
  return data[pos];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the array for reading.
/// @return A const pointer to the array data.
template <class T>
const T *XArray<T>::Data() const
  {
  return data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the array for reading and writing.
/// @return A pointer to the array data.
template <class T>
T *XArray<T>::Data()
  {
  return data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of used elements.
/// @return The length of the array.
template <class T>
SIZE_T XArray<T>::Length() const
  {
  return count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the size occupied by the array.
/// @return The size occupied by the array, in elements.
template <class T>
SIZE_T XArray<T>::Size() const
  {
  return size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the block size of the array.
/// @return The block size of the array, in elements.
template <class T>
SIZE_T XArray<T>::BlockSize() const
  {
  return block_size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new block size.
/// @param[in] blsize - A new block size of the array, in elements;
/// must be > 0, debug version asserts it.
template <class T>
void XArray<T>::SetBlockSize(SIZE_T blsize)
  {
  Assert(blsize < MAX_SIZE_T);
  block_size = blsize;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add a new element to the end of the array.
/// @param[in] elem - A reference to the new element.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Add(const T &elem)
  {
  if (this->Expand(count + 1) != SUCCESS)
    return FAILURE;
  data[count++] = elem;
  Assert(count <= size);

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add new elements to the end of the array.
///
/// The method adds (copies) the specified number of elements pointed by @a elem
/// to the end of the array.
/// @param[in] elem - A pointer to the array of new elements.
/// @param[in] len - The length of the array of new elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Append(const T *elem, SIZE_T len)
  {
  Assert(len < MAX_SIZE_T);

  if (this->Expand(count + len) != SUCCESS)
    return FAILURE;

  for (SIZE_T i = 0; i < len; i++)
    data[count++] = elem[i];

  Assert(count <= size);

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Insert new elements to the specified position.
///
/// The method inserts (copies) the specified number of new elements to the
/// specified position of this array. The new elements are inserted even if
/// the position >= size of this array.
/// @param[in] elem - A pointer to the array of new elements.
/// @param[in] pos - A position for the insertion;
/// must be >= 0, debug version asserts it.
/// @param[in] len - The length of the array of new elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Insert(const T *elem, SIZE_T pos, SIZE_T len)
  {
  Assert(len < MAX_SIZE_T && pos < MAX_SIZE_T);

  // Allocation new memory, if needed
  SIZE_T new_len = (pos > count) ? (pos + len) : (count + len);
  if (this->Expand(new_len) != SUCCESS)
    return FAILURE;

  // Shift elements to end of the new array
  SIZE_T i;
  for (i = count; i > pos; )
    {
    i--;
    data[i + len] = data[i];
    }

  // Insert of the new elements
  for (i = 0;  i < len; i++)
    data[pos + i] = elem[i];

  count = new_len;
  Assert(count <= size);

  return SUCCESS;
  }  // Insert()

//////////////////////////////////////////////////////////////////////////////
/// Put a new element to the specified position.
///
/// The method puts a new element to the specified position.
/// The new element is put even if the position >= size of array.
/// @param[in] elem - A reference to the new element.
/// @param[in] pos - A position for the insertion;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Put(const T &elem, SIZE_T pos)
  {
  Assert(pos < MAX_SIZE_T);

  if (this->Expand(pos + 1) != SUCCESS)
    return FAILURE;

  data[pos++] = elem;
  if (count < pos)
    count = pos;

  Assert(count <= size);

  return SUCCESS;
  }  // Put()

//////////////////////////////////////////////////////////////////////////////
/// Exclude a number of elements starting from the specified position.
///
/// The method decreases the length of the array @a len elements beginning from
/// the specified position.  The size of the array is not changed.
/// @param[in] pos - A position of the first element to be excluded;
/// must be >= 0 and less than length of array, debug version asserts it.
/// @param[in] len - The number of excluded elements;
/// must be >= 0, debug version asserts it.
/// @note All elements of the array from the position @a pos + @a len
/// to the end of the array are copied to the position @a pos; the length
/// of the array is changed; the order of the rest elements is
/// not changed.
template <class T>
void XArray<T>::Exclude(SIZE_T pos, SIZE_T len)
  {
  Assert(pos < MAX_SIZE_T && len < MAX_SIZE_T && pos < count);

  if (pos + len < count)     // In this case shift elements
    {
    for (SIZE_T i = pos; i + len < count; i++)
      data[i] = data[len + i];

    count -= len;
    }
  else
    {
    count = pos;
    }

  return;
  }  // Exclude()

//////////////////////////////////////////////////////////////////////////////
/// Exclude one element at the specified position.
///
/// The method removes an element at the specified position and moves the last
/// element to this position. The length of the array is decreased by one.
/// The size of the array is not changed.
/// @param[in] pos - A position of the element to be removed;
/// must be >= 0 and less than length of array, debug version asserts it.
template <class T>
void XArray<T>::Remove(SIZE_T pos)
  {
  Assert(pos < MAX_SIZE_T && pos < count);

  count--;
  if (pos < count)
    data[pos] = data[count];
  }

//////////////////////////////////////////////////////////////////////////////
/// Change the size of the array.
///
/// The method changes a real size of the array to the specified size.
/// The memory is reallocated (either expanded or shrunk) if necessary,
/// that is, if the requested size differs from the current one.
/// If the new size is less than the array length, the length will be equal to
/// the new size.
/// @param[in] new_size - A new size of the array, in elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Resize(SIZE_T new_size)
  {
  if (new_size >= MAX_SIZE_T)
    {
    // Possible overflow...
    return User()->InternalError("XArray: new_size=%Iu (overflow?)", new_size);
    }
  Assert(new_size < MAX_SIZE_T);
  if (new_size == size)
    return SUCCESS;

  // Process zero case specially
  if (new_size == 0)
    {
    delete[] data;
    if (data != NULL)
      DelMemStat(size * sizeof(T), data, "XArray");
    data = NULL;
    count = size = 0;
    return SUCCESS;
    }

  // Allocate new array
  if (MAX_SIZE_T / new_size < sizeof(T))
    {
    // Possible overflow...
    return User()->InternalError("XArray: new_size=%Iu * %Iu (overflow?)",
      new_size, sizeof(T));
    }
  T *new_data = new T[new_size];
  AddMemStat(new_size * sizeof(T), new_data, "XArray");
  if (new_data == NULL)
    {
    User()->ErrorNoMemory(new_size * sizeof(T));
    return FAILURE;
    }
  if (data != NULL)
    DelMemStat(size * sizeof(T), data, "XArray");
  // Update size and length
  size = new_size;
  if (count > size)
    count = size;

  // Copy data
  for (SIZE_T i = 0; i < count; i++)
    new_data[i] = data[i];

  // Delete old area
  delete[] data;
  data = new_data;
  return SUCCESS;
  }  // Resize()

//////////////////////////////////////////////////////////////////////////////
/// Decrease the length of the array.
///
/// The method decreases the length of the array to the specified number of
/// elements.
/// Memory is not reallocated.
/// @param[in] new_count - A new length;
/// must be >= 0 and no more than the length of array, debug version asserts it.
template <class T>
void XArray<T>::Truncate(SIZE_T new_count)
  {
  Assert(new_count < MAX_SIZE_T && new_count <= count);
  count = new_count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the array.
///
/// The method sets array's length (number of elements accessible via
/// "operator []"). If necessary, the array is expanded to this length.
/// The array is never shrunk by this method.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Allocate(SIZE_T new_len)
  {
  Assert(new_len < MAX_SIZE_T);
  if (new_len >= MAX_SIZE_T)
    {
    // Possible overflow...
    return User()->InternalError("XArray::Allocate(%Iu): overflow?", new_len);
    }
  if (new_len <= size)
    {
    // Reallocation is not needed
    count = new_len;
    return SUCCESS;
    }

  // Realloc memory
  if (Resize(new_len) != SUCCESS)
    return FAILURE;

  count = new_len;
  return SUCCESS;
  }  // Allocate()

//////////////////////////////////////////////////////////////////////////////
/// The method sets the array's length (the number of elements accessible via
/// "operator []") and the size exactly to the specified number.
/// Memory is reallocated, if necessary.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::SetLength(SIZE_T new_len)
  {
  Assert(new_len < MAX_SIZE_T);
  // Realloc memory
  if (Resize(new_len) != SUCCESS)
    return FAILURE;
  count = new_len;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the array.
///
/// The method makes sure that the length of the array is at least as
/// specified by the parameter. The array can be expanded but not shrunk and not truncated.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Grow(SIZE_T new_len)
  {
  Assert(new_len < MAX_SIZE_T);
  if (new_len >= MAX_SIZE_T)
    return User()->InternalError("XArray::Grow(%Iu): overflow?", new_len);
  // Never decrease the array's length
  if (new_len <= count)
    return SUCCESS;  // Reallocation is not needed
  // Realloc memory
  // Use Expand(), not Resize()!
  if (Expand(new_len) != SUCCESS)
    return FAILURE;
  count = new_len;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Fast swap of arrays.
///
/// The method quickly swaps two arrays: just swaps the data pointers without
/// actual transposing of the elements.
/// @param[in, out] a - The first array to be swapped.
/// @param[in, out] b - The second array to be swapped.
template <class T>
void XArray<T>::SwapArrays(XArray<T> &a, XArray<T> &b)
  {
  Swap(a.data, b.data);
  Swap(a.size, b.size);
  Swap(a.count, b.count);
  Swap(a.block_size, b.block_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy the array.
/// The method copies an array to this array.
/// Memory under the new array is reallocated.
/// @param[in] sour - A source array.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Copy(const XArray<T> &sour)
  {
  // Reallocate memory
  if (this->Resize(sour.size) != SUCCESS)
    return FAILURE;

  // Copy data
  count = sour.count;
  for (SIZE_T i = 0; i < count; i++)
    data[i] = sour[i];

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// The assignment operator.
/// The method copies an array to this array.
/// Memory under the new array is reallocated.
/// @param[in] sour - A source array.
/// @return A reference to this array.
template <class T>
XArray<T> &XArray<T>::operator =(const XArray<T> &sour)
  {
  (void)(this->Copy(sour));
  return(*this);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
///
/// The method sets all array elements to the same value.
/// @param[in] val - A source value.
/// @return A reference to this array.
template <class T>
XArray<T> &XArray<T>::operator =(const T &val)
  {
  Set(val);
  return(*this);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
///
/// The method sets all array elements to the same value.
/// @param[in] val - A source value.
template <class T>
void XArray<T>::Set(const T &val)
  {
  for (SIZE_T i = 0; i < count; i++)
    data[i] = val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set some elements to the same value.
///
/// The method sets a block of array elements to the same value.
/// @param[in] val - A source value.
/// @param[in] pos - The starting position of elements to be set,
/// must be >= 0 and less than length of array, debug version asserts it.
/// @param[in] n - The number of elements to set, pos + n
/// must be >= 0 and less than the length of array, debug version asserts it.
template <class T>
void XArray<T>::Set(const T &val, SIZE_T pos, SIZE_T n)
  {
  for (SIZE_T i = 0; i < n; i++)
    data[pos + i] = val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change size of the array.
///
/// The method expands the allocated size, if necessary
/// (i.e. if the requested size is greater than the current one).
/// @param[in] needed_size - The needed size of the memory for the array.
/// @return SUCCESS / FAILURE (if memory was not allocated).
template <class T>
OKAY XArray<T>::Expand(SIZE_T needed_size)
  {
  SIZE_T new_size;
  if (needed_size >= MAX_SIZE_T)
    return User()->InternalError("XArray::Expand(%Iu): overflow?", needed_size);

  if (needed_size <= size)
    return SUCCESS;

  new_size = block_size;
  if (needed_size > block_size)
    {
#ifdef _WIN64
    // KISS
    new_size = size + (((needed_size - size) + (block_size - 1)) / block_size) * block_size;
#else
    const SIZE_T large_blk_size = MAX_INT / 8;
    if (needed_size < large_blk_size)
      {
      // Use old strategy for "usual" cases
      new_size = size + (((needed_size - size) + (block_size - 1)) / block_size) * block_size;
      }
    else
      {
      // Apply special algorithm for huge cases
      SIZE_T n_free_blocks = (MAX_INT - needed_size) / large_blk_size;
      new_size = MAX_INT - n_free_blocks * large_blk_size;
      }
#endif
    }

  return this->Resize(new_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Permute the array.
///
/// The method permutes the array according to the provided order.
/// @param[in] perm - A permutation array of the same length as this array;
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::Permute(const SIZE_T *perm)
  {
  if (count <= 1)
    {
    return SUCCESS;
    }

  // Allocate new array
  T *new_data = new T[count];
  AddMemStat(count * sizeof(T), new_data, "XArray");
  if (new_data == NULL)
    return FAILURE;
  Assert(data != NULL);
  DelMemStat(size * sizeof(T), data, "XArray");

  // Update size
  size = count;

  // Copy data with permutation
  for (SIZE_T i = 0; i < count; i++)
    new_data[i] = data[perm[i]];


  // Delete old area
  delete[] data;
  data = new_data;

  return SUCCESS;
  }  // Permute()

//////////////////////////////////////////////////////////////////////////////
/// Append an array to this array.
/// @param[in] sour - A source array to add.
/// @return A reference to this array.
template <class T>
XArray<T> &XArray<T>::operator +=(const XArray<T> &sour)
  {
  Append(sour.Data(), sour.Length());
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Begin serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
/// @param[in] tag - A chunk name.
template <class T>
void XArray<T>::BegChunk(Serializer &inout, const char *tag)
  {
  // Open a chunk
  inout.BegChunk(tag);
  // Process array's length (0 is default!)
  if (inout.Import())
    {
    // Reading...
    Resize();                        // Free memory for sure
    UINT64 total = 0;
    inout.Value((INT64 &)total);     // Read number of elements
    Assert(total < MAX_SIZE_T);
    if (total >= MAX_SIZE_T)
      {
      User()->InternalError("XArray serialization: %I64u elements, use x64 version", total);
      inout.SetAllocError();
      return;
      }
    if (Allocate(total) != SUCCESS)  // Allocate memory for the elements
      inout.SetAllocError();
    }
  else if (count != 0)
    {
    // Writing...
    // Write non-zero number of elements
    UINT64 total = count;
    inout.Value((INT64 &)total);
    }
  }  // BegChunk()

//////////////////////////////////////////////////////////////////////////////
/// End serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
template <class T>
void XArray<T>::EndChunk(Serializer &inout)
  {
  inout.EndChunk();     // Close the chunk
  }

//////////////////////////////////////////////////////////////////////////////
/// Set array size to Length(sour) and copy only used array part.
/// @param[in] sour - Input array.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY XArray<T>::SetArray(const XArray<T> &sour)
  {
  // Reallocate memory
  if (Resize(sour.count) != SUCCESS)
    return FAILURE;

  size = sour.count;
  count = size;
  // Copy data
  for (SIZE_T i = 0; i < count; i++)
    data[i] = sour[i];
  return SUCCESS;
  }

INTEGRA_NAMESPACE_END

#endif
