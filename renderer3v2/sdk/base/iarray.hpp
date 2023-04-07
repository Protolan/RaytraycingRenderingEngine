/// @file
///
/// @brief Declaration of IArray class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @internal
/// @author Eed - Eugene Denisov, AT58, '04.03.30.

#ifndef _AT58_IARRAY_HPP_
#define _AT58_IARRAY_HPP_

#include "export.h"
#include "str.hpp"

INTEGRA_NAMESPACE_START

class MemoryClass;

/// Base class for all arrays.
class IArray
  {
  public:
    /// Set class and array names.
    INTAPI_BASE void SetName(const char *class_name, const char *array_name);
    /// Destructor.
    INTAPI_BASE virtual ~IArray();
  protected:
    /// Constructor.
    INTAPI_BASE IArray();
    /// Add a block to memory statistics.
    INTAPI_BASE void AddMemStat(SIZE_T size, void *p, const char *str);
    /// Delete a block from memory statistics.
    INTAPI_BASE void DelMemStat(SIZE_T size, void *p, const char *str);
  protected:
    /// The object responsible for memory allocations.
    MemoryClass *mem_class;
    /// Array name.
    Str name;
  };  // class IArray

INTEGRA_NAMESPACE_END
#endif
