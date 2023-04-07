/// @file
///
/// @brief Dealing with threads, declarations.
///
/// The described here functions provide a unified interface for Multithread
/// supporting. The current implementation is intended
/// only for the Windows platform.
///
/// @sa @ref base_mainpage
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pbb - Boris Barladian, '00.12.07
/// @author Pls - Lev Shapiro, '00.12.07

#ifndef _ELML_THREADS_HPP_
#define _ELML_THREADS_HPP_

/// Constant to inidicate infinite.
#define INFINITE 0xFFFFFFFF

#include "export.h"

INTEGRA_NAMESPACE_START

/// Query the number of physical cores.
INTAPI_BASE int NumberOfPhysicalCores();
/// Query the number of logical cores.
INTAPI_BASE int NumberOfLogicalCores();
/// Query the number of NUMA nodes.
INTAPI_BASE int NumberOfNUMANodes();
/// Query the NUMA node physical cores.
INTAPI_BASE int NUMANodePhysicalCores(int id);
/// Query the NUMA node logical cores.
INTAPI_BASE int NUMANodeLogicalCores(int id);

//////////////////////////////////////////////////////////////////////////////
/// @brief Data for support of multithreading.
///
/// @brief The structure used for passing data to the Multithreading support
/// functions.
struct ThreadContext
  {
  /// A pointer to the function which will be executed in a thread.
  int (*RunThread)(void *data);
  /// A pointer to the passed data.
  void *data_ptr;
  };

//////////////////////////////////////////////////////////////////////////////
/// Thread timing. All the times are in 100-nanosecond units.
struct ThreadTiming
  {
  /// Thread creation time.
  INT64 CreationTime;
  /// Thread exit time.
  INT64 ExitTime;
  /// Thread time that the thread has executed in kernel mode.
  INT64 KernelTime;
  /// Thread time that the thread has executed in user mode.
  INT64 UserTime;
  };

/// @name Thread related functions
//@{
/// Create a thread and run it immediately.
INTAPI_BASE void *IntCreateThread(ThreadContext *cont, const char *name = NULL,
                                  bool use_groups = false, int numa_node_id = -1);
/// Close a thread.
INTAPI_BASE void IntCloseThread(void *handle);
/// Resume a thread.
INTAPI_BASE DWORD  IntResumeThread(void *handle);
/// Suspend a thread.
INTAPI_BASE DWORD  IntSuspendThread(void *handle);
/// Get current thread ID.
INTAPI_BASE void *IntGetCurrentThreadId();
/// Set thread priority above normal.
INTAPI_BASE bool SetThreadPriorityAboveNormal(void *handle);
/// Set thread priority below normal.
INTAPI_BASE bool SetThreadPriorityBelowNormal(void *handle);
/// Set normal thread priority.
INTAPI_BASE bool SetNormalThreadPriority(void *handle);
/// Block execution and wait for specified moment.
INTAPI_BASE int IntWaitForSingleThread(void const *lpHandle,
                                       DWORD dwMilliseconds);
/// Block the execution and wait for the specified moment.
INTAPI_BASE int IntWaitForMultipleThreads(DWORD nCount,
                                          void * const *lpHandles,
                                          bool fWaitAll, DWORD dwMilliseconds);
/// Get thread timing information.
INTAPI_BASE bool IntGetThreadTimes(void *handle, ThreadTiming &times);
//@}

/// @name Critical section related functions
//@{
/// Create and initialize a critical section object.
INTAPI_BASE void IntInitializeCriticalSection(void **crt, int sc = 0);
/// Delete a critical section object.
INTAPI_BASE void IntDeleteCriticalSection(void *crt);
/// Wait for ownership of the specified critical section object.
INTAPI_BASE void IntEnterCriticalSection(void *crt);
/// Release ownership of the specified critical section object.
INTAPI_BASE void IntLeaveCriticalSection(void *crt);
//@}

/// @name Event related functions
//@{
/// Create an event.
INTAPI_BASE void *IntCreateEvent();
/// Create an event.
INTAPI_BASE void *IntCreateEvent(bool manual_reset);
/// Create a named event.
INTAPI_BASE void *IntCreateEvent(bool manual_reset, const char *name);
/// Open an event.
INTAPI_BASE void *IntOpenEvent(const char *name);
/// Close an event.
INTAPI_BASE void IntCloseEvent(void *h_event);
/// Reset an event.
INTAPI_BASE bool IntResetEvent(void *h_event);
/// Set an event.
INTAPI_BASE bool IntSetEvent(void *h_event);
/// Block execution and wait for specified moment.
INTAPI_BASE int IntWaitForSingleEvent(void const *lpHandle,
                                      DWORD dwMilliseconds);
/// Block the execution and wait for the specified moment.
INTAPI_BASE int IntWaitForMultipleEvents(DWORD nCount,
                                         void * const *lpHandles,
                                         bool fWaitAll, DWORD dwMilliseconds);
//@}

/// @name Shared memory related functions
//@{
/// Create a mapping file.
INTAPI_BASE void *IntCreateFileMapping(const char *name, DWORD size);
/// Open a mapping file.
INTAPI_BASE void *IntOpenFileMapping(const char *name);
/// Close a mapping file.
INTAPI_BASE void IntCloseFileMapping(void *handle);
/// Map a shared memory block.
INTAPI_BASE void *IntMapViewOfFile(void *mapfile);
/// Unmap the shared memory block.
INTAPI_BASE bool IntUnmapViewOfFile(void *data_block);
/// Get the real size of the shared memory block.
INTAPI_BASE DWORD IntGetSharedMemBlockSize(void *data_block);
//@}

/// @name Messages and window procedures related functions
//@{
/// Send message.
INTAPI_BASE void *IntSendMessage(HANDLE wnd, DWORD msg, void *data);
/// Send notify message (returns immediately).
INTAPI_BASE bool IntSendNotifyMessage(HANDLE wnd, DWORD msg, void *data);
/// Send message with the callback function (returns immediately,
/// calls the callback function after handling a message).
INTAPI_BASE bool IntSendMessageCallback(HANDLE wnd, DWORD msg, void *data,
                                        void *callback_proc, unsigned long callback_param);
/// Get the WM_USER code.
INTAPI_BASE int GetUserMsgOffset();
/// Get a window procedure.
INTAPI_BASE void *IntGetWndProc(HANDLE wnd);
/// Set a window procedure.
INTAPI_BASE void *IntSetWndProc(HANDLE wnd, void *new_wnd_proc);
/// Call a window procedure.
INTAPI_BASE void *IntCallWindowProc(void *old_wnd_proc,
                                        HANDLE wnd,
                                        DWORD msg,
                                        DWORD wParam,
                                        void *lParam);
/// Send a window to the front of all other windows and activate it.
INTAPI_BASE bool IntSetForegroundWindow(HANDLE hwnd);
/// Get the window which is currently active and on the top.
INTAPI_BASE HANDLE IntGetForegroundWindow();
/// Set a window as topmost or non-topmost without activating it.
INTAPI_BASE void IntSetWndTopmost(HANDLE wnd, bool topmost);
/// Show or hide a window with all it's children.
INTAPI_BASE void IntShowWindow(HANDLE wnd, bool show);
/// Check is process with given identifier running.
INTAPI_BASE bool IntIsProcessRunning(DWORD pid);
/// Check if the specified window exists.
INTAPI_BASE bool IntIsWindow(HANDLE wnd);
/// Check if the specified window is visible.
INTAPI_BASE bool IntIsWindowVisible(HANDLE wnd);
/// Enable or disable the specified window.
INTAPI_BASE bool IntEnableWindow(HANDLE wnd, bool enabled);
/// Get the parent of the specified window.
INTAPI_BASE HANDLE IntGetParent(HANDLE wnd);
/// Get the process ID of a window.
INTAPI_BASE DWORD IntGetWindowPID(HANDLE wnd);
/// Get the current process ID.
INTAPI_BASE DWORD IntGetCurrentProcessId();
//@}

/// OR 64bit operation with integer value (thread-safe).
INTAPI_BASE INT64 IntInterlockedOr64(INT64 &value, INT64 mask);
/// AND 64bit operation with integer value (thread-safe).
INTAPI_BASE INT64 IntInterlockedAnd64(INT64 &value, INT64 mask);
/// COMAPRE and SWAP 64bit operation with integer value (thread-safe).
INTAPI_BASE INT64 IntInterlockedCompareExchange64(INT64 &value, INT64 new_value, INT64 comp_value);

/// Increment integer value (thread-safe).
INTAPI_BASE int IntInterlockedIncrement(int &value);
/// Increment unsigned value (thread-safe).
INTAPI_BASE int IntInterlockedIncrement(unsigned &value);
/// Increment 64bit integer value (thread-safe).
INTAPI_BASE INT64 IntInterlockedIncrement(INT64 &value);
/// Increment 64bit unsigned value (thread-safe).
INTAPI_BASE UINT64 IntInterlockedIncrement(UINT64 &value);

/// Add integer value (thread-safe): pd += d
INTAPI_BASE int AtomicAdd(int &pd, int d); 
/// Add integer value (thread-safe): pd += d
INTAPI_BASE unsigned AtomicAdd(unsigned &pd, unsigned d); 
/// Add 64bit integer value (thread-safe): pd += d
INTAPI_BASE INT64 AtomicAdd(INT64 &pd, INT64 d); 
/// Add 64bit integer value (thread-safe): pd += d
INTAPI_BASE UINT64 AtomicAdd(UINT64 &pd, UINT64 d); 
/// Add float value (thread-safe): pd += d
INTAPI_BASE float AtomicAdd(float &pd, float d); 
/// Add doublr value (thread-safe): pd += d
INTAPI_BASE double AtomicAdd(double &pd, double d); 

INTEGRA_NAMESPACE_END

#endif

