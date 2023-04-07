/// @internal
/// @file
///
/// @brief Dealing with threads, definitions.
///
/// @author Pbb - Boris Barladian, '00.12.07
/// @author Pls - Lev Shapiro, '00.12.07
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004

#ifdef _WIN32
/// Minimally required Win32 SDK version.
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <math.h>
#else  // Linux
#include <errno.h>
#include <pthread.h>
#include <sys/resource.h>
#define WFMO
#include "pevents.hxx"
using namespace neosmart;
#endif

#include <integra.hpp>
#include <base/arrays.hpp>
#include <base/envi.hpp>
#include <base/str.hpp>
#include <base/time.hpp>
#include <base/user.hpp>

#ifdef _WIN32
#include "cores.hxx"
#include "cputopology.hxx"
#endif
#include "threads.hpp"

// 64 bit interlocked functions stub for 32 bit
#if _MSC_VER < 1900 && !defined _WIN64

#include <base/tsync.hpp>

/// Critical section handle.
static TSync sync;

//////////////////////////////////////////////////////////////////////////
/// 64 bit interlocked increment stub.
/// @param[in, out] value Value to increment.
/// @return Incremented value.
static INT64 InterlockedIncrement64(volatile INT64 *value)
  {
  sync.Mono();
  (*value)++;
  sync.Multi();
  return *value;
  }  // InterlockedIncrement64()

//////////////////////////////////////////////////////////////////////////
/// 64 bit interlocked add stub.
/// @param[in, out] addend Value to add to.
/// @param[in] value Value to add.
/// @return Resulting value.
static INT64 InterlockedExchangeAdd64(volatile INT64 *addend, INT64 value)
  {
  sync.Mono();
  (*addend) += value;
  sync.Multi();
  return *addend;
  }  // InterlockedExchangeAdd64()

//////////////////////////////////////////////////////////////////////////
/// 64 bit interlocked compare and exchange stub.
/// @param[in, out] destination Destination pointer.
/// @param[in] exchange Value to assign.
/// @param[in] comparand Initial value of destination to compare.
/// @return Previous destination value.
static INT64 InterlockedCompareExchange64(volatile INT64 *destination, INT64 exchange, INT64 comparand)
  {
  sync.Mono();
  INT64 initial = *destination;
  if (*destination == comparand)
    *destination = exchange;
  sync.Multi();
  return initial;
  }  // InterlockedCompareExchange64()

#endif  // !defined _WIN64

INTEGRA_NAMESPACE_START

/// Thread processing function.
#if defined(_WIN32) || defined(DOXYGEN)
DWORD WINAPI ThreadFunc(LPVOID data);
#else  // Linux
void *ThreadFunc(void *data);
#endif

/// Flag to report number of physical cores to log file only once.
static bool physical_reported = false;
/// Flag to report number of logical cores to log file only once.
static bool logical_reported = false;
/// Flag to report number of NUMA nodes to log file only once.
static bool numa_reported = false;

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS

//////////////////////////////////////////////////////////////////////////////
/// Thread processing function.
///
/// This is an application-defined function that serves as the starting
/// address for a thread.
/// @param[in] data Pointer to receiving data.
/// @return Reason for calculation's finish.
#if defined(_WIN32) || defined(DOXYGEN)
DWORD WINAPI ThreadFunc(LPVOID data)
  {
  ThreadContext *cont = (ThreadContext *)data;
  return cont->RunThread(cont->data_ptr);
  }
#else  // Linux
void *ThreadFunc(void *data)
  {
  ThreadContext *cont = (ThreadContext *)data;
  return (void *)(cont->RunThread(cont->data_ptr));
  }
#endif

#ifdef WIN32
#pragma pack(push, 8)
typedef struct THREADNAME_INFO
  {
  DWORD dwType;
  LPCSTR szName;
  DWORD dwThreadID;
  DWORD dwFlags;
  } THREADNAME_INFO;
#pragma pack(pop)
#endif

//////////////////////////////////////////////////////////////////////////////
/// Create a thread and run it immediately.
///
/// The function creates a thread to execute within the address space of
/// the calling process. The thread runs immediately after creation.
/// @param[in] cont - Data for sending to ThreadFunc.
/// @param[in] name - Thread name (for debugging purpose).
/// @param[in] use_groups - If @c true, new threads will be distributed evenly
///            among all available processor groups. Has effect only on systems
///            with more than 64 logical cores; must be set to @c true for
///            calculating threads to use all cores on such systems.
/// @param[in] numa_node_id - If use_groups is true and numa_node_id is >= 0, then
///            newly created thread will be locked to that specific node (and group).
/// @return The thread handle.
void *IntCreateThread(ThreadContext *cont, const char *name, bool use_groups, int numa_node_id)
  {
#ifdef WIN32
  HANDLE thread = NULL;
  DWORD thread_id = 0;

  thread = CreateThread(NULL, 0, ThreadFunc, cont, 0, &thread_id);
  if (thread == NULL)
    return NULL;

  if (use_groups)
    {
    if (numa_node_id >= 0)
      ModifyThreadNUMANode(thread, numa_node_id);
    else
      ModifyThreadGroup(thread);
    }

  if (name != NULL)
    {
    Sleep(10);
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = thread_id;
    info.dwFlags = 0;
    __try
      {
      RaiseException(0x406D1388, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
      }
    __except(EXCEPTION_EXECUTE_HANDLER)
      {
      }
    }
  return (void *)thread;
#else  // Linux
  pthread_t thread;
  struct sched_param param;
  int policy, pri_min, pri_max;

  if (pthread_create(&thread, NULL, ThreadFunc, cont) != 0)
    return NULL;
  // Set real-time medium (0.5) priority for new thread
  policy = SCHED_RR;
  pri_min = sched_get_priority_min(policy);
  pri_max = sched_get_priority_max(policy);
  if (pri_min != -1 && pri_max != -1)
    {
    param.sched_priority = (pri_max - pri_min) / 2;
    pthread_setschedparam(thread, policy, &param);
    }
  return (void *)thread;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Close a thread.
///
/// The function closes an open thread handle.
/// @param[in] handle - Handle to close.
void IntCloseThread(void *handle)
  {
#ifdef WIN32
  CloseHandle(handle);
#else  // Linux
  // thread handles do not need in closing under Linux
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Resume thread.
///
/// Function decrements a thread's suspend count.
/// @param[in] handle Handle of the thread.
/// @return If the function succeeds, the return value is the thread's
/// previous suspend count. If the function fails, the return value is
/// 0xFFFFFFFF. To get extended error information, call GetLastError.
DWORD IntResumeThread(void *handle)
  {
#ifdef WIN32
  return ResumeThread((HANDLE)handle);
#else
  // Not implemented. There is no way to suspend/resume other thread
  // in Linux; besides, this method is not used anywhere yet.
  Assert(false);
  return 0xFFFFFFFF;
#endif
  }  // IntResumeThread()

//////////////////////////////////////////////////////////////////////////////
/// Suspend a thread.
/// @param[in] handle - Handle of the thread.
/// @return If the function succeeds, the return value is the thread's
/// previous suspend count; otherwise, it is 0xFFFFFFFF.
/// To get extended error information, use the GetLastError function.
DWORD IntSuspendThread(void *handle)
  {
#ifdef WIN32
  return SuspendThread((HANDLE)handle);
#else
  // Not implemented. There is no way to suspend/resume other thread
  // in Linux; besides, this method is not used anywhere yet.
  Assert(false);
  return 0xFFFFFFFF;
#endif
  }  // IntSuspendThread()

//////////////////////////////////////////////////////////////////////////////
/// Get current thread ID
/// @return Caller thread's ID.
INTAPI_BASE void *IntGetCurrentThreadId()
  {
#ifdef WIN32
  return (void *)(INT64)GetCurrentThreadId();
#else  // Linux
  return (void *)pthread_self();
#endif
  }  // IntGetCurrentThreadId()

//////////////////////////////////////////////////////////////////////////////
/// Set thread priority above normal.
/// @param[in] handle - Handle of the thread.
/// @return false if function failed.
INTAPI_BASE bool SetThreadPriorityAboveNormal(void *handle)
  {
#ifdef WIN32
  BOOL ret = SetThreadPriority((HANDLE)handle, THREAD_PRIORITY_ABOVE_NORMAL);
  return (ret) ? true : false;
#else  // Linux
  struct sched_param param;
  int policy, pri_min, pri_max;

  // Set higher (0.75) priority
  policy = SCHED_RR;
  pri_min = sched_get_priority_min(policy);
  pri_max = sched_get_priority_max(policy);
  if (pri_min == -1 || pri_max == -1)
    return false;
  param.sched_priority = (int)((pri_max - pri_min) * 0.75 / 2);
  if (pthread_setschedparam((pthread_t)handle, policy, &param) != 0)
    return false;
  return true;
#endif
  }  // SetThreadPriorityAboveNormal()

//////////////////////////////////////////////////////////////////////////////
/// Set thread priority below normal.
/// @param[in] handle - Handle of the thread.
/// @return false if function failed.
INTAPI_BASE bool SetThreadPriorityBelowNormal(void *handle)
  {
#ifdef WIN32
  BOOL ret = SetThreadPriority((HANDLE)handle, THREAD_PRIORITY_BELOW_NORMAL);
  return (ret) ? true : false;
#else  // Linux
  struct sched_param param;
  int policy, pri_min, pri_max;

  // Set lower (0.25) priority
  policy = SCHED_RR;
  pri_min = sched_get_priority_min(policy);
  pri_max = sched_get_priority_max(policy);
  if (pri_min == -1 || pri_max == -1)
    return false;
  param.sched_priority = (int)((pri_max - pri_min) * 0.25 / 2);
  if (pthread_setschedparam((pthread_t)handle, policy, &param) != 0)
    return false;
  return true;
#endif
  }  // SetThreadPriorityBelowNormal()

//////////////////////////////////////////////////////////////////////////////
/// Set normal thread priority.
/// @param[in] handle - Handle of the thread.
/// @return false if function failed.
INTAPI_BASE bool SetNormalThreadPriority(void *handle)
  {
#ifdef WIN32
  BOOL ret = SetThreadPriority((HANDLE)handle, THREAD_PRIORITY_NORMAL);
  return (ret) ? true : false;
#else  // Linux
  struct sched_param param;
  int policy, pri_min, pri_max;

  // Set medium (0.5) priority
  policy = SCHED_RR;
  pri_min = sched_get_priority_min(policy);
  pri_max = sched_get_priority_max(policy);
  if (pri_min == -1 || pri_max == -1)
    return false;
  param.sched_priority = (pri_max - pri_min) / 2;
  if (pthread_setschedparam((pthread_t)handle, policy, &param) != 0)
    return false;
  return true;
#endif
  }  // SetNormalThreadPriority()

//////////////////////////////////////////////////////////////////////////////
/// Block execution and wait for the specified moment.
///
/// The  function returns when one of the following occurs:
/// - Either the specified thread is in the signaled state.
/// - The time-out interval elapses.
/// @param[in] lpHandle Thread handle.
/// @param[in] dwMilliseconds Specifies the time-out interval, in milliseconds.
/// @return
/// - -1: Time-out return.
/// - -2: Failure return.
/// -  0 The state of the specified thread is signaled.
int IntWaitForSingleThread(const void *lpHandle, DWORD dwMilliseconds)
  {
#ifdef WIN32
  DWORD ret = WaitForSingleObject((HANDLE)lpHandle, dwMilliseconds);
  if (ret == WAIT_TIMEOUT)
    return -1;
  if (ret == WAIT_ABANDONED)
    return -2;
  return 0;
#else  // Linux
  timespec ts;
  unsigned sec;
  UINT64 nanosec;

  if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    return -2;

  sec = dwMilliseconds / 1000;
  dwMilliseconds -= sec * 1000;
  ts.tv_sec += sec;
  nanosec = ts.tv_nsec + dwMilliseconds * 1000 * 1000;
  if (nanosec >= 1000 * 1000 * 1000)
    {
    sec = (unsigned)(nanosec / 1000 / 1000 / 1000);
    nanosec -= sec * 1000 * 1000 * 1000;
    ts.tv_sec += sec;
    }
  ts.tv_nsec = nanosec;
  if (pthread_timedjoin_np((pthread_t)lpHandle, NULL, &ts) == ETIMEDOUT)
    return -1;
  return 0;
#endif
  }  // IntWaitForSingleThread()

/// Structure to pass data to waiting threads.
struct WaitingThreadData
  {
  /// Number of objects to wait.
  DWORD nCount;
  /// Handles of objects to wait.
  void * const *lpHandles;
  /// Wait for all or for one.
  bool fWaitAll;
  };

//////////////////////////////////////////////////////////////////////////////
/// Provide thread calculations.
/// @param[in] data Provide access to the thread data.
/// @return retcode.
static int WaitingThread(void *data)
  {
  WaitingThreadData *thread_data = (WaitingThreadData *)data;
  Assert(thread_data->nCount <= MAXIMUM_WAIT_OBJECTS);
  DWORD ret = WaitForMultipleObjects(thread_data->nCount, thread_data->lpHandles, thread_data->fWaitAll, MAX_INT);
  return ret;
  }

//////////////////////////////////////////////////////////////////////////////
/// Block execution and wait for specified moment.
///
/// The  function returns when one of the following occurs:
/// - Either any one or all of the specified threads are in the signaled state.
/// - The time-out interval elapses.
/// @param[in] nCount - Specifies the number of thread handles in the
/// array pointed to by lpHandles.
/// @note Maximum number of accepted thread handles is MAXIMUM_WAIT_OBJECTS ^ 2.
/// All remaining handles will be ignored (not processed).
/// @param[in] lpHandles - Pointer to an array of thread handles.
/// @param[in] fWaitAll - Specifies the wait type. If @b true, the function
/// returns when the state all threads in the lpHandles array is signaled.
/// If @b false, the function returns when the state of any one of the
/// threads set to is signaled. In the latter case, the return value
/// indicates the thread whose state caused the function to return.
/// @param[in] dwMilliseconds - Specifies the time-out interval, in milliseconds.
/// @return
/// - -1: Time-out return.
/// - -2: Failure return.
/// -  0 - nCount: lpHandles array index of the thread that satisfied the wait.
int IntWaitForMultipleThreads(DWORD nCount, void * const *lpHandles,
                              bool fWaitAll, DWORD dwMilliseconds)
  {
#ifdef WIN32
  DWORD ret;

  // WaitForMultipleObjects() can wait for up to MAXIMUM_WAIT_OBJECTS handles
  if (nCount <= MAXIMUM_WAIT_OBJECTS)
    {
    // Use simple way
    ret = WaitForMultipleObjects(nCount, lpHandles, fWaitAll, dwMilliseconds);
    if (ret == WAIT_TIMEOUT)
      return -1;
    if (ret == WAIT_FAILED)
      return -2;
    return ret - WAIT_OBJECT_0;
    }

  // Organize handles into groups, wait for each group in separate thread.
  Assert(nCount <= MAXIMUM_WAIT_OBJECTS * MAXIMUM_WAIT_OBJECTS);
  TArray<void *> waiting_threads;
  TArray<ThreadContext> thread_contexts;
  TArray<WaitingThreadData> thread_data;
  int n, i, left;
  Str thr_name;

  n = (int)ceil((double)nCount / (double)MAXIMUM_WAIT_OBJECTS);
  if (n > MAXIMUM_WAIT_OBJECTS)
    n = MAXIMUM_WAIT_OBJECTS;
  thread_data.Allocate(n);
  thread_contexts.Allocate(n);

  // Create threads
  ret = WAIT_OBJECT_0;
  for (i = 0, left = nCount; i < n; i++, left -= MAXIMUM_WAIT_OBJECTS)
    {
    thread_data[i].nCount = Min(left, MAXIMUM_WAIT_OBJECTS);
    thread_data[i].lpHandles = lpHandles + i * MAXIMUM_WAIT_OBJECTS;
    thread_data[i].fWaitAll = fWaitAll;
    thread_contexts[i].data_ptr = &thread_data[i];
    thread_contexts[i].RunThread = WaitingThread;
    thr_name.Printf("Waiting Thread %d", i);
    waiting_threads.Add(IntCreateThread(&thread_contexts[i], thr_name));
    if (waiting_threads[i] == NULL)
      {
      waiting_threads.Truncate(i);
      ret = WAIT_FAILED;
      break;
      }
    }

  if (ret != WAIT_FAILED)
    {
    // Wait for all waiting threads
    ret = WaitForMultipleObjects(waiting_threads.Length(), waiting_threads.Data(), fWaitAll, dwMilliseconds);
    }

  // Determine return code
  int rc;
  if (ret == WAIT_TIMEOUT)
    rc = -1;
  else if (ret == WAIT_FAILED)
    rc = -2;
  else
    {
    DWORD inner_ret;
    if (GetExitCodeThread(waiting_threads[ret - WAIT_OBJECT_0], &inner_ret) == 0)
      rc = -2;
    else if (inner_ret == WAIT_TIMEOUT)
      rc = -1;
    else if (inner_ret == WAIT_FAILED)
      rc = -2;
    else
      rc = (ret - WAIT_OBJECT_0) * MAXIMUM_WAIT_OBJECTS + (inner_ret - WAIT_OBJECT_0);
    }

  // Terminate waiting threads
  for (i = 0; i < waiting_threads.Length(); i++)
    {
    TerminateThread(waiting_threads[i], 0);
    IntCloseThread(waiting_threads[i]);
    }

  return rc;
#else  // Linux
  if (nCount == 0)
    return -2;

  Timer timer;
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 1000;

  int done;
  TArray<bool> done_arr;
  if (fWaitAll)
    {
    done_arr.Allocate(nCount);
    done_arr.Set(false);
    }

  for (done = 0; done < nCount && timer.Elapsed() < dwMilliseconds; )
    {
    // loop for all threads - check one by one
    for (int i = 0; i < nCount && done < nCount && timer.Elapsed() < dwMilliseconds; i++)
      {
      // Skip already signaled threads
      if (done_arr[i])
        continue;
      if (pthread_tryjoin_np((pthread_t)lpHandles[i], NULL) != EBUSY)
        {
        if (!fWaitAll)
          return i;
        done_arr[i] = true;
        done++;
        }
      }
    nanosleep(&req, NULL);
    }
  if (done < nCount)
    return -1;
  return 0;
#endif
  }  // IntWaitForMultipleThreads()

//////////////////////////////////////////////////////////////////////////////
/// Get thread timing information.
/// @param[in] handle Thread handle.
/// @param[in] times Thread times.
/// @return true/false.
/// @note All the times are in 100-nanosecond units.
INTAPI_BASE bool IntGetThreadTimes(void *handle, ThreadTiming &times)
  {
#ifdef WIN32
  FILETIME ct, et, kt, ut;
  if (GetThreadTimes((HANDLE)handle, &ct, &et, &kt, &ut) == 0)
    return false;

  times.CreationTime = ct.dwHighDateTime * 0xffffffff + ct.dwLowDateTime;
  times.ExitTime     = et.dwHighDateTime * 0xffffffff + et.dwLowDateTime;
  times.KernelTime   = kt.dwHighDateTime * 0xffffffff + kt.dwLowDateTime;
  times.UserTime     = ut.dwHighDateTime * 0xffffffff + ut.dwLowDateTime;
  return true;
#else  // Linux
  Assert(false);
  return false;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Create and initialize a critical section object.
/// @param[in] crt - Pointer to the created critical section object.
/// @param[in] sc - Spin count
void IntInitializeCriticalSection(void **crt, int sc)
  {
#ifdef WIN32
  LPCRITICAL_SECTION lpCriticalSection = new CRITICAL_SECTION;
  InitializeCriticalSection(lpCriticalSection);
  if (sc > 0)
    InitializeCriticalSectionAndSpinCount(lpCriticalSection, 10000);
  *crt = lpCriticalSection;
#else  // Linux
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_t *mutex = new pthread_mutex_t;
  pthread_mutex_init(mutex, &attr);
  pthread_mutexattr_destroy(&attr);
  *crt = mutex;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Delete critical section object.
/// @param[in] crt - Pointer to the critical section object.
void IntDeleteCriticalSection(void *crt)
  {
#ifdef WIN32
  LPCRITICAL_SECTION lpCriticalSection = (LPCRITICAL_SECTION)crt;
  DeleteCriticalSection(lpCriticalSection);
  delete crt;
#else  // Linux
  pthread_mutex_destroy((pthread_mutex_t *)crt);
  delete (pthread_mutex_t *)crt;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Wait for ownership of the specified critical section object.
///
/// The function returns when the calling thread is granted ownership.
/// @param[in] crt Pointer  to the critical section object.
void IntEnterCriticalSection(void *crt)
  {
#ifdef WIN32
  EnterCriticalSection((LPCRITICAL_SECTION)crt);
#else  // Linux
  pthread_mutex_lock((pthread_mutex_t *)crt);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Release ownership of the specified critical section object.
/// @param[in] crt - Pointer to the critical section object.
void IntLeaveCriticalSection(void *crt)
  {
#ifdef WIN32
  LeaveCriticalSection((LPCRITICAL_SECTION)crt);
#else  // Linux
  pthread_mutex_unlock((pthread_mutex_t *)crt);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Create event.
///
/// The function creates an unnamed event object.
/// @return A handle to the created event object.
void *IntCreateEvent()
  {
#ifdef WIN32
  return (void *)CreateEvent(NULL, TRUE, FALSE, NULL);
#else  // Linux
  return (void *)CreateEvent(true, false);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Create event.
///
/// The function creates an unnamed event object.
/// @param[in] manual_reset - If this parameter is TRUE, the function creates 
/// a manual-reset event object, which requires the use of the ResetEvent function 
/// to set the event state to nonsignaled. If this parameter is FALSE, the 
/// function creates an auto-reset event object, and system automatically 
/// resets the event state to nonsignaled after a single waiting thread has been released.
/// @return A handle to the created event object.
void *IntCreateEvent(bool manual_reset)
  {
#ifdef WIN32
  return (void *)CreateEvent(NULL, manual_reset, FALSE, NULL);
#else  // Linux
  return (void *)CreateEvent(manual_reset, false);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Create a named event.
/// @param[in] manual_reset If this parameter is @b true, the function creates
/// a manual-reset event object, which requires the use of the IntResetEvent
/// function to set the event state to nonsignaled. If this parameter is
/// @b false, the function creates an auto-reset event object, and system
/// automatically resets the event state to nonsignaled after a single waiting
/// thread has been released.
/// @param[in] name The name of the event object.
/// @return If the function succeeds, the return value is a handle to the event
/// object. If the named event object existed before the function call, the
/// function returns a handle to the existing object and GetLastError returns
/// ERROR_ALREADY_EXISTS. If the function fails, the return value is NULL.
/// To get extended error information, call GetLastError.
void *IntCreateEvent(bool manual_reset, const char *name)
  {
#ifdef WIN32
  return (void *)CreateEvent(NULL, manual_reset, FALSE, name);
#else  // Linux
  Assert(false);
  return NULL;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Open an event.
/// This function opens a named event.
/// @param[in] name Name of the event.
/// @return A handle to the event object.
void *IntOpenEvent(const char *name)
  {
#ifdef WIN32
  return (void *)OpenEvent(EVENT_ALL_ACCESS, false, name);
#else  // Linux
  // Not implemented. This method is used only in Catia and Qt branches.
  Assert(false);
  return NULL;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Close an event.
///
/// The function closes an open event handle.
/// @param[in] h_event - Handle to close.
void IntCloseEvent(void *h_event)
  {
#ifdef WIN32
  CloseHandle(h_event);
#else  // Linux
  DestroyEvent((neosmart_event_t)h_event);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Reset event.
///
/// The function sets the state of the specified event object to nonsignaled.
/// @param[in] h_event - Handle to the event object.
/// @return If the function succeeds, the return value is nonzero.
/// If the function fails, the return value is zero.
/// To get extended error information, call GetLastError.
bool IntResetEvent(void *h_event)
  {
#ifdef WIN32
  return (ResetEvent(h_event) != 0);
#else  // Linux
  return (ResetEvent((neosmart_event_t)h_event) == 0);
#endif
  }  // IntResetEvent()

//////////////////////////////////////////////////////////////////////////////
/// Set event.
///
/// Function sets the state of the specified event object to signaled.
/// @param[in] h_event Handle to the event object.
/// @return If the function succeeds, the return value is nonzero.
/// If the function fails, the return value is zero.
/// To get extended error information, call GetLastError.
bool IntSetEvent(void *h_event)
  {
#ifdef WIN32
  return (SetEvent(h_event) != 0);
#else  // Linux
  return (SetEvent((neosmart_event_t)h_event) == 0);
#endif
  }  // IntSetEvent()

//////////////////////////////////////////////////////////////////////////////
/// Block execution and wait for the specified moment.
///
/// The  function returns when one of the following occurs:
/// - Either the specified event is in the signaled state.
/// - The time-out interval elapses.
/// @param[in] lpHandle Event handle.
/// @param[in] dwMilliseconds Specifies the time-out interval, in milliseconds.
/// @return
/// - -1: Time-out return.
/// - -2: Failure return.
/// -  0 The state of the specified event is signaled.
int IntWaitForSingleEvent(const void *lpHandle, DWORD dwMilliseconds)
  {
#ifdef WIN32
  DWORD ret = WaitForSingleObject((HANDLE)lpHandle, dwMilliseconds);
  if (ret == WAIT_TIMEOUT)
    return -1;
  if (ret == WAIT_ABANDONED)
    return -2;
  return 0;
#else  // Linux
  int ret = WaitForEvent((neosmart_event_t)lpHandle, dwMilliseconds);
  if (ret == ETIMEDOUT)
    return -1;
  if (ret != 0)
    return -2;
  return 0;
#endif
  }  // IntWaitForSingleEvent()

//////////////////////////////////////////////////////////////////////////////
/// Block execution and wait for specified moment.
///
/// The  function returns when one of the following occurs:
/// - Either any one or all of the specified events are in the signaled state.
/// - The time-out interval elapses.
/// @param[in] nCount - Specifies the number of event handles in the
/// array pointed to by lpHandles.
/// @note Maximum number of accepted event handles is MAXIMUM_WAIT_OBJECTS ^ 2.
/// All remaining handles will be ignored (not processed).
/// @param[in] lpHandles - Pointer to an array of event handles.
/// @param[in] fWaitAll - Specifies the wait type. If @b true, the function
/// returns when the state all events in the lpHandles array is signaled.
/// If @b false, the function returns when the state of any one of the
/// events set to is signaled. In the latter case, the return value
/// indicates the event whose state caused the function to return.
/// @param[in] dwMilliseconds - Specifies the time-out interval, in milliseconds.
/// @return
/// - -1: Time-out return.
/// - -2: Failure return.
/// -  0 - nCount: lpHandles array index of the event that satisfied the wait.
int IntWaitForMultipleEvents(DWORD nCount, void * const *lpHandles,
                              bool fWaitAll, DWORD dwMilliseconds)
  {
#ifdef WIN32
  // In Windows all objects are processed by the same function
  return IntWaitForMultipleThreads(nCount, lpHandles, fWaitAll, dwMilliseconds);
#else  // Linux
  int ret, index;
  ret = WaitForMultipleEvents((neosmart_event_t *)lpHandles, nCount, fWaitAll, dwMilliseconds, index);
  if (ret == ETIMEDOUT)
    return -1;
  if (ret != 0)
    return -2;
  return index;
#endif
  }  // IntWaitForMultipleEvents()

//////////////////////////////////////////////////////////////////////////////
/// Create a mapping file.
///
/// The function creates a mapping file.
/// @param[in] name - Name of mapping file.
/// @param[in] size - Size of mapping file.
void *IntCreateFileMapping(const char *name, DWORD size)
  {
#ifdef WIN32
  return (void *)CreateFileMapping(INVALID_HANDLE_VALUE,
    NULL,
    PAGE_READWRITE,
    0,
    size,
    name);
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
  return 0;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Open a mapping file.
///
/// The function opens a mapping file.
/// @param[in] name - Name of mapping file.
void *IntOpenFileMapping(const char *name)
  {
#ifdef WIN32
  return (void *)OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
  return 0;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Close a mapping file.
///
/// The function closes an open mapping file.
/// @param[in] handle - Handle to close.
void IntCloseFileMapping(void *handle)
  {
#ifdef WIN32
  CloseHandle(handle);
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
#endif
  }

void *IntMapViewOfFile(void *mapfile)
  {
#ifdef WIN32
  void *p = (void *)MapViewOfFile(mapfile,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    0);
  if (p == NULL)
    {
    Str str_err = Envi::FormatErrorMsg(GetLastError());
    User()->LogMessage("Error opening shared memory block for mapfile %#x: %s",
      mapfile, str_err.IsEmpty() ? "<no description>" : str_err.Data());
    }
  return p;
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
  return 0;
#endif
  }

bool IntUnmapViewOfFile(void *data_block)
  {
#ifdef WIN32
  return (UnmapViewOfFile(data_block) != 0);
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
  return false;
#endif
  }

DWORD IntGetSharedMemBlockSize(void *data_block)
  {
#ifdef WIN32
  MEMORY_BASIC_INFORMATION mInfo;
  // Getting the size of the memory block. It is used mainly when the block is already mapped
  // in the other process and we don't know it's size, but also it can let us know that the 
  // newly created block is larger than we asked - usually it's size is multiple to page size - 4 Kbytes
  VirtualQuery(data_block, &mInfo, sizeof(MEMORY_BASIC_INFORMATION));
  return (DWORD)mInfo.RegionSize;
#else  // Linux
  // Not implemented. This method is used only in Catia branch.
  Assert(false);
  return 0;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Query the number of physical cores.
///
/// @return The number of physical cores at the local computer or
/// value set by environment variable INT_THREAD_CFG (set INT_THREAD_CFG=P:#).
int NumberOfPhysicalCores()
  {
  int nproc;

#if defined(WIN32)
  nproc = NumOfCores(true);
  if (nproc == 0)
    {
    // Old OS; use old way
    CpuTopology cpu;
    nproc = cpu.NumberOfSystemCores();
    }
#elif (OS_MARK == 'U')   /* IRIS */

  // Not implemented; use number of logical cores instead
  nproc = sysmp(MP_NPROCS, 0, 0, 0);

#elif (OS_MARK == 'L')

  // Not implemented; use number of logical cores instead
  struct rlimit lim;

  getrlimit(RLIMIT_NPROC, &lim);
  nproc = lim.rlim_cur;

#else /* All other environments */

  // Not implemented - use default
  nproc = 1;

#endif

  int hw_nproc = nproc;
  Str env = Envi::GetEnv("INT_THREAD_CFG");
  nproc = Envi::GetInt(env, "P", nproc);
  if (!physical_reported)
    {
    User()->LogMessage("ExtCores: physical cores %d hw, %d ev", hw_nproc, nproc);
    physical_reported = true;
    }
  return (nproc > 0) ? nproc : 1;
  }  // NumberOfPhysicalCores()

//////////////////////////////////////////////////////////////////////////////
/// Query the number of logical cores.
///
/// Logical cores include also "virtual" cores provided by e.g. HyperThreading. 
/// @return The number of logical cores at the local computer or
/// value set by environment variable INT_THREAD_CFG (set INT_THREAD_CFG=L:#).
int NumberOfLogicalCores()
  {
  int nproc;

#if defined(WIN32)
  nproc = NumOfCores(false);
  if (nproc == 0)
    {
    // Old OS; use old way
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    nproc = (int)SystemInfo.dwNumberOfProcessors;
    }
#elif (OS_MARK == 'U')   /* IRIS */

  nproc = sysmp(MP_NPROCS, 0, 0, 0);

#elif (OS_MARK == 'L')

  struct rlimit lim;

  getrlimit(RLIMIT_NPROC, &lim);
  nproc = lim.rlim_cur;

#else /* All other environments */

  /* Not implemented - use default */
  nproc = 1;

#endif

  int hw_nproc = nproc;
  Str env = Envi::GetEnv("INT_THREAD_CFG");
  nproc = Envi::GetInt(env, "L", nproc);
  if (!logical_reported)
    {
    User()->LogMessage("ExtCores: logical cores %d hw, %d ev", hw_nproc, nproc);
    logical_reported = true;
    }
  return (nproc > 0) ? nproc : 1;
  }  // NumberOfLogicalCores()

//////////////////////////////////////////////////////////////////////////////
/// Query the number of NUMA nodes.
///
/// @return The number of NUMA nodes at the local computer.
int NumberOfNUMANodes()
  {
  int nnuma;

#if defined(WIN32)
  Str env = Envi::GetEnv("INT_THREAD_CFG");
  if (!env.IsEmpty())
    {
    if (Envi::GetInt(env, "N", 1) == 0)
      {
      if (!numa_reported)
        {
        User()->LogMessage("ExtCores: NUMA is disabled by INT_THREAD_CFG");
        numa_reported = true;
        }
      return 0;
      }
    }

  nnuma = NumOfNUMA();
#else /* All other environments */

  /* Not implemented - use default */
  nnuma = 0;

#endif

  if (!numa_reported)
    {
    User()->LogMessage("ExtCores: NUMA nodes %d", nnuma);
    for (int i = 0; i < nnuma; i++)
      {
      char mask_str[128];
      ULONG_PTR mask = NUMAMask(i);
      int k;
      for (k = 0; k < 8 * sizeof(ULONG_PTR); k++)
        {
        mask_str[k] = char(('0' + (mask & 1)));
        mask >>= 1;
        }
      mask_str[k] = '\0';

      User()->LogMessage("ExtCores: NUMA node %d: group %d, %d:%d cores (%s)",
        i, NUMAGroup(i), NUMACores(i, true), NUMACores(i, false), mask_str);
      }
    numa_reported = true;
    }
  return (nnuma >= 0) ? nnuma : 0;
  }  // NumberOfNUMANodes()

//////////////////////////////////////////////////////////////////////////////
/// Query the number of NUMA node physical cores.
///
/// @param[in] id NUMA node index.
/// @return The number of NUMA node physical cores at the local computer.
int NUMANodePhysicalCores(int id)
  {
  int nproc;

#if defined(WIN32)
  nproc = NUMACores(id, true);
#else /* All other environments */

  /* Not implemented - use default */
  nproc = 0;

#endif

  return (nproc >= 0) ? nproc : 0;
  }  // NUMANodePhysicalCores()

//////////////////////////////////////////////////////////////////////////////
/// Query the number of NUMA node logical cores.
///
/// @param[in] id NUMA node index.
/// @return The number of NUMA node logical cores at the local computer.
int NUMANodeLogicalCores(int id)
  {
  int nproc;

#if defined(WIN32)
  nproc = NUMACores(id, false);
#else /* All other environments */

  /* Not implemented - use default */
  nproc = 0;

#endif

  return (nproc >= 0) ? nproc : 0;
  }  // NUMANodeLogicalCores()

//////////////////////////////////////////////////////////////////////////////
/// OR 64bit operation with integer value (thread-safe).
///
/// @param[in] value The variable to be modified.
/// @param[in] mask The applied mask
/// @return Resulting modified value.
///
INTAPI_BASE INT64 IntInterlockedOr64(INT64 &value, INT64 mask)
  {
#ifdef _WIN32
  return (INT64)InterlockedOr64((LONGLONG *)&value, (LONGLONG)mask);
#else  // Linux
  return __sync_or_and_fetch(&value, mask);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// AND 64bit operation with integer value (thread-safe).
///
/// @param[in] value The variable to be modified.
/// @param[in] mask The applied mask
/// @return Resulting modified value.
///
INTAPI_BASE INT64 IntInterlockedAnd64(INT64 &value, INT64 mask)
  {
#ifdef _WIN32
  return (INT64)InterlockedAnd64((LONGLONG *)&value, (LONGLONG)mask);
#else  // Linux
  return __sync_and_and_fetch(&value, mask);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// COMAPRE and SWAP 64bit operation with integer value (thread-safe).
///
/// @param[in,out] value The destination value.
/// @param[in] new_value The exchange value.
/// @param[in] comp_value The value to compare to destination value.
/// @return Resulting modified value.
///
INTAPI_BASE INT64 IntInterlockedCompareExchange64(INT64 &value, INT64 new_value, INT64 comp_value)
  {
#ifdef _WIN32
  return (INT64)InterlockedCompareExchange64((LONGLONG *)&value, (LONGLONG)new_value, (LONGLONG)comp_value);
#else  // Linux
  return __sync_val_compare_and_swap(&value, comp_value, new_value);
#endif
  }


//////////////////////////////////////////////////////////////////////////////
/// Increment integer value (thread-safe).
///
/// @param[in] value The variable to be incremented.
/// @return Resulting incremented value.
///
INTAPI_BASE int IntInterlockedIncrement(int &value)
  {
#ifdef _WIN32
  return (int)InterlockedIncrement((LONG *)&value);
#else  // Linux
  return __sync_add_and_fetch(&value, 1);
#endif
  }


//////////////////////////////////////////////////////////////////////////////
/// Increment unsigned value (thread-safe).
///
/// @param[in] value The variable to be incremented.
/// @return Resulting incremented value.
///
INTAPI_BASE int IntInterlockedIncrement(unsigned &value)
  {
#ifdef _WIN32
  return (int)InterlockedIncrement((LONG *)&value);
#else  // Linux
  return __sync_add_and_fetch(&value, 1);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Increment 64bit integer value (thread-safe).
///
/// @param[in] value The variable to be incremented.
/// @return Resulting incremented value.
///
INTAPI_BASE INT64 IntInterlockedIncrement(INT64 &value)
  {
#ifdef _WIN32
  return (INT64)InterlockedIncrement64((LONGLONG *)&value);
#else  // Linux
  return __sync_add_and_fetch(&value, 1);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Increment 64bit unsigned value (thread-safe).
///
/// @param[in] value The variable to be incremented.
/// @return Resulting incremented value.
///
INTAPI_BASE UINT64 IntInterlockedIncrement(UINT64 &value)
  {
#ifdef _WIN32
  return (UINT64)InterlockedIncrement64((LONGLONG *)&value);
#else  // Linux
  return __sync_add_and_fetch(&value, 1);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Add integer value (thread-safe): pd += d
///
/// @param[in] pd The value will be replaced with pd + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE unsigned AtomicAdd(unsigned &pd, unsigned d)  
  {  
#ifdef _WIN32
  return InterlockedExchangeAdd((LONG *)&pd, d);
#else  // Linux
  return __sync_fetch_and_add(&pd, d);
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Add integer value (thread-safe): pd += d
///
/// @param[in] pd The value will be replaced with pd + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE int AtomicAdd(int &pd, int d)  
  {  
#ifdef _WIN32
  return InterlockedExchangeAdd((LONG *)&pd, d);
#else  // Linux
  return __sync_fetch_and_add(&pd, d);
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Add 64bit integer value (thread-safe): pd += d
///
/// @param[in] pd The value will be replaced with pd + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE INT64 AtomicAdd(INT64 &pd, INT64 d)  
  {  
#ifdef _WIN32
  return InterlockedExchangeAdd64((LONGLONG *)&pd, d);
#else  // Linux
  return __sync_fetch_and_add(&pd, d);
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Add 64bit integer value (thread-safe): pd += d
///
/// @param[in] pd The value will be replaced with pd + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE UINT64 AtomicAdd(UINT64 &pd, UINT64 d)  
  {  
#ifdef _WIN32
  return InterlockedExchangeAdd64((LONGLONG *)&pd, d);
#else  // Linux
  return __sync_fetch_and_add(&pd, d);
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Add float value (thread-safe): pd_ref += d
///
/// @param[in] pd_ref The value will be replaced with pd_ref + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE float AtomicAdd(float &pd_ref, float d)  
  {  
#if defined(WIN32)
#if _MSC_VER >= 1400
  float *pd = &pd_ref;
  union  
    {  
    volatile LONG iOld;  
    float fOld;  
    };  
  union  
    {  
    LONG iNew;  
    float dNew;  
    };  
  for(;;)  
    {  
    iOld = *(LONG*)pd;   // current old value
    dNew = fOld + d;  
    if (InterlockedCompareExchange((volatile LONG*)pd, iNew, iOld) == iOld)  
      return fOld;
    }
#else
  Assert(false); // not implemented
  return 0;
#endif
#else  // Linux
  float *pd = &pd_ref;
  union  
    {  
    long iOld;  
    float fOld;  
    };  
  union  
    {  
    long iNew;  
    float dNew;  
    };  
  for(;;)  
    {  
    iOld = *(long *)pd;   // current old value
    dNew = fOld + d;  
    if (__sync_bool_compare_and_swap((long *)pd, iOld, iNew))  
      return fOld;
    }  
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Add double value (thread-safe): pd_ref += d
///
/// @param[in] pd_ref The value will be replaced with pd_ref + d.
/// @param[in] d The value to be added to the pd parameter
/// @return The function returns the initial value of the pd parameter.
INTAPI_BASE double AtomicAdd(double &pd_ref, double d)  
  {  
#if defined(WIN32)
#if _MSC_VER >= 1400
  double *pd = &pd_ref;
  union  
    {  
    volatile LONGLONG iOld;  
    double dOld;  
    };  
  union  
    {  
    LONGLONG iNew;  
    double dNew;  
    };  
  for(;;)  
    {  
    iOld = *(LONGLONG*)pd;   // current old value
    dNew = dOld + d;  
    if (InterlockedCompareExchange64((volatile LONGLONG*)pd, iNew, iOld) == iOld)  
      return dOld;
    }
#else
  Assert(false); // not implemented
  return 0;
#endif
#else  // Linux
  double *pd = &pd_ref;
  union  
    {  
    long long iOld;  
    double dOld;  
    };  
  union  
    {  
    long long iNew;  
    double dNew;  
    };  
  for(;;)  
    {  
    dOld = *(long long *)pd;   // current old value
    dNew = dOld + d;  
    if (__sync_bool_compare_and_swap((long long *)pd, iOld, iNew))  
      return dOld;
    }  
#endif
  }  

//////////////////////////////////////////////////////////////////////////////
/// Send message.
///
/// The function sends a message to a window. It returns after return of the
/// callback function of the target window.
/// @param[in] wnd Handle to a window.
/// @param[in] msg ID of a message.
/// @param[in] data Pointer to some data which can be proceeded to the window.
/// @return The result returned by a callback function of the target window.
void *IntSendMessage(HANDLE wnd, DWORD msg, void *data)
  {
#ifdef WIN32
  return (void *)SendMessage((HWND)wnd, UINT(WM_USER + msg), 0, (LPARAM)data);
#else
  Assert(false);  // not implemented
  return NULL;
#endif
  }
//////////////////////////////////////////////////////////////////////////////
/// Send notify message.
///
/// The function sends a notification to a window. It returns immediately.
/// @param[in] wnd Handle to a window.
/// @param[in] msg ID of a message.
/// @param[in] data Pointer to some data which can be proceeded to the window.
/// @return True in case of success; false otherwise.
bool IntSendNotifyMessage(HANDLE wnd, DWORD msg, void *data)
  {
#ifdef WIN32
  return SendNotifyMessage((HWND)wnd, UINT(WM_USER + msg), 0, (LPARAM)data) != 0 ? true : false;
#else
  Assert(false);  // not implemented
  return false;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Send message with the callback function (returns immediately,
/// calls the callback function after handling a message).
///
/// @param[in] wnd Handle to a window.
/// @param[in] msg ID of a message.
/// @param[in] data Pointer to some data which can be proceeded to the window.
/// @param[in] callback_proc Pointer to the callback procedure.
/// @param[in] callback_param Parameter of the callback function.
/// @return True in case of success; false otherwise.
INTAPI_BASE bool IntSendMessageCallback(HANDLE wnd, DWORD msg, void *data,
                                        void *callback_proc, unsigned long callback_param)
  {
#if defined(WIN32) && (_MSC_VER >= 1400)
  return SendMessageCallback((HWND)wnd, (UINT)(WM_USER + msg), 0, (LPARAM)data,
    (SENDASYNCPROC)callback_proc, (ULONG_PTR)callback_param) != 0 ? true : false;
#else
  Assert(false); // not implemented
  return false;
#endif
  }

/// Get the WM_USER code.
///
/// The user message codes should be greater than WM_USER message. This function
/// gets the WM_USER code which should be added to any user message code.
/// @return WM_USER value.
INTAPI_BASE int GetUserMsgOffset()
  {
#ifdef WIN32
  return WM_USER;
#else
  Assert(false);  // not implemented
  return 0;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a window procedure.
///
/// @param[in] wnd Handle to a window.
/// @return Pointer to the current window procedure of the target window.
INTAPI_BASE void *IntGetWndProc(HANDLE wnd)
  {
#if defined(WIN32) && (_MSC_VER >= 1400)
#pragma warning(push)
#pragma warning(disable:4312)
  return (void *)GetWindowLongPtr((HWND)wnd, GWLP_WNDPROC);
#pragma warning(pop)
#else
  Assert(false);  // not implemented
  return NULL;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a window procedure.
///
/// @param[in] wnd Handle to a window.
/// @param[in] new_wnd_proc Pointer to the new WNDPROC.
/// @return Pointer to the previous WNDPROC.
INTAPI_BASE void *IntSetWndProc(HANDLE wnd, void *new_wnd_proc)
  {
#if defined(WIN32) && (_MSC_VER >= 1400)
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4244)
  return (void *)SetWindowLongPtr((HWND)wnd, GWLP_WNDPROC, (LONG_PTR)new_wnd_proc);
#pragma warning(pop)
#else
  Assert(false);  // not implemented
  return NULL;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Call a window procedure.
///
/// This function is used when the callback function (WNDPROC) of some window 
/// should be changed for handling some additional messages. The new callback
/// function should call the old one after handling it's own messages.
/// The IntCallWindowProc function calls the old WNDPROC.
/// @param[in] old_wnd_proc Pointer to the old WNDPROC.
/// @param[in] wnd Handle to a window.
/// @param[in] msg ID of a message.
/// @param[in] wParam First argument of the WNDPROC.
/// @param[in] lParam Second argument of the WNDPROC.
/// @return The result of the old WNDPROC.
INTAPI_BASE void *IntCallWindowProc(void *old_wnd_proc,
                                        HANDLE wnd,
                                        DWORD msg,
                                        DWORD wParam,
                                        void *lParam)
  {
#ifdef WIN32
  return (void *)CallWindowProc((WNDPROC)old_wnd_proc,
                        (HWND)wnd,
                        (UINT)msg,
                        (WPARAM)wParam,
                        (LPARAM)lParam);
#else
  Assert(false);  // not implemented
  return NULL;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Send a window to the front of all other windows and activate it.
/// @param[in] hwnd Handle to the target window.
/// @return true if successfull; false otherwise.
INTAPI_BASE bool IntSetForegroundWindow(HANDLE hwnd)
  {
#ifdef WIN32
  return (SetForegroundWindow((HWND)hwnd) == TRUE ? true : false);
#else
  Assert(false);  // not implemented
  return false;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the window which is currently active and on the top.
/// @return Handle to the window.
INTAPI_BASE HANDLE IntGetForegroundWindow()
  {
#ifdef WIN32
  return (HANDLE)GetForegroundWindow();
#else
  Assert(false);  // not implemented
  return NULL;
#endif
  }

#ifdef WIN32

//////////////////////////////////////////////////////////////////////////////
/// Set a window as topmost or non-topmost without activating it.
/// @param[in] wnd Handle to a window.
/// @param[in] topmost If true, set the window as topmost.
/// If not, set the window as non-topmost.
INTAPI_BASE void IntSetWndTopmost(HANDLE wnd, bool topmost)
  {
  if (wnd == NULL)
    return;
  if (topmost == TRUE)
    SetWindowPos((HWND)wnd, HWND_TOPMOST, 0, 0, 0, 0,
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
  else
    SetWindowPos((HWND)wnd, HWND_NOTOPMOST, 0, 0, 0, 0,
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
  }

INTAPI_BASE void IntShowWindow(HANDLE wnd, bool show)
  {
  if (show)
    ShowWindow((HWND)wnd, SW_SHOWNORMAL);
  else
    {
    ShowWindow((HWND)wnd, SW_MINIMIZE);
    ShowWindow((HWND)wnd, SW_HIDE);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Check if process with given identifier exists.
/// @param[in] pid Unique process identifier.
/// @return true if process is running and false in other case.
INTAPI_BASE bool IntIsProcessRunning(DWORD pid)
  {
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
    PROCESS_VM_READ, FALSE, pid);

  if (hProcess == NULL)
    return false;

  CloseHandle(hProcess);
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check if the specified window exists.
/// @param[in] wnd Window handle.
/// @return true if the window exists, false otherwise.
INTAPI_BASE bool IntIsWindow(HANDLE wnd)
  {
  return (IsWindow((HWND)wnd) != 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check if the specified window is visible.
/// @param[in] wnd Window handle.
/// @return true if the window is visible, false otherwise.
INTAPI_BASE bool IntIsWindowVisible(HANDLE wnd)
  {
  return (IsWindowVisible((HWND)wnd) != 0);
  }

/// Enable or disable the specified window.
INTAPI_BASE bool IntEnableWindow(HANDLE wnd, bool enabled)
  {
  return (EnableWindow((HWND)wnd, (BOOL)enabled) != 0);
  }

/// Get the parent of the specified window.
INTAPI_BASE HANDLE IntGetParent(HANDLE wnd)
  {
  return (HANDLE)GetParent((HWND)wnd);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the process ID of a window.
/// #param[in] wnd Handle to the window.
INTAPI_BASE DWORD IntGetWindowPID(HANDLE wnd)
  {
  DWORD pid;
  GetWindowThreadProcessId((HWND)wnd, &pid);
  return pid;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current process ID.
/// @return Current (calling) process ID.
INTAPI_BASE DWORD IntGetCurrentProcessId()
  {
#ifdef WIN32
  return GetCurrentProcessId();
#else
  Assert(false); // Not implemented
  return 0;
#endif
  }

#endif

INTEGRA_NAMESPACE_END
