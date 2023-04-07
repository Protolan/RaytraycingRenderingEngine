/// @file
///
/// @brief Declaration of TDict template class.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01

#ifndef _KLBC_DICT_HPP_
#define _KLBC_DICT_HPP_

#include "arrays.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic dictionary of elements of an arbitrary type.
template <class KEY, class VALUE>
class TDict
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline TDict(int block_size = 10);
    /// Copy constructor.
    inline TDict(const TDict<KEY, VALUE> &sour);
    /// Destructor.
    inline ~TDict();
    //@}

  public:
    /// @name Access to the elements
    //@{
    /// Get a reference to the value for the specified key.
    VALUE &operator [](const KEY &key);
    /// Get a value for the specified key.
    const VALUE &operator [](const KEY &key) const;
    /// Find a value for the specified key and return pointer to a value or NULL.
    VALUE *FindValue(const KEY &key);
    /// Find a value for the specified key and return pointer to a value or NULL.
    const VALUE *FindValue(const KEY &key) const;

    /// Direct access to the key with the specified index.
    const KEY &GetKey(int i) const;
    /// Direct access to the key with the specified index.
    KEY &Key(int i);
    /// Direct access to the value with the specified index.
    const VALUE &GetValue(int i) const;
    /// Direct access to the value with the specified index.
    VALUE &Value(int i);
    //@}

  public:
    /// @name Length and sizes
    //@{
    /// Get the number of elements in the dictionary.
    inline int Length() const;
    /// Change (expand) the length of the dictionary.
    OKAY Accommodate(int new_len = 0);
    /// Get the block size of the dictionary.
    inline int BlockSize() const;
    /// Set a new block size.
    inline void SetBlockSize(int block_size);
    //@}

  public:
    /// @name Addition and removal of elements
    //@{
    /// Put a new pair.
    OKAY Put(const KEY &new_key, const VALUE &new_value);
    /// Replace a pair.
    OKAY Enter(const KEY &key, const VALUE &new_value);
    /// Exclude a pair keeping order of elements.
    void Exclude(const KEY &key);
    /// Exclude a pair keeping order of elements.
    void Exclude(int ind);
    /// Remove a pair not keeping order of elements.
    void Remove(const KEY &key);
    /// Remove all pairs.
    inline void RemoveElements();
    /// Remove all pairs and release memory.
    inline void RemoveAll();
    //@}

  public:
    /// @name Search
    //@{
    /// Find the pair for the specified key.
    bool Find(const KEY &key, VALUE *search_value = NULL) const;
    //@}

  protected:
    /// @name Storage
    //@{
    /// Internal storage object.
    struct TDictElem
      {
      /// @name Comparison operators
      //@{
      /// Equal.
      inline bool operator ==(const struct TDictElem elem2) const;
      /// Less.
      inline bool operator <(const struct TDictElem elem2) const;
      //@}

      /// Key.
      KEY m_key;
      /// Value.
      VALUE m_value;
      };
    /// Array of pairs.
    TArray<TDictElem> m_contents;
    //@}
  };  // class TDict


//////////////////////////////////////////////////////////////////////////////
// Methods of the class TDict

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method creates an empty dictionary. Block size
/// (the number of element placeholders to add per once for
/// the underlying implementation) is remembered.
/// @param[in] block_size - The size of the block memory, in elements;
/// must be > 0, debug version asserts it.
template <class KEY, class VALUE>
TDict<KEY, VALUE>::TDict(int block_size) : 
  m_contents(block_size)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] sour - A source object of the class.
template <class KEY, class VALUE>
TDict<KEY, VALUE>::TDict(const TDict<KEY, VALUE> &sour) :
  m_contents(sour.m_contents)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method releases all memory occupied by the array.
template <class KEY, class VALUE>
TDict<KEY, VALUE>::~TDict()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a reference to the value for the specified key.
///
/// If there is no a value with this key in the dictionary yet,
/// a new element with this key and a dummy value is created,
/// and the reference to the value part of it is
/// returned. Thus, the result may be used as an l-value.
/// @param[in] key - A key for search of a corresponding value.
/// @return A reference to the corresponding value.
template <class KEY, class VALUE>
VALUE &TDict<KEY, VALUE>::operator [] (const KEY &key)
  {
  int i;         // counter for the loop
  int arr_count; // number elements in the array

  arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      return (m_contents[i].m_value);
    }

  // Key was not found
  TDictElem elem;
  elem.m_key = key;
  m_contents.Put(elem, arr_count);
  return (m_contents[arr_count].m_value);
  }  // operator []()

//////////////////////////////////////////////////////////////////////////////
/// Get a value for the specified key.
///
/// If there is no a value with this key in the dictionary yet,
/// an undefined value is returned.
/// @param[in] key A key for search of a value.
/// @return A corresponding value.
template <class KEY, class VALUE>
const VALUE &TDict<KEY, VALUE>::operator [] (const KEY &key) const
  {
  static VALUE res;
  int i;         // counter for the loop
  int arr_count; // number elements in the array

  arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      return (m_contents[i].m_value);
    }

  // Key was not found - return garbage (meaning that this code is also garbage, once there is no any sign, that key is found) 
  return res;
  }


//////////////////////////////////////////////////////////////////////////////
/// Find a value for the specified key and return pointer to a value or NULL.
///
/// If there is no a value with this key in the dictionary, return NULL.
/// @param[in] key A key for search of a value.
/// @return A corresponding value.
template <class KEY, class VALUE>
VALUE *TDict<KEY, VALUE>::FindValue(const KEY &key)
  {
  int arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (int i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      return (&m_contents[i].m_value);
    }

  // Key was not found - return NULL. 
  return NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find a value for the specified key and return pointer to a value or NULL.
///
/// If there is no a value with this key in the dictionary, return NULL.
/// @param[in] key A key for search of a value.
/// @return A corresponding value.
template <class KEY, class VALUE>
const VALUE *TDict<KEY, VALUE>::FindValue(const KEY &key) const
  {
  int arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (int i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      return (&m_contents[i].m_value);
    }

  // Key was not found - return NULL. 
  return NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Direct access to the key with the specified index.
/// @param[in] i - An index of a pair.
/// @return A key with the specified index.
template <class KEY, class VALUE>
const KEY &TDict<KEY, VALUE>::GetKey(int i) const
  {
  return m_contents[i].m_key;
  }

//////////////////////////////////////////////////////////////////////////////
/// Direct access to the key with the specified index.
/// @param[in] i - An index of a pair.
/// @return A key with the specified index.
template <class KEY, class VALUE>
KEY &TDict<KEY, VALUE>::Key(int i)
  {
  return m_contents[i].m_key;
  }

//////////////////////////////////////////////////////////////////////////////
/// Direct access to the value with the specified index.
/// @param[in] i - An index of a pair.
/// @return A value with the specified index.
template <class KEY, class VALUE>
VALUE &TDict<KEY, VALUE>::Value(int i)
  {
  return m_contents[i].m_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Direct access to the value with the specified index.
/// @param[in] i - An index of a pair.
/// @return A value with the specified index.
template <class KEY, class VALUE>
const VALUE &TDict<KEY, VALUE>::GetValue(int i) const
  {
  return m_contents[i].m_value;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the number of elements in the dictionary.
/// @return The number of elements in the dictionary.
template <class KEY, class VALUE>
int TDict<KEY, VALUE>::Length() const
  {
  return m_contents.Length();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the block size of the dictionary.
/// @return The block size of the dictionary area.
template <class KEY, class VALUE>
int TDict<KEY, VALUE>::BlockSize() const
  {
  return m_contents.BlockSize();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new block size.
/// @param[in] block_size - A new block size of the dictionary, in elements;
/// must be > 0, debug version asserts it.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::SetBlockSize(int block_size)
  {
  Assert(block_size > 0);
  m_contents.SetBlockSize(block_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Put a new pair.
///
/// The method puts a pair (key, value) to this dictionary.
/// No check is made whether the key is already in the dictionary.
/// Debug version asserts presence of the key in the dictionary.
/// @param[in] new_key A new key.
/// @param[in] new_value  A new value.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class KEY, class VALUE>
OKAY TDict<KEY, VALUE>::Put(const KEY &new_key, const VALUE &new_value)
  {
  TDictElem elem;

#ifdef DEBUG
  if (this->Find(new_key))
    Assert(false);
#endif

  elem.m_key = new_key;
  elem.m_value = new_value;

  return (m_contents.Add(elem));
  }

//////////////////////////////////////////////////////////////////////////////
/// Replace a pair.
///
/// The method searches a pair (key, value) with the key equal to
/// the specified one and replaces the value by the specified value.
/// If a pair with the specified key was not found, a new element is
/// added to the dictionary.
/// @param[in] key - A key for search.
/// @param[in] new_value - A new value.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class KEY, class VALUE>
OKAY TDict<KEY, VALUE>::Enter(const KEY &key,
                              const VALUE &new_value)
  {
  TDictElem elem; // current element of the array
  int arr_count;  // number elements in the array

  arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (int i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      {
      m_contents[i].m_value = new_value;
      return SUCCESS;
      }
    }  // for (i = 0; i < arr_count; i++)

  // Key was not found then add pair (key, new_value)
  elem.m_key = key;
  elem.m_value = new_value;
  return m_contents.Add(elem);
  }  // Enter()

//////////////////////////////////////////////////////////////////////////////
/// Exclude a pair keeping order of elements.
///
/// The method removes a pair (key, value) with the specified key.
/// The order of the rest pairs is not changed because
/// the method shifts the remaining pairs keeping their order intact.
/// The size occupied by the dictionary is not changed.
/// @param[in] key - A key of the pair to be removed.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::Exclude(const KEY &key)
  {
  int length; // length of the dictionary array

  length = m_contents.Length();
  for (int i = 0; i < length; i++)
    {
    if (m_contents[i].m_key == key)
      {
      m_contents.Exclude(i);
      return;
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Exclude a pair keeping order of elements.
///
/// The method removes a pair with the specified index.
/// The order of the rest pairs is not changed because
/// the method shifts the remaining pairs keeping their order intact.
/// The size occupied by the dictionary is not changed.
/// @param[in] ind - An index of the pair to be removed.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::Exclude(int ind)
  {
  m_contents.Exclude(ind);
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove a pair not keeping order of elements.
///
/// The method removes a pair (key, value) with the specified m_key.
/// The order of the rest pairs can be changed because the method
/// copies the last pair of the dictionary to the
/// position of the removed one. The size occupied by the dictionary is not 
/// changed.
/// @param[in] key - A key of the pair to be removed.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::Remove(const KEY &key)
  {
  int length; // length of the dictionary array

  length = m_contents.Length();
  for (int i = 0; i < length; i++)
    {
    if (m_contents[i].m_key == key)
      {
      m_contents.Remove(i);
      return;
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove all pairs.
///
/// The method removes all pairs from the dictionary. Memory is not 
/// reallocated.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::RemoveElements()
  {
  m_contents.Allocate(0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove all pairs and release memory.
///
/// The method removes all pairs from the dictionary. The memory is released.
template <class KEY, class VALUE>
void TDict<KEY, VALUE>::RemoveAll()
  {
  m_contents.Resize();
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the pair for the specified key.
/// @param[in] key - A key for the search.
/// @param[out] search_value A pointer to the found value.
/// @return @b true, if the pair was found, or @b false otherwise.
/// If @b true, and @b search_value is not NULL, *search_value is set to
/// the value of the found pair.
template <class KEY, class VALUE>
bool TDict<KEY, VALUE>::Find(
    const KEY &key, VALUE *search_value) const
  {
  int i;         // counter for the loop
  int arr_count; // number elements in the array

  arr_count = m_contents.Length();
  // Loop all elements for the search key
  for (i = 0; i < arr_count; i++)
    {
    if (m_contents[i].m_key == key) // Key was found
      {
      if (search_value != NULL)
        *search_value = m_contents[i].m_value;
      return true;
      }
    }  // for (i = 0; i < arr_count; i++)

  return false; // Key was not found
  }  // Find()

//////////////////////////////////////////////////////////////////////////////
/// Change (expand) the length of the dictionary.
///
/// The method makes sure the size of the dictionary is big enough to 
/// accommodate the specified number of elements. The dictionary can be
/// expanded (if necessary) but its actual length is not changed.
/// @param[in] new_len - A new size of the dictionary;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class KEY, class VALUE>
OKAY TDict<KEY, VALUE>::Accommodate(int new_len)
  {
  int count = Length();
  if (m_contents.Grow(new_len) != SUCCESS)
    return FAILURE;

  return m_contents.Allocate(count);
  }

//////////////////////////////////////////////////////////////////////////////
// Methods of the struct TDictElem

//////////////////////////////////////////////////////////////////////////////
/// Equal.
///
/// Comparison operator.
/// @param[in] elem2 - A structure to compare with.
/// @return A result of the comparison.
template <class KEY, class VALUE>
bool TDict<KEY, VALUE>::TDictElem::operator ==(const struct TDictElem elem2)
  const
  {
  return m_key == elem2.m_key;
  }

//////////////////////////////////////////////////////////////////////////////
/// Less.
///
/// Comparison operator.
/// @param[in] elem2 - A structure to compare with.
/// @return A result of the comparison.
template <class KEY, class VALUE>
bool TDict<KEY, VALUE>::TDictElem::operator <(const struct TDictElem elem2)
  const
  {
  return m_key < elem2.m_key;
  }

INTEGRA_NAMESPACE_END
#endif
