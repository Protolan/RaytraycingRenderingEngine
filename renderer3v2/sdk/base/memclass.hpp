/// @file
///
/// @brief Declaration of MemoryClass class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @internal
/// @author Eed - Eugene Denisov, AT58, '04.03.30.

#ifndef _AT58_MEMORYCLASS_HPP_
#define _AT58_MEMORYCLASS_HPP_

#include "arrays.hpp"
#include "export.h"
#include "file.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Provides memory management for Array classes.
class MemoryClass
  {
  public:
    /// @name Name related methods
    //@{
    /// Get the name of the class.
    inline const Str &GetName() const;
    //@}

  public:
    /// @name Memory statistics methods
    //@{
    /// Add size.
    inline void Add(SIZE_T size);
    /// Subtract size.
    inline void Del(SIZE_T size);
    //@}

  public:
    /// @name Memory reporting methods
    //@{
    /// Get current size.
    INTAPI_BASE SIZE_T CurSize();
    /// Get max size.
    INTAPI_BASE SIZE_T MaxSize();
    //@}

  public:
    /// @name Iteration methods
    //@{
    /// Get the first object.
    INTAPI_BASE static MemoryClass *FirstClass();
    //@}

  public:
    /// @name Iteration methods
    //@{
    /// Get the next MemoryClass object.
    INTAPI_BASE MemoryClass *NextClass();
    //@}

  public:
    /// @name Name related methods
    //@{
    /// Get memory class for specified name.
    INTAPI_BASE static MemoryClass *GetClass(const char *class_name);
    //@}

  public:
    /// @name Memory reporting methods
    //@{
    /// Get size of memory occupied by class.
    INTAPI_BASE static SIZE_T AllocatedByClass(const char *class_name);
    /// Get size of memory occupied by all classes.
    INTAPI_BASE static SIZE_T AllocatedByAll();
    /// Print memory allocation table for class.
    INTAPI_BASE static void PrintMapForClass(const char *class_name, TArray<Str> &out);
    /// Print memory allocation table for all classes.
    INTAPI_BASE static void PrintMapForAll(TArray<Str> &out);
    //@}

  public:
    /// @name Common tracing methods
    //@{
    /// Should this block be traced or not.
    inline static bool ToTrace(SIZE_T size);
    /// Trace file.
    inline static File &TraceFile();
    //@}

  private:
    /// Constructor.
    MemoryClass(const char *class_name);

  private:
    /// @brief Trace mode enumeration.
    ///
    /// Trace mode enumeration.
    enum TraceMode
      {
      /// Mode is undefined.
      UNDEFINED,
      /// Tracing is activated.
      TRACE,
      /// Tracing is deactivated.
      NO_TRACE
      };

  private:
    /// @name Common tracing attributes
    //@{
    /// Current trace mode.
    INTAPI_BASE static TraceMode trace_mode;
    /// Filename for writing memory trace.
    INTAPI_BASE static File trace_file;
    /// Size starting from which to trace.
    INTAPI_BASE static SIZE_T trace_size;
    //@}

  private:
    /// @name Particular class attributes
    //@{
    /// Class name.
    Str name;
    /// Memory currently occupied by class.
    SIZE_T cur_size;
    /// Max memory ever occupied by class.
    SIZE_T max_size;
    //@}

  private:
    /// @name Representation of all registered memory classes
    //@{
    /// Pointer to top of stack.
    static MemoryClass *top;
    //@}

  private:
    /// @name Representation of all registered memory classes
    //@{
    /// Pointer to next object.
    MemoryClass *next;
    //@}
  };  // class MemoryClass


//////////////////////////////////////////////////////////////////////////////
// Inline methods of MemoryClass


/////////////////////////////////////////////////////////////////////////////
/// Get name of the class.
/// @return The name of this class.
const Str &MemoryClass::GetName() const
  {
  return name;
  }

/////////////////////////////////////////////////////////////////////////////
/// Add size.
///
/// The method adds the specified size to the memory allocation statistics.
/// @param[in] size - The size of a block.
void MemoryClass::Add(SIZE_T size)
  {
  cur_size += size;
  if (cur_size > max_size)
    max_size = cur_size;
  }


/////////////////////////////////////////////////////////////////////////////
/// Subtract size.
/// The method subtracts the specified size from the memory statistics.
/// @param[in] size - the size of a block.
void MemoryClass::Del(SIZE_T size)
  {
  cur_size -= size;
  }


/////////////////////////////////////////////////////////////////////////////
/// Should this block be traced or not.
/// @param[in] size - A block size.
/// @return @b true, if trace mode is enabled and the size matches the trace 
/// condition.
bool MemoryClass::ToTrace(SIZE_T size)
  {
  return (trace_mode == TRACE && trace_size <= size);
  }


/////////////////////////////////////////////////////////////////////////////
/// Trace file.
/// @return A reference to the trace file.
File &MemoryClass::TraceFile()
  {
  return trace_file;
  }

INTEGRA_NAMESPACE_END
#endif
