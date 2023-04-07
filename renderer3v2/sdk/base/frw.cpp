/// @internal
/// @file
///
/// @brief Implementation of own "bug free" versions of fread() and fwrite().
///
/// @author Eed - Eugene Denisov '06.07.06
///
/// Copyright &copy; INTEGRA, Inc., 2006-2012

#include <integra.hpp>

#include "frw.h"

/// Size of the chunk to be read or written at once.
static const size_t CHUNK_LEN = 0x1000000;

//////////////////////////////////////////////////////////////////////////////
/// Workaround for "large files on local mapped drive" bug in fread().
/// Method replaces fread() and reads requested size of data by chunks.
/// Parameters, returned value and semantics are equal to fread() ones.
/// @param[in] buffer Storage location for data.
/// @param[in] size Item size in bytes.
/// @param[in] count Maximum number of items to be read.
/// @param[in] stream Pointer to FILE structure.
/// @return The number of full items actually read.
size_t __cdecl integra_fread(void *buffer, size_t size, size_t count, FILE *stream)
  {
#ifdef _WIN32
  size_t total = size * count;
  size_t rest = total, read = 0;
  unsigned char *ptr = (unsigned char *)buffer;

  if (size == 0 || count == 0)
    return 0;

  do
    {
    read = fread(ptr, 1, rest > CHUNK_LEN ? CHUNK_LEN : rest, stream);
    ptr += read;
    rest -= read;
    }
  while (read > 0 && rest > 0);
  return (ptr - (unsigned char *)buffer) / size;
#else
  return fread(buffer, size, count, stream);
#endif
  }  // end of integra_fread()

//////////////////////////////////////////////////////////////////////////////
/// Workaround for "large files on network drive" bug in fwrite().
/// Method replaces fwrite() and writes requested size of data by chunks.
/// Parameters, returned value and semantics are equal to fwrite() ones.
/// @param[in] buffer Pointer to data to be written.
/// @param[in] size Item size in bytes.
/// @param[in] count Maximum number of items to be written.
/// @param[in] stream Pointer to FILE structure.
/// @return The number of full items actually written.
size_t __cdecl integra_fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
  {
#ifdef _WIN32
  size_t total = size * count;
  size_t rest = total, wrote = 0;
  unsigned char *ptr = (unsigned char *)buffer;

  if (size == 0 || count == 0)
    return 0;

  do
    {
    wrote = fwrite(ptr, 1, rest > CHUNK_LEN ? CHUNK_LEN : rest, stream);
    ptr += wrote;
    rest -= wrote;
    }
  while (wrote > 0 && rest > 0);
  return (ptr - (unsigned char *)buffer) / size;
#else
  return fwrite(buffer, size, count, stream);
#endif
  }  // integra_fwrite()
