/// @file
///
/// @brief Declaration of methods used for work with cores
///        using newest Win32 API functions.
///
/// @author Eed - Eugene Denisov, '15.03.10.
///
/// Copyright &copy; INTEGRA, Inc., 2015.

#ifndef _CORES_HXX_
#define _CORES_HXX_

INTEGRA_NAMESPACE_START

/// Get total number of cores via newest Win32 API.
int NumOfCores(bool physical);

/// Get total number of processor groups via newest Win32 API.
int NumOfGroups();

/// Get total number of NUMA groups via newest Win32 API.
int NumOfNUMA();

/// Get NUMA node size via newest Win32 API.
int NUMACores(int id, bool physical);

/// Get NUMA node group via newest Win32 API.
WORD NUMAGroup(int id);

/// Get NUMA node mask via newest Win32 API.
ULONG_PTR NUMAMask(int id);

/// Assign given thread to next appropriate processor group.
OKAY ModifyThreadGroup(HANDLE thread);

/// Assign given thread to given NUMA node.
OKAY ModifyThreadNUMANode(HANDLE thread, int numa_node_id);

INTEGRA_NAMESPACE_END
#endif  // Header wrapper
