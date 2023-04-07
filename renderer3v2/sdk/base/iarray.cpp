/// @internal
/// @file
///
/// @brief Definition of IArray class.
///
/// @author Eed - Eugene Denisov, AT58, '04.03.30.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#include <integra.hpp>
#include "iarray.hpp"
#include "memclass.hpp"

INTEGRA_NAMESPACE_START

/**
@class IArray base/iarray.hpp

Base class for all arrays.

This class provides functionality necessary to support memory
usage statistics. All array classes must be inherited from it.

@sa @ref base_mainpage
**/

/////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// Assigns default values to internal variables.
IArray::IArray()
  {
  mem_class = MemoryClass::GetClass(NULL);
  }

/////////////////////////////////////////////////////////////////////////////
/// Destructor.
IArray::~IArray()
  {
  }


/////////////////////////////////////////////////////////////////////////////
/// Set class and array names.
///
/// Set new class and array names and change the memory class, if necessary.
/// @param[in] class_name A new class name.
/// @param[in] array_name A new array name. 
void IArray::SetName(const char *class_name, const char *array_name)
  {
  name = array_name;
  // Obtain and store (change, if necessary) current memory class.
  mem_class = MemoryClass::GetClass(class_name);
  }

/////////////////////////////////////////////////////////////////////////////
/// Add a block to memory statistics.
///
/// Updates memory stat for the own class and prints info to a trace file.
/// @param[in] size Size of block.
/// @param[in] p Block address.
/// @param[in] str Any string; will be printed in the same line.
void IArray::AddMemStat(SIZE_T size, void *p, const char *str)
  {
  // Workaround for bug in TBT
  if (mem_class == NULL)
    mem_class = MemoryClass::GetClass(NULL);

  if (size != 0 && p != NULL)
    mem_class->Add(size);

  if (mem_class->ToTrace(size))
    mem_class->TraceFile().Printf("Alloc,%#p,%#p,%s,%s,%s\n", p, size,
      mem_class->GetName().IsNull() ? "UNNAMED" :
      mem_class->GetName().Data(),
      name.IsNull() ? "UNNAMED" : name.Data(),
      str);
  }

/////////////////////////////////////////////////////////////////////////////
/// Delete a block from memory statistics.
///
/// Updates memory stat for the own class and prints info to a trace file.
/// @param[in] size Size of block.
/// @param[in] p Block address.
/// @param[in] str Any string; will be printed in the same line.
void IArray::DelMemStat(SIZE_T size, void *p, const char *str)
  {
  // Workaround for bug in TBT
  if (mem_class == NULL)
    mem_class = MemoryClass::GetClass(NULL);

  if (size != 0 && p != NULL)
    mem_class->Del(size);

  if (mem_class->ToTrace(size))
    mem_class->TraceFile().Printf(" Free,%#p,%#p,%s,%s,%s\n", p, size,
      mem_class->GetName().IsNull() ? "UNNAMED" :
      mem_class->GetName().Data(),
      name.IsNull() ? "UNNAMED" : name.Data(),
      str);
  }

INTEGRA_NAMESPACE_END
