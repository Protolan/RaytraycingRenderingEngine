/// @internal
/// @file
///
/// @brief Complex numbers library.
///
/// @author Yek - Edward Klyshinsky, SSSE, 05.08.09.
///         Ose - Sergey Ershov, KLBL, 05.10.07.
///         Enk - Nickolay Kirilov, KLBL, 05.10.21.
///
/// Copyright &copy; INTEGRA, Inc., 2005.

#include <integra.hpp>

#include "complex.hpp"

INTEGRA_NAMESPACE_START

/**
@class TComplex math/complex.hpp

Fast implementation of complex numbers math.

@param T - Base element type. Allowed values: float, double.

**/


template<> const TComplex<double> TComplex<double>::i(0, 1);
template<> const TComplex<double> TComplex<double>::ZERO(0, 0);
template<> const TComplex<float>  TComplex<float>::i(0, 1);
template<> const TComplex<float>  TComplex<float>::ZERO(0, 0);

INTEGRA_NAMESPACE_END
