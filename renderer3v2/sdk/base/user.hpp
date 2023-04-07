/// @file
///
/// @brief Declaration of IUser class.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '99.07.15

#ifndef _KLXX_USER_HPP_
#define _KLXX_USER_HPP_

#include "iarray.hpp"
#include "file.hpp"
#include "str.hpp"
#include "export.h"

INTEGRA_NAMESPACE_START
class Entity;

#if (_MSC_VER >= 1400) || defined(__linux)
/// Macro allowing access to untranslated string.
#define ErrorMessage(x, ...) ErrorMessageEx(ExTr##x, x, ##__VA_ARGS__)
/// Macro allowing access to untranslated string.
#define WarningMessage(x, ...) WarningMessageEx(ExTr##x, x, ##__VA_ARGS__)
/// Macro allowing access to untranslated string.
#define StatusMessage(x, ...) StatusMessageEx(ExTr##x, x, ##__VA_ARGS__)
/// Dummy macro for translation of user strings.
#define ExTr
/// Dummy macro for translation of user strings.
#define ExTrTr(str, ...) FTr(str, ##__VA_ARGS__)
/// Dummy macro for translation of user strings.
#define ExTrFTr(str, ...) FTr(str, ##__VA_ARGS__)
/// Dummy macro for translation of user strings.
#define ExTrtr(str, ...) ftr(str, ##__VA_ARGS__)
/// Dummy macro for translation of user strings.
#define ExTrftr(str, ...) ftr(str, ##__VA_ARGS__)
#else // Not supported under VC++ 6.0
/// Macro allowing access to extended function.
#define ErrorMessage(x) ErrorMessageEx(x, x)
/// Macro allowing access to extended function.
#define WarningMessage(x) WarningMessageEx(x, x)
/// Macro allowing access to extended function.
#define StatusMessage(x) StatusMessageEx(x, x)
#endif

/// A stream-level interface to interact with the end user.
class IUser
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE IUser(unsigned active_progr_delay = 1000U);
    /// Destructor.
    INTAPI_BASE virtual ~IUser();

  private:
    /// Copy constructor.
    IUser(const IUser &);
    //@}

  public:
    /// @name General messages
    //@{
    /// Issue an error message to the user and wait for confirmation.
    INTAPI_BASE OKAY ErrorMessageEx(const CHAR *format_notr, const CHAR *format, ...);
    /// Issue a warning message to the user and wait for confirmation.
    INTAPI_BASE void WarningMessageEx(const CHAR *format_notr, const CHAR *format, ...);
    /// Postpone warnings or display postponed warnings.
    INTAPI_BASE void PostponeWarnings(bool postpone);
    /// Issue a status information.
    INTAPI_BASE void StatusMessageEx(const CHAR *format_notr, const CHAR *format = NULL, ...);
    /// Clear status message field.
    INTAPI_BASE void ClearStatusMessage();
    /// Issue a message to the log.
    INTAPI_BASE void LogMessage(const CHAR *format, ...);
    /// Formatted output to the stdout.
    INTAPI_BASE void Printf(const char *format, ...);
    /// Notify specified action.
    INTAPI_BASE virtual void NotifyAction(Entity *object, const Str &name);
    //@}

  public:
    /// @name Extended message abilities
    //@{
    /// Issue a message that requires confirmation.
    INTAPI_BASE void AskOK(const CHAR *format, ...);
    /// Issue a message that requires a reply Ok or Cancel.
    INTAPI_BASE bool AskOKCancel(const CHAR *format, ...);
    /// Issue a message that requires a reply Yes or No.
    INTAPI_BASE bool AskYESNo(const CHAR *format, ...);
    /// Issue a message that requires a reply Yes or No.
    INTAPI_BASE bool AskYesNO(const CHAR *format, ...);
    /// Issue a message with custom replies.
    INTAPI_BASE virtual int Information(const CHAR *msg,
                const CHAR *button0text = NULL,
                const CHAR *button1text = NULL,
                const CHAR *button2text = NULL,
                int def_button = 0, int esc_button = -1);
    //@}

    /// @name File selection
    //@{
    /// Get the file name using Open File dialog.
    INTAPI_BASE virtual PathStr SelectOpenPathName(const PathStr &pathname, 
      const UStr &sel_dlg_caption, const TArray<UStr> &filter_list, const TArray<UStr> &filter_names_list,
      int &selected_filter);
    /// Get the file name using Save File dialog.
    INTAPI_BASE virtual PathStr SelectSavePathName(const PathStr &pathname, 
      const UStr &sel_dlg_caption, const TArray<UStr> &filter_list, const TArray<UStr> &filter_names_list,
      int &selected_filter);
    /// Get the folder name using folder selection dialog.
    INTAPI_BASE virtual PathStr SelectFolderPathName(const PathStr &pathname, const UStr &sel_dlg_caption);
    //@}
  public:
    /// @name Specific error messages
    //@{
    /// A "no memory" message.
    INTAPI_BASE OKAY ErrorNoMemory(INT64 amount = -1);
    /// An "internal error" message.
    INTAPI_BASE OKAY InternalError(const CHAR *format = NULL, ...);
    //@}

  public:
    /// @name Progress indication
    //@{
    /// Start the progress indication.
    INTAPI_BASE virtual void InitProgress(bool stopable = false, bool busy_mode = false);
    /// Show the progress indication.
    INTAPI_BASE virtual void ShowProgress(double to_be_done, const CHAR *format, ...);
    /// End the progress indication.
    INTAPI_BASE virtual void TermProgress();
    /// Get the progress status.
    INTAPI_BASE bool InProgress() const;
    /// Get progress quiet mode.
    INTAPI_BASE bool GetProgressQuietMode();
    /// Set progress quiet mode.
    INTAPI_BASE void SetProgressQuietMode(bool quiet = false);
    /// Set sign of the thread controls usage.
    INTAPI_BASE virtual void UseThreadControls(bool val);
    /// Switch multiprogress bar usage On/Off.
    INTAPI_BASE void SetMultiProgress(bool val);
    /// Check multiprogress bar usage state.
    INTAPI_BASE bool IsMultiProgress() const;
    /// Is operation under progress stopable?
    INTAPI_BASE bool IsProgressStopable() const;

    /// Whether it is called from thread different from that User had been created in
    INTAPI_BASE bool IsInOriginalThread() const;
    //@}

  protected:
    /// @name Progress processing
    //@{
    /// Start of progress indication.
    virtual void InitProgr(int level) = 0;
    /// Show progress indication.
    virtual void ShowProgr(double done, const CHAR *msg) = 0;
    /// End of progress indication.
    virtual void TermProgr(int level) = 0;
    //@}

  protected:
    /// @name Get progress indication information
    /// These methods are to be used by ShowProgr() to get current progress information for user. 
    //@{
    /// Get the progress count.
    INTAPI_BASE int ProgrCount() const;
    /// Get busy mode flag.
    INTAPI_BASE bool BusyMode() const;
    /// Get progress message for given level.
    INTAPI_BASE const CHAR *GetProgressMsg(int level) const;
    /// Get current progress value for given level.
    INTAPI_BASE double GetProgressVal(int level) const;
    //@}

  public:
    /// @name Break processing
    //@{
    /// Check for the user break.
    INTAPI_BASE virtual bool Break();
    /// Check for the user break without calling QT or console functions.
    INTAPI_BASE bool IsBreak() const;
    /// Set the break flag.
    INTAPI_BASE virtual void SetBreak();
    /// Reset the break flag.
    INTAPI_BASE void ResetBreak();
    /// Set exit flag.
    INTAPI_BASE void SetExit();
    /// Process UI events, if any.
    INTAPI_BASE virtual void ProcessEvents();
    //@}

  public:
    /// Set the topmost flag.
    INTAPI_BASE virtual void SetTopmost(bool topmost);
    /// Set the event filter used flag (Temporary!!!).
    INTAPI_BASE virtual void SetEventFilterUsed(bool used);
    /// Check - is event filter used.
    INTAPI_BASE virtual bool IsEventFilterUsed();
    /// Set CATIA mode.
    INTAPI_BASE void SetCATIAMode(bool mode);
    /// Get CATIA mode.
    INTAPI_BASE bool GetCATIAMode();


  public:
    /// Get the data folder of the application.
    INTAPI_BASE virtual const PathStr &GetDataPath();

  public:
    /// Format number according to current locale.
    INTAPI_BASE virtual UStr NumberFormat(const char *number, int dec_num = 0);

  protected:
    /// Types of reply allowed in Ask() methods.
    enum ReplyStyle
      {
      /// 'Ok' allowed.
      RS_OK,
      /// 'Ok' and 'Cancel' allowed.
      RS_OKCANCEL,
      /// 'Yes' and 'No' allowed.
      RS_YESNO
      };

  private:
    /// @brief Nested progress information.
    ///
    /// The structure contains information about nested progress level:
    /// - left and right bounds of subprogress in whole top level progress bar;
    /// - progress message in effect
    struct ProgrInfo
      {
      /// Left bound.
      double p_left;
      /// Right bound.
      double p_right;
      /// Last progress message for the level (NULL if no).
      UStr p_last_msg;
      /// Last progress time.
      unsigned p_next_time;
      /// Last log message time for long-time operations.
      unsigned p_next_log_time;
      /// Busy mode flag.
      bool p_busy_mode;
      /// Level initialization flag.
      bool p_inited;
      };

  private:
    /// Number of progress level to be processed.
    enum { MAX_PROGRESS_LEVEL = 12 };

  protected:
    /// @name Abstract methods
    //@{
    /// Issue an error message to the user and wait for the confirmation.
    virtual void ErrorMsg(const CHAR *msg) = 0;
    /// Issue a warning message to the user and wait for the confirmation.
    virtual void WarningMsg(const CHAR *msg) = 0;
    /// Issue a status information.
    virtual void StatusMsg(const CHAR *msg) = 0;
    /// Issue a message to the log.
    INTAPI_BASE virtual void LogMsg(const CHAR *msg);
    /// Issue a message that requires a reply.
    virtual bool Ask(const CHAR *msg, ReplyStyle rs, bool def) = 0;
    /// Formatted output to the stdout.
    virtual void PrintMsg(const CHAR *msg) = 0;
    //@}

  public:
    /// Start script execution (system usage).
    INTAPI_BASE int StartScripting();
    /// Finish script execution (system usage).
    INTAPI_BASE void FinishScripting(int progr_count);

  protected:
    /// Indicate if a script is being executed.
    INTAPI_BASE bool InScript();

  public:
    /// @name IOF signature
    //@{
    /// Get the IOF signature string.
    INTAPI_BASE static const char *GetIofSignature();
    /// Set the IOF signature string.
    INTAPI_BASE static void SetIofSignature(const char *signature);
    //@}

  public:
    /// @name Version information
    //@{
    /// Get the application version.
    INTAPI_BASE static const char *GetApplVersion();
    /// Set the application version.
    INTAPI_BASE static void SetApplVersion(const char *version);
    //@}

  public:
    /// @name Paths
    //@{
    /// Get path to common documents Integra folder.
    INTAPI_BASE static const PathStr &GetCommonIntegraPath();
    /// Get path to personal documents Integra folder.
    INTAPI_BASE static const PathStr &GetPersIntegraPath();
    /// Get path to application data Integra folder.
    INTAPI_BASE static const PathStr &GetAppdataIntegraPath();
    /// Get path to common directory for application with given version.
    INTAPI_BASE static const PathStr GetCommonApplVersPath(const char *version = NULL);
    /// Get path to personal directory for application with given version.
    INTAPI_BASE static const PathStr GetPersApplVersPath(const char *version = NULL);
    /// Get portable path to common documents Integra folder.
    INTAPI_BASE static const PathStr GetPortableCommonIntegraPath();
    /// Get portable path to common documents directory for application with given version.
    INTAPI_BASE static const PathStr GetPortableApplVersPath(const char *version = NULL);
    /// Get portable path to application data directory for application with given version.
    INTAPI_BASE static const PathStr &GetPortableAppdataPath();
    /// Set path to application INI file.
    INTAPI_BASE static void SetINIFile(const CHAR *ini_file);
    /// Get path to application INI file.
    INTAPI_BASE static const PathStr &GetINIFile();
    /// Get path to application log file.
    INTAPI_BASE static const PathStr &GetLogFilePath();
    //@}

  public:
    /// Get current user time.
    INTAPI_BASE static unsigned SessionElapsed();

  public:
    /// @name Log file control
    //@{
    /// Switch Log file On.
    INTAPI_BASE static void SwitchLogFileOn();
    /// Switch Log file Off.
    INTAPI_BASE static void SwitchLogFileOff();
    /// Switch Log file Off.
    INTAPI_BASE static bool LoggingIsOn();
    /// Get time in a user friendly form.
    INTAPI_BASE static const UStr SessionTimeStamp(bool show_ms = false);
    //@}

  protected:
    INTAPI_BASE static File &GetLogFile();

  public:
    /// @name QuickEdit mode control
    //@{
    /// Disable QuickEdit mode.
    INTAPI_BASE static void DisableQuickEdit();
    /// Restore QuickEdit mode.
    INTAPI_BASE static void RestoreQuickEdit();
    //@}

  public:
    /// Start the progress indication.
    INTAPI_BASE virtual void InitProgressMT(bool stopable = false, bool busy_mode = false);
    /// Show the progress indication.
    INTAPI_BASE virtual void ShowProgressMT(double to_be_done, const CHAR *format, ...);
    /// End the progress indication.
    INTAPI_BASE virtual void TermProgressMT();
    /// Check for the user break.
    INTAPI_BASE virtual bool BreakMT();
    /// Set the break flag.
    INTAPI_BASE virtual void SetBreakMT();
    /// Reset the break flag.
    INTAPI_BASE void ResetBreakMT();
    /// Show progress from the last saved value.
    INTAPI_BASE void ShowLastProgress(const char *str = NULL);
    /// Terminate active progress completely and close progress bar.
    INTAPI_BASE void TermLastProgress();

  protected:
    /// Path to the macro data folder.
    PathStr data_path;

  private:
    /// Number of currently entered progresses.
    int m_progr_count;
    /// Number of currently active progresses.
    int m_active_progr_count;
    /// Delay for active progress indication (msec).
    unsigned m_active_progr_delay;
    /// Number of scripts beeing executed.
    int m_script_count;
    /// Progress quiet mode.
    bool m_progr_quiet;
    /// Break indicator.
    bool break_flag;
    /// Exit flag.
    bool m_exit_flag;
    /// Bounds of nested progresses (total MAX_PROGRESS_LEVEL).
    ProgrInfo m_progr_info[MAX_PROGRESS_LEVEL];
    /// Last passed progress message.
    UStr m_prev_progr_msg;
    /// Use multi progress bar.
    bool m_multi_progr;
    /// Is operation under progress stopable?
    bool m_stopable;
    /// Flag indicating - postpone warnings or display them immediately.
    bool m_postpone_warn;
    /// Pointer to array of postponed warnings.
    IArray *m_postponed_warn; // TArray can't be compiled here.
    /// User thread ID.
    void *user_progr_thread_id;
    /// User thread id for multithread mode.
    void *user_progr_thread_id_mt;
    /// Last progress value for multithread mode.
    double last_progr_value;
    /// Last progress message for multithread mode.
    UStr last_progr_msg;
    /// Last SHOWN progress value for multithread mode.
    double last_shown_progr_value;

  private:
    /// IOF signature.
    static Str s_iof_sign;
    /// Application version.
    static Str s_version_str;
    /// Path to the common documents Integra folder.
    static PathStr s_common_integra;
    /// Path to the personal documents Integra folder.
    static PathStr s_personal_integra;
    /// Path to the common application data Integra folder.
    static PathStr s_appdata_integra;
    /// Path to common directory for application with given version.
    static PathStr s_common_appl_vers;
    /// Path to personal directory for application with given version.
    static PathStr s_pers_appl_vers;
    /// The current path of the application.
    static PathStr s_current_path;
    /// The path to INI file of the application.
    static PathStr s_ini_file;
    /// CATIA working mode (used in I2 Server and other service utilities).
    bool catia_mode;

  }; // class IUser

//////////////////////////////////////////////////////////////////////////////
// Global methods

/// Print debug message to the debugger's output window.
INTAPI_BASE void Debug(const char *format, ...);

/// Set a new User object for the given task.
INTAPI_BASE void SetUser(IUser *user);

/// Set a new User object for the given thread.
INTAPI_BASE void SetThreadUser(IUser *user, void *id);

/// Access the current User object of given task.
INTAPI_BASE IUser *User();

//////////////////////////////////////////////////////////////////////////////
/// Assert with automatical log message.
#define AssertLog(_Expression) (void)( (!!(_Expression)) || \
  (Assert(_Expression), User()->LogMessage("Assert (%s). File %s, line %d", #_Expression, __FILE__, __LINE__), 0 ))

INTEGRA_NAMESPACE_END

#endif

