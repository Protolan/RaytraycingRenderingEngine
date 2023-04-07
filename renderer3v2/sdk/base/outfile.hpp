/// @file
///
/// @brief Declaration of OutFile class.
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Ose - Sergey Ershov , '01.12.10

#ifndef _KLBS_OUTFILE_HPP_
#define _KLBS_OUTFILE_HPP_

#include "export.h"
#include "file.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Output to a text file with indentation.
class OutFile
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor by a full path to the file.
    inline OutFile(const PathStr &pathname, IUser *user = NULL);
    /// Constructor by File object.
    inline OutFile(const File &src_file);
    /// Copy constructor.
    inline OutFile(const OutFile &src_file);
    /// Destructor.
    inline ~OutFile();
    //@}

  public:
    /// @name Opening, closing
    //@{
    /// Open the file for writing in the text mode.
    INTAPI_BASE OKAY Open();
    /// Close the file.
    INTAPI_BASE OKAY Close();
    //@}

  public:
    /// @name Left margin
    //@{
    /// Get the left margin.
    inline int  LeftMargin() const;
    /// Set the left margin.
    inline void SetLeftMargin(int n);
    //@}

  public:
    /// @name Output to file
    //@{
    /// Formatted output to the file.
    INTAPI_BASE int Printf(const char *format, ...);
    /// Write a string from a char buffer to the text file.
    INTAPI_BASE void WriteStr(const char *text, bool newline = true);
    //@}

  public:
    /// @name Other file services
    //@{
    /// Flush a file stream.
    inline void Flush();
    /// Delete the file.
    inline OKAY Remove();
    /// Rename the file.
    inline OKAY Rename(const PathStr &new_file_name);
    /// Get a full pathname to the file.
    inline PathStr PathName() const;
    /// Detect the error during a file operation.
    inline bool IsError();
    //@}

  protected:
    /// Insert a necessary number of blanks.
    INTAPI_BASE void InsertIndent();

  private:
    /// File object.
    File file;
    /// String of blanks whose length equals to left margin.
    Str  indent;
    /// Whether we are at line begin so the next printing is to be indented.
    bool begline;
  };  // class OutFile

//////////////////////////////////////////////////////////////////////////////
// Inline methods of OutFile class.

//////////////////////////////////////////////////////////////////////////////
/// Constructor by a full path to the file.
/// @param[in] pathname - A full path to the file.
/// @param[in] user - A pointer to user object for errors reporting;
/// if @c NULL, errors will not be reported.
OutFile::OutFile(const PathStr &pathname, IUser *user) :
  file(pathname, user)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor by a File object.
/// @param[in] src_file - A File object.
/// @note File must not be opened. Debug version asserts this.
OutFile::OutFile(const File &src_file)
  : file(src_file)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src_file - A source object.
/// @note The file must not be open. Debug version asserts this.
OutFile::OutFile(const OutFile &src_file)
  : file(src_file.file)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The file will be closed if it was open.
OutFile::~OutFile()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the left margin.
/// @return The left margin.
int OutFile::LeftMargin() const
  {
  return indent.Length();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the left margin.
///
/// The method sets a new left margin (indentation), i.e. the number of blanks
/// inserted at the beginning of each line.
/// @param[in] n - A new left margin.
void OutFile::SetLeftMargin(int n)
  {
  indent.Resize(n);
  }

//////////////////////////////////////////////////////////////////////////////
/// Flush a file stream.
///
/// The file must be open. Debug version asserts it.
void OutFile::Flush()
  {
  file.Flush();
  }

//////////////////////////////////////////////////////////////////////////////
/// Delete the file.
///
/// Previously, the file will be closed if it was open.
/// The file may not be opened. Debug version asserts it.
/// @return SUCCESS or FAILURE.
OKAY OutFile::Remove()
  {
  return file.Remove();
  }

//////////////////////////////////////////////////////////////////////////////
/// Rename the file.
///
/// Previously, the file will be closed if it was open.
/// The file may not be open. Debug version asserts it.
/// @param[in] new_file_name - A new file name.
/// @return SUCCESS or FAILURE.
OKAY OutFile::Rename(const PathStr &new_file_name)
  {
  return file.Rename(new_file_name);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the full pathname to the file.
/// @return The full pathname to the file.
PathStr OutFile::PathName() const
  {
  return file.PathName();
  }

//////////////////////////////////////////////////////////////////////////////
/// Detect the error during a file operation.
/// @return A result of the detection.
bool OutFile::IsError()
  {
  return file.IsError();
  }

INTEGRA_NAMESPACE_END
#endif
