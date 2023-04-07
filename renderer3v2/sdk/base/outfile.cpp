/// @internal
/// @file
///
/// @brief Definition of OutFile class.
///
/// @author Ose - Sergey Ershov , '01.12.10
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004

#include <stdarg.h>

#include <integra.hpp>
#include "str.hpp"
#include "file.hpp"
#include "outfile.hpp"

INTEGRA_NAMESPACE_START

/**
@class OutFile base/outfile.hpp

Output to a text file with indentation.

The class OutFile is used for output to a text file (i.e. it offers only
the "writing" part of the class File). Besides that it allows setting of
a margin offset so that all lines are automatically indented.
This is helpful while saving nested objects with indentation of children.

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Open the file for writing in text mode.
/// @return SUCCESS or FAILURE (opening error).
OKAY OutFile::Open()
  {
  if (file.Open("w") != SUCCESS)
    return FAILURE;
  indent = "";
  begline = true;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Close the file.
///
/// File must be opened. Debug version asserts if this condition is violated.
/// @return
/// - SUCCESS (if there were no problems when working with the file and
/// the file was closed successfully);
/// - FAILURE (either I/O error while working with file or closing error).
OKAY OutFile::Close()
  {
  indent.Resize(0);
  begline = true;
  return file.Close();
  }

//////////////////////////////////////////////////////////////////////////////
/// Formatted output to file.
///
/// Analog of the fprintf() function, the printed text is preceded with blanks
/// so that it begins from the left margin.
/// For allowed formats please refer to fprintf() manual.
/// @param[in] str_format Format string;
/// may not be NULL, debug version asserts it.
/// @return The number of characters written (without indents),
/// not including the terminating null character,
/// or a negative value if an output error occurs.
int OutFile::Printf(const char *str_format, ...)
  {
  char str[Str::MAX_LENGTH + 1];

  va_list arg_list;
  va_start(arg_list, str_format);
  int res = _vsnprintf(str, Str::MAX_LENGTH,
                       (const char *)str_format, arg_list);
  va_end(arg_list);
  str[Str::MAX_LENGTH] = '\0';
  WriteStr(str, false);
  return res;
  }  // Printf()

//////////////////////////////////////////////////////////////////////////////
/// Write a string from a char buffer to a text file.
///
/// The method writes to the file data from the buffer till
/// (excluding) the terminating null character.
/// @note The file must be opened for writing in the text mode.
/// Debug version asserts it.
/// @param[in] text - Buffer with data to write;
/// may not be NULL, debug version asserts it.
/// @param[in] newline - Whether to add the new line character.
/// Default value is true.
void OutFile::WriteStr(const char *text, bool newline)
  {
  // indent future output by inserting blanks:
  // if this is first output in line
  if (begline)
    InsertIndent();

  // write string and end-of-line
  file.WriteStr(text, newline);
  if (newline || strchr(text, '\n'))
    begline = true; // next line will start from left margin
  else
    begline = false; // next writing will APPEND to this line
  }  // WriteStr()

//////////////////////////////////////////////////////////////////////////////
/// Insert necessary number of blanks.
///
/// The method inserts blanks so that subsequent file.Printf()... begins
/// from the desired left margin. The file must be open.
void OutFile::InsertIndent()
  {
  if (!indent.IsNull() && indent.Length() > 0)
    file.WriteStr(indent, false); // do not pad with newline!
  }

INTEGRA_NAMESPACE_END
