/// @internal
/// @file
///
/// @brief Thread synchronization definitions (system dependent).
///
/// @author Pnd - Nickolay Derjabin, '02.06.20
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004

/// Minimally required Win32 SDK version.
#ifdef _WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#else
#include <semaphore.h>
#include <time.h>
#endif

#include <integra.hpp>
#include "threads.hpp"
#include "tsync.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class TSync base/tsync.hpp

Synchronization of threads belonging to the
same process via the critical section mechanism.

Only one thread at a time can enter a critical section.

The class keeps a Windows critical section in its representation.
An instance of the class provides the methods Mono() and Multi() to enter
and leave the critical section. The methods return no value.

Mono() blocks execution of the current thread if the critical
section is occupied by other thread of the same process.

TryMono() attempts to enter the critical section without blocking the
calling thread, and returns immediately.
If succeeded (returned true) it enters the critical section,
i.e. the thread gets exclusive access to it as if we called Mono().
If failed (returned false) the thread was denied that access,
and the caller must decide what to do: wait or return with failure etc.
Anyway it must not execute the code belonging to the critical section.

The same thread may issue nested Mono() calls on the same critical section.
They do not block the thread execution. To leave the critical section,
the thread must issue the same number of Multi() calls.
This practice is not recommended, however.

The programmer should avoid deadlock situations that are possible if
several critical sections are used together.

Example:

@code
  // Create a critical section
  TSync sync;

  .....

  // Enter the critical section
  sync.Mono();

  ... // code belonging to the critical section

  // Leave the critical section
  sync.Multi();

  .....

  // attempt to enter the critical section; if not allowed, return
  if (!sync.TryMono())
    return FAILURE;

  ... // code belonging to the critical section

  // Leave the critical section
  sync.Multi();
@endcode

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
TSync::TSync()
  {
#ifdef _WIN32
  handle = new CRITICAL_SECTION;
  Assert(handle != NULL);
  InitializeCriticalSectionAndSpinCount((LPCRITICAL_SECTION)handle, 1000);
//  InitializeCriticalSection((LPCRITICAL_SECTION)handle);
#else
  handle = new sem_t;
  Assert(handle != NULL);
  int res = sem_init((sem_t *)handle, 0, 1);
  Assert(res == 0);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
TSync::~TSync()
  {
#ifdef _WIN32
  DeleteCriticalSection((LPCRITICAL_SECTION)handle);
  delete handle;
#else
  sem_destroy((sem_t *)handle);
  delete (sem_t *)handle;
#endif
  handle = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator is unfeasible for handles.
///
/// This operator does nothing and causes assert violation in debug version.
/// @param[in] src Source object.
void TSync::operator =(const TSync &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor is unfeasible for handles.
///
/// This operator does nothing and causes assert violation in debug version.
/// @param[in] src Source object.
TSync::TSync(const TSync &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
/// Enter the critical section.
void TSync::Mono()
  {
  // Order of destruction for static objects is undefined; prevent crash
  if (handle == NULL)
    return;
#ifdef _WIN32
  EnterCriticalSection((LPCRITICAL_SECTION)handle);
#else
  sem_wait((sem_t *)handle);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Leave the critical section.
void TSync::Multi()
  {
  // Order of destruction for static objects is undefined; prevent crash
  if (handle == NULL)
    return;
#ifdef _WIN32
  LeaveCriticalSection((LPCRITICAL_SECTION)handle);
#else
  sem_post((sem_t *)handle);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Try to enter the critical section with IMMEDIATE return w/o blocking 
/// the thread.
///
/// In case the call succeeds, the calling thread gets exclusive access and
/// occupies the critical section blocking other threads from it.
/// @return 
/// - @b true if the thread gets exclusive access to the section;
/// - @b false if this was denied (because another thread occupies
/// the critical section).
bool TSync::TryMono()
  {
#ifdef _WIN32
  return TryEnterCriticalSection((LPCRITICAL_SECTION)handle) == TRUE;
#else
  return sem_trywait((sem_t *)handle) == 0;
#endif
  }

/**
@class TAutoSync base/tsync.hpp

Automation of TSync usage.

The class provides some automation to the thread synchronization via
critical sections mechanism implemented in TSync.
Namely, its object remembers a pointer to a TSync object it was
"created from" and on deletion of the object leaves the critical section.

This makes life easier when a function has many control paths, and using
TSync one would need to call Multi() before each return statement:

@code
void MyClass::Method()
  {
  tsync.Mono(); // enter critical section
  ...
  if (condition1)
    {
    tsync.Multi(); // leave critical section
    return;
    }
  ...
  if (condition2)
    {
    tsync.Multi(); // leave critical section
    return;
    }
  ...
  tsync.Multi(); // leave critical section
  return;
  }
@endcode

Here tsync is a member variable of @c MyClass (because this object can not
be local and must be "persistent": either static or a class member).

In such a code, it is easy to forget to and miss leaving the critical section;
and that will be fatal. Using @c TAutoSync, this code can be rewritten as

@code
void MyClass::Method()
  {
  // creates a local object whose destruction will leave the critical section
  TAutoSync async(&tsync);
 
  async.Mono(); // enter critical section
  ...
  if (condition1)
    return;
  ...
  if (condition2)
    return;
  ...
  return;
  }
@endcode

Now any return from the function destroys the local variable async and its
destructor calls @c Multi() for the critical section it shares i.e. @c tsync.
Thus Multi() and Mono() can be equally called on @c tsync and @c async with
the same effect.

One must be careful because critical section objects can't be copied,
so this class keeps a pointer to TSync it was created from (tsync) and
one must take care the latter does not disappear prematurely.

This class intentionally has no %TryMono() - its usage
means that if it returns "false", the function which called it 
returns (with failure); and thus would destroy "TAutoSync"
and so release the critical section, which is not what we need.

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method creates a (local) object for given critical section object.
/// The latter is unchanged and not copied (just a pointer is copied)
/// so one must take care that it does not disappear prematurely.
/// The calls Multi() and Mono() are forwarded to that object;
/// so in a sense created is a "delegate".
///
/// @param[in] tsync - A pointer to a critical section object;
/// debug version asserts that it is not NULL.
TAutoSync::TAutoSync(TSync *tsync) : sync(tsync)
  {
  Assert(sync != NULL);
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// Leaves the critical section. Does not delete the critical section
/// object it refers to (except for switching it to Multi()).
TAutoSync::~TAutoSync()
  {
  Multi();
  sync = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Enter the critical section blocking the calling thread if it is occupied.
///
/// @attention Be careful while using this method with main
/// thread of an application because blocking it may block child
/// threads so they never leave critical section and the
/// application gets frozen forever.
void TAutoSync::Mono()
  {
  if (sync != NULL)
    sync->Mono();
  }

//////////////////////////////////////////////////////////////////////////////
/// Leave the critical section.
void TAutoSync::Multi()
  {
  if (sync != NULL)
    sync->Multi();
  }

/**
@class TEvent base/tsync.hpp

Synchronization of threads using an event.

A TEvent is a synchronization object having two possible states:
@b signaled and @b non-signaled. The initial state of TEvent is 
non-signaled.
The state is changed to signaled when some thread calls the Signal()
method on this TEvent object. Signal() does not block execution
of the current thread.

A thread can call the Wait() method of the event.
If the event state is signaled, the thread is not blocked.
Otherwise, execution of the thread is blocked until the state of
the event becomes signaled. In either case the state of the event
is turned to non-signaled.

If there are several process waiting for the same TEvent, only one
of them (arbitrary) is unblocked per one Signal() call.

The class keeps a Windows event handle in its representation.

Example:

@code
  // Initialization code
  TEvent event;

  .....

  // Executed by one thread to be sure that some conditions are satisfied
  // The thread may be blocked here.
  event.Wait();

  .....

  // Executed by other thread when the conditions are justified.
  // This unblocks the first thread.
  event.Signal();
@endcode

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
TEvent::TEvent()
  {
#ifdef _WIN32
  event = CreateEvent(NULL, false, false, NULL);
  Assert(event != NULL);
#else
  event = new sem_t;
  Assert(event != NULL);
  int res = sem_init((sem_t *)event, 0, 0);
  Assert(res == 0);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
TEvent::~TEvent()
  {
#ifdef _WIN32
  if (CloseHandle(event) == 0)
    Assert(false);
#else
  if (sem_destroy((sem_t *)event) != 0)
    Assert(false);
  delete (sem_t *)event;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Enter the critical section.
void TEvent::Signal()
  {
#ifdef _WIN32
  if (SetEvent(event) == 0)
    Assert(false);
#else
  int val;
  sem_getvalue((sem_t *)event, &val);
  if (val < 1)
    if (sem_post((sem_t *)event) != 0)
      Assert(false);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Leave the critical section.
void TEvent::Wait()
  {
#ifdef _WIN32
  const int time_out = 1 * 1000;  // in milliseconds
  WaitForSingleObject(event, time_out);
#else
  sem_wait((sem_t *)event);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor is unfeasible for handles.
///
/// This operator does nothing and causes assert violation in debug version.
/// @param[in] src Source object.
TEvent::TEvent(const TEvent &src) { Assert(false); }


/**
@class TEventSet base/tsync.hpp


Set of events that can be awaited together.

The class allows collecting a set of events that can be awaited together.

The class does not create or destroy events but uses externally supplied
ones (in the form of TEvent or Windows handler).
Externally supplied events are assumed to have the auto-reset status.

The class keeps an array of event handlers in their representation.

@sa @ref base_mainpage
**/


//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src Source object.
TEventSet::TEventSet(const TEventSet &src)
  {
  events = src.events;
  }

//////////////////////////////////////////////////////////////////////////////
/// Add an event.
/// @param[in] event - An event to add.
/// @return Normally SUCCESS. FAILURE is returned on lack of memory.
/// The end user is notified about the failure via the User class.
OKAY TEventSet::Add(TEvent &event)
  {
  return events.Add(event.event);
  }

//////////////////////////////////////////////////////////////////////////////
/// Add an event using the Windows event handler.
/// @param[in] event - An event to add to the set.
/// @return Normally SUCCESS. FAILURE is returned on lack of memory.
/// The end user is notified about the failure via User class.
OKAY TEventSet::Add(void *event)
  {
  Assert(event != NULL);
  if (events.Add(event) != SUCCESS)
    {
    User()->ErrorNoMemory();
    return FAILURE;
    }
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove an event.
///
/// The method removes an event from the set.
/// It is assumed that the event is present in the set.
///
/// Removing an event does not destroy the event.
/// The event should be destroyed externally by the TEvent destructor.
///
/// If some event is to be destroyed, it must be first removed
/// from all TEventSet objects it was included to.
///
/// A TEventSet object may be destroyed even if some events were
/// not removed from it yet.
/// @param[in] event - An event to remove from the set.
void TEventSet::Remove(TEvent &event)
  {
  Remove(event.event);
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove an event using the Windows event handler.
///
/// The method removes an event from the set.
/// It is assumed that the event is present in the set.
///
/// Removing an event does not destroy the event.
/// The event should be destroyed externally by an explicit Windows call.
///
/// If some event is to be destroyed, it must be first removed
/// from all TEventSet objects it was included to.
///
/// A TEventSet object may be destroyed even if some events were
/// not removed from it yet.
/// @param[in] event - An event to remove from the set.
void TEventSet::Remove(void *event)
  {
  int index = 0;
  if (!events.Find(event, &index))
    {
    Assert(false);  // event not found
    return;
    }
  events.Remove(index);
  }

//////////////////////////////////////////////////////////////////////////////
/// Wait for any event in the set.
///
/// The method waits until any of the events in the set is signaled.
///
/// If some event in the set has the signaled state, the current thread
/// is not blocked, otherwise the thread is blocked until at least one
/// event in the set becomes signaled. The state of one of such events
/// is turned to non-signaled, and the thread is unblocked.
void TEventSet::Wait()
  {
#ifdef _WIN32
  const int time_limit = 1 * 1000;  // in msecs
  IntWaitForMultipleEvents(events.Length(), events.Data(), false, time_limit);
#else
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 1000;
  for (; ; )
    {
    // loop for all events - check one by one
    for (int i = 0; i < events.Length(); i++)
      if (sem_trywait((sem_t *)(events[i])) == 0)
        return;
    nanosleep(&req, NULL);
    }
#endif
  }

INTEGRA_NAMESPACE_END
