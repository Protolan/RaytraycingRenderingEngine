/// @file
/// @brief Get real execution time of code pieces in threads (for debugging).
///
/// Copyright &copy; INTEGRA, Inc., 2007.
///
/// @internal
/// @author Pnd - Nick Deriabin, AK64, '07.11.14.

#ifndef _AK64_THREAD_TIME_HPP_
#define _AK64_THREAD_TIME_HPP_

#include "export.h"

INTEGRA_NAMESPACE_START

/// Get real execution time of code pieces in threads (for debugging).
class ThreadTimes
  {
  public:
    /// Constructor.
    INTAPI_BASE ThreadTimes();
    /// Destructor.
    INTAPI_BASE ~ThreadTimes();
    /// Add a thread for profiling.
    INTAPI_BASE void AddThread();
    /// Reset profiling.
    INTAPI_BASE void Reset();
    /// Start piece of code to profile.
    INTAPI_BASE void Start();
    /// End piece of code to profile.
    INTAPI_BASE void End();
    /// Print profile results to log file.
    INTAPI_BASE void Print(const char *title);

  private:
    /// Number of threads under profiling.
    int n_threads;
    /// Thread identifiers.
    DWORD tidents[16];
    /// Thread handles.
    HANDLE handles[16];
    /// Start time of currently profiled piece of code.
    int start[16];
    /// Accumulated thread times.
    int sum[16];
  };

INTEGRA_NAMESPACE_END

#endif

