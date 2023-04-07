/// @internal
/// @file
///
/// @brief Declaration of AnnInFile class.
///
/// @author Ose - Sergey Ershov '01.12.10
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004

#include <integra.hpp>

#include <base/user.hpp>

#include "annfile.hpp"
#include "arrays.hpp"
#include "file.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Max count of lines in the text file.
static const int MAX_LINES = 1000000;


/**
@class AnnInFile base/annfile.hpp

Reads annotated text files, with comments and keywords.

The class AnnInFile is a syntax parser enabling to read usual Integra
syntax blocks like

@code
sigma 65.0 ;; angle of incidence
Reflectance ;; reflectance as a function of wavelength
    1.0 2.0 3.0 4.0
    5.0 6.0 7.0 8.0
enddef ;; end of reflectance
@endcode

from a text file.

It understands end-line comments ";;" and can also read "named variables"
(preceded by keywords) of the basic InFile types.
It also allows reading of arrays or tables (arrays of arrays)
of those types in one call.

@sa @ref base_mainpage
**/

/**
@enum AnnInFile::StopToken

Stop criteria for array reading - determines when array reading stops.
Array reading functions have the parameter which determines the criterion
of successful end of reading:

<table>
<tr>
  <td>Value of <b>stopat</b></td><td>Reading is to stop</td>
</tr>
<tr>
  <td>0 .. 16,777,215</td><td>on reading <b>stopat</b> elements</td>
</tr>
<tr>
  <td>STOPAT_ENDLINE</td><td>at the end of the current line</td>
</tr>
<tr>
  <td>STOPAT_ENDDEF</td><td>at the first <b>enddef</b></td>
</tr>
<tr>
  <td>STOPAT_ENDDEF | STOPAT_ENDLINE</td>
  <td>at <b>enddef</b> which must be on the current line</td>
</tr>
</table>
**/

/**
@enum AnnInFile::WrapModeEnum

Wrap mode - determines if a bracketed expression or an array of them
can continue across lines.

Here is a simple example:

<table>
<tr>
  <td><b>WRAP_NONE</b></td>
  <td><b>WRAP_ENDLINE</b></td>
  <td><b>WRAP_ELEMENT</b></td>
</tr>
<tr>
  <td>| (1, 1) ;; comment</td>
  <td>| ;; line ends<br>(1, 1) ;; continued</td>
  <td>| ;; line ends<br>(1, ;; expression is on the next line<br>
1) ;; and wraps across it</td>
</tr>
<tr>
  <td>| (1, 1) (1, 2) ;; comment</td>
  <td>| ;; line ends<br>(1, 1) (1, 2) ;; expressions are on the next line<br>
(3, 1) (5, 2) ;; continued</td>
  <td>| ;; line ends<br>(1, 1) (1, ;; expressions are on the next line<br>
1) (6,0) ;; continued<br>(3, 1) (5, 2) ;; continued</td>
</tr>
</table>

The brackets are '(' and ')'. The top row is for a single ("scalar") element,
the bottom row is for an array. The '' marks the end of current line,
the vertical line '|' marks "cursor" (the current position).
**/


//////////////////////////////////////////////////////////////////////////////
/// Check the next keyword.
///
/// The method checks if the next word (after the cursor) matches a given
/// text; and if so, reads it in. If the current line ended (there is nothing
/// but delimiters), reads in the next line.
///
/// If a keyword is absent or not matched, the cursor is left before the
/// first non-delimiter. Otherwise it is moved past the keyword.
///
/// Case sensitivity of the match is set by the CaseSensitivity() method.
/// @param[in] keyword - An expected keyword.
/// @return SUCCESS if the expected keyword matched; FAILURE otherwise.
/// @note The method sets the "last error".
OKAY AnnInFile::Read(const char *keyword)
  {
  int ierr;
  // if line ends we can look for keyword on the next
  // (affects Read(void) behaviour)
  WrapMode(true);

  ierr = InFile::Read(keyword, respect_case);
  if (ierr == SUCCESS)
    SetLastError(SP_NOERROR);
  else
    SetLastError(SP_NOKEYWORD);

  return ierr;
  }  // Read(const char *keyword)

//////////////////////////////////////////////////////////////////////////////
/// Read a text value.
///
/// The method reads the next word delimited with blanks or tabs,
/// starting from the current position. If this line ends (there is
/// nothing but delimiters), the method reads in the next line if allowed
/// by the parameter @a wrap; but the whole word must be within one line.
///
/// If failed, the cursor is left before the first non-delimiter.
/// Else it is moved past the word read.
/// @param[out] val - The read-in word.
/// @param[in] wrap - If @b true, allows reading of the next line if the current
/// one ends.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character (on this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - this word can not be read in into a Str variable.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(Str &val, bool wrap)
  {
  // if line ends, can we take next one? (affects Read(void) behaviour)
  WrapMode(wrap);

  // If NOT allowed to go to next line(s) while current ended...
  if (!CanTakeNextLine() && CurLineEnded())
    return SetLastError(SP_ENDSTREAM);

  // now try to read from current line and current position
  if (InFile::Read(val) == SUCCESS)
    return SetLastError(SP_NOERROR);
  else
    {
    // if we failed to read cursor was not moved. If in the meanwhile
    // curline is empty (cursor is at its end) then we simply
    // exhauset valid read area (stream or line if allowed to read within
    //line only)
    if (CurLineEnded())
      return SetLastError(SP_ENDSTREAM);
    else
      return SetLastError(SP_IMPROPERWORD);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a bool value.
///
/// The method reads a logical value delimited with blanks or tabs,
/// starting from the current position.
/// If this line ends (there is nothing but delimiters), the method
/// reads in the next line if allowed by the parameter @a wrap,
/// but the whole word must be within ONE line.
///
/// A logical value in a file is represented by the words
/// <b>true/false, on/off, yes/no, enabled/disabled</b>
/// (case insensitive).
///
/// If failed, the cursor is left before the first non-delimiter.
/// Else it is moved past the word read.
/// @param[out] val - The read-in value.
/// @param[in] wrap - If @b true, allows reading of next line if current one ended.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - this word can not be read in into a bool variable.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(bool &val, bool wrap)
  {
  // if line ends, can we take next one? (affects Read(void) behaviour)
  WrapMode(wrap);

  // If NOT allowed to go to next line(s) while current ended...
  if (!CanTakeNextLine() && CurLineEnded())
    return SetLastError(SP_ENDSTREAM);

  // now try to read from current line and current position
  if (InFile::Read(val) == SUCCESS)
    return SetLastError(SP_NOERROR);
  else
    {
    // if we failed to read cursor was not moved. If in the meanwhile
    // curline is empty (cursor is at its end) then we simply
    // exhauset valid read area (stream or line if allowed to read within
    // line only)
    if (CurLineEnded())
      return SetLastError(SP_ENDSTREAM);
    else
      return SetLastError(SP_IMPROPERWORD);
    }
  }


//////////////////////////////////////////////////////////////////////////////
/// Read an integer value.
///
/// The method reads an integer value delimited with blanks or tabs,
/// starting from the current position.
/// If this line ends (there is nothing but delimiters), the method
/// reads in next line if allowed by the parameter @a wrap;
/// but the value must be within ONE line.
///
/// If failed, the cursor is left before the first non-delimiter.
/// Else it is moved past the value read.
/// @param[out] val - The read-in value.
/// @param[in] wrap - If @b true, allows reading the next line if the current
/// one ended.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - this value can not be read in into an integer variable.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(int &val, bool wrap)
  {
  // if line ends, can we take next one? (affects Read(void) behaviour)
  WrapMode(wrap);

  // If NOT allowed to go to next line(s) while current ended...
  if (!CanTakeNextLine() && CurLineEnded())
    return SetLastError(SP_ENDSTREAM);

  // now try to read from current line and current position
  if (InFile::Read(val) == SUCCESS)
    return SetLastError(SP_NOERROR);
  else
    {
    // if we failed to read cursor was not moved. If in the meanwhile
    // curline is empty (cursor is at its end) then we simply
    // exhauset valid read area (stream or line if allowed to read within
    // line only)
    if (CurLineEnded())
      return SetLastError(SP_ENDSTREAM);
    else
      return SetLastError(SP_IMPROPERWORD);
    }
  }



//////////////////////////////////////////////////////////////////////////////
/// Read a double value.
///
/// The method reads a double value delimited with blanks or tabs,
/// starting from the current position.
/// If this line ends (there is nothing but delimiters), the method
/// reads in the next line if allowed by the parameter @a wrap;
/// but the value must be within ONE line.
///
/// If failed, the cursor is left before the first non-delimiter.
/// Else it is moved past the value read.
/// @param[out] val - The read-in value.
/// @param[in] wrap - If @b true, allows reading of the next line if the
/// current one ended.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - this value can not be read in into a double variable.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(double &val, bool wrap)
  {
  // if line ends, can we take next one? (affects Read(void) behaviour)
  WrapMode(wrap);

  // If NOT allowed to go to next line(s) while current ended...
  if (!CanTakeNextLine() && CurLineEnded())
    return SetLastError(SP_ENDSTREAM);

  // now try to read from current line and current position
  if (InFile::Read(val) == SUCCESS)
    return SetLastError(SP_NOERROR);
  else
    {
    // if we failed to read cursor was not moved. If in the meanwhile
    // curline is empty (cursor is at its end) then we simply
    // exhauset valid read area (stream or line if allowed to read within
    // line only)
    if (CurLineEnded())
      return SetLastError(SP_ENDSTREAM);
    else
      return SetLastError(SP_IMPROPERWORD);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed text.
///
/// The method reads a text (with whitechars) which is bracketed with the
/// @a left and @a right brackets.
/// The brackets are not included to the output, as well as newlines. If there
/// are nested brackets like "<abc d<ef g]h ijk]" where brackets
/// are < and ], then reading goes up to the right bracket,
/// which matches the opening one, i.e. the 2nd ] here.
///
/// The brackets are case insensitive.
///
/// If the left bracket is NULL, then reading starts from the current position.
///
/// Whitechars are skipped before the left bracket
/// or, if it is NULL, before the first non-delimiter.
///
/// @code
/// Read(str, "\"", "\"", true);
/// @endcode
///
/// reads a quoted text and then discards quotes.
///
/// @code
/// Read(str, NULL, "end", true)
/// @endcode
///
/// reads until "end"; the latter is discarded.
/// @param[out] val - The read-in text.
/// @param[in] left - A opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap
/// - WRAP_NONE - confines to the current line.
/// - WRAP_ENDLINE - allows reading of a new line when the current one is
/// empty but the expression can not split across lines.
/// - WRAP_ELEMENT - even the expression may split across lines.
/// @return
/// - SP_NOERROR - if successful.
/// - SP_ENDSTREAM - no a non-delimiter character
/// (new line is read, if the wrap mode is set).
/// - SP_NOOPENING - no @a left bracket found
/// (the first non-delimiter symbol differs from @a left).
/// - SP_NOCLOSURE - no @a right bracket is found.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(Str &val, const char *left,
                                       const char *right, WrapModeEnum wrap)
  {
  // if line ends, can we take next one? (affects Read(void) behaviour)
  WrapMode(wrap != 0);
  return SetLastError(InFile::Read(val, left, right, (wrap == WRAP_ELEMENT)));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a text value preceded with a keyword.
///
/// The method reads a text value preceded with a keyword.
/// First, it attempts to read the keyword from the current position
/// (new lines are skipped if needed). If fails, leaves the
/// cursor at the first non-delimiter after the current position, if
/// succeeds, moves it past the keyword.
///
/// Case sensitivity of the match is controlled by the CaseSensitivity() call.
///
/// If the keyword was read, the method attempts to read in a value
/// after the keyword. It may start on the next line if wrap=true.
///
/// If the reading succeeds, the cursor is moved past the read in value.
/// Otherwise it is left before the first non-delimiter after
/// the keyword.
/// @param[in] keyword - A keyword.
/// @param[out] val - The read-in value.
/// @param[in] wrap - If true, allows reading of the next line.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - the following keyword is not a valid text value.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, Str &val,
                                       bool wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(val, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a bool value preceded with a keyword.
///
/// The method reads a logical value preceded with a keyword.
/// First, it attempts to read a keyword from the current position
/// (new lines are read in if needed). If fails, leaves
/// the cursor at the first non-delimiter after the current position, if
/// succeeds, moves it past the keyword.
///
/// A logical value in a file is represented by the words
/// <b>true/false, on/off, yes/no, enabled/disabled</b>
/// (case insensitive).
///
/// Case sensitivity is controlled by the CaseSensitivity() call.
///
/// If the keyword was read, the method attempts to read in a value
/// after the keyword. It may start on the next line if wrap=true.
///
/// If reading succeeds, the cursor is moved past the read in value.
/// Otherwise it is left before the first non-delimiter after
/// the keyword.
/// @param[in] keyword - A keyword.
/// @param[out] val - The read-in value.
/// @param[in] wrap - Allows reading of the next line.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - the following keyword is not a valid bool value.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, bool &val,
                                       bool wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(val, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an integer value preceded with a keyword.
///
/// The method reads an integer value preceded with a keyword.
/// First, it attempts to read the keyword from the current position
/// (new lines are read in if needed). If fails, leaves the
/// cursor at the first non-delimiter after the current position; if
/// succeeds, moves it past the keyword.
///
/// Case sensitivity is controlled by the CaseSensitivity() call.
///
/// If the keyword was read, the method attempts to read in a value
/// after the keyword. It may start on the next line if wrap=true.
///
/// If the reading succeeds, the cursor is moved past the read in value.
/// Otherwise it is left before the first non-delimiter after
/// the keyword.
/// @param[in] keyword - A keyword.
/// @param[out] val - The read-in value.
/// @param[in] wrap - Allows reading the next line.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - the following keyword is not a valid integer value.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, int &val,
                                       bool wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(val, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a double value preceded with a keyword.
///
/// The method reads a double value preceded with a keyword.
/// First, it attempts to read a keyword from the current position
/// (new lines are read in if needed). If fails, leaves
/// the cursor at the first non-delimiter after the current position, if
/// succeeds, moves it past the keyword.
///
/// Case sensitivity is controlled by the CaseSensitivity() call.
///
/// If the keyword was read, the method attempts to read in a value
/// after the keyword. It may start on the next line if wrap=true.
///
/// If the reading succeeds, the cursor is moved past the read in value.
/// Otherwise it is left before the first non-delimiter after
/// the keyword.
/// @param[in] keyword - A keyword.
/// @param[out] val - The read-in value.
/// @param[in] wrap - Allows reading of the next line.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character (in this line
/// when wrap=false or in the whole file when wrap=true).
/// - SP_IMPROPERWORD - the following keyword is not a valid double value.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, double &val,
                                       bool wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(val, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed text preceded with a keyword.
///
/// The method first attempts to read the keyword from the current position
/// (new lines are read in if needed). If fails, leaves
/// the cursor at the first non-delimiter after the current position, if
/// succeeds, moves it past the keyword.
///
/// Case sensitivity is controlled by the CaseSensitivity() call.
///
/// If the keyword was read, the method attempts to read in a bracketed
/// expression after the keyword. It may start on the next line if wrapping
/// is enabled.
///
/// To learn about bracketed expression, see
///
/// Read(Str &val, const char *left, const char *right, WrapModeEnum wrap)
///
/// In short, brackets are any strings which bound an expression
/// e.g. "def" and "enddef". If the left bracket is NULL, it is ignored.
/// Brackets are not included in the output expression.
/// @param[in] keyword - A keyword.
/// @param[out] val - The read-in text.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap -
/// - WRAP_NONE - confines to the current line.
/// - WRAP_ENDLINE - allows reading of a new line when the current one is empty
/// but expression can not split across lines.
/// - WRAP_ELEMENT - even expression may split across lines.
/// @return
/// - SP_NOERROR - if successful.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOOPENING - no left bracket found
/// (first non-delimiter symbol differ from it).
/// - SP_NOCLOSURE - bracket is not closed.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, Str &val,
                               const char *left, const char *right,
                               WrapModeEnum wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(val, left, right, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of text values.
///
/// The method reads in array of strings (words) until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// Array elements are words i.e. strings w/o whitechars which
/// are separated from each other with the blank, tab or end-of-line characters.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of words.
/// @param[in]  stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no @b enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a Str variable
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
/// @note If STOPAT_ENDDEF was not specified, then @b enddef is read as
/// an ordinary word into an array element.
InFile::StreamParseErr AnnInFile::Read(TArray<Str> &array, int stopat)
  {
  Str    val;
  StreamParseErr ierr;
  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(val, !(stopat & STOPAT_ENDLINE));
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add(val) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of bool values.
///
/// The method reads in an array of bool values until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// A logical value in a file is represented by the words
/// <b>true/false, on/off, yes/no, enabled/disabled</b>
/// (case insensitive).
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of values.
/// @param[in]  stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - the word encountered is not a valid bool value.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<bool> &array, int stopat)
  {
  bool val;
  StreamParseErr ierr;
  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(val, !(stopat & STOPAT_ENDLINE));
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add(val) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of integer values.
///
/// The method reads in an array of integer values until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of values.
/// @param[in]  stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef
///   which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no @b enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - the word encountered is not a valid integer value.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<int> &array, int stopat)
  {
  int    val;
  StreamParseErr ierr;
  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(val, !(stopat & STOPAT_ENDLINE));
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add(val) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of double values.
///
/// The method reads in an array of double values until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of values.
/// @param[in]  stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered is not a valid double value.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<double> &array, int stopat)
  {
  double val;
  StreamParseErr ierr;
  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(val, !(stopat & STOPAT_ENDLINE));
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add(val) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of float values.
///
/// The method reads in an array of float values until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of values.
/// @param[in]  stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered is not a valid float value.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<float> &array, int stopat)
  {
  double val;
  StreamParseErr ierr;
  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(val, !(stopat & STOPAT_ENDLINE));
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add((float)val) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of bracketed text values.
///
/// The method reads in an array of bracketed strings until either the line ends,
/// or @b enddef is encountered or the specified number of elements are read in.
///
/// Array elements are bracketed expression, see
///
/// Read(Str &val, const char *left, const char *right, WrapModeEnum wrap)
///
/// In short, brackets are any strings which bound an expression
/// e.g. "def" and "enddef". If the left bracket is NULL, it is ignored.
/// Brackets are not included in the output expression.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[out] array - The read-in array of strings.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @param[in] wrap
/// - WRAP_NONE - confines to the current line.
/// - WRAP_ENDLINE - allows reading of a new line when the current one is empty
/// but the expression can not split across lines.
/// - WRAP_ELEMENT - even the expression may split across lines.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read if wrap mode is set).
/// - SP_NOOPENING - no left bracket found
/// (the first non-delimiter symbol differ from it).
/// - SP_NOCLOSURE - no closing bracket.
/// - SP_NOENDDEF - no @b enddef (in this line if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - the word encountered that can not be read in
/// into Str variable
/// - SP_GENERROR - insufficient memory to add an element to array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<Str> &array,
                               const char *left, const char *right,
                               int stopat, WrapModeEnum wrap)
  {
  Str    item;
  StreamParseErr ierr;

  if (stopat & STOPAT_ENDLINE)
    wrap = WRAP_NONE;
  else if (wrap == WRAP_NONE) // stopat allows multiline
    wrap = WRAP_ENDLINE;

  array.Allocate(0);
  for ( ; ; )
    {
    if (IsError()) // stream ended or other stream error
      {
      if (array.Length() == 0)
        return SetLastError(SP_NOOPENING);
      else
        return SetLastError(SP_NOENDDEF);
      }

    if ((stopat & STOPAT_ENDDEF) && Read("enddef") == SUCCESS)
      break;

    ierr = Read(item, left, right, wrap);
    if (ierr != SP_NOERROR)
      {
      if (CurLineEnded() && (stopat & STOPAT_ENDDEF))
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (array.Add(item) != SUCCESS)
      return SetLastError(SP_GENERR);

    if (!(stopat & STOPAT_ENDLINE) && !(stopat & STOPAT_ENDDEF)
        && array.Length() == stopat)
      break;

    // reading was allowed from this line only and it ended
    if (CurLineEnded() && (stopat & STOPAT_ENDLINE))
      {
      if (stopat & STOPAT_ENDDEF)
        return SetLastError(SP_NOENDDEF);
      else
        break;
      }
    }
  return SetLastError(SP_NOERROR);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of text values preceded with a keyword.
///
/// The method reads in a keyword-preceded array of string "words"
/// (strings w/o delimiters separated
/// from each other with the blank, tab, end-of-line characters.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in an array after
/// the keyword; see description of
///
/// Read(TArray<Str> &array, int stopat)
///
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of words.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at enddef which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element
/// (in this line, if STOPAT_ENDLINE was specified).
/// - SP_NOENDDEF - no enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a Str variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
/// @note If STOPAT_ENDDEF was not specified, then enddef is read as
/// ordinary word into array element.
InFile::StreamParseErr AnnInFile::Read(const char *keyword, TArray<Str> &array, int stopat)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, stopat));
  }


//////////////////////////////////////////////////////////////////////////////
/// Read an array of bool values preceded with a keyword.
///
/// The method reads in a keyword-preceded array of bool values (in a file
/// they are represented by the case insensitive words
/// true/false, on/off, yes/no, enabled/disabled).
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case, the method attempts to read in an array after
/// the keyword; see description of
///
/// Read(TArray<bool> &array, int stopat)
///
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of values.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - the specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element
/// (in this line, if STOPAT_ENDLINE was specified).
/// - SP_NOENDDEF - no @b enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a bool variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<bool> &array, int stopat)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, stopat));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of integer values preceded with a keyword.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case, the method attempts to read in an array after
/// the keyword; see description of
///
/// Read(TArray<int> &array, int stopat)
///
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of values.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at enddef which must be in current line;
/// - any other integer value - specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// - SP_NOOPENING - no first element
/// (in this line, if STOPAT_ENDLINE was specified).
/// - SP_NOENDDEF - no @b enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into an integer variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<int> &array, int stopat)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, stopat));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of double values, preceded with a keyword.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case, the method attempts to read in an array after
/// the keyword; see description of
///
/// Read(TArray<double> &array, int stopat).
///
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of values.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if necessary).
/// - SP_NOOPENING - no first element
/// (in this line, if STOPAT_ENDLINE was specified).
/// - SP_NOENDDEF - no enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a double variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<double> &array, int stopat)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, stopat));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of float values, preceded with a keyword.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case, the method attempts to read in an array after
/// the keyword; see description of
///
/// Read(TArray<float> &array, int stopat).
///
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of values.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - specified number of elements will be read.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if necessary).
/// - SP_NOOPENING - no first element
/// (in this line, if STOPAT_ENDLINE was specified).
/// - SP_NOENDDEF - no enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a float variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<float> &array, int stopat)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, stopat));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read an array of bracketed text values preceded with a keyword.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in an array after
/// the keyword. It may start on the next line if WRAP_NONE was not specified.
/// Reading goes until the line ends, or @b enddef is encountered or "stopat"
/// elements are read in, as determined by @a stopat.
/// Array elements are bracketed expression, see
///
/// Read(Str &val, const char *left, const char *right, WrapModeEnum wrap)
///
/// In short, brackets are any strings which bound a expression
/// e.g. @b def and @b enddef. If the left bracket is NULL, it is ignored.
/// Brackets are not included in the output expression.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if STOPAT_ENDDEF was specified.
/// @param[in] keyword - A keyword.
/// @param[out] array - The read-in array of expressions.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] stopat - Determines when the reading stops:
/// - STOPAT_ENDLINE - at the end of the current line;
/// - STOPAT_ENDDEF - at @b enddef;
/// - STOPAT_ENDDEF | STOPAT_ENDLINE - at @b enddef which must be in the current line;
/// - any other integer value - the specified number of elements is read.
/// @param[in] wrap
/// - WRAP_NONE - confines to the current line.
/// - WRAP_ENDLINE - allows reading of a new line when the current one is empty
/// but the expression can not split across lines.
/// - WRAP_ELEMENT - even the expression may split across lines.
/// @return
/// - SP_NOERROR - success.
/// - SP_NOKEYWORD - the first word differs from @a keyword.
/// - SP_ENDSTREAM - no non-delimiter character.
/// (new line is read, if the wrap mode is set).
/// - SP_NOOPENING - no first element (in this line, if STOPAT_ENDLINE
/// was specified); or no opening bracket in an element.
/// - SP_NOCLOSURE - no closing bracket (in this line for
/// stopat&STOPAT_ENDLINE)
/// - SP_NOENDDEF - no enddef (in this line, if STOPAT_ENDLINE was specified).
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a Str variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
/// @note If STOPAT_ENDDEF was not specified, then @b enddef is read as
/// an ordinary word into an array element.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword, TArray<Str> &array,
                               const char *left, const char *right,
                               int stopat, WrapModeEnum wrap)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(array, left, right, stopat, wrap));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of text values.
///
/// The method reads in a table (array of arrays) of strings (words).
/// In a file, it is a block up to @b enddef. Each file line
/// is read into the array and becomes a table row. Rows may
/// have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[out] table - The table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a Str variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<TArray<Str> > &table)
  {
  StreamParseErr      ierr;
  TArray<Str> row;
  table.Allocate(0);
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    // skip separators and empty lines before rows.
    WrapMode(true);
    SkipSeparators();

    if (IsError()) // unable to read
      return SetLastError(SP_ENDSTREAM);

    if (Read("enddef") == SUCCESS)
      return SetLastError(SP_NOERROR);

    ierr = Read(row, STOPAT_ENDLINE);
    if (ierr != SP_NOERROR)
      {
      if (ierr == SP_NOOPENING) // no entries up to the end of file!
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (table.Add(row) != SUCCESS)
      return SetLastError(SP_GENERR);
    }
  return SetLastError(SP_NOCLOSURE);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of bool values.
///
/// The method reads in a table (array of arrays) of bool values.
/// In a file, it is a block up to @b enddef. Each file line
/// is read into the array and becomes a table row. Rows may
/// have different dimensions.
/// Each element is a case insensitive word, one of
/// <b>true/false, on/off, yes/no, enabled/disabled</b>.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into bool variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<TArray<bool> > &table)
  {
  StreamParseErr          ierr;
  TArray<bool> row;
  table.Allocate(0);
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    // skip separators and empty lines before rows.
    WrapMode(true);
    SkipSeparators();

    if (IsError()) // unable to read
      return SetLastError(SP_ENDSTREAM);
    if (Read("enddef") == SUCCESS)
      return SetLastError(SP_NOERROR);

    ierr = Read(row, STOPAT_ENDLINE);
    if (ierr != SP_NOERROR)
      {
      if (ierr == SP_NOOPENING) // no entries up to the end of file!
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (table.Add(row) != SUCCESS)
      return SetLastError(SP_GENERR);
    }
  return SetLastError(SP_NOCLOSURE);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read table of integer values.
///
/// The method reads in a table (array of arrays) of integer values.
/// In a file, it is a block up to @b enddef. Each file line
/// is read into the array and becomes a table row. Rows may
/// have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into an integer variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<TArray<int> > &table)
  {
  StreamParseErr      ierr;
  TArray<int> row;
  table.Allocate(0);
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    // skip separators and empty lines before rows.
    WrapMode(true);
    SkipSeparators();

    if (IsError()) // unable to read
      return SetLastError(SP_ENDSTREAM);
    if (Read("enddef") == SUCCESS)
      return SetLastError(SP_NOERROR);

    ierr = Read(row, STOPAT_ENDLINE);
    if (ierr != SP_NOERROR)
      {
      if (ierr == SP_NOOPENING) // no entries up to the end of file!
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (table.Add(row) != SUCCESS)
      return SetLastError(SP_GENERR);
    }
  return SetLastError(SP_NOCLOSURE);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of double values.
///
/// The method reads in a table (array of arrays) of double values.
/// In a file, it is a block up to @b enddef. Each file line
/// is read into the array and becomes a table row. Rows may
/// have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a double variable.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<TArray<double> > &table)
  {
  StreamParseErr         ierr;
  TArray<double> row;
  table.Allocate(0);
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    // skip separators and empty lines before rows.
    WrapMode(true);
    SkipSeparators();

    if (IsError()) // unable to read
      return SetLastError(SP_ENDSTREAM);
    if (Read("enddef") == SUCCESS)
      return SetLastError(SP_NOERROR);

    ierr = Read(row, STOPAT_ENDLINE);
    if (ierr != SP_NOERROR)
      {
      if (ierr == SP_NOOPENING) // no entries up to the end of file!
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (table.Add(row) != SUCCESS)
      return SetLastError(SP_GENERR);
    }
  return SetLastError(SP_NOCLOSURE);
  }


//////////////////////////////////////////////////////////////////////////////
/// Read a table of bracketed text values.
///
/// The method reads in a table (array of arrays) of "bracketed expressions".
/// In file, it is a block up to @b enddef. Each file line
/// is read into the array and becomes a table row. Rows may
/// have different dimensions.
///
/// About bracketed expression, see
///
/// Read(Str &val, const char *left, const char *right, WrapModeEnum wrap)
///
/// In short, brackets are any strings which bound expression
/// e.g. @b def and @b enddef. If the left bracket is NULL, it is ignored.
/// Brackets are not included to the output expression.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[out] table - A table of read values.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOOPENING - no opening bracket in an element.
/// - SP_NOCLOSURE - no closing bracket.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered is not a valid bracketed expression.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(TArray<TArray<Str> > &table,
                               const char *left, const char *right)
  {
  StreamParseErr      ierr;
  TArray<Str> row;
  table.Allocate(0);
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    // skip separators and empty lines before rows.
    WrapMode(true);
    SkipSeparators();

    if (IsError()) // unable to read
      return SetLastError(SP_ENDSTREAM);
    if (Read("enddef") == SUCCESS)
      return SetLastError(SP_NOERROR);

    ierr = Read(row, left, right, STOPAT_ENDLINE);
    if (ierr != SP_NOERROR)
      {
      if (ierr == SP_NOOPENING) // no entries up to the end of file!
        return SetLastError(SP_NOENDDEF);
      else
        return SetLastError(ierr);
      }

    if (table.Add(row) != SUCCESS)
      return SetLastError(SP_GENERR);
    }
  return SetLastError(SP_NOCLOSURE);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of text values preceded with a keyword.
///
/// The method reads in an keyword-preceded table (array of arrays) of
/// words i.e. strings w/o whitechars which in a file are
/// separated from each other with the blank, tab or end-of-line characters.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in a table after
/// the keyword. In a file, a table is a block up to @b enddef. Each
/// file line is read into the array and becomes a table row.
/// Rows may have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
///
/// @param[in] keyword - A keyword.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element in a current line.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read
/// in into a Str variable
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<TArray<Str> > &table)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(table));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of bool values preceded with a keyword.
///
/// The method reads in an keyword-preceded table (array of arrays) of
/// bool values which are represented by the (case insensitive) words
/// <b>true/false, on/off, yes/no, enabled/disabled</b>.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in a table after
/// the keyword. In a file, a table is a block up to @b enddef. Each
/// file line is read into the array and becomes a table row.
/// Rows may have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[in] keyword - A Keyword.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element in a current line.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into a bool variable.
/// - SP_GENERROR - insufficient memory to add an element to the array
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                               TArray<TArray<bool> > &table)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(table));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of integer values preceded with a keyword.
///
/// The method reads in a keyword-preceded table (array of arrays) of
/// integer values.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in a table after
/// the keyword. In a file, a table is a block up to @b enddef. Each
/// file line is read into the array and becomes a table row.
/// Rows may have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[in] keyword - A keyword.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element in a current line.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into an integer variable.
/// - SP_GENERROR - insufficient memory to add an element to the array
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<TArray<int> > &table)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(table));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of double values preceded with a keyword.
///
/// The method reads in a keyword-preceded table (array of arrays) of
/// double values.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in a table after
/// the keyword. In a file, a table is a block up to @b enddef. Each
/// file line is read into the array and becomes a table row.
/// Rows may have different dimensions.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[in] keyword - A keyword.
/// @param[out] table - A table of read values.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// new line is read, if wrap mode is set).
/// - SP_NOOPENING - no first element in a current line.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered that can not be read in
/// into double variable.
/// - SP_GENERROR - insufficient memory to add an element to the array
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<TArray<double> > &table)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(table));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a table of bracketed text values preceded with a keyword.
///
/// The method first attempts to read the keyword (case sensitivity is set by
/// CaseSensitivity()) from the current position; new lines are read in if
/// needed. If fails, leaves the cursor at the first non-delimiter
/// after the current position, if succeeds, moves it past the keyword.
///
/// In the latter case the method attempts to read in a table after
/// the keyword. In a file, a table is a block up to @b enddef. Each
/// file line is read into the array and becomes a table row.
/// Rows may have different dimensions.
///
/// About bracketed expressions, see
///
/// Read(Str &val, const char *left, const char *right, WrapModeEnum wrap)
///
/// In short, brackets are any strings which bound an expression
/// e.g. @b def and @b enddef. If the left bracket is NULL, it is ignored.
/// Brackets are not included to the output expression.
///
/// The method skips separators and empty lines between rows.
///
/// The cursor is left after the last successfully read
/// element or after @b enddef if succeeded.
/// @param[in] keyword - A keyword.
/// @param[out] table - A table of read values.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @return
/// - SP_NOERROR - success.
/// - SP_ENDSTREAM - no non-delimiter character
/// (new line read, if wrap mode is set).
/// - SP_NOOPENING - no first element in a line
/// or no opening bracket in an element.
/// - SP_NOCLOSURE - no closing bracket.
/// - SP_NOENDDEF - no enddef.
/// - SP_IMPROPERWORD - a word encountered is not a valid bracketed expression.
/// - SP_GENERROR - insufficient memory to add an element to the array.
/// @note The method sets the "last error".
InFile::StreamParseErr AnnInFile::Read(const char *keyword,
                                       TArray<TArray<Str> > &table,
                                       const char *left, const char *right)
  {
  if (Read(keyword) != SUCCESS)
    return SetLastError(SP_NOKEYWORD);
  return SetLastError(Read(table, left, right));
  }

//////////////////////////////////////////////////////////////////////////////
/// Read the current line.
///
/// The method overrides the InFile method by discarding the line past the
/// comment ";;" and removing trailing separators.
///
/// This method checks if the current line is not empty (i.e.
/// there is something but delimiters); if so, returns SUCCESS.
/// Otherwise reads new line from the file, truncates it and
/// checks if anything remained etc. until arrives at a non-delimiter.
/// @return SUCCESS / FAILURE (on the file end).
OKAY AnnInFile::ReadCurLine()
  {
  int i, end = -1;

  for ( ; ; )
    {
    // check if the remainder of curline is not only delimiters
    if (!CurLineEnded())
      break;

    // cur. line ended, read next
    // to make InFile::ReadCurLine() to take next line from file
    TruncateCurLineFrom(0);
    if (InFile::ReadCurLine() != SUCCESS)
      return FAILURE;

    // clip comment
    if (CurLine().Find(";;", &end) && end >= 0)
      TruncateCurLineFrom(end);

    // remove trailing separators:
    const Str &curline = CurLine();
    if (curline.IsNull())
      continue;

    for (i = curline.Length() - 1; i >= 0; i--)
      {
      if (!IsSeparator(curline[i]))
        break;
      }
      TruncateCurLineFrom(i + 1);
    }
  return SUCCESS;
  }  // ReadCurLine()

//////////////////////////////////////////////////////////////////////////////
/// Set the last error.
///
/// The method stores description of the error into an internal variable,
/// to be retrieved later by LastError(). The format is like this:
/// "Missed enddef at line 55 col 6 file f:\myfile.dat"
/// @param[in] code - An error code; if not a StreamParseErr member,
/// then error text will name it "unknown error".
/// @return - The error code, passed as a parameter.
InFile::StreamParseErr AnnInFile::SetLastError(int code)
  {
  switch (code)
    {
    case SP_NOERROR:
      lasterr = Tr("No error");
      return (StreamParseErr)code;

    case SP_GENERR:
      lasterr = Tr("Memory of stream error");
      break;

    case SP_ENDSTREAM:
      if (multiline)
        lasterr = Tr("File stream ended");
      else
        lasterr = Tr("Line ended while reading in line scope");
      break;

    case SP_NOOPENING:
      lasterr = Tr("Missed opening bracket");
      break;

    case SP_NOCLOSURE:
      lasterr = Tr("Missed closing bracket");
      break;

    case SP_NOENDDEF:
      lasterr = Tr("Missed enddef");
      break;

    case SP_IMPROPERWORD:
      lasterr = Tr("Unexpected word");
      break;

    case SP_NOKEYWORD:
      lasterr = Tr("Keyword absent");
      break;

    case SP_BADDIMENSION:
      lasterr = Tr("Bad dimension");
      break;

    case SP_NOSECTION:
      lasterr = Tr("Data section is absent");
      break;

    case SP_BADVALUE:
      lasterr = Tr("Inadmissible numerical values");
      break;

    case SP_INCONSISTENT:
      lasterr = Tr("Inconsistent data");
      break;

    default:
      lasterr = Tr("Unknown error");
      break;
    }

  lasterr += Tr(" at ");
  lasterr += Where();
  return (StreamParseErr)code;
  }  // SetLastError()


///////////////////////////////////////////////////////////////////////////////
/// Skip text in file to the specified keyword.
/// @param keyword String of the keyword.
/// @return SUCCESS - file was skipped successfully, FAILURE - otherwise
OKAY AnnInFile::SkipToKeyword(const Str &keyword)
  {
  Str str;
  while (ReadLine(str) == SUCCESS)
    {
    str.Clip();
    if (str.IFind(keyword))
      return SUCCESS;
    }
  return FAILURE;
  }

INTEGRA_NAMESPACE_END

