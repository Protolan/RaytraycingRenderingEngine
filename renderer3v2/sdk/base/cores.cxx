/// @file
///
/// @brief Definition of methods used for work with cores
///        using newest Win32 API functions.
///
/// @author Eed - Eugene Denisov, '15.03.10.
///
/// Copyright &copy; INTEGRA, Inc., 2015.

#ifdef _WIN32

#ifndef DOXYGEN
//////////////////////////////////////////////////////////////////////////////
// We need new declarations from SDK v8.1, which are absent in our v6.0A.
// We'll include necessary typedefs directly in this file.
// Later all this must be removed in favour of new 'windows.h'.

// 1. Redefine declarations in old 'windows.h' to avoid conflicts
#define _GROUP_AFFINITY _GROUP_AFFINITY_OLD
#define GROUP_AFFINITY GROUP_AFFINITY_OLD
#define PGROUP_AFFINITY PGROUP_AFFINITY_OLD
#define _PROCESSOR_CACHE_TYPE _PROCESSOR_CACHE_TYPE_OLD
#define PROCESSOR_CACHE_TYPE PROCESSOR_CACHE_TYPE_OLD
#define _PROCESSOR_GROUP_INFO _PROCESSOR_GROUP_INFO_OLD
#define PROCESSOR_GROUP_INFO PROCESSOR_GROUP_INFO_OLD
#define PPROCESSOR_GROUP_INFO PPROCESSOR_GROUP_INFO_OLD
#define _PROCESSOR_RELATIONSHIP _PROCESSOR_RELATIONSHIP_OLD
#define PROCESSOR_RELATIONSHIP PROCESSOR_RELATIONSHIP_OLD
#define PPROCESSOR_RELATIONSHIP PPROCESSOR_RELATIONSHIP_OLD
#define _NUMA_NODE_RELATIONSHIP _NUMA_NODE_RELATIONSHIP_OLD
#define NUMA_NODE_RELATIONSHIP NUMA_NODE_RELATIONSHIP_OLD
#define PNUMA_NODE_RELATIONSHIP PNUMA_NODE_RELATIONSHIP_OLD
#define _CACHE_RELATIONSHIP _CACHE_RELATIONSHIP_OLD
#define CACHE_RELATIONSHIP CACHE_RELATIONSHIP_OLD
#define PCACHE_RELATIONSHIP PCACHE_RELATIONSHIP_OLD
#define _GROUP_RELATIONSHIP _GROUP_RELATIONSHIP_OLD
#define GROUP_RELATIONSHIP GROUP_RELATIONSHIP_OLD
#define PGROUP_RELATIONSHIP PGROUP_RELATIONSHIP_OLD
#define _LOGICAL_PROCESSOR_RELATIONSHIP _LOGICAL_PROCESSOR_RELATIONSHIP_OLD
#define LOGICAL_PROCESSOR_RELATIONSHIP LOGICAL_PROCESSOR_RELATIONSHIP_OLD
#define _PROC_THREAD_ATTRIBUTE_NUM _PROC_THREAD_ATTRIBUTE_NUM_OLD
#define PROC_THREAD_ATTRIBUTE_NUM PROC_THREAD_ATTRIBUTE_NUM_OLD
#define CacheUnified CacheUnified_OLD
#define CacheInstruction CacheInstruction_OLD
#define CacheData CacheData_OLD
#define CacheTrace CacheTrace_OLD
#define RelationProcessorCore RelationProcessorCore_OLD
#define RelationNumaNode RelationNumaNode_OLD
#define RelationCache RelationCache_OLD
#define RelationProcessorPackage RelationProcessorPackage_OLD
#define RelationGroup RelationGroup_OLD
#define RelationAll RelationAll_OLD
#define _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_OLD
#define SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_OLD
#define PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX_OLD

#include <windows.h>

// 2. Cancel redefinitions
#undef _GROUP_AFFINITY
#undef GROUP_AFFINITY
#undef PGROUP_AFFINITY
#undef _PROCESSOR_CACHE_TYPE
#undef PROCESSOR_CACHE_TYPE
#undef _PROCESSOR_GROUP_INFO
#undef PROCESSOR_GROUP_INFO
#undef PPROCESSOR_GROUP_INFO
#undef _PROCESSOR_RELATIONSHIP
#undef PROCESSOR_RELATIONSHIP
#undef PPROCESSOR_RELATIONSHIP
#undef _NUMA_NODE_RELATIONSHIP
#undef NUMA_NODE_RELATIONSHIP
#undef PNUMA_NODE_RELATIONSHIP
#undef _CACHE_RELATIONSHIP
#undef CACHE_RELATIONSHIP
#undef PCACHE_RELATIONSHIP
#undef _GROUP_RELATIONSHIP
#undef GROUP_RELATIONSHIP
#undef PGROUP_RELATIONSHIP
#undef _LOGICAL_PROCESSOR_RELATIONSHIP
#undef LOGICAL_PROCESSOR_RELATIONSHIP
#undef _PROC_THREAD_ATTRIBUTE_NUM
#undef PROC_THREAD_ATTRIBUTE_NUM
#undef CacheUnified
#undef CacheInstruction
#undef CacheData
#undef CacheTrace
#undef RelationProcessorCore
#undef RelationNumaNode
#undef RelationCache
#undef RelationProcessorPackage
#undef RelationGroup
#undef RelationAll
#undef _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
#undef SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
#undef PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX

// 3. Declare necessary types

/// KAFFINITY
typedef ULONG_PTR KAFFINITY;

/// GROUP_AFFINITY
typedef struct _GROUP_AFFINITY {
  KAFFINITY Mask;
  WORD      Group;
  WORD      Reserved[3];
} GROUP_AFFINITY, *PGROUP_AFFINITY;

/// PROCESSOR_CACHE_TYPE
typedef enum _PROCESSOR_CACHE_TYPE { 
  CacheUnified,
  CacheInstruction,
  CacheData,
  CacheTrace
} PROCESSOR_CACHE_TYPE;

/// PROCESSOR_GROUP_INFO
typedef struct _PROCESSOR_GROUP_INFO {
  BYTE      MaximumProcessorCount;
  BYTE      ActiveProcessorCount;
  BYTE      Reserved[38];
  KAFFINITY ActiveProcessorMask;
} PROCESSOR_GROUP_INFO, *PPROCESSOR_GROUP_INFO;

/// PROCESSOR_RELATIONSHIP
typedef struct _PROCESSOR_RELATIONSHIP {
  BYTE           Flags;
  BYTE           Reserved[21];
  WORD           GroupCount;
  GROUP_AFFINITY GroupMask[ANYSIZE_ARRAY];
} PROCESSOR_RELATIONSHIP, *PPROCESSOR_RELATIONSHIP;

/// NUMA_NODE_RELATIONSHIP
typedef struct _NUMA_NODE_RELATIONSHIP {
  DWORD          NodeNumber;
  BYTE           Reserved[20];
  GROUP_AFFINITY GroupMask;
} NUMA_NODE_RELATIONSHIP, *PNUMA_NODE_RELATIONSHIP;

/// CACHE_RELATIONSHIP
typedef struct _CACHE_RELATIONSHIP {
  BYTE                 Level;
  BYTE                 Associativity;
  WORD                 LineSize;
  DWORD                CacheSize;
  PROCESSOR_CACHE_TYPE Type;
  BYTE                 Reserved[20];
  GROUP_AFFINITY       GroupMask;
} CACHE_RELATIONSHIP, *PCACHE_RELATIONSHIP;

/// GROUP_RELATIONSHIP
typedef struct _GROUP_RELATIONSHIP {
  WORD                 MaximumGroupCount;
  WORD                 ActiveGroupCount;
  BYTE                 Reserved[20];
  PROCESSOR_GROUP_INFO GroupInfo[ANYSIZE_ARRAY];
} GROUP_RELATIONSHIP, *PGROUP_RELATIONSHIP;

/// LOGICAL_PROCESSOR_RELATIONSHIP
typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
    RelationProcessorCore,
    RelationNumaNode,
    RelationCache,
    RelationProcessorPackage,
    RelationGroup,
    RelationAll = 0xffff
} LOGICAL_PROCESSOR_RELATIONSHIP;

/// SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX {
  LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
  DWORD                          Size;
  union {
    PROCESSOR_RELATIONSHIP Processor;
    NUMA_NODE_RELATIONSHIP NumaNode;
    CACHE_RELATIONSHIP     Cache;
    GROUP_RELATIONSHIP     Group;
  };
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX;

/// Pointer to SetThreadGroupAffinity() function.
typedef WINBASEAPI BOOL (WINAPI *STGA)(HANDLE hThread,
                                       CONST GROUP_AFFINITY *GroupAffinity,
                                       PGROUP_AFFINITY PreviousGroupAffinity);

/// Pointer to GetLogicalProcessorInformationEx() function.
typedef BOOL (*GLPIEX)(LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
                       PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
                       PDWORD ReturnedLength);


#endif  // DOXYGEN

#include <integra.hpp>
#include "cores.hxx"
#include <base/arrays.hpp>
#include <base/envi.hpp>
#include <base/user.hpp>

INTEGRA_NAMESPACE_START

/// Flag to report glpiex problems to log file only once.
static bool glpiex_reported = false;
/// Flag to report stga problems to log file only once.
static bool stga_reported = false;
/// Flag to report stga NUMA problems to log file only once.
static bool stgan_reported = false;
/// Flag to report number of cores to log file only once.
static bool cores_reported = false;
/// Flag to report number of groups to log file only once.
static bool groups_reported = false;
/// Flag to report number of NUMA groups to log file only once.
static bool numa_reported = false;

/// Number of physical processors.
static int num_physical = -1;
/// Number of logical processors.
static int num_logical = -1;
/// Number of processor groups.
static int num_groups = 0;
/// Processor affinities for every processor group.
static KAFFINITY *groups_masks = NULL;
/// Current number of processor group for next thread.
static int current_group = 0;

/// List of groups in which physical processor cores are defined.
static TArray<WORD> core_groups;
/// List of affinity masks for each physical processor core.
static TArray<KAFFINITY> core_masks;

/// Number of NUMA groups.
static int num_numa = -1;
/// Threat processors as NUMA nodes, for testing
static bool processos_as_numa;
/// Number of physical cores in each NUMA node.
static TArray<int> numa_physical;
/// Number of logical cores in each NUMA node.
static TArray<int> numa_logical;
/// List of groups in which NUMA nodes are defined.
static TArray<WORD> numa_groups;
/// List of affinity masks for each NUMA node.
static TArray<KAFFINITY> numa_masks;

//////////////////////////////////////////////////////////////////////////////
// Static functions declarations

/// Smart envelope for GetLogicalProcessorInformationEx()
static DWORD IntGetLogicProcInfEx(LOGICAL_PROCESSOR_RELATIONSHIP rel_type,
                                  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX **buf);

//////////////////////////////////////////////////////////////////////////////
/// Get total number of cores via newest Win32 API.
/// @param[in] physical - if @c true, number of physical cores is returned;
///                       if @c false, number of logical cores is returned.
/// @return Number of cores of requested kind; if failure (required API is not
/// supported on current platform, or not enough memory) - 0 is returned.
int NumOfCores(bool physical)
  {
  // If already was inquired, return saved values
  if (num_physical >= 0)
    return physical ? num_physical : num_logical;

  // Inquiry OS once and store numbers of cores
  DWORD length;
  PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf, cur;

  num_physical = num_logical = 0;
  core_groups.Truncate();
  core_masks.Truncate();
  length = IntGetLogicProcInfEx(RelationProcessorCore, &buf);
  if (length == 0)
    return 0;

  // Count cores
  int i, p = 0, l = 0;
  cur = buf;
  KAFFINITY mask;
  for ( ; ; )
    {
    // Safety check
    if (cur->Size == 0 || length < cur->Size)
      {
      if (!cores_reported)
        {
        User()->LogMessage("ExtCores: glpiex returned inconsistent data for cores, ignored");
        cores_reported = true;
        }
      p = l = 0;
      break;
      }
    // Process only requested relationships
    if (cur->Relationship == RelationProcessorCore)
      {
      mask = cur->Processor.GroupMask[0].Mask;
      core_groups.Add(cur->Processor.GroupMask[0].Group);
      core_masks.Add(mask);
      for (i = 0; i < sizeof(mask) * 8; i++)
        {
        if (mask & ((KAFFINITY)1 << i))
          l++;
        }
      p++;
      }
    length -= cur->Size;
    if (length == 0)
      break;
    cur = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((BYTE *)cur + cur->Size);
    }  // for()
  num_physical = p;
  num_logical = l;
  delete[] buf;
  if (!cores_reported)
    {
    User()->LogMessage("ExtCores: glpiex returned %d:%d cores", num_physical, num_logical);
    cores_reported = true;
    }
  return physical ? num_physical : num_logical;
  }  // NumOfCores()

//////////////////////////////////////////////////////////////////////////////
/// Get total number of processor groups via newest Win32 API.
/// @return Number of processor groups. If failure, 1 is returned for safety.
int NumOfGroups()
  {
  // If already was inquired, return saved value
  if (num_groups > 0)
    return num_groups;

  // Inquiry OS once and store data
  num_groups = 1;
  DWORD length;
  PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf;

  length = IntGetLogicProcInfEx(RelationGroup, &buf);
  if (length == 0)
    return num_groups;

  // Safety check
  if (buf->Size == 0 || length < buf->Size)
    {
    if (!groups_reported)
      {
      User()->LogMessage("ExtCores: glpiex returned inconsistent data for groups, ignored");
      groups_reported = true;
      }
    delete[] buf;
    return num_groups;
    }
  // Process only requested relationships
  if (buf->Relationship == RelationGroup)
    {
    num_groups = buf->Group.ActiveGroupCount;
    // Array is created here and never destroyed;
    // it must exist until program termination
    groups_masks = new KAFFINITY[num_groups];
    if (groups_masks != NULL)
      for (int i = 0; i < num_groups; i++)
        groups_masks[i] = buf->Group.GroupInfo[i].ActiveProcessorMask;
    }
  delete[] buf;
  if (!groups_reported)
    {
    User()->LogMessage("ExtCores: glpiex returned %d groups", num_groups);
    groups_reported = true;
    }
  return num_groups;
  }  // NumOfGroups()

//////////////////////////////////////////////////////////////////////////////
/// Get total number of NUMA nodes via newest Win32 API.
/// @return Number of NUMA nodes. If failure, 0 is returned.
int NumOfNUMA()
  {
  // If already was inquired, return saved value
  if (num_numa >= 0)
    return num_numa;

  processos_as_numa = false;
  Str env = Envi::GetEnv("INT_THREAD_CFG");
  if (!env.IsEmpty())
    {
    if (Envi::GetInt(env, "T", 0) == 1)
      {
      User()->LogMessage("ExtCores: processors as NUMA is enabled");
      processos_as_numa = true;
      }
    }

  num_numa = 0;
  numa_physical.Truncate();
  numa_logical.Truncate();
  numa_groups.Truncate();
  numa_masks.Truncate();

  if (NumOfCores(true) <= 0)
    return num_numa;

  // This is for testing NUMA functionality on multiprocessor PC without NUMA
  // If flag is set we are getting processors information instead on NUMA nodes
  if (processos_as_numa)
    {
    // Inquiry OS once and store data
    DWORD length;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf, cur;

    length = IntGetLogicProcInfEx(RelationProcessorPackage, &buf);
    if (length == 0)
      return num_numa;

    // Count cores
    int i;
    cur = buf;
    for ( ; ; )
      {
      // Safety check
      if (cur->Size == 0 || length < cur->Size)
        {
        if (!numa_reported)
          {
          User()->LogMessage("ExtCores: glpiex returned inconsistent data for NUMA nodes, ignored");
          numa_reported = true;
          }
        break;
        }
      // Process only requested relationships
      if (cur->Relationship == RelationProcessorPackage)
        {
        int p(0), l(0);

        for (i = 0; i < core_masks.Length(); i++)
          if (cur->Processor.GroupMask[0].Group == core_groups[i] && 
            (cur->Processor.GroupMask[0].Mask & core_masks[i]))
            p++;

        for (i = 0; i < sizeof(cur->Processor.GroupMask[0].Mask) * 8; i++)
          if (cur->Processor.GroupMask[0].Mask & ((KAFFINITY)1 << i))
            l++;

        num_numa++;
        numa_physical.Add(p);
        numa_logical.Add(l);
        numa_groups.Add(cur->Processor.GroupMask[0].Group);
        numa_masks.Add(cur->Processor.GroupMask[0].Mask);
        }
      length -= cur->Size;
      if (length == 0)
        break;
      cur = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((BYTE *)cur + cur->Size);
      }  // for()
    delete[] buf;
    }  // Processors info
  else  // Get NUMA nodes info
    {
    // Inquiry OS once and store data
    DWORD length;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buf, cur;

    length = IntGetLogicProcInfEx(RelationNumaNode, &buf);
    if (length == 0)
      return num_numa;

    // Count cores
    int i;
    cur = buf;
    for ( ; ; )
      {
      // Safety check
      if (cur->Size == 0 || length < cur->Size)
        {
        if (!numa_reported)
          {
          User()->LogMessage("ExtCores: glpiex returned inconsistent data for NUMA nodes, ignored");
          numa_reported = true;
          }
        break;
        }
      // Process only requested relationships
      if (cur->Relationship == RelationNumaNode)
        {
        int p(0), l(0);

        for (i = 0; i < core_masks.Length(); i++)
          if (cur->NumaNode.GroupMask.Group == core_groups[i] && (
            cur->NumaNode.GroupMask.Mask & core_masks[i]))
            p++;

        for (i = 0; i < sizeof(cur->NumaNode.GroupMask.Mask) * 8; i++)
          if (cur->NumaNode.GroupMask.Mask & ((KAFFINITY)1 << i))
            l++;

        num_numa++;
        numa_physical.Add(p);
        numa_logical.Add(l);
        numa_groups.Add(cur->NumaNode.GroupMask.Group);
        numa_masks.Add(cur->NumaNode.GroupMask.Mask);
        }
      length -= cur->Size;
      if (length == 0)
        break;
      cur = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((BYTE *)cur + cur->Size);
      }  // for()
    delete[] buf;
    }  // NUMA nodes info

  if (!numa_reported)
    {
    User()->LogMessage("ExtCores: glpiex returned %d NUMA nodes", num_numa);
    numa_reported = true;
    }

  return num_numa;
  }  // NumOfNUMA()

//////////////////////////////////////////////////////////////////////////////
/// Get NUMA node size via newest Win32 API.
/// @param[in] id NUMA node index.
/// @param[in] physical - if @c true, number of physical cores is returned;
///                       if @c false, number of logical cores is returned.
/// @return NUMA node group. If failure, 0 is returned.
int NUMACores(int id, bool physical)
  {
  if (id < 0 || id >= NumOfNUMA())
    return 0;
  return physical ? numa_physical[id] : numa_logical[id];
  }  // NUMACores()

//////////////////////////////////////////////////////////////////////////////
/// Get NUMA node group via newest Win32 API.
/// @param[in] id NUMA node index.
/// @return NUMA node group. If failure, 0 is returned.
WORD NUMAGroup(int id)
  {
  if (id < 0 || id >= NumOfNUMA())
    return 0;
  return numa_groups[id];
  }  // NUMAGroup()

//////////////////////////////////////////////////////////////////////////////
/// Get NUMA node mask via newest Win32 API.
/// @param[in] id NUMA node index.
/// @return NUMA node mask. If failure, 0 is returned.
ULONG_PTR NUMAMask(int id)
  {
  if (id < 0 || id >= NumOfNUMA())
    return 0;
  return numa_masks[id];
  }  // NUMAMask()

// Optimization results in permanent failures of SetThreadGroupAffinity().
// Perhaps bug in compiler.
#pragma optimize("", off)
//////////////////////////////////////////////////////////////////////////////
/// Assign given thread to next appropriate processor group.
///
/// Each next thread is assigned to next processor group; all available
/// processor groups are used, so large number of threads will be distributed
/// evenly among all available processor groups.
/// @param[in] thread Thread to be assigned to appropriate processor group.
/// @return SUCCESS or FAILURE.
OKAY ModifyThreadGroup(HANDLE thread)
  {
#ifdef _WIN64
  STGA stga;

  int groups = NumOfGroups();

  if (thread == NULL)
    return FAILURE;

  // If only one processor group is available, do nothing
  if (groups < 2)
    return SUCCESS;

  if (groups_masks == NULL)
    {
    if (!stga_reported)
      {
      User()->LogMessage("ExtCores: groups affinity not found");
      stga_reported = true;
      }
    return FAILURE;
    }

  // Find necessary function
  stga = (STGA)GetProcAddress(GetModuleHandle("kernel32"),
                              "SetThreadGroupAffinity");
  if (stga == NULL)
    {
    if (!stga_reported)
      {
      User()->LogMessage("ExtCores: stga not found");
      stga_reported = true;
      }
    return NULL;
    }

  GROUP_AFFINITY group_affinity;
  memset(&group_affinity, 0, sizeof(GROUP_AFFINITY));
  group_affinity.Group = (WORD)current_group;
  group_affinity.Mask = groups_masks[current_group];
  if (stga(thread, &group_affinity, NULL) == 0)
    {
    User()->LogMessage("ExtCores: stga for group %u mask %p error code %#x",
      group_affinity.Group, group_affinity.Mask, GetLastError());
    return FAILURE;
    }

  // Calculate group number for next thread
  current_group = (current_group + 1) % groups;
  return SUCCESS;
#else
  // 32 bit Windows does not support more than 64 cores.
  if (!stga_reported)
    {
    User()->LogMessage("ExtCores: stga not supported on 32 bit");
    stga_reported = true;
    }
  return SUCCESS;
#endif
  }  // ModifyThreadGroup()

//////////////////////////////////////////////////////////////////////////////
/// Assign given thread to given NUMA node and corresponding group.
/// @param[in] thread Thread to be assigned to appropriate NUMA node.
/// @param[in] numa_node_id Index of NUMA node to assign thread to.
/// @return SUCCESS or FAILURE.
OKAY ModifyThreadNUMANode(HANDLE thread, int numa_node_id)
  {
#ifdef _WIN64
  STGA stga;

  int numas = NumOfNUMA();

  if (thread == NULL)
    return FAILURE;

  // If only one NUMA node is available, do nothing
  if (numas < 2)
    return SUCCESS;

  if (numa_node_id < 0 || numa_node_id >= numas)
    return FAILURE;

  if (numa_groups.Length() != numas || numa_masks.Length() != numas)
    {
    if (!stgan_reported)
      {
      User()->LogMessage("ExtCores: NUMA affinity not found");
      stgan_reported = true;
      }
    return FAILURE;
    }

  // Find necessary function
  stga = (STGA)GetProcAddress(GetModuleHandle("kernel32"),
    "SetThreadGroupAffinity");
  if (stga == NULL)
    {
    if (!stga_reported)
      {
      User()->LogMessage("ExtCores: stga not found");
      stga_reported = true;
      }
    return NULL;
    }

  GROUP_AFFINITY group_affinity;
  memset(&group_affinity, 0, sizeof(GROUP_AFFINITY));
  group_affinity.Group = (WORD)numa_groups[numa_node_id];
  group_affinity.Mask = numa_masks[numa_node_id];
  if (stga(thread, &group_affinity, NULL) == 0)
    {
    User()->LogMessage("ExtCores: stga for group %u mask %p error code %#x",
      group_affinity.Group, group_affinity.Mask, GetLastError());
    return FAILURE;
    }

  return SUCCESS;
#else
  // 32 bit Windows does not support more than 64 cores.
  if (!stga_reported)
    {
    User()->LogMessage("ExtCores: stga not supported on 32 bit");
    stga_reported = true;
    }
  return SUCCESS;
#endif
  }
#pragma optimize("", on)

//////////////////////////////////////////////////////////////////////////////
// Static functions

//////////////////////////////////////////////////////////////////////////////
/// Smart envelope for GetLogicalProcessorInformationEx()
///
/// Function determines availability of GetLogicalProcessorInformationEx()
/// in system, and calls it with specified relationship type.
/// Array of SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX structures is returned.
/// @param[in] rel_type Relationship type.
/// @param[out] buf Resulting array of SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
///                 structures.
/// @return Length of buf, in bytes. In case of failure 0 is returned and value
///         of @c buf is undefined. If positive number is returned, caller is
///         responsible to free allocated buffer @c buf using @c delete[].
static DWORD IntGetLogicProcInfEx(LOGICAL_PROCESSOR_RELATIONSHIP rel_type,
                                  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX **buf)
  {
#ifdef _WIN64
  GLPIEX glpiex;
  DWORD length = 0;
  PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer;

  glpiex = (GLPIEX)GetProcAddress(GetModuleHandle("kernel32"),
                                  "GetLogicalProcessorInformationEx");
  if (glpiex == NULL)
    {
    if (!glpiex_reported)
      {
      User()->LogMessage("ExtCores: glpiex not found");
      glpiex_reported = true;
      }
    return 0;
    }
  if (glpiex(rel_type, NULL, &length))
    return 0;
  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    return 0;
  buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)new BYTE[length];
  if (buffer == NULL)
    return 0;
  if (!glpiex(rel_type, buffer, &length))
    {
    delete[] buffer;
    return 0;
    }
  if (length == 0)
    {
    delete[] buffer;
    return 0;
    }
  *buf = buffer;
  return length;
#else
  // Above code results in unpredictable behavior on 32 bit.
  // Reason could not be found. Code is disabled for 32 bit.
  // Anyhow, 32 bit Windows does not support more than 64 cores.
  if (!glpiex_reported)
    {
    User()->LogMessage("ExtCores: glpiex not supported on 32 bit");
    glpiex_reported = true;
    }
  return 0;
#endif
  }  // IntGetLogicProcInfEx()

INTEGRA_NAMESPACE_END

#endif
