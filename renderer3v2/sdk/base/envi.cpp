/// @internal
/// @file
///
/// @brief Definition of Envi class.
///
/// @author Pnd - Nick Derjabin, AT58, '04.03.30.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#ifdef _WIN32
#include <windows.h>
#include <Winuser.h>
#include <dbghelp.h>
#include <direct.h>
#include <gl/gl.h>
#include <Icm.h>
#include <psapi.h>
#include <intrin.h>
#include <comutil.h>
#include <wbemcli.h>
#include <wbemidl.h>
#else
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#endif

#include <integra.hpp>

#include "arrays.hpp"
#include "base.hpp"
#include "dict.hpp"
#include "envi.hpp"
#include "file.hpp"
#include "marray.hpp"
#include "str.hpp"
#include "time.hpp"
#include "tsync.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/// Megabyte in bytes.
static const double MB = 1024 * 1024;

/// Read buffer length for environment variable value.
static const int ENV_BUF_LENGTH = 32767;

/// Critical section handle.
static TSync dump_sync;

/// Find specific separate word in string.
static bool FindWord(const char *str, const char *word);

/// Own handler for unhandled exceptions.
LONG OwnUnhandledExceptionFilter(LPEXCEPTION_POINTERS exc_ptr);

/// Write dump into file.
static void WriteDump(void *exc_inf);

/**
@class IntMemoryException base/envi.hpp

Memory allocation error exception.

**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
/// @param[in] s Size of memory block which allocation failed.
IntMemoryException::IntMemoryException(INT64 s)
  {
  size = s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src Source object.
IntMemoryException::IntMemoryException(const IntMemoryException &src)
  {
  size = src.size;
  }

//////////////////////////////////////////////////////////////////////////////
/// Cast to INT64 type.
/// @return Size of memory block which allocation failed.
IntMemoryException::operator INT64() const
  {
  return size;
  }

/**
@class Envi base/envi.hpp

Singleton class providing various system specific features.

This implementation is for the WIN32 platform.

@sa @ref base_mainpage
**/


/////////////////////////////////////////////////////////////////////////////
/// Convert bytes to MBytes.
/// @param[in] bytes - The number of bytes to convert.
/// @result The value converted to MegaBytes.
/// @note The value is rounded up to the nearest MB value.
int Envi::Bytes2MBytes(SIZE_T bytes)
  {
  return (int)((bytes + MB / 2) / MB);
  }

/////////////////////////////////////////////////////////////////////////////
/// Convert bytes to user-readable string.
/// @param[in] bytes Number of bytes.
/// @return String in format with 1 decimal sign.
UStr Envi::Bytes2HumanReadable(INT64 bytes)
  {
  if (bytes < 0)
    return NULL;
  UStr ret_str;
  if (bytes < 1024)
    ret_str.Printf("%.1f%s", (double)bytes, Tr(" bytes"));
  else if (bytes < MB)
    ret_str.Printf("%.1f%s", ((double)bytes + 1024.0 / 2.0) / 1024.0, Tr(" KB"));
  else if (bytes < MB * 1024)
    ret_str.Printf("%.1f%s", ((double)bytes + MB / 2.0) / MB, Tr(" MB"));
  else if (bytes < MB * MB)
    ret_str.Printf("%.1f%s", ((double)bytes + MB * 1024 / 2.0) / (MB * 1024), Tr(" GB"));
  else
    ret_str.Printf("%.1f%s", ((double)bytes + MB * MB / 2.0) / (MB * MB), Tr(" TB"));
  return ret_str;
  }

/////////////////////////////////////////////////////////////////////////////
/// Get various memory statistics for current process.
///
/// All returned memory amounts are in megabytes. Pass NULL for unnecessary argument(s).
/// @param[out] used - Amount of virtual memory, currently used (committed) by a process.
/// @param[out] limit - Total amount of virtual memory, available to a process.
/// @param[out] phys_free - Free amount of physical memory, available to a process.
/// @param[out] phys_limit - Total amount of physical memory, available to a process.
/// @param[out] peak_ws - Peak working set value during the lifetime of a process.
/// @param[out] peak_pf - Peak value of the commit charge during the lifetime of a process.
void Envi::GetMemSize(int *used, int *limit, int *phys_free, int *phys_limit, int *peak_ws, int *peak_pf)
  {
#ifndef INTEXPORT_LTKERNEL // Below function is not available in VC 6.0
#ifdef _WIN32
  if (used != NULL || limit != NULL || phys_free != NULL || phys_limit != NULL)
    {
    MEMORYSTATUSEX stat;
    stat.dwLength = sizeof(stat);
    if (GlobalMemoryStatusEx(&stat) != 0)
      {
      if (used != NULL)
        *used = Bytes2MBytes((SIZE_T)(stat.ullTotalVirtual - stat.ullAvailVirtual));
      if (limit != NULL)
        *limit = Bytes2MBytes((SIZE_T)Min(stat.ullTotalPageFile, stat.ullTotalVirtual));
      if (phys_free != NULL)
        *phys_free = Bytes2MBytes((SIZE_T)Min(stat.ullAvailPhys, stat.ullAvailVirtual));
      if (phys_limit != NULL)
        *phys_limit = Bytes2MBytes((SIZE_T)Min(stat.ullTotalPhys, stat.ullAvailVirtual));
      }
    else
      {
      if (used != NULL)
        *used = 0;
      if (limit != NULL)
        *limit = 0;
      if (phys_free != NULL)
        *phys_free = 0;
      if (phys_limit != NULL)
        *phys_limit = 0;
      }
    }
  if (peak_ws != NULL || peak_pf != NULL)
    {
    PROCESS_MEMORY_COUNTERS counters;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters)) != 0)
      {
      if (peak_ws != NULL)
        *peak_ws = Bytes2MBytes(counters.PeakWorkingSetSize);
      if (peak_pf != NULL)
        *peak_pf = Bytes2MBytes(counters.PeakPagefileUsage);
      }
    else
      {
      if (peak_ws != NULL)
        *peak_ws = 0;
      if (peak_pf != NULL)
        *peak_pf = 0;
      }
    }
#else
  struct sysinfo sinfo;
  if (sysinfo(&sinfo) == 0)
    {
    SIZE_T total = sinfo.totalram + sinfo.totalswap;
    if (used != NULL)
      *used = Bytes2MBytes((total - sinfo.freeram - sinfo.freeswap) *
                           sinfo.mem_unit);
    if (limit != NULL)
      *limit = Bytes2MBytes(total * sinfo.mem_unit);
    if (phys_free != NULL)
      *phys_free = Bytes2MBytes(sinfo.freeram * sinfo.mem_unit);
    if (phys_limit != NULL)
      *phys_limit = Bytes2MBytes(sinfo.totalram * sinfo.mem_unit);
    }
  else
    {
    if (used != NULL)
      *used = 0;
    if (limit != NULL)
      *limit = 0;
    if (phys_free != NULL)
      *phys_free = 0;
    if (phys_limit != NULL)
      *phys_limit = 0;
    }
#error Not implemented yet
  if (peak_ws != NULL)
    *peak_ws = 0;
  if (peak_pf != NULL)
    *peak_pf = 0;
#endif
#else // INTEXPORT_LTKERNEL
  if (used != NULL)
    *used = 0;
  if (limit != NULL)
    *limit = 0;
  if (phys_free != NULL)
    *phys_free = 0;
  if (phys_limit != NULL)
    *phys_limit = 0;
  if (peak_ws != NULL)
    *peak_ws = 0;
  if (peak_pf != NULL)
    *peak_pf = 0;
#endif
  }  // GetMemSize()

/////////////////////////////////////////////////////////////////////////////
/// Get size of the largest available memory block.
/// @return Size of the largest available memory block,
/// in bytes, with 1K precision.
SIZE_T Envi::AvailMemBlockSize()
  {
  TArray<DWORD> arr;
  SIZE_T mini = 0, maxi = 2047 * 1024 * 1024;
  while (maxi - mini > 1)
    {
    SIZE_T cur = (mini + maxi) / 2;
    if (arr.Allocate((int)(cur / 4)) == SUCCESS)
      {
      arr.Resize();
      mini = cur;
      }
    else
      {
      maxi = cur;
      }
    }
  return mini;
  }  // AvailMemBlockSize()

//////////////////////////////////////////////////////////////////////////////
/// Process environment read access.
///
/// Get a value of an environment variable. Both environment variables
/// defined when the process was started and those set later by
/// the PutEnv() method can be accessed.
/// @param[in] name - A name of the variable to get.
/// @return A value of this variable, or an empty string if the variable 
/// is not defined in the local environment.
Str Envi::GetEnv(const char *name)
  {
  // valid name string required
  Assert(name != NULL);

  // environment variable value read buffer
  char *env_buf = new char[ENV_BUF_LENGTH + 1];
  if (env_buf == NULL)
    return NULL;

#ifdef _WIN32
  if (GetEnvironmentVariable(name, env_buf, ENV_BUF_LENGTH) == 0)
    env_buf[0] = '\0';
#else
  const char *env = getenv(name);
  if (env == NULL)
    env_buf[0] = '\0';
  else
    strncpy(env_buf, env, ENV_BUF_LENGTH);
#endif

  Str str_out(env_buf);
  delete[] env_buf;
  return str_out;
  }  // GetEnv()

//////////////////////////////////////////////////////////////////////////////
/// Process environment write access.
///
/// The method adds the environment variable @a name to the local
/// environment string table. If there was already such a variable
/// defined in the environment table, its old value is over-written with
/// the new one. If @a value is NULL, the variable is deleted from the process
/// environment.
/// @note If @a value == "" (an emtpy string), the effect is the same
/// as if @a value == NULL, i.e. the variable is deleted.
/// @param[in] name - A variable name string.
/// @param[in] value A variable value string or NULL. 
/// @return FAILURE if the variable could not be set, e.g. the functionality 
/// is not supported.
OKAY Envi::PutEnv(const char *name, const char *value)
  {
  // valid name string required
  Assert(name != NULL);

  const char *set_val = value;
  if (set_val != NULL && set_val[0] == '\0')
    set_val = NULL;
#ifdef _WIN32
  if (SetEnvironmentVariable(name, set_val))
    return SUCCESS;
#else
  if (set_val == NULL)
    {
    unsetenv(name); 
    return SUCCESS;
    }
  if (setenv(name, set_val, true) == 0)
    return SUCCESS;
#endif

  return FAILURE;
  }  // PutEnv()

/// Separator of field name and value.
const char ENV_DEF = ':';
/// Field definition separator.
const char ENV_SEP = ',';
/// String separator (optional).
const char ENV_STR = '"';
//////////////////////////////////////////////////////////////////////////////
/// Get 'value' of 'name' field from 'env' string.
/// @param[in] env Environment string.
/// @param[in] name Field name.
/// @return 'value' string.
/// @internal 
/// @note Borrowed from ievl.c.
Str Envi::GetFrom(const Str &env, const Str &name)
  {
  // Current position in 'env' string.
  const char *ee;
  // Current position in 'name' string.
  const char *nn;

  if (env.IsEmpty() || name.IsEmpty())
    return Str("");

  // Place for extracted string.
  char *buf = new char[ENV_BUF_LENGTH + 1];
  if (buf == NULL)
    return NULL;

  ee = env.Data();
  for ( ; ; )
    {
    // Return NULL when config empty.
    if (*ee == '\0')
      {
      delete[] buf;
      return NULL;
      }

    // Find field name in config.
    for (nn = name.Data(); *nn != '\0' && (toupper(*ee) == toupper(*nn)); nn++)
      ee++;

    // If not match.
    if (*nn != '\0' || *ee != ENV_DEF)
      {
      // Skip to end of var/field, or begin of string value.
      while (*ee != ENV_SEP && *ee != ENV_STR && *ee != '\0')
        ee++;

      // Skip ENV_SEP character.
      if (*ee == ENV_SEP)
        ee++;
      else if (*ee == ENV_STR)
        // Skip field value of string type (limited by ENV_STR).
        {
        // Skip beginning ENV_STR character.
        ee++;

        // Skip string between ENV_STR characters.
        while (*ee != ENV_STR && *ee != '\0')
          ee++;

        // Skip ending ENV_STR character.
        if (*ee == ENV_STR)
          ee++;
        }
      }
    else
      // String match OK.
      {
      char *b;

      // Skip ENV_DEF character.
      Assert(*ee == ENV_DEF);
        ee++;

      // If string between ENV_STR chars.
      if (*ee == ENV_STR)
        {
        // Skip beginning ENV_STR character.
        ee++;

        // Copy till ending ENV_STR.
        b = buf;
        while (*ee != ENV_STR && *ee != '\0' && ((b - buf) < ENV_BUF_LENGTH))
          *b++ = *ee++;

        // Skip ending ENV_STR character.
        if (*ee == ENV_STR)
          ee++;
        }
      else
        {
        // Copy value till end of field/var.
        b = buf;
        while (*ee != ENV_SEP && *ee != '\0' && ((b - buf) < ENV_BUF_LENGTH))
          *b++ = *ee++;
        }

      // Mark end of string in 'buf'.
      *b = '\0';

      Str str_out(buf);
      delete[] buf;
      return str_out;
      }
    }
  }  // GetFrom()

//////////////////////////////////////////////////////////////////////////////
/// Get integer from environment string for given field.
/// Function tries to retrieve a value string for given field,
/// and converts it into  integer  value - which is returned to
/// the caller. If 'name' field is not defined (or conversion
/// failed), a default value is returned.
/// @param[in] env Environment string.
/// @param[in] name String describing name of the field.
/// @param[in] def_val Default value that is used in case
///                    of 'name' is not defined.
/// @return Value for given option field.
/// @internal 
/// @note Borrowed from ievl.c.
int Envi::GetInt(const Str &env, const Str &name, int def_val)
  {
  if (env.IsEmpty())
    return def_val;

  Str buf = GetFrom(env, name);

  if (buf.IsEmpty())  // || env_atoi(v, &i) != 0)
    return def_val;
  int res = 0;
  if (sscanf(buf.Data(), "%d", &res) != 1)
    return def_val;
  return res;
  }  // GetInt()

//////////////////////////////////////////////////////////////////////////////
/// Allocation of aligned block.
///
/// Method allocates memory block with specified alignment.
/// After usage memory must be deallocated using AlignedFree() method.
/// @param[in] size Required size of memory block, in bytes.
/// @param[in] align Required alignement.
/// @return Pointer to allocated block or NULL if no more memory.
void *Envi::AlignedMalloc(SIZE_T size, int align)
  {
  void *ptr, *retptr;

  if ((align & (align - 1)) != 0)
    {
    Assert(false);
    return NULL;
    }

  align = ((unsigned)align > sizeof(SIZE_T *) ? align : sizeof(SIZE_T *)) - 1;

  ptr = malloc(sizeof(void *) + align + size);
  if (ptr == NULL)
    {
    Assert(false);
    return NULL;
    }

  retptr = (void *)(((SIZE_T)ptr + sizeof(SIZE_T *) + align) & ~align);
  ((SIZE_T *)retptr)[-1] = (SIZE_T)ptr;
  Assert(retptr > ptr);
  Assert(&((SIZE_T *)retptr)[-1] >= ptr);

  return (void *)retptr;
  }  // AlignedMalloc()

//////////////////////////////////////////////////////////////////////////////
/// Deallocation of aligned block.
///
/// Method deallocates aligned memory block
/// which was allocated by AlignedMalloc().
/// @param[in] ptr Pointer to memory block to deallocate.
void Envi::AlignedFree(void *ptr)
  {
  if (ptr == NULL)
    return;

  void *p = (void *)(((SIZE_T *)ptr)[-1]);
  Assert(ptr > p);

  free(p);
  }

//////////////////////////////////////////////////////////////////////////////
/// Pointer to really allocated block.
void *Envi::AlignedMemBlockPtr(void *ptr)
  {
  if (ptr == NULL)
    return NULL;

  void *p = (void *)(((SIZE_T *)ptr)[-1]);
  //Assert(ptr > p);
  if (ptr <= p) // is already broken
    return NULL;

  return p;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current working directory.
///
/// @return Resulting path to the current working directory.
///
/// All failures are asserted.
///
PathStr Envi::CurDir()
  {
  CHAR buffer[Str::MAX_LENGTH + 1];
  // Receive CWD by getcwd()
  if (getcwd(buffer, Str::MAX_LENGTH) == NULL)
    {
    Assert(false);  // getcwd() error
    buffer[0] = '\0';
    }
  return PathStr(buffer);
  }  // CurDir()

//////////////////////////////////////////////////////////////////////////////
/// Set current path to given location.
///
/// @param[in] path The folder path to set.
/// @return SUCCESS or FAILURE.
///
/// If non-NULL path is given, the current working directory is set to this path
/// (that must be an existing folder path).
/// All errors are reported with FAILURE returned.
///
/// As a special case, the @c path may be NULL (omitted).
/// Then the current path is set to some default location (Users/Public/Documents/Integra).
/// In this special case, errors are asserted but not reported, and the method returns SUCCESS always.
///
OKAY Envi::ChDir(const PathStr &path)
  {
  if (path.IsNull())
    {
    // Set default current path
    PathStr def_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_DOCUMENTS, PathStr::SITYPE_CURRENT);
    if (def_path.IsEmpty() || !def_path.IsDir())
      def_path = PathStr::GetSIFolder(PathStr::SIFOLDER_COMMON_DOCUMENTS, PathStr::SITYPE_DEFAULT);
    if (def_path.IsEmpty() || !def_path.IsDir())
      {
      Assert(false);  // Can't get system path
      return SUCCESS;
      }
    def_path = PathStr(def_path, "Integra");
    if (!def_path.IsDir())
      def_path.MakeDir();
    if (chdir(def_path.Data()) != 0)
      Assert(false);  // chdir() error
    return SUCCESS;
    }
  // Check that the folder exists
  Assert(path.IsDir());
  if (!path.IsDir())
    return User()->ErrorMessage(Tr("Folder '%s' doesn't exist"), path.Data());
  // Change Current Working Directory by chdir()
  if (chdir(path.Data()) != 0)
    return User()->ErrorMessage(Tr("Folder '%s' isn't accessible"), path.Data());
  return SUCCESS;
  }  // ChDir()

//////////////////////////////////////////////////////////////////////////////
/// Get the Windows directory.
///
/// Method returns path to the Windows directory (typically "C:\Windows").
/// @return Resulting path to the Windows directory.
PathStr Envi::WinDir()
  {
  CHAR buffer[Str::MAX_LENGTH + 1];
#ifdef _WIN32
  if (GetWindowsDirectory(buffer, Str::MAX_LENGTH) == 0)
    {
    // Error in GetWindowsDirectory()
    Assert(false);
    buffer[0] = '\0';
    }
#else
  buffer[0] = '\0';
#endif
  return PathStr(buffer);
  }  // WinDir()

//////////////////////////////////////////////////////////////////////////////
/// Get the Windows system directory.
///
/// Method returns path to the Windows directory (typically "C:\Windows\System32").
/// @return Resulting path to the Windows system directory.
PathStr Envi::SysDir()
  {
  CHAR buffer[Str::MAX_LENGTH + 1];
#ifdef _WIN32
  if (GetSystemDirectory(buffer, Str::MAX_LENGTH) == 0)
    {
    // Error in GetSystemDirectory()
    Assert(false);
    buffer[0] = '\0';
    }
#else
  buffer[0] = '\0';
#endif
  return PathStr(buffer);
  }  // SysDir()

//////////////////////////////////////////////////////////////////////////////
/// Get description of system error.
///
/// Method returns error message corresponding to system error code.
/// Typically should be applied to the result of GetLastError() call.
/// @param[in] error Error code.
/// @return String describing the error for given code. In case of failure
/// an empty string is returned.
Str Envi::FormatErrorMsg(unsigned error)
  {
  Str ret = Str("");
#ifdef _WIN32
  char *buf = NULL;
  if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error, 0, (LPTSTR)&buf, 0, NULL) != 0)
    {
    ret = Str(buf);
    LocalFree(buf);
    ret.Clip();
    }
#endif  
  return ret;
  }  // FormatErrorMsg()

//////////////////////////////////////////////////////////////////////////////
/// Check for Intel video.
///
/// Method returns true, if Intel videocard is detected as a display
/// device attached to desktop.
/// @return True if Intel videocard is detected.
bool Envi::IsIntelVideo()
  {
#ifdef _WIN32
  // Try OpenGL first
  char *p = (char *)glGetString(GL_VENDOR);
  if (p != NULL)
    return FindWord(p, "Intel");
#endif

#if defined(_WIN32) && (_MSC_VER >= 1400)
  // Use Win32 GDI (less reliable)
  DISPLAY_DEVICE disp_dev;
  for (int i = 0; ; i++)
    {
    disp_dev.cb = sizeof(DISPLAY_DEVICE);
    if (EnumDisplayDevices(NULL, i, &disp_dev, 0) == 0)
      break;
    if (disp_dev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
      if (FindWord(disp_dev.DeviceString, "Intel"))
        return true;
    }
#endif

  return false;
  }  // IsIntelVideo()

//////////////////////////////////////////////////////////////////////////////
/// Get standard windows monitor profile pathname.
/// @param[out] icm_pathname Pathname of the ICM file or empty path.
/// @return True, if pathname is found, false otherwise.
/// @note If request LCS_sRGB - sRGB will be returned.
bool Envi::GetWindowsMonitorICC(PathStr &icm_pathname)
  {
  DWORD pdw_size = 0;
  BOOL res = GetStandardColorSpaceProfile(NULL, LCS_WINDOWS_COLOR_SPACE , NULL, &pdw_size);
  if (pdw_size <= 1)
    return false;
  char *buf = new char[pdw_size];
  res = GetStandardColorSpaceProfile(NULL, LCS_WINDOWS_COLOR_SPACE, buf, &pdw_size);
  if (res)
    icm_pathname = PathStr(buf);
  delete [] buf;
  if (res)
    return true;
  return false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get monitor profile pathname for the current display, specified by user.
/// @param[out] icm_pathname Pathname of the ICM file or empty path.
/// @return True, if pathname is found, false otherwise.
bool Envi::GetMonitorICC(PathStr &icm_pathname)
  {
  DISPLAY_DEVICE disp_dev; 
  char device_name[33];	// 32 + 1 for the null-terminator
  char *disp_name = NULL;
  TArray<unsigned short> w_disp_name;

  ZeroMemory(&disp_dev, sizeof(disp_dev));
  disp_dev.cb = sizeof(disp_dev); 
  int disp_ind = 0;
    
  // After the first call to EnumDisplayDevices, 
  // disp_dev.DeviceString is the adapter name.
  if (!EnumDisplayDevices(NULL, disp_ind, &disp_dev, 0)) 
    return false;

	strncpy(device_name, disp_dev.DeviceName, 32);
      
  // After second call, disp_dev.DeviceString is the 
  // monitor name for that device.
  if (!EnumDisplayDevices(device_name, disp_ind, &disp_dev, EDD_GET_DEVICE_INTERFACE_NAME))
    return false;

  disp_name = disp_dev.DeviceKey;    
  UStr::ToUnicode(disp_name, w_disp_name); 
  DWORD buf_sz;
  BOOL ret = WcsGetDefaultColorProfileSize(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER, (PCWSTR)w_disp_name.Data(),
               CPT_ICC, CPST_RGB_WORKING_SPACE, 1, &buf_sz);
  if (!ret)
    return false;

  TArray<unsigned short> buf;
  buf.Allocate(buf_sz);
  buf.Set(0);

  ret = WcsGetDefaultColorProfile(WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER, (PCWSTR)w_disp_name.Data(), 
               CPT_ICC, CPST_RGB_WORKING_SPACE, 1, buf_sz, (LPWSTR)buf.Data());
  if (!ret)
    return false;
  char path_buf[201];
  buf_sz = 200;
  ret = GetColorDirectory(NULL, path_buf, &buf_sz);
  if (!ret)
    return false;

  UStr icm_name;  
  icm_name.FromUnicode(buf.Data());
  icm_pathname = PathStr(path_buf, icm_name);

  return true;
  }  // GetMonitorICC()

//////////////////////////////////////////////////////////////////////////////
/// Report hardware details to the log file.
///
/// Method reports hardware details (such as CPU, videocards, OS version)
/// to the log file.
void Envi::LogHardware()
  {
  Str res;

  if (GetCPU(res) == SUCCESS)
    User()->LogMessage("CPU: %s", res.Data());
  else
    User()->LogMessage("Retrieval of CPU model is not supported by CPU");

  TArray<Str> videos;
  if (GetVideo(videos) == SUCCESS)
    for (int i = 0; i < videos.Length(); i++)
      User()->LogMessage("Videocard: %s", videos[i].Data());
  else
    User()->LogMessage("Retrieval of videocard information failed");

  if (GetOS(res) == SUCCESS)
    User()->LogMessage("OS: %s", res.Data());
  else
    User()->LogMessage("Retrieval of OS information failed");
  }  // LogHardware()

//////////////////////////////////////////////////////////////////////////////
/// Get CPU model.
/// @param[out] res String containing CPU model as returned by cpuid().
/// @return FAILURE if retrieval of CPU model failed; SUCCESS otherwise.
OKAY Envi::GetCPU(Str &res)
  {
  res = NULL;
  // Report CPU model
  int cpu_info[4] = {0, 0, 0, 0};
  char cpu_string[48];
  __cpuid(cpu_info, 0x80000000);
  if ((unsigned)cpu_info[0] >= 0x80000004)
    {
    __cpuid(cpu_info, 0x80000002);
    memcpy(cpu_string, cpu_info, sizeof(cpu_info));
    __cpuid(cpu_info, 0x80000003);
    memcpy(cpu_string + 16, cpu_info, sizeof(cpu_info));
    __cpuid(cpu_info, 0x80000004);
    memcpy(cpu_string + 32, cpu_info, sizeof(cpu_info));
    res = cpu_string;
    return SUCCESS;
    }
  return FAILURE;
  }  // GetCPU()

//////////////////////////////////////////////////////////////////////////////
/// Get videocard(s) details.
///
/// @param[out] videos Array of strings, one string per videocard. Primary,
/// mirroring and attached to desktop devices are marked (in the end of string).
/// @return FAILURE if videocard information could not be retrieved;
///         SUCCESS otherwise.
OKAY Envi::GetVideo(TArray<Str> &videos)
  {
  videos.Truncate();
#ifdef _WIN32
  // Find specific flags for all videocards by their names
  GArray<UStr> primary, desktop, mirroring;
  int i = 0;
  DISPLAY_DEVICE dd;
  for ( ; ; )
    {
    dd.cb = sizeof(DISPLAY_DEVICE);
    if (EnumDisplayDevices(NULL, i++, &dd, 0) == 0)
      break;
    if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
      primary.Add(dd.DeviceString);
    if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
      desktop.Add(dd.DeviceString);
    if (dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
      mirroring.Add(dd.DeviceString);
    }

  // Report videocards with driver details via WMI
  HRESULT hr;
  hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); // COINIT_MULTITHREADED);
  if (FAILED(hr))
    {
    User()->LogMessage("Envi::GetVideo(): CoInitializeEx() failed");
    return FAILURE;
    }

  IWbemLocator *pIWbemLocator = NULL;
  hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER,
                        __uuidof(IWbemLocator), (LPVOID *) &pIWbemLocator);
  if (FAILED(hr))
    {
    User()->LogMessage("Envi::GetVideo(): CoCreateInstance() failed");
    CoUninitialize();
    return FAILURE;
    }

  IWbemServices *pIWbemServices;
  BSTR bstrServer = SysAllocString(L"\\\\.\\root\\cimv2");
  hr = pIWbemLocator->ConnectServer(bstrServer, NULL, NULL, 0L, 0L,
                                    NULL, NULL, &pIWbemServices);
  SysFreeString(bstrServer);
  if (FAILED(hr))
    {
    User()->LogMessage("Envi::GetVideo(): ConnectServer() failed");
    CoUninitialize();
    return FAILURE;
    }

  hr = CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                         NULL, RPC_C_AUTHN_LEVEL_CALL,
                         RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_DEFAULT);
  if (FAILED(hr))
    {
    User()->LogMessage("Envi::GetVideo(): CoSetProxyBlanket() failed");
    pIWbemServices->Release();
    CoUninitialize();
    return FAILURE;
    }

  IEnumWbemClassObject *pEnum;
  BSTR bstrWQL  = SysAllocString(L"WQL");
  BSTR bstrPath = SysAllocString(L"select * from Win32_VideoController");
  hr = pIWbemServices->ExecQuery(bstrWQL, bstrPath, WBEM_FLAG_FORWARD_ONLY,
                                 NULL, &pEnum);
  SysFreeString(bstrWQL);
  SysFreeString(bstrPath);
  if (FAILED(hr))
    {
    User()->LogMessage("Envi::GetVideo(): ExecQuery() failed");
    pIWbemServices->Release();
    CoUninitialize();
    return FAILURE;
    }
  IWbemClassObject *pObj = NULL;
  ULONG uReturned;
  VARIANT var;
  for ( ; ; )
    {
    Str card, name, s;
    hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &uReturned);
    if (FAILED(hr) || uReturned != 1)
      break;
    hr = pObj->Get(L"Name", 0, &var, NULL, NULL);
    s.Printf("%S", SUCCEEDED(hr) ? var.bstrVal : L"unknown");
    name = s;
    card = s;
    hr = pObj->Get(L"DriverVersion", 0, &var, NULL, NULL);
    s.Printf("; driver version: %S", SUCCEEDED(hr) ? var.bstrVal : L"unknown");
    card += s;
    hr = pObj->Get(L"DriverDate", 0, &var, NULL, NULL);
    if(SUCCEEDED(hr))
      s.Printf(" (%.4S.%.2S.%.2S)", var.bstrVal, var.bstrVal + 4, var.bstrVal + 6);
    else
      s.Printf(" (unknown)");
    card += s;
    pObj->Release();
    if (primary.Find(name))
      card += UStr(", primary");
    if (desktop.Find(name))
      card += UStr(", desktop");
    if (mirroring.Find(name))
      card += UStr(", mirroring");
    videos.Add(card);
    }
  pEnum->Release();
  pIWbemServices->Release();
  CoUninitialize();
#endif
  return videos.Length() > 0 ? SUCCESS : FAILURE;
  }  // GetVideo()

//////////////////////////////////////////////////////////////////////////////
/// Get OS version.
/// @param[out] res String containing OS version.
/// @return FAILURE if OS information could not be retrieved;
///         SUCCESS otherwise.
OKAY Envi::GetOS(Str &res)
  {
  res = NULL;
#ifdef _WIN32
  PathStr tmpd;
  if (tmpd.MkTempDir() != SUCCESS)
    return FAILURE;
  PathStr cmd("cmd"), tmpf(tmpd, "ver_out", "txt");
  const CHAR *args[] = { "/c", "ver", ">", tmpf.Data(), NULL };
  if (cmd.Run(args, true, PathStr::NO_CONSOLE) == SUCCESS)
    {
    File f(tmpf);
    if (f.Open("r") == SUCCESS)
      {
      Str s;
      while (f.ReadStr(s) == SUCCESS)
        res += s;
      f.Close();
      res.Clip();
      }
    }
  tmpd.Remove();
#endif
  return res.IsEmpty() ? FAILURE : SUCCESS;
  }  // GetOS()

//////////////////////////////////////////////////////////////////////////////
/// Issue audio signal - beep.
void Envi::Beep()
  {
  //cout << '\a';
  printf("\a");
  }

//////////////////////////////////////////////////////////////////////////////
/// Set own filter for unhandled exceptions.
///
/// Own filter creates MiniDump file in log folder.
void Envi::SetOwnExceptionFilter()
  {
  SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)OwnUnhandledExceptionFilter);
  }

//////////////////////////////////////////////////////////////////////////////
/// Find specific separate word in string. Case insensitive.
/// @param[in] str String to search word in.
/// @param[in] word Word to look for in str. Only whole word is searched;
///                 it is not recognized as a part of bigger word.
/// @return True if found.
static bool FindWord(const char *str, const char *word)
  {
  for (const char *p = str; *p != '\0'; p++)
    {
    if (strnicmp(p, word, strlen(word)) == 0 &&
        (p == str || !isalpha(p[-1])) &&
        !isalpha(p[strlen(word)]))
      return true;
    }
  return false;
  }  // FindWord()

/// Pointer type for TermLic() function.
typedef void (*TermLicType)();

//////////////////////////////////////////////////////////////////////////////
/// Own handler for unhandled exceptions.
/// @param[in] exc_ptr Pointer to exception information.
/// @return Value expected from UnhandledExceptionFilter function.
LONG OwnUnhandledExceptionFilter(LPEXCEPTION_POINTERS exc_ptr)
  {
#ifdef _WIN32
  dump_sync.Mono();
  static bool dump_created = false;
  if (dump_created)
    {
    dump_sync.Multi();
    return EXCEPTION_CONTINUE_SEARCH;
    }
  dump_created = true;

  MINIDUMP_EXCEPTION_INFORMATION exc_inf;
  exc_inf.ThreadId = GetCurrentThreadId();
  exc_inf.ExceptionPointers = exc_ptr;
  exc_inf.ClientPointers = FALSE;

  WriteDump(&exc_inf);

  // Close license for sure;
  // avoid linkage of imagetools, find necessary function
  TermLicType term_lic;
  HMODULE dll = GetModuleHandle("imagetools.dll");
  if (dll != NULL)
    {
    term_lic = (TermLicType)GetProcAddress(dll, "?TermLic@License@@SAXXZ");
    if (term_lic != NULL)
      term_lic();
    else
      User()->LogMessage("Unable to find TermLic() entry point");
    }

  dump_sync.Multi();
  return EXCEPTION_CONTINUE_SEARCH;
#else
  return 0;
#endif
  }  // OwnUnhandledExceptionFilter()

//////////////////////////////////////////////////////////////////////////////
/// Write dump into file.
///
/// Method writes dump file into log files directory.
/// Name of the file is the same as used for log file.
/// @param[in] exc_inf Pointer to exception information.
static void WriteDump(void *exc_inf)
  {
#ifdef _WIN32
  const int flags = MiniDumpWithThreadInfo | MiniDumpWithDataSegs;

  // To ensure log existance
  User()->SwitchLogFileOn();
  // Get log filename
  PathStr fname = User()->GetLogFilePath();
  fname.SetExtension("dmp");

  HANDLE hfile = CreateFile(fname.Data(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hfile != INVALID_HANDLE_VALUE)
    {
    HANDLE proc = GetCurrentProcess();
    if (MiniDumpWriteDump(proc, GetProcessId(proc), hfile, (MINIDUMP_TYPE)flags,
                          (PMINIDUMP_EXCEPTION_INFORMATION)exc_inf, NULL, NULL))
      {
      User()->LogMessage("Unhandled exception; dump file created");

      // Remove obsolete *.dmp files, leave only 3 latest
      int i;
      TArray<PathStr> fnames;
      MArray<unsigned> ftimes;
      TDict<unsigned, PathStr> fdict;

      PathStr dir(fname.Path());
      PathStr mask("*.dmp");
      dir.ListFileNames(fnames, &mask);
      for (i = 0; i < fnames.Length(); i++)
        {
        PathStr f(dir, fnames[i]);
        Time t(f.FileTime());
        fdict.Put((unsigned)t, f);
        ftimes.Add((unsigned)t);
        }
      ftimes.QSort();
      unsigned limit = ftimes.Length() > 3 ? ftimes[ftimes.Length() - 3]: 0;
      for (i = 0; i < fdict.Length(); i++)
        if (fdict.GetKey(i) < limit && fdict.GetValue(i) != fname)
          {
          File f(fdict.GetValue(i));
          f.Remove();
          }
      }
    else
      {
      DWORD err = GetLastError();
      Str str_err = Envi::FormatErrorMsg(err);
      User()->LogMessage("Unhandled exception; dump file creation failed (error code %#x: %s)",
        err, str_err.IsEmpty() ? "<no description>" : str_err.Data());
      }
    CloseHandle(hfile);
    }

  // To ensure report of used memory in log
  User()->SwitchLogFileOff();
#endif
  }  // WriteDump()

INTEGRA_NAMESPACE_END
