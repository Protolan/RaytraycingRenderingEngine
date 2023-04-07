/// @file
///
/// @brief Interprocess synchronization declarations (system dependent).
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '02.03.27

#ifndef _ELMG_MPSYNC_HPP_
#define _ELMG_MPSYNC_HPP_

#include "export.h"

INTEGRA_NAMESPACE_START

/// @brief Multiprocess synchronization (synchronization of
/// threads potentially belonging to different processes).
class MPSync
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE MPSync();
    /// Copy constructor is unfeasible for handles.
    inline MPSync(const MPSync &src);
    /// Destructor.
    INTAPI_BASE virtual ~MPSync();
    //@}

  public:
    /// @name Interprocess synchronization methods
    //@{
    /// Lock the plug.
    INTAPI_BASE OKAY Lock();
    /// Unlock the plug.
    INTAPI_BASE OKAY Unlock();
    //@}

  private:
    /// Mutex handle (system specific).
    void *handle;
  };  // class MPSync

//////////////////////////////////////////////////////////////////////////////
// Inline methods of MPSync class


//////////////////////////////////////////////////////////////////////////////
/// Copy constructor is unfeasible for handles.
///
/// This operator does nothing and causes assert violation in debug version.
/// @param[in] src Source object.
MPSync::MPSync(const MPSync &src)
  {
  Assert(false);
  }

INTEGRA_NAMESPACE_END
#endif
