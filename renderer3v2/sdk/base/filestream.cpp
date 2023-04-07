/// @internal
/// @file
///
/// @brief Declaration of ReadFileStream and WriteFileStream classes.
///
/// @author Pnd - Nickolay Deryabin, '00.12.28
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004

#include <integra.hpp>
#include "arrays.hpp"
#include "file.hpp"
#include "filestream.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class ReadFileStream base/filestream.hpp

The ReadFileStream class is the implementation of ByteStream for reading from 
ordinary files.

@sa @ref base_mainpage
**/

/// Buffer size for read file stream class.
static const int R_BUF_SIZE = 1024 * 1024;
/// Number of portions to report progress.
static const int R_PORTIONS = 10;

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method constructs the stream from underlying file with specified name.
///
/// @param[in] file_name Name of file.
///
ReadFileStream::ReadFileStream(const PathStr &file_name) :
  ByteStream(true),  // import direction
  m_file(file_name, User())
  {
  m_fsize = 0;
  m_n_portions = 0;
  m_progress = false;
  m_portion_size = 0;
  m_cur_pos = 0;
  m_eof = false;
  }  // ReadFileStream constructor

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor (forbidden).
///
/// @param[in] src Source object.
///
ReadFileStream::ReadFileStream(const ReadFileStream &src) :
  ByteStream(src),
  m_file(src.m_file)
  {
  Assert(false);
  }  // ReadFileStream copy constructor

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
ReadFileStream::~ReadFileStream()
  {
  Assert(!m_file.Opened());
  if (m_file.Opened())
    m_file.Close();
  }  // ReadFileStream destructor

//////////////////////////////////////////////////////////////////////////////
/// Open stream.
///
/// @return SUCCESS or FAILURE. FAILURE means I/O error.
///
OKAY ReadFileStream::Open()
  {
  Assert(!m_file.Opened());
  // Allocate buffer
  if (m_buffer.Allocate(R_BUF_SIZE) != SUCCESS)
    return User()->ErrorNoMemory();
  // Open file
  if (m_file.Open("rb") != SUCCESS)
    return FAILURE;
  // Get file size
  const PathStr &fname = m_file.PathName();
  m_fsize = (fname.FileSize() + R_BUF_SIZE) / R_BUF_SIZE;
  Assert(m_fsize > 0);
  m_n_portions = 0;
  m_progress = false;
  m_portion_size = 0;
  m_cur_pos = 0;
  m_eof = false;
  return SUCCESS;
  }  // ReadFileStream::Open()

//////////////////////////////////////////////////////////////////////////////
/// Close stream.
///
/// @return SUCCESS or FAILURE. FAILURE means I/O error while working with file stream.
///
OKAY ReadFileStream::Close()
  {
  Assert(m_file.Opened());
  // Turn off progress indication
  if (m_progress)
    User()->TermProgress();
  // Close the file
  return m_file.Close();
  }  // ReadFileStream::Close()

//////////////////////////////////////////////////////////////////////////////
/// Read next data portion.
///
/// It sets m_cur_pos to zero and m_portion_size to size of data read (> 0 always).
/// On end of file, endless portions of zeros are emulated.
///
void ReadFileStream::ReadPortion()
  {
  m_cur_pos = 0;
  m_portion_size = 0;
  SIZE_T act_len;
  if (m_eof)
    {
    // The buffer was cleared to this moment
    // Return portion of zeros
    m_portion_size = R_BUF_SIZE;
    return;
    }
  // Show progress at the end of each portion
  if (((m_n_portions % R_PORTIONS) == 0) && (m_n_portions > 0))
    {
    // Turn on progress indication eventually
    if (!m_progress)
      {
      User()->InitProgress(true);
      m_progress = true;
      }
    INT64 next = m_n_portions + R_PORTIONS;
    User()->ShowProgress((double)next / (double)m_fsize, Tr("Loading %s"), m_file.PathName().Data());
    }
  // Read the file
  m_file.Read(m_buffer.Data(), R_BUF_SIZE, act_len);
  if (act_len <= 0)
    {
    // End of file was reached
    // Set EOF indication, and clear the buffer
    m_eof = true;
    memset(m_buffer.Data(), 0, R_BUF_SIZE);
    // Return portion of zeros
    m_portion_size = R_BUF_SIZE;
    return;
    }
  // Return real portion read
  m_portion_size = (int)act_len;
  m_n_portions++;
  }  // ReadFileStream::ReadPortion()

//////////////////////////////////////////////////////////////////////////////
/// Read single byte.
///
/// The method reads a single byte from the file.
///
/// @param[out] ptr Pointer to a location to put read byte. 
///
void ReadFileStream::Byte(void *ptr)
  {
  // Read portion in if needed
  if (m_cur_pos >= m_portion_size)
    ReadPortion();
  // Get next byte
  *static_cast<BYTE *>(ptr) = m_buffer[m_cur_pos++];
  }  // ReadFileStream::Byte()

//////////////////////////////////////////////////////////////////////////////
/// Read several bytes.
///
/// The method reads several bytes from the file.
///
/// @param[out] ptr Pointer to a location to put read bytes. 
/// @param[in] len Number of bytes to read.
///
void ReadFileStream::Bytes(void *ptr, SIZE_T len)
  {
  // Read bytes by portion
  while (len > 0)
    {
    // Read portion in if needed
    if (m_cur_pos >= m_portion_size)
      ReadPortion();
    // Get portion data
    int size_to_get = m_portion_size - m_cur_pos;
    if ((SIZE_T)size_to_get > len)
      size_to_get = (int)len;
    memcpy(static_cast<BYTE *>(ptr), m_buffer.Data() + m_cur_pos, size_to_get);
    m_cur_pos += size_to_get;
    ptr = (BYTE *)ptr + size_to_get;
    len -= size_to_get;
    }
  }  // ReadFileStream::Bytes()

//////////////////////////////////////////////////////////////////////////////
/// Check for end of file.
///
/// @return @b true if end of file was reached, @b false otherwise.
///
bool ReadFileStream::EndOfStream()
  {
  return m_eof;
  }  // ReadFileStream::EndOfStream()

/// Whether this is from/to a FILE or MEMORY
bool ReadFileStream::IsFile() const
  {
  return true;
  }

/**

@class WriteFileStream base/filestream.hpp

ByteStream writing to a file.

This is implementation of ByteStream for writing to ordinary files.

@sa @ref base_mainpage

**/

/// Buffer size for write file stream class.
static const int W_BUF_SIZE = 1024 * 1024;
/// Number of portions to report progress.
static const int W_PORTIONS = 10;

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method constructs the stream from an underlying file with the specified name.
///
/// @param[in] file_name - A name of the file.
///
WriteFileStream::WriteFileStream(const PathStr &file_name) :
  ByteStream(false),  // export direction
  m_file(file_name, User())
  {
  m_n_portions = 0;
  m_progress = false;
  m_cur_pos = 0;
  }  // WriteFileStream constructor

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor (forbidden).
///
/// @param[in] src - A source object.
///
WriteFileStream::WriteFileStream(const WriteFileStream &src) :
  ByteStream(src),
  m_file(src.m_file)
  {
  Assert(false);
  }  // WriteFileStream copy constructor

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// Destroys the stream.
///
WriteFileStream::~WriteFileStream()
  {
  Assert(!m_file.Opened());
  if (m_file.Opened())
    m_file.Close();
  }  // WriteFileStream destructor

//////////////////////////////////////////////////////////////////////////////
/// Open the stream.
/// @return SUCCESS or FAILURE. FAILURE means an I/O error.
OKAY WriteFileStream::Open()
  {
  Assert(!m_file.Opened());
  // Allocate buffer
  if (m_buffer.Allocate(W_BUF_SIZE) != SUCCESS)
    return User()->ErrorNoMemory();
  // Open file
  if (m_file.Open("wb") != SUCCESS)
    return FAILURE;
  m_n_portions = 0;
  m_progress = false;
  m_cur_pos = 0;
  return SUCCESS;
  }  // WriteFileStream::Open()

//////////////////////////////////////////////////////////////////////////////
/// Close the stream.
///
/// @return SUCCESS or FAILURE. FAILURE means an I/O error while working with the file stream.
///
OKAY WriteFileStream::Close()
  {
  Assert(m_file.Opened());
  // Write last portion
  if (m_cur_pos > 0)
    m_file.Write(m_buffer.Data(), m_cur_pos);
  // Turn off progress indication
  if (m_progress)
    User()->TermProgress();
  // Close the file
  return m_file.Close();
  }  // WriteFileStream::Close()

//////////////////////////////////////////////////////////////////////////////
/// Write a single byte.
///
/// The method writes a single byte to the file.
///
/// @param[in] ptr A pointer to a location to get the byte to write.
///
void WriteFileStream::Byte(void *ptr)
  {
  // Put byte to the buffer
  Assert(m_cur_pos < W_BUF_SIZE);
  m_buffer[m_cur_pos++] = *(BYTE *)ptr;
  if (m_cur_pos < W_BUF_SIZE)
    return;
  // Flush the buffer
  WritePortion();
  }  // WriteFileStream::Byte()

//////////////////////////////////////////////////////////////////////////////
/// Write several bytes.
///
/// The method writes several bytes to the file.
///
/// @param[in] ptr A pointer to a location to get the bytes to write.
/// @param[in] len The number of bytes to read.
///
void WriteFileStream::Bytes(void *ptr, SIZE_T len)
  {
  // Write bytes
  Assert(m_cur_pos < W_BUF_SIZE);
  for ( ; ; )
    {
    // Size of next portion to put to the buffer
    int portion_size = W_BUF_SIZE - m_cur_pos;
    if ((SIZE_T)portion_size > len)
      portion_size = (int)len;
    // Put the portion to the buffer
    memcpy(m_buffer.Data() + m_cur_pos, ptr, portion_size);
    m_cur_pos += portion_size;
    if (m_cur_pos < W_BUF_SIZE)
      return;
    ptr = (BYTE *)ptr + portion_size;
    len -= portion_size;
    // Flush the buffer
    WritePortion();
    }
  }  // WriteFileStream::Bytes()

//////////////////////////////////////////////////////////////////////////////
/// Write next data portion.
///
void WriteFileStream::WritePortion()
  {
  // Write portion
  m_file.Write(m_buffer.Data(), W_BUF_SIZE);
  m_cur_pos = 0;
  m_n_portions++;
  // Proceed by W_PORTIONS portions
  if ((m_n_portions % W_PORTIONS) != 0)
    return;
  // Turn on progress indication eventually
  if (!m_progress)
    {
    User()->InitProgress(true, true);
    m_progress = true;
    }
  // Report progress
  const int M = 1024 * 1024;
  INT64 size = (m_n_portions * W_BUF_SIZE) / M;
  User()->ShowProgress(0, Tr("Saving %s (%I64d Mb written)", "Progress"), m_file.PathName().Data(), size);
  }  // WriteFileStream::WritePortion()

/// Whether this is from/to a FILE or MEMORY
bool WriteFileStream::IsFile() const
  {
  return true;
  }

INTEGRA_NAMESPACE_END

