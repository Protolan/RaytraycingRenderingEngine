/// @file
///
/// @brief Declaration of Envi class.
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @internal
/// @author Pnd - Nick Derjabin, AT58, '04.03.30.

#ifndef _AT58_ENVI_HPP_
#define _AT58_ENVI_HPP_

#include "export.h"
#include "arrays.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Memory allocation error exception.
class IntMemoryException
  {
  public:
    /// Constructor.
    INTAPI_BASE IntMemoryException(INT64 size);
    /// Copy constructor.
    INTAPI_BASE IntMemoryException(const IntMemoryException &src);
    /// Cast to INT64 type.
    INTAPI_BASE operator INT64 () const;
  private:
    /// Size of memory block which allocation failed.
    INT64 size;
  };  // class IntMemoryException

/// Singleton class providing various system specific features.
class Envi
  {
  public:
    /// @name Queries for memory size
    //@{
    /// Get various memory statistics for current process.
    INTAPI_BASE static void GetMemSize(int *used, int *limit,
      int *phys_free, int *phys_limit, int *peak_ws, int *peak_pf);
    /// Get size of the largest available memory block.
    INTAPI_BASE static SIZE_T AvailMemBlockSize();
    /// Convert bytes to MBytes.
    INTAPI_BASE static int Bytes2MBytes(SIZE_T bytes);
    /// Convert bytes to user-readable string.
    INTAPI_BASE static UStr Bytes2HumanReadable(INT64 bytes);
    //@}

  public:
    /// @name Process environment access
    //@{
    /// Process environment read access.
    INTAPI_BASE static Str GetEnv(const char *name);
    /// Process environment write access.
    INTAPI_BASE static OKAY PutEnv(const char *name, const char *value);
    /// Get 'value' of 'name' field from 'env' string.
    INTAPI_BASE static Str GetFrom(const Str &env, const Str &name);
    /// Get integer from environment string for given field.
    INTAPI_BASE static int GetInt(const Str &env, const Str &name, 
      int def_val);
    //@}

  public:
    /// @name Memory allocation / deallocation
    //@{
    /// Allocation of aligned block.
    INTAPI_BASE static void *AlignedMalloc(SIZE_T size, int align);
    /// Deallocation of aligned block.
    INTAPI_BASE static void AlignedFree(void *ptr);
    /// Pointer to really allocated block.
    INTAPI_BASE static void *AlignedMemBlockPtr(void *ptr);
    //@}

  public:
    /// @name Directories processing
    //@{
    /// Get the current working directory.
    INTAPI_BASE static PathStr CurDir();
    /// Set current path.
    INTAPI_BASE static OKAY ChDir(const PathStr &path = NULL);
    /// Get the Windows directory.
    INTAPI_BASE static PathStr WinDir();
    /// Get the Windows system directory.
    INTAPI_BASE static PathStr SysDir();
    //@}

  public:
    /// @name Errors processing
    //@{
    /// Get description of system error.
    INTAPI_BASE static Str FormatErrorMsg(unsigned error);
    //@}

  public:
    /// @name Hardware specifics
    //@{
    /// Check for Intel video.
    INTAPI_BASE static bool IsIntelVideo();
    /// Get standard windows monitor profile pathname.
    INTAPI_BASE static bool GetWindowsMonitorICC(PathStr &icm_pathname);
    /// Get monitor profile pathname for the current display, specified by user.
    INTAPI_BASE static bool GetMonitorICC(PathStr &icm_pathname);
    /// Report hardware details to the log file.
    INTAPI_BASE static void LogHardware();
    /// Get CPU model.
    INTAPI_BASE static OKAY GetCPU(Str &res);
    /// Get videocard(s) details.
    INTAPI_BASE static OKAY GetVideo(TArray<Str> &videos);
    /// Get OS version.
    INTAPI_BASE static OKAY GetOS(Str &res);
    //@}

    /// Issue audio signal - beep.
    INTAPI_BASE static void Beep();
    /// Set own filter for unhandled exceptions.
    INTAPI_BASE static void SetOwnExceptionFilter();
  };  // class Envi

INTEGRA_NAMESPACE_END

#endif

