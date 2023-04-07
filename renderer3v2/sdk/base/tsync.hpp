/// @file
///
/// @brief Thread synchronization declarations (system dependent).
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '02.06.20

#ifndef _ELMG_TSYNC_HPP_
#define _ELMG_TSYNC_HPP_

#include "export.h"
#include "garray.hpp"

INTEGRA_NAMESPACE_START

/// @brief Synchronization of threads belonging to the
/// same process via the critical section mechanism.
class TSync
  {
  public:
    /// @name Constructor, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE TSync();
    /// Destructor.
    INTAPI_BASE virtual ~TSync();
    //@}

  public:
    /// @name Interthread synchronization methods
    //@{
    /// Enter the critical section.
    INTAPI_BASE void Mono();
    /// Leave the critical section.
    INTAPI_BASE void Multi();
    /// @brief Try to enter the critical section with
    /// IMMEDIATE return w/o blocking thread.
    INTAPI_BASE bool TryMono();
    //@}

  private:
    /// @name Forbidden methods
    //@{
    /// Assignment operator is unfeasible for handles.
    void operator =(const TSync &src);
    /// Copy constructor is unfeasible for handles.
    TSync(const TSync &src);
    //@}

  private:
    /// Critical section handle (system specific).
    void *handle;
  };  // class TSync

/// Automation of TSync usage.
class TAutoSync
  {
  public:
    /// @name Constructor, destructor
    //@{
    /// Constructor.
    INTAPI_BASE TAutoSync(TSync *tsync);
    /// Destructor.
    INTAPI_BASE virtual ~TAutoSync();
    //@}

  public:
    /// @name Interthread synchronization methods
    //@{
    /// @brief Enter the critical section blocking the calling thread if it is 
    /// occupied.
    INTAPI_BASE void Mono();
    /// Leave the critical section.
    INTAPI_BASE void Multi();
    //@}

  private:
    /// Pointer to the shared critical section.
    TSync *sync;
  };  // class TAutoSync

/// Synchronization of threads using an event.
class TEvent
  {
  public:
    /// @name Constructor, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE TEvent();
    /// Destructor.
    INTAPI_BASE virtual ~TEvent();
    //@}

  public:
    /// @name Interthread synchronization methods
    //@{
    /// Signal an event.
    INTAPI_BASE void Signal();
    /// Wait for the event.
    INTAPI_BASE void Wait();
    //@}

  private:
    /// @name Forbidden methods
    //@{
    /// Copy constructor is unfeasible for handles.
    INTAPI_BASE TEvent(const TEvent &src);
    //@}

  private:
    /// Event handle (system specific).
    void *event;

    /// Friend class.
    friend class TEventSet;
  };  // class TSync


/// A set of events that can be awaited together.
class TEventSet
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline TEventSet();
    /// Copy constructor.
    INTAPI_BASE TEventSet(const TEventSet &src);
    /// Destructor.
    inline virtual ~TEventSet();
    //@}

  public:
    /// @name Events processing
    //@{
    /// Add an event.
    INTAPI_BASE OKAY Add(TEvent &event);
    /// Add an event, using Windows event handler.
    INTAPI_BASE OKAY Add(void *event);
    /// Remove an event.
    INTAPI_BASE void Remove(TEvent &event);
    /// Remove an event, using Windows event handler.
    INTAPI_BASE void Remove(void *event);
    /// Wait for any event in the set.
    INTAPI_BASE void Wait();
    //@}

  private:
    /// A set of event handlers.
    GArray<void *> events;
  };  // class TSync

//////////////////////////////////////////////////////////////////////////////
// Inline methods of TEventSet class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
TEventSet::TEventSet()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
TEventSet::~TEventSet()
  {
  }

INTEGRA_NAMESPACE_END
#endif
