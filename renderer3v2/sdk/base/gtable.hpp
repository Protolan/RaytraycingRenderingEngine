/// @file
///
/// @brief Declaration of GTable template class.
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Oek - Edward Kopylov, '01.11.22

#ifndef _KLBC_GTABLE_HPP_
#define _KLBC_GTABLE_HPP_

#include "table.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic table of elements, which may be compared for equality.
template <class T>
class GTable : public TTable<T>
  {
  public:
    /// @name Constructors
    //@{
    /// Default constructor. 
    inline GTable();
    /// Constructor by the number of cells.
    inline GTable(int n);
    //@}

  public:
    /// @name Comparison
    //@{
    /// Tables comparison.
    bool operator ==(const GTable<T> &src) const;
    //@}

  public:
    /// @name Search
    //@{
    /// Find an element.
    bool Find(const T &elem) const;
    /// Find an element in the specified cell.
    bool Find(const T &elem, int ind, int *pos = NULL) const;
  };  // class GTable

/// GTable instantiation for integers.
typedef GTable<int> IGTable;

//////////////////////////////////////////////////////////////////////////////
// Methods of the class GTable

//////////////////////////////////////////////////////////////////////////////
/// Default constructor. 
template <class T>
GTable<T>::GTable()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor by the number of cells.
/// @param[in] n A number of cells; must be > 0.
template <class T>
GTable<T>::GTable(int n) : TTable<T>(n)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Tables comparison.
///
/// The method performs the per-element comparison of the tables.
/// @param[in] src A table to compare with.
/// @return @b true, if all elements are correspondently equal;
/// @b false otherwise.
template <class T>
bool GTable<T>::operator ==(const GTable<T> &src) const
  {
  if (Length() != src.Length())
    return false;

  TArray<T> lst, src_lst;
  for (int i = 0; i < Length(); i++)
    {
    GetList(i, lst);
    src.GetList(i, src_lst);
    if (lst.Length() != src_lst.Length())
      return false;
    for (int j = 0; j < lst.Length(); j++)
      if (!(lst[j] == src_lst[j]))
        return false;
    }

  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find an element in the specified cell.
/// @param[in] elem - An element to find.
/// @param[in] ind - An index of the table cell for the search.
/// @param[out] pos - A position of the found element in the list of elements
/// in the specified cell.
/// @return @b YES, if the element was found, or @n NO otherwise.
/// If @b YES, and pos is not NULL, *pos is set to the first (zero-based)
/// position of the element found in the list of elements of the given cell.
template <class T>
bool GTable<T>::Find(const T &elem, int ind, int *pos) const
  {
  TArray<T> lst;
  GetList(ind, lst);
  for (int j = 0; j < lst.Length(); j++)
    {
    if (elem == lst[j])
      {
      if (pos != NULL)
        *pos = j;
      return YES;
      }
    }

  return NO;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find an element.
/// @param[in] elem - An element to find.
/// @return @b YES, if the element was found, or @b NO otherwise.
template <class T>
bool GTable<T>::Find(const T &elem) const
  {
  TArray<T> lst;
  for (int i = 0; i < Length(); i++)
    {
    GetList(i, lst);
    for (int j = 0; j < lst.Length(); j++)
      {
      if (elem == lst[j])
        return YES;
      }
    }

  return NO;
  }

INTEGRA_NAMESPACE_END
#endif // _KLBC_GTABLE_HPP_
