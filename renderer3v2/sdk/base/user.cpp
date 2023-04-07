/// @internal
/// @file
///
/// @brief Implementation of global User object.
///
/// @author Eed - Eugene Denisov, KLON, '04.07.22.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#include <integra.hpp>

#include "batchuser.hpp"
#include "user.hpp"
#include "threads.hpp"

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
// Static variables - User objects storage

/// Default User object.
static BatchUser batch_user;

/// Current User object; initially it is a BatchUser object.
static IUser *cur_user = &batch_user;

/// User object for the specified thread
static IUser *thread_user = NULL;

/// Id of the current thread
static void *thread_id = 0;


//////////////////////////////////////////////////////////////////////////////
/// Access the default BatchUser object of the given task.
/// @return A pointer to the default BatchUser object.
IUser *GetBatchUser()
  {
  return &batch_user;
  }

//////////////////////////////////////////////////////////////////////////////
/// Access the current User object of the given task.
/// @return A pointer to the current User object.
IUser *User()
  {
  if (thread_user != NULL)
    {
    void *current_id = IntGetCurrentThreadId();
    if (current_id == thread_id)
      return thread_user;
    }
  return cur_user;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new User object for the given task.
/// @param[in] user - A pointer to a new User object.
void SetUser(IUser *user)
  {
  cur_user = user;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a new User object for the given thread.
/// @param[in] user - A pointer to a new User object.
/// @param[in] id - Identifier of the thread
void SetThreadUser(IUser *user, void *id)
  {
  thread_user = user;
  thread_id   = id;
  }

//////////////////////////////////////////////////////////////////////////////
/// C interface for User::LogMessage() to be used in legacy C libraries.
///
/// @param[in] str - Message to be passed to User::LogMessage().
START_C_DECLS
INTAPI_BASE void user_log_message(const char *str)
  {
  User()->LogMessage(str);
  }
END_C_DECLS

INTEGRA_NAMESPACE_END
