/// @internal
/// @file
///
/// @brief Subsidiary file defining import/export switch.
///
/// @author Pnd - Nick Deriabin, AE32, '04.05.24.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

/**
@def INTAPI_BASE
This macro simplifies usage of dllexport/dllimport directives.
**/

#ifdef _WIN32
#ifdef INTEXPORT_BASE
#define INTAPI_BASE __declspec(dllexport)
#elif defined INTEXPORT_LTKERNEL
#define INTAPI_BASE
#else
#define INTAPI_BASE __declspec(dllimport)
#endif
#else // not _WIN32
#define INTAPI_BASE
#endif
