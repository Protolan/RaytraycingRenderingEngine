/// @internal
/// @file
///
/// @brief Interprocess synchronization definitions (system dependent).
///
/// @author Pnd - Nickolay Derjabin, '02.03.27
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <integra.hpp>
#include "mpsync.hpp"

INTEGRA_NAMESPACE_START

/**
@class MPSync base/mpsync.hpp

Multiprocess synchronization (synchronization of
threads potentially belonging to different processes).

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
MPSync::MPSync()
  {
#ifdef _WIN32
  handle = CreateMutex(NULL, false, NULL);
#else
  handle = new pthread_mutex_t;
  pthread_mutex_init((pthread_mutex_t *)handle, NULL);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
MPSync::~MPSync()
  {
#ifdef _WIN32
  CloseHandle(handle);
#else
  pthread_mutex_destroy((pthread_mutex_t *)handle); 
#endif
  }


//////////////////////////////////////////////////////////////////////////////
/// Lock the plug.
/// @return SUCCESS or FAILURE.
OKAY MPSync::Lock()
  {
#ifdef _WIN32
  return (WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0) ?
         SUCCESS : FAILURE;
#else
  return (pthread_mutex_lock((pthread_mutex_t *)handle) == 0) ?
         SUCCESS : FAILURE;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Unlock the plug.
/// @return SUCCESS or FAILURE.
OKAY MPSync::Unlock()
  {
#ifdef _WIN32
  return (ReleaseMutex(handle) != 0) ? SUCCESS : FAILURE;
#else
  return (pthread_mutex_unlock((pthread_mutex_t *)handle) == 0) ?
         SUCCESS : FAILURE;
#endif
  }

INTEGRA_NAMESPACE_END
