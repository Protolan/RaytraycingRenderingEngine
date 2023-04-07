/// @file
///
/// @brief Basic system support.
///
/// Here most commonly used features are provided.
///
/// @sa @ref base_mainpage
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '01.03.13

#include <integra.hpp>

#ifndef _KLBC_BASE_HPP_
#define _KLBC_BASE_HPP_

INTEGRA_NAMESPACE_START

/// Swap the values of two arguments of any type T.
template <class T>
inline void Swap(T &u, T &v);

/// Find the minimal value of two arguments of any type T.
template <class T>
inline T Min(const T &u, const T &v);

/// Find the minimal value of three arguments of any type T.
template <class T>
inline T Min3(const T &u, const T &v, const T &w);

/// Find the maximal value of two arguments of any type T.
template <class T>
inline T Max(const T &u, const T &v);

/// Find the maximal value of three arguments of any type T.
template <class T>
inline T Max3(const T &u, const T &v, const T &w);

//////////////////////////////////////////////////////////////////////////////
/// Swap the values of two arguments of any type T.
///
/// The method swaps values of two arguments of any type T.
/// The template parameter T must have "operator =()" defined.
/// @param[in, out] u - The first argument to swap.
/// @param[in, out] v - The second argument to swap.
template <class T>
inline void Swap(T &u, T &v)
  {
  T temp = u;
  u = v;
  v = temp;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the minimal value of two arguments of any type T.
///
/// The method finds the minimal value of two arguments of any type T.
/// The template parameter T must have "operator <()" defined.
/// @param[in] u - The first argument.
/// @param[in] v - The Second argument.
/// @return The minimum of @a u, @a v.
template <class T>
inline T Min(const T &u, const T &v)
  {
  return (u < v) ? u : v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the minimal value of three arguments of any type T.
///
/// The method finds the minimal value of three arguments of any type T.
/// The template parameter T must have "operator <()" defined.
/// @param[in] u - The first argument.
/// @param[in] v - The second argument.
/// @param[in] w - The third argument.
/// @return The minimum of @a u, @a v, @a w.
template <class T>
inline T Min3(const T &u, const T &v, const T &w)
  {
  return Min(u, Min(v, w));
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the maximal value of two arguments of any type T.
///
/// The method finds the maximal value of two arguments of any type T.
/// The template parameter T must have "operator >()" defined.
/// @param[in] u - The first argument.
/// @param[in] v - The second argument.
/// @return The maximum of @a u, @a v.
template <class T>
inline T Max(const T &u, const T &v)
  {
  return (u > v) ? u : v;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the maximal value of three arguments of any type T.
///
/// The method  finds the maximal value of three arguments of any type T.
/// The template parameter T must have "operator >()" defined.
/// @param[in] u - The first argument.
/// @param[in] v - The second argument.
/// @param[in] w - The third argument.
/// @return The maximum of @a u, @a v, @a w.
template <class T>
inline T Max3(const T &u, const T &v, const T &w)
  {
  return Max(u, Max(v, w));
  }

INTEGRA_NAMESPACE_END
#endif
