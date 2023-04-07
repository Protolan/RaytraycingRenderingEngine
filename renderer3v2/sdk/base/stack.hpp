/// @file
///
/// @brief Declaration of TStack template class.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Oek - Edward Kopylov '99.12.01

#ifndef _KLBC_STACK_HPP_
#define _KLBC_STACK_HPP_

#include "arrays.hpp"

INTEGRA_NAMESPACE_START

/// Dynamic stack of elements of an arbitrary type.
template <class T>
class TStack
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline TStack(int n = 1);
    /// Destructor.
    inline ~TStack();
    //@}

  public:
    /// @name Block size
    //@{
    /// Get the block size of the stack.
    inline int BlockSize() const;
    /// Set a new block size.
    inline void SetBlockSize(int blsize);
    //@}

  public:
    /// @name Access to elements
    //@{
    /// Push an element.
    inline OKAY Push(const T &elem);
    /// Push an empty element.
    inline OKAY Push();
    /// Access the top element.
    inline T &Top();
    /// Access the top element.
    inline const T &Top() const;
    /// Pop the stack.
    inline OKAY Pop(T &elem);
    /// Pop the stack.
    inline OKAY Pop();
    /// Clear the stack.
    inline void Clear();
    /// Check whether the stack is empty.
    inline bool IsEmpty() const;
    //@}

  protected:
    /// @name Protected members
    //@{
    /// The array of elements.
    TArray<T> arr;
    //@}
  }; // class TStack

//////////////////////////////////////////////////////////////////////////////
// Inline methods of TStack class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method initializes the stack (allocates memory) for the specified
/// number of elements.
/// @param[in] n - The number of elements to initially allocate memory for.
template <class T>
TStack<T>::TStack(int n) : arr(n)   // set array block size as n
  {
#if 0 // Do not allocate memory initially (it is crucial for performance) :Pnd
  arr.Resize(n);
  Assert(arr.Size() == n);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
template <class T>
TStack<T>::~TStack()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the block size of the stack, in elements.
/// @return The block size of the stack, in elements.
template <class T>
int TStack<T>::BlockSize() const
  {
  return arr.BlockSize();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new block size.
/// @param[in] blsize - A new block size of the stack, in elements;
/// must be > 0, debug version asserts it.
template <class T>
void TStack<T>::SetBlockSize(int blsize)
  {
  arr.SetBlockSize(blsize);
  }

//////////////////////////////////////////////////////////////////////////////
/// Push an element.
/// @param[in] elem - A new element to push into the stack.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TStack<T>::Push(const T &elem)
  {
  return arr.Add(elem);
  }

//////////////////////////////////////////////////////////////////////////////
/// Push an empty element.
///
/// The method pushes an empty element that may be filled later directly on
/// the stack using the Top() method.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TStack<T>::Push()
  {
  return arr.Allocate(arr.Length() + 1);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the top element.
/// @note The stack must not be empty, debug version asserts it.
/// @return A reference to the top element.
template <class T>
T &TStack<T>::Top()
  { 
  Assert(!IsEmpty());; 
  return arr[arr.Length() - 1]; 
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the top element.
/// @note The stack must not be empty, debug version asserts it.
/// @return A reference to the top element.
template <class T>
const T &TStack<T>::Top() const
  { 
  Assert(!IsEmpty());; 
  return arr[arr.Length() - 1]; 
  }

//////////////////////////////////////////////////////////////////////////////
/// Pop the stack.
/// @note The stack must not be empty, debug version asserts it.
/// @param[out] elem - An element popped from the stack.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TStack<T>::Pop(T &elem)
  { 
  Assert(!IsEmpty());; 
  elem = Top(); 
  return arr.Allocate(arr.Length() - 1); 
  }

//////////////////////////////////////////////////////////////////////////////
/// Pop the stack.
/// @note The stack must not be empty, debug version asserts it.
/// @return SUCCESS / FAILURE (memory allocation failure).
template <class T>
OKAY TStack<T>::Pop()
  { 
  Assert(!IsEmpty());; 
  return arr.Allocate(arr.Length() - 1); 
  }

//////////////////////////////////////////////////////////////////////////////
/// Clear the stack.
///
/// The method removes all elements from the stack. Memory is not reallocated.
template <class T>
void TStack<T>::Clear()
  {
  arr.Truncate();
  }

//////////////////////////////////////////////////////////////////////////////
/// Check whether the stack is empty.
/// @return @b true if the stack is empty; @b false otherwise.
template <class T>
bool TStack<T>::IsEmpty() const
  {
  return (arr.Length() == 0);
  }

INTEGRA_NAMESPACE_END
#endif //  _KLBC_STACK_HPP_
