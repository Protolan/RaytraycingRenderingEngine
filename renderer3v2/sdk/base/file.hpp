/// @file
///
/// @brief Declarations of File and InFile classes.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01

#ifndef _KLBC_FILE_HPP_
#define _KLBC_FILE_HPP_

#include "export.h"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Envelope for CRT type.
typedef fpos_t FPOS_T;

class IUser;

/// Implements operations on files.
class File
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE File();
    /// Constructor, sets the full path to the file.
    INTAPI_BASE File(const PathStr &full_pathname, IUser *user = NULL);
    /// Copy constructor.
    INTAPI_BASE File(const File &file);
    // Destructor.
    INTAPI_BASE virtual ~File();
    //@}

  public:
    /// @name Opening and closing
    //@{
    /// Open the file.
    INTAPI_BASE OKAY Open(const char *mode);
    /// Close the file.
    INTAPI_BASE OKAY Close();
    /// Check - is file opened or not.
    INTAPI_BASE bool Opened();
    //@}

  public:
    /// @name Reading and writing
    //@{
    /// Read one char
    INTAPI_BASE virtual int GetCh();
    /// Read a string from the file to a Str object.
    INTAPI_BASE OKAY ReadStr(Str &str);
    /// Read a string from the file to a Str object with restriction of string length.
    INTAPI_BASE OKAY ReadStr(Str &str, int max_length);
    /// Read a string from the file to a char buffer.
    INTAPI_BASE OKAY ReadStr(char *out_buff, int len_buff, bool *out_nl = NULL);
    /// Read data from the binary file to a BYTE buffer.
    INTAPI_BASE void Read(BYTE *out_buff, SIZE_T len_buff, SIZE_T &out_size);
    /// Formatted input from the file.
    INTAPI_BASE int Scanf(const CHAR *format, ...);
    /// Write a string from a Str object to the text file.
    INTAPI_BASE void WriteStr(const Str &str, bool newline = true);
    /// Write a string from a char buffer to the text file.
    INTAPI_BASE void WriteStr(const char *out_buff, bool newline = true);
    /// Write data from a BYTE buffer to the binary file.
    INTAPI_BASE void Write(const BYTE *buff, SIZE_T size);
    /// Formatted output to the file.
    INTAPI_BASE int Printf(const CHAR *format, ...);
    /// Flush a file stream.
    INTAPI_BASE void Flush();
    //@}

  public:
    /// @name Operations with path
    //@{
    /// Get full pathname of the file.
    INTAPI_BASE const PathStr &PathName() const;
    /// Delete the file.
    INTAPI_BASE OKAY Remove();
    /// Rename the file.
    INTAPI_BASE OKAY Rename(const PathStr &new_file_name);
    /// Detect the error during file operation.
    INTAPI_BASE bool IsError();
    //@}

  public:
    /// @name Copying
    //@{
    /// Copy file.
    INTAPI_BASE static OKAY Copy(const PathStr &from,
                                 const PathStr &to,
                                 IUser *user = NULL);
    /// Copy the file with creation of destination directories.
    INTAPI_BASE static OKAY CopyWithDir(const PathStr &name_from,
                                        const PathStr &name_to,
                                        IUser *user = NULL);
    //@}
  public:

    /// Compare two files.
    INTAPI_BASE static bool Compare(const PathStr &path1, const PathStr &path2);
    /// Get the file length.
    INTAPI_BASE INT64 GetLength();

  public:
    /// @name Operations with the file pointer
    //@{
    /// Move the pointer to a new location relative to the beginning of the file.
    INTAPI_BASE OKAY Seek(INT64 offset);
    /// Move the pointer to a new location relative to its current position.
    INTAPI_BASE OKAY SeekCur(INT64 offset);
    /// Move the pointer to a new location relative to the end of file.
    INTAPI_BASE OKAY SeekEnd(INT64 offset);

    /// Get the current pointer position.
    INTAPI_BASE OKAY GetPos(FPOS_T &pos);
    /// Set the current pointer position.
    INTAPI_BASE OKAY SetPos(const FPOS_T &pos);
    /// Test for end of file.
    INTAPI_BASE bool IsEndOfFile();
    //@}

  protected:
    /// @name Enumerations
    //@{
    /// Text/binary mode.
    enum TypeMode
      {
      /// Text mode.
      MODE_TEXT,
      /// Binary mode.
      MODE_BINARY
      };
    /// Access mode.
    enum AccessMode
      {
      /// Read access.
      MODE_READ,
      /// Write access.
      MODE_WRITE,
      /// Read-write access.
      MODE_READ_WRITE
      };
    //@}

  protected:
    /// @name Protected members
    //@{
    /// File descriptor.
    FILE *fd;
    /// Text/binary mode.
    TypeMode mode_tb;
    /// Read/write mode.
    AccessMode mode_rw;
    /// User object to use for end user notification.
    IUser *user;
    //@}

  private:
    /// @name Private members
    //@{
    /// Full pathname to file.
    PathStr name;
    //@}
  }; // class File


/// Implements a token by token input from text files.
class InFile
  {
  public:
    /// @name Enumerations
    //@{
    /// Stream parsing error.
    enum StreamParseErr
      {
      /// No error, the token has been read in successfully.
      SP_NOERROR      = SUCCESS,
      /// Insufficient memory, file stream error etc.
      SP_GENERR       = -1,
      /// Stream (or curline if reading was allowed in one line only) ended.
      /// Usually while reading bracketed expressions:
      /// the left (opening) bracket is not found. This means that the
      /// first non-delimiter characters do not contain the left bracket.
      /// In derived classes this is also used to signal that the file stream
      /// (or current line if reading was allowed within one line) ended
      /// before encountering a non-delimiter.
      SP_ENDSTREAM    = -2,
      /// Unexpected token. The word is not a valid textual representation
      /// of the expected token. For example, while attempting
      /// to read in an integer, the cursor is in between "SigmaValues" (an
      /// array of doubles);
      SP_IMPROPERWORD = -3,
      /// Missed left bracket. While reading a bracketed expressions,
      /// the left (opening) bracket is not found.
      SP_NOOPENING    = -4,
      /// Missed right bracket. While reading a bracketed expressions,
      /// the right (closing) bracket is not found.
      SP_NOCLOSURE    = -5,
      /// Missed "enddef".
      SP_NOENDDEF     = -6,
      /// The line does not begin with the given keyword.
      /// While reading a "named" variable like here:
      ///
      /// @code
      /// sigma 0.1234 ;; sigma in keyword (a variable name)
      /// @endcode
      ///
      /// the first word differs from the given keyword.
      SP_NOKEYWORD    = -7,
      /// Improper array dimension (when e.g. matrix rows are
      /// of different length).
      SP_BADDIMENSION = -8,
      /// The whole section (i.e. a file block between some keyword regarded
      /// as a section name opening it and @b enddef closing it) is absent.
      ///
      /// @code
      /// TabulatedFunction ;; the section for object TabulatedFunction begins
      ///   Abscissae 0.1 0.2 0.3 enddef ;; x values
      ///   Values      1.1 5.2 8.3 enddef ;; f(x) values
      /// enddef ;; end object TabulatedFunction
      /// @endcode
      ///
      /// This is used in derived classes to show that what begins from
      /// the cursor is not the expected information about the given object;
	  /// this code means that there is no syntax error and we may try to read
      /// another object from this position.
      SP_NOSECTION    = -9,
      /// Syntax is OK but a numerical value is out of range.
      SP_BADVALUE     = -10,
      /// Syntax is OK but the read-in data is internally controversial.
      SP_INCONSISTENT = -11
      };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor, initializes by a full path to the file.
    INTAPI_BASE InFile(const PathStr &pathname, IUser *user = NULL);
    /// Constructor, initializes by the given file.
    INTAPI_BASE InFile(const File &src_file);
    /// Copy constructor.
    INTAPI_BASE InFile(const InFile &src_file);
    /// Destructor.
    INTAPI_BASE virtual ~InFile();
    //@}

  public:
    /// @name Opening and closing
    //@{
    /// Open the file.
    INTAPI_BASE virtual OKAY Open(bool txt_mode = true);
    /// Close the file.
    INTAPI_BASE virtual OKAY Close();
    //@}

  public:
    /// @name Line input/output
    //@{
    /// Read (the rest of) the line.
    INTAPI_BASE virtual OKAY ReadLine(Str &str);
    /// Get the current line number.
    INTAPI_BASE int LineNo() const;
    /// Get text specifying the current position in the file.
    INTAPI_BASE Str Where() const;
    /// Check for the end of line.
    INTAPI_BASE virtual bool CurLineEnded();
    //@}

  public:
    /// @name Read services
    //@{
    /// Read one char
    INTAPI_BASE virtual int GetCh();

    /// Skip separators.
    INTAPI_BASE void SkipSeparators();
    /// Skip a fixed text.
    INTAPI_BASE OKAY SkipText(const char *text);
    /// Skip a fixed text ignoring case.
    INTAPI_BASE OKAY SkipTextIgnCase(const char *text);

    /// Skip up to the end of the current line.
    INTAPI_BASE OKAY SkipCurLine();

    /// Read the specified word.
    INTAPI_BASE virtual OKAY Read(const char *word, bool respect_case = true);
    /// Read a text.
    INTAPI_BASE virtual OKAY Read(Str &val);
    /// Read an integer value.
    INTAPI_BASE virtual OKAY Read(int &val);
    /// Read a float value.
    INTAPI_BASE virtual OKAY Read(float &val);
    /// Read a double value.
    INTAPI_BASE virtual OKAY Read(double &val);
    /// Read a logical value.
    INTAPI_BASE virtual OKAY Read(bool &val);
    /// Read a bracketed text.
    INTAPI_BASE virtual StreamParseErr Read(Str &val,
                                    const char *left, const char *right,
                                    bool wrap = false);
    /// Read a bracketed integer value.
    INTAPI_BASE virtual StreamParseErr Read(int &val,
                                    const char *left, const char *right,
                                    bool wrap = false);
    /// Read a bracketed float value.
    INTAPI_BASE virtual StreamParseErr Read(float &val,
                                    const char *left, const char *right,
                                    bool wrap = false);
    /// Read a bracketed double value.
    INTAPI_BASE virtual StreamParseErr Read(double &val,
                                    const char *left, const char *right,
                                    bool wrap = false);
    //@}

  public:
    /// @name Other file services
    //@{
    /// Delete the file.
    INTAPI_BASE virtual OKAY Remove();
    /// Rename the file.
    INTAPI_BASE virtual OKAY Rename(const PathStr &new_file_name);
    /// Detect the error during file operation.
    INTAPI_BASE virtual bool IsError();
    /// Get the full pathname to the file.
    INTAPI_BASE virtual PathStr PathName() const;
    /// Set separators.
    INTAPI_BASE void SetSeparators(char *str);
    /// Set Comments separator.
    INTAPI_BASE void SetCommentsSeparator(char *str);
    /// Get the current pointer position.
    INTAPI_BASE OKAY GetPos(FPOS_T &pos);
    /// Set the current pointer position.
    INTAPI_BASE OKAY SetPos(const FPOS_T &pos);
    /// Test for end of file.
    INTAPI_BASE bool IsEndOfFile();
    //@}

  protected:
    /// @name Auxilary protected services
    //@{
    /// Make sure that the current line is read.
    INTAPI_BASE virtual OKAY ReadCurLine();
    /// Check if the character is an in-line separator.
    inline virtual bool IsSeparator(char ch);
    /// Read a real number and convert it.
    INTAPI_BASE OKAY ReadReal(Str &buf);
    /// Get the whole current line.
    INTAPI_BASE Str &CurLine();
    /// Discard tail.
    INTAPI_BASE virtual OKAY TruncateCurLineFrom(int pos);
    //@}

  private:
    /// @name Private members
    //@{
    /// Source file.
    File m_file;
    /// Current line read.
    Str m_line;
    /// Current position within the line.
    int m_pos;
    // Current line number.
    int m_line_no;
    /// List of separators.
    Str m_separators;
    /// Comments separator. End of string from this separator is ignored.
    Str m_comm_sep;
    //@}
  }; // class InFile

//////////////////////////////////////////////////////////////////////////////
/// Check if the character is an in-line separator.
/// @param[in] ch - A character to check.
/// @return True if the character is a separator; false otherwise.
bool InFile::IsSeparator(char ch)
  {
  return (strrchr(m_separators, ch) != NULL);
  }

INTEGRA_NAMESPACE_END

#endif

