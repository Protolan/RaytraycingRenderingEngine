/// @file
///
/// @brief Declaration of BatchUser class.
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '00.12.18

#ifndef _KLXX_BATCHUSER_HPP_
#define _KLXX_BATCHUSER_HPP_

#include <base/file.hpp>
#include <base/time.hpp>
#include <base/user.hpp>

#include "export.h"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/// User object for a batch mode environment.
class BatchUser : public IUser
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE BatchUser();
    /// Destructor.
    INTAPI_BASE virtual ~BatchUser();
    //@}

  protected:
    /// @name Implementation of abstract methods
    //@{
    /// Issue an error message to the user and wait for confirmation.
    INTAPI_BASE virtual void ErrorMsg(const CHAR *msg);
    /// Issue a warning message to the user and wait for confirmation.
    INTAPI_BASE virtual void WarningMsg(const CHAR *msg);
    /// Issue a status information.
    INTAPI_BASE virtual void StatusMsg(const CHAR *msg);
    /// Issue a message that requires reply.
    INTAPI_BASE virtual bool Ask(const CHAR *msg, ReplyStyle rs, bool def);
    INTAPI_BASE virtual int Information(const CHAR *msg, const CHAR *button0text = NULL,
      const CHAR *button1text = NULL, const CHAR *button2text = NULL,
      int def_button = 0, int esc_button = -1);
    /// Start of progress indication.
    INTAPI_BASE virtual void InitProgr(int level);
    /// End of progress indication.
    INTAPI_BASE virtual void TermProgr(int level);
    /// Show progress indication.
    INTAPI_BASE virtual void ShowProgr(double done, const CHAR *msg);
    /// Formatted output to the stdout.
    INTAPI_BASE virtual void PrintMsg(const CHAR *msg);
    /// Check for user break
    INTAPI_BASE virtual bool Break();
    //@}

  }; // class BatchUser

/// Access the default BatchUser object of the given task.
INTAPI_BASE IUser *GetBatchUser();

INTEGRA_NAMESPACE_END

#endif

