/// @file
///
/// @brief Declaration of TBArray template class.
///
/// Copyright &copy; INTEGRA, Inc., 2003-2004.
///
/// @internal
/// @author Pbb - Boris Barladian '03.04.01

#ifndef _KLBC_BARRAY_HPP_
#define _KLBC_BARRAY_HPP_

#include "base.hpp"
#include "iarray.hpp"
#include "serializer.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic array of elements of an arbitrary type.
template <class T>
class TBArray : public IArray
  {
  public:
    /// @name Constants
    //@{
    /// Optimal block size for the array.
    static const int OPTIMAL_BLOCK_SIZE = (8 * 64 * 1024 - 32) / sizeof(T);
    // Previous numbers:
    // 64 * 1024 * 4 / sizeof(T);
    // 192 * 1024 * 4 / sizeof(T);
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    explicit TBArray(int the_block_size = OPTIMAL_BLOCK_SIZE);
    /// Copy constructor.
    TBArray(const TBArray<T> &sour);
    /// Destructor.
    inline virtual ~TBArray();
    //@}

  public:
    /// @name Access to the elements
    //@{
    /// Get a reference to an array element.
    inline T &operator [](int pos);
    /// Get a const reference to an array element.
    inline const T &operator [](int pos) const;
    /// Get a pointer to the specified block.
    inline T *BlockData(int block) const;
    /// Get an index of the block with the specified element.
    inline int BlockIndex(int pos) const;
    /// Get an index of the first element in the specified block.
    inline int FirstInBlock(int block_index) const;
    //@}

  public:
    /// @name Length and sizes
    //@{
    /// Get the number of used elements.
    inline int Length() const;
    /// Get the size occupied by the array.
    inline int Size() const;
    /// Get the block size of the array.
    inline int BlockSize() const;
    /// Set a new block size.
    inline void SetBlockSize(int blsize);
    //@}

  public:
    /// @name Addition of elements
    //@{
    /// Add a new element to the end of the array.
    OKAY Add(const T &elem);
    /// Add new elements to the end of the array.
    OKAY Append(const T *pelem, int len = 1);
    /// Insert new elements to specified position.
    OKAY Insert(const T *pelem, int pos, int len = 1);
    /// Put a new element to the specified position.
    OKAY Put(const T &elem, int pos);
    //@}

  public:
    /// @name Removal of elements
    //@{
    /// Exclude a number of elements starting from the specified position.
    void Exclude(int pos, int len = 1);
    /// Exclude one element at the specified position.
    void Remove(int pos);
    //@}

  public:
    /// @name Size and length change
    //@{
    /// Decrease size of the array.
    inline void Truncate(int new_count = 0);
    /// Change size of the array.
    OKAY Resize(int new_size = 0);
    /// Change (expand) length of array.
    OKAY Allocate(int new_len);
    /// @brief Change (expand) length of the array and fill the
    /// allocated memory with 0.
    OKAY ZeroAllocate(int new_len);
    /// Change (expand) length of array.
    OKAY Grow(int new_len);

  public:
    /// @name Swap arrays
    //@{
    /// Fast swap of arrays.
    static void SwapArrays(TBArray<T> &a, TBArray<T> &b);
    //@}

  public:
    /// @name Copying, assignment
    //@{
    /// Copy an array.
    OKAY Copy(const TBArray<T> &sour);
    /// Assignment operator.
    TBArray<T> &operator =(const TBArray<T> &sour);
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
    OKAY Expand(int needed_size);
    //@}

  private:
    /// @name Private members
    //@{
    /// Pointer on the array of the blocks of elements.
    T **data;
    /// Number of elements in the array.
    int count;
    /// Number of allocated blocks.
    int size;
    /// Number of elements in the memory block.
    int block_size;

  };  // class TBArray

//////////////////////////////////////////////////////////////////////////////
// Methods of the class TBArray


//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Initialization by default: the area of the array is set to NULL,
/// size and length are set to zero, block size is set to the parameter.
/// @param[in] the_block_size - A size of the block memory, in elements;
/// must be > 0, debug version asserts it.
template <class T>
TBArray<T>::TBArray(int the_block_size)
  {
  Assert(the_block_size > 0);
  size = 0;
  count = 0;
  block_size = the_block_size;
  data = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] sour - A source object of the class.
template <class T>
TBArray<T>::TBArray(const TBArray<T> &sour)
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
TBArray<T>::~TBArray()
  {
  if (data == NULL)
    return;
  for (int ic = 0; ic < size; ic++)
    {
    delete[] data[ic];
    DelMemStat(block_size * sizeof(T), data[ic], "TBArray data block");
    }
  delete[] data;
  DelMemStat(size * sizeof(T *), data, "TBArray control block");
  data = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a reference to an array element.
///
/// The method returns a reference to the array element located in the
/// zero-based position @a pos.
/// @param[in] pos - An index of the element;
/// must be >= 0 and less than length of array, debug version asserts it.
/// @return A reference to the element.
template <class T>
T &TBArray<T>::operator [](int pos)
  {
  Assert(pos >= 0 && pos < count);
  return data[pos / block_size][pos % block_size];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a const reference to an array element.
///
/// The method returns a const reference to the array element located
/// in the zero-based position @a pos.
/// @param[in] pos - An index of the element;
/// must be >= 0 and less than length of array, debug version asserts it.
/// @return A const reference to the element.
template <class T>
const T &TBArray<T>::operator [](int pos) const
  {
  Assert(pos >= 0 && pos < count);
  return data[pos / block_size][pos % block_size];
  }


//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the specified block.
///
/// The method gets a pointer to the first element in the specified block.
/// @param[in] block - An index of the block;
/// must be >= 0 and less than number of blocks, debug version asserts it.
/// @return A pointer to the first element in the block.
template <class T>
T* TBArray<T>::BlockData(int block) const
  {
  return data[block];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get an index of the block with the specified element.
///
/// The method gets an index of the block where an element with the specified
/// index @a pos is stored.
/// @param[in] pos - An index of the element;
/// must be >= 0 and less than length of array, debug version asserts it.
/// @return An index of the block with the specified element.
template <class T>
int TBArray<T>::BlockIndex(int pos) const
  {
  return pos / block_size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get an index of the first element in the specified block.
///
/// The method gets an index of the first element from the specified block.
/// @param[in] block_index - An index of the block.
/// @return An index of the first element in the specified block.
template <class T>
int TBArray<T>::FirstInBlock(int block_index) const
  {
  return block_index * block_size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of used elements.
/// @return The length of the array.
template <class T>
int TBArray<T>::Length() const
  {
  return count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the size occupied by the array.
/// @return The size occupied by the array, in elements.
template <class T>
int TBArray<T>::Size() const
  {
  return size * block_size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the block size of the array.
/// @return The block size of the array, in elements.
template <class T>
int TBArray<T>::BlockSize() const
  {
  return block_size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new block size.
/// @note This method can be used only when the size of the array is 0
/// (i.e. no memory is allocated). Debug version asserts it.
/// @param[in] blsize - A new block size of the array, in elements;
/// must be > 0, debug version asserts it.
template <class T>
void TBArray<T>::SetBlockSize(int blsize)
  {
  Assert(blsize > 0);
  block_size = blsize;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add a new element to the end of the array.
/// @param[in] elem - A reference to the new element.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Add(const T &elem)
  {
  if (this->Expand(count + 1) != SUCCESS)
    return FAILURE;
  (*this)[count++] = elem;
  Assert(count <= size * block_size);

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add new elements to the end of the array.
///
/// The method adds (copies) the specified number of elements pointed by the
/// pointer @a elem to the end of the array.
/// @param[in] elem - A pointer to an array of new elements.
/// @param[in] len - The length of the array of new elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Append(const T *elem, int len)
  {
  Assert(len > 0);

  if (this->Expand(count + len) != SUCCESS)
    return FAILURE;

  for (int i = 0; i < len; i++)
    (*this)[count++] = elem[i];

  Assert(count <= size * block_size);

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Insert new elements to the specified position.
///
/// The method inserts (copies) the specified number of new elements to the
/// specified position. The new elements are inserted even if position >= size
/// of the array.
/// @param[in] elem - A pointer to an array of new elements.
/// @param[in] pos - A position for the insertion;
/// must be >= 0, debug version asserts it.
/// @param[in] len - The length of the array of new elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Insert(const T *elem, int pos, int len)
  {
  Assert(len >= 0 && pos >= 0);

  // Allocation new memory, if needed
  int new_len = (pos > count) ? (pos + len) : (count + len);
  if (this->Expand(new_len) != SUCCESS)
    return FAILURE;

  // Shift elements to end of the new array
  int i;
  int old_count = count;
  count = new_len;
  for (i = old_count; i > pos; )
    {
    i--;
    (*this)[i + len] = (*this)[i];
    }

  // Insert of the new elements
  for (i = 0;  i < len; i++)
    (*this)[pos + i] = elem[i];

  Assert(count <= size * block_size);

  return SUCCESS;
  }  // Insert()

//////////////////////////////////////////////////////////////////////////////
/// Put a new element to the specified position.
///
/// The method puts a new element to the specified position.
/// The new element is put even if position >= size of the array.
/// @param[in] elem - A reference to a new element.
/// @param[in] pos - A position for the insertion;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Put(const T &elem, int pos)
  {

  Assert(pos >= 0);

  if (this->Expand(pos + 1) != SUCCESS)
    return FAILURE;

  if (count <= pos)
    count = pos + 1;

  (*this)[pos++] = elem;
  Assert(count <= size * block_size);
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Exclude a number of elements starting from the specified position.
///
/// The method decreases the length of the array by the @a len elements
/// beginning from the specified position.  The size of the array is not
/// changed.
/// @param[in] pos - A position of the first element to be excluded;
/// must be >= 0 and less than length of array, debug version asserts it.
/// @param[in] len - The number of excluded elements;
/// must be >= 0, debug version asserts it.
/// @note All elements of the array from the position @a pos + @a len
/// to the end of the array are copied to the position @a pos; the length
/// of the array is changed; the order of the rest elements is
/// not changed.
template <class T>
void TBArray<T>::Exclude(int pos, int len)
  {

  Assert(len > 0 && pos >= 0 && pos < count);

  if (pos + len < count)     // In this case shift elements
    {
    for (int i = pos; i + len < count; i++)
      (*this)[i] = (*this)[len + i];

    count -= len;
    }
  else
    {
    count = pos;
    }

  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Exclude one element at the specified position.
///
/// The method removes an element at the specified position and moves the last
/// element to this position. The length of the array is decreased by one.
/// The size of the array is not changed.
/// @param[in] pos - A position of the element to be removed;
/// must be >= 0 and less than length of array, debug version asserts it.
template <class T>
void TBArray<T>::Remove(int pos)
  {
  Assert(pos >= 0 && pos < count);

  if (pos < count)
    (*this)[pos] = (*this)[count - 1];
  count--;
  }

//////////////////////////////////////////////////////////////////////////////
/// Decrease size of the array.
///
/// The method decreases the length of the array to the specified number of
/// elements.
/// Memory is not reallocated.
/// @param[in] new_count - A new length;
/// must be >= 0 and not more than length of array, debug version asserts it.    // Decreases a length of array to new_count elements. Memory is not
template <class T>
void TBArray<T>::Truncate(int new_count)
  {
  Assert(new_count >= 0 && new_count <= count);
  count = new_count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change size of the array.
///
/// The method changes the current size of the array to the specified size.
/// Memory is reallocated (either expanded or shrunk) if necessary,
/// that is, if the requested size differs from the current one.
/// If the new size is less than the array length, the length will be equal
/// to the new size.
/// @param[in] new_count - A new size of the array, in elements;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Resize(int new_count)
  {
  if ((new_count < 0) || (new_count + block_size - 1 < 0))
    {
    // May we run out of 32 bit...
    return User()->InternalError("TBArray: new_size=%d (32 bit overflow?)",
      new_count);
    }
  // Process zero case specially
  if (new_count == 0)
    {
    for (int ic = 0; ic < size; ic++)
      {
      delete[] data[ic];
      DelMemStat(block_size * sizeof(T), data[ic], "TBArray data block");
      }
    delete[] data;
    DelMemStat(size * sizeof(T *), data, "TBArray control block");
    data = NULL;
    count = size = 0;
    return SUCCESS;
    }

  int req_size = (new_count + block_size - 1) / block_size;
  if (req_size == size)
    return SUCCESS;

  // Allocate new array
  T **new_data;
  int old_size = size;
  size = req_size;
  new_data = new T*[size];
  AddMemStat(size * sizeof(T *), new_data, "TBArray control block");
  if (new_data == NULL)
    return FAILURE;
  memset(new_data, 0, sizeof(T*) * size);
  int ret = SUCCESS;
  if (size < old_size)
    {
    for (int ic = 0; ic < old_size; ic++)
      {
      if (ic < size)
        new_data[ic] = data[ic];
      else
        {
        delete [] data[ic];
        DelMemStat(block_size * sizeof(T), data[ic], "TBArray data block");
        }
      }
    }
  else
    {
    for (int ic = 0; ic < size; ic++)
      {
      if (ic < old_size)
        new_data[ic] = data[ic];
      else
        {
        new_data[ic] = new T[block_size];
        AddMemStat(block_size * sizeof(T), new_data[ic], "TBArray data block");
        if (new_data[ic] == NULL)
          {
          ret = FAILURE;
          size = ic;
          break;
          }
        }
      }
    }


  if (count > new_count)
    count = new_count;

  // Delete old area
  delete[] data;
  DelMemStat(old_size * sizeof(T *), data, "TBArray control block");
  data = new_data;
  return ret;
  }  // Resize()

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the array.
///
/// The method sets the array length (the number of elements accessible via
/// "operator []"). If necessary, the array is expanded to this length.
/// The array is never shrunk by this method.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Allocate(int new_len)
  {
  Assert(new_len >= 0);
  if (new_len <= size * block_size)
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
  }

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the array and fill the allocated memory
/// with 0.
///
/// The method sets the array length (the number of elements accessible via
/// "operator []") and fills all memory (occupied by the array) with 0.
/// If necessary, the array is expanded to this length.
/// The array is never shrunk by this method.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::ZeroAllocate(int new_len)
  {
  int old_size = count / block_size;
  int old_len = count;
  old_len %= block_size;
  if (Allocate(new_len) != SUCCESS)
    return FAILURE;
  if (old_size >= size)
    return SUCCESS;
  memset(data[old_size] + old_len, 0, sizeof(T) * (block_size - old_len));
  for (++old_size ; old_size < size; ++old_size)
    memset(data[old_size], 0 , sizeof(T) * block_size);
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the array.
///
/// The method makes sure that the length of the array is at least as
/// specified by the parameter. The array can be expanded but not shrunk and
/// not truncated.
/// @param[in] new_len - A new length;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Grow(int new_len)
  {
  Assert(new_len >= 0);
  // Never decrease the array's length
  if (new_len <= count)
    return SUCCESS;  // Reallocation is not needed

  // Realloc memory
  // Use Expand(), not Tresize()!
  if (Expand(new_len) != SUCCESS)
    return FAILURE;

  count = new_len;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Fast swap of arrays.
///
/// The method quickly swaps two arrays: data pointers are swapped without
/// actual transposing of the elements.
/// @param[in, out] a - The first array to be swapped.
/// @param[in, out] b - The second array to be swapped.
template <class T>
void TBArray<T>::SwapArrays(TBArray<T> &a, TBArray<T> &b)
  {
  Swap(a.data, b.data);
  Swap(a.size, b.size);
  Swap(a.count, b.count);
  Swap(a.block_size, b.block_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy an array.
/// The method copies an array to this array.
/// Memory under the new array is reallocated.
/// @param[in] sour - A source array.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TBArray<T>::Copy(const TBArray<T> &sour)
  {
  // Reallocate memory
  if (block_size != sour.block_size)
    {
    // If block size is different - deallocate all memory
    Resize();
    this->block_size = sour.block_size;
    }
  // Allocate the same amount of memory
  if (sour.size > 0)
    {
    if (this->Resize(sour.size * block_size - 1) != SUCCESS)
      return FAILURE;
    }
  else
    size = sour.size;

  // Copy data
  count = sour.count;
  for (int i = 0; i < count; i++)
    (*this)[i] = sour[i];
  return SUCCESS;
  }  // Copy()

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// The method copies an array to this array.
/// Memory under the new array is reallocated.
/// @param[in] sour - A source array.
/// @return A reference to this array.
template <class T>
TBArray<T> &TBArray<T>::operator =(const TBArray<T> &sour)
  {
  (void)(this->Copy(sour));
  return(*this);
  }

//////////////////////////////////////////////////////////////////////////////
/// Change size of the array.
///
/// The method expands the allocated size if necessary
/// (i.e. if the requested size is greater than the current one).
/// @param[in] needed_size - The needed size of the memory for the array.
/// @return SUCCESS / FAILURE (if memory was not allocated).
template <class T>
OKAY TBArray<T>::Expand(int needed_size)
  {
  int new_size = (needed_size + block_size - 1) / block_size;

  if (new_size <= size)
    return SUCCESS;

  return this->Resize(needed_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Begin serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
/// @param[in] tag - A chunk name.
template <class T>
void TBArray<T>::BegChunk(Serializer &inout, const char *tag)
  {
  // Open a chunk
  inout.BegChunk(tag);
  // Process array's length (0 is default!)
  if (inout.Import())
    {
    // Reading...
    Resize();                        // Free memory for sure
    unsigned total = 0;
    inout.Value(total);              // Read number of elements
    if (Allocate(total) != SUCCESS)  // Allocate memory for the elements
      inout.SetAllocError();
    }
  else if (count != 0)
    {
    // Writing...
    // Write non-zero number of elements
    inout.Value((unsigned &)count);
    }
  }  // BegChunk()

//////////////////////////////////////////////////////////////////////////////
/// End serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
template <class T>
void TBArray<T>::EndChunk(Serializer &inout)
  {
  inout.EndChunk();     // Close the chunk
  }  

INTEGRA_NAMESPACE_END
#endif  // _KLBC_BARRAY_HPP_
