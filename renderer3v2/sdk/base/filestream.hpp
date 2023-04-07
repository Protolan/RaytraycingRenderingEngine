/// @file
///
/// @brief Declaration of ReadFileStream and WriteFileStream classes.
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pnd - Nickolay Deryabin, '00.12.28

#ifndef _KLDB_FILESTREAM_HPP_
#define _KLDB_FILESTREAM_HPP_

#include "arrays.hpp"
#include "bytestream.hpp"
#include "export.h"
#include "file.hpp"

INTEGRA_NAMESPACE_START

/// Implements reading of ByteStream from a file.
class ReadFileStream : public ByteStream
  {
  public:
    /// Constructor.
    INTAPI_BASE ReadFileStream(const PathStr &file_name);
    /// Destructor.
    INTAPI_BASE virtual ~ReadFileStream();

  protected:
    /// Copy constructor (forbidden).
    INTAPI_BASE ReadFileStream(const ReadFileStream &src);

  public:
    /// @name Opening and closing
    //@{
    /// Open the stream.
    INTAPI_BASE virtual OKAY Open();
    /// Close the stream.
    INTAPI_BASE virtual OKAY Close();
    //@}

    /// Whether this is from/to a FILE or MEMORY
    INTAPI_BASE virtual bool IsFile() const;

  private:
    /// Read next data portion.
    void ReadPortion();

  public:
    /// @name Reading
    //@{
    /// Read a single byte.
    INTAPI_BASE virtual void Byte(void *ptr);
    /// Read several bytes.
    INTAPI_BASE virtual void Bytes(void *ptr, SIZE_T len);
    /// Check for the end of file.
    INTAPI_BASE virtual bool EndOfStream();
    //@}

  private:
    /// Underlying file.
    File m_file;
    /// The file's size.
    INT64 m_fsize;
    /// Number of portions read.
    INT64 m_n_portions;
    /// Progress mode indicator.
    bool m_progress;
    /// Buffer.
    TArray<BYTE> m_buffer;
    /// Size of data portion read to the buffer.
    int m_portion_size;
    /// Current position in the buffer.
    int m_cur_pos;
    /// End Of File indicator.
    bool m_eof;

  };  // class ReadFileStream


/// ByteStream writing to a file.
class WriteFileStream : public ByteStream
  {
  public:
    /// Constructor.
    INTAPI_BASE WriteFileStream(const PathStr &file_name);
    /// Destructor.
    INTAPI_BASE virtual ~WriteFileStream();

  protected:
    /// Copy constructor (forbidden).
    INTAPI_BASE WriteFileStream(const WriteFileStream &src);

  public:
    /// @name Opening and closing
    //@{
    /// Open the stream.
    INTAPI_BASE virtual OKAY Open();
    /// Close the stream.
    INTAPI_BASE virtual OKAY Close();
    //@}

  public:
    /// @name Writing
    //@{
    /// Write a single byte.
    INTAPI_BASE virtual void Byte(void *ptr);
    /// Write several bytes.
    INTAPI_BASE virtual void Bytes(void *ptr, SIZE_T len);
    //@}

    /// Whether this is from/to a FILE or MEMORY
    INTAPI_BASE virtual bool IsFile() const;

  private:
    /// Write next data portion.
    void WritePortion();

  private:
    /// Underlying file.
    File m_file;
    /// Number of portions written.
    INT64 m_n_portions;
    /// Progress mode indicator.
    bool m_progress;
    /// Buffer.
    TArray<BYTE> m_buffer;
    /// Current position in the buffer.
    int m_cur_pos;

  };  // class WriteFileStream

INTEGRA_NAMESPACE_END

#endif

