/// @file
///
/// @brief Global functions to be defined in each plugin (DLL).
///
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @internal
/// @author Eed - E. Denisov KLON, '04.07.06.

#ifndef _KLON_PLUGINS_HPP_
#define _KLON_PLUGINS_HPP_

#if defined _WIN32 && !defined DOXYGEN
#define DLLEXPORT      __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// Initialize plugin (<b>provided by the user</b>).
extern "C" DLLEXPORT OKAY InitClasses();
/// Terminate plugin (<b>provided by the user</b>).
extern "C" DLLEXPORT void TermClasses();
/// Get plugin version.
extern "C" DLLEXPORT int VersionId();

#endif  // Header wrapper
