/// @file
///
/// @brief Declaration of InputFile class for fast input.
///
/// Copyright &copy; INTEGRA, Inc., 2014.
///
/// @internal
/// @author Pnd - Nick Derjabin, AKFx, '14.09.24.

#ifndef _AKFX_INPUT_FILE_HPP_
#define _AKFX_INPUT_FILE_HPP_

#include "export.h"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// This class is intended for fast import of huge text files.
class InputFile
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    INTAPI_BASE InputFile(const CHAR *pathname, const char *comment_mark = NULL);
    /// Destructor.
    INTAPI_BASE virtual ~InputFile();
    //@}

  protected:
    /// Copy constructor (forbidden).
    InputFile(const InputFile &src);

  public:
    /// @name Opening and closing
    //@{
    /// Open the file.
    INTAPI_BASE virtual OKAY FOpen();
    /// Close the file.
    INTAPI_BASE virtual OKAY FClose();
    //@}

  public:
    /// @name Read services
    //@{
    /// Read given keyword (case sensitive).
    INTAPI_BASE bool FReadKeyWord(const char *keyword);
    /// Read given keyword (case insensitive).
    INTAPI_BASE bool FReadKEYWORD(const char *keyword);
    /// Read a logical value.
    INTAPI_BASE bool FReadBool(bool &value);
    /// Read an integer value.
    INTAPI_BASE bool FReadInt(int &value);
    /// Read a floating point value.
    INTAPI_BASE bool FReadDouble(double &value);
    /// Read a textual value.
    INTAPI_BASE bool FReadText(Str &value);
    /// Read the rest of the line.
    INTAPI_BASE bool FReadRestOfLine(UStr &str);
    //@}

  public:
    /// @name EOL / EOF control
    //@{
    /// Check for the end of line.
    INTAPI_BASE bool FIsEOL();
    /// Read end of line.
    INTAPI_BASE bool FReadEOL();
    /// Skip current line.
    INTAPI_BASE void FSkipLine();
    /// Test for end of file.
    INTAPI_BASE bool FIsEndOfFile() const;
    //@}

  public:
    /// @name Diagnostics support
    //@{
    /// Get the current line number.
    INTAPI_BASE virtual int LineNo() const;
    /// Get text specifying the current position in the file.
    INTAPI_BASE Str Where() const;
    /// Get the full pathname to the file.
    INTAPI_BASE const PathStr &PathName() const;
#if 0
    /// Get file time.
    INTAPI_BASE virtual unsigned FileTime() const;
#endif
    //@}

  protected:
    /// Virtuel fread() implementation.
    virtual int SysFRead(CHAR *dest_area, int size);

  private:
    /// Read next portion of the file to the buffer.
    virtual void ReadPortion();
    /// Move to the next line.
    void MoveToNextLine();
    /// Move to the next non-EOL token.
    virtual void MoveToNextToken();
    /// Skip spaces within the current line.
    void SkipSpacesInLine();

  protected:
    /// Pathname of the file.
    PathStr m_pathname;
    /// File descriptor (NULL if file hasn't been opened).
    FILE *m_fd;

  private:
    /// @name Members set in constructors
    //@{
    /// Comment mark. End of string from this separator is ignored.
    const char *m_comment;
    /// The length of the comment mark.
    int m_com_mark_len;
    //@}

  private:
    /// @name Working members
    //@{
    /// Read area (not NULL when file is opened).
    CHAR *m_read_area;
    /// Current position within the buffer (not NULL when file is opened).
    CHAR *m_cursor;
    // Current line number.
    int m_line_no;
    // Flag of the end of file reaching.
    bool m_eof;
    //@}

  }; // class InputFile

INTEGRA_NAMESPACE_END

#endif

