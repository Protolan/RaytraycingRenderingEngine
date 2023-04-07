/// @file
///
/// @brief Declaration for own "bug free" versions of fread() and fwrite().
///
/// Copyright &copy; INTEGRA, Inc., 2006-2012
///
/// @internal
/// @author Eed - Eugene Denisov '06.07.06

#ifndef _AE32_FRW_H_
#define _AE32_FRW_H_

#include "export.h"

START_C_DECLS
#ifndef _WIN32
#define __cdecl
#endif

/// Workaround for "large files on local mapped drive" bug in fread().
INTAPI_BASE size_t __cdecl integra_fread(void *buffer, size_t size, size_t count, FILE *stream);

/// Workaround for "large files on network drive" bug in fwrite().
INTAPI_BASE size_t __cdecl integra_fwrite(const void *buffer, size_t size, size_t count, FILE *stream);

#ifndef _WIN32
#define __cdecl
#endif
END_C_DECLS

#endif
