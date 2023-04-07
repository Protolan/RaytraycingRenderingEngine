/// @file
///
/// @brief Plugin-related internal global functions.
///
/// @author Eed - E. Denisov KLON, '04.07.06.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

#ifndef _KLON_PLUGINS_HXX_
#define _KLON_PLUGINS_HXX_

#ifndef _WIN32
#define HMODULE void *
#endif

#include "export.h"

INTEGRA_NAMESPACE_START

/// Notifier about plug-in loading.
INTAPI_BASE void PluginLoading(const HMODULE dll);

INTEGRA_NAMESPACE_END
#endif  // Header wrapper
