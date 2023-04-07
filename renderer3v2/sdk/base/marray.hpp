/// @file
///
/// @brief Definition of MArray template class
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Pnd - Nikolay Derjabin<BR>
///         Rkd - Kirill Dmitriev<BR>
///         Ose - Sergey Ershov

#ifndef _KLBC_MARRAY_HPP_
#define _KLBC_MARRAY_HPP_

#include <base/garray.hpp>

INTEGRA_NAMESPACE_START

/// Dynamic array of elements of an arbitrary type,
/// for which '==' and '<' operations are applicable.
/// MArray class provides array sort and fast binary search.
template <class T>
class MArray : public GArray<T>
  {
  public:
    /// @name Constructors
    //@{
    /// Initialization by default.
    explicit MArray(int the_block_size = 10);
    /// Constructor from the given values.
    inline MArray(const T *val, int length,
                  int the_block_size = TArray<T>::DEF_BLOCK_SIZE);
    /// Copy constructor
    inline MArray(const MArray<T> &sour);
    //@}

    /// Comparison operator.
    bool operator == (const MArray<T> &another) const;
    /// Comparison operator.
    inline bool operator != (const MArray<T> &another) const;
    /// Scale elements value to the given factor.
    inline MArray &operator *=(const double &c);
    /// Add elements of the given array to elements of 'this' array.
    inline MArray<T> &operator += (const TArray<T> &sour);

    /// @name Sorting of external array in the ascending order
    //@{
    /// Sorts array of the elements of T type.
    static void QSort(T *data, int count);

    /// Sorts array of the elements of T type by the comparison function.
    static void QSort(T *data, int count, bool (*less)(const T &a, const T &b));

    /// Defines ascending order of the elements in array.
    static void QSort(const T *data, int count, TArray<int> &perm);

    /// @brief Sorts elements in the ascending order.
    /// Preserves relative order of the equivalent elements.
    static void StableSort(T *data, int count);
    /// @brief Defines ascending order of the elements in array preserving
    /// relative order of equivalent elements.
    static void StableSort(const T *data, int count, TArray<int> &perm);
    /// @brief Sorts array of the elements of T type by the comparison
    /// function, preserving relative order of equivalent elements.
    static void StableSortLessEqual(T *data, int count);
    //@}

    /// @name Sorting of internal array in the ascending order
    //@{
    /// Sorts all elements of this array in the ascending order
    inline void QSort();

    /// Defines ascending order of the elements in this array
    inline void QSort(TArray<int> &perm) const;

    /// Sorts first @a len elements of this array in the ascending order
    inline void QSort(int len);

    /// Sorts all elements of this array by the comparison function
    inline void QSort(bool (*less)(const T &a, const T &b));

    /// Sorts first @a len elements of this array by the comparison function
    inline void QSort(int len, bool (*less)(const T &a, const T &b));

    /// @brief Sorts all elements of this array in the ascending order.
    /// Preserves relative order of the equivalent elements.
    inline void StableSort();
    //@}

    /// @name Search in the ascending array
    /// Be carefull, read complete description - actually it returns position
    /// of the next element after the last element equal to the searched!
    //@{
    /// Finds position of value @a x in an ascending array.
    inline int BinarySearch(const T &x) const;

    /// Finds position of value @a x in an ascending external array.
    inline static int BinarySearch(const T &x, const T *data, int n);
    //@}

    /// @name Search in descending array
    /// @a Array may be sorted to descending order by the method QSort() with
    /// appropriate function @a less().
    //@{
    /// Finds position of value @a x in a descending this array
    inline int BinarySearchRev(const T &x) const;
    //@}

    /// @name Search in descending external array
    /// %Array may be sorted to descending order by the method QSort() with
    /// appropriate function @a less().
    //@{
    /// Finds position of value @a x in a descending array.
    inline static int BinarySearchRev(const T &x, const T *data, int n);
    //@}

    /// Find maximal value in the array.
    inline T MaxValue() const;
    /// Find minimal value in the array.
    inline T MinValue() const;
    /// Find index of element with maximal value.
    inline int MaxValueIndex() const;
    /// Calculate a sum of values in the array.
    inline T Sum() const;

    /// Create new array, resized with linear interpolation.
    inline OKAY ResizeLinear(int n, MArray<T> &out) const;
    /// Pack elements of this array - leave only non-equal.
    inline void Pack();
  };  // class MArray


// -----------------------------------------------------------------
//              Methods of the class MArray
// -----------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Initialization by default.
///
/// Creates empty array with zero length. Block size (length of the blocks on
/// memory allocation) is set to the parameter.
///
/// @note Block size must be >0. Debug version asserts this.
/// @param the_block_size - Size of the block on memory allocation.
template <class T>
MArray<T>::MArray(int the_block_size) : GArray<T>(the_block_size)
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
MArray<T>::MArray(const T *val, int length, int the_block_size)
  : GArray<T>(val, length, the_block_size)
  {
  }

///////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// @param sour - Source array
template <class T>
MArray<T>::MArray(const MArray<T> &sour) : GArray<T>(sour)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison operator.
///
/// @param[in] another Another array to compare.
///
/// @return Result of element by element comparision.
///
template <class T>
bool MArray<T>::operator == (const MArray<T> &another) const
  {
  int len = Length();
  if (another.Length() != len)
    return false;
  for (int i = 0; i < len; i++)
    {
    if (this->data[i] != another.data[i])
      return false;
    }
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison operator.
///
/// @param[in] another Another array to compare.
///
/// @return Result of element by element comparision.
///
template <class T>
bool MArray<T>::operator != (const MArray<T> &another) const
  {
  return !(this->operator == (another));
  }

///////////////////////////////////////////////////////////////////////////////
/// Scale elements value to the given factor.
/// The method multiplies all elements by the specified one.
/// @note T type must have 'operator *=" defined.
/// @param[in] c - A multiplier.
/// @return A reference to "this" array.
template <class T>
MArray<T> &MArray<T>::operator *=(const double &c)
  {
  for (int i = 0; i < this->count; i++)
    data[i] = (T)(data[i] * c);
  return *this;
  }  // 'operator *=()'

///////////////////////////////////////////////////////////////////////////////
/// Add elements of the given array to elements of 'this' array.
/// @note T type must have 'operator +=" defined.
///       Arrays must have the same length.
/// @param[in] sour - Added array.
/// @return A reference to "this" array.
template <class T>
MArray<T> &MArray<T>::operator += (const TArray<T> &sour)
  {
  Assert(count == sour.count);
  for (int i = 0; i < this->count; i++)
    data[i] += sour.data[i];
  return *this;
  }


///////////////////////////////////////////////////////////////////////////////
/// Sorts elements in the ascending order.
///
/// @param[in, out] data  - Elements to be sorted.
/// @param[in] count - Number of the elements in the array.
template <class T>
void MArray<T>::QSort(T *data, int count)
  {
  // variables used for stack
  Assert(count <= 2147483647);  // stack size restriction
  const int QS_STACK_SIZE = 130;  // should be enough for 32
  int stack[QS_STACK_SIZE];
  int stck_top = 0; // top of the stack

  // Variables used for sorting
  int i, j, k;
  int ir = count - 1; // right element
  int il = 0; // left element

  // Partitioning element
  T a;

  for ( ; ; )
    {
    // Insertion sort
    if (ir - il <= 6)
      {
      for (j = il + 1; j <= ir; j++)
        {
        a = data[j];
        for (i = j - 1; i >= il; i--)
          {
          if (data[i] < a || data[i] == a) 
            break;
          data[i + 1] = data[i];
          }
        data[i + 1] = a;
        }
      if (stck_top == 0) 
        break;

      // Pop the stack
      ir = stack[stck_top--];
      il = stack[stck_top--];
      }
    else
      {
      k = (il + ir) >> 1; // Choose median of left, center, and right elements
                          // as partitioning element a. Also rearrange so that
                          // data[l] # data[l+1] # data[ir]

      Swap(data[k], data[il + 1]);
      if (data[ir] < data[il])
        Swap(data[il], data[ir]);
      if (data[ir] < data[il + 1])
        Swap(data[il + 1], data[ir]);
      if (data[il + 1] < data[il])
        Swap(data[il], data[il + 1]);

      i = il + 1; // Initialize pointers for partitioning
      j = ir;
      a = data[il + 1]; // Partitioning element

      for ( ; ; )
        {
        do
          i++;
        while (data[i] < a); // Scan up to find element > a
        do
          j--;
        while (a < data[j]); // Scan down to find element < a
        if (j < i)
          break; // Pointers crossed. Partitioning complete
        Swap(data[i], data[j]); // Exchange elements
        }

      data[il + 1] = data[j]; // Insert partitioning element
      data[j] = a;
      stck_top += 2;
      Assert(stck_top < QS_STACK_SIZE);

      if (ir - i + 1 >= j - il)
        {
        stack[stck_top] = ir;
        stack[stck_top - 1] = i;
        ir = j - 1;
        }
      else
        {
        stack[stck_top] = j - 1;
        stack[stck_top - 1] = il;
        il = i;
        }
      }
    }
  } // MArray<T>::QSort()

///////////////////////////////////////////////////////////////////////////////
/// Defines ascending order of the elements in array.
///
/// Order of the elements in @a data array is not changed. Resulting contents
/// of the @a perm array defines the ascending order of the @a data array,
/// cf. the output @a perm array satisfies the following conditions:
/// - perm[i] <> perm[i + 1];
/// - data[perm[i]] <= data[perm[i + 1]] for i = 0, ... count - 2.
///
/// @param[in] data  - Elements to be sorted.
/// @param[in] count - Number of the elements in the array.
/// @param[out] perm - Result of sorting.
template <class T>
void MArray<T>::QSort(const T *data, int count, TArray<int> &perm)
  {
  // variables used for stack
  Assert(count <= 2147483647);  // stack size restriction
  const int QS_STACK_SIZE = 130;  // should be enough for 32
  int stack[QS_STACK_SIZE];
  int stck_top = 0; // top of the stack
  Assert(count <= perm.Length());

  // Variables used for sorting
  int i, j, k;
  int ir = count - 1; // right element
  int il = 0; // left element

  // Partitioning element
  T a;
  int b;

  for ( ; ; )
    {
    // Insertion sort
    if (ir - il <= 6)
      {
      for (j = il + 1; j <= ir; j++)
        {
        a = data[perm[j]];
        b = perm[j];
        for (i = j - 1; i >= il; i--)
          {
          if (data[perm[i]] < a || data[perm[i]] == a) 
            break;
          perm[i + 1] = perm[i];
          }
        perm[i + 1] = b;
        }
      if (stck_top == 0) 
        break;

      // Pop the stack
      ir = stack[stck_top--];
      il = stack[stck_top--];
      }
    else
      {
      k = (il + ir) >> 1; // Choose median of left, center, and right elements
                          // as partitioning element a. Also rearrange so that
                          // data[l] # data[l+1] # data[ir]

      Swap(perm[k], perm[il + 1]);
      if (data[perm[ir]] < data[perm[il]])
        Swap(perm[il], perm[ir]);
      if (data[perm[ir]] < data[perm[il + 1]])
        Swap(perm[il + 1], perm[ir]);
      if (data[perm[il + 1]] < data[perm[il]])
        Swap(perm[il], perm[il + 1]);

      i = il + 1; // Initialize pointers for partitioning
      j = ir;
      a = data[perm[il + 1]]; // Partitioning element
      b = perm[il + 1];

      for ( ; ; )
        {
        do
          i++;
        while (data[perm[i]] < a); // Scan up to find element > a
        do
          j--;
        while (a < data[perm[j]]); // Scan down to find element < a
        if (j < i)
          break; // Pointers crossed. Partitioning complete
        Swap(perm[i], perm[j]); // Exchange elements
        }

      perm[il + 1] = perm[j]; // Insert partitioning element
      perm[j] = b;
      stck_top += 2;
      Assert(stck_top < QS_STACK_SIZE);

      if (ir - i + 1 >= j - il)
        {
        stack[stck_top] = ir;
        stack[stck_top - 1] = i;
        ir = j - 1;
        }
      else
        {
        stack[stck_top] = j - 1;
        stack[stck_top - 1] = il;
        il = i;
        }
      }
    }
  } // MArray<T>::QSort()

//////////////////////////////////////////////////////////////////////////////
/// Sorts elements of T type by the comparison function.
///
/// Sorts elements in the ascending order. Elements comparison is realized by
/// the third parameter of the method (function @a less()) instead of operator
/// @a <. This method provides sorting in any order depending on proper chosen
/// function @a less().
///
/// @param[in, out] data  - Elements to be sorted.
/// @param[in] count - Number of the elements in the array.
/// @param[in] less  - Pointer to the function. This function compares two
/// elements @a a and @a b of the T type and returns true if first element less
/// than second element.
template <class T>
void MArray<T>::QSort(T *data, int count, bool (*less)(const T &a, const T&b))
  {
  // variables used for stack
  Assert(count <= 2147483647);  // stack size restriction
  const int QS_STACK_SIZE = 130;  // should be enough for 32
  int stack[QS_STACK_SIZE];
  int stck_top = 0; // top of the stack

  // Variables used for sorting
  int i, j, k;
  int ir = count - 1; // right element
  int il = 0; // left element

  // Partitioning element
  T a;

  for ( ; ; )
    {
    // Insertion sort
    if (ir - il <= 6)
      {
      for (j = il + 1; j <= ir; j++)
        {
        a = data[j];
        for (i = j - 1; i >= il; i--)
          {
          if (less(data[i],a) || (!less(data[i], a) && !less(a, data[i])))
            break;
          data[i + 1] = data[i];
          }
        data[i + 1] = a;
        }
      if (stck_top == 0) break;

      // Pop the stack
      ir = stack[stck_top--];
      il = stack[stck_top--];
      }
    else
      {
      k = (il + ir) >> 1; // Choose median of left, center, and right elements
                          // as partitioning element a. Also rearrange so that
                          // data[l] # data[l+1] # data[ir]

      Swap(data[k], data[il + 1]);
      if (less(data[ir], data[il]))
        Swap(data[il], data[ir]);
      if (less(data[ir], data[il + 1]))
        Swap(data[il + 1], data[ir]);
      if (less(data[il + 1], data[il]))
        Swap(data[il], data[il + 1]);

      i = il + 1; // Initialize pointers for partitioning
      j = ir;
      a = data[il + 1]; // Partitioning element

      for ( ; ; )
        {
        do i++; while (less(data[i], a)); // Scan up to find element > a
        do j--; while (less(a, data[j])); // Scan down to find element < a
        if (j < i) break; // Pointers crossed. Partitioning complete
        Swap(data[i], data[j]); // Exchange elements
        }

      data[il + 1] = data[j]; // Insert partitioning element
      data[j] = a;
      stck_top += 2;
      Assert(stck_top < QS_STACK_SIZE);

      if (ir - i + 1 >= j - il)
        {
        stack[stck_top] = ir;
        stack[stck_top - 1] = i;
        ir = j - 1;
        }
      else
        {
        stack[stck_top] = j - 1;
        stack[stck_top - 1] = il;
        il = i;
        }
      }
    }
  }

///////////////////////////////////////////////////////////////////////////////
/// Sorts all elements of this array in the ascending order.
template <class T>
void MArray<T>::QSort()
  {
  QSort(this->data, this->count);
  }

///////////////////////////////////////////////////////////////////////////////
/// Pack elements of this array - leave only non-equal.
///
/// Remaining elements are sorted in the ascending order.
template <class T>
void MArray<T>::Pack()
  {
  QSort();
  int prev = 0;
  for (int i = 1; i < count; i++)
    {
    if (data[i] == data[prev])
      continue;
    data[++prev] = data[i];
    }
  Resize(prev + 1);
  }

///////////////////////////////////////////////////////////////////////////////
/// Defines ascending order of the elements in this array.
///
/// Order of the elements in this array is not changed. Resulting contents of
/// the @a perm array defines the ascending order of this array, cf. the output
/// @a perm array satisfies the following conditions:
/// - perm[i] <> perm[i + 1];
/// - data[perm[i]] <= data[perm[i + 1]] for i = 0, ... @a n - 2, where @a data
/// is this array, @a n - length of the array.
///
/// @param[out] perm  - Result of sorting.
template <class T>
void MArray<T>::QSort(TArray<int> &perm) const
  {
  QSort(this->data, this->count, perm);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sorts first @a len elements of this array in the ascending order.
///
/// @param len - Number of the first sorted elements of this array.
template <class T>
void MArray<T>::QSort(int len)
  {
  QSort(this->data, len);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sorts all elements of this array by the comparison function.
///
/// Sorts elements in the ascending order. Elements comparison is realized by
/// the parameter of the method (function @a less()) instead of operator @a <.
/// This method provides sorting in any order depending on proper chosen
/// function @a less().
///
/// @param[in] less  - Pointer to the function. This function compares two
/// elements @a a and @a b of the T type and returns true if first element
/// less than second element.
template <class T>
void MArray<T>::QSort(bool (*less)(const T &a, const T &b))
  {
  QSort(this->data, this->count, less);
  }

///////////////////////////////////////////////////////////////////////////////
/// Sorts first @a len elements of this array by the comparison function.
///
/// Sorts elements in the ascending order. Elements comparison is realized by
/// the second parameter of the method (function @a less()) instead of operator
/// @a <.
/// This method provides sorting in any order depending on proper chosen
/// function @a less().
///
/// @param len - Number of the first sorted elements of this array.
/// @param[in] less  - Pointer to the function. This function compares two
/// elements @a a and @a b of the T type and returns true if first element less
/// than second element.
template <class T>
void MArray<T>::QSort(int len, bool (*less)(const T &a, const T &b))
  {
  QSort(this->data, len, less);
  }

///////////////////////////////////////////////////////////////////////////////
/// Finds position of value @a x in an ASCENDING array.
///
/// Finds position of value @a x in an ASCENDING array @a data i.e. index i
/// such that data[i-1] <= x < data[i].
///
/// If @a x is outside the array ranges, then:
/// - i = 0 if x < data[0] or
/// - i = n if x >= data[n - 1],  where @a n is length of the array.
///
/// @param[in] x    - Value whose position in array is to be found.
/// @param[in] data - Data array.
/// @param[in] n    - Length of the array.
/// @return Obtained index.
template <class T>
int MArray<T>::BinarySearch(const T &x, const T *data, int n)
  {
  int mid, low, high;
  low = 0;
  high = n - 1;
//  if (x < data[0])
//    return 0;
//
  // Bisection: find integer "root"
  while (low <= high)
    {
    mid = (low + high) >> 1;
    if (x < data[mid])
      high = mid - 1;
    else
      low = mid + 1;
    }
  Assert((0 <= low) && (low <= n));;
  return low;
  }  // BinarySearch()

///////////////////////////////////////////////////////////////////////////////
/// Finds position of value @a x in an ASCENDING array.
///
/// Finds position of value @a x in an ASCENDING array i.e. index i
/// such that data[i-1] <= x < data[i], where data is array of elements.
///
/// If @a x is outside the array ranges, then:
/// - i = 0 if x < data[0] or
/// - i = n if x >= data[n - 1],  where @a n is length of the array
///
/// @param[in] x - Value whose position in array is to be found.
/// @return Obtained index.
template <class T>
int MArray<T>::BinarySearch(const T &x) const
  {
  return BinarySearch(x, this->data, this->count);
  }

///////////////////////////////////////////////////////////////////////////////
/// Finds position of value @a x in a DESCENDING array.
///
/// Finds position of value @a x in an DESCENDING array @a data i.e. index i
/// such that data[i] < x <= data[i-1].
///
/// If @a x is outside the array ranges, then:
/// - i = n if x <= data[n-1], where @a n is length of the array or
/// - i = 0 if x > data[0].
///
/// @param[in] x    - Value whose position in array is to be found.
/// @param[in] data - Data array.
/// @param[in] n    - Length of the array.
/// @return Obtained index.
template <class T>
int MArray<T>::BinarySearchRev(const T &x, const T* data, int n)
  {
  int mid, low, high;
  low = 0;
  high = n - 1;

//  if (data[0] < x)
//    return 0;
//
  // Bisection: find integer "root"
  while (low <= high)
    {
    mid = (low + high) >> 1;
    if (data[mid] < x)
      high = mid - 1;
    else
      low = mid + 1;
    }
  Assert((0 <= low) && (low <= n));;
  return low;
  }  // BinarySearchRev()

///////////////////////////////////////////////////////////////////////////////
/// Finds position of value @a x in a DESCENDING this array.
///
/// Finds position of value @a x in an DESCENDING array i.e. index i
/// such that data[i] < x <= data[i-1], where data is array of elements.
///
/// If @a x is outside the array ranges, then:
/// - i = n if x <= data[n - 1] where @a n length of the array or
/// - i = 0 if x > data[0].
///
/// @param[in] x    - Value whose position in array is to be found.
/// @return Obtained index.
template <class T>
int MArray<T>::BinarySearchRev(const T &x) const
  {
  return BinarySearchRev(x, this->data, this->count);
  }

//////////////////////////////////////////////////////////////////////////////
/// Auxiliary class for StableSort - keeps value and it's initial order.
template <class T>
class ValOrd
  {
  public:
    /// Default Constructor.
    inline ValOrd();
    /// Constructor.
    inline ValOrd(T val, int ord);
    /// Operator ==
    inline bool operator == (const ValOrd<T> &u) const;
    /// Operator <
    inline bool operator < (const ValOrd<T> &u) const;
  public:
    /// Value.
    T m_val;
    /// Initial order of value in array.
    int m_ord;
  };

//////////////////////////////////////////////////////////////////////////////
/// Default Constructor.
template <class T>
ValOrd<T>::ValOrd()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// @param val Value.
/// @param ord Initial order of value in array.
template <class T>
ValOrd<T>::ValOrd(T val, int ord)
  {
  m_val = val;
  m_ord = ord;
  }

//////////////////////////////////////////////////////////////////////////////
/// Operator ==
///
/// @param u Operand for comparison.
/// @return Result of comparison.
template <class T>
bool ValOrd<T>::operator == (const ValOrd<T> &u) const
  {
  return (m_val == u.m_val && m_ord == u.m_ord);
  }

//////////////////////////////////////////////////////////////////////////////
/// Operator <
///
/// @param u Operand for comparison.
/// @return Result of comparison.
template <class T>
bool ValOrd<T>::operator < (const ValOrd<T> &u) const
  {
  return (m_val == u.m_val ? (m_ord < u.m_ord) : (m_val < u.m_val));
  }

//////////////////////////////////////////////////////////////////////////////
/// Defines ascending order of the elements in array.
/// Preserves relative order of the equivalent elements.
///
/// Order of the elements in @a data array is not changed. Resulting contents
/// of the @a perm array defines the ascending order of the @a data array, cf.
/// the output @a perm array satisfies the following conditions:
/// - perm[i] <> perm[i + 1];
/// - data[perm[i]] <= data[perm[i + 1]] for i = 0, ... count - 2.
///
/// @param[in] data  - Elements to be sorted.
/// @param[in] count - Number of the elements in the array.
/// @param[out] perm - Result of sorting.
template <class T>
void MArray<T>::StableSort(const T *data, int count, TArray<int> &perm)
  {
  // Auxiliary array of elements containing value together with order.
  MArray<ValOrd<T> > sort_arr;
  sort_arr.Allocate(count);

  for (int i = 0; i < count; i++)
    sort_arr[i] = ValOrd<T>(data[i], i);

  MArray<ValOrd<T> >::QSort(sort_arr.Data(), count, perm);
  }

//////////////////////////////////////////////////////////////////////////////
/// Sorts elements in the ascending order.
/// Preserves relative order of the equivalent elements.
///
/// @param[in, out] data  - Elements to be sorted.
/// @param[in] count - Number of the elements in the array.
template <class T>
void MArray<T>::StableSort(T *data, int count)
  {
  // Auxiliary array of elements containing value together with order.
  MArray<ValOrd<T> > sort_arr;
  sort_arr.Allocate(count);

  for (int i = 0; i < count; i++)
    sort_arr[i] = ValOrd<T>(data[i], i);

  MArray<ValOrd<T> >::QSort(sort_arr.Data(), count);

  for (int i = 0; i < count; i++)
    data[i] = sort_arr[i].m_val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Auxiliary class for StableSortLessEqual - keeps value and it's initial
/// order and uses template functions, which should be supported by the
/// object:
/// bool Less(const T &b) const
/// bool Equal(const T &b) const
template <class T>
class ValOrdLessEqual
  {
  public:
    /// Default Constructor.
    inline ValOrdLessEqual();
    /// Constructor.
    inline ValOrdLessEqual(T val, int ord);
    /// Operator ==
    inline bool operator == (const ValOrdLessEqual<T> &u) const;
    /// Operator <
    inline bool operator < (const ValOrdLessEqual<T> &u) const;
  public:
    /// Value.
    T m_val;
    /// Initial order of value in array.
    int m_ord;
  };

//////////////////////////////////////////////////////////////////////////////
/// Default Constructor.
template <class T>
ValOrdLessEqual<T>::ValOrdLessEqual()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// @param val Value.
/// @param ord Initial order of value in array.
template <class T>
ValOrdLessEqual<T>::ValOrdLessEqual(T val, int ord)
: m_val(val), m_ord(ord)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Operator ==
///
/// @param u Operand for comparison.
/// @return Result of comparison.
template <class T>
bool ValOrdLessEqual<T>::operator == (const ValOrdLessEqual<T> &u) const
  {
  return (m_val.Equal(u.m_val) && m_ord == u.m_ord);
  }

//////////////////////////////////////////////////////////////////////////////
/// Operator <
///
/// @param u Operand for comparison.
/// @return Result of comparison.
template <class T>
bool ValOrdLessEqual<T>::operator < (const ValOrdLessEqual<T> &u) const
  {
  return (m_val.Equal(u.m_val) ? (m_ord < u.m_ord) : (m_val.Less(u.m_val)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Sorts array of the elements of T type by the comparison
/// function, preserving relative order of equivalent elements.
/// Uses template functions, which should be supported by the object:
/// bool Less(const T &b) const
/// bool Equal(const T &b) const
/// @param[in] data Pointer to array to be sorted.
/// @param[in] count Number of elements in the array.
template <class T>
void MArray<T>::StableSortLessEqual(T *data, int count)
  {
  // Auxiliary array of elements containing value together with order.
  MArray<ValOrdLessEqual<T> > sort_arr;
  sort_arr.Allocate(count);

  for (int i = 0; i < count; i++)
    sort_arr[i] = ValOrdLessEqual<T>(data[i], i);

  MArray<ValOrdLessEqual<T> >::QSort(sort_arr.Data(), count);

  for (int i = 0; i < count; i++)
    data[i] = sort_arr[i].m_val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Sorts all elements of this array in the ascending order.
/// Preserves relative order of the equivalent elements.
template <class T>
void MArray<T>::StableSort()
  {
  StableSort(this->data, this->count);
  }

//////////////////////////////////////////////////////////////////////////////
/// Find maximal value in the array.
/// @return Maximal value in the array.
/// @note If array has 0 length - return value is undefined.
template <class T>
T MArray<T>::MaxValue() const
  {
  T max_value(0);
  if (this->count < 1)
    return max_value;
  max_value = this->data[0];
  for (int i = 1; i < this->count; i++)
    {
    if (max_value < this->data[i])
      max_value = this->data[i];
    }
  return max_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find minimal value in the array.
/// @return Minimal value in the array.
/// @note If array has 0 length - return value is undefined.
template <class T>
T MArray<T>::MinValue() const
  {
  T min_value(0);
  if (this->count < 1)
    return min_value;
  min_value = this->data[0];
  for (int i = 1; i < this->count; i++)
    {
    if (this->data[i] < min_value)
      min_value = this->data[i];
    }
  return min_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find index of element with maximal value.
/// @return Index of element with maximal value.
/// @note If array has 0 length - -1 is returned. 
/// If several elements have maximal value - the first is returned.
template <class T>
int MArray<T>::MaxValueIndex() const
  {
  T max_value(0);
  ind = 0;
  if (this->count < 1)
    return -1;
  ind = 0;
  max_value = this->data[0];
  for (int i = 1; i < this->count; i++)
    {
    if (max_value < this->data[i])
      {
      max_value = this->data[i];
      ind = i;
      }
    }
  return ind;
  }

//////////////////////////////////////////////////////////////////////////////
/// Calculate a sum of values in the array.
/// @return A sum of values in the array.
template <class T>
T MArray<T>::Sum() const
  {
  T sum(0);
  for (int i = 0; i < this->count; i++)
    sum += this->data[i];
  return sum;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new array, resized with linear interpolation.
///
/// @param n New number of the elements.
/// @param out Created array.
/// @return SUCCESS if array is created successfully, FAILURE - otherwise.
template <class T>
OKAY MArray<T>::ResizeLinear(int n, MArray<T> &out) const
  {
  Assert(n > 0 && count > 0);
  if (n == count)
    {
    out = *this;
    return SUCCESS;
    }

  if (out.Allocate(n) != SUCCESS)
    return FAILURE;

  if (n == 1)
    {
    out[0] = (*this)[0];
    return SUCCESS;
    }

  for (int i = 0; i < n; i++)
    {
    double row = double(i) * (count - 1) / double(n - 1);
    int r = (int)floor(row);
    int rn = r + 1;
    if (rn > count - 1)
      rn = count - 1;
    double u = row - r;

    out[i] = (*this)[r]  * (1.0 - u) + (*this)[rn] * u;
    }  // for (int i = 0; i < n; i++)

  return SUCCESS;
  }


INTEGRA_NAMESPACE_END
#endif
