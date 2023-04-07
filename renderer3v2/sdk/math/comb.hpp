/// @file
///
/// @brief Combinatoric algorithms header file
///
/// Copyright &copy; INTEGRA, Inc., 2010.
///
/// @internal
/// @author Rkv - Kostya Vostryakov

#ifndef _AIIK_COMB_HPP_
#define _AIIK_COMB_HPP_


#include <base/arrays.hpp>

#include "vect3.hpp"

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
/// Get number of k combinations from n objects
/// @param[in] n Number of all elements
/// @param[in] k Number of elements for choose
inline int CombinationsNum(int n, int k)
  {
  if (n < 0 || k < 0)
    {
    Assert(false && "Invalid negative parameter in CombinationsNum()");
    return 0;
    }

  if (n < k)
    return 0;
  if (n == k)
    return 1;

  int delta, imax;

  if (k < n-k)
    {
    delta = n-k;
    imax = k;
    }
  else 
    {
    delta = k;
    imax = n-k;
    }

  int c = delta + 1;

  for (int i = 2; i <= imax; ++i)
    c = (c * (delta + i)) / i;

  return c;
  }

//////////////////////////////////////////////////////////////////////////////
/// Generate fist combination in lexicographic order
/// @param[in, out] c Working array with length k
/// @param[in] n Number of all elements
/// @param[in] k Number of elements for choose
template<class T> inline
void InitCombination(TArray<T> &c, int n, int k)
  {
  c.Allocate(k);
  //const int N = CombinationsNum(n, k);
  for (int i = 0; i < k; i++)
    c[i] = i;
  }

//////////////////////////////////////////////////////////////////////////////
/// Generate next combination in lexicographic order
/// NextCombination(c, 5, 3) with start c value = { 0, 1, 2 } will generate
/// { 0, 3, 4 }, { 0, 1, 3 }, { 1, 2, 3 }, { 0, 1, 4 } { 1, 2, 4 }
/// { 0, 2, 3 } { 1, 3, 4 }, { 0, 2, 4 } { 2, 3, 4 } till return false
/// @param[in, out] c Working array with length k
/// @param[in] n Number of all elements
/// @param[in] k Number of elements for choose
/// @return true - if these is combination, false - if the end is over
template<class T> inline
bool NextCombination(TArray<T> &c, int n, int k)
  {
  if (c[0] == n - k)
    return false;

  int i;
  for (i = k - 1; i > 0 && c[i] == n - k + i; --i) ;

  ++c[i];

  for (int j = i; j < k - 1; ++j)
    c[j+1] = c[j] + 1;

  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Reverse elements in iterator range [first, last]
/// @param[in] first Iterator is pointed to head
/// @param[in] last Iterator is following by tail
template<class BiIter> inline
void Reverse(BiIter first, BiIter last)
  {	
  for (; first != last && first != --last; ++first)
    Swap(*first, *last);
  }

//////////////////////////////////////////////////////////////////////////////
/// Generate next permutation in iterator range [first, last]
/// @param[in] first Iterator is pointed to head
/// @param[in] last Iterator is following by tail
/// @return true - if these is permutation, false - if the end is over
template<class BiIter> inline
bool NextPermutation(BiIter first, BiIter last)
  {
  Assert(first < last);
  BiIter next = last;
  if (first == last || first == --next)
    return false;

  for ( ; ; )
    {	 
    // find rightmost element smaller than successor
    BiIter next1 = next;
    if (*--next < *next1)
      {	 
      // swap with rightmost element that's smaller, flip suffix
      BiIter mid = last;
      for ( ; !(*next < *--mid); )
        ;
      Swap(*next, *mid);
      Reverse(next1, last);
      return (true);
      }

    if (next == first)
      {	 
      // pure descending, flip all
      Reverse(first, last);
      return false;
      }
    }
  }


INTEGRA_NAMESPACE_END
#endif
