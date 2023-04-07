/// @internal
/// @file
///
/// @brief Definition of MemoryClass class.
///
/// @author Eed - Eugene Denisov, AT58, '04.03.30.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#include <stdlib.h>
#include <integra.hpp>
#include "base.hpp"
#include "envi.hpp"
#include "memclass.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Name of the trace file.
static const char *MEM_TRACE_FILE = "MEMCLASS_TRACE_FILE";
/// Minimal size which should be traced.
static const char *MEM_TRACE_SIZE = "MEMCLASS_TRACE_SIZE";

// Static members initialization
MemoryClass::TraceMode MemoryClass::trace_mode = MemoryClass::UNDEFINED;
File MemoryClass::trace_file;
SIZE_T MemoryClass::trace_size = 0;
MemoryClass *MemoryClass::top = NULL;

/**
@class MemoryClass base/memclass.hpp

The MemoryClass class provides memory management for array classes.

This class performs memory allocation/deallocation, traces these
procedures and collects memory usage statistics.

Trace file name (where all allocations/deallocations are recorded)
is specified via the environment variable:

@code
set MEMCLASS_TRACE_FILE=mem_trace.out
@endcode

If this variable is not specified, no trace is performed.

It is possible to trace only allocations/deallocations of more than
a certain size. This size (in bytes) is specified via the environment variable

@code
set MEMCLASS_TRACE_SIZE=512
@endcode

If this variable is not specified, tracing of all sizes is performed.

@sa @ref base_mainpage
**/

/////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method sets an own class name and resets counters.
/// @param[in] class_name Class name.
MemoryClass::MemoryClass(const char *class_name)
  {
  cur_size = max_size = 0;
  name = class_name;

  if (trace_mode == UNDEFINED)
    {
    // It is first run of constructor
    char *ev = getenv(MEM_TRACE_FILE);
    // Get environment variable value
    if (ev != NULL)
      {
      trace_mode = TRACE;
      trace_file = File(ev);
      trace_file.Open("w");
      trace_file.Printf("Action,Address,Size,Class,Array,Comment\n");
      ev = getenv(MEM_TRACE_SIZE);
      if (ev != NULL)
        trace_size = Max(atoi(ev), 0);
      }
    else
      trace_mode = NO_TRACE;
    }

  if (top == NULL)
    {
    next = NULL;
    top = this;
    }
  else
    {
    next = top->next;
    top->next = this;
    }
  }  // MemoryClass()


/////////////////////////////////////////////////////////////////////////////
/// Get the current size.
/// @return The currently occupied memory size for this class, in bytes.
SIZE_T MemoryClass::CurSize()
  {
  return cur_size;
  }

/////////////////////////////////////////////////////////////////////////////
/// Get max size.
/// @return The maximal occupied size for this class, in bytes.
SIZE_T MemoryClass::MaxSize()
  {
  return max_size;
  }


/////////////////////////////////////////////////////////////////////////////
/// Get first object.
/// @return Pointer to first object in the stack.
MemoryClass *MemoryClass::FirstClass()
  {
  return top;
  }


/////////////////////////////////////////////////////////////////////////////
/// Get the next MemoryClass object.
/// @return A pointer to next object in the stack.
MemoryClass *MemoryClass::NextClass()
  {
  return next;
  }


/////////////////////////////////////////////////////////////////////////////
/// Get the memory class for specified name.
///
/// If there is no such a class, it is created.
/// If a move from one class to another was requested,
/// the statistics will not be moved as there is no such possibility.
/// @param[in] class_name A class name.
/// @return A new memory class
MemoryClass *MemoryClass::GetClass(const char *class_name)
  {
  MemoryClass *p;

  // Search among existing classes
  for (p = top; p != NULL; p = p->next)
    if (p->name == class_name)
      return p;

  // No class with such name - add one.
  p = new MemoryClass(class_name);
  return p;
  }


/////////////////////////////////////////////////////////////////////////////
/// Get size of memory occupied by the class.
/// @param[in] class_name - A class name whose memory should be reported.
/// @return Total size of memory blocks occupied by the class, in bytes.
SIZE_T MemoryClass::AllocatedByClass(const char *class_name)
  {
  MemoryClass *p;

  for (p = top; p != NULL; p = p->next)
    if (p->name == class_name)
      return p->cur_size;

  // Not found
  return 0;
  }


/////////////////////////////////////////////////////////////////////////////
/// Get size of memory occupied by all classes.
/// @return Total size of memory blocks occupied by all classes, in bytes.
SIZE_T MemoryClass::AllocatedByAll()
  {
  SIZE_T size = 0;
  MemoryClass *p;

  for (p = top; p != NULL; p = p->next)
    size += p->cur_size;

  return size;
  }


/////////////////////////////////////////////////////////////////////////////
/// Print memory a allocation table for the class.
/// @param[in] class_name - A class name whose memory should be reported.
/// @param[out] out An array of strings to append the output.
void MemoryClass::PrintMapForClass(const char *class_name,TArray<Str> &out)
  {
  Str s;
  MemoryClass *p;

  out.Add("    Cur size    Max size  Class name");
  out.Add("------------------------------------");

  for (p = top; p != NULL; p = p->next)
    if (p->name == class_name)
      {
      s.Printf("%10dMB%10dMB  %s",
               Envi::Bytes2MBytes(p->cur_size),
               Envi::Bytes2MBytes(p->max_size),
               p->name.IsNull() ? "UNNAMED" : p->name.Data());
      out.Add(s);
      break;
      }
  if (p == NULL)
    {
    s.Printf("   not found   not found  %s", class_name);
    out.Add(s);
    }
  }  // PrintMapForClass()

/////////////////////////////////////////////////////////////////////////////
/// Print memory allocation table for all classes.
/// @param[out] out Array of string to append the output.
void MemoryClass::PrintMapForAll(TArray<Str> &out)
  {
  Str s;
  MemoryClass *p;

  out.Add("    Cur size    Max size  Class name");
  out.Add("------------------------------------");

  for (p = top; p != NULL; p = p->next)
    {
    s.Printf("%10dMB%10dMB  %s",
             Envi::Bytes2MBytes(p->cur_size),
             Envi::Bytes2MBytes(p->max_size),
             p->name.IsNull() ? "UNNAMED" : p->name.Data());
    out.Add(s);
    }
  }

INTEGRA_NAMESPACE_END
