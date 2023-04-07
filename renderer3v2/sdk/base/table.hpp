/// @file
///
/// @brief Declaration of TTable template class.
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Fgk - Gennadij Kirejko, '00.10.01
/// @author Fvg - Vjacheslav Glukhov, '00.10.01

#ifndef _AIIP_TTABLE_HPP_
#define _AIIP_TTABLE_HPP_

#include "arrays.hpp"
#include "file.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic table of elements of an arbitrary type.
template <class T>
class TTable 
  {
  public:
    /// @name Constructor, destructor
    //@{
    /// Default constructor.
    inline TTable();
    /// Constructor by the number of the cells.
    inline TTable(int n);
    /// Destructor.
    inline ~TTable();
    //@}

  public:
    /// @name Length and sizes
    //@{
    /// Get the number of cells in the table.
    inline int Length() const;
    /// Change the number of cells.
    inline OKAY Resize(int n);
    /// Check existence of elements in the cell.
    inline bool IsEmpty(int ind) const;
    //@}

  public:
    /// @name Addition and removal
    //@{
    /// Add a new element in the cell.
    inline OKAY Insert(int ind, const T &elem0);
    /// Delete the specified element from the cell.
    OKAY Remove(int ind, const T &elem0);
    //@}

  public:
    /// @name Access to elements
    //@{
    /// Get the list of elements in the cell.
    TArray<T> &GetList(int ind);
    /// Get the list of elements in the cell.
    void GetList(int ind, TArray<T> &list) const;
    //@}

    /// Assignation operator
    TTable<T> &operator = (const TTable<T> &src);
    /// Serialization.
    void Serialization(Serializer &inout);

  private:
    /// @name Main arrays
    //@{
    /// Indices of the first elements in the cells.
    TArray<int> head;
    /// Indices of the next elements in the current cell.
    TArray<int> next;
    /// Values of the elements of the cells.
    TArray<T> elem;
    /// Temporary buffer for the elements in the current cell.
    TArray<T> list;
    //@}
  };

/// TTable instantiation for integers.
typedef TTable<int> ITable;

//////////////////////////////////////////////////////////////////////////////
// Inline methods of the class TTable

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
template <class T>
TTable<T>::TTable()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor by the number of the cells .
/// @param[in] n - An initial number of cells.
template <class T>
TTable<T>::TTable(int n)
  {
#ifdef DEBUG
  Assert(Resize(n) == SUCCESS);
#else
  (void)Resize(n);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
template <class T>
TTable<T>::~TTable()
  {
  }


//////////////////////////////////////////////////////////////////////////////
/// Change the number of cells.
///
/// All data will be destroyed and internal parameters will be
/// set to the initial state.
/// @param[in] n - A new number of cells.
/// @return SUCCESS if all is OK, FAILURE if memory allocation failed.
template <class T>
OKAY TTable<T>::Resize(int n)
  {
  if (head.Allocate(n) != SUCCESS)
    return FAILURE;
  for (int i = 0; i < head.Length(); i++)
    head[i] = 0;
  next.Allocate(0);
  next.SetBlockSize(n);
  elem.Allocate(0);
  elem.SetBlockSize(n);
  if (next.Add(0) != SUCCESS)
    return FAILURE;
  return elem.Allocate(1);
  }

//////////////////////////////////////////////////////////////////////////////
/// Add a new element to the cell.
/// @param[in] ind - An index of the cell to add an element to.
/// @param[in] elem0 - An element to add.
/// @return SUCCESS if all is OK, FAILURE if allocation memory is failure.
template <class T>
OKAY TTable<T>::Insert(int ind, const T &elem0)
  {
  if (next.Add(head[ind]) != SUCCESS)
    return FAILURE;
  head[ind] = next.Length() - 1;
  return elem.Add(elem0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Delete the specified element from the cell.
///
/// The method deletes the first element equal to the specified one
/// from the specified cell.
/// @param[in] ind - An index of the cell to delete an element from;
/// must be >= 0 and < number of cells, debug version assert it.
/// @param[in] elem0 - An element to delete.
/// @return SUCCESS if all is OK, FAILURE if allocation memory is failure.
template <class T>
OKAY TTable<T>::Remove(int ind, const T &elem0)
  {
  Assert(ind >= 0 && ind < head.Length());
  int cur = head[ind];
  if (cur == 0)
    return FAILURE; // cell is empty
  if (elem[cur] == elem0)
    head[ind] = next[cur];

  while (next[cur] != 0)
    {
    int cur_next = next[cur];
    if (elem[cur_next] == elem0)
      {
      next[cur] = next[cur_next];
      return SUCCESS;
      }
    cur = cur_next;
    }
  return FAILURE;
  }  // Remove()

//////////////////////////////////////////////////////////////////////////////
/// Get the list of elements in the cell.
/// @param[in] ind - An index of the cell to get elements from;
/// must be >= 0 and < number of cells, debug version assert it.
/// @return The list of elements stored in the specified cell.
/// These elements are copied to an internal buffer and its reference is 
/// returned.
template <class T>
TArray<T> &TTable<T>::GetList(int ind)
  {
  Assert(ind >= 0 && ind < head.Length());
  (void)list.Allocate(0);    // list.count = 0
  int cur = head[ind];
  while(cur != 0)
    {
    list.Add(elem[cur]);
    cur = next[cur];
    }
  return list;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the list of elements in the cell.
/// @param[in] ind_cell - An index of the cell to get elements from;
/// must be >= 0 and < number of cells, debug version assert it.
/// @param[out] list - An array where the elements will be copied to.
template <class T>
void TTable<T>::GetList(int ind_cell, TArray<T> &list) const
  {
  Assert(ind_cell >= 0 && ind_cell < head.Length());
  (void)list.Allocate(0);    // list.count = 0
  int cur = head[ind_cell];
  while(cur != 0)
    {
    list.Add(elem[cur]);
    cur = next[cur];
    }
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check existence of elements in the cell.
/// @param[in] ind - An index of the cell to check existence of elements;
/// must be >= 0 and < number of cells, debug version assert it.
/// @return @b true, if there are no elements.
template <class T>
bool TTable<T>::IsEmpty(int ind) const
  { 
  Assert(ind >= 0 && ind < head.Length());
  return (head[ind] == 0); 
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of cells in the table.
/// @return The number of cells in the table.
template <class T>
int TTable<T>::Length() const
  {
  return head.Length();
  }

///////////////////////////////////////////////////////////////////////////////
/// Serialization.
template <class T>
void TTable<T>::Serialization(Serializer &inout)
  {
  // Indices of the first elements in the cells: TArray<int> head;
  if (inout.Import())
    head.Resize();

  head.BegChunk(inout, "TableHead");
  int numb = head.Length();
  if (numb > 0)
    inout.Value((int *)head.Data(), numb);
  head.EndChunk(inout);

  // Indices of the next elements in the current cell: TArray<int> next;
  if (inout.Import())
    next.Resize();

  next.BegChunk(inout, "TableNext");
  numb = next.Length();
  if (numb > 0)
    inout.Value((int *)next.Data(), numb);
  next.EndChunk(inout);

  // Values of the elements of the cells: TArray<T> elem;
  if (inout.Import())
    elem.Resize();

  elem.BegChunk(inout, "TableElem");
  numb = elem.Length();
  if (numb > 0)
    inout.Value((T *)elem.Data(), numb);
  elem.EndChunk(inout);
  }

///////////////////////////////////////////////////////////////////////////////
/// Assignation operator.
/// @param src Source object.
/// @return Reference on 'this' object.
template <class T>
TTable<T> &TTable<T>::operator = (const TTable<T> &src)
  {
  head = src.head;
  next = src.next;
  elem = src.elem;
  return (*this);
  }
                                                                  
INTEGRA_NAMESPACE_END
#endif // #ifndef _AIIP_TTABLE_HPP_
