/// @internal
/// @file
///
/// @brief Definition of UStr class.
///
/// @author Pnd - Nickolay Derjabin, '00.12.29
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004

#ifdef _WIN32
#include <windows.h>
#endif

#include <integra.hpp>

#include "arrays.hpp"
#include "str.hpp"
#include "serializer.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/**
@class UStr base/str.hpp

This class is aimed to deal with strings of CHARs.

This class must be used for all the strings participating
in User Interface.  Currently the class is essentially identical
(inherited from) the Str class based on the char type but the underlying type
may be changed in future to, say, wchar_t (16 bit UNICODE).
Therefore the user must make no assumptions about particular
CHAR representation, including memory size occupied by it.

To deal with the UStr class, the user must always convert string and
character literals to the (CHAR *) and CHAR types, respectively.
The INTEGRA_T() macro must be used for this.

@code
INTEGRA_T('A')
INTEGRA_T("Hello, world!\n")
@endcode

@sa @ref base_mainpage

@internal
Internal UStr object representation is nothing else as an Str object.
**/

/// Pointer to translation function.
UStr (*UStr::tr_func)(const char *context, const char *str, const char *comment) = NULL;

//////////////////////////////////////////////////////////////////////////////
/// Serialize the string.
/// @note Default is NULL.
/// @param[in, out] inout Serializer.
/// @param[in] tag Chunk name.
void UStr::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = NULL;
  if (inout.Import() || !IsNull())
    inout.Value(*this);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert a multibyte string to/from a Unicode UTF-8 string.
///
/// The method converts a multibyte string (in the given codepage) to
/// an UTF-8 string (Unicode) and vice versa.
/// @param[in] input - A source string for conversion.
/// @param[in] codepage - A codepage.
/// @param[in] to_utf8 - A direction of the conversion.
/// If @b true, the string will be converted @b to UTF-8;
/// if @b false, the string will be converted @b from UTF-8.
/// @return SUCCESS if the conversion was successful; FAILURE otherwise.
/// FAILURE can be caused either by incorrect characters in the input string
/// (characters not allowed for the specified codepage), or by system
/// related problems (if the conversion is not possible by some reason).
OKAY UStr::Encode(const Str &input, CodePage codepage, bool to_utf8)
  {
#ifdef _WIN32
  int size, rc;

  // Do we have something to process?
  if (input.IsEmpty())
    {
    *this = "";
    return SUCCESS;
    }

  // Ask size of new unicode string (in widechars)
  size = MultiByteToWideChar(to_utf8 ? codepage : CP_UTF8,
                             0, input.Data(), -1, NULL, 0);
  if (size == 0)
    {
    Assert(false);
    return FAILURE;
    }

  // Buffer for unicode string
  LPWSTR unicode_str = new WCHAR[size];

  // Convert multibyte string to unicode string
  rc = MultiByteToWideChar(to_utf8 ? codepage : CP_UTF8,
                           0, input.Data(), -1, unicode_str, size);
  if (rc == 0)
    {
    Assert(false);
    delete[] unicode_str;
    return FAILURE;
    }

  // Ask size of new UTF8 multibyte string

  size = WideCharToMultiByte(to_utf8 ? CP_UTF8 : codepage,
                             0, unicode_str, -1, 0, 0, NULL, NULL);
  if (size == 0)
    {
    Assert(false);
    delete[] unicode_str;
    return FAILURE;
    }

  // Buffer for UTF-8 multibyte string
  char *utf8_str = new char[size];

  // Convert unicode string to UTF-8 string
  rc = WideCharToMultiByte(to_utf8 ? CP_UTF8 : codepage,
                           0, unicode_str, -1, utf8_str, size, NULL, NULL);
  if (rc == 0)
    {
    Assert(false);
    delete[] utf8_str;
    delete[] unicode_str;
    return FAILURE;
    }

  *this = utf8_str;

  delete[] utf8_str;
  delete[] unicode_str;
  return SUCCESS;
#else  // Linux
  Assert(false);  // Not implemented
  return FAILURE;
#endif
  }  // UStr::Encode()

//////////////////////////////////////////////////////////////////////////////
/// Convert a multibyte string to a Unicode UTF-16 string.
///
/// Method converts this string from multibyte to Unicode. String length
/// is determined automatically, till the '0' char (which is included
/// into resulting Unicode string).
/// @param[in] input Source MBCS string.
/// @param[out] unicode Resulting Unicode string.
/// @param[in] codepage Required codepage; by default DEF_CP (CP_ACP) is used.
/// @return SUCCESS if the conversion was successful; FAILURE otherwise.
OKAY UStr::ToUnicode(const CHAR *input, TArray<unsigned short> &unicode, CodePage codepage)
  {
#ifdef _WIN32
  if (input == NULL)
    {
    unicode.Truncate();
    return SUCCESS;
    }
  // Ask size of new unicode string (in widechars)
  int size = MultiByteToWideChar(codepage, 0, input, -1, NULL, 0);
  if (size == 0)
    {
    Assert(false);
    return FAILURE;
    }
  // Buffer for unicode string
  unicode.Allocate(size);
  // Convert multibyte string to unicode string
  if (MultiByteToWideChar(codepage, 0, input, -1, (LPWSTR)unicode.Data(), size) == 0)
    {
    Assert(false);
    unicode.Truncate();
    return FAILURE;
    }
  return SUCCESS;
#else  // Linux
  Assert(false);  // Not implemented
  return FAILURE;
#endif
  }  // UStr::ToUnicode()

//////////////////////////////////////////////////////////////////////////////
/// Convert a Unicode UTF-16 string to a multibyte string.
///
/// Method converts given Unicode string to multibyte and puts it
/// to this string. String length is determined automatically, till
/// the '0' char (which must be presented in Unicode string).
///
/// @param[in] unicode Null-terminated source Unicode string (not NULL).
/// @param[in] codepage Required codepage; by default DEF_CP (CP_ACP) is used.
/// @return SUCCESS if the conversion was successful; FAILURE otherwise.
/// @note On successful execution the string gets non-NULL value.
///
OKAY UStr::FromUnicode(const unsigned short *unicode, CodePage codepage)
  {
#ifdef _WIN32
  Assert(unicode != NULL);
  if ((unicode == NULL) || (unicode[0] == 0))
    {
    *this = "";
    return SUCCESS;
    }
  // Ask size of new multibyte string
  int size = WideCharToMultiByte(codepage, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL);
  if (size == 0)
    {
    Assert(false);
    *this = NULL;
    return FAILURE;
    }
  // Alloc multibyte string
  Resize(size);
  // Convert unicode string to multibyte string
  if (WideCharToMultiByte(codepage, 0, (LPCWSTR)unicode, -1, XData(), size, NULL, NULL) == 0)
    {
    Assert(false);
    *this = NULL;
    return FAILURE;
    }
  return SUCCESS;
#else  // Linux
  Assert(false);  // Not implemented
  return FAILURE;
#endif
  }  // UStr::FromUnicode()

/////////////////////////////////////////////////////////////////////////
/// Get unique name (prefix) for List of names.
///
/// @param[in] list List of names.
/// @param[in, out] uniq_name Unique name.
/// @return FAILURE in case of memory allocation error and SUCCESS otherwise.
/// @note uniq_name while method calling is considered as unique name prefix.
OKAY UStr::GetUniqueName(TArray<const UStr *> &list, UStr &uniq_name)
  {
  int list_len = list.Length();
  if (list_len == 0)
    return SUCCESS;

  int ipos = uniq_name.Length();
  int in;

  // Checking for uniqueness of uniq_name
  if (ipos != 0)
    {
    for (in = 0; in < list_len; in++)
      {
      UStr &name = (UStr &)(*list[in]);
      if (name.SubStrCmp((const char *)uniq_name.Data()) == 0)
        break;
      }
    if (in == list_len)
      return SUCCESS;
    }

  UStr str_ind("0123456789");
  TArray<bool> fail_mas;
  if (fail_mas.Allocate(list_len) != SUCCESS)
    return FAILURE;

  // Remove last digit from the name.
  ipos = uniq_name.Length();
  if (ipos != 0 && isdigit(uniq_name[ipos - 1]))
    {
    ipos--;
    uniq_name.Resize(ipos);
    }
  
  for (in = 0; in < list_len; in++)
    {
    if (list[in]->Length() <= ipos)
      fail_mas[in] = false;
    else
      fail_mas[in] = true;
    }

  int tmp[256];
  bool condition = true;
  while (condition)
    {
    memset(tmp, 0, sizeof(tmp));
    for (in = 0; in < list_len; in++)
      {
      if (!fail_mas[in])
        continue;
      const UStr &name = *list[in];
      tmp[name[ipos]]++;
      }

    int min_num = list_len + 1;
    int min_ind = 0;
    for (int ic = 0; ic < 10; ic++)
      {
      int ii = str_ind[ic];
      ii = tmp[ii];
      if (ii >= min_num)
        continue;
      min_ind = ic;
      min_num = ii;
      if (min_num == 0)
        break;
      }

    char sym = str_ind[min_ind];
    uniq_name += str_ind.SubStr(min_ind, 1);
    if (min_num == 0)
      break;

    for (in = 0;  in < list_len;  in++)
      {
      if (!fail_mas[in])
        continue;
      const UStr &name = *list[in];
      if ((name[ipos] != sym) || (name.Length() <= ipos))
        fail_mas[in] = false;
      }

    ipos++;
    }

  return SUCCESS;
  } // UStr::GetUniqueName()

//////////////////////////////////////////////////////////////////////////////
/// Set translation function.
///
/// The method sets translation function to be used later by Tr() function.
/// It must be a function of the following type:
/// <b>static UStr TrFunc(const char *context, const char *str, const char *comment).</b>
/// For details see description of tr() function in Qt manual.
/// @note This method must be called before first call of Tr().
/// @param[in] tr_f - Address of translation function.
void UStr::SetTr(UStr (*tr_f)(const char *context, const char *str, const char *comment))
  {
  tr_func = tr_f;
  }

//////////////////////////////////////////////////////////////////////////////
/// Translate a user string.
///
/// The method converts the given char string to the UStr type,
/// possibly translating it to the native language at the same time.
/// For details see description of tr() function in Qt manual.
/// @note It is forbidden to call this method directly. Use Tr() macro instead.
/// @note To provide translation, UStr::SetTr() must be called beforehand.
/// @param[in] context - Name of translation context.
/// @param[in] str - A string to be translated.
/// @param[in] comment - A comment (part of translation context).
/// @return The translated string.
UStr GlobTr(const char *context, const char *str, const char *comment)
  {
  if (UStr::tr_func == NULL)
    return UStr(str);

  // Convert context
  Str cont(context);
  
  // Cut Integra namespace (if any) in the beginning of context
  if (cont.SubStrCmp("Integra::") == 0)
    cont.Exclude(0, (int)strlen("Integra::"));

  // Find all double colons of top level
  TArray<int> pos;
  pos.Add(0);
  int brackets = 0;
  for (int i = 0; i < cont.Length(); i++)
    {
    if (cont[i] == '<')
      {
      brackets++;
      continue;
      }
    if (cont[i] == '>')
      {
      Assert(brackets > 0);
      if (brackets > 0)
        brackets--;
      continue;
      }
    if (brackets == 0 && cont[i] == ':' && cont[i + 1] == ':')
      pos.Add(i + 2);
    }
  if (pos.Length() > 1)
    {
    cont = cont.SubStr(pos[0], pos[pos.Length() - 1] - pos[0] - 2);
    // Template classes are not supported
    if (cont.FindFirst('<') || cont.FindFirst('>'))
      cont = "@default";
    }
  else
    cont = "@default";

  // Call translation function
  return (UStr::tr_func)(cont.Data(), str, comment);
  }

INTEGRA_NAMESPACE_END
