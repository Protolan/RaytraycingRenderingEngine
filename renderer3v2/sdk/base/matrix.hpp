/// @file
///
/// @brief Declaration of TMatrix template class.
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Ose - Sergey Ershov, '00.05.01

#ifndef _RGNB_MATRIX_HPP_
#define _RGNB_MATRIX_HPP_

#include <math/vect2.hpp>

#include "barrays.hpp"
#include "serializer.hpp"

INTEGRA_NAMESPACE_START

/// Two-dimensional matrix of arbitrary elements with basic operations.
template <class T>
class TMatrix
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline explicit TMatrix();
    /// Constructor.
    inline explicit TMatrix(int n_r, int n_c);
    /// Constructor from the given values.
    inline TMatrix(const T *val, int n_r, int n_c);

    /// Copy constructor.
    inline TMatrix(const TMatrix<T> &m);
    /// Destructor.
    virtual ~TMatrix();
    //@}

  public:
    /// @name Resizing
    //@{
    /// Set dimensions and (re)allocate memory, retain data.
    inline OKAY Resize(int n_r, int n_c);
    /// Set dimensions and (re)allocate memory, data are not retained.
    inline OKAY Allocate(int n_r, int n_c);
    /// @brief Set dimensions and (re)allocate memory (minimum reallocations),
    /// data are not retained.
    inline OKAY AllocateFast(int n, int m);
    /// Create new matrix, scaled with cells stretching.
    inline OKAY Scale(int row_scale, int col_scale, TMatrix<T> &out) const;
    //@}

  public:
    /// @name Query dimensions
    //@{
    /// Get resolution of matrix: x - columns, y - rows.
    inline Vect2i GetRes() const;
    /// Get the number of rows.
    inline int NRows() const;
    /// Get the number of columns.
    inline int NColumns() const;
    /// Get number of elements in the value array.
    inline SIZE_T Length() const;
    /// Get the specified dimension.
    inline int Dimension(int i) const;
    //@}

  public:
    /// @name Addition and removal of rows and columns
    //@{
    /// Insert rows.
    inline OKAY InsertRows(int i, int n = 1, int m0 = -1);
    /// Inserts columns.
    inline OKAY InsertColumns(int i, int n = 1, int n0 = -1);
    /// Remove rows.
    inline OKAY RemoveRows(int i, int n = 1);
    /// Remove columns.
    inline OKAY RemoveColumns(int i, int n = 1);
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline TMatrix &operator =(const TMatrix<T> &src);
    /// Set all elements to the same value.
    inline TMatrix &operator =(const T &c);
    /// Set all elements to the same value.
    inline void Set(const T &val);
    //@}

    /// @name Arithmetic operators
    //@{
    /// Scale elements value to the given factor.
    inline TMatrix &operator *=(const double &c);
    /// Addition operator.
    inline TMatrix<T> &operator +=(const TMatrix<T> &m);
    //@}

    /// @name Comparison
    //@{
    /// Comparison of matrices on equality.
    bool operator ==(const TMatrix<T> &src) const;
    /// Comparison of matrices on inequality.
    bool operator !=(const TMatrix<T> &src) const;
    //@}

  public:
    /// @name Access data
    //@{
    /// Cast to a pointer to a matrix.
    inline operator T *();
    /// Cast to a const pointer to a matrix.
    inline operator const T *() const;
    /// Get a pointer to the matrix.
    inline T *Data();
    /// Get a const pointer to the matrix.
    inline const T *Data() const;
    /// Access the specified row.
    inline SArray<T> operator [](int i);
    /// Access the specified row.
    inline const SArray<T> operator [](int i) const;
    /// Access the specified row.
    inline T *Slice(int i);
    /// Access the specified row.
    inline const T *Slice(int i) const;
    /// Set row from the given pointer to row values.
    inline void SetRow(int row, const T *val_ptr);
    /// Access the specified element.
    inline T &operator ()(int i, int j);
    /// Access the specified element.
    inline const T &operator ()(int i, int j) const;
    /// Access the element wrapped in a linear array.
    inline T &operator ()(SIZE_T i);
    /// Access the element wrapped in a linear array.
    inline const T &operator ()(SIZE_T i) const;
    //@}

  public:
    /// @name Auxiliary methods
    //@{
    /// Find maximal value in the matrix.
    inline T MaxValue() const;
    /// Find minimal value in the matrix.
    inline T MinValue() const;
    /// Check - is the matrix empty - is not allocated.
    inline bool IsEmpty() const;
    /// Fast exchange of two matrices.
    static void SwapMatrix(TMatrix<T> &m1, TMatrix<T> &m2);
    /// Flip matrix in the vertical direction (along rows).
    inline void VertFlip();
    /// Flip matrix in the horizontal direction (along columns).
    inline void HorFlip();
    /// Crop matrix by specified region.
    inline OKAY Crop(const Point2i &beg, const Vect2i &size);
    /// Copy rectangle from another matrix.
    inline void CopyFrom(const TMatrix<T> &src, const Point2i &dst_beg, const Point2i &src_beg, const Vect2i &size);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Begin serialization.
    inline void BegChunk(Serializer &inout, const char *tag);
    /// End serialization.
    inline void EndChunk(Serializer &inout);
    //@}

  public:
    /// @name Scaling and zooming functions
    //@{
    /// Create new matrix, resized with bilinear interpolation.
    inline OKAY ResizeBilinear(int n_rows, int n_cols, TMatrix<T> &out) const;
    /// Create new matrix, resized with bilinear interpolation cyclically.
    inline OKAY ResizeBilinearCycled(int n_rows, int n_cols, TMatrix<T> &out, 
      bool cycle_rows, bool cycle_cols) const;
    /// Create new matrix, resized with bilinear interpolation cyclically with mask.
    inline OKAY ResizeBilinearCycledWithMask(int n_rows, int n_cols, TMatrix<T> &out, 
      bool cycle_rows, bool cycle_cols, const TMatrix<bool> &mask) const;
    /// Create new Matrix, scaled to half size.
    inline OKAY ScaleToHalf(TMatrix<T> &out) const;
    /// Create new Matrix, zoomed out to given factor.
    inline OKAY ZoomOut(int row_n, int col_n, TMatrix<T> &out) const;
    //@}
    /// Get interpolated pixel value, considering that data are cycled.
    inline void GetInterpPixCycled(const Vect2d &pnt, T &out) const;

  private:
    /// Data as a linear array.
    T *m_data;

  private:
    /// @name Dimensions
    /// @internal @warning Do not change layout of these two variables -
    /// method Dimension() depends on it.
    //@{
    /// Number of rows.
    int m_n_rows;
    /// Number of columns.
    int m_n_cols;
    //@}
  };  // class TMatrix

//////////////////////////////////////////////////////////////////////////////
// Methods of the class TMatrix

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method creates a matrix of zero dimensions.
template <class T>
TMatrix<T>::TMatrix()
  {
  m_n_rows = m_n_cols = 0;
  m_data = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method creates a matrix with the specified numbers of rows and columns.
/// All elements are initialised by the default constructor of type T.
/// @param[in] n_r - The number of rows.
/// @param[in] n_c - The number of columns.
template <class T>
TMatrix<T>::TMatrix(int n_r, int n_c)
  {
  m_n_rows = m_n_cols = 0;
  m_data = NULL;
  Allocate(n_r, n_c);
  }

///////////////////////////////////////////////////////////////////////////////
/// Constructor from the given values.
/// @param[in] val The given data array.
/// @param[in] n_r - The number of rows.
/// @param[in] n_c - The number of columns.
/// @note Is array succesfully constructed or not - can be controlled
/// via the Length() of the array, which is 0, if data allocation failed.
template <class T>
TMatrix<T>::TMatrix(const T *val, int n_r, int n_c)
  {
  m_n_rows = m_n_cols = 0;
  m_data = NULL;
  if (Allocate(n_r, n_c) != SUCCESS)
    return;
  SIZE_T len = (SIZE_T)n_r * (SIZE_T)n_c;
  for (SIZE_T i = 0; i < len; i++)
    m_data[i] = val[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
template <class T>
TMatrix<T>::~TMatrix()
  {
  Allocate(0, 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] m - A source object.
template <class T>
TMatrix<T>::TMatrix(TMatrix<T> const &m)
  {
  m_n_rows = m_n_cols = 0;
  m_data = NULL;
  *this = m;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// The method copies the specified matrix to "this" matrix.
/// The result is the matrix of the same dimensions as the source one.
/// Memory is reallocated if necessary.
/// @param[in] src - A source matrix.
/// @return A reference to "this" matrix.
template <class T>
TMatrix<T> &TMatrix<T>::operator =(TMatrix<T> const &src)
  {
  if (this != &src)  // obviate "a=a"
    {
    if (Allocate(src.m_n_rows, src.m_n_cols) == SUCCESS)
      {
      // we don't use 'memcpy' because elements may contain pointers...
      SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
      for (SIZE_T i = 0; i < n; i++)
        m_data[i] = src.m_data[i];
      }
    }
  return *this;
  }  // operator =()

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
/// @param[in] c A value to be assigned to all matrix elements.
/// @return A reference to "this" matrix.
template <class T>
TMatrix<T> &TMatrix<T>::operator =(T const &c)
  {
  // we don't use 'memcpy' because elements may contain pointers...
  SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  for (SIZE_T i = 0; i < n; i++)
    m_data[i] = c;
  return *this;
  }  // operator =()

//////////////////////////////////////////////////////////////////////////////
/// Set all elements to the same value.
/// @param[in] val A value to be assigned to all matrix elements.
/// @return A reference to "this" matrix.
template <class T>
void TMatrix<T>::Set(const T &val)
  {
  // we don't use 'memcpy' because elements may contain pointers...
  SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  for (SIZE_T i = 0; i < n; i++)
    m_data[i] = val;
  }   

//////////////////////////////////////////////////////////////////////////////
/// Scale elements value to the given factor.
///
/// The method multiplies all elements by the specified one.
/// @note T type must have 'operator *=" defined.
/// @param[in] c - A multiplier.
/// @return A reference to "this" matrix.
template <class T>
TMatrix<T> &TMatrix<T>::operator *=(const double &c)
  {
  SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  for (SIZE_T i = 0; i < n; i++)
    m_data[i] = (T)(m_data[i] * c);
  return *this;
  }  // 'operator *=()'

//////////////////////////////////////////////////////////////////////////////
/// Addition operator.
///
/// The method adds the specified matrix to "this" matrix.
/// The result is the matrix of the same dimensions as the source one.
/// @param[in] m - Added matrix.
/// @return A reference to "this" matrix.
template <class T>
TMatrix<T> &TMatrix<T>::operator +=(const TMatrix<T> &m)
  {
  Assert(GetRes() == m.GetRes());
  // we don't use 'memcpy' because elements may contain pointers...
  SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  for (SIZE_T i = 0; i < n; i++)
    m_data[i] += m.m_data[i];

  return *this;
  }  // operator +=()

//////////////////////////////////////////////////////////////////////////////
/// Comparison of matrices on equality.
///
/// The method compares two matrices on the per-element basis.
/// @param[in] src A matrix to compare with.
/// @return @b true, if all the elements are correspondently equal;
/// @b false otherwise.
template <class T>
bool TMatrix<T>::operator ==(const TMatrix<T> &src) const
  {
  if (m_n_rows != src.m_n_rows || m_n_cols != src.m_n_cols)
    return false;
  SIZE_T n = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  for (SIZE_T i = 0; i < n; i++)
    {
    if (!(m_data[i] == src.m_data[i]))
      return false;
    }
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Comparison of matrices on inequality.
///
/// The method compares two matrices on the per-element basis.
/// @param[in] src A matrix to compare with.
/// @return @b true, if matrices are not equal, @c false otherwise.
///
template <class T>
bool TMatrix<T>::operator !=(const TMatrix<T> &src) const
  {
  return !(*this == src);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set dimensions and (re)allocate memory, data are not retained.
///
/// The method sets new numbers of rows and columns and allocates/reallocates
/// memory if necessary to keep exactly n_r*n_c elements.
/// Reallocation occurs only is n_r*n_c differs from current value.
///
/// Data is not kept in case of re-allocation.
/// @param[in] n_r - The new number of rows.
/// @param[in] n_c - The new number of columns.
/// @return SUCCESS or FAILURE (allocation failure).
template <class T>
OKAY TMatrix<T>::Allocate(int n_r, int n_c)
  {
  if (n_r < 0 || n_c < 0)
    {
    Assert(false);
    return FAILURE;
    }

  if (n_r > 0 && n_c > 0)
    {
    // check n_r*n_c*sizeof(T) >= MAX_SIZE_T in such a way as
    // to avoid possible overflows for huge n_r and n_c
    if ((MAX_SIZE_T / n_r) / n_c < sizeof(T))
      {
      m_n_rows = m_n_cols = 0;
      if (m_data != NULL)
        delete m_data;
      m_data = NULL;
      return FAILURE;
      }
    }

  SIZE_T n_el = (SIZE_T)n_r * (SIZE_T)n_c;
  SIZE_T av_n_el = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  if (av_n_el != n_el || (n_el > 0 && m_data == NULL))
    {
    // free existing memory
    if (av_n_el > 0 && m_data != NULL)
      delete [] m_data;
    m_data = NULL;
    // and allocate new block
    if (n_el > 0)
      {
      //User()->LogMessage("Allocate Matrix %d * %d * %d", n_r, n_c, sizeof(T)); // Catch allocations
      m_data = new T[n_el];
      if (m_data == NULL)
        {
        m_n_rows = m_n_cols = 0;
        return FAILURE;
        }
      }
    }
  m_n_rows = n_r;
  m_n_cols = n_c;

  return SUCCESS;
  }  // Allocate()

//////////////////////////////////////////////////////////////////////////////
/// Set dimensions and (re)allocate memory, data are not retained.
/// It try to minimize reallocations
///
/// The method sets new numbers of rows and columns and allocates/reallocates
/// memory if new matrix should has greater memory.
/// Reallocation occurs only is n*m > than current value.
///
/// Data is not kept in case of re-allocation.
/// @param[in] n_r - The new number of rows.
/// @param[in] n_c - The new number of columns.
/// @return SUCCESS or FAILURE (allocation failure).
template <class T>
OKAY TMatrix<T>::AllocateFast(int n_r, int n_c)
  {
  SIZE_T n_el = (SIZE_T)n_r * (SIZE_T)n_c;
  SIZE_T av_n_el = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;

  if ((m_n_rows > 0) && (m_n_cols > 0) && (m_data != NULL) &&
      (n_r > 0) && (n_c > 0) && (n_el <= av_n_el))
    {
    m_n_rows = n_r;
    m_n_cols = n_c;
    return SUCCESS;
    }
  return Allocate(n_r, n_c);
  }  // AllocateFast()

//////////////////////////////////////////////////////////////////////////////
/// Set dimensions and (re)allocate memory, retain data.
///
/// The method changes dimensions of the matrix to new ones.
/// Reallocation occurs only if n_r*n_c differs from current value.
///
/// The existing data is kept: those elements which are present in both
/// old and new matrices retain their values.
/// If dimensions decrease, some elements which are outside the new matrix
/// are lost. If dimensions are increased, the new elements are initialised
/// by the default constructor of the type T.
/// @param[in] n_r - The new number of rows.
/// @param[in] n_c - The new number of columns.
/// @return SUCCESS or FAILURE (allocation failure).
template <class T>
OKAY TMatrix<T>::Resize(int n_r, int n_c)
  {
  if (m_n_rows == n_r && m_n_cols == n_c)
    return SUCCESS;  // nothing to do

  if (n_r < 0 || n_c < 0)
    {
    Assert(false);
    return FAILURE;
    }

  if (n_r > 0 && n_c > 0)
    {
    // check n_r*n_c*sizeof(T) >= MAX_SIZE_T in such a way as
    // to avoid possible overflows for huge n_r and n_c
    if ((MAX_SIZE_T / n_r) / n_c < sizeof(T))
      {
      m_n_rows = m_n_cols = 0;
      if (m_data != NULL)
        delete m_data;
      m_data = NULL;
      return FAILURE;
      }
    }

  SIZE_T av_n_el = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;

  if (m_data == NULL || av_n_el == 0)  // no data!
    return Allocate(n_r, n_c);
  else // we need to keep old data
    {
    // First create a temporary matrix of desired (new) dimensions
    TMatrix<T> b(n_r, n_c);
    if (b.m_data == NULL && n_r !=0 && n_c != 0)
      return FAILURE;

    // Then transfer data from current matrix into the new one.
    int min_n1 = Min(m_n_rows, n_r);
    int min_n2 = Min(m_n_cols, n_c);
    for (int i = 0; i < min_n1; i++)
      {
      for (int j = 0; j < min_n2; j++)
        b(i, j) = (*this)(i, j);
      }
    // and finally swap the temporary matrix and this one.
    SwapMatrix(b, *this);
    }

  return SUCCESS;
  }  // Resize()

//////////////////////////////////////////////////////////////////////////////
/// Get resolution of matrix: x - columns, y - rows.
/// @return Resolution of matrix.
template <class T>
Vect2i TMatrix<T>::GetRes() const
  {
  return Vect2i(m_n_cols, m_n_rows);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of rows.
/// @return - The number of rows.
template <class T>
int TMatrix<T>::NRows() const
  {
  return m_n_rows;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of columns.
/// @return - The number of columns.
template <class T>
int TMatrix<T>::NColumns() const
  {
  return m_n_cols;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get number of elements in the value array.
/// @return - The number of columns.
template <class T>
SIZE_T TMatrix<T>::Length() const
  {
  SIZE_T length = (SIZE_T)m_n_cols * (SIZE_T)m_n_rows;
  Assert(length <= MAX_SIZE_T);
  return length;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the specified dimension.
/// @param[in] i A required dimension; must be 0 or 1, debug version asserts it.
/// @return The number of rows, if i == 0; the number of columns, if i == 1.
template <class T>
int TMatrix<T>::Dimension(int i) const
  {
  Assert(i == 0 || i == 1);
  return *(&m_n_rows + i);
  }

//////////////////////////////////////////////////////////////////////////////
/// Insert rows.
///
/// The method inserts the specified number of rows before the specified
/// (i0-th) one,
/// so after the insertion, new rows will have indices <em>i0, ..., i0+n-1</em>.
/// New elements are initialised by the default constructor of type T.
///
/// A special case is when the number of columns is 0.
/// In this case the number of elements both before and after insertion is 0.
/// In this case all what is done is that the number of rows increases.
/// @param[in] i0 The row before which to insert new rows;
///   it must be <= number of rows in the original matrix.
///   It will be the index of the first inserted row
/// @param[in] n The number of rows to insert.
/// @param[in] m0 Number of columns to insert (effective if matrix has no rows yet).
///   Default -1 value has no effect.
/// @return SUCCESS or FAILURE (if not enough memory or i0 > number of rows).
///
template <class T>
OKAY TMatrix<T>::InsertRows(int i0, int n, int m0)
  {
  int i, j;
  if ((m_n_rows == 0) && (m0 >= 0))
    m_n_cols = m0;

  if (m_n_cols == 0)
    {
    // matrix actually does not exist (data array size m_n_rows*m_n_cols=0)
    // and won't actually exist(!); just change dimensions
    m_n_rows += n;
    return SUCCESS;
    }

  if (i0 > m_n_rows)  // matrix data size=0 for ancols number of rows
    return FAILURE;

  // Create a temporary matrix of desired (new) dimensions
  TMatrix<T> b(m_n_rows + n, m_n_cols);
  if (!b.m_data)
    return FAILURE;

  // then transfer data from current matrix into the new one
  if (m_data != NULL)  // while inserting 1st row, it may occur that a=NULL!
    {
    for (i = 0; i < i0; i++)
      {
      for (j = 0; j < m_n_cols; j++)
        b(i, j) = (*this)(i, j);
      }
    for (i = i0; i < m_n_rows; i++)
      {
      for (j = 0; j < m_n_cols; j++)
        b(i + n, j) = (*this)(i, j);
      }
    }
  // and finally swap the temporary matrix and this one.
  SwapMatrix(b, *this);
  return SUCCESS;
  }  // InsertRows()

//////////////////////////////////////////////////////////////////////////////
/// Inserts columns.
///
/// The method inserts the specified number of columns before the specified
/// (j0-th) one,
/// so after the insertion, new columns will have indices
/// <em>j0, ..., j0+n-1</em>.
/// New elements are initialised by the default constructor of type T
///
/// A special case is when the number of rows is 0.
/// In this case the number of elements both before and after insertion is 0.
/// In this case all what is done is that the number of columns increases by n.
/// @param[in] j0 - The column before which to insert the new columns.
/// It must be <= number of columns in the original matrix.
/// It will be the index of the first inserted column.
/// @param[in] n - The number of columns to insert.
/// @param[in] n0 Number of rows to insert (effective if matrix has no columns yet).
///   Default -1 value has no effect.
/// @return SUCCESS or FAILURE (if not enough memory or j0 > number of columns)
///
template <class T>
OKAY TMatrix<T>::InsertColumns(int j0, int n, int n0)
  {
  int i, j;
  if ((m_n_cols == 0) && (n0 >= 0))
    m_n_rows = n0;

  if (m_n_rows == 0)
    {
    // matrix actually does not exist (data array size m_n_rows*m_n_cols=0)
    // and won't actually exist(!); just change dimensions
    m_n_cols += n;
    return SUCCESS;
    }

  if (j0 > m_n_cols)  // matrix data size=0 for ancols number of columns
    return FAILURE;

  // Create a temporary matrix of desired (new) dimensions
  TMatrix<T> b(m_n_rows, m_n_cols + n);
  if (!b.m_data)
    return FAILURE;

  // then transfer data from current matrix into the new one
  if (m_data != NULL)  // while inserting 1st column, it may occur that a=NULL!
    {
    for (i = 0; i < m_n_rows; i++)
      {
      for (j = 0; j < j0; j++)
        b(i, j) = (*this)(i, j);
      for (j = j0; j < m_n_cols; j++)
        b(i, j + n) = (*this)(i, j);
      }
    }
  // and finally swap the temporary matrix and this one.
  SwapMatrix(b, *this);
  return SUCCESS;
  }  // InsertColumns()

//////////////////////////////////////////////////////////////////////////////
/// Access the specified element.
///
/// @param[in] i - A row;
/// @param[in] j - A column;
/// @note debug version asserts that indices are not out of range.
/// @return A reference to the element (i,j).
template <class T>
T &TMatrix<T>::operator ()(int i, int j)
  {
  Assert(i >= 0 && i < m_n_rows && j >= 0 && j < m_n_cols);;
  return m_data[(SIZE_T)i * (SIZE_T)m_n_cols + j];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the specified element.
///
/// @param[in] i - A row;
/// @param[in] j - A column;
/// @note debug version asserts that indices are not out of range.
/// @return A const reference to the element (i,j).
template <class T>
const T &TMatrix<T>::operator ()(int i, int j) const
  {
  Assert(i >= 0 && i < m_n_rows && j >= 0 && j < m_n_cols);;
  return m_data[(SIZE_T)i * (SIZE_T)m_n_cols + j];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the specified row.
///
/// The method returns the specified row as a protected (with index check) array
/// linked to the memory occupied by that row
/// (i.e. no new memory is allocated).
///
/// Therefore A[i][j] contains the j-th element of this row but works a
/// bit slower than A(i,j) because a new object (yet tiny) of array is
/// created and a couple of members are copied.
///
/// When this array is destroyed, the matrix's memory remains intact
/// Also when the matrix is destroyed or re-allocated, the SArray
/// representing its row becomes invalid which is not checked for, so
/// the caller must care about that.
///
/// Despite the array returned is a new object, it contains a reference
/// to a matrix row inside and the values of their elements change
/// synchronously because in fact these are the same elements. So
/// one can modify the source matrix via this object. Assignment of
/// one slice to another copies elements whose indices are present
/// in both sets so one can assign one row of a matrix to another.
/// @param[in] i - A row;
/// debug version asserts that the index is not out of range.
/// @return The array of the specified row.
template <class T>
SArray<T> TMatrix<T>::operator [](int i)
  {
  Assert(i >= 0 && i < m_n_rows);;
  return SArray<T>(m_data + (SIZE_T)i * (SIZE_T)m_n_cols, m_n_cols);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the specified row.
///
/// The method returns the specified row as a protected (with index check) array
/// linked to the memory occupied by that row
/// (i.e. no new memory is allocated).
/// The row is returned as a "const" object so it does not allow modification of
/// the source matrix (because the latter is also const and so does
/// not assume that it will be changed).
///
/// Therefore A[i][j] contains the j-th element of this row but works a
/// bit slower than A(i,j) because a new object (yet tiny) of the array is
/// created and a couple of members are copied.
///
/// When this array is destroyed, the matrix's memory remains intact
/// Also when the matrix is destroyed or re-allocated, the SArray
/// representing its row becomes invalid which is not checked for, so
/// the caller must care about that.
///
/// Despite the array returned is a new object, it contains a reference
/// to a matrix row inside and the values of their elements change
/// synchronously because in fact these are the same elements.
/// @param[in] i - A row;
/// debug version asserts that the index is not out of range.
/// @return The array of the specified row.
template <class T>
const SArray<T> TMatrix<T>::operator [](int i) const
  {
  Assert(i >= 0 && i < m_n_rows);;
  return SArray<T>(m_data + (SIZE_T)i * (SIZE_T)m_n_cols, m_n_cols);
  }

//////////////////////////////////////////////////////////////////////////////
/// Fast exchange of two matrices.
///
/// The method quickly swaps (exchanges) matrices without physically moving
/// elements (just swaps data pointers).
/// @param[in] m1 - The first matrix to swap.
/// @param[in] m2 - The second matrix to swap.
template <class T>
void TMatrix<T>::SwapMatrix(TMatrix<T> &m1, TMatrix<T> &m2)
  {
  int nrows_tmp, ncols_tmp;
  T *a_tmp;
  nrows_tmp = m1.m_n_rows;
  ncols_tmp = m1.m_n_cols;
  a_tmp  = m1.m_data;

  m1.m_n_rows = m2.m_n_rows;
  m1.m_n_cols = m2.m_n_cols;
  m1.m_data = m2.m_data;

  m2.m_n_rows = nrows_tmp;
  m2.m_n_cols = ncols_tmp;
  m2.m_data = a_tmp;
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Flip matrix in the vertical direction (along rows).
template <class T>
void TMatrix<T>::VertFlip()
  {
  for (int r = 0; r < m_n_rows / 2; r++)
    {
    for (int c = 0; c < m_n_cols; c++)
      Swap(*(m_data + (SIZE_T)m_n_cols * r + c), *(m_data + (SIZE_T)m_n_cols * (m_n_rows - 1 - r) + c));
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Flip matrix in the horizontal direction (along columns).
template <class T>
void TMatrix<T>::HorFlip()
  {
  for (int r = 0; r < m_n_rows; r++)
    {
    for (int c = 0; c < m_n_cols / 2; c++)
      Swap(*(m_data + (SIZE_T)m_n_cols * r + c), *(m_data + (SIZE_T)m_n_cols * r + m_n_cols - 1 - c));
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Crop matrix by specified region.
/// @param[in] beg Starting point.
/// @param[in] size Size of clipped image.
/// @return SUCCESS in all cases.
template <class T>
OKAY TMatrix<T>::Crop(const Point2i &beg, const Vect2i &size)
  {
  Vect2i cur_res = GetRes();
  Assert(beg.x < cur_res.x && beg.y < cur_res.y && beg.x + size.x <= cur_res.x && beg.y + size.y <= cur_res.y);
  for (int y = 0; y < size.y; y++)
    {
    for (int x = 0; x < size.x; x++)
      m_data[(SIZE_T)cur_res.x * y + x] = m_data[(SIZE_T)cur_res.x * (beg.y + y) + beg.x + x];
    }
  return Resize(size.y, size.x);
  }


//////////////////////////////////////////////////////////////////////////////
/// Copy rectangle from another matrix.
///
/// @param[in] src Source bitmap.
/// @param[in] dst_beg Start point in current matrix.
/// @param[in] src_beg Start point in the source matrix.
/// @param[in] size Size of copied rectangle.
template <class T>
void TMatrix<T>::CopyFrom(const TMatrix<T> &src, const Point2i &dst_beg, const Point2i &src_beg, const Vect2i &size)
  {
  const Vect2i src_res = src.GetRes();
  const Vect2i dst_res = GetRes();
  Assert(src_beg.x + size.x <= src_res.x);
  Assert(src_beg.y + size.y <= src_res.y);
  Assert(dst_beg.x + size.x <= dst_res.x);
  Assert(dst_beg.y + size.y <= dst_res.y);
  for (int i = 0; i < size.y; i++)
    {
    T *src_ptr = src.m_data + (SIZE_T)src_res.x * (src_beg.y + i) + src_beg.x;
    T *dst_ptr = m_data + (SIZE_T)dst_res.x * (dst_beg.y + i) + dst_beg.x;
    for (int j = 0; j < size.x; j++)
      *dst_ptr++ = *src_ptr++;
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Begin serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A serializer.
/// @param[in] tag - A chunk name.
template <class T>
void TMatrix<T>::BegChunk(Serializer &inout, const char *tag)
  {
  // Open a chunk
  inout.BegChunk(tag);
  // Process array's length (0 is default!)
  if (inout.Import())
    {
    // Reading...
    Allocate(0, 0);
    int n_r = 0, n_c = 0;
    inout.Value(n_r);
    inout.Value(n_c);
    SIZE_T total = 0;
    inout.Value(total);           // Read number of elements
    Assert(total == (SIZE_T)n_r * (SIZE_T)n_c);
    if (Allocate(n_r, n_c) != SUCCESS)
      inout.SetAllocError();
    }
  else if ((SIZE_T)m_n_rows * (SIZE_T)m_n_cols != 0)
    {
    // Writing...
    // Write non-zero number of elements
    inout.Value(m_n_rows);              // Write m_n_rows
    inout.Value(m_n_cols);              // Write m_n_cols
    SIZE_T total = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
    inout.Value(total); // Is not necessary
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// End serialization.
///
/// See @ref base_tarray_serialization for details.
/// @param[in, out] inout - A Serializer.
template <class T>
void TMatrix<T>::EndChunk(Serializer &inout)
  {
  inout.EndChunk();     // Close the chunk
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove rows.
///
/// The method removes the specified number of rows beginning from the
/// specified one.
/// @param[in] row - A row starting from which the rows will be removed.
/// it must be <= number of rows in original matrix.
/// @param[in] n - The number of rows to remove.
/// If (row + n) > NRows() then (NRows() - rows) rows will be removed.
/// @return SUCCESS or FAILURE (allocation failure or when @a row
/// exceeds the number of rows in the original matrix.
template <class T>
OKAY TMatrix<T>::RemoveRows(int row, int n)
  {
  if (row < 0 || row >= m_n_rows)
    return FAILURE;

  if (n == 0)
    return SUCCESS;  // nothing to do

  // Check correctnes number of the deleted columns
  if (row + n > m_n_rows)
    n = m_n_rows - row;

  // Calculate rows in new matrix
  int new_nrows = m_n_rows - n;

  if (m_data == NULL)
    {
    m_n_rows = new_nrows;
    return SUCCESS;
    }

  // Calculate size of the new buffer
  SIZE_T size = (SIZE_T)new_nrows * (SIZE_T)m_n_cols;

  // If remove all rows from matrix
  if (size == 0)
    {
    delete[] m_data;
    m_data = NULL;
    m_n_rows = new_nrows;
    return SUCCESS;
    }

  // Allocate memory under new matrix
  T *new_data = new T[size];
  if (new_data == NULL)
    return FAILURE;

  // Copy before removed part
  int r = 0;
  SIZE_T p = 0;
  SIZE_T p0 = 0;
  for (r = 0; r < row; r++)
    for (int c = 0; c < m_n_cols; c++)
      new_data[p++] = m_data[p0++];

  // Copy after removed part
  p0 = (SIZE_T)(row + n) * m_n_cols;
  for (r = row + n; r < m_n_rows; r++)
    for (int c = 0; c < m_n_cols; c++)
      new_data[p++] = m_data[p0++];

  delete[] m_data;
  m_data = new_data;
  m_n_rows = new_nrows;
  return SUCCESS;
  }  // RemoveRows()

//////////////////////////////////////////////////////////////////////////////
/// Remove columns.
///
/// The method removes the specified number of columns beginning from the
/// specified one.
/// @param[in] col - The column starting from which the columns will be removed.
/// it must be <= number of columns in original matrix.
/// @param[in] n - The number of columns to remove.
/// If (col + n) > NColumns() then (NColumns() - col) columns will be removed.
/// @return SUCCESS or FAILURE (allocation failure or when @a col
/// exceeds the number of columns in the original matrix.
template <class T>
OKAY TMatrix<T>::RemoveColumns(int col, int n)
  {
  if (col < 0 || col >= m_n_cols)
    return FAILURE;

  if (n == 0)
    return SUCCESS;  // nothing to do

  // Check correctnes number of the deleted columns
  if (col + n > m_n_cols)
    n = m_n_cols - col;

  // Calculate colmns in new matrix
  int new_ncols = m_n_cols - n;

  if (m_data == NULL)
    {
    m_n_cols = new_ncols;
    return SUCCESS;
    }

  // Calculate size of the new buffer
  SIZE_T size = (SIZE_T)m_n_rows * (SIZE_T)new_ncols;

  // If remove all columns from matrix
  if (size == 0)
    {
    delete[] m_data;
    m_data = NULL;
    m_n_cols = new_ncols;
    return SUCCESS;
    }

  // Allocate memory under new matrix
  T *new_data = new T[size];
  if (new_data == NULL)
    return FAILURE;

  SIZE_T p0 = 0;  // pointed on the first symbol of the 'r' row
  SIZE_T p  = 0;  // current position in the new buffer
  for (int r = 0; r < m_n_rows; r++)
    {
    int c = 0;

    // Copy before removed part
    for (c = 0; c < col; c++)
      new_data[p++] = m_data[p0 + c];

    // Copy after removed part
    for (c = col + n; c < m_n_cols; c++)
      new_data[p++] = m_data[p0 + c];

    p0 += m_n_cols;
    }

  delete[] m_data;
  m_data = new_data;
  m_n_cols = new_ncols;
  return SUCCESS;
  }  // RemoveColumns()

//////////////////////////////////////////////////////////////////////////////
/// Cast to a pointer to a matrix.
/// @return A pointer to a C-style matrix which contains all elements
/// (actually just returns a pointer to the own memory block).
template <class T>
TMatrix<T>::operator T*()
  {
  return m_data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Cast to a const pointer to a matrix.
/// @return A const pointer to a C-style matrix which contains all elements
/// (actually just returns a pointer to the own memory block).
template <class T>
TMatrix<T>::operator const T*() const
  {
  return m_data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get A pointer to matrix.
/// @return A pointer to a C-style matrix which contains all elements
/// (actually just returns a pointer to the own memory block).
template <class T>
T *TMatrix<T>::Data()
  {
  return m_data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a const pointer to a matrix.
/// @return A const pointer to a C-style matrix which contains all elements
/// (actually just returns a pointer to the own memory block).
template <class T>
const T *TMatrix<T>::Data() const
  {
  return m_data;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the specified row.
///
/// The method returns a pointer to a C-style array containing the
/// specified row.
/// @param[in] i - An index of the row to return;
/// debug version asserts that the index is not out of range.
/// @return A pointer to an array containing the specified row.
template <class T>
T *TMatrix<T>::Slice(int i)
  {
  Assert(i >= 0 && i < m_n_rows);
  return m_data + (SIZE_T)m_n_cols * i;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the specified row.
///
/// The method returns a const pointer to a C-style array containing the
/// specified row.
/// @param[in] i - An index of the row to return;
/// debug version asserts that the index is not out of range.
/// @return A const pointer to an array containing the specified row.
template <class T>
const T *TMatrix<T>::Slice(int i) const
  {
  Assert(i >= 0 && i < m_n_rows);
  return m_data + (SIZE_T)m_n_cols * i;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set row from the given pointer to row values.
/// @param[in] row Row index.
/// @param[in] val_ptr Pointer to row values.
template <class T>
void TMatrix<T>::SetRow(int row, const T *val_ptr)
  {
  Assert(row < m_n_rows);
  for (int i = 0; i < m_n_cols; i++)
    m_data[(SIZE_T)m_n_cols * row + i] = val_ptr[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access an element wrapped in a linear array.
/// @param[in] i - An index of an element;
/// must be < NRows() * NColumns(), debug version asserts it.
/// @return A reference to the specified element.
template <class T>
T &TMatrix<T>::operator ()(SIZE_T i)
  {
  Assert(i >= 0 && (SIZE_T)i < (SIZE_T)m_n_rows * m_n_cols);
  return m_data[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Access an element wrapped in a linear array.
/// @param[in] i - An index of the element;
/// must be < NRows() * NColumns(), debug version asserts it.
/// @return A const reference to the specified element.
template <class T>
const T &TMatrix<T>::operator ()(SIZE_T i) const
  {
  Assert(i >= 0 && (SIZE_T)i < (SIZE_T)m_n_rows * m_n_cols);
  return m_data[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new matrix, resized with bilinear interpolation.
///
/// Boundary values coincide, if resizing is only in the one direction.
/// @param n_rows New number of the rows.
/// @param n_cols New number of the columns.
/// @param[out] out Returned matrix.
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
template <class T>
OKAY TMatrix<T>::ResizeBilinear(int n_rows, int n_cols, TMatrix<T> &out) const
  {
  Assert(n_rows >= 1 && n_cols >= 1);
  if (n_rows == m_n_rows && n_cols == m_n_cols)
    {
    out = *this;
    return SUCCESS;
    }

  if (out.Allocate(n_rows, n_cols) != SUCCESS)
    return FAILURE;
  double row_scale = n_rows > 1 ? (m_n_rows - 1) / double(n_rows - 1) : 0;
  double col_scale = n_cols > 1 ? (m_n_cols - 1) / double(n_cols - 1) : 0;

  for (int i = 0; i < n_rows; i++)
    {
    double row = double(i) * row_scale;
    int r = (int)floor(row);
    int rn = r + 1;
    if (rn > m_n_rows - 1)
      rn = m_n_rows - 1;
    double u = row - r;

    for (int j = 0; j < n_cols; j++)
      {
      double col = double(j) * col_scale;
      int c = (int)floor(col);
      int cn = c + 1;
      if (cn > m_n_cols - 1)
        cn = m_n_cols - 1;
      double t = col - c;

      out(i, j) = (T)((*this)(r, c)   * ((1.0 - t) * (1.0 - u)) +
                      (*this)(r, cn)  * (       t  * (1.0 - u)) +
                      (*this)(rn, c)  * ((1.0 - t) *        u)  +
                      (*this)(rn, cn) * (       t  *        u));
      }  // for (int j = 0; j < n_cols; j++)
    }  // for (int i = 0; i < n_rows; i++)
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get interpolated pixel value, considering that data are cycled.
/// @param[in] pnt Coordinates of the pixel.
/// @param[out] out Output value.
template <class T>
void TMatrix<T>::GetInterpPixCycled(const Vect2d &pnt, T &out) const
  {
  int r = (int)floor(pnt.y);
  int rn = r + 1;
  if (rn > m_n_rows - 1)
    rn = 0;
  double u = pnt.y - r;

  int c = (int)floor(pnt.x);
  int cn = c + 1;
  if (cn > m_n_cols - 1)
    cn = 0;
  double t = pnt.x - c;

  out = (T)((*this)(r, c)   * ((1.0 - t) * (1.0 - u)) +
            (*this)(r, cn)  * (       t  * (1.0 - u)) +
            (*this)(rn, c)  * ((1.0 - t) *        u)  +
            (*this)(rn, cn) * (       t  *        u));
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new matrix, resized with bilinear interpolation cyclically.
///
/// Last and first values are mutually interpolated, if proper flag is set.
/// @param n_rows New number of the rows.
/// @param n_cols New number of the columns.
/// @param[out] out Returned matrix.
/// @param cycle_rows If rows are cycled (last is intrepolated with first).
/// @param cycle_cols If columns are cycled (last is intrepolated with first).
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
template <class T>
OKAY TMatrix<T>::ResizeBilinearCycled(int n_rows, int n_cols, TMatrix<T> &out, 
  bool cycle_rows, bool cycle_cols) const
  {
  Assert(n_rows >= 1 && n_cols >= 1);
  if (n_rows == m_n_rows && n_cols == m_n_cols)
    {
    out = *this;
    return SUCCESS;
    }

  if (out.Allocate(n_rows, n_cols) != SUCCESS)
    return FAILURE;
  double row_scale = cycle_rows ? (double)m_n_rows / n_rows : (n_rows > 1 ? (m_n_rows - 1) / double(n_rows - 1) : 0);
  double col_scale = cycle_cols ? (double)m_n_cols / n_cols : (n_cols > 1 ? (m_n_cols - 1) / double(n_cols - 1) : 0);

  for (int i = 0; i < n_rows; i++)
    {
    double row = cycle_rows ? (double(i + 0.5) * row_scale - 0.5) : (double(i) * row_scale);
    if (row < 0)
      row += m_n_rows;
    int r = (int)floor(row);
    int rn = r + 1;
    if (rn > m_n_rows - 1)
      {
      if (cycle_rows)
        rn = 0;
      else  
        rn = m_n_rows - 1;
      }  
    double u = row - r;

    for (int j = 0; j < n_cols; j++)
      {
      double col = cycle_cols ? (double(j + 0.5) * col_scale - 0.5) : (double(j) * col_scale);
      if (col < 0)
        col += m_n_cols;
      int c = (int)floor(col);
      int cn = c + 1;
      if (cn > m_n_cols - 1)
        {
        if (cycle_cols)
          cn = 0;
        else  
          cn = m_n_cols - 1;
        }  
      double t = col - c;

      out(i, j) = (T)((*this)(r, c)   * ((1.0 - t) * (1.0 - u)) +
                      (*this)(r, cn)  * (       t  * (1.0 - u)) +
                      (*this)(rn, c)  * ((1.0 - t) *        u)  +
                      (*this)(rn, cn) * (       t  *        u));
      }  // for (int j = 0; j < n_cols; j++)
    }  // for (int i = 0; i < n_rows; i++)
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new matrix, resized with bilinear interpolation cyclically with mask.
///
/// Last and first values are mutually interpolated, if proper flag is set.
/// @param n_rows New number of the rows.
/// @param n_cols New number of the columns.
/// @param[out] out Returned matrix.
/// @param cycle_rows If rows are cycled (last is intrepolated with first).
/// @param cycle_cols If columns are cycled (last is intrepolated with first).
/// @param[in] mask Mask matrix.
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
///
template <class T>
OKAY TMatrix<T>::ResizeBilinearCycledWithMask(int n_rows, int n_cols, TMatrix<T> &out, 
  bool cycle_rows, bool cycle_cols, const TMatrix<bool> &mask) const
  {
  Assert(n_rows >= 1 && n_cols >= 1);
  if (n_rows == m_n_rows && n_cols == m_n_cols)
    {
    out = *this;
    return SUCCESS;
    }

  Assert((mask.NRows() == m_n_rows) && (mask.NColumns() == m_n_cols));
  if ((mask.NRows() != m_n_rows) || (mask.NColumns() != m_n_cols))
    return FAILURE;

  if (out.Allocate(n_rows, n_cols) != SUCCESS)
    return FAILURE;
  double row_scale = cycle_rows ? (double)m_n_rows / n_rows : (n_rows > 1 ? (m_n_rows - 1) / double(n_rows - 1) : 0);
  double col_scale = cycle_cols ? (double)m_n_cols / n_cols : (n_cols > 1 ? (m_n_cols - 1) / double(n_cols - 1) : 0);

  T elem_arr[4];
  bool mask_arr[4];

  for (int i = 0; i < n_rows; i++)
    {
    double row = cycle_rows ? (double(i + 0.5) * row_scale - 0.5) : (double(i) * row_scale);
    if (row < 0)
      row += m_n_rows;
    int r = (int)floor(row);
    int rn = r + 1;
    if (rn > m_n_rows - 1)
      {
      if (cycle_rows)
        rn = 0;
      else
        rn = m_n_rows - 1;
      }
    double u = row - r;

    for (int j = 0; j < n_cols; j++)
      {
      double col = cycle_cols ? (double(j + 0.5) * col_scale - 0.5) : (double(j) * col_scale);
      if (col < 0)
        col += m_n_cols;
      int c = (int)floor(col);
      int cn = c + 1;
      if (cn > m_n_cols - 1)
        {
        if (cycle_cols)
          cn = 0;
        else
          cn = m_n_cols - 1;
        }
      double t = col - c;

      mask_arr[0] = mask(r, c);
      mask_arr[1] = mask(r, cn);
      mask_arr[2] = mask(rn, c);
      mask_arr[3] = mask(rn, cn);
      elem_arr[0] = (T)(*this)(r, c);
      elem_arr[1] = (T)(*this)(r, cn);
      elem_arr[2] = (T)(*this)(rn, c);
      elem_arr[3] = (T)(*this)(rn, cn);
      
      int total = 0;
      T sum = 0;
      for (int i = 0; i < 4; i++)
        {
        if (mask_arr[i])
          {
          total++;
          sum += elem_arr[i];
          }
        }

      if ((total > 0) && (total < 4))
        {
        sum = (T)(sum / total);
        for (int i = 0; i < 4; i++)
          {
          if (!mask_arr[i])
            elem_arr[i] = sum;
          }
        }

      out(i, j) = (T)(elem_arr[0] * ((1.0 - t) * (1.0 - u)) +
                      elem_arr[1] * (t * (1.0 - u)) +
                      elem_arr[2] * ((1.0 - t) * u) +
                      elem_arr[3] * (t  * u));
      }  // for (int j = 0; j < n_cols; j++)
    }  // for (int i = 0; i < n_rows; i++)
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new matrix, scaled with cells stretching.
///
/// @param[in] row_scale Scaling coefficient for rows.
/// @param[in] col_scale Scaling coefficient for columns.
/// @param[out] out Returned matrix.
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
template <class T>
OKAY TMatrix<T>::Scale(int row_scale, int col_scale, TMatrix<T> &out) const
  {
  Assert(row_scale > 0 && col_scale > 0);
  if (row_scale < 1 || col_scale < 1)
    return FAILURE;
  if (row_scale == 1 && col_scale == 1)
    {
    out = *this;
    return SUCCESS;
    }

  if (out.Allocate(m_n_rows * row_scale, m_n_cols * col_scale) != SUCCESS)
    return FAILURE;

  for (int i = 0; i < m_n_rows * row_scale; i++)
    {
    double row = double(i) / row_scale;
    int r = (int)floor(row);
    int rn = r + 1;
    if (rn > m_n_rows - 1)
      rn = m_n_rows - 1;
    //double u = row - r;

    for (int j = 0; j < m_n_cols * col_scale; j++)
      {
      double col = double(j) / col_scale;
      int c = (int)floor(col);
      int cn = c + 1;
      if (cn > m_n_cols - 1)
        cn = m_n_cols - 1;
      //double t = col - c;

      out(i, j) = (T)((*this)(r, c));
      }  // for (int j = 0; j < n_cols; j++)
    }  // for (int i = 0; i < n_rows; i++)
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create new Matrix, scaled to half size.
/// @param out Scaled matrix.
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
template <class T>
OKAY TMatrix<T>::ScaleToHalf(TMatrix<T> &out) const
  {
  Vect2i res = GetRes();
  Vect2i new_res = res / 2;

  if (out.Allocate(new_res.y, new_res.x) != SUCCESS)
    return FAILURE;

  T *dst = out.Data();
  for (int y = 0; y < new_res.y; y++)
    {
    const T *src = m_data + (SIZE_T)res.x * 2 * y;
    for (int x = 0; x < new_res.x; x++)
      {
      *dst++ = (T)((*src + src[1] + src[res.x] + src[res.x + 1]) * 0.25);
      src += 2;
      }
    }

  return SUCCESS;
  } // TMatrix<T>::ScaleToHalf()

//////////////////////////////////////////////////////////////////////////////
/// Create new Matrix, zoomed out to given factor.
/// @param[in] row_n Zoom out factor for rows.
/// @param[in] col_n Zoom out factor for columns.
/// @param out Scaled matrix.
/// @return SUCCESS if matrix is created successfully, FAILURE - otherwise.
template <class T>
OKAY TMatrix<T>::ZoomOut(int row_n, int col_n, TMatrix<T> &out) const
  {
  Vect2i res = GetRes();
  Vect2i new_res(res.x / col_n, res.y / row_n);

  if (out.Allocate(new_res.y, new_res.x) != SUCCESS)
    return FAILURE;

  T *dst = out.Data();
  double dvd = (double)row_n * col_n;
  for (int y = 0; y < new_res.y; y++)
    {
    for (int x = 0; x < new_res.x; x++)
      {
      T sum = 0;
      for (int yb = 0 ; yb < row_n; yb++)
        {
        const T *src = m_data + (SIZE_T)res.x * ((SIZE_T)row_n * y + yb) + (SIZE_T)col_n * x;
        for (int xb = 0 ; xb < col_n; xb++)
          sum += *src++;
        }
      
      *dst++ = (T)(sum / dvd);
      }
    }

  return SUCCESS;
  } // TMatrix<T>::ZoomOut()

//////////////////////////////////////////////////////////////////////////////
/// Find maximal value in the matrix.
/// @return Maximal value in the matrix.
/// @note If matrix has 0 length - return value is undefined.
template <class T>
T TMatrix<T>::MaxValue() const
  {
  T max_value(0);
  SIZE_T length = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;
  if (length < 1)
    return max_value;
  max_value = m_data[0];
  for (SIZE_T i = 1; i < length; i++)
    {
    if (max_value < m_data[i])
      max_value = m_data[i];
    }
  return max_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find minimal value in the matrix.
/// @return Minimal value in the matrix.
/// @note If matrix has 0 length - return value is undefined.
template <class T>
T TMatrix<T>::MinValue() const
  {
  T min_value(0);
  SIZE_T length = (SIZE_T)m_n_rows * (SIZE_T)m_n_cols;

  if (length < 1)
    return min_value;
  min_value = m_data[0];
  for (SIZE_T i = 1; i < length; i++)
    {
    if (m_data[i] < min_value)
      min_value = m_data[i];
    }
  return min_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check - is the matrix empty - is not allocated.
/// @return True, if matrix has zero size.
template <class T>
bool TMatrix<T>::IsEmpty() const
  {
  return m_n_rows == 0 || m_n_cols == 0;
  }

INTEGRA_NAMESPACE_END

#endif

