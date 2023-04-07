/// @internal
/// @file
///
/// @brief Definitions of MemStream, ReadMemStream and WriteMemStream classes.
///
/// @author Pnd - Nickolay Derjabin, '01.02.26
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004

#include <integra.hpp>
#include "memstream.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class MemStream base/memstream.hpp

Abstract base class for memory read / write streams.

The only purpose of MemStream is to define additional
abstract methods Data() and Length().

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
/// @param[in] imp - The I/O direction; @b true for reading, @b false for 
/// writing.
MemStream::MemStream(bool imp) :
  ByteStream(imp)  // import direction
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The method is forbidden, debug version asserts it.
/// @param[in] src - A source object.
MemStream::MemStream(const MemStream &src) :
  ByteStream(src)
  {
  Assert(false);  // Copy constructor is forbidden
  }

/// Whether this is from/to a FILE or MEMORY
bool MemStream::IsFile() const
  {
  return false;
  }

/**
@class ReadMemStream base/memstream.hpp

This is an implementations of ByteStream for reading from a memory area.
ReadMemStream is an input stream.

@sa @ref base_mainpage

@internal @note
To deal with existing IMEL, this stream does not use XArray
but is initialized by pointer to area allocated somewhere (in IMEL)
using malloc().  It is the caller's responsibility to free this memory
(if needed) as soon as the stream is deleted.
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
/// The method takes a pointer to a memory area and the length of the area.
/// This area will be used an an input byte stream.
/// The area must be allocated by the caller (for example, via malloc()) and filled
/// by an appropriate stream's contents.  The caller also should free the memory
/// as soon as the stream is deleted.
/// @param[in] area - An address of a memory area.
/// @param[in] length - The length of the memory area.
ReadMemStream::ReadMemStream(const BYTE *area, SIZE_T length) :
  MemStream(true),  // import direction
  area(area),
  length(length)
  {
  Assert((area != NULL) || (length == 0));
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The method is forbidden, debug version asserts it.
/// @param[in] src - A source object.
ReadMemStream::ReadMemStream(const ReadMemStream &src) :
  MemStream(src),
  area(src.area),
  length(src.length)
  {
  Assert(false);  // Copy constructor is forbidden
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method destroys the streams. The memory occupied by the stream is freed.
ReadMemStream::~ReadMemStream()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Open the stream.
/// @note This method rewinds the stream.
/// @return SUCCESS always.
OKAY ReadMemStream::Open()
  {
  cur = 0;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Close the stream.
/// @return SUCCESS always.
OKAY ReadMemStream::Close()
  {
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a single byte.
///
/// The method reads a single byte from the memory area.
/// @param[out] ptr - A pointer to a location to put the read byte.
void ReadMemStream::Byte(void *ptr)
  {
  // Read a byte
  BYTE *bptr = (BYTE *)ptr;
  *bptr = (cur < length) ? area[cur] : (BYTE)0;
  cur++;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read several bytes.
/// The method reads several bytes from the memory area.
/// @param[out] ptr - A pointer to a location to put the read bytes.
/// @param[in] len - The number of bytes to read.
void ReadMemStream::Bytes(void *ptr, SIZE_T len)
  {
  Assert(len >= 0);
  // Read bytes
  if (cur + len < length)
    memcpy(ptr, area + cur, len);
  else
    memset(ptr, 0, len);
  cur += len;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for the end of the stream.
///
/// The method checks if the end of the area was reached on reading.
/// @return @b true if the end of the memory area was reached, @b false 
/// otherwise.
bool ReadMemStream::EndOfStream()
  {
  return (cur > length);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get address of the area.
/// @return An address of the memory area (passed as a parameter to the 
/// constructor).
const BYTE *ReadMemStream::Data()
  {
  return area;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the length of the area.
/// @return The length of the memory area (passed as a parameter to the 
/// constructor).
SIZE_T ReadMemStream::Length()
  {
  return length;
  }


/**
@class WriteMemStream base/memstream.hpp

This is an implementations of ByteStream for writing to a memory area.
WriteMemStream is an output stream.

The class allocates the memory area internally, using XArray<BYTE>.
This array will be used as the output byte stream.

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method allocates a memory area.
/// @param[in] block_size - The block size of the array.
/// @param[in] initial_size - The initial size of the array (will be allocated).
WriteMemStream::WriteMemStream(SIZE_T block_size, SIZE_T initial_size) :
  MemStream(false),  // export direction
  area(block_size)
  {
  // Allocate initial size
  area.Resize(initial_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src - A source object.
WriteMemStream::WriteMemStream(const WriteMemStream &src) :
  MemStream(src),
  area(src.area)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Open the stream.
/// @note This method rewinds the stream.
/// @return SUCCESS always.
OKAY WriteMemStream::Open()
  {
  // Rewind the stream
  area.Truncate();
  cur = 0;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Close the stream.
/// @return SUCCESS or FAILURE. FAILURE means a memory allocation error.
/// The default User is notified.
OKAY WriteMemStream::Close()
  {
  if (cur == area.Length())
    return SUCCESS;
  User()->WarningMessage(Tr("Out of memory - servers will be aborted"));
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write single byte.
///
/// The method writes a single byte to the memory area.
/// @param[in] ptr - A pointer to a location to get the byte to write.
void WriteMemStream::Byte(void *ptr)
  {
  // Do nothing on overflow
  if (cur >= MAX_SIZE_T)
    return;
  // Calculate new size
  if (area.Size() < cur + 1)
    {
    SIZE_T new_size = area.Size();
    while (new_size < cur + 1)
      {
      new_size = new_size * 2 + 1;
      if (new_size >= MAX_SIZE_T)
        break;
      }
    if (new_size < MAX_SIZE_T)
      area.Resize(new_size);
    }

  if ((cur + 1 < MAX_SIZE_T) && area.Grow(cur + 1) == SUCCESS)
    area[cur] = *(BYTE *)ptr;
  cur++;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write several bytes.
///
/// The method writes several bytes to the memory area.
/// @param[in] ptr - A pointer to a location to get the bytes to write.
/// @param[in] len - The number of bytes to write.
void WriteMemStream::Bytes(void *ptr, SIZE_T len)
  {
  Assert(len < MAX_SIZE_T);
  // Do nothing on overflow
  if (cur >= MAX_SIZE_T)
    return;
  // Calculate new size
  if (area.Size() < cur + len)
    {
    SIZE_T new_size = area.Size();
    while (new_size < cur + len)
      {
      new_size = new_size * 2 + 1;
      if (new_size >= MAX_SIZE_T)
        break;
      }
    if (new_size < MAX_SIZE_T)
      area.Resize(new_size);
    }
  if ((cur + len < MAX_SIZE_T) && area.Grow(cur + len) == SUCCESS)
    memcpy(area.Data() + cur, ptr, len);
  cur += len;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get address of the area.
/// @return An address of the memory area.
const BYTE *WriteMemStream::Data()
  {
  return area.Data();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get length of the area.
/// @return A length of the memory area.
SIZE_T WriteMemStream::Length()
  {
  return area.Length();
  }

INTEGRA_NAMESPACE_END
