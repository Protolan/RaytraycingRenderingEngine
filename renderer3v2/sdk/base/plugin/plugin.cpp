/// @internal
/// @file
///
/// @brief Plugin (DLL) global functions.
///
/// @author Eed - E. Denisov, KLON, '04.07.22.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

// Exclude rarely-used stuff from Windows headers
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else  // Linux
#include <dlfcn.h>
#endif

#include <integra.hpp>
#include <base/plugins.hpp>
#include <base/plugins.hxx>

#ifdef _WIN32

START_C_DECLS
#include <prot.h>
END_C_DECLS

using namespace Integra;

//////////////////////////////////////////////////////////////////////////////
/// @brief DLL Entry point.
///
/// DLL Entry point function that is called by the system to initialize or
/// terminate given DLL.
/// @param[in] dll_handle Handle of DLL being loaded.
/// @param[in] reason The reason for the call:
/// - DLL_PROCESS_ATTACH: when DLL is loaded
/// - DLL_PROCESS_DETACH: when DLL is unloaded
/// @return On loading: TRUE if success, FALSE otherwise.
/// On FALSE, DLL is immediately unloaded.
/// On other reasons the return value is ignored.
BOOL DLLEXPORT APIENTRY DllMain(HINSTANCE dll_handle, DWORD reason, LPVOID)
  {
  if (reason == DLL_PROCESS_ATTACH)
    {
#ifdef DLL_PROT
    if (prot_return("INTDEV", "1.00") == 0)
      return FALSE;
#endif
    PluginLoading(dll_handle);
    }
  return TRUE;
  }
#else // Linux
//////////////////////////////////////////////////////////////////////////////
/// @brief DLL Entry point.
///
/// Function that is called by the system to initialize given DLL.
extern "C" __attribute__((constructor)) BOOL DllMain()
  {
  Dl_info info;
  if (dladdr((const void *)DllMain, &info) != 0)
    {
    HMODULE dll = dlopen(info.dli_fname, RTLD_LAZY);
    if (dll != NULL)
      {
      PluginLoading(dll);
      dlclose(dll);
      }
    }
  return TRUE;
  }
#endif

//////////////////////////////////////////////////////////////////////////////
/// @brief Get version ID.
/// 
/// Get version ID.
/// Function is used for version control during plugin loading.
/// @return Repository version used to create plugin.
int VersionId()
  {
  return REPOSITORY_VERSION;
  }

