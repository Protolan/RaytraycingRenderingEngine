/// @file
///
/// @brief Three-dimensional matrix of arbitrary elements with basic operations.
///
/// Copyright &copy; INTEGRA, Inc., 2003-2004.
///
/// @internal
/// @author Ose - Sergey Ershov, '03.04.25.

#ifndef KLBM_TMATRIX3D_HPP
#define KLBM_TMATRIX3D_HPP

#include <math/vect2.hpp>

#include "barrays.hpp"

INTEGRA_NAMESPACE_START

/// Typified three-dimensional matrix A(i,j,k).
template <class T> class TMatrix3D
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Destructor.
    virtual ~TMatrix3D();
    /// Default constructor.
    inline explicit TMatrix3D();
    /// Constructor.
    inline explicit TMatrix3D(int n1, int n2, int n3);
    /// Copy constructor.
    inline TMatrix3D(const TMatrix3D<T> &m);
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline TMatrix3D &operator =(const TMatrix3D<T> &src);
    /// Set all elements to the same value.
    inline TMatrix3D &operator =(const T &c);
    /// Multiply all elements by the same value.
    inline TMatrix3D &operator *=(const double &c);
    //@}

  public:
    /// @name Comparison
    //@{
    /// Comparison of matrices.
    bool operator ==(const TMatrix3D<T> &src) const;
    //@}

  public:
    /// @name Allocation, query
    //@{
    /// Set dimensions and (re)allocate memory, data are not retained.
    inline OKAY Allocate(int n1, int n2, int n3);
    /// Set dimensions and (re)allocate memory, retain data.
    inline OKAY Resize(int n1, int n2, int n3);
    /// Get the specified dimension.
    inline int Dimension(int i) const;
    /// Get whole matrix data length.
    int Length() const;
    //@}

  public:
    /// @name Access data
    //@{
    /// Access to an element.
    inline const T &operator()(int i, int j, int k) const;
    /// Access to an element.
    inline T &operator()(int i, int j, int k);
    /// Access a matrix "line" in the last dimension.
    inline SArray<T> operator()(int i, int j);
    /// Access a matrix "line" in the last dimension.
    inline const SArray<T> operator()(int i, int j) const;
    /// Access a raw "slice" in the last dimension.
    inline T *Slice(int i, int j);
    /// Access a raw "slice" in the last dimension.
    inline const T *Slice(int i, int j) const;
    /// Get a pointer to the matrix.
    inline T *Data();
    /// Direct access to the data of this matrix.
    inline const T *Data() const;
    /// Size of the data.
    inline SIZE_T Size() const;
    //@}

  public:
    /// @name Auxiliary methods
    //@{
    /// Fast exchange of two matrices.
    static void SwapMatrix(TMatrix3D<T> &m1, TMatrix3D<T> &m2);
    /// Flip matrix in the vertical direction - along rows (direction with size m_n1).
    inline void VertFlip();
    /// Flip matrix in the horizontal directions - along columns (direction with size m_n2).
    inline void HorFlip();
    /// Crop matrix by specified region in X and Y directions.
    inline OKAY Crop(const Point2i &beg, const Vect2i &size);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Begin serialization.
    inline void BegChunk(Serializer &inout, const char *tag);
    /// End serialization.
    inline void EndChunk(Serializer &inout);
    //@}

  private:
    /// @name Dimensions
    //@{
    /// First dimension (rows).
    int m_n1;
    /// Second dimension (columns).
    int m_n2;
    /// Third dimension.
    int m_n3;
    //@}

    /// Memory block.
    T *m_data;
  };  // class TMatrix3D


//////////////////////////////////////////////////////////////////////////////
// Methods  of class TMatrix3D


//////////////////////////////////////////////////////////////////////////////
/// Destructor.
template <class T>
TMatrix3D<T>::~TMatrix3D()
  {
  Allocate(0, 0, 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method constructs an empty matrix (all dimensions = 0).
template <class T>
TMatrix3D<T>::TMatrix3D() : 
  m_data(NULL), 
  m_n1(0), m_n2(0), m_n3(0)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// Constructs a matrix of desired dimensions.
/// @param[in] n1 - The first dimension.
/// @param[in] n2 - The second dimension.
/// @param[in] n3 - The third dimension.
template <class T>
TMatrix3D<T>::TMatrix3D(int n1, int n2, int n3)
  {
  m_n1 = m_n2 = m_n3 = 0;
  m_data = NULL;
  Allocate(n1, n2, n3);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] m - A source matrix.
template <class T>
TMatrix3D<T>::TMatrix3D(TMatrix3D<T> const &m)
  {
  m_n1 = m_n2 = m_n3 = 0;
  m_data = NULL;
  *this = m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method copies data and dimensions from a source to the destination,
/// resizing the destination if needed.
/// @param[in] src - A source matrix.
/// @return A reference to this matrix.
template <class T>
TMatrix3D<T> &TMatrix3D<T>::operator =(TMatrix3D<T> const &src)
  {
  if (this != &src) // obviate "a=a"
    {
    if (Allocate(src.m_n1, src.m_n2, src.m_n3) == SUCCESS)
      {
      // we don't use 'memcpy' because elements may contain pointers...
      SIZE_T i, n = (SIZE_T)m_n1 * m_n2 * m_n3;
      for (i = 0; i < n; i++)
        m_data[i] = src.m_data[i];
      }
    }
  return *this;
  }  // operator =()


//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
/// @param[in] c - The value to assign to all elements.
/// @return A reference to this matrix.
template <class T>
TMatrix3D<T> &TMatrix3D<T>::operator =(const T &c)
  {
  // we don't use 'memcpy' because elements may contain pointers...
  SIZE_T i, n = (SIZE_T)m_n1 * m_n2 * m_n3;
  for (i = 0; i < n; i++)
    m_data[i] = c;
  return *this;
  }  // operator =()

//////////////////////////////////////////////////////////////////////////////
/// Multiplies all elements by the same value.
/// @param[in] c A scale factor.
/// @return A reference to this matrix.
template <class T>
TMatrix3D<T> &TMatrix3D<T>::operator *=(const double &c)
  {
  SIZE_T i, n = (SIZE_T)m_n1 * m_n2 * m_n3;
  for (i = 0; i < n; i++)
    m_data[i] *= c;
  return *this;
  }  // operator *=()

//////////////////////////////////////////////////////////////////////////////
/// Comparison of matrices.
///
/// The method compares two matrices on the per-element basis.
/// @param[in] src A matrix to compare with.
/// @return @b true, if all the elements are correspondently equal;
/// @b false otherwise.
template <class T>
bool TMatrix3D<T>::operator ==(const TMatrix3D<T> &src) const
  {
  if (m_n1 != src.m_n1 || m_n2 != src.m_n2 || m_n3 != src.m_n3)
    return false;
  SIZE_T n = (SIZE_T)m_n1 * m_n2 * m_n3;
  for (SIZE_T i = 0; i < n; i++)
    if (!(m_data[i] == src.m_data[i]))
      return false;
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set dimensions and (re)allocate memory, data are not retained.
///
/// If the target size of block (m_n1*m_n2*m_n3) coincides with
/// the current one, method does not re-allocate it.
/// @param[in] n1 - The first dimension.
/// @param[in] n2 - The second dimension.
/// @param[in] n3 - The third dimension.
/// @return SUCCESS / FAILURE (allocation failure).
template <class T>
OKAY TMatrix3D<T>::Allocate(int n1, int n2, int n3)
  {
  if (n1 < 0 || n2 < 0 || n3 < 0)
    {
    Assert(false);
    return FAILURE;
    }

  if (n1 > 0 && n2 > 0 && n3 > 0)
    {
    // check n1*n2*n3*sizeof(T) >= MAX_SIZE_T in such a way as
    // to avoid possible overflows for huge n and m
    if ((((MAX_SIZE_T / n1) / n2) / n3) < sizeof(T))
      {
      m_n1 = m_n2 = m_n3 = 0;
      if (m_data != NULL)
        delete m_data;
      m_data = NULL;
      return FAILURE;
      }
    }


  SIZE_T new_len = (SIZE_T)n1 * n2 * n3;
  SIZE_T prev_len = (SIZE_T)m_n1  * m_n2  * m_n3;

  if (prev_len != new_len || (new_len > 0 && m_data == NULL))
    {
    // free existing memory
    if (prev_len > 0 && m_data != NULL)
      delete [] m_data;
    m_data = NULL;

    // and alocate new block
    if (new_len > 0)
      {
      m_data = new T[new_len];
      if (m_data == NULL)
        {
        m_n1 = m_n2 = m_n3 = 0;
        return FAILURE;
        }
      }
    }
  m_n1 = n1;
  m_n2 = n2;
  m_n3 = n3;

  return SUCCESS;
  }  // Allocate()

//////////////////////////////////////////////////////////////////////////////
/// Set dimensions and (re)allocate memory, retain data.
///
/// If the target size of block (m_n1*m_n2*m_n3) coincides with
/// the current one, method does not re-allocate it.
/// @param[in] n1 - The first dimension.
/// @param[in] n2 - The second dimension.
/// @param[in] n3 - The third dimension.
/// @return SUCCESS / FAILURE (allocation failure).
template <class T>
OKAY TMatrix3D<T>::Resize(int n1, int n2, int n3)
  {
  if (n1 < 0 || n2 < 0 || n3 < 0)
    {
    Assert(false);
    return FAILURE;
    }

  if (n1 > 0 && n2 > 0 && n3 > 0)
    {
    // check n1*n2*n3*sizeof(T) >= MAX_SIZE_T in such a way as
    // to avoid possible overflows for huge n and m
    if ((((MAX_SIZE_T / n1) / n2) / n3) < sizeof(T))
      {
      m_n1 = m_n2 = m_n3 = 0;
      if (m_data != NULL)
        delete m_data;
      m_data = NULL;
      return FAILURE;
      }
    }

  SIZE_T prev_len = (SIZE_T)m_n1  * m_n2  * m_n3;

  if (m_data == NULL || prev_len == 0)  // no data!
    return Allocate(n1, n2, n3);
  else // we need to keep old data
    {
    // First create a temporary matrix of desired (new) dimensions
    TMatrix3D<T> b(n1, n2, n3);
    if (b.m_data == NULL && n1 !=0 && n2 != 0 && n3 != 0)
      return FAILURE;

    // Then transfer data from current matrix into the new one
    int min_n1 = Min(m_n1, n1);
    int min_n2 = Min(m_n2, n2);
    int min_n3 = Min(m_n3, n3);
    for (int i = 0; i < min_n1; i++)
      {
      for (int j = 0; j < min_n2; j++)
        {
        for (int k = 0; k < min_n3; k++)
          b(i, j, k) = (*this)(i, j, k);
        }
      }
    // and finally swap the temporary matrix and this one.
    SwapMatrix(b, *this);
    }

  return SUCCESS;
  }  // Resize()

//////////////////////////////////////////////////////////////////////////////
/// Get the specified dimension.
/// @param[in] i - the index whose dimension is queried: 0, 1 or 2.
/// Debug version asserts that index is not out of range.
/// @return The required dimension.
template <class T>
int TMatrix3D<T>::Dimension(int i) const
  {
  Assert(i >= 0 && i < 3);
  return *(&m_n1 + i);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get whole matrix data length.
/// @return The data length.
template <class T>
int TMatrix3D<T>::Length() const
  {
  return m_n1 * m_n2 * m_n3;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to an element.
/// @param[in] i - The first index.
/// @param[in] j - The second index.
/// @param[in] k - The third index.
/// @note Debug version asserts that the indices are not out of range.
/// @return A reference (l-value) to matrix(i,j,k).
template <class T>
T &TMatrix3D<T>::operator ()(int i, int j, int k)
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2 && k >= 0 && k < m_n3);
  return m_data[(SIZE_T)k + m_n3 * (j + m_n2 * i)];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to an element.
/// @param[in] i - The first index.
/// @param[in] j - The second index.
/// @param[in] k - The third index.
/// @note Debug version asserts that indices are not out of range.
/// @return A const reference (l-value) to matrix(i,j,k).
template <class T>
const T &TMatrix3D<T>::operator ()(int i, int j, int k) const
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2 && k >= 0 && k < m_n3);
  return m_data[(SIZE_T)k + m_n3 * (j + m_n2 * i)];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access a matrix "line" in the last dimension.
///
/// The method accesses a matrix "line" in the last dimension (like a row of
/// a two-dimensional matrix) as a protected (with index check) array
/// linked to the memory occupied by that row (i.e. no new memory is
/// allocated).
///
/// In other words, the object returned is a matrix slice in which only the
/// last index changes: @b A(i,j,*) in the FORTRAN style.
///
/// Therefore A(i,j)[k] contains the k-th element of this row, same as
/// A(i,j,k) but works a bit slower because the new object (yet tiny) of
/// array is created and a couple of members are copied.
///
/// When this array is destroyed, the matrix's memory remains intact.
/// Also when the matrix is destroyed or re-allocated, the SArray
/// representing its row becomes invalid which is not checked for, so
/// the caller must care about that.
///
/// Despite the array returned is a new object, it contains a reference
/// to the matrix row inside and the values of their elements change
/// synchronously because in fact these are the same elements. So
/// modifications in the matrix are immediately "reflected" in this array
/// because theirs are same elements. Likewise, one can modify the matrix
/// via it.
///
/// Assignment of one slice to another copies elements whose indices are
/// present in both sets so one can assign one row of matrix to another.
/// @param[in] i - The first index of the slice.
/// @param[in] j - The second index of the slice.
/// @note Debug version asserts that indices i,j are not out of range.
/// @return The array of the (i,j)-th "row".
template <class T>
SArray<T> TMatrix3D<T>::operator()(int i, int j)
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2);;
  return SArray<T>(m_data + (SIZE_T)m_n3 * (j + m_n2 * i), m_n3);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the matrix "line" in the last dimension.
///
/// The method accesses the matrix "line" in the last dimension (like a row of
/// a two-dimensional matrix) as a protected (with index check) array
/// linked to the memory occupied by that row (i.e. no new memory is
/// allocated).
///
/// In other words the object returned is a matrix slice in which only the
/// last index changes: @b A(i,j,*) in the FORTRAN style.
///
/// Therefore A(i,j)[k] contains the k-th element of this row, same as
/// A(i,j,k) but works a bit slower because a new object (yet tiny) of
/// the array is created and a couple of members are copied.
///
/// When this array is destroyed, the matrix's memory remains intact.
/// Also when the matrix is destroyed or re-allocated, the SArray
/// representing its row becomes invalid which is not checked for, so
/// the caller must care about that.
///
/// Despite the array returned is a new object, it contains a reference
/// to the matrix row inside and the values of their elements change
/// synchronously because in fact these are the same elements. So
/// modifications in the matrix are immediately "reflected" in this array
/// because theirs are same elements. But since the object returned is
/// "const" one can not modify matrix via it.
///
/// Assignment of one slice to another copies elements whose indices are
/// present in both sets so one can assign one row of a matrix to another.
/// @param[in] i - The first index of the slice.
/// @param[in] j - The second index of the slice.
/// @note Debug version asserts that indices i,j are not out of range.
/// @return The array of (i,j)-th "row".
template <class T>
const SArray<T> TMatrix3D<T>::operator()(int i, int j) const
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2);;
  return SArray<T>(m_data + (SIZE_T)m_n3 * (j + m_n2 * i), m_n3);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the raw "slice" in the last dimension.
///
/// The method accesses the raw "slice" in the last dimension i.e. retrieves
/// the starting address of the matrix "line" in which only the 3rd index 
/// changes while the first two indices are fixed (i,j).
/// @param[in] i - The first index of the slice.
/// @param[in] j - The second index of the slice.
/// @note Debug version asserts that indices are not out of range.
/// @return The address of the "section" in the last index.
template <class T>
T *TMatrix3D<T>::Slice(int i, int j)
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2);;
  return m_data + (SIZE_T)m_n3 * (j + m_n2 * i);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the raw "slice" in the last dimension.
///
/// The method accesses the raw "slice" in the last dimension i.e. retrieves
/// the starting address of the matrix "line" in which only the 3rd index 
/// changes while the first two indices are fixed (i,j).
/// @param[in] i - The first index of the slice.
/// @param[in] j - The second index of the slice.
/// @note Debug version asserts that indices are not out of range.
/// @return The address of the "section" in the last index.
template <class T>
const T *TMatrix3D<T>::Slice(int i, int j) const
  {
  Assert(i >= 0 && i < m_n1 && j >= 0 && j < m_n2);;
  return m_data + (SIZE_T)m_n3 * (j + m_n2 * i);
  }

//////////////////////////////////////////////////////////////////////////////
/// Fast exchange of two matrices.
///
/// The method exchanges two matrices in a fast fashion: just
/// swaps internal pointers without transposing elements.
/// @param[in] m1 - The first matrix to be swapped.
/// @param[in] m2 - The second matrix to be swapped.
template <class T> void
TMatrix3D<T>::SwapMatrix(TMatrix3D<T> &m1, TMatrix3D<T> &m2)
  {
  Swap(m1.m_n1, m2.m_n1);
  Swap(m1.m_n2, m2.m_n2);
  Swap(m1.m_n3, m2.m_n3);
  Swap(m1.m_data, m2.m_data);
  }

//////////////////////////////////////////////////////////////////////////////
/// Flip matrix in horizontal direction - along columns (direction with size m_n2).
template <class T>
void TMatrix3D<T>::HorFlip()
  {
  SIZE_T r_size = m_n2 * m_n3;
  for (int r = 0; r < m_n1; r++)
    {
    for (int c = 0; c < m_n2 / 2; c++)
      {
      for (int z = 0; z < m_n3; z++)
        Swap(*(m_data + r_size * r + c * m_n3 + z), *(m_data + r_size * r + (m_n2 - 1 - c) * m_n3 + z));
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Flip matrix in the vertical direction - along rows (direction with size m_n1).
template <class T>
void TMatrix3D<T>::VertFlip()
  {
  SIZE_T r_size = m_n2 * m_n3;
  for (int r = 0; r < m_n1 / 2; r++)
    {
    for (int c = 0; c < m_n2; c++)
      {
      for (int z = 0; z < m_n3; z++)
        Swap(*(m_data + r_size * r + c * m_n3 + z), *(m_data + r_size * (m_n1 - 1 - r) + m_n3 * c + z));
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Crop matrix by specified region in m_n2 and m_n1 directions.
/// @param[in] beg Starting point.
/// @param[in] size Size of clipped image.
/// @return SUCCESS in all cases.
template <class T>
OKAY TMatrix3D<T>::Crop(const Point2i &beg, const Vect2i &size)
  {
  Vect2i cur_res(m_n2, m_n1);
  Assert(beg.x < cur_res.x && beg.y < cur_res.y && beg.x + size.x <= cur_res.x && beg.y + size.y <= cur_res.y);
  for (int y = 0; y < size.y; y++)
    {
    for (int x = 0; x < size.x; x++)
      {
      for (int z = 0; z < m_n3; z++)
        m_data[((SIZE_T)cur_res.x * y + x) * m_n3 + z] = m_data[((SIZE_T)cur_res.x * (beg.y + y) + beg.x + x) * m_n3 + z];
      }
    }

  return Resize(size.y, size.x, m_n3);
  }


//////////////////////////////////////////////////////////////////////////////
/// Begin serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
/// @param[in] tag - A chunk name.
template <class T>
void TMatrix3D<T>::BegChunk(Serializer &inout, const char *tag)
  {
  int x = m_n1, y = m_n2, z = m_n3;
  inout.BegChunk(tag);
  inout.Value(x);
  inout.Value(y);
  inout.Value(z);
  if (inout.Import())
    {
    if (Allocate(x, y, z) != SUCCESS)
      inout.SetAllocError();
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// End serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
template <class T> void TMatrix3D<T>::EndChunk(Serializer &inout)
  {
  inout.EndChunk();
  }

///////////////////////////////////////////////////////////////////////////////
/// Direct access to the data of this matrix.
/// @return Pointer on the internal data.
template <class T>
T *TMatrix3D<T>::Data()
  {
  return m_data;
  }

///////////////////////////////////////////////////////////////////////////////
/// Direct access to the data of this matrix.
/// @return Pointer on the internal data.
template <class T>
const T *TMatrix3D<T>::Data() const
  {
  return m_data;
  }

///////////////////////////////////////////////////////////////////////////////
/// Size of the data.
/// @return Size of the internal data
template <class T>
SIZE_T TMatrix3D<T>::Size() const
  {
  return (SIZE_T)m_n1 * m_n2 * m_n3;
  }

INTEGRA_NAMESPACE_END
#endif
