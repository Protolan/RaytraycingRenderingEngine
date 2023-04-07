/// @file
/// @brief Get real execution time of code pieces in threads (for debugging).
///
/// Copyright &copy; INTEGRA, Inc., 2007.
///
/// @internal
/// @author Pnd - Nick Deriabin, AK64, '07.11.14.

#ifdef _WIN32
#include <windows.h>
#endif
#include <integra.hpp>
#include "thread_time.h"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**

@class ThreadTimes base/thread_time.h

Get real execution time of code pieces in threads (for debugging).

The class use system feature of thread timing to get
execution time of selected pieces of code in thread environment.
It is intended for profiling and can be used for release configuration.

Usage:

@code
// Main application thread:

ThreadTimes thr_timer;

.....

thr_timer.Reset();

.....

thr_timer.Print();


// Profiled thread:

thr_timer.AddThread();

.....

// One or more profiled pieces of code
thr_timer.Start();
.....
thr_timer.End();

@endcode

**/

#if defined _WIN32 && !defined DOXYGEN

//////////////////////////////////////////////////////////////////////////////
/// Get system thread times.
///
/// @param[in] handle Thread handle.
///
static int GetThreadUserTime(HANDLE handle)
  {
  FILETIME t1, t2, t3, t4;
  GetThreadTimes(handle, &t1, &t2, &t3, &t4);
  return (int)t4.dwLowDateTime;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// It should be executed in main thread of application.
///
ThreadTimes::ThreadTimes()
  {
  n_threads = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// It should be executed in main thread of application.
///
ThreadTimes::~ThreadTimes()
  {
  for (int i = 0; i < n_threads; i++)
    CloseHandle(handles[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Add a thread for profiling.
///
/// It should be executed in profiled thread, normally when the thread starts.
///
void ThreadTimes::AddThread()
  {
  if (n_threads >= 16)
    return;
  DWORD tident = GetCurrentThreadId();
  for (int i = 0; i < n_threads; i++)
    {
    if (tidents[i] == tident)
      return;
    }
  tidents[n_threads] = tident;
  handles[n_threads] = OpenThread(THREAD_ALL_ACCESS, FALSE, tident);
  sum[n_threads] = 0;
  n_threads++;
  }

//////////////////////////////////////////////////////////////////////////////
/// Reset profiling.
///
/// It should be executed in main thread of application.
///
void ThreadTimes::Reset()
  {
  for (int i = 0; i < n_threads; i++)
    sum[i] = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start piece of code to profile.
///
/// It should be executed in profiled thread.
///
void ThreadTimes::Start()
  {
  DWORD tident = GetCurrentThreadId();
  for (int i = 0; i < n_threads; i++)
    {
    if (tidents[i] == tident)
      start[i] = GetThreadUserTime(handles[i]);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// End piece of code to profile.
///
/// It should be executed in profiled thread.
///
void ThreadTimes::End()
  {
  DWORD tident = GetCurrentThreadId();
  for (int i = 0; i < n_threads; i++)
    {
    if (tidents[i] == tident)
      sum[i] += GetThreadUserTime(handles[i]) - start[i];
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Print profile results to log file.
///
/// It should be executed in main thread of application.
///
/// @param[in] title Mark text for log message.
///
void ThreadTimes::Print(const char *title)
  {
  User()->LogMessage("%s thread times", title);
  for (int i = 0; i < n_threads; i++)
    User()->LogMessage("   %4.3f", (double)sum[i] / 10000000);
  }

#endif  // _WIN32

INTEGRA_NAMESPACE_END

