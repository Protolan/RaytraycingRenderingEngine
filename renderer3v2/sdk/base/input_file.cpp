/// @internal
/// @file
///
/// @brief Definition of InputFile class for fast input.
///
/// @author Pnd - Nick Derjabin, AKFx, '14.09.24.
///
/// Copyright &copy; INTEGRA, Inc., 2014.

#include <locale.h>
#ifdef _WIN32
#include <mbstring.h>
#endif
#include <integra.hpp>
#include "input_file.hpp"
#if 0
#include "time.hpp"
#endif
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class InputFile base/input_file.hpp

The class is used for token by token input from text files.
It is optimized for reading huge files.

Input text file is assumed to consists of ASCII characters mainly.
In addition to ASCII, MBCS sequences are allowed in limited contexts:
- inside strings (supposed to be implemented later)
- inside comments
- inside non-interpreted text returned by FReadRestOfLine()

Comment may be turned on be specifying a command mark.
Any text starting from command mark up to the end of the line is ignored.

Inputted text is considered and processed as a sequence of @b tokens.
The tokens are: identifiers, numbers, strings, delimiters, etc., and EOL (end of line).
Generally, tokens are separated by spaces.
There is a notion of @b cursor pointing to the current position inside the file.

Line on the file is said as @b empty if it contains EOL token only.
Empty lines are ignored during file procession.
So someone can consider that each line of the file contains at least two tokens,
the latter token being EOF.

For each kind of token (including EOL) there is a FreadXXX() method that gets that token.
If the token has been parsed successfully, the cursor is moved to postion after the token,
and @c true is returned.
Otherwise, cursor is moved to the start of the next token (skiping separators only if any),
and @c false is returned.

Each end of line must be parsed explicitly using FReadEOL() or other special methods.
When EOL is parsed, cursor is moved to the beginning of the next non-EOL token in the file.
FIsEOL() method may be used to check reaching the end of line without parsing it.

The end of file is reached when the cursor points to CTRL_Z character.  Nothing can be read from the file then.

**/

/// Locale used for formatting.
static _locale_t s_locale;

/// Character table.
static char s_char_table[256];
/// Flag of character table initialization.
static bool s_chars_inited = false;

/// Space character flag.
static const int S_SPACE = 0x1;
/// Quote mark flag.
static const int S_QUOTE = 0x2;
/// Delimiter character flag.
static const int S_DELIM = 0x4;
/// Letter character flag.
static const int S_LETTER = 0x8;
/// Digit character flag.
static const int S_DIGIT = 0x10;
/// Graphical character flag (other characters allowed in string values).
static const int S_GRAPH = 0x20;

/// "Word" characters mask.
static const int S_WORD_MASK = (S_LETTER | S_DIGIT | S_GRAPH);

/// Ctrl-Z character.
static const int CTRL_Z = 26;
/// Size of line buffer.
static const int LINE_BUFFER_SIZE = 4096;
/// Size of file buffer.
static const int FILE_BUFFER_SIZE = 16 * 4096;

/// Initialize character table.
static void InitCharacterTable();

//////////////////////////////////////////////////////////////////////////////
/// Initialize character table.
///
void InitCharacterTable()
  {
  // Execute once
  if (s_chars_inited)
    return;
  s_chars_inited = true;
  // Create locale at once
  s_locale = _create_locale(LC_NUMERIC, "C");
  char *chars;
  int len, i;
  // ------ Spaces
  chars = " \t\r\v\f";
  len = (int)strlen(chars);
  for (i = 0; i < len; i++)
    s_char_table[chars[i]] = S_SPACE;
  // ------ Qutes
  s_char_table['"'] = S_QUOTE;
  s_char_table['\''] = S_QUOTE;
  // ------ Delimiters
  chars = ",;:=<>()[]{|}";
  len = (int)strlen(chars);
  for (i = 0; i < len; i++)
    s_char_table[chars[i]] = S_DELIM;
  // ------ Letters
  chars = "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLNMOPQRSTUVWXYZ";
  len = (int)strlen(chars);
  for (i = 0; i < len; i++)
    s_char_table[chars[i]] = S_LETTER;
  // ------ Digits
  chars = "0123456789";
  len = (int)strlen(chars);
  for (i = 0; i < len; i++)
    s_char_table[chars[i]] = S_DIGIT;
  // ------ "Graphical" characters
  chars = ".!$+-/%@*&~";
  len = (int)strlen(chars);
  for (i = 0; i < len; i++)
    s_char_table[chars[i]] = S_GRAPH;
  // ------ Remining chars: #'?\^`
  }  // InitCharacterTable()

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// @param[in] pathname Pathname of file to read.
/// @param[in] comment_mark The comment mark for the input file.
///   Any text from this mark up to end of the line is ignored.
///   By default, the mark is absent (NULL).
///
/// @attention
/// Comment_mark (if not NULL) must be a _static_ string like ";;"
/// as the class (for efficiency reasons) remembers pointer to it!
/// 
InputFile::InputFile(const CHAR *pathname, const char *comment_mark)
  {
  // Empty comment mark is meaningless
  Assert((comment_mark == NULL) || (*comment_mark != '\0'));

  InitCharacterTable();

  m_pathname = pathname;
  m_comment = NULL;
  m_com_mark_len = 0;
  if (comment_mark != NULL)
    {
    m_comment = comment_mark;
    m_com_mark_len = (int)strlen(m_comment);
    }

  m_fd = NULL;
  m_read_area = NULL;
  m_cursor = NULL;
  m_eof = false;
  }  // Constructor

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor (forbidden).
///
/// @param[in] src Source file.
///
InputFile::InputFile(const InputFile &src)
  {
  Assert(false);
  }  // Copy constructor

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// @note The file will be closed if it was open.
///
InputFile::~InputFile()
  {
  if (m_fd != NULL)
    FClose();
  }  //  Destructor

//////////////////////////////////////////////////////////////////////////////
/// Open the file.
///
/// The method opens the file for reading in the text mode.
/// Cursor is set to the beginning of first non-EOL token in the file.
///
/// @return SUCCESS / FAILURE.
///
/// @note File must not be opened.
///
OKAY InputFile::FOpen()
  {
  Assert(m_fd == NULL);
  if (m_fd != NULL)
    return FAILURE;
  m_read_area = new CHAR[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 3];
  if (m_read_area == NULL)
    return User()->ErrorNoMemory();
  m_fd = fopen(m_pathname.Data(), "rb");
  if (m_fd == NULL)
    return User()->ErrorMessage(Tr("Can't open file '%s' for reading"), m_pathname.Data());
  // Trap for too long lines
  m_read_area[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE] = '\n';
  m_read_area[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 1] = CTRL_Z;
  m_read_area[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2] = 'X';
  // Initialize cursor
  m_cursor = m_read_area + LINE_BUFFER_SIZE - 1;
  *m_cursor = '\n';
  m_line_no = 0;
  m_eof = false;
  // Read first file portion
  ReadPortion();
  // Move to the first non-EOL token, if any
  MoveToNextToken();
  return SUCCESS;
  }  // FOpen()

//////////////////////////////////////////////////////////////////////////////
/// Close the file.
///
/// @return SUCCESS or FAILURE.
///
/// SUCCESS is returned if there were no problems when working with the file and the file was closed successfully.
/// FAILURE is returned on any I/O error while working with the file or a closing error.
///
/// @note File must be opened.
///
OKAY InputFile::FClose()
  {
  Assert(m_fd != NULL);
  if (m_fd == NULL)
    return FAILURE;
  int err1 = ferror(m_fd);
  int err2 = fclose(m_fd);
  m_fd = NULL;
  if (m_read_area[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2] != 'X')
    {
    User()->WarningMessage(Tr("Too long lines detected reading file '%s'.\nProsession errors are possible"),
      m_pathname.Data());
    }
  delete m_read_area;
  m_read_area = NULL;
  m_cursor = NULL;
  m_line_no = 0;
  m_eof = false;
  if ((err1 != 0) || (err2 != 0))
    return User()->ErrorMessage(Tr("Errors occured reading file %s"), m_pathname.Data());
  return SUCCESS;
  }  // FClose()

//////////////////////////////////////////////////////////////////////////////
/// Read given keyword (case sensitive).
///
/// @param[in] keyword Expected keyword.
/// @return @c true, if the keyword has been parsed, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If the token under cursor matches given keyword,
/// cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
/// @note In general, keyword may consists of any "word" characters.
///
bool InputFile::FReadKeyWord(const char *keyword)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  while ((s_char_table[*(unsigned char *)end_ptr] & S_WORD_MASK) != NULL)
    end_ptr++;
  char next_char = *end_ptr;
  // Do comparison
  *end_ptr = '\0';
  bool equal = (strcmp(m_cursor, keyword) == 0);
  *end_ptr = next_char;
  if (!equal)
    return false;
  // Advance cursor
  m_cursor = end_ptr;
  return true;
  }  // FReadKeyWord()

//////////////////////////////////////////////////////////////////////////////
/// Read given keyword (case insensitive).
///
/// @param[in] keyword Expected keyword.
/// @return @c true, if the keyword has been parsed, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If the token under cursor matches given keyword,
/// then cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
/// @note In general, keyword may consists of any "word" characters.
///
bool InputFile::FReadKEYWORD(const char *keyword)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  while ((s_char_table[*(unsigned char *)end_ptr] & S_WORD_MASK) != NULL)
    end_ptr++;
  char next_char = *end_ptr;
  // Do comparison
  *end_ptr = '\0';
  bool equal = (_stricmp(m_cursor, keyword) == 0);
  *end_ptr = next_char;
  if (!equal)
    return false;
  // Advance cursor
  m_cursor = end_ptr;
  return true;
  }  // FReadKEYWORD()

//////////////////////////////////////////////////////////////////////////////
/// Read a logical value.
///
/// @param[out] value Value read.
/// @return @c true, if the value has been parsed, @c false otherwise.
///
/// Logical values in file are represented by designated words:
/// - the words 1-true-yes-on-enabled are read in as "true".
/// - the words 0-false-no-off-disabled are read in as "false".
///
/// Cursor is moved to skip spaces if any.
/// If the token under cursor matches one of the designated words,
/// then resulting value is set, cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
bool InputFile::FReadBool(bool &value)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  while ((s_char_table[*(unsigned char *)end_ptr] & S_WORD_MASK) != NULL)
    end_ptr++;
  char next_char = *end_ptr;
  // Do comparison
  *end_ptr = '\0';
  int res = 2;  // Indicates no match
  if  (strcmp(m_cursor, "0") == 0)
    res = 0;
  else if (strcmp(m_cursor, "1") == 0)
    res = 1;
  else if (_stricmp(m_cursor, "false") == 0)
    res = 0;
  else if (_stricmp(m_cursor, "true") == 0)
    res = 1;
  else if (_stricmp(m_cursor, "no") == 0)
    res = 0;
  else if (_stricmp(m_cursor, "yes") == 0)
    res = 1;
  else if (_stricmp(m_cursor, "off") == 0)
    res = 0;
  else if (_stricmp(m_cursor, "on") == 0)
    res = 1;
  else if (_stricmp(m_cursor, "disabled") == 0)
    res = 0;
  else if (_stricmp(m_cursor, "enabled") == 0)
    res = 1;
  *end_ptr = next_char;
  // Whether the value has been parsed
  if (res > 1)
    return false;
  // Assign value
  value = (res != 0);
  // Advance cursor
  m_cursor = (CHAR *)end_ptr;
  return true;
  }  // FReadBool()

//////////////////////////////////////////////////////////////////////////////
/// Read an integer value.
///
/// @param[out] value Value read.
/// @return @c true, if the value has been parsed, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If the token under cursor is recognized as an integral number,
/// then resulting value is set, cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
bool InputFile::FReadInt(int &value)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  if ((*end_ptr == '-') || (*end_ptr == '+'))
    end_ptr++;
  if (s_char_table[*(unsigned char *)end_ptr] != S_DIGIT)
    return false;
  end_ptr++;
  while (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
    end_ptr++;
  // Parse the value
  char next_char = *end_ptr;
  *end_ptr = '\0';
  value = _atoi_l(m_cursor, s_locale);
  *end_ptr = next_char;
  // Advance cursor
  m_cursor = end_ptr;
  return true;
  }  // FReadInt()

//////////////////////////////////////////////////////////////////////////////
/// Read a floating point value.
///
/// @param[out] value Value read.
/// @return @c true, if the value has been parsed, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If the token under cursor is recognized as a floating point number,
/// then resulting value is set, cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
bool InputFile::FReadDouble(double &value)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  bool valid = false;
  if ((*end_ptr == '-') || (*end_ptr == '+'))
    end_ptr++;
  // Scan integral part
  if (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
    valid = true, end_ptr++;
  while (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
    end_ptr++;
  // Check for radix character
  if (*end_ptr == '.')
    {
    *end_ptr++;
    if (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
      valid = true, end_ptr++;
    while (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
      end_ptr++;
    }
  if (!valid)
    return false;
  // Check for exponent part
  if ((*end_ptr == 'e') || (*end_ptr == 'E'))
    {
    end_ptr++;
    if ((*end_ptr == '-') || (*end_ptr == '+'))
      end_ptr++;
    // scan exponent
    if (s_char_table[*(unsigned char *)end_ptr] != S_DIGIT)
      return false;
    end_ptr++;
    while (s_char_table[*(unsigned char *)end_ptr] == S_DIGIT)
      end_ptr++;
    }
  char next_char = *end_ptr;
  // Parse the value
  *end_ptr = '\0';
  _CRT_DOUBLE dblval;
  _atodbl_l(&dblval, m_cursor, s_locale);
  value = dblval.x;
  *end_ptr = next_char;
  // Whether the value has been parsed
  // Advance cursor
  m_cursor = (CHAR *)end_ptr;
  return true;
  }  // FReadDouble()

//////////////////////////////////////////////////////////////////////////////
/// Read a textual value.
///
/// @param[out] value Resulting string.
/// @return @c true, if the value was read, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If any "word" characters follow, then the whole token consiting of them
/// is read to resulting value, cursor is moved beyond the token, and @c true is returned.
/// Otherwise @c false is returned and cursor remains at the start of the token.
///
/// If @c true id returned, resulting string is guaranteed to be not empty.
///
bool InputFile::FReadText(Str &value)
  {
  // Return false on EOF and errors
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Search for end of the token
  char *end_ptr = m_cursor;
  while ((s_char_table[*(unsigned char *)end_ptr] & S_WORD_MASK) != NULL)
    end_ptr++;
  // Check for presence
  if (m_cursor == end_ptr)
    return false;
  // Get the value
  value.Set(m_cursor, (int)(end_ptr - m_cursor));
  // Advance cursor
  m_cursor = (CHAR *)end_ptr;
  return true;
  }  // FReadText()

//////////////////////////////////////////////////////////////////////////////
/// Read the rest of the line without parsing tokens in it.
///
/// Usage of this method is not recommended.  It is a legacy technology.
///
/// @param[out] str resulting string.
/// @return @c true, if the string was read, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// The rest of the line starting from the cursor,
/// except for trailing spaces and comment, is read to resulting string.
/// Cursor is moved to the start of the next line.
///
/// The method returns @c true, if the rest of line (may be empty string) has been read,
/// @c false being returned on end of file or file error.
///
bool InputFile::FReadRestOfLine(UStr &str)
  {
  // Check for line presence
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip leading spaces
  SkipSpacesInLine();
  // Find end of line
  char *next_nl = (char *)memchr(m_cursor, '\n', m_read_area + LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2 - m_cursor);
  if (next_nl == NULL)
    User()->InternalError("InputFile::FReadRestOfLine() - no NL");
  // Eclude trailing spaces
  char *end_ptr = next_nl;
  while ((end_ptr > m_cursor) && (s_char_table[*(unsigned char *)(end_ptr - 1)] == S_SPACE))
    end_ptr--;
  char end_char = *end_ptr;
  *end_ptr = '\0';
  // Excude comment if any.
  if (m_comment != NULL)
    {
#ifdef _WIN32
    char *p_comment = (char *)_mbsstr((unsigned char *)m_cursor, (unsigned char *)m_comment);
#else  // Linux
    char *p_comment = strstr(m_cursor, m_comment);
#endif
    if (p_comment != NULL)
      *p_comment = '\0';
    }
  // Take the string
  str = m_cursor;
  *end_ptr = end_char;
  // Move to the next non-empty line
  m_cursor = next_nl;
  MoveToNextToken();
  return true;
  }  // FReadRestOfLine()

//////////////////////////////////////////////////////////////////////////////
/// Check if end of line has been reached.
///
/// @return @c true, if end of line has been reached, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If cursor points to EOL, then @c true is returned, otherwise @c false is returned.
///
bool InputFile::FIsEOL()
  {
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip spaces
  SkipSpacesInLine();
  return *m_cursor == '\n';
  }  // FIsEOL()

//////////////////////////////////////////////////////////////////////////////
/// Read end of line.
///
/// @return @c true, if end of file was reached and read, @c false otherwise.
///
/// Cursor is moved to skip spaces if any.
/// If cursor points to EOL, then @c true is returned,
/// cursor being moved to the first token of next non-empty line.
/// Otherwise @c false is returned without further moving of cursor.
///
bool InputFile::FReadEOL()
  {
  // Check for line presence
  if (*m_cursor == CTRL_Z)
    return false;
  // Skip spaces
  SkipSpacesInLine();
  if (*m_cursor != '\n')
    return false;
  // Move to the next non-empty line
  MoveToNextToken();
  return true;
  }  // FReadEOL();

//////////////////////////////////////////////////////////////////////////////
/// Skip current line.
///
/// The rest of the current line is ignored.
/// Cursor is moved to the first token of next non-empty line.
/// 
void InputFile::FSkipLine()
  {
  if (*m_cursor == CTRL_Z)
    return;
  // Find end of line
  m_cursor = (char *)memchr(m_cursor, '\n', m_read_area + LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2 - m_cursor);
  if (m_cursor == NULL)
    User()->InternalError("InputFile::FSkipLine() - no NL");
  // Move to the next non-empty line
  MoveToNextToken();
  }  // FSkipLine()

//////////////////////////////////////////////////////////////////////////////
/// Test for end of file.
///
/// @return True if end of file has been reached (or file error occurred).
///
bool InputFile::FIsEndOfFile() const 
  {
  return *m_cursor == CTRL_Z;
  }  // FIsEndOfFile()

//////////////////////////////////////////////////////////////////////////////
/// Get the current line number.
///
/// @return Number of the last line read.
///
int InputFile::LineNo() const
  {
  return m_line_no;
  }  // LineNo()

//////////////////////////////////////////////////////////////////////////////
/// Get a text specifying current position in the file.
///
/// @return A string describing where we are in the file, in the form "line 23 pos 55 of file f:\myfile.dat".
///
Str InputFile::Where() const
  {
  char *prev_nl = m_cursor - 1;
  while ((*prev_nl != '\n') && (prev_nl >= m_read_area))
    prev_nl--;
  int pos = (int)(m_cursor - prev_nl);
  Str str;
  str.Printf("line %i pos %i in file \"%s\"", m_line_no, pos, m_pathname.Data());
  return str;
  }  // Where()

//////////////////////////////////////////////////////////////////////////////
/// Get pathname to the file.
///
/// @return The pathname to the file.
///
const PathStr &InputFile::PathName() const
  {
  return m_pathname;
  }  // FPathName()

#if 0
//////////////////////////////////////////////////////////////////////////////
/// Get file time.
///
/// @return File modification time.
///
unsigned InputFile::FileTime() const
  {
  return m_pathname.FileTime();
  }  // FileTime()
#endif

//////////////////////////////////////////////////////////////////////////////
/// Virtual fread() implementation.
///
/// Conrary to fread(), re-implementations of this method may return -1 indicating file format error.
///
/// @param[in] dest_area Area to read data to.
/// @param[in] size Number of bytes to read.
/// @return Number of bytes actially read (>= 0).
///
int InputFile::SysFRead(CHAR *dest_area, int size)
  {
  return (int)fread(dest_area, 1, (SIZE_T)size, m_fd);
  }  // SysFRead()

//////////////////////////////////////////////////////////////////////////////
/// Read next portion of the file to the buffer.
///
void InputFile::ReadPortion()
  {
  Assert(!m_eof);
  // Read next file portion
  int portion = SysFRead(m_read_area + LINE_BUFFER_SIZE, FILE_BUFFER_SIZE);
  if (portion == FILE_BUFFER_SIZE)
    return;
  // Stop on file format error
  if (portion < 0)
    portion = 0;
  // Last portion of the file has been read 
  m_eof = true;
  // Ensure that the last line is ended by the newline character
  if (m_read_area[LINE_BUFFER_SIZE + portion - 1] != '\n')
    m_read_area[LINE_BUFFER_SIZE + portion++] = '\n';
  // Add closing _Z character
  m_read_area[LINE_BUFFER_SIZE + portion] = CTRL_Z;
  }  // ReadPortion()

//////////////////////////////////////////////////////////////////////////////
/// Move to the next line.
///
/// This method parses the new line character.
///
/// The method guarantees that the whole next line is in the buffer.
/// If necessary, end of the buffer is copied to the beginning,
/// and next file portion is read.
///
void InputFile::MoveToNextLine()
  {
  Assert(*m_cursor == '\n');
  m_cursor++;
  m_line_no++;
  if ((m_cursor < m_read_area + FILE_BUFFER_SIZE) || m_eof)
    return;
  // Trap for too long lines
  if (m_cursor > m_read_area + LINE_BUFFER_SIZE + FILE_BUFFER_SIZE)
    m_read_area[LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2] = 'Y';
  // Copy end of the buffer to the beginning
  memcpy(m_read_area, m_read_area + FILE_BUFFER_SIZE, LINE_BUFFER_SIZE);
  m_cursor -= FILE_BUFFER_SIZE;
  // Read next portion of the file
  ReadPortion();
  }  // MoveToNextLine()

//////////////////////////////////////////////////////////////////////////////
/// Move to the next non-EOL token possibly skipping empty lines
///
/// Cursor is moved forward until next non-empty line is found.
/// Then cursor is set to the first token (not EOL!) of that line.
///
void InputFile::MoveToNextToken()
  {
  Assert(*m_cursor == '\n');
  for ( ; ; )
    {
    // Move to the next line
    MoveToNextLine();
    // Check for EOF or error
    if (*m_cursor == CTRL_Z)
      return;
    // Skip spaces
    SkipSpacesInLine();
    // Stop at non-empty line
    if (*m_cursor != '\n')
      return;
    }
  }  // MoveToNextToken()

//////////////////////////////////////////////////////////////////////////////
/// Skip spaces within the current line.
///
void InputFile::SkipSpacesInLine()
  {
  Assert(*m_cursor != CTRL_Z);
  // Skip spaces
  while (s_char_table[*(unsigned char *)m_cursor] == S_SPACE)
    m_cursor++;
  // Skip comment if any
  if ((m_comment != NULL) && strncmp(m_cursor, m_comment, m_com_mark_len) == 0)
    {
    // Find end of line
    m_cursor = (char *)memchr(m_cursor, '\n', m_read_area + LINE_BUFFER_SIZE + FILE_BUFFER_SIZE + 2 - m_cursor);
    if (m_cursor == NULL)
      User()->InternalError("InputFile::SkipSpacesInLine() - no NL");
    Assert(*m_cursor == '\n');
    }
  }  // SkipSpacesInLine()

INTEGRA_NAMESPACE_END

