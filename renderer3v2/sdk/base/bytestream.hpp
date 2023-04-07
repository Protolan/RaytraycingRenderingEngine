/// @file
///
/// @brief Declaration of ByteStream class.
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '00.12.28

#ifndef _KLDB_BYTESTREAM_HPP_
#define _KLDB_BYTESTREAM_HPP_

INTEGRA_NAMESPACE_START

/// Abstract byte stream interface.
class ByteStream
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    inline ByteStream(bool imp);
    /// Copy constructor.
    inline ByteStream(const ByteStream &src);
    /// Destructor.
    inline virtual ~ByteStream();
    //@}

  public:
    /// @name Opening and closing
    //@{
    /// Open the stream.
    virtual OKAY Open() = 0;
    /// Close the stream.
    virtual OKAY Close() = 0;
    //@}

  public:
    /// @name Reading and writing
    //@{
    /// Get serialization direction.
    inline bool Import();
    /// Lower level per-byte I/O.
    virtual void Byte(void *ptr) = 0;
    /// Lower level I/O.
    virtual void Bytes(void *ptr, SIZE_T len) = 0;
    /// Check for end of stream.
    inline virtual bool EndOfStream();
    //@}

    /// Whether this is from/to a FILE or MEMORY
    virtual bool IsFile() const = 0;

  private:
    /// Direction: import (read) or export (write).
    bool import;

  };  // class ByteStream

//////////////////////////////////////////////////////////////////////////////
// Inline methods of ByteStream

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method constructs the base object storing the argument (I/O direction)
/// in the representation.
/// @param[in] imp - I/O direction; @b true for reading, @b false for writing. 
ByteStream::ByteStream(bool imp)
  {
  // Remember serialization direction
  import = imp;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The method is forbidden; debug version asserts it.
/// @param[in] src - A source object.
ByteStream::ByteStream(const ByteStream &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method destroys the base object.
 ByteStream::~ByteStream()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get serialization direction.
/// @return @b YES if this stream reads data;
/// @b NO, if this stream writes data.
bool ByteStream::Import()
  {
  return import;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for end of stream.
///
/// The method checks if end of stream (end of file, for example) was reached.
/// @return @b true if end of stream was reached, @b false otherwise.
/// @note The default implementation returns false.
bool ByteStream::EndOfStream()
  {
  return false;
  }

INTEGRA_NAMESPACE_END
#endif
