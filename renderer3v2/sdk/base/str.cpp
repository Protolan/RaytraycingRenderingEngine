/// @internal
/// @file
///
/// @brief Definition of Str class.
///
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01
/// @author Pnd - Nickolay Derjabin, '99.04.01
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004

#include <stdarg.h>
#include <locale.h>
#ifdef _WIN32
#include <mbstring.h>
#else
#include <wchar.h>
#endif

#include <integra.hpp>

#include "str.hpp"
#include "serializer.hpp"

#include <math/math.hpp>

INTEGRA_NAMESPACE_START

/**
@class Str base/str.hpp

Processing of strings.

This class is aimed to deal with text strings in a useful way
if efficiency is not very important. Elements of the text strings are
of type 'char' and contain text in plain English.

The class may not be used for strings participating in the
user interface - use the UStr class instead.

@sa @ref base_mainpage

@internal

Internal Str object representation is nothing else as a
(char *) pointer to a memory allocated dynamically (may be NULL).
Operations with Str objects typically reallocate the memory area.
Thus, copying of Str objects includes allocation of new memory area and
copying the original string to it.
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor from a string.
/// @param[in] pch - A source string.
Str::Str(const char *pch)
  {
  buf = NULL;
  *this = pch;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from an array of characters.
/// @param[in] pch - Array data.
/// @param[in] n - The array length.
Str::Str(const char *pch, int n)
  {
  buf = NULL;

  if (pch != NULL && n != 0)
    {
    int len = (n + 1) * sizeof(char);
    buf = (char *)malloc(len);
    if (buf != NULL)
      {
      memcpy(buf, pch, n);
      buf[n] = '\0';
      }
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// Memory is released.
Str::~Str()
  {
  if (buf != NULL)
    free(buf);
  }

//////////////////////////////////////////////////////////////////////////////
/// Allocate string with the given length of string.
/// @param[in] n - The string length.
/// @note String is filled by space chars.
/// @return SUCCESS of FAILURE (not enough memory).
OKAY Str::Allocate(int n)
  {
  if (buf != NULL)
    free(buf);
  buf = NULL;

  if (n != 0)
    {
    int len = (n + 1) * sizeof(char);
    buf = (char *)malloc(len);
    if (buf == NULL)
      return FAILURE;
    memset(buf, ' ', n);
    buf[n] = '\0';
    }
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] s - A source object.
Str::Str(const Str &s)
  {
  buf = NULL;
  *this = s.buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] s - A source string.
/// @return A reference to the assigned string.
void Str::operator = (const char *s)
  {
  // Fool-proof
  if (s == buf)
    return;

  if (s == NULL)
    {
    if (buf != NULL)
      free(buf);
    buf = NULL;
    }
  else
    {
    SIZE_T len = (strlen(s) + 1) * sizeof(char);
    buf = (char *)realloc(buf, len);
    if (buf != NULL)
      memcpy(buf, s, len);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
///
/// This method initializes this string by a text specified by pointer and size arguments.
///
/// @param[in] txt Pointer to the source text.
/// @param[in] n Number of characters to copy.
///
void Str::Set(const char *txt, int n)
  {
  Assert(txt != NULL);
  Assert(txt != buf);
  Assert(n >= 0);
  buf = (char *)realloc(buf, n + 1);
  if (buf != NULL)
    {
    memcpy(buf, txt, n);
    buf[n] = '\0';
    }
  }  // Set()

//////////////////////////////////////////////////////////////////////////////
/// Concatenation and assignment operator.
/// @param[in] sour - A source string.
/// @return A reference to the assigned string.
void Str::operator +=(const char *sour)
  {
  if (sour == NULL)
    return;
  if (buf == NULL)
    {
    *this = sour;
    return;
    }

  SIZE_T len1 = strlen(buf);
  SIZE_T len2 = strlen(sour);

  char *new_buf = (char *)realloc(buf, len1 + len2 + 1);
  if (new_buf != NULL)
    {
    buf = new_buf;
    memcpy(buf + len1, sour, len2 + 1);
    }
  }  // operator +=()

//////////////////////////////////////////////////////////////////////////////
/// Remove blanks from the string.
///
/// The method removes all spaces (' ') from the string.
/// @return A reference to the modified string.
void Str::RemoveBlanks()
  {
  if (buf != NULL)
    {
    int i, j;
    for (i = j = 0; buf[i] != '\0'; ++i)
      {
      if (buf[i] != ' ') 
        buf[j++] = buf[i];
      }
    buf[j] = '\0';
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove first and end blanks from string.
///
/// Method removes all spaces (' ', tab and other - 0x09 - 0x0D or 0x20) from 
/// begin and from end of the string.
/// @return Reference to modified string.
void Str::Clip()
  {
  if (buf != NULL)
    {
    int i = Length() - 1;
    while (i >= 0 && isspace(buf[i]))
      {
      buf[i--] = '\0';
      }

    if (i > 0)
      {
      int j = 0;
      while (isspace(buf[j]) && j < i)
        j++;

      int k = 0;
      while(j <= i)
        buf[k++] = buf[j++];

      buf[k] = '\0';
      }
    }  // if (buf != NULL)
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove trailing zeros in fractional part of real number.
///
/// Method assumes this string is a real number (it is not checked though).
/// Number in exponential format is not processed; from number in fixed format
/// trailing zeros in fractional part (after dot) are removed. If dot becomes
/// the last character, it is removed too.
void Str::RemoveTrailZeros()
  {
  int l = Length() - 1;
  if (l < 1)
    return;
  if (FindFirst('e') || FindFirst('E'))
    return;
  if (!FindFirst('.') && !FindFirst(*localeconv()->decimal_point))
    return;

  while (buf[l] == '0' && l > 1)
    buf[l--] = '\0';
  if ((buf[l] == '.' || buf[l] == *localeconv()->decimal_point) && l > 0)
    buf[l] = '\0';
  }

//////////////////////////////////////////////////////////////////////////////
/// Concatenation operator.
/// @param[in] s - A string to be added to this string.
/// @return The result of the concatenation.
Str Str::operator +(const Str &s) const
  {
  Str s1(*this);
  s1 += s;
  return s1;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a substring.
///
/// The method returns the substring (a new Str object) containing the
/// specified number of characters from this string beginning from the
/// specified position.
/// @param[in] pos - A position of the beginning of the substring;
/// must be >= 0, debug version asserts it.
/// @param[in] nchars - A length of the substring;
/// must be >= 0, debug version asserts it.
/// @note If @a pos + @a nchars >= the length of the string, then
/// the rest will be padded by blanks.
/// @return The substring (a new string).
Str Str::SubStr(int pos, int nchars)  const
  {
  Assert(pos >= 0 && nchars >= 0);

  Str new_str;   // new string;
  new_str.buf = (char *)malloc(nchars + 1);
  if (new_str.buf != NULL)
    {
    int len = Length();
    memset(new_str.buf, ' ', nchars);
    if (buf != NULL && pos < len)
      {
      int sub_len = len - pos;
      if (sub_len > nchars)
        sub_len = nchars;
      memcpy(new_str.buf, buf + pos, sub_len);
      }
    new_str.buf[nchars] = '\0';
    }   // if (new_str.buf != NULL)

  return new_str;
  }  // SubStr()

//////////////////////////////////////////////////////////////////////////////
/// Compare a substring.
/// @param[in] str - A string to compare with.
/// @param[in] start A start position in this string to begin the comparison
/// from.
/// @return
/// - value < 0 if this string < than @a str;
/// - value = 0 if this string is identical to @a str;
/// - value > 0 if this string > than @a str.
int Str::SubStrCmp(const char *str, int start) const
  {
  if (buf == NULL)
    {
    if (str == NULL)
      return 0;
    return -1;
    }
  if (str == NULL)
    return 1;

  return strncmp(buf + start, str, strlen(str));
  }

//////////////////////////////////////////////////////////////////////////////
/// Case sensitive comparison of n chars.
/// @param[in] s - A string to compare with.
/// @param[in] n - Number of chars for comparison.
/// @return
/// - value < 0 if this string < than s;
/// - value = 0 if this string is identical to s;
/// - value > 0 if this string > than s.
int Str::NCmp(const char *s, int n) const
  {
  if (buf == NULL)
    {
    if (s == NULL)
      return 0;
    return -1;
    }
  if (s == NULL)
    return 1;

  return strncmp(buf, s, n);
  }

//////////////////////////////////////////////////////////////////////////////
/// Case insensitive comparison of n chars.
/// @param[in] s - A string to compare with.
/// @param[in] n - Number of chars for comparison.
/// @return
/// - value < 0 if this string < than s;
/// - value = 0 if this string is identical to s;
/// - value > 0 if this string > than s.
int Str::NICmp(const char *s, int n) const
  {
  if (buf == NULL)
    {
    if (s == NULL)
      return 0;
    return -1;
    }
  if (s == NULL)
    return 1;

  return strnicmp(buf, s, n);
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the first occurrence of a character.
///
/// The method finds the first occurrence of the specified character in the
/// string.
/// @param[in] c - A character to search.
/// @param[out] out_pos - A position of the found character;
/// pass NULL if not necessary.
/// @return @b true, if the character was found, @b false otherwise.
/// If @b true, and @a out_pos is not NULL, @a *out_pos is set to the
/// (zero-based) position of the found character in the string.
bool Str::FindFirst(char c, int *out_pos) const
  {
  return Find(c, 0, out_pos);
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the last occurrence of a character.
///
/// The method finds the last occurrence of the specified character in the
/// string.
/// @param[in] c - A character to search.
/// @param[out] out_pos - A position of the found character;
/// pass NULL if not necessary.
/// @return @b true, if the character was found, or @b false otherwise.
/// If @b true, and @a out_pos is not NULL, @a *out_pos is set to the
/// (zero-based) position of the found character in the string.
bool Str::FindLast(char c, int *out_pos)  const
  {
  if (buf == NULL)
    return false;

#ifdef _WIN32
  char *found = (char *)_mbsrchr((unsigned char *)buf, c);
  if (found == NULL)
    return false;
#else  // Linux
  mbstate_t ps;
  SIZE_T char_len;
  char *found = buf;
  char *found_last = NULL;
  while (*found != '\0')
    {
    char_len = mbrlen(found, MB_CUR_MAX, &ps);
    if (char_len == 0)
      break;
    if (char_len == 1 && *found == c)
      found_last = found;
    if (char_len < 0)
      char_len = 1;
    found += char_len;
    }
  if (*found == '\0' && c == '\0')
    found_last = found;
  if (found_last == NULL)
    return false;
  found = found_last;
#endif

  if (out_pos != NULL)
    *out_pos = int(found - buf);

  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find the first occurrence of a character from the given position.
///
/// @param[in] c - A character to search.
/// @param[in] in_pos - Starting position to search a character;
/// @param[out] out_pos - A position of the found character;
/// pass NULL if not necessary.
/// @return @b true, if the character was found, @b false otherwise.
/// If @b true, and @a out_pos is not NULL, @a *out_pos is set to the
/// (zero-based) position of the found character in the string.
bool Str::Find(char c, int in_pos, int *out_pos) const
  {
  Assert(0 <= in_pos);
  if (buf == NULL)
    return false;

#ifdef _WIN32
  char *found = (char *)_mbschr((unsigned char *)(buf + in_pos), c);
  if (found == NULL)
    return false;
#else  // Linux
  mbstate_t ps;
  SIZE_T char_len;
  char *found = buf + in_pos;
  while (*found != '\0')
    {
    char_len = mbrlen(found, MB_CUR_MAX, &ps);
    if (char_len == 0)
      break;
    if (char_len == 1 && *found == c)
      break;
    if (char_len < 0)
      char_len = 1;
    found += char_len;
    }
  if (*found == '\0' && c != '\0')
    return false;
#endif

  if (out_pos != NULL)
    *out_pos = int(found - buf);
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find a string.
///
/// The method finds the specified string inside this string starting the
/// search from the specified position.
/// @param[in] s - A string to search for.
/// @param[out] out_pos - A position of the found string in this string.
/// @param[in] start - A position to start the search from.
/// @return @b YES, if the string was found, or @b NO otherwise.
/// If @b YES, and @a out_pos is not NULL, @a *out_pos is set to the
/// (zero-based) position of the found string in this string.
bool Str::Find(const Str &s, int *out_pos, int start) const
  {
  if (buf == NULL || s.buf == NULL)
    {
    if (buf == s.buf)
      {
      if (out_pos != NULL)
        *out_pos = 0;
      return true;
      }
    return false;
    }
  Assert(0 <= start);
#ifdef _WIN32
  char *p = (char *)_mbsstr((unsigned char *)(buf + start), (unsigned char *)s.buf);
#else  // Linux
  char *p = strstr(buf + start, s.buf);
#endif
  if (p == NULL)
    return false;

  if (out_pos != NULL)
    *out_pos = (int)(p - buf);
  return true;
  }  // Find()


///////////////////////////////////////////////////////////////////////////////
/// Find a string (case insensitive).
///
/// The method finds the specified string inside this string starting the
/// search from the specified position.
/// @param[in] s - A string to search for.
/// @param[out] out_pos - A position of the found string in this string.
/// @param[in] start - A position to start the search from.
/// @return @b YES, if the string was found, or @b NO otherwise.
/// If @b YES, and @a out_pos is not NULL, @a *out_pos is set to the
/// (zero-based) position of the found string in this string.
bool Str::IFind(const Str &s, int *out_pos, int start) const
  {
  Str s1(*this);
  Str s2(s);
  s1.ToUpper();
  s2.ToUpper();
  return s1.Find(s2, out_pos, start);
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert the string to upper case.
void Str::ToUpper()
  {
  if (buf != NULL)
    for (int i = 0; buf[i] != '\0'; i++)
      buf[i] = (char)toupper(buf[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert the string to lower case.
void Str::ToLower()
  {
  if (buf != NULL)
    for (int i = 0; buf[i] != '\0'; i++)
      buf[i] = (char)tolower(buf[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove a substring.
///
/// The method removes the specified number of characters
/// starting from the specified position.
/// @param[in] pos - A position of the first character to be removed;
/// must be >= 0, debug version asserts it.
/// @param[in] n - The number of characters to be removed;
/// must be >= 0, debug version asserts it.
/// @return A reference to the resulting string.
void Str::Exclude(int pos, int n)
  {
  Assert(pos >= 0 && n >= 0);
  int len = Length();
  if (buf != NULL && pos < len)
    {
    int i;
    for (i = pos; i + n < len; i++)
      buf[i] = buf[n + i];
    buf[i] = '\0';
    char *new_buf = (char *)realloc(buf, i + 1);
    Assert(new_buf != NULL);  // should be valid always
    buf = new_buf;
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Resize the string.
///
/// The method reallocates the string to the specified size,
/// padding with blanks, if necessary.
/// @param[in] nchars _ A new size of the string;
/// must be >= 0, debug version asserts it.
/// @return SUCCESS / FAILURE.
OKAY Str::Resize(int nchars)
  {
  Assert(nchars >= 0);

  int len = Length();

  char *new_buf = (char *)realloc(buf, nchars + 1);
  if (new_buf == NULL)
    return FAILURE;
  buf = new_buf;

  if (nchars > len)
    memset(buf + len, ' ', nchars - len);

  buf[nchars] = '\0';

  return SUCCESS;
  }  // Resize()


//////////////////////////////////////////////////////////////////////////////
/// Clear the string.
///
/// The method clears the string.
void Str::Clear()
  {
  if (buf != NULL)
    free(buf);
  buf = NULL;
  }  // Clear()

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the string, if it is not NULL, or to empty string.
/// 
/// It allows to print NULL not as (null), but as empty string.
/// @return A pointer to the buffer of the string, if it is not NULL, or to empty string.
const char *Str::PData() const
  {
  static char *empty_str = "";
  if (buf == NULL)
    return empty_str;
  return buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Formatted output to the string.
///
/// Analog of the sprintf() function. For allowed formats, please refer to
/// sprintf() manual.
/// @note A used internal buffer is restricted by MAX_LENGTH bytes.
/// @param[in] str_format - A format string;
/// may not be NULL, debug version asserts it.
void Str::Printf(const char *str_format, ...)
  {
  char buf_str[MAX_LENGTH + 1];
  Assert(str_format != NULL);

  va_list arg_list;
  va_start(arg_list, str_format);
  _vsnprintf(buf_str, MAX_LENGTH, str_format, arg_list);
  va_end(arg_list);
  buf_str[MAX_LENGTH] = '\0';
  (*this) = buf_str;
  }

//////////////////////////////////////////////////////////////////////////////
/// Formatted input from the string.
///
/// Analog of the sscanf() function. For allowed formats please refer to
/// the sscanf() manual.
/// @param[in] str_format - A format string;
/// may not be NULL, debug version asserts it.
/// @return The number of fields successfully converted and assigned; 
/// the return value does not include fields that were read but not assigned. 
/// A return value of 0 indicates that no fields were assigned. 
/// The return value is EOF for an error or if the end of the string is reached before the first conversion. 
int Str::Scanf(const char *str_format, ...) const
  {
  Assert(str_format != NULL);

  int ip;
  void *args[100];
  for (ip = 0; ip < 100; ip++)
    args[ip] = NULL;
  va_list arg_list;
  va_start(arg_list, str_format);
  for (ip = 0; ip < 100; ip++)
    {
    args[ip] = va_arg(arg_list, void *);
    if (args[ip] == NULL)
      break;
    }
  va_end(arg_list);

  int res = sscanf(buf, (const char *)str_format,
    args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9],
    args[10], args[11], args[12], args[13], args[14], args[15], args[16], args[17], args[18], args[19],
    args[20], args[21], args[22], args[23], args[24], args[25], args[26], args[27], args[28], args[29],
    args[30], args[31], args[32], args[33], args[34], args[35], args[36], args[37], args[38], args[39],
    args[40], args[41], args[42], args[43], args[44], args[45], args[46], args[47], args[48], args[49],
    args[50], args[51], args[52], args[53], args[54], args[55], args[56], args[57], args[58], args[59],
    args[60], args[61], args[62], args[63], args[64], args[65], args[66], args[67], args[68], args[69],
    args[70], args[71], args[72], args[73], args[74], args[75], args[76], args[77], args[78], args[79],
    args[80], args[81], args[82], args[83], args[84], args[85], args[86], args[87], args[88], args[89],
    args[90], args[91], args[92], args[93], args[94], args[95], args[96], args[97], args[98], args[99]);

  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Append a formatted output to the string.
///
/// Analog of the sprintf() function. For allowed formats, please refer to
/// sprintf() manual.
/// The same as Printf() except that the new formatted output does not replace
/// the original contents of this string but is just appended to it.
/// @note A used internal buffer is restricted by MAX_LENGTH bytes.
/// @param[in] str_format - A format string;
/// may not be NULL, debug version asserts it.
void Str::Catf(const char *str_format, ...)
  {
  char buf_str[MAX_LENGTH + 1];
  Assert(str_format != NULL);

  va_list arg_list;
  va_start(arg_list, str_format);
  _vsnprintf(buf_str, MAX_LENGTH, str_format, arg_list);
  va_end(arg_list);
  buf_str[MAX_LENGTH] = '\0';
  (*this) += buf_str;
  }

//////////////////////////////////////////////////////////////////////////////
/// Serialize the string.
/// @note The default is NULL.
/// @param[in, out] inout - A Serializer.
/// @param[in] tag - A chunk name.
void Str::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = NULL;
  if (inout.Import() || (buf != NULL))
    inout.Value(*this);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Check the string for a real number and convert it.
///
/// The method checks the string for a real number and converts it
/// to a format acceptable to localized atof().
/// @return SUCCESS / FAILURE.
OKAY Str::CheckAndConvert2Real()
  {
  // Check that the string is not empty
  if (IsEmpty())
    return FAILURE;
  // Validate the real number follows
  int pos = 0;
  if ((buf[pos] == '-') || (buf[pos] == '+'))
    pos++;
  // Scan integral part
  if (!isdigit(buf[pos]))
    return FAILURE;
  while (isdigit(buf[pos]))
    pos++;
  // Check for radix character
  if (buf[pos] == '.')
    {
    pos++;
    // scan fractional part
    if (!isdigit(buf[pos]))
      return FAILURE;
    while (isdigit(buf[pos]))
      pos++;
    }
  // Check for exponent part
  if ((buf[pos] == 'e') || (buf[pos] == 'E'))
    {
    int pos1 = pos + 1;
    if ((buf[pos1] == '-') || (buf[pos1] == '+'))
      pos1++;
    // scan exponent
    while (isdigit(buf[pos1]))
      {
      pos1++;
      pos = pos1;
      }
    }
  if (pos < Length() -1)
    return FAILURE;

  // Take care about regional settings
  int dot_pos;
  if (FindFirst('.', &dot_pos))
    buf[dot_pos] = *localeconv()->decimal_point;
  return SUCCESS;
  }  // CheckAndConvert2Real()

//////////////////////////////////////////////////////////////////////////////
/// Check the string for a integer number and return integer value.
///
/// The method checks the string for a integer number and returns this number.
/// @param[out] ok
/// @return Integer.
int Str::IntegerNum(bool &ok) const
  {
  // Check that the string is not empty
  if (IsEmpty())
    {
    ok = false;
    return 0;
    }

  // Validate the real number follows
  int pos = 0;
  while ((buf[pos] == ' ') || (buf[pos] == '\t'))
    pos++;

  if ((buf[pos] == '-') || (buf[pos] == '+'))
    pos++;

  while (isdigit(buf[pos]))
    pos++;

  while ((buf[pos] == ' ') || (buf[pos] == '\t'))
    pos++;

  ok = pos == this->Length();
  return atoi(SubStr(0, pos).Data());
  }

//////////////////////////////////////////////////////////////////////////////
/// Check the string for a double number and return double value.
///
/// The method checks the string for a double number and returns this number.
/// @param[out] ok
/// @return Double value.
double Str::DoubleNum(bool &ok) const
  {
  Str str(*this);
  str.Clip();
  double val = 0.0;
  ok = false;
  if (str.CheckAndConvert2Real() == SUCCESS)
    {
    ok = true;
    val = atof(str);
    }
  return val;
  }

//////////////////////////////////////////////////////////////////////////////
/// Truncate the real numbers to minimize the quantity of digits after point.
/// @note This function minimizes number of digits after point keeping order of magnitude.
/// For example, for digits == 1 this function will convert 1.234 to 1.2 and 0.00000123 to 0.000001.
/// @param[in] num The number for processing.
/// @param[in] prec Maximal total number of digits after point. Numbers less than 10^-prec will be considered as 0.
/// @param[in] digits Number of significant digits after point.
INTAPI_BASE void Str::TruncateReal(double num, int prec, int digits)
  {
  int exp_ind = (int)Log10(Abs(num));
  if (exp_ind >= 0) // Absolute value is greater than 1
    Printf("%.*f", digits, num);
  else if (exp_ind >= -prec)
    Printf("%.*f", Abs(exp_ind) + digits, num);
  else // The number is too small, so it will be printed as just zero.
    Printf("0");
  RemoveTrailZeros();
  }


INTEGRA_NAMESPACE_END
