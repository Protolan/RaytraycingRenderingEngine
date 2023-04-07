/// @file
///
/// @brief Affine matrix decomposition.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2014.
///
/// @internal
/// @author Piv Ildar Valiev                      '99.12.28<BR>
/// @notes  On the base of the file quat.h created by Dan Silva from maxsdk.

#ifndef _DECOMPO_HPP_
#define _DECOMPO_HPP_

#include "vect3.hpp"
#include "matrix43.hpp"
#include "quatern.hpp"

INTEGRA_NAMESPACE_START

template <class T>
class AffParts 
  {
  /// Translation.
  TVect3<T> t;    
  /// Essential rotation.
  Quatern<T> q;   
  /// Stretch rotation.
  Quatern<T> u;  
  /// Stretch factors.
  TVect3<T> k;    
  /// Sign of determinant.
  T f;     
  };


template <class T>
void SpectralDecomp(TMatrix43<T> m, TVect3<T> &s, Quatern<T> &q);
template <class T>
void DecompoAffine(TMatrix43<T> A, AffParts<T> *parts);
template <class T>
void inv_affine(AffParts<T> *parts, AffParts<T> *inverse);

INTEGRA_NAMESPACE_END
#endif // #ifndef _DECOMPO_HPP_ 

// 'decompo.hpp' 
