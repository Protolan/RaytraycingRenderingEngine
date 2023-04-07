/// @internal
/// @file
///
/// @brief Definition of ByteStream class.
///
/// @author Eed - Eugene Denisov, '04.10.25
///
/// Copyright &copy; INTEGRA, Inc., 2004


/** 
@class ByteStream base/bytestream.hpp

Abstract byte stream interface.

This is an abstract class describing an I/O stream of bytes.

The stream can be as in (to read) as out (to write).
The stream keeps its direction as a private (constant) member
set as a constructor parameter.

Particular implementations of the class could read or write data
from / to a file or another media.

Copy constructor for the class is forbidden.

@par Example

Usage of streams is straightforward: open, input / output, close:

@code
OKAY StreamIO(ByteStream &io)
  {
  char s1[6], s2[6];
  char c1, c2, c3;
  // Open the stream
  if (io.Open() != SUCCESS)
    return FAILURE;  // I/O error
  // Initialize data when writing
  if (!io.Import())
    {
    strcpy(s1, "Hello");
    strcpy(s2, "World");
    c1 = ',', c2 = ' ', c3 = '!';
    }
  // Perform input / output
  io.Bytes(s1, 5);
  io.Byte(&c1);
  io.Byte(&c2);
  io.Bytes(s2, 5);
  io.Byte(&c3);
  // Print data when reading
  if (io.Import())
    printf("%s%c%c%s%c", s1, c1, c2, s2, c3);
  // Close the stream
  if (io.Close() != SUCCESS)
    return FAILURE;  // I/O error
  return SUCCESS;
  }
@endcode

@sa @ref base_mainpage
**/

/**
@fn ByteStream::Open()

Open the stream.

@return SUCCESS or FAILURE. FAILURE means an I/O error.
**/

/**
@fn ByteStream::Close()
Close the stream.
@return SUCCESS or FAILURE. FAILURE means an I/O error.
**/

/**
@fn ByteStream::Byte(void *ptr)

Lower level per-byte I/O.

The method reads or writes a single byte from / to the stream.
@param[in, out] ptr - A pointer to a location to put a read byte (on reading)
or get the byte to write (on writing).
**/

/**
@fn ByteStream::Bytes(void *ptr, SIZE_T len)

Lower level I/O.

The method reads or writes several bytes from / to the stream.
@param[in, out] ptr - A pointer to a location to put the read bytes (on reading)
or get the bytes to write (on writing).
@param[in] len - The number of bytes to read or write.
**/
