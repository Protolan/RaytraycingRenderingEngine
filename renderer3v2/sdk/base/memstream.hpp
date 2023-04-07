/// @file
///
/// @brief Declarations of MemStream, ReadMemStream and WriteMemStream classes.
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '01.02.26

#ifndef _KLDB_MEMSTREAM_HPP_
#define _KLDB_MEMSTREAM_HPP_

#include "xarray.hpp"
#include "bytestream.hpp"
#include "export.h"

INTEGRA_NAMESPACE_START

/// Abstract base class for memory read / write streams.
class MemStream : public ByteStream
  {
  public:
    /// @name Constructors
    //@{
    /// Constructor.
    INTAPI_BASE MemStream(bool imp);
    /// Copy constructor.
    INTAPI_BASE MemStream(const MemStream &src);
    //@}

  public:
    /// @name Access the memory area
    //@{
    /// Get the address of the area.
    INTAPI_BASE virtual const BYTE *Data() = 0;
    /// Get the length of the area.
    INTAPI_BASE virtual SIZE_T Length() = 0;
    //@}

    /// Whether this is from/to a FILE or MEMORY
    INTAPI_BASE virtual bool IsFile() const;

  };  // class MemStream


/// Implements reading of ByteStream from a memory area.
class ReadMemStream : public MemStream
  {
  public:
    /// @name Constructors, destructor, assignment
    //@{
    /// Constructor.
    INTAPI_BASE ReadMemStream(const BYTE *area, SIZE_T length);
    /// Constructor.
    INTAPI_BASE ReadMemStream(const ReadMemStream &src);
    /// Destructor.
    INTAPI_BASE virtual ~ReadMemStream();
    /// Assignment operator.
    inline void operator =(const ReadMemStream &src);
    //@}

  public:
    /// @name Opening and Closing
    //@{ 
    /// Open stream.
    INTAPI_BASE virtual OKAY Open();
    /// Close stream.
    INTAPI_BASE virtual OKAY Close();
    //@}

  public:
    /// @name Reading
    //@{
    /// Read single byte.
    INTAPI_BASE virtual void Byte(void *ptr);
    /// Read several bytes.
    INTAPI_BASE virtual void Bytes(void *ptr, SIZE_T len);
    /// Check for end of stream.
    INTAPI_BASE virtual bool EndOfStream();
    //@}

  public:
    /// @name Access the memory area
    //@{
    /// Get address of area.
    INTAPI_BASE virtual const BYTE *Data();
    /// Get length of area.
    INTAPI_BASE virtual SIZE_T Length();
    //@}

  private:
    /// Underlying memory area address (allocated elsewhere).
    const BYTE *area;
    /// Underlying memory area length.
    const SIZE_T length;
    /// Current position.
    SIZE_T cur;
  };  // class ReadMemStream


/// ByteStream writing to a memory area.
class WriteMemStream : public MemStream
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    INTAPI_BASE WriteMemStream(SIZE_T block_size = 64000, SIZE_T initial_size = 64000);
    /// Copy constructor.
    INTAPI_BASE WriteMemStream(const WriteMemStream &src);
    /// Destructor.
    inline virtual ~WriteMemStream();
    //@}

  public:
    /// @name Opening and Closing
    //@{
    /// Open stream.
    INTAPI_BASE virtual OKAY Open();
    /// Close stream.
    INTAPI_BASE virtual OKAY Close();
    //@}

  public:
    /// @name Reading
    //@{
    /// Write single byte.
    INTAPI_BASE virtual void Byte(void *ptr);
    /// Write several bytes.
    INTAPI_BASE virtual void Bytes(void *ptr, SIZE_T len);
    //@}

  public:
    /// @name Access the memory area
    //@{
    /// Get address of the area.
    INTAPI_BASE virtual const BYTE *Data();
    /// Get length of area.
    INTAPI_BASE virtual SIZE_T Length();
    //@}

  private:
    /// Underlying memory area.
    XArray<BYTE> area;
    /// Current position.
    SIZE_T cur;
  };  // class WriteMemStream

//////////////////////////////////////////////////////////////////////////////
// Inline methods of ReadMemStream class

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @note The method is forbidden, debug version asserts it.
/// @param[in] src - A source object.
void ReadMemStream::operator =(const ReadMemStream &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
// Inline methods of WriteMemStream class

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
WriteMemStream::~WriteMemStream()
  {
  }

INTEGRA_NAMESPACE_END
#endif
