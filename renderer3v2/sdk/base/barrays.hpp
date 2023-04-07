/// @file
///
/// @brief Declaration of BArray, SArray, Array template classes.
///
/// Copyright &copy; INTEGRA, Inc., 2003-2004.
///
/// @internal
/// @author Ose - Sergey Ershov, '03.04.25

#ifndef _KLBM_BARRAYS_HPP_
#define _KLBM_BARRAYS_HPP_

#include "arrays.hpp"
#include "serializer.hpp"

INTEGRA_NAMESPACE_START

// forward references
template <class T> class BArray;

/// Base class for SArray and Array.
template <class T>
class BArray
  {
  public:
    /// Destructor.
    inline virtual ~BArray();

    /// Get the number of used elements.
    inline int Length() const;

  public:
    /// @name Access to elements
    //@{
    /// Get a reference to an array element.
    inline T &operator [](int i);
    /// Get a const reference to an array element.
    inline const T &operator [](int i) const;
    /// Get a pointer to the array.
    inline T *Data();
    /// Get a pointer to the array.
    inline const T *Data() const;
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline BArray<T> &operator =(const BArray<T> &src);
    /// Set all elements to the same value.
    inline BArray<T> &operator =(const T &src);
    /// Set the same value.
    inline void SetVal(const T &val);
    /// Set from pointer to data array.
    inline void Set(const T *ptr);
    /// Add from pointer to data array.
    inline void Add(const T *ptr);
    //@}
    /// Scale elements value to the given factor.
    inline BArray &operator *=(const double &c);
    /// Scale elements value to the given factor.
    inline BArray &operator *=(const float &c);
    /// Scale values of all elements by scalar.
    inline void Scale(const T &v);

  protected:
    /// @name Constructors
    //@{
    /// Constructor.
    inline BArray(T *memblock, int size);
    /// Copy constructor.
    inline BArray(const BArray &from);
    //@}

  protected:
    /// @name Creation and copying
    //@{
    /// Initialize the object.
    inline void Create(T *memblock, int size);
    /// Copy elements from the positions present in both objects.
    inline virtual void Copy(const BArray &src);
    //@}

    /// Get a non-const pointer to the array.
    inline T *XData() const;

  private:
    /// Contiguous memory block which contains (at least) count of elements.
    T *data;
    /// The number of elements array operates with.
    int count;
  };  // class BArray

/// Slice array.
template <class T>
class SArray : public BArray<T>
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    inline SArray(T *memblock, int size);
    /// Constructor.
    inline SArray(const BArray<T> &src);
    /// Destructor.
    inline virtual ~SArray();
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline SArray<T> &operator =(const BArray<T> &src);
    /// Set all elements to the same value.
    inline SArray<T> &operator =(const T &src);
  };  // class SArray

/// @brief Weak array. It does not master, copy, alloc and free of memory. 
/// It assigns pointers in copy constructor and assignment operator.
template <class T>
class WArray : public BArray<T>
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    inline WArray();
    /// Constructor.
    inline WArray(T *memblock, int size);
    /// Constructor.
    inline WArray(const BArray<T> &src);
    /// Copy constructor.
    inline WArray(const WArray<T> &src);
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline WArray<T> &operator =(const WArray<T> &src);
    /// Assignment operator.
    inline WArray<T> &operator =(const BArray<T> &src);
  };  // class SArray

/// Array which allocates an own memory block.
template <class T>
class Array : public BArray<T>
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline Array();
    /// Constructor.
    inline Array(int n);
    /// Copy constructor.
    inline Array(const Array<T> &from);
    /// Destructor.
    inline virtual ~Array();
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline Array<T> &operator =(const BArray<T> &src);
    /// Set all elements to the same value.
    inline Array<T> &operator =(const T &src);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Begin serialization.
    inline void BegChunk(Serializer &inout, const char *tag);
    /// End serialization.
    inline void EndChunk(Serializer &inout);
    /// Serialize the object in chunks.
    inline void Serialize(Serializer &inout);
    /// Serialize the object without chunks.
    inline void Value(Serializer &inout);
    //@}

  protected:
    /// Release memory and reset the count of elements etc.
    inline void Free();
    /// Change the length of the array.
    inline OKAY Allocate(int nc);
    /// Copy an array to this one.
    inline virtual void Copy(const BArray<T> &src);
  };  // class Array


//////////////////////////////////////////////////////////////////////////////
// Methods of the class BArray

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method does not release memory because its
/// origin is unknown in this class.
template <class T>
BArray<T>::~BArray()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of used elements.
/// @return The number of elements.
template <class T>
int BArray<T>::Length() const
  {
  return count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a reference to an array element.
/// @param[in] i - A zero-based index of the element.
/// @return A reference to the i-th element (for reading/writing).
template <class T>
T &BArray<T>::operator [](int i)
  {
  Assert (i >=0 && i < count);
  return data[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a const reference to an array element.
/// @param[in] i - A zero-based index of the element.
/// @return A constant reference to the i-th element (for reading).
template <class T>
const T &BArray<T>::operator [](int i) const
  {
  Assert (i >=0 && i < count);
  return data[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method  copies "admissible" elements (present in both sets)
/// from the source object to this one; the array dimension is unchanged.
/// @param[in] src - A source object.
/// @return A reference to this object.
template <class T>
BArray<T> &BArray<T>::operator =(const BArray<T> &src)
  {
  Copy(src); 
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
///
/// The method  assigns the specified value to all elements.
/// @param[in] src - A value to be assigned to elements.
/// @return A reference to this object.
template <class T>
BArray<T> &BArray<T>::operator =(const T &src)
  {
  if (data != NULL)
    {
    for (int i = 0; i < count; i++)
      data[i] = src;
    }
  else
    Assert(false);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the same value.
/// @param[in] val The value to set to array.
template <class T>
void BArray<T>::SetVal(const T &val)
  {
  for (int i = 0; i < count; i++)
    data[i] = val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set from pointer to data array.
/// @param[in] ptr Pointer to data array.
template <class T>
void BArray<T>::Set(const T *ptr)
  {
  for (int i = 0; i < count; i++)
    data[i] = ptr[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Add from pointer to data array.
/// @param[in] ptr Pointer to data array.
template <class T>
void BArray<T>::Add(const T *ptr)
  {
  for (int i = 0; i < count; i++)
    data[i] += ptr[i];
  }

///////////////////////////////////////////////////////////////////////////////
/// Scale elements value to the given factor.
/// The method multiplies all elements by the specified one.
/// @note T type must have 'operator *=" defined.
/// @param[in] c - A multiplier.
/// @return A reference to "this" array.
template <class T>
BArray<T> &BArray<T>::operator *=(const double &c)
  {
  for (int i = 0; i < count; i++)
    data[i] *= c;
  return *this;
  }  // 'operator *=()'

///////////////////////////////////////////////////////////////////////////////
/// Scale elements value to the given factor.
/// The method multiplies all elements by the specified one.
/// @note T type must have 'operator *=" defined.
/// @param[in] c - A multiplier.
/// @return A reference to "this" array.
template <class T>
BArray<T> &BArray<T>::operator *=(const float &c)
  {
  for (int i = 0; i < count; i++)
    data[i] *= c;
  return *this;
  }  // 'operator *=()'

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the array.
///
/// The method  allows derived classes to access the memory block.
/// @return A pointer to the array of elements.
template <class T>
T *BArray<T>::Data()
  {
  return data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the array.
///
/// The method  allows derived classes to access the memory block.
/// @return A pointer to the array of elements.
template <class T>
const T *BArray<T>::Data() const
  {
  return data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a non-const pointer to the array.
///
/// The method  allows derived classes to access the memory block.
/// @return A pointer to the array of elements.
template <class T>
T *BArray<T>::XData() const
  {
  return data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// Constructor from an <b>externally allocated memory block</b>
/// (which can be handled with various allocators).
/// @param[in] memblock - An externally created memory block.
/// @param[in] size - The number of its elements to operate with.
template <class T>
BArray<T>::BArray(T *memblock, int size)
  {
  data = memblock;
  count = size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// Declared as 'private' mainly to remove it from public methods.
/// @param[in] from - Source object.
template <class T>
BArray<T>::BArray(const BArray &from)
  {
  data = from.data;
  count = from.count;
  }

//////////////////////////////////////////////////////////////////////////////
/// Initialize the object.
///
/// The method  initialises the object: assigns the address of the memory block
/// which keeps an array of elements and the number of those elements.
/// So to say, a "post factum" constructor.
/// @param[in] memblock - A starting address of the memory block to keep 
/// elements.
/// @param[in] size - The count of those elements; the memory block size must  
/// be enough for them.
template <class T>
void BArray<T>::Create(T *memblock, int size)
  {
  data = memblock; 
  count = size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy elements from positions present in both objects.
///
/// The method  copies elements from positions present in both source and
/// destination objects; the array dimension is unchanged.
/// @note Derived classes may change its behavior by resizing the array.
/// @note This function is a "core" of the assignment operator which
/// allows overriding its behaviour in derived classes
/// because there are problems with overriding assignment
/// operators themselves.
/// @param[in] src - A source object whose elements are copied to this array.
template <class T>
void BArray<T>::Copy(const BArray &src)
  {
  if (&src != this)
    {
    if (data != NULL && src.data != NULL)
      {
      int i, n = Min(Length(), src.Length());
      for (i = 0; i < n; i++)
        data[i] = src.data[i];
      }
    else
      Assert(false);
    }
  }  // Copy()

//////////////////////////////////////////////////////////////////////////////
/// Scale values of all elements by scalar.
/// @param[in] v - Scalar value.
///
template <class T>
inline void BArray<T>::Scale(const T &v)
  {
  for (int i = 0; i < Length(); i++)
    data[i] *= v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set values of array b to value of array a.
/// @param[in] a - Target array.
/// @param[in] b - Source array.
///
template <class T>
inline void Set(BArray<T> &a, const BArray<T> &b)
  {
  Assert(a.Length() == b.Length());
  for (int i = 0; i < a.Length(); i++)
    a[i] = b[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Add values of array b to value of array a.
/// @param[in] a - Target array.
/// @param[in] b - Source array.
///
template <class T>
inline void Add(BArray<T> &a, const BArray<T> &b)
  {
  Assert(a.Length() == b.Length());
  for (int i = 0; i < a.Length(); i++)
    a[i] += b[i];
  }

//////////////////////////////////////////////////////////////////////////////
// Methods of the class WArray

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
template <class T>
WArray<T>::WArray() : BArray<T>(NULL, 0)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method  constructs an object from an externally created memory block.
/// @param[in] memblock - An externally created memory block.
/// @param[in] size - The number of its elements to operate with.
template <class T>
WArray<T>::WArray(T *memblock, int size)
  : BArray<T>(memblock, size)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src - A source object.
template <class T>
WArray<T>::WArray(const WArray<T> &src)
  : BArray<T>(src)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
/// @param[in] src - A source object.
template <class T>
WArray<T>::WArray(const BArray<T> &src)
  : BArray<T>(src)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method copies only pointer.
/// @param[in] src - A source object.
/// @return A reference to this object.
template <class T>
WArray<T> &WArray<T>::operator =(const BArray<T> &src)
  {
  Create(src.Data(), src.Length());
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method copies only pointer.
/// @param[in] src - A source object.
/// @return A reference to this object.
template <class T>
WArray<T> &WArray<T>::operator =(const WArray<T> &src)
  {
  Create(src.XData(), src.Length());
  return *this;
  }


//////////////////////////////////////////////////////////////////////////////
// Methods of the class SArray

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method  constructs an object from an externally created memory block.
/// @param[in] memblock - An externally created memory block.
/// @param[in] size - The number of its elements to operate with.
template <class T>
SArray<T>::SArray(T *memblock, int size) : 
  BArray<T>(memblock, size)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src - A source object.
template <class T>
SArray<T>::SArray(const BArray<T> &src) : 
  BArray<T>(src)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method does not release memory because it is not own to this class.
template <class T>
SArray<T>::~SArray()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method  copies "admissible" elements (present in both sets)
/// from the source to the current object; the array dimension is unchanged.
/// @param[in] src - A source object.
/// @return A reference to this object.
template <class T>
SArray<T> &SArray<T>::operator =(const BArray<T> &src)
  {
  Copy(src);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
///
/// The method  assigns the specified value to all elements.
/// @param[in] src - A value to be assigned to elements.
/// @return A reference to this object.
template <class T>
SArray<T> &SArray<T>::operator =(const T &src)
  {
  BArray<T>::operator=(src);
  return *this;
  }


//////////////////////////////////////////////////////////////////////////////
// Methods of the class Array

/// Destructor; releases memory because it is own.
template <class T>
Array<T>::~Array()
  {
  Free();
  }

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method  creates an array of 0 elements.
template <class T>
Array<T>::Array()
  : BArray<T>(NULL, 0)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method  constructs an array of desired size.
/// @param[in] n - A desired array length.
template <class T>
Array<T>::Array(int n)
  : BArray<T>(n > 0 ? new T[n] : NULL, n)
  {
  Assert(n >= 0);
  };

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// The method  creates an array of the same size and with the same values of 
/// elements.
/// @param[in] from - A source object.
template <class T>
Array<T>::Array(const Array<T> &from)
  : BArray<T>(new T[from.Length()],
  from.Length())
  {
  BArray<T>::operator=(from);
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method  resizes (if needed) the array and copies @b all source 
/// elements.
/// @param[in] src - A source object.
/// @return A reference to this object.
template <class T>
Array<T> &Array<T>::operator =(const BArray<T> &src)
  {
  Copy(src);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
/// @param[in] src - A value to be assignment to the elements.
/// @return A reference to this object.
template <class T>
Array<T> &Array<T>::operator =(const T &src)
  {
  BArray<T>::operator =(src);
  return *this;
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy an array to this one.
///
/// The method  resizes this object (so that its dimension equals to
/// that of the source one) and copies elements from the latter.
/// @param[in] src - A source object.
/// @return A reference to this object.
/// @note This derived class changes BArray behavior by resizing the array.
template <class T> void
Array<T>::Copy(const BArray<T> &src)
  {
  // Resize (if needed!!):
  Allocate(src.Length());

  // copy values of elements
  BArray<T>::Copy(src);
  }

///////////////////////////////////////////////////////////////////////////////
/// Begin serialization.
///
/// The method  opens lower a level serialization (serializes the class "header",
/// not elements).
/// @param[in, out] inout - A serializer.
/// @param[in] tag - A chunk name.
template <class T> void
Array<T>::BegChunk(Serializer &inout, const char *tag)
  {
  unsigned total = 0;

  // Open a chunk
  inout.BegChunk(tag);
  // Process array's length (0 is default!)
  if (inout.Import())
    {
    // Reading...
    inout.Value(total);              // Read number of elements

    // Allocate memory for the elements
    if (Allocate(total) != SUCCESS)
      inout.SetAllocError();
    }
  else if (this->Length() != 0)
    {
    // Writing...
    // Write non-zero number of elements
    total = this->Length();
    inout.Value(total);
    }
  };  // BegChunk()

///////////////////////////////////////////////////////////////////////////////
/// End serialization.
///
/// The method  closes the lower level serialization.
/// @param[in, out] inout - A serializer.
template <class T> void
Array<T>::EndChunk(Serializer &inout)
  {
  inout.EndChunk();     // Close the chunk
  };

///////////////////////////////////////////////////////////////////////////////
/// Serialize the object in chunks.
/// @param[in, out] inout - A serializer.
template <class T> void
Array<T>::Serialize(Serializer &inout)
  {
  BegChunk(inout, "spctrlDistr");
  for (int i = 0; i < this->Length(); i++)
    (*this)[i].SerializeValue(inout);
  EndChunk(inout);
  };

///////////////////////////////////////////////////////////////////////////////
/// Serialize the object without chunks.
/// @param[in, out] inout - A serializer.
template <class T> void
Array<T>::Value(Serializer &inout)
  {
  int l = this->Length();
  inout.Value(l);

  if (inout.Import())
    {
    if (Allocate(l) != SUCCESS)
      inout.SetAllocError();
    }

  for (int i = 0; i < this->Length(); i++)
    (*this).SerializeValue(inout);
  };  // Value()

///////////////////////////////////////////////////////////////////////////////
/// Release memory and reset the count of elements, etc.
template <class T>
void Array<T>::Free()
  {
  if (this->Length() > 0 && this->Data())
    {
    T* data = this->Data();
    delete [] data;

    /// remember that there is no memory nor elements...
    this->Create(NULL, 0);
    }
  }

///////////////////////////////////////////////////////////////////////////////
/// Change length of the array.
///
/// The method  changes the memory block size, if necessary (that is, if the 
/// new value differs from the current one). The elements are @b not copied to
/// the resized block.
/// @param nc - A target length of the array.
/// @return SUCCESS / FAILURE.
template <class T> OKAY Array<T>::Allocate(int nc)
  {
  if (nc < 0)
    {
    // May we run out of 32 bit...
    Assert(false);
    return FAILURE;
    }
  if (nc == this->Length())
    return SUCCESS;

  // resize:
  Free();
  T *memblock = new T[nc];
  if (!memblock)
    {
    this->Create(NULL, 0);
    return FAILURE;
    }

  // transfer to base class:
  Create(memblock, nc);
  return SUCCESS;
  }  // Allocate()

INTEGRA_NAMESPACE_END
#endif // _KLBM_BARRAY_HPP_
