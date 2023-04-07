/// @internal
/// @file
///
/// @brief Definition of IUser class.
///
/// @author Pnd - Nickolay Derjabin, '02.01.12
///
/// Copyright &copy; INTEGRA, Inc., 2002-2004

#include <stdarg.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#ifndef DOXYGEN
#define getpid _getpid
#endif
#endif // _WIN32

#include <integra.hpp>

#include "arrays.hpp"
#include "envi.hpp"
#include "file.hpp"
#include "stack.hpp"
#include "threads.hpp"
#include "time.hpp"
#include "tsync.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class IUser base/user.hpp

A stream-level interface to interact with the end user.

This base class is used to organize an UI-independent dialogue with the end user.
Normally GUI application provides a derived class to be used for overall
communication with the user.

The class provides general methods to issue error, warning, status and
log messages. Commonly used messages can be issued by specialized methods,
such as ErrorNoMemory(), InternalError().
Messages of debug purpose can be issued by the global function Debug().

The class provides general methods to issue messages that require reply:
AskOK(), AskOKCancel(), AskYESNo(), AskYesNO().

For progress indication, the methods InitProgress(), ShowProgress() and
TermProgress() should be used. In addition, the Break() method can be used
to query if the user has canceled the operation.

The GetDataPath() method returns path for the folder containing the
application's library data. The SetRootFolder() method sets the name of
the program module.

The methods GetApplVersion() and SetApplVersion() provide access to the
current version of the application, which uses this User object.
This data will be used during saving of IOF files as a text by an
application and can be read by user in some text viewer.

The global function User() is used to get current User object in each
particular application. Particular Graphical User Interface (GUI)
should provide an own implementation of the class. The global function SetUser()
should be used to set a new User object. The default User object always
available for all applications is BatchUser.

@sa @ref base_mainpage

**/

/// IOF signature.
Str IUser::s_iof_sign;
/// Application version.
Str IUser::s_version_str;
/// Path to the common documents Integra folder.
PathStr IUser::s_common_integra;
/// Path to the personal documents Integra folder.
PathStr IUser::s_personal_integra;
/// Path to the common application data Integra folder.
PathStr IUser::s_appdata_integra;
/// Path to common directory for application with given version.
PathStr IUser::s_common_appl_vers;
/// Path to personal directory for application with given version.
PathStr IUser::s_pers_appl_vers;
/// The current path of the application.
PathStr IUser::s_current_path;
/// The path to INI file of the application.
PathStr IUser::s_ini_file;
/// User timer.
static Timer s_user_timer;
/// Log file.
static File s_log;
/// Log sync object.
TSync s_sync;
/// Number of another thread problems to assert.
static int thread_asserts = 1;
/// Initial state of QuickEdit mode. -1: not obtained yet, 0: disabled, 1: enabled.
static int qe_mode = -1;
/// Log message delay for ShowLastProgress().
static const unsigned PROGRESS_LOG_DELAY = 120000U;

#ifdef DEBUG
/// Number of progress indication problems to assert.
static int progr_asserts = 1;
/// Flag to trap missed ShowProgress() calls.
static bool term_progr_flag = false;
#endif

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// @param[in] active_progr_delay Delay for active progress indication.
///
/// If active_progr_delay is not zero, InitProg() is called with some delay.
///
IUser::IUser(unsigned active_progr_delay)
  {
  m_progr_count = 0;
  m_active_progr_count = 0;
  m_active_progr_delay = active_progr_delay;
  m_script_count = 0;
  m_progr_quiet = false;
  break_flag = false;
  m_exit_flag = false;
  m_multi_progr = false;
  m_stopable = false;
  m_prev_progr_msg = "";
  m_postpone_warn = false;
  m_postponed_warn = new TArray<UStr>;
  user_progr_thread_id = IntGetCurrentThreadId();
  catia_mode = false;
  last_progr_value = last_shown_progr_value = .0;
  }  // Constructor

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
IUser::~IUser()
  {
  delete m_postponed_warn;
  SwitchLogFileOff();
  }  //  Destructor

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor (forbidden).
///
/// @param[in] src Source object.
///
IUser::IUser(const IUser &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue an error message to the user and wait for confirmation.
///
/// The method issues an ERROR message to the user (may include confirmation from the user).
/// Message is issued also to log.
////
/// A typical GUI opens a message box with the error message and the OK button.
///
/// @note Instead of explicit call of this method use macro
/// ErrorMessage(Tr("..."), ...) - it is automatically expanded
/// to this method with appropriate parameters conversion to ensure
/// only English messages in log file. Usage of one of translation functions
/// (Tr(), FTr(), tr(), ftr()) is obligatory in first argument.
///
/// @param[in] format_notr Not translated format string.
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
/// @return FAILURE always (for convenience).
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
/// @note To output formatted message, virtual ErrorMsg() method is used.
///
OKAY IUser::ErrorMessageEx(const CHAR *format_notr, const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str_notr[Str::MAX_LENGTH + 1], str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str_notr, Str::MAX_LENGTH, (char *)format_notr, arg_list);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str_notr[Str::MAX_LENGTH] = '\0';
  str[Str::MAX_LENGTH] = '\0';
  LogMessage("ERROR: %s", str_notr);
  ErrorMsg(str);
  return FAILURE;
  }  // ErrorMessageEx()

//////////////////////////////////////////////////////////////////////////////
/// Issue a warning message to the user and wait for confirmation.
///
/// The method issues a WARNING message to the user (may include confirmation from the user).
/// Message is issued also to log.
///
/// A typical GUI opens a message box with the warning message and the OK button.
///
/// @note Instead of explicit call of this method use macro
/// WarningMessage(Tr("..."), ...) - it is automatically expanded
/// to this method with appropriate parameters conversion to ensure
/// only English messages in log file. Usage of one of translation functions
/// (Tr(), FTr(), tr(), ftr()) is obligatory in first argument.
///
/// @param[in] format_notr Not translated format string.
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
/// @note To output formatted message, virtual WarningMsg() method is used.
///
void IUser::WarningMessageEx(const CHAR *format_notr, const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str_notr[Str::MAX_LENGTH + 1], str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str_notr, Str::MAX_LENGTH, (char *)format_notr, arg_list);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str_notr[Str::MAX_LENGTH] = '\0';
  str[Str::MAX_LENGTH] = '\0';
  LogMessage("WARNING: %s", str_notr);
  if (!m_postpone_warn)
    WarningMsg(str);
  else
    {
    UStr ustr(str);
    ((TArray<UStr> *)m_postponed_warn)->Add(ustr);
    }
  }  // WarningMessageEx()

//////////////////////////////////////////////////////////////////////////////
/// Postpone warnings or display postponed warnings.
/// @param[in] postpone - If true - warnings are postponed, otherwise displayed immediately, 
/// including accumulated postponed ones.
void IUser::PostponeWarnings(bool postpone)
  {
  if (m_postpone_warn && !postpone)
    {
    TArray<UStr> &postponed_warnings = *((TArray<UStr> *)m_postponed_warn);
    for (int i = 0; i < postponed_warnings.Length(); i++)
      WarningMsg(postponed_warnings[i]);
    postponed_warnings.Resize();  
    }
  m_postpone_warn = postpone;
  }


//////////////////////////////////////////////////////////////////////////////
/// Issue the status information.
///
/// The method issues a STATUS message to the user (without confirmation from the user).
/// Message is issued also to log.
///
/// A typical GUI writes the message to the status field of the main application window.
/// The empty format parameter (the default) cleans the status message.
///
/// A typical GUI writes the message to the status field of the main application window.
/// The empty format parameter (the default) cleans the status message.
///
/// @note Instead of explicit call of this method use macro
/// StatusMessage(Tr("..."), ...) - it is automatically expanded
/// to this method with appropriate parameters conversion to ensure
/// only English messages in log file. Usage of one of translation functions
/// (Tr(), FTr(), tr(), ftr()) is obligatory in first argument.
///
/// @param[in] format_notr Not translated format string.
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
/// @note To output formatted message, virtual StatusMsg() method is used.
///
void IUser::StatusMessageEx(const CHAR *format_notr, const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str_notr[Str::MAX_LENGTH + 1], str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str_notr, Str::MAX_LENGTH, (char *)format_notr, arg_list);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str_notr[Str::MAX_LENGTH] = '\0';
  str[Str::MAX_LENGTH] = '\0';
  LogMsg(str_notr);
  StatusMsg(str);
  }  // StatusMessageEx()

//////////////////////////////////////////////////////////////////////////////
/// Clear status message area.
///
void IUser::ClearStatusMessage()
  {
  StatusMsg("");
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue a message to the log.
///
/// The method issues a LOG message.
///
/// GUI could write the message to the Message Log window.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
/// @note To output formatted message, virtual LogMsg() method is used.
///
void IUser::LogMessage(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  LogMsg(str);
  }  // LogMessage()

//////////////////////////////////////////////////////////////////////////////
/// Issue a message to the stdout.
///
/// It is ordinary printf function in common case.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
/// @note To output formatted message, virtual PrintMsg() method is used.
///
void IUser::Printf(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  PrintMsg(str);
  }  // Printf()

///////////////////////////////////////////////////////////////////////////////
/// Notify specified action.
///
/// @param object Entity object for the exec action.
/// @param name Name of the action.
///
void IUser::NotifyAction(Entity *object, const Str &name)
  {
  }  // NotifyAction()

///////////////////////////////////////////////////////////////////////////////
/// Set sign of the thread controls usage.
///
/// @param val true if controls for the secondary thread must be used.
///
void IUser::UseThreadControls(bool val)
  {
  }  // UseThreadControls()

//////////////////////////////////////////////////////////////////////////////
/// Issue a message that requires a confirmation.
///
/// A typical GUI will open a message box with the message and the OK button.
///
/// @param[in] format ormat string and possibly values to substitute to it, like in printf().
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
///
void IUser::AskOK(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  Ask(str, RS_OK, true);
  }  // AskOK()

//////////////////////////////////////////////////////////////////////////////
/// Issues a message that requires a reply Ok or Cancel.
///
/// The default reply is OK. A typical GUI will open a message box with the
/// message and the OK (default) and CANCEL buttons.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
/// @return @b true if OK was selected; @b false if CANCEL was selected.
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
///
bool IUser::AskOKCancel(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  return Ask(str, RS_OKCANCEL, true);
  }  // AskOKCancel()

//////////////////////////////////////////////////////////////////////////////
/// Issues a message that requires a reply Yes or No.
///
/// The default reply is YES. A typical GUI will open a message box with the
/// message and the YES (default) and NO buttons.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
/// @return @b true if YES was selected; @b false if NO was selected.
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
///
bool IUser::AskYESNo(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  return Ask(str, RS_YESNO, true);
  }  // AskYESNo()

//////////////////////////////////////////////////////////////////////////////
/// Issues a message that requires a reply Yes or No.
///
/// The default reply is NO. A typical GUI will open a message box with the
/// message and the YES and NO (default) buttons.
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
/// @return @b true if YES was selected; @b false if NO was selected.
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
///
bool IUser::AskYesNO(const CHAR *format, ...)
  {
  // Format the message
  Assert(format != NULL);
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Use a virtual method
  return Ask(str, RS_YESNO, false);
  }  // AskYesNO()

//////////////////////////////////////////////////////////////////////////////
/// Issue a message with custom replies.
///
/// Method issues an informational message that requires reply,
/// with up to 3 variants of answer.
/// @param[in] msg The message.
/// @param[in] button0text Text of the first button.
/// @param[in] button1text Text of the second button.
/// @param[in] button2text Text of the third button.
/// @param def_button Default button number, when Return or Enter is pressed.
/// @param esc_button Escape button number, when Esc is pressed.
/// @return Index of the pressed button.
///
/// @note Default implementation - Is not implemented (for ITBT and Catia).
int IUser::Information(const CHAR *msg, const CHAR *button0text,
                 const CHAR *button1text, const CHAR *button2text,
                 int def_button, int esc_button)
  {
  int ret = def_button;
  return ret;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the file name using Open File dialog.
/// @param[in] pathname Initial path to look in.
/// @param[in] sel_dlg_caption File dialog caption.
/// @param[in] filter_list List of file filters (only extensions - for CATIA standard).
/// @param[in] filter_names_list List of file filter descriptions (should be with extensions too - for Qt standard).
/// @param[in, out] selected_filter At input - initially selected filter index, at output - last selected index in dialog.
/// @return Selected file name.
PathStr IUser::SelectOpenPathName(const PathStr &pathname, 
      const UStr &sel_dlg_caption, const TArray<UStr> &filter_list, const TArray<UStr> &filter_names_list,
      int &selected_filter)
  {
  Assert(false); // Implement it for user, for which it is called. Otherwise - default path is returned.
  return pathname;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the file name using Save File dialog.
/// @param[in] pathname Initial path to look in.
/// @param[in] sel_dlg_caption File dialog caption.
/// @param[in] filter_list List of file filters (only extensions - for CATIA standard).
/// @param[in] filter_names_list List of file filter descriptions (should be with extensions too - for Qt standard).
/// @param[in, out] selected_filter At input - initially selected filter index, at output - last selected index in dialog.
/// @return Selected file name.
PathStr IUser::SelectSavePathName(const PathStr &pathname, 
      const UStr &sel_dlg_caption, const TArray<UStr> &filter_list, const TArray<UStr> &filter_names_list,
      int &selected_filter)
  {
  Assert(false); // Implement it for user, for which it is called. Otherwise - default path is returned.
  return pathname;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the folder name using folder selection dialog.
/// @param[in] pathname Initial path to look in.
/// @param[in] sel_dlg_caption File dialog caption.
/// @return Selected folder name.
PathStr IUser::SelectFolderPathName(const PathStr &pathname, const UStr &sel_dlg_caption)
  {
  Assert(false); // Implement it for user, for which it is called. Otherwise - default path is returned.
  return pathname;
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue a "No memory" message.
///
/// The method issues a "No memory" error message.
/// Message is issued also to log.
///
/// @param[in] amount - The size of memory, for which allocation failed.
/// @return FAILURE always (for convenience).
///
OKAY IUser::ErrorNoMemory(INT64 amount)
  {
  // Prevent for reentrance for sure
  static bool entered = false;
  if (entered)
    return FAILURE;
  entered = true;
  if (amount == -1)
    {
    LogMessage("MEMORY ERROR: Out of virtual memory");
    ErrorMsg(Tr("Out of virtual memory"));
    }
  else
    {
    CHAR str[Str::MAX_LENGTH + 1];
    sprintf(str, Tr("Out of virtual memory, couldn't allocate %I64d bytes"), amount);
    LogMessage("MEMORY ERROR: Out of virtual memory, couldn't allocate %I64d bytes", amount);
    ErrorMsg(str);
    }
  Assert(false);  // Notify for DEBUG
  entered = false;
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Internal error message.
///
/// The method issues a message about an internal application error.
/// Message is issued also to log.
///
/// An internal application error is a notification of the application
/// failure due to internal logical reasons (likely bugs).
/// The method displays a message like "Internal application error -
/// please contact @em company_name" accompanied by the description provided by
/// the method's arguments.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
/// @return FAILURE always (for convenience).
///
/// @note Formatted line must fit into the Str::MAX_LENGTH size.
///
OKAY IUser::InternalError(const CHAR *format, ...)
  {
  // NULL format will result in crash
  if (format == NULL)
    {
    Assert(false);
    format = "";
    }
  // Format the message
  CHAR str[Str::MAX_LENGTH + 1];
  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  // Add common part
  UStr str2(Tr("Internal application error - please contact Integra"));
  str2 += "\n";
  str2 += str;
  LogMessage("INTERNAL ERROR: %s", str);
  // Use a virtual method
  ErrorMsg(str2);
  Assert(false);   // Notify internal application error for DEBUG
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start progress indication.
///
/// The method indicates start of progress indication. It should be used
/// when the application starts long, time consuming activity that may be
/// optionally interrupted by the user.
///
/// Application must call TermProgress() to close the progress indication.
/// Between InitProgress() and TermProgress(), the application may call
/// ShowProgress() and Break().
///
/// Nested InitProgress() and TermProgress() are allowed.
///
/// User break is reset by the most outer InitProgress() and TermProgress() calls.
///
/// Special busy mode is intended to show busy indication only instead of progress in percentages.
/// It is useful when progress is not known in advance.
///
/// This method calls virtual InitProg() method that must be reimplemented
/// by derived classes (for example, to show progress bar).
/// This InitProgress() method itself should not be re-implemented.
///
/// @param[in] stopable Is operation under progress stopable? Is considered only 
///   for the top progress level.
/// @param[in] busy_mode Busy mode flag.
///
void IUser::InitProgress(bool stopable, bool busy_mode)
  {
  if (m_progr_quiet)
    return;
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id)
    return;
 
    // Update progress information
  int level = m_progr_count++;
  if (level >= MAX_PROGRESS_LEVEL)
    return;
  ProgrInfo &prog_info = m_progr_info[level];
  prog_info.p_left = prog_info.p_right = 0.0;
  prog_info.p_last_msg = NULL;
  prog_info.p_busy_mode = busy_mode;
  prog_info.p_next_time = s_user_timer.Elapsed() + m_active_progr_delay;
  prog_info.p_next_log_time = s_user_timer.Elapsed();
  // Top level initializations
  if (level == 0)
    {
    // Break is reset by the most outer progress
    break_flag = false;
    m_stopable = stopable;
    }
#ifdef DEBUG
  else
    {
    if (term_progr_flag)
      {
      // This assertion indicates problem with progress indication:
      // In the sequence of progress indication calls
      //
      //   InitProgress()
      //   .....
      //     InitProgress()
      //     .....
      //     TermProgress()
      //     InitProgress() <- Assert
      ///
      // ShowProgress() call is required at least before the last (asserted) InitProgress().
      // Absence of ShowProgress() causes jump back in progress indication.
      //Assert(progr_asserts-- <= 0); // This is design failure - a lot of such sequences can be called during preprocessing.
      term_progr_flag = false;
      }
    }
#endif
  if (m_active_progr_delay <= 0U)
    {
    int active_level = m_active_progr_count++;
    Assert(level == active_level);
    InitProgr(active_level);
    }
  }  // InitProgress()

//////////////////////////////////////////////////////////////////////////////
/// Show progress indication.
///
/// The method updates progress indication.
///
/// A typical GUI shows the activity progress as a graphic bar,
/// and renews the message in the status bar by the new one.
///
/// The method must be used between InitProgress() and TermProgress() calls.
/// Optimally, the method should be called about 10 times per second on
/// modest computer (in release configuration).
///
/// Value passed to method, must not be current progress value;
/// it must be progress value supposed to be reached by next
/// ShowProgress() call, that is, some forecast. This is necessary for correct
/// processing of possible nested progresses. Keep in mind that top level
/// process which initiated progress indication, may be not aware of
/// any nested processes (even if they are not exist now,
/// they may appear in the future). IUser provides correct scaling
/// of any number of nested processes with any depth provided each
/// process which initiated progress, follows above condition.
///
/// Each process performs progress in bounds from 0.0 to 1.0. If it
/// happens some progress is actually a subprogress of some outer progress,
/// IUser automatically scales nested subprogress' region into
/// some part of topmost progress region.
///
/// @param[in] to_be_done Progress value forecast.
///   For "ordinary" mode, it should be in range [0.0, 1.0].
///   For convenience values greater than 1.0 are allowed and treated as 1.0.
///   For busy indication mode the value is ignored (set it to 0).
/// @param[in] format Format string and possibly values to substitute to it,
/// the same as in printf(). If this parameter is NULL (not recommended!!!),
/// string from previous call of this method will be used.
///
/// Derived classes can customize progress output be re-implementing
/// virtual InitProgr(), ShowProgr(), TermProgr() methods.
///
/// @note Formatted line must fit into Str::MAX_LENGTH size.
///
void IUser::ShowProgress(double to_be_done, const CHAR *format, ...)
  {
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id)
    return;
  
  if (m_progr_quiet)
    return;
  Assert(m_progr_count > 0);  // i.e. InProgress()
  if (m_progr_count <= 0)
    return;
  Assert(to_be_done >= 0);
  int current_level = m_progr_count - 1;
  if (current_level >= MAX_PROGRESS_LEVEL)
    return;
#ifdef DEBUG
  term_progr_flag = false;
#endif
  // Update progress message for current progress level
  ProgrInfo &current_prog_info = m_progr_info[current_level];
  current_prog_info.p_last_msg = NULL;
  if (format != NULL)
    {
    CHAR str[Str::MAX_LENGTH + 1];
    va_list arg_list;
    va_start(arg_list, format);
    _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
    va_end(arg_list);
    str[Str::MAX_LENGTH] = '\0';
    current_prog_info.p_last_msg = str;
    }
#if 1
  else
    Assert(false); // Provide some message!!!
#endif
  // Update progress value for current progress level
  if (!current_prog_info.p_busy_mode)
    {
    // "Ordinary" progress values are in [0..100] range
    if (to_be_done > 1.0)
      to_be_done = 1.0;
    current_prog_info.p_left = current_prog_info.p_right;
    current_prog_info.p_right = to_be_done * 100.0;
#ifdef DEBUG
    if ((current_prog_info.p_right <= current_prog_info.p_left) && (to_be_done < 1.0) && (to_be_done > 0.0))
      {
      // This assertion (where p_right is typically equal to p_left)
      // indicates problem with ShowProgress():
      // no room has been reserved for following actions.
      // Second condition makes an exception allowing for the progress to reach 100%
      Assert(progr_asserts-- <= 0);
      }
#endif
    }
  // Get current time
  unsigned cur_time = s_user_timer.Elapsed();
  // Activate next progress level eventually
  if (m_progr_count > m_active_progr_count)
    {
    ProgrInfo &next_progr = m_progr_info[m_active_progr_count];
    // Next progress level is activated with some delay (if activated at all)
    if (cur_time >= next_progr.p_next_time)
      {
      int active_level = m_active_progr_count++;
      InitProgr(active_level);
      // Correct time for next progress level activation
      if (current_level > active_level)
        m_progr_info[active_level + 1].p_next_time = cur_time + m_active_progr_delay;
      // Correct time for previous progress level continuation
      if (active_level > 0)
        m_progr_info[active_level - 1].p_next_time = cur_time + m_active_progr_delay;
      }
    }
  // Check if any progress was activated
  if (m_active_progr_count <= 0)
    return;
  int active_level = m_active_progr_count - 1;
  Assert(active_level <= current_level);
  // Prevent progress indication for outer progress,
  // if not activated inner progress exists
  if (active_level < current_level)
    return;
  // Don't show progress indication too often
  ProgrInfo &active_progr_info = m_progr_info[active_level];
  if (cur_time < active_progr_info.p_next_time)
    return;
  active_progr_info.p_next_time = cur_time + 200U;
  // Get active progress message
  const CHAR *message = m_progr_info[active_level].p_last_msg;
  if (message == NULL)
    {
    for (int level = current_level; level >= 0; level--)
      {
      message = m_progr_info[level].p_last_msg;
      if (message != NULL)
        break;
      }
    }
  if (message == NULL)
    {
    // No progress message is available
    // Assert(progr_asserts-- <= 0); // Such ShowProgress calls without message are added by Pnd to Kernel::Render
    message = INTEGRA_T("");
    }
  // Get active progress value
  double done = 0.0;
  for (int level = current_level; level >= 0; level--)
    {
    ProgrInfo &progress = m_progr_info[level];
    if (progress.p_busy_mode)
      {
      if (active_level > level)
        {
        // Don't count upper progresses
        break;
        }
      if (active_level == level)
        {
        // Advance progress for active busy mode (successive negative integral values are used)
        progress.p_left -= 1.0;
        done = progress.p_left;
        break;
        }
      // Don't count lower progresses
      done = 0.0;
      }
    done = progress.p_left + (done / 100.0) * (progress.p_right - progress.p_left);
    Assert((0.0 <= done) && (done <= 100.0));
    }
  // Call virtual show progress method
  ShowProgr(done, message);
  last_progr_value = done;
  last_progr_msg = message;
  }  // ShowProgress()

//////////////////////////////////////////////////////////////////////////////
/// End progress indication.
///
/// The method indicates end of progress indication started by
/// most recent InitProgress().
///
/// @note Any implementation of the method must call base IUser::TermProgress()
/// method, which performs necessary internal terminations.
void IUser::TermProgress()
  {
  if (m_progr_quiet)
    return;
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id)
    return;
  
  Assert(m_progr_count > 0);
  if (m_progr_count <= 0)
    return;
  // TermProgr() now is called at each level with real progress
  int level = --m_progr_count;
  if (level >= MAX_PROGRESS_LEVEL)
    return;
  if (m_active_progr_count > m_progr_count)
    TermProgr(--m_active_progr_count);
  Assert(m_active_progr_count <= m_progr_count);
  // Top level terminations
  if (level == 0)
    {
    // Reset break flag
    break_flag = false;
    m_stopable = false;  // for sure
    }
#ifdef DEBUG
  term_progr_flag = (level > 0) ? true : false;
#endif
  }  // TermProgress()

//////////////////////////////////////////////////////////////////////////////
/// Start progress indication.
/// It is much the same as InitProgress(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See InitProgress() for description.
///
/// @param[in] stopable - whether or not calculations inside the current block
///                       InitProgressMT()/TermProgressMT() can be interrupted 
/// @param[in] busy_mode - if 'true', shown will be just the busy indicator. 
///                        Intended to use when the progress is not known in advance
void IUser::InitProgressMT(bool stopable, bool busy_mode)
  {
  if (m_progr_quiet)
    return;
  void *current_id = IntGetCurrentThreadId();

  if (m_progr_count == 0)
    {
    user_progr_thread_id_mt = current_id;
    }
  else if (current_id != user_progr_thread_id_mt)
    {
    if (thread_asserts-- > 0)
      {
      Assert(false);
      LogMessage("InitProgress() called from another thread");
      }
    return;
    }
  // Update progress information
  int level = m_progr_count++;
  if (level >= MAX_PROGRESS_LEVEL)
    return;
  ProgrInfo &prog_info = m_progr_info[level];
  prog_info.p_left = prog_info.p_right = 0.0;
  prog_info.p_last_msg = NULL;
  prog_info.p_busy_mode = busy_mode;
  prog_info.p_next_time = s_user_timer.Elapsed() + m_active_progr_delay;
  // Top level initializations
  if (level == 0)
    {
    // Break is reset by the most outer progress
    break_flag = false;
    m_stopable = stopable;
    }
#ifdef DEBUG
  else
    {
    if (term_progr_flag)
      {
      // This assertion indicates problem with progress indication:
      // In the sequence of progress indication calls
      //
      //   InitProgress()
      //   .....
      //     InitProgress()
      //     .....
      //     TermProgress()
      //     InitProgress() <- Assert
      ///
      // ShowProgress() call is required at least before the last (asserted) InitProgress().
      // Absence of ShowProgress() causes jump back in progress indication.
      //Assert(progr_asserts-- <= 0); // This is design failure - a lot of such sequences can be called during preprocessing.
      term_progr_flag = false;
      }
    }
#endif
  if (m_active_progr_delay <= 0U)
    {
    int active_level = m_active_progr_count++;
    Assert(level == active_level);
    InitProgr(active_level);
    }
  }  // InitProgress()

//////////////////////////////////////////////////////////////////////////////
/// Show progress indication.
/// It is much the same as ShowProgress(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See ShowProgress() for description.
///
/// @param[in] to_be_done Progress value forecast.
///   For "ordinary" mode, it should be in range [0.0, 1.0].
///   For convenience values greater than 1.0 are allowed and treated as 1.0.
///   For busy indication mode the value is ignored (set it to 0).
///
/// @param[in] format Format string and possibly values to substitute to it,
/// the same as in printf(). If this parameter is NULL (not recommended!!!),
/// string from previous call of this method will be used.
///
/// @note Formatted line must fit into Str::MAX_LENGTH size.
void IUser::ShowProgressMT(double to_be_done, const CHAR *format, ...)
  {
#if 0 // HV-43 Ose
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id_mt)
    {
    if (thread_asserts-- > 0)
      {
      Assert(false);
      LogMessage("ShowProgress() called from another thread");
      }
    return;
    }
#endif

  if (m_progr_quiet)
    return;
  Assert(m_progr_count > 0);  // i.e. InProgress()
  if (m_progr_count <= 0)
    return;
  Assert(to_be_done >= 0);
  int current_level = m_progr_count - 1;
  if (current_level >= MAX_PROGRESS_LEVEL)
    return;
#ifdef DEBUG
  term_progr_flag = false;
#endif
  // Update progress message for current progress level
  ProgrInfo &current_prog_info = m_progr_info[current_level];
  current_prog_info.p_last_msg = NULL;
  if (format != NULL)
    {
    CHAR str[Str::MAX_LENGTH + 1];
    va_list arg_list;
    va_start(arg_list, format);
    _vsnprintf((char *)str, Str::MAX_LENGTH, (char *)format, arg_list);
    va_end(arg_list);
    str[Str::MAX_LENGTH] = '\0';
    current_prog_info.p_last_msg = str;
    }
#if 1
  else
    Assert(false); // Provide some message!!!
#endif
  // Update progress value for current progress level
  if (!current_prog_info.p_busy_mode)
    {
    // "Ordinary" progress values are in [0..100] range
    if (to_be_done > 1.0)
      to_be_done = 1.0;
    current_prog_info.p_left = current_prog_info.p_right;
    current_prog_info.p_right = to_be_done * 100.0;
#ifdef DEBUG
    if ((current_prog_info.p_right <= current_prog_info.p_left) && (to_be_done < 1.0) && (to_be_done > 0.0))
      {
      // This assertion (where p_right is typically equal to p_left)
      // indicates problem with ShowProgress():
      // no room has been reserved for following actions.
      // Second condition makes an exception allowing for the progress to reach 100%
      Assert(progr_asserts-- <= 0);
      }
#endif
    }

  // Get current time
  unsigned cur_time = s_user_timer.Elapsed();
  // Activate next progress level eventually
  if (m_progr_count > m_active_progr_count)
    {
    ProgrInfo &next_progr = m_progr_info[m_active_progr_count];
    // Next progress level is activated with some delay (if activated at all)
    if (cur_time >= next_progr.p_next_time)
      {
      int active_level = m_active_progr_count++;
#if 0 // HV-43 Ose
      InitProgr(active_level);
#endif
      // Correct time for next progress level activation
      if (current_level > active_level)
        m_progr_info[active_level + 1].p_next_time = cur_time + m_active_progr_delay;
      // Correct time for previous progress level continuation
      if (active_level > 0)
        m_progr_info[active_level - 1].p_next_time = cur_time + m_active_progr_delay;
      }
    }
  // Check if any progress was activated
  if (m_active_progr_count <= 0)
    return;
  int active_level = m_active_progr_count - 1;
  Assert(active_level <= current_level);
  // Prevent progress indication for outer progress,
  // if not activated inner progress exists
  if (active_level < current_level)
    return;
  // Don't show progress indication too often
  ProgrInfo &active_progr_info = m_progr_info[active_level];
  if (cur_time < active_progr_info.p_next_time)
    return;
  active_progr_info.p_next_time = cur_time + 200U;
  // Get active progress message
  const CHAR *message = m_progr_info[active_level].p_last_msg;
  if (message == NULL)
    {
    for (int level = current_level; level >= 0; level--)
      {
      message = m_progr_info[level].p_last_msg;
      if (message != NULL)
        break;
      }
    }
  if (message == NULL)
    {
    // No progress message is available
    // Assert(progr_asserts-- <= 0); // Such ShowProgress calls without message are added by Pnd to Kernel::Render
    message = INTEGRA_T("");
    }
  // Get active progress value
  double done = 0.0;
  for (int level = current_level; level >= 0; level--)
    {
    ProgrInfo &progress = m_progr_info[level];
    if (progress.p_busy_mode)
      {
      if (active_level > level)
        {
        // Don't count upper progresses
        break;
        }
      if (active_level == level)
        {
        // Advance progress for active busy mode (successive negative integral values are used)
        progress.p_left -= 1.0;
        done = progress.p_left;
        break;
        }
      // Don't count lower progresses
      done = 0.0;
      }
    done = progress.p_left + (done / 100.0) * (progress.p_right - progress.p_left);
    Assert((0.0 <= done) && (done <= 100.0));
    }
  
  last_progr_value = done;
  last_progr_msg = message;
  }  // ShowProgress()

//////////////////////////////////////////////////////////////////////////////
/// End progress indication.
///
/// It is much the same as TermProgress(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See TermProgress() for description.
void IUser::TermProgressMT()
{
  if (m_progr_quiet)
    return;
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id_mt)
    {
    if (thread_asserts-- > 0)
      {
      Assert(false);
      LogMessage("TermProgress() called from another thread");
      }
    return;
    }

  Assert(m_progr_count > 0);
  if (m_progr_count <= 0)
    return;
  // TermProgr() now is called at each level with real progress
  int level = --m_progr_count;
  if (level >= MAX_PROGRESS_LEVEL)
    return;
  if (m_active_progr_count > m_progr_count)
    {
    m_active_progr_count--;
//    TermProgr(m_active_progr_count);
    }
  Assert(m_active_progr_count <= m_progr_count);
  // Top level terminations
  if (level == 0)
    {
    // Reset break flag
    break_flag = false;
    m_stopable = false;  // for sure
    }
#ifdef DEBUG
  term_progr_flag = (level > 0) ? true : false;
#endif
}  // TermProgress()

//////////////////////////////////////////////////////////////////////////////
/// Get the progress status.
///
/// @return @b true if any InitProgress() is in effect.
/// Note: InitProgr() may be not called!
///
bool IUser::InProgress() const
  {
  return m_progr_count > 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Show progress from the last saved value.
///
/// This 'saved value' is the one passed to the last ShowProgress() or ShowProgressMT();
/// It is remembered there and can be shown again by this method.
/// @param[in] str Default log message value which is used if no actual progress data
/// has been prepared up to now.
void IUser::ShowLastProgress(const char *str)
  {
  bool from_another_thread = false;
  
  void *current_id = IntGetCurrentThreadId();
  if (current_id != user_progr_thread_id)
    {
    from_another_thread = true;
    }
  int current_level = m_progr_count - 1;
  if (current_level >= MAX_PROGRESS_LEVEL)
    return;
  // Get current time
  unsigned cur_time = s_user_timer.Elapsed();
  // Check if any progress was activated
  if (m_active_progr_count <= 0)
    {
    if (m_progr_count <= 0)
      return;
    int active_level = m_progr_count - 1;
    ProgrInfo &active_progr_info = m_progr_info[active_level];
    if (cur_time >= active_progr_info.p_next_log_time)
      {
      Str log_str;
      if (str == NULL)
        log_str.Printf("Operation in progress");
      else
        log_str = str;
      LogMessage(log_str);
      active_progr_info.p_next_log_time = cur_time + PROGRESS_LOG_DELAY;
      }
    return;
    }
  int active_level = m_active_progr_count - 1;
  Assert(active_level <= current_level);
  // Prevent progress indication for outer progress,
  // if not activated inner progress exists
  if (active_level < current_level)
    return;
  // Don't show progress indication too often
  ProgrInfo &active_progr_info = m_progr_info[active_level];
  if (cur_time < active_progr_info.p_next_time)
    return;
  //active_progr_info.p_next_time = cur_time + 200U;
  if (cur_time >= active_progr_info.p_next_log_time)
    {
    LogMessage("%s %5.1f%% done", last_progr_msg, last_progr_value);
    active_progr_info.p_next_log_time = cur_time + PROGRESS_LOG_DELAY;
    }
  
  if (from_another_thread)
    return;
  if (InProgress() && fabs(last_progr_value - last_shown_progr_value) > 0.01)
    {
    ShowProgr(last_progr_value, last_progr_msg);
    last_shown_progr_value = last_progr_value;
    }

  if (!InProgress())
    TermProgr(0); // To close progress indicator in Qt
  } // ShowLastProgress()

//////////////////////////////////////////////////////////////////////////////
/// Terminate active progress completely and close progress bar.
void IUser::TermLastProgress()
  {
  TermProgr(0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get current user time.
///
/// @return Current user time.
///
unsigned IUser::SessionElapsed()
  {
  return s_user_timer.Elapsed();
  }  // Elapsed()

//////////////////////////////////////////////////////////////////////////////
/// Get progress quiet mode.
///
/// @return Progress quiet mode.
///
bool IUser::GetProgressQuietMode()
  {
  return m_progr_quiet;
  }  // GetProgressQuietMode()

//////////////////////////////////////////////////////////////////////////////
/// Set progress quiet mode.
///
/// @param[in] quiet Progress mode (quet or not).
///
void IUser::SetProgressQuietMode(bool quiet)
  {
//  Assert(m_progr_count == 0);
  m_progr_quiet = quiet;
  }  // SetProgressQuietMode()

//////////////////////////////////////////////////////////////////////////////
/// @fn IUser::InitProgr(int level)
///
/// Handle start of the progress indication.
///
/// The method is called when progress indication (may be nested) is initiated.
///
/// @param[in] level Current progress level - 0 (the top most call), 1, 2, etc.
///

//////////////////////////////////////////////////////////////////////////////
/// @fn IUser::TermProgr(int level)
///
/// Handle end of the progress indication.
///
/// The method is called when progress indication (may be nested) is terminated.
///
/// @param[in] level Current progress level - 0 (the top most call), 1, 2, etc.
///

//////////////////////////////////////////////////////////////////////////////
/// @fn void IUser::ShowProgr(double done, const CHAR *msg)
///
/// Show progress indication.
///
/// Derived classes must implement this method to show the current progress information.
///
/// @param[in] done A value showing the current progress.
///   For ordinary progress, the value is in percentages (0 <= done <= 100).
///   For busy mode, successive negative integral values are passed (-1, -2, etc.)
/// @param[in] msg A progress message to issue (not NULL).
///

//////////////////////////////////////////////////////////////////////////////
/// Get active progress count.
///
/// @return Number of nested active progresses (i.e. number of InitProgr() calls in effect).
///
int IUser::ProgrCount() const
  {
  return m_active_progr_count;
  }  // ProgrCount()

//////////////////////////////////////////////////////////////////////////////
/// Get busy mode indication.
///
/// This method can be used by InitProgr(), TermProg().
/// It indicates which mode is to be used for further progress indication.
///
/// @return @c true if busy mode is to be used; @c false if "ordinary" mode is to be used.
///
/// Busy mode is intended to show just a busy indication instead of progress in percentages.
///
/// This mode is set in InitProgress().
///
bool IUser::BusyMode() const
  {
  Assert(m_active_progr_count >= 0);
  if (m_active_progr_count <= 0)
    return false;
  return m_progr_info[m_active_progr_count - 1].p_busy_mode;
  }  // BusyMode()

//////////////////////////////////////////////////////////////////////////////
/// Get progress message for given level.
///
/// This method can be used by ShowProgr().
/// It is intended for multi-progress controls solely.
///
/// @param[in] level Progress level for which progress message is to be reported.
///   Valid range is <c>0..ProgrCount() - 1</c>.
/// @return Progress message for the level (may be empty but not NULL).
///
const CHAR *IUser::GetProgressMsg(int level) const
  {
  Assert(m_active_progr_count > 0);
  Assert((0 <= level) && (level < m_active_progr_count));
  if ((level < 0) || (level >= m_active_progr_count))
    return INTEGRA_T("");
  const CHAR *message = m_progr_info[level].p_last_msg;
  return (message != NULL) ? message : INTEGRA_T("");
  }  // GetProgressMsg()

//////////////////////////////////////////////////////////////////////////////
/// Get current progress value for given level.
///
/// This method can be used by ShowProgr().
/// It is intended for multi-progress controls solely.
///
/// @param[in] level Progress level for which progress value is to be reported.
///   Valid range is <c>0..ProgrCount() - 1</c>.
/// @return Progress value in question.
///   For ordinary progress, the value is in percents (from 0.0 to 100.0).
///   For busy mode, negative integral values are returned.
///
double IUser::GetProgressVal(int level) const
  {
  Assert(m_active_progr_count > 0);
  Assert((0 <= level) && (level < m_active_progr_count));
  if ((level < 0) || (level >= m_active_progr_count))
    return 0.0;
  return m_progr_info[level].p_left;
  }  // GetProgressVal()

//////////////////////////////////////////////////////////////////////////////
/// Check for user break.
///
/// The method checks if a break was requested by the end user.
///
/// A typical GUI has the CANCEL button for this purpose and / or special key(s).
///
/// The method must be used between the InitProgress() and TermProgress() calls.
/// @return @c true, if a break was requested by the user during the
/// current action (with the progress indication enabled); @c false otherwise.
bool IUser::Break()
  {
  return break_flag || m_exit_flag;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for the user break without calling QT or console functions.
///
/// Function is used where call of QT or console functions is not allowed (non-main threads).
/// Break flag could be set by SetBreak().
/// @return @c true if break flag was set.
bool IUser::IsBreak() const
  {
  return break_flag || m_exit_flag;
  }


//////////////////////////////////////////////////////////////////////////////
/// Set break flag.
void IUser::SetBreak()
  {
  break_flag = true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Reset the break flag.
///
/// This method provide avoiding break of upper level progress,
/// when it is necessary.
INTAPI_BASE void IUser::ResetBreak()
  {
  break_flag = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set exit flag.
void IUser::SetExit()
  {
  m_exit_flag = true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for user break.
///
/// It is much the same as Break(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See Break() for description.
/// @return @c true, if a break was requested by the user during the
/// current action (with the progress indication enabled); @c false otherwise.
bool IUser::BreakMT()
  {
  s_sync.Mono(); // Should it be here?
  bool ret = break_flag || m_exit_flag;
  s_sync.Multi();
  return ret;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set break flag.
///
/// It is much the same as SetBreak(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See SetBreak() for description.
void IUser::SetBreakMT()
  {
  s_sync.Mono();
  break_flag = true;
  s_sync.Multi();
  }

//////////////////////////////////////////////////////////////////////////////
/// Reset the break flag.
///
/// It is much the same as ResetBreak(), but intended for use in the code when
/// the "progressed" are done in one thread, while progress indication and interruption
/// are in another thread. See ResetBreak() for description.
INTAPI_BASE void IUser::ResetBreakMT()
  {
  s_sync.Mono();
  break_flag = false;
  s_sync.Multi();
  }

//////////////////////////////////////////////////////////////////////////////
/// Process UI events, if any.
void IUser::ProcessEvents()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the topmost flag.
void IUser::SetTopmost(bool topmost)
  {
  Assert(false); // not implemented
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the event filter used flag (Temporary!!!).
void IUser::SetEventFilterUsed(bool used)
  {
  Assert(false); // not implemented;
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check - is event filter used.
bool IUser::IsEventFilterUsed()
  {
  return false; // is not implemented.
  }

//////////////////////////////////////////////////////////////////////////////
/// Set CATIA mode.
void IUser::SetCATIAMode(bool mode)
  {
  catia_mode = mode;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get CATIA mode.
bool IUser::GetCATIAMode()
  {
  return catia_mode;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get path to common documents Integra folder.
/// @return Required path.
const PathStr &IUser::GetCommonIntegraPath()
  {
  if (s_common_integra.IsEmpty())
    {
    PathStr common_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_DOCUMENTS, PathStr::SITYPE_CURRENT);
    if (common_path.IsEmpty())
      common_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_DOCUMENTS, PathStr::SITYPE_DEFAULT);
    if (common_path.IsEmpty())
      {
      Assert(false);
      return PathStr::GetRootPath();
      }
    s_common_integra = PathStr(common_path, "Integra");
    }
  return s_common_integra;
  }  // GetCommonIntegraPath()

//////////////////////////////////////////////////////////////////////////////
/// Get path to personal documents Integra folder.
/// @return Required path.
const PathStr &IUser::GetPersIntegraPath()
  {
  if (s_personal_integra.IsEmpty())
    {
    PathStr personal_path = PathStr::GetSIFolder(PathStr::SIFOLDER_PERSONAL, PathStr::SITYPE_CURRENT);
    if (personal_path.IsEmpty())
      personal_path = PathStr::GetSIFolder(PathStr::SIFOLDER_PERSONAL, PathStr::SITYPE_DEFAULT);
    if (personal_path.IsEmpty())
      {
      Assert(false);
      return PathStr::GetRootPath();
      }
    s_personal_integra = PathStr(personal_path, "Integra");
    }
  return s_personal_integra;
  }  // GetPersIntegraPath()

//////////////////////////////////////////////////////////////////////////////
/// Get path to application data Integra folder.
/// @return Required path.
const PathStr &IUser::GetAppdataIntegraPath()
  {
  if (s_appdata_integra.IsEmpty())
    {
    PathStr appdata_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_APPDATA, PathStr::SITYPE_CURRENT);
    if (appdata_path.IsEmpty())
      appdata_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_APPDATA, PathStr::SITYPE_DEFAULT);
    if (appdata_path.IsEmpty())
      {
      Assert(false);
      return PathStr::GetRootPath();
      }
    s_appdata_integra = PathStr(appdata_path, "Integra");
    }
  return s_appdata_integra;
  }  // GetAppdataIntegraPath()

//////////////////////////////////////////////////////////////////////////////
/// Get path to common directory for application with given version.
/// @param[in] version Application name and version to use. If NULL,
///                    value set by SetApplVersion() will be used.
/// @return Required path.
const PathStr IUser::GetCommonApplVersPath(const char *version)
  {
  if (version == NULL && !s_common_appl_vers.IsEmpty())
      return s_common_appl_vers;

  if (version != NULL)
    {
    GetCommonIntegraPath();
    if (!s_common_integra.IsEmpty())
      return PathStr(s_common_integra, version);
    }

  return PathStr::GetRootPath();
  }  // GetCommonApplVersPath()

//////////////////////////////////////////////////////////////////////////////
/// Get path to personal directory for application with given version.
/// @param[in] version Application name and version to use. If NULL,
///                    value set by SetApplVersion() will be used.
/// @return Required path.
const PathStr IUser::GetPersApplVersPath(const char *version)
  {
  if (version == NULL && !s_pers_appl_vers.IsEmpty())
    return s_pers_appl_vers;

  if (version != NULL)
    {
    GetPersIntegraPath();
    if (!s_personal_integra.IsEmpty())
      return PathStr(s_personal_integra, version);
    }

  return PathStr::GetRootPath();
  }  // GetPersApplVersPath()

//////////////////////////////////////////////////////////////////////////////
/// Get portable path to common documents Integra folder.
///
/// Depending on "portable" mark file, it will be ether
/// "C:/Users/Public/Documents/Integra"
/// or application root directory.
/// @return Required path.
const PathStr IUser::GetPortableCommonIntegraPath()
  {
  PathStr path(PathStr(PathStr::GetRootPath(), "macros"), "portable", "dat");
  if (path.IsFile())
    return PathStr::GetRootPath();
  return GetCommonIntegraPath();
  }  // GetPortableCommonIntegraPath()

//////////////////////////////////////////////////////////////////////////////
/// Get portable path to common documents directory for application with given version.
///
/// Depending on "portable" mark file, it will be ether
/// "C:/Users/Public/Documents/Integra/<Product> <Version>"
/// or application root directory.
/// @param[in] version Application name and version to use. If NULL,
///                    value set by SetApplVersion() will be used.
/// @return Required path.
const PathStr IUser::GetPortableApplVersPath(const char *version)
  {
  PathStr path(PathStr(PathStr::GetRootPath(), "macros"), "portable", "dat");
  if (path.IsFile())
    return PathStr::GetRootPath();
  return GetCommonApplVersPath(version);
  }  // GetPortableApplVersPath()

//////////////////////////////////////////////////////////////////////////////
/// Get portable path to application data directory for application with given version.
///
/// Depending on "portable" mark file, it will be ether
/// "C:/ProgramData/Integra" or application root directory.
/// @return Required path.
const PathStr &IUser::GetPortableAppdataPath()
  {
  PathStr path(PathStr(PathStr::GetRootPath(), "macros"), "portable", "dat");
  if (path.IsFile())
    return PathStr::GetRootPath();
  return GetAppdataIntegraPath();
  }  // GetPortableAppdataPath()

//////////////////////////////////////////////////////////////////////////////
/// Get data folder of the application.
///
/// The method gets path for the data folder of the application.
///
/// The default implementation returns the "macros" subfolder of the root
/// application folder, if the folder exists, the root application folder
/// otherwise.
/// @return The path to the data folder.
const PathStr &IUser::GetDataPath()
  {
  if (data_path.IsEmpty())
    {
    // Initialize the value
    data_path = PathStr(PathStr::GetRootPath(), "macros");
    if (!data_path.IsDir())
      data_path = PathStr::GetRootPath();
    }
  return data_path;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the IOF signature string.
/// @return The current IOF signature string, set previously by SetIofSignature().
/// If nothing was set, NULL is returned.
const char *IUser::GetIofSignature()
  {
  return s_iof_sign.Data();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the IOF signature string.
///
/// Method sets IOF signature string, which will be written into IOF file in
/// its beginning. Set NULL to write current application name and version
/// (this is default behavior). Set empty string ("") to not write any signature.
/// Ctrl-Z symbol is not allowed in this string, such string will be ignored.
/// @param[in] signature Signature string to be written into IOF file.
void IUser::SetIofSignature(const char *signature)
  {
  Str s(signature);
  if (s.FindFirst('\032'))
    return;
  s_iof_sign = s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get an application version.
/// @return the current version of the application, which uses this User
/// object.
const char *IUser::GetApplVersion()
  {
  return s_version_str.Data();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set an application version.
/// @param[in] version - The application name with version to be returned
/// further by GetApplVersion().
void IUser::SetApplVersion(const char *version)
  {
  s_version_str = Str(version);
  s_pers_appl_vers = PathStr(GetPersIntegraPath(), s_version_str);
  if (s_personal_integra.IsEmpty())
    s_pers_appl_vers = PathStr::GetRootPath();
  s_common_appl_vers = PathStr(GetCommonIntegraPath(), s_version_str);
  if (s_common_integra.IsEmpty())
    s_common_appl_vers = PathStr::GetRootPath();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set path to application INI file.
///
/// Method stores application-wide path to INI file in User object.
/// It is responsibility of application startup routine to call
/// this method as soon as path to INI file becomes known.
/// @param[in] ini_file Path to application INI file.
void IUser::SetINIFile(const CHAR *ini_file)
  {
  s_ini_file = ini_file;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get path to application INI file.
///
/// Always check result for empty string - this mean application starup routine
/// did not set path to INI file.
/// @return Path to application INI file.
const PathStr &IUser::GetINIFile()
  {
  return s_ini_file;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get path to application log file.
///
/// @return Path to the log file.
const PathStr &IUser::GetLogFilePath()
  {
  return s_log.PathName();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get application log file.
///
/// @return The log file.
File &IUser::GetLogFile()
  {
  return s_log;
  }

//////////////////////////////////////////////////////////////////////////////
/// Print a debug message to the debugger's output window.
///
/// In the DEBUG configuration, the  method provides output of a debug message to the debugger's output window.
/// In the release configuration, the  method does nothing.
///
/// @param[in] format Format string and possibly values to substitute to it, like in printf().
///
/// @note Implemented for Windows only.
///
void Debug(const char *format, ...)
  {
#if defined(DEBUG) && defined(WIN32)
  // Format the message
  char str[Str::MAX_LENGTH + 1];

  va_list arg_list;
  va_start(arg_list, format);
  _vsnprintf((char *)str, Str::MAX_LENGTH,
             (format == NULL) ? "" : (char *)format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  OutputDebugString(strcat(str, "\n"));
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Switch Log file On.
///
void IUser::SwitchLogFileOn()
  {
  if (s_log.Opened())
    return;

  // Make sure "logs" directory exists
  PathStr log_dir(GetPersApplVersPath(), "logs");
  if (!log_dir.IsDir())
    log_dir.MakeDir();
  // Construct name of session log file
  Time current_time;
  UStr stmp = current_time.DateStamp();
  PathStr exe_path;
  exe_path.GetExecutablePath();
  Str exe_name = exe_path.BaseName();
  UStr id_str;
  id_str.Printf("%s.%s-%02d.%02d.%02d-%d", exe_name.Data(), stmp.Data(), current_time.hour,
    current_time.minute, current_time.second, getpid());
  PathStr log_name(log_dir, id_str, "log");
  s_log = File(log_name);

  // Open log file
  s_log.Open("a");
  Assert(s_log.Opened());
  }  // SwitchLogFileOn()

//////////////////////////////////////////////////////////////////////////////
/// Switch Log file Off.
///
void IUser::SwitchLogFileOff()
  {
  if (!s_log.Opened())
    return;

  // Print memory statistic
  int limit, phys_limit, peak_ws, peak_pf;
  Str timestamp = s_user_timer.TimeStamp();
  Envi::GetMemSize(NULL, &limit, NULL, &phys_limit, &peak_ws, &peak_pf);

  unsigned id;
#ifdef _WIN32
  id = GetCurrentThreadId();
#else
#error Assign "id" by thread identifier
#endif

  s_sync.Mono();
  s_log.Printf("%s [%u] Virtual memory:     %6d MB\n", timestamp.Data(), id, limit);
  s_log.Printf("%s [%u] Physical memory:    %6d MB\n", timestamp.Data(), id, phys_limit);
  s_log.Printf("%s [%u] Peak working set:   %6d MB\n", timestamp.Data(), id, peak_ws);
  s_log.Printf("%s [%u] Peak commit charge: %6d MB\n", timestamp.Data(), id, peak_pf);
  s_sync.Multi();

  if (s_log.Close() != SUCCESS)
    Assert(false);
  }  // SwitchLogFileOff()

//////////////////////////////////////////////////////////////////////////////
/// Check whether logging is on.
///
/// @return @c true, if logging is on, @c false otherwise.
///
bool IUser::LoggingIsOn()
  {
  return s_log.Opened();
  }  // LoggingIsOn()

/// QuickEdit flag for console mode.
const int ENABLE_QUICK_EDIT = 0x0040;

//////////////////////////////////////////////////////////////////////////////
/// Disable QuickEdit mode.
///
/// Call this method in the beginning of console application to disable
/// QuickEdit mode and thus to avoid occasional pausing of application.
void IUser::DisableQuickEdit()
  {
#ifdef _WIN32
  DWORD mode;
  HANDLE console = GetStdHandle(STD_INPUT_HANDLE); 

  if (console == NULL)
    return;

  if (GetConsoleMode(console, &mode) == 0)
    return;

  // Do not overwrite initially stored mode
  if (qe_mode < 0)
    qe_mode = (mode & ENABLE_QUICK_EDIT) ? 1 : 0;

  if (mode & ENABLE_QUICK_EDIT)
    {
    mode &= ~ENABLE_QUICK_EDIT;
    SetConsoleMode(console, mode);
    }
#endif
  }  // DisableQuickEdit()

//////////////////////////////////////////////////////////////////////////////
/// Restore QuickEdit mode.
///
/// Call this method to restore initial state of QuickEdit mode, which was
/// at the moment of DisableQuickEdit() call. If DisableQuickEdit()
/// was not called, this method does nothing.
void IUser::RestoreQuickEdit()
  {
#ifdef _WIN32
  if (qe_mode != 1)
    return;

  DWORD mode;
  HANDLE console = GetStdHandle(STD_INPUT_HANDLE); 

  if (console == NULL)
    return;

  if (GetConsoleMode(console, &mode) == 0)
    return;

  mode |= ENABLE_QUICK_EDIT;
  SetConsoleMode(console, mode);
#endif
  }  // RestoreQuickEdit()

//////////////////////////////////////////////////////////////////////////////
/// Get time in a user friendly form.
/// @param[in] show_ms - Specifies a format of time;
/// - @b true - time is printed in the form "hh:mm:ss.mmm";
/// - @b false - time is printed in the form "hh:mm:ss".
/// @return A string with current user time.
const UStr IUser::SessionTimeStamp(bool show_ms)
  {
  return s_user_timer.TimeStamp(show_ms);
  }  // SessionTimeStamp()

//////////////////////////////////////////////////////////////////////////////
/// Switch multiprogress bar usage On/Off.
/// @param val State of the flag.
void IUser::SetMultiProgress(bool val)
  {
  Assert(!InProgress());
  m_multi_progr = val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check multiprogress bar usage state.
/// @return State of the flag.
bool IUser::IsMultiProgress() const
  {
  return m_multi_progr;
  }

//////////////////////////////////////////////////////////////////////////////
/// Is operation under progress stopable?
/// @return State of the flag.
bool IUser::IsProgressStopable() const
  {
  return m_stopable;
  }

/// Whether it is called from thread different from that User had been created in
/// If yes, one can call InitProgress(), ShowProgress(), TermProgress() freely, else not
bool IUser::IsInOriginalThread() const
  {
  return IntGetCurrentThreadId() == user_progr_thread_id;
  }


//////////////////////////////////////////////////////////////////////////////
/// Format number according to current locale.
///
/// Method formats number (represented by string of characters).
/// Digits before decimal separator are grouped by 3.
/// Decimal and thousand separators are taken from user locale settings.
/// @param[in] number - source number to convert.
///   This string can only contain the following characters: 
///   - characters '0' through '9';
///   - one decimal point (dot) if the number is a floating-point value.
///   - a minus sign in the first character position if the number is a negative value.
///   All other characters are invalid.
/// The function returns not changed string if this parameter deviates from these rules.
/// @param[in] dec_num - number of digits to print after decimal separator;
///            if 0, decimal separator is not printed too.
/// @return Formatted number (represented by string of characters) or source number if
///         formatting could not be done.
UStr IUser::NumberFormat(const char *number, int dec_num)
  {
#ifdef _WIN32
  // Since by default GetNumberFormat() always prints integer as float,
  // explicitly fill NUMBERFMT structure as we need.
  TArray<CHAR> buf;
  NUMBERFMT num_fmt;
  char dec_sep[5], th_sep[5];

  num_fmt.NumDigits = dec_num;
  num_fmt.LeadingZero = 0;
  num_fmt.Grouping = 3; 
  num_fmt.lpDecimalSep = dec_sep;
  num_fmt.lpThousandSep = th_sep;
  num_fmt.NegativeOrder = 1; 
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, dec_sep, 5);
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, th_sep, 5);
  buf.Allocate(GetNumberFormat(LOCALE_USER_DEFAULT, 0, number, &num_fmt, NULL, 0));
  if (buf.Length() == 0)
    return (UStr)number;
  GetNumberFormat(LOCALE_USER_DEFAULT, 0, number, &num_fmt, buf.Data(), buf.Length());
  return (UStr)buf.Data();
#else
  return (UStr)number;
#endif
  }  // NumberFormat()

//////////////////////////////////////////////////////////////////////////////
// Description of abstract methods

/**
@fn void IUser::ErrorMsg(const CHAR *msg)

Issue an error message to the user and wait for confirmation.
Derived implementation of the User class must provide own
implementation of this method.

The method issues an ERROR message to the user
(may include confirmation from the user).

A typical GUI opens a message box with the error message and the OK button.
@param[in] msg - The message; may not be NULL.
**/

/**
@fn void IUser::WarningMsg(const CHAR *msg)

Issue a warning message to the user and wait for confirmation.

The method issues a WARNING message to the user
(may include a confirmation from the user).
A derived implementation of the User class must provide own
implementation of this method.

A typical GUI opens a message box with the warning message and the OK button.
@param[in] msg - The message; may not be NULL.
**/

/**
@fn void IUser::StatusMsg(const CHAR *msg)

Issue a status information.

The method issues a STATUS message to the user
(may include a confirmation from the user).
A derived implementation of the User class must provide an own
implementation of this method.

A typical GUI writes the message to the status field of the
main application window. The empty format parameter (the default)
cleans the status message.
@param[in] msg - The message; may not be NULL.
**/

//////////////////////////////////////////////////////////////////////////////
/// Issue a message to the log.
///
/// The method issues a LOG message to the Message Log.
/// A derived implementation of the User class must provide an own
/// implementation of this method.
///
/// A typical GUI writes the message to the Message Log window.
/// @param[in] msg - The message; may not be NULL.
///
void IUser::LogMsg(const CHAR *msg)
  {
  if (!s_log.Opened())
    return;

  unsigned id;
#ifdef _WIN32
  id = GetCurrentThreadId();
#else
#error Assign "id" by thread identifier
#endif

  s_sync.Mono();
  s_log.Printf("%s [%u] %s\n", s_user_timer.TimeStamp().XData(), id, msg);
  s_log.Flush();
  s_sync.Multi();
  }  // LogMsg()

/**
@fn bool IUser::Ask(const CHAR *msg, ReplyStyle rs, bool def)

Issue a message that requires a reply.

The method issues the message to the user that requires a confirmation from him.
A derived implementation of the User class must provide an own
implementation of this method.

A typical GUI opens a message box with the message and buttons specified via
the parameter.
@param[in] msg - The message; may not be NULL.
@param[in] rs - A value of the ReplyStyle type that specifies possible buttons
that can present ('Ok, 'Ok/Cancel', 'Yes/No').
@param[in] def - Defines which button in pair ('Ok/Cancel', 'Yes/No') is the
default.
@return true if positive reply was obtained.
**/

//////////////////////////////////////////////////////////////////////////////
/// Start script execution (system usage).
///
/// @return Current progress count (it should be saved and used as argument
///   forassociated FinishScripting() call).
///
int IUser::StartScripting()
  {
  Assert(m_script_count >= 0);
  m_script_count++;
  return m_progr_count;
  }  // StartScripting()

//////////////////////////////////////////////////////////////////////////////
/// Finish script execution (system usage).
///
/// @param[in] progr_count Current progress count returned by associated StartScripting() call.
///
void IUser::FinishScripting(int progr_count)
  {
  // Pending progresses are terminated for sure
  while (m_progr_count > progr_count)
    TermProgress();
  Assert(m_progr_count == progr_count);
  m_progr_count = progr_count;
  m_script_count--;
  Assert(m_script_count >= 0);
  Assert(m_active_progr_count <= m_progr_count);
  }  // FinishScripting()

//////////////////////////////////////////////////////////////////////////////
/// Indicate if a script is being executed.
bool IUser::InScript()
  {
  return m_script_count > 0;
  }  // InScript()

INTEGRA_NAMESPACE_END
