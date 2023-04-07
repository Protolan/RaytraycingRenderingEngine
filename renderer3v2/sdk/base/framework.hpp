/// @file
///
/// @brief Plugin-related global functions.
///
/// @sa @ref base_mainpage
/// Copyright &copy; INTEGRA, Inc., 2004.
///
/// @internal
/// @author Eed - E. Denisov KLON, '04.12.23.

#ifndef _KLON_FRAMEWORK_HPP_
#define _KLON_FRAMEWORK_HPP_

#include "export.h"

INTEGRA_NAMESPACE_START

/// Initialize Integra Object Framework.
INTAPI_BASE void InitFrameWork(int version = REPOSITORY_VERSION);
/// Terminate Integra Object Framework.
INTAPI_BASE void TermFrameWork();
/// Load a plugin DLL.
INTAPI_BASE OKAY LoadPlugin(const char *name, bool required = false);

INTEGRA_NAMESPACE_END
#endif  // Header wrapper
