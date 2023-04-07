/// @internal
/// @file
///
/// @brief Plugin-related functions definitions.
///
/// @author Eed - E. Denisov KLON, '04.07.06.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#ifdef _WIN32
#include <windows.h>
#else  // Linux
#include <dlfcn.h>
#define LoadLibrary(name)            dlopen(name, RTLD_LAZY | RTLD_GLOBAL)
#define FreeLibrary(dll)             dlclose(dll)
#define GetProcAddress(dll, symbol)  dlsym(dll, symbol)
#endif

#include <integra.hpp>
#include <python_version.hpp>
#include "dict.hpp"
#include "envi.hpp"
#include "framework.hpp"
#include "garray.hpp"
#include "plugins.hpp"
#include "plugins.hxx"
#include "str.hpp"
#include "user.hpp"

/// Pointer type for InitClasses() functions.
typedef int (*InitClassesType)();

/// Pointer type for TermClasses() functions.
typedef void (*TermClassesType)();

/// Pointer type for VersionId() functions.
typedef int (*VersionIdType)();

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
// Static variables

/// Count of initializations of the framework.
static int init_no = 0;

/// List of explicitly loaded plugins.
static TDict<HMODULE, int> loaded_plugins;
/// List of to-be-initialized plugins.
static GArray<HMODULE> plugins_to_init;
/// List of initialized plugins.
static GArray<HMODULE> inited_plugins;

//////////////////////////////////////////////////////////////////////////////
// Static function prototypes

/// Initializes all plugins from to-be-inited list.
static OKAY InitAllPlugins();
/// Terminates specific set of plugins.
static void TermAllPlugins(const GArray<HMODULE> &plugins_to_term);


//////////////////////////////////////////////////////////////////////////////
/// Initialize Integra Object Framework.
///
/// This function initializes Integra Object Framework.
///
/// The shell must initialize Integra Object Framework
/// using this global function before usage of plugins.
///
/// This function performs DLL version control: if an incompatible DLL
/// was loaded, user is informed about this and the application is terminated.
///
/// The User object must be already available.
/// @param[in] version - The repository version. Normally the user need
/// no change the default value.
void InitFrameWork(int version)
  {
  if (init_no++ == 0)
    {
#ifdef WIN32
    _set_error_mode(_OUT_TO_MSGBOX);
#endif
    // Check version
    if (REPOSITORY_VERSION != version)
      {
      User()->ErrorMessage(Tr("Couldn't load \"base.dll\": bad version -"
        " expected %#x, found %#x"), version, REPOSITORY_VERSION);
      Assert(false);
      // Further work is impossible, termination.
      exit(1);
      }

    // Determine Python location
    PathStr py_root_path = Envi::GetEnv(PYTHON_PATH_EV);
    if (py_root_path.IsEmpty())
      py_root_path = IUser::GetPortableAppdataPath();
    py_root_path.AddPath(Str("Python-") + Str(PYTHON_VER));

    // Add Python paths
    PathStr py_bin_path(py_root_path, "bin");
    PathStr py_win32_path(py_root_path, "Lib");
#ifdef DEBUG
    py_bin_path += Str("d");
#endif
#ifdef _WIN64
    py_bin_path += Str("64");
    py_win32_path += Str("64");
#else
    py_bin_path += Str("32");
    py_win32_path += Str("32");
#endif
    py_win32_path = PathStr(PathStr(py_win32_path, "site-packages"), "win32");
    PathStr py_add_path(py_bin_path);
    py_add_path += Str(";") + py_win32_path + Str(";") + Envi::GetEnv("PATH");
    Envi::PutEnv("PATH", py_add_path);

    // Check presence of Python DLL
    PathStr py_dll(PYTHON_VER);
    py_dll = Str("python") + py_dll.SubStr(0, 1) + py_dll.SubStr(2, 1);
#ifdef DEBUG
    py_dll += Str("_d");
#endif
    py_dll += Str(".dll");
    PathStr py_dll_path(py_bin_path, py_dll);
    if (py_dll_path.IsFile())
      User()->LogMessage("%s found in %s", py_dll.Data(), py_bin_path.Data());
    else
      {
      User()->LogMessage("%s not found in %s", py_dll.Data(), py_bin_path.Data());
      py_dll_path = py_dll;
      if (py_dll_path.FindOnPath())
        User()->LogMessage("Unexpectedly found %s", py_dll_path.Data());
      else
        User()->LogMessage("%s not found on the path as well", py_dll.Data());
      }
    }

  // Some plugins already could be loaded by OS.
  // Initialize all of them.
  if (InitAllPlugins() != SUCCESS)
    {
    Assert(false);
    // Clear list to avoid repetitive attempts of failed initializations
    plugins_to_init.Truncate();
    // Further work is impossible, termination.
    exit(1);
    }
  }  // InitFrameWork()

//////////////////////////////////////////////////////////////////////////////
/// Terminate Integra Object Framework.
///
/// The shell must terminate Integra Object Framework before exit.
void TermFrameWork()
  {
  switch (init_no)
    {
    case 0:
      Assert(false);
      return;
    case 1:
      // Terminate all initialized plugins
      TermAllPlugins(inited_plugins);
      inited_plugins.Truncate();

      // Unload all loaded plugins
      for (int i = loaded_plugins.Length() - 1; i >= 0; i--)
        for (int j = 0; j < loaded_plugins.Value(i); j++)
          FreeLibrary(loaded_plugins.GetKey(i));
      loaded_plugins.RemoveElements();
      // No break here - fall down to decrement counter
    default:
      init_no--;
    }
  }  // TermFrameWork()

//////////////////////////////////////////////////////////////////////////////
/// Load a plugin DLL.
///
/// All IOF plugins must be loaded using this function.
///
/// If an extension is not specified, .dll is assumed.
///
/// If no path is specified, the DLL is searched in the following directories,
/// in exactly this order:
/// 
/// -# The directory where the executable module for
///    the current process is located.
/// -# The current directory.
/// -# The Windows system directory (usually it is C:\\Windows\\System32).
/// -# The Windows 16-bit system directory (usually it is C:\\Windows\\System).
/// -# The Windows directory (usually it is C:\\Windows).
/// -# The directories listed in the PATH environment variable.
///
/// If found, an attempt to load the plugin is performed.
/// During loading, version control of the plugin is performed: it must have
/// the same version as the main executable.
/// If successful, the plugin's InitClasses() global function is executed.
/// If the function returns FAILURE, DLL is immediately unloaded.
/// @param[in] name - Plugin (i.e. DLL) name.
/// @param[in] required - Defines behavior in the case when the requested plugin
/// was not loaded by any reason. If @b true - a
/// corresponding error message is issued and FAILURE is returned;
/// if @b false - no error message and SUCCESS is returned.
/// The default value is @b false.
/// @return SUCCESS, if loading is OK; FAILURE otherwise.
OKAY LoadPlugin(const char *name, bool required)
  {
  Assert(name != NULL && strlen(name) > 0);

  // Framework must be initialized already
  if (init_no == 0)
    {
    Assert(false);
    return FAILURE;
    }

  // Save original lists - it will be necessary for correct
  // "rewind" of initializations in case of failure.
  TDict<HMODULE, int> org_loaded_plugins(loaded_plugins);
  GArray<HMODULE> org_inited_plugins(inited_plugins);
  GArray<HMODULE> org_plugins_to_init(plugins_to_init);

  PathStr fn(name);
#ifndef _WIN32
  // For Linux it is necessary to make full library name
  fn = PathStr(fn.Path(), UStr("lib") + fn.BaseName(), ".so");
#endif

  // Try to load DLL
  HMODULE dll = LoadLibrary(fn.Data());
  if (dll == NULL)
    {
    DWORD err = GetLastError();
    Str str_err = Envi::FormatErrorMsg(err);
    if (!required)
      {
      User()->LogMessage("Couldn't load \"%s\" (error code %#x: %s)",
        name, err, str_err.IsEmpty() ? "<no description>" : str_err.Data());
      return SUCCESS;
      }
    User()->ErrorMessage(Tr("Couldn't load \"%s\" (error code %#x: %s)"),
      name, err, str_err.IsEmpty() ? "<no description>" : str_err.Data());
    return FAILURE;
    }

  // If it was somehow loaded already (though
  // may be not inited still), just return.
  int *plug_cnt = loaded_plugins.FindValue(dll);
  if (plug_cnt != NULL)
    {
    (*plug_cnt)++;
    return SUCCESS;
    }

  if (org_inited_plugins.Find(dll) ||
      org_plugins_to_init.Find(dll))
    {
    // This DLL was already loaded implicitly;
    // Add it also to list of explicitly loaded DLLs
    loaded_plugins.Put(dll, 1);
    return SUCCESS;
    }

  // Well, it was not loaded yet
   
  // At least one item must appear in plugins_to_init
  // as a result of LoadLibrary() call.
  if (plugins_to_init.Length() == 0)
    {
    // Perhaps plugin.obj was not linked with this DLL
    Assert(false);
    FreeLibrary(dll);
    if (!required)
      return SUCCESS;
    return FAILURE;
    }

  // DLL was loaded - add it to loaded and exclude from to_be_inited lists
  loaded_plugins.Put(dll, 1);
  int pos;
  if (!plugins_to_init.Find(dll, &pos))
    Assert(false);
  plugins_to_init.Exclude(pos);

  // Additional plugins could be loaded by OS
  // as a result of specified DLL loading.
  // Initialize all of them in order of their loading.
  if (InitAllPlugins() == SUCCESS)
    {
    // Finally initialize specified DLL
    plugins_to_init.Add(dll);
    if (InitAllPlugins() == SUCCESS)
      return SUCCESS;
    }

  // Failure during initialization of some plugin.
  // Terminate plugins which were initialized already during this session
  GArray<HMODULE> plugins_to_term = inited_plugins;
  if (plugins_to_term.Length() > 0)
    {
    plugins_to_term.Exclude(0, org_inited_plugins.Length());
    TermAllPlugins(plugins_to_term);
    }

  // Unload plugins which were loaded already during this session
  for (int i = loaded_plugins.Length() - 1;
       i >= org_loaded_plugins.Length(); i--)
    {
    FreeLibrary(loaded_plugins.GetKey(i));
    }

  // Restore lists to original state
  loaded_plugins = org_loaded_plugins;
  inited_plugins = org_inited_plugins;
  plugins_to_init.Truncate();

  if (!required)
    return SUCCESS;
  return FAILURE;
  }  // LoadPlugin()

//////////////////////////////////////////////////////////////////////////////
/// Notifier about plug-in loading.
///
/// Function notifies BASE library that certain DLL is loaded by system.
/// This function is called from DllMain() in each plug-in DLL.
/// @param[in] dll Plugin (i.e. DLL) handle (assigned by OS).
/// @warning It is forbidden to call this function directly.
void PluginLoading(const HMODULE dll)
  {
  plugins_to_init.Add((HMODULE)dll);
  }

//////////////////////////////////////////////////////////////////////////////
/// Initializes all plugins from init_list.
///
/// Call InitClasses() for all plugins in init_list.
///
/// @return FAILURE, if at least one plugin from the list failed to
/// initialize; SUCCESS otherwise.
OKAY InitAllPlugins()
  {
  HMODULE dll;
  PathStr fn;
  InitClassesType init_proc;
  TermClassesType term_proc;
  VersionIdType   vers_proc;

  while (plugins_to_init.Length() > 0)
    {
    dll = plugins_to_init[0];
    plugins_to_init.Exclude(0);

    // Filename for errors reporting
    fn.GetExecutablePathByHandle(dll);

    // Check for InitClasses()
    init_proc = (InitClassesType)GetProcAddress(dll, "InitClasses");
    if (init_proc == NULL)
      {
      User()->ErrorMessage(Tr("Couldn't load \"%s\": bad format - "
                              "there is no initialization routine"),
                           fn.Data());
      return FAILURE;
      }

    // Check for TermClasses()
    term_proc = (TermClassesType)GetProcAddress(dll, "TermClasses");
    if (term_proc == NULL)
      {
      User()->ErrorMessage(Tr("Couldn't load \"%s\": bad format - "
                              "there is no termination routine"),
                           fn.Data());
      return FAILURE;
      }

    // Check for VersionId()
    vers_proc = (VersionIdType)GetProcAddress(dll, "VersionId");
    if (vers_proc == NULL)
      {
      User()->ErrorMessage(Tr("Couldn't load \"%s\": bad format - "
                              "there is no version information"),
                           fn.Data());
      return FAILURE;
      }

    // Check version
    if (vers_proc() != REPOSITORY_VERSION)
      {
      User()->ErrorMessage(Tr("Couldn't load \"%s\": bad version -"
        " expected %#x, found %#x"), fn.Data(), REPOSITORY_VERSION, vers_proc());
      return FAILURE;
      }

    // Call InitClasses()
    if (init_proc() != SUCCESS)
      {
      User()->ErrorMessage(Tr("Couldn't load \"%s\": initialization failure"),
                           fn.Data());
      return FAILURE;
      }

    inited_plugins.Add(dll);
    } // while()

  return SUCCESS;
  }  // InitAllPlugins()


//////////////////////////////////////////////////////////////////////////////
/// Terminates all plugins.
///
/// Call TermClasses() for plugins. Plugins are terminated in reverse order.
/// @param[in] plugins_to_term List of plugins to be terminated.
void TermAllPlugins(const GArray<HMODULE> &plugins_to_term)
  {
  TermClassesType term_proc;
  for (int i = plugins_to_term.Length() - 1; i >= 0 ; i--)
    {
    term_proc = (TermClassesType)GetProcAddress(plugins_to_term[i],
                                                "TermClasses");
    Assert(term_proc != NULL);
    term_proc();
    }
  }  // TermAllPlugins()

INTEGRA_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////////
// Description of global functions

/**
@fn extern "C" OKAY InitClasses()

Initialize plugin (<b>provided by the user</b>).

This global function must be provided by the user for each plugin (DLL).
It is called once when the plugin (DLL) is loaded.
The function must do all initialization actions.
@warning It is forbidden to call this function directly.
@return SUCCESS, if plugin was successfully initialized,
FAILURE otherwise.  If FAILURE is returned,
the DLL will be immediately unloaded without TermClasses() call.
**/

/**
@fn extern "C" void TermClasses()

Terminate plugin (<b>provided by the user</b>).

This global function must be provided by the user for each plugin (DLL).
It is called once when the plugin is unloaded.
The function should do all termination actions.
@warning It is forbidden to call this function directly.
**/
