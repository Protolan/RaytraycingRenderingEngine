/// @internal
/// @file
///
/// @brief Definitions of File and InFile classes.
///
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004

// WIN32
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#endif

#include <stdarg.h>
#include <locale.h>
#include <float.h>

#include <integra.hpp>
#include "arrays.hpp"
#include "envi.hpp"
#include "file.hpp"
#include "frw.h"
#include "user.hpp"

INTEGRA_NAMESPACE_START

/// Max count of lines in text file.
static const int MAX_LINES = 1000000;

/**
@class File base/file.hpp

The class is used for the work with files (opening, closing, reading, writing,
deleting, renaming, copying).

@sa @ref base_mainpage
**/

/**
@enum File::TypeMode

Text/binary mode.

Constants describing type of the file.
**/

/**
@enum File::AccessMode

Access Mode.

Constants describing access rights to the file.
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Initializes the file with the filename "Untitled".
File::File() : name(INTEGRA_T("Untitled"))
  {
  fd = NULL;
  user = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor, sets a full path to the file.
///
/// @param[in] full_pathname - A pathname of the file to be constructed.
/// @param[in] user - An optional User object to be used to notify the end user
/// about errors.  Set it to NULL to turn off the feature.
File::File(const PathStr &full_pathname, IUser *user) : name(full_pathname)
  {
  fd = NULL;
  this->user = user;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The file @a sour may not be opened. Debug version asserts it.
/// @param[in] sour - A source object to copy.
File::File(const File &sour) : name(sour.name)
  {
  Assert(sour.fd == NULL);
  fd = NULL;
  user = sour.user;
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
/// @note The file will be closed if it is opened.
File::~File()
  {
  if (fd != NULL)
    Close();
  }

//////////////////////////////////////////////////////////////////////////////
/// Open the file.
///
/// The method opens the file according to the requested mode.
/// @note A path to the file must be specified. Debug version asserts it.
/// @note Debug version asserts if an invalid mode is specified.
/// @param[in] mode - Opening mode. The list of allowed modes:
/// - "r" - open for reading only; the file must exist.
/// - "w" - create a new file or open an existing file for writing.
/// - "a" - open for writing at the end-of-file or create for writing
///          if the file does not exist.
/// - "r+" - open an existing file for reading and writing.
/// - "w+" - open an empty file for reading and writing.
/// - "a+" - open for reading and appending; create a file
///          if it does not exist.
///
/// When the "r+", "w+", or "a+" access type is specified, both reading and
/// writing are allowed (the file is said to be open for "update").
/// However, when you switch between reading and writing, there must be
/// an intervening Flush(), SetPos(), or any of Seek*() operation.
/// The current position can be specified for the SetPos or Seek*()
/// operation, if desired.
///
/// By default, the file is opened in the text mode.
/// The character 'b' may be appended to the above modes to open the file
/// in the binary mode ('b' may also precede the '+' sign).
/// @return SUCCESS or FAILURE (opening error).
OKAY File::Open(const char *mode)
  {
  char open_mode[4];

  // File must be closed
  Assert(fd == NULL);

  // Name of the file must be defined
  Assert(!name.IsNull());

  // First symbol from fixed range
  Assert(mode[0] == 'r' || mode[0] == 'w' || mode[0] == 'a');

  if (mode[0] == 'r')
    mode_rw = MODE_READ;
  else // mode[0] == 'w' or mode[0] == 'a'
    mode_rw = MODE_WRITE;

  mode_tb = MODE_TEXT;  // default
  for (const char *c = mode + 1; *c != '\0'; c++)
    switch(*c)
      {
      case '+':
        mode_rw = MODE_READ_WRITE;
        break;
      case 'b':
        mode_tb = MODE_BINARY;
        break;
      default:
        // Unrecognized mode symbol
        Assert(false);
        break;
      }

  open_mode[0] = mode[0];
  open_mode[1] = '\0';

  if (mode_rw == MODE_READ_WRITE)
    strcat(open_mode, "+");
  if (mode_tb == MODE_BINARY)
    strcat(open_mode, "b");
#ifdef _WIN32
  else
    strcat(open_mode, "t");
#endif

  fd = fopen((char *)name.Data(), open_mode);
  if (fd != NULL)
    return SUCCESS;

  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Can't open file %s for reading. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Can't open file %s for writing. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }  // Open()

//////////////////////////////////////////////////////////////////////////////
/// Check - is file opened or not.
/// @return true of opened.
bool File::Opened()
  {
  return fd != NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Close the file.
/// @note ANSI fclose() compatible.
/// @note The file must be opened. Debug version asserts it.
/// @return SUCCESS (if there were no problems when working with the file
/// and the file was closed successfully); FAILURE (either an I/O error while
/// working with the file or a closing error).
OKAY File::Close()
  {
  Assert(fd != NULL);      // file must be opened
  int err1 = ferror(fd);  // remember if errors occurred
  int err2 = fclose(fd);  // remember close errors
  fd = NULL;
  if (!err1 && !err2)
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    int err;
    if (err1 != 0)
      err = err1;
    else
      err = errno;
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(err));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(err));
    }
  return FAILURE;
  }  // Close()

//////////////////////////////////////////////////////////////////////////////
/// Read a string from the file to a Str object.
///
/// The method reads a string @a str from the file. The new line character
/// will not be put to the resulting Str object.
/// @note The file must be opened on reading. Debug version asserts it.
/// @note Take into account that under Win32 in the text mode CTRL+Z is
/// interpreted as an end-of-file character and CR-LF combinations
/// are translated into a single LF on input.
/// @param[out] str - A Str object to a read string to.
/// @return SUCCESS or FAILURE (reading error).
OKAY File::ReadStr(Str &str)
  {
  Assert(fd != NULL);
  Assert(mode_rw != MODE_WRITE);

  char buf_str[Str::MAX_LENGTH + 1];   // + 1 for sure...

  bool newl;
  if (ReadStr(buf_str, Str::MAX_LENGTH, &newl) != SUCCESS)
    return FAILURE;
  str = buf_str;
  while (!newl)
    {
    if (ReadStr(buf_str, Str::MAX_LENGTH, &newl) != SUCCESS)
      break;
    str += buf_str;
    }

  return SUCCESS;
  }  // ReadStr()

//////////////////////////////////////////////////////////////////////////////
/// Read one char.
/// @return The read char.
int File::GetCh()
  {
  return fgetc(fd);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a string from the file to a Str object with restriction of string length.
///
/// The method reads a string @a str from the file. The new line character
/// will not be put to the resulting Str object.
/// @note The file must be opened on reading. Debug version asserts it.
/// @note Take into account that under Win32 in the text mode CTRL+Z is
/// interpreted as an end-of-file character and CR-LF combinations
/// are translated into a single LF on input.
/// @param[out] str - A Str object to a read string to.
/// @param[in] max_length Maximal length of the string.
/// @return SUCCESS or FAILURE (reading error).
OKAY File::ReadStr(Str &str, int max_length)
  {
  Assert(fd != NULL);
  Assert(mode_rw != MODE_WRITE);

  TArray<char> buf_str;
  if (buf_str.Allocate(max_length + 1) != SUCCESS)   // + 1 for sure...
    return FAILURE;

  bool newl;
  if (ReadStr(buf_str.Data(), max_length, &newl) != SUCCESS)
    return FAILURE;
  str = buf_str.Data();

  return SUCCESS;
  }  // ReadStr()

//////////////////////////////////////////////////////////////////////////////
/// Read a string from the file to a char buffer.
///
/// The method reads a string from the file to a buffer with the specified size.
/// The new line character will not be put to the resulting buffer.
/// If the buffer is not big enough to hold the string, then the next
/// @a len_buff - 1 characters are read into the buffer followed by the null char.
/// @note The file must be opened on reading. Debug version asserts it.
/// @note Take into account that under Win32 in the text mode CTRL+Z is
/// interpreted as an end-of-file character and CR-LF combinations
/// are translated into a single LF on input.
/// @param[out] out_buff - A buffer for the string to be read;
/// may not be NULL, debug version asserts it.
/// @param[in] len_buff - The length of the buffer;
/// must be > 0, debug version asserts it.
/// @param[out] out_nl - whether the newline char was read or not.
/// Pass NULL if this info is not necessary.
/// @return SUCCESS/FAILURE (reading error).
OKAY File::ReadStr(char *out_buff, int len_buff, bool *out_nl)
  {
  Assert(fd != NULL);
  Assert(mode_rw != MODE_WRITE);
  Assert(out_buff != NULL && len_buff > 0);

  if (fgets(out_buff, len_buff, fd) == NULL)
    {
    if (ferror(fd) != 0 && user != NULL)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(ferror(fd)));
    return FAILURE;
    }

  // Remove NL character if any
  if (out_nl != NULL)
    *out_nl = false;
  for (char *pc = out_buff; *pc != '\0'; pc++)
    {
    if (*pc == '\n' || *pc == '\r')
      {
      *pc = '\0';
      if (out_nl != NULL)
        *out_nl = true;
      break;
      }
    }

  return SUCCESS;
  }  // ReadStr()

//////////////////////////////////////////////////////////////////////////////
/// Read data from the binary file to a BYTE buffer.
///
/// The method reads the specified number of bytes from the file (opened in
/// the binary mode) to a buffer.
/// @note The file must be opened for reading in the binary mode.
/// Debug version asserts it.
/// @note Method does not inform about errors, but remembers them.
/// Close() checks for all the errors occured during working with file.
/// After finishing file operations always call Close() and check
/// value returned by it.
/// @param[out] out_buff - A buffer for the data;
/// may not be NULL, debug version asserts it.
/// @param[in] len_buff - The length of the buffer, in bytes;
/// must be > 0, debug version asserts it.
/// @param[out] out_size -The length of the read data, in bytes.
void File::Read(BYTE *out_buff, SIZE_T len_buff, SIZE_T &out_size)
  {

  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY && mode_rw != MODE_WRITE);
  Assert(out_buff != NULL && len_buff < MAX_SIZE_T);

  out_size = integra_fread(out_buff, 1, len_buff, fd);

  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write a string from a Str object to the text file.
///
/// The method writes a string, optionally followed by the new line character,
/// to the file.
/// @note File must be opened for writing in the text mode.
/// Debug version asserts it.
/// @note Method does not inform about errors, but remembers them.
/// Close() checks for all the errors occured during working with file.
/// After finishing file operations always call Close() and check
/// value returned by it.
/// @param[in] str - A string to be written.
/// @param[in] newline - Whether to add the new line character.
/// Default value is true.
/// @return SUCCESS/FAILURE (writing error).
void File::WriteStr(const Str &str, bool newline)
  {

  Assert(fd != NULL);
  Assert(!str.IsNull());
  Assert(mode_tb == MODE_TEXT && mode_rw != MODE_READ);

  WriteStr((char *)str.Data(), newline);
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write a string from a char buffer to the text file.
///
/// The method writes to the file (opened in the text mode) data from a buffer
/// till (not including) the terminating null character.
/// @note The file must be opened for writing in the text mode.
/// Debug version asserts it.
/// @note Method does not inform about errors, but remembers them.
/// Close() checks for all the errors occured during working with file.
/// After finishing file operations always call Close() and check
/// value returned by it.
/// @param[in] out_buff - A buffer with data to write;
/// may not be NULL, debug version asserts it.
/// @param[in] newline - Whether to add the new line character.
/// Default value is true.
void File::WriteStr(const char *out_buff, bool newline)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_TEXT && mode_rw != MODE_READ);
  Assert(out_buff != NULL);

  fputs(out_buff, fd);
  if (newline)
    fputs("\n", fd);
  }

//////////////////////////////////////////////////////////////////////////////
/// Write data from a BYTE buffer to the binary file.
///
/// The method writes the specified number of bytes to the file (opened as
/// a binary one) from @a out_buff.
/// @note The file must be opened for writing in the binary mode.
/// Debug version asserts it.
/// @note Method does not inform about errors, but remembers them.
/// Close() checks for all the errors occured during working with file.
/// After finishing file operations always call Close() and check
/// value returned by it.
/// @param[in] out_buff - A buffer with data to write;
/// may not be NULL, debug version asserts it.
/// @param[in] size - The size of the data to write, in bytes;
/// must be > 0, debug version asserts it.
void File::Write(const BYTE *out_buff, SIZE_T size)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY && mode_rw != MODE_READ);
  Assert(out_buff != NULL && size < MAX_SIZE_T);

  integra_fwrite(out_buff, sizeof(BYTE), size, fd);
  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Flush a file stream.
/// @note The file must be open. Debug version asserts it.
void File::Flush()
  {
  Assert(fd != NULL);

  fflush(fd);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the full pathname of the file.
/// @return The full pathname to the file.
const PathStr &File::PathName() const
  {
  return name;
  }

//////////////////////////////////////////////////////////////////////////////
/// Delete the file.
///
/// The method deletes the file; previously, the file will be closed if it
/// was open.
/// @return SUCCESS or FAILURE (removal error).
/// @note ANSI remove() compatible.
/// @note The file may not be open. Debug version asserts it.
OKAY File::Remove()
  {
  Assert(fd == NULL);
  Assert(!name.IsNull());

  if (remove((char *)name.Data()) != 0)
    {
    if (user != NULL)
      user->ErrorMessage(Tr("Can't remove file %s. %s."), name.Data(), strerror(errno));
    return FAILURE;
    }

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Rename the file.
///
/// The method renames the file to a new filename; previously, the file
/// will be closed if it was open.
/// The method can be used to move files between directories
/// (but not between drives). Directories cannot be moved.
/// @note ANSI rename() compatible.
/// @note The file may not be opened. Debug version asserts it.
/// @param[in] new_file_name - A new name of the file.
/// @return SUCCESS / FAILURE.
OKAY File::Rename(const PathStr &new_file_name)
  {
  Assert(fd == NULL);
  Assert(!name.IsNull() && !new_file_name.IsNull());

  if (rename(name.Data(), new_file_name.Data()) != SUCCESS)
    {
    if (user != NULL)
      user->ErrorMessage(Tr("Can't rename file %s to %s. %s."),
            name.Data(), new_file_name.Data(), strerror(errno));
    return FAILURE;
    }

  name = new_file_name;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Detect the error during file operation.
/// @return A result of the detection (true if there was an error).
bool File::IsError()
  {
  Assert(fd != NULL);
  return ferror(fd) != 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Formatted output to the file.
///
/// Analog of the fprintf() function. For allowed formats please refer to
/// the fprintf() manual.
/// @param[in] str_format - A format string;
/// may not be NULL, debug version asserts it.
/// @return The number of characters written, not including the terminating
/// null character, or a negative value if an output error occurs.
int File::Printf(const CHAR *str_format, ...)
  {
  Assert(fd != NULL);
  Assert(str_format != NULL);

  va_list arg_list;
  va_start(arg_list, str_format);

  int res = vfprintf(fd, (const char *)str_format, arg_list);

  va_end(arg_list);

  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Formatted input from the file.
///
/// Analog of the fscanf() function. For allowed formats please refer to
/// the fscanf() manual.
/// @param[in] str_format - A format string;
/// may not be NULL, debug version asserts it.
/// @return The number of fields successfully converted and assigned.
/// null character, or a negative value if an output error occurs.
int File::Scanf(const CHAR *str_format, ...)
  {
  Assert(fd != NULL);
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

  int res = fscanf(fd, (const char *)str_format,
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
/// Copy the file.
/// @param[in] name_from - A name of the source file.
/// @param[in] name_to - A name of the target file.
/// @param[in] user - An optional User object to be used to notify the end
/// user about errors.  Set it to NULL to turn off the feature.
/// @return  SUCCESS / FAILURE.
OKAY File::Copy(const PathStr &name_from, const PathStr &name_to, IUser *user)
  {
#if (OS_MARK == 'I')   // WIN32

  if (!CopyFile(name_from.Data(), name_to.Data(), false))
    {
    if (user != NULL)
      {
      Str str_err = Envi::FormatErrorMsg(GetLastError());
      user->ErrorMessage(Tr("Can't copy file %s to %s. %s"),
        name_from.Data(), name_to.Data(),
        str_err.IsEmpty() ? "Unknown error." : str_err.Data());
      }
    return FAILURE;
    }

#else                  // UNIX

  File fl_from(name_from);
  File fl_to(name_to);

  if (fl_from.Open("rb") != SUCCESS)
    return FAILURE;

  if (fl_to.Open("wb") != SUCCESS)
    return FAILURE;

  const SIZE_T BUF_LEN = 1000;
  BYTE buff[BUF_LEN];  // currently at stack
  for ( ; ; )
    {
    SIZE_T len = 0;
    fl_from.Read(buff, BUF_LEN, len);
    if (len <= 0)
      break;
    fl_to.Write(buff, len);
    }

  if ((fl_from.Close() != SUCCESS) || (fl_to.Close() != SUCCESS))
    {
    remove(fl_to.name.Data());
    if (user != NULL)
      user->ErrorMessage(Tr("Can't copy file %s to %s. %s."),
            name_from.Data(), name_to.Data(), strerror(errno));
    return FAILURE;
    }

#endif

  return SUCCESS;
  }  // Copy()


/// Copy a file with creation of destination directories.
/// @param[in] name_from Name of the source file.
/// @param[in] name_to Name of the target file.
/// @param[in] user Optional User object to be used to notify end user
/// about errors.  Set it to NULL to turn off the feature.
/// @return  SUCCESS / FAILURE.
OKAY File::CopyWithDir(const PathStr &name_from, const PathStr &name_to,
                       IUser *user)
  {
  // name could contain subdirectory name
  PathStr dest_dir = name_to.Path();
  if (!dest_dir.IsDir() && dest_dir.MakeDir() != SUCCESS)
    {
    if (user != NULL)
      User()->ErrorMessage(Tr("Can't create directory %s. %s."), dest_dir.Data(), strerror(errno));
    return FAILURE;  // proper directory doesn't exist and can't be created
    }

  return File::Copy(name_from, name_to, user);
  }

//////////////////////////////////////////////////////////////////////////////
/// Compare two files.
/// @param[in] path1 Path to the first file.
/// @param[in] path2 Path to the second file.
/// @return true if the files are equal; false otherwise.
bool File::Compare(const PathStr &path1, const PathStr &path2)
  {
  if (!path1.IsFile() || !path2.IsFile())
    return false;
  File f1(path1);
  File f2(path2);
  f1.Open("rb");
  f2.Open("rb");

  INT64 len1 = f1.GetLength();
  INT64 len2 = f2.GetLength();
  if (len1 != len2)
    {
    f1.Close();
    f2.Close();
    return false;
    }

  const int buf_len = 4096;

  BYTE buf1[buf_len], buf2[buf_len];
  SIZE_T l1, l2;
  SIZE_T red_len = 0; 
  for (; ;)
    {
    f1.Read(buf1, buf_len, l1);
    f2.Read(buf2, buf_len, l2);
    if (l1 != l2)
      {
      f1.Close();
      f2.Close();
      return false;
      }
    if (l1 == 0)
      {
      f1.Close();
      f2.Close();
      return true;
      }
    red_len += l1;
    int ret = memcmp(buf1, buf2, l1);
    if (ret != 0)
      {
      f1.Close();
      f2.Close();
      return false;
      }

    if (l1 < buf_len)
      break;
    }

  f1.Close();
  f2.Close();
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the file length.
/// @return File length.
INT64 File::GetLength()
  {
  if (!Opened() || fd == NULL)
    return 0;
#ifdef _WIN32
  int fd_d = _fileno(fd); 
  return _filelengthi64(fd_d);
#else // Linux
  struct stat st;
  int fd_d = fileno(fd);
  if (fstat(fd_d, &st) != 0)
    {
    Assert(false);
    return 0;
    }
  return st.st_size;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Move the pointer to a new location relative to the beginning of the file.
///
/// The method moves the file pointer to a new location that is 'offset'
/// bytes from the beginning of the file.
/// @param[in] offset - A new position of the file pointer.
/// @return SUCCESS / FAILURE.
OKAY File::Seek(INT64 offset)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY || offset == 0);
#ifdef _WIN32
#if _MSC_VER >= 1400
  if (_fseeki64(fd, offset, SEEK_SET) == 0)
#else
  if (fseek(fd, (long)offset, SEEK_SET) == 0)
#endif
#else // Linux
  if (fseek(fd, offset, SEEK_SET) == 0)
#endif
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Move the pointer to a new location relative to its current position.
///
/// The method moves the file pointer to a new location that is 'offset'
/// bytes from its current position.
/// @param[in] offset - A new position of the file pointer.
/// @return SUCCESS / FAILURE.
OKAY File::SeekCur(INT64 offset)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY);
#ifdef _WIN32
#if _MSC_VER >= 1400
  if (_fseeki64(fd, offset, SEEK_CUR) == 0)
#else
  if (fseek(fd, (long)offset, SEEK_CUR) == 0)
#endif
#else // Linux
  if (fseek(fd, offset, SEEK_CUR) == 0)
#endif
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Move the pointer to a new location relative to the end of the file.
///
/// The method moves the file pointer to a new location that is 'offset'
/// bytes from the end of the file.
/// @param[in] offset - A new position of the file pointer.
/// @return SUCCESS / FAILURE.
OKAY File::SeekEnd(INT64 offset)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY);
#ifdef _WIN32
#if _MSC_VER >= 1400
  if (_fseeki64(fd, offset, SEEK_END) == 0)
#else
  if (fseek(fd, (long)offset, SEEK_END) == 0)
#endif
#else // Linux
  if (fseek(fd, offset, SEEK_END) == 0)
#endif
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current pointer position from file beginning.
///
/// @param[out] pos - The current pointer position.
/// @note Any modifications of returned pointer position are
/// not allowed because internal structure of it is different
/// on different platforms i.e. unknown. It can be only passed
/// to SetPos() method to restore position later.
/// @return SUCCESS / FAILURE.
OKAY File::GetPos(FPOS_T &pos)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY);
  if (fgetpos(fd, &pos) == 0)
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the current pointer position from file beginning.
///
/// @param[in] pos - The current pointer position.
/// @note Any modifications of pointer position are not allowed
/// because internal structure of it is different on different
/// platforms i.e. unknown. The only way to obtain valid pointer
/// position is to call GetPos() method.
/// @return SUCCESS / FAILURE.
OKAY File::SetPos(const FPOS_T &pos)
  {
  Assert(fd != NULL);
  Assert(mode_tb == MODE_BINARY);
  if (fsetpos(fd, &pos) == 0)
    return SUCCESS;
  // Notify end user about the error
  if (user != NULL)
    {
    if (mode_rw == MODE_READ)
      user->ErrorMessage(Tr("Error reading file %s. %s."), name.Data(), strerror(errno));
    else
      user->ErrorMessage(Tr("Error writing file %s. %s."), name.Data(), strerror(errno));
    }
  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Test for end of file.
bool File::IsEndOfFile()
  {
  return feof(fd) != 0;
  }

/**
@class InFile base/file.hpp

The class is used for token by token input from text files.

@par Built-in types

These are the types which can be read in from a file with
the overloaded function Read():
<table>
<tr>
  <td><b>Type name</b></td>
  <td><b>Text in file</b></td>
  <td><b>C++ data type</b></td>
  <td><b>Read-in value</b></td>
</tr>
<tr>
  <td>word</td><td>SigmaValues</td><td>Str</td><td>SigmaValues</td>
</tr>
<tr>
  <td>bool</td><td>true</td><td>bool</td><td>true</td>
</tr>
<tr>
  <td>integer</td><td>-125</td><td>int</td><td>-125</td>
</tr>
<tr>
  <td>real</td><td>-1.14e-008</td><td>double, float</td><td>-1.14e-008</td>
</tr>
<tr>
  <td>bracketed</td><td><abc <123 ] d efg ]</td>
  <td>Str</td><td>abc <123 ] d efg</td>
</tr>
</table>

@par Representation of bool

In a file, a bool value is represented as one of the following
(case insensitive) words:

- true
- on
- yes
- enabled

which represent @b true, or

- false
- off
- no
- disabled

which represent @b false.

@par Bracketed expressions

It is a generalisation of the "word". The latter is a "blank-delimited"
string (all characters between the first and the last non-delimiter).
The bracketed expression is the text between arbitrary "brackets":

<table>
<tr>
  <td><b>Text in file</b></td>
  <td><b>Left bracket</b></td>
  <td><b>Right Bracket</b></td>
  <td><b>Read-in string</b></td>
  <td><b>Comment</b></td>
</tr>
<tr>
  <td>| " word is"</td><td>&quot;</td><td>&quot;</td>
  <td>_word is</td><td>most standard</td>
</tr>
<tr>
  <td>| <width, height></td><td><</td><td>></td>
  <td>width, height</td><td>most standard</td>
</tr>
<tr>
  <td>| x = y / 2;</td><td>NULL</td><td>;</td>
  <td>x = y / 2</td><td>only right bracket</td>
</tr>
<tr>
  <td>| def hi, world! enddef;</td><td>def</td><td>enddef</td>
  <td>_hi, world!_</td><td>multicharacter brackets</td>
</tr>
<tr>
  <td>| (ab(cd)ef)</td><td>(</td><td>)</td>
  <td>ab(cd)ef</td><td>nested brackets</td>
</tr>
<tr>
  <td>| &quot;Integra,<br>Inc.&quot;</td><td>&quot;</td><td>&quot;</td>
  <td>Integra, Inc.</td><td>multiline</td>
</tr>
</table>
The vertical line '|' marks "cursor" (current position);
symbol '' marks newline character; symbol '_'
in the whole read-in string shows leading/trailing blanks (spaces).

The brackets are not included in the read-in text. If there are both the left
and right brackets, reading goes until the match for the first opening
bracket is found; if the left bracket is omitted, then the reading goes
from the current position until the first occurrence of the closing bracket.

The closing (right) bracket must not be NULL nor can it end with separators.
Debug version asserts that.

In case of a multiline expression trailing separators are discarded;
of the leading separators retained is only one so that parts of a
multiline expression separated in the file by the newline character will be
separated by one delimiter character.

The bracketed expression may split across lines. The newline character
is not included in the read-in string.

@par Reading values

This section describes behavior of methods of the Read() family.
Each method reads a value of the correspondent type.
The reading goes like this: first separators (default are blanks and tabs)
are skipped;
if after that the current line is empty then the next one is read and
so on, until the "cursor" reaches either EOF or a non-delimiter character.
Then  the "word" starting from this character is read in.

The method reading a Str value reads this word (from the first non-delimiter
to the last non-delimiter within one line) into a string.

The other Read() versions read this word until the first character
that may not belong to a valid text representation of this value.
For example:

<table>
<tr>
  <td><b>text in file</b></td>
  <td><b>as Str</b></td>
  <td><b>as bool</b></td>
  <td><b>as int</b></td>
  <td><b>as real</b></td>
</tr>
<tr>
  <td>| -1.15e-008x aha</td><td>-1.15e-008x</td><td>n/a</td>
  <td>-1</td><td>-1.15e-008</td>
</tr>
<tr>
  <td>| enabled aha</td><td>enabled</td><td>true</td><td>n/a</td><td>n/a</td>
</tr>
</table>

The vertical line '|' marks the "cursor" (current position).

After reading, the cursor is left after the last successfully read-in
character. In the above example, if the reading was into a double (or float),
it is left before "x" and if the reading was into an integer, it is left
before the period '.'.

@par Read bracketed values

This section describes behavior of methods of the Read(..., left, right, ...)
family. They read a bracketed value of the correspondent type.
The methods read a value which is bracketed with @a left - @a right.
Brackets are not included to the output, as well as newlines. If there are
nested brackets like "@<abcd@<efg]hijk]" where brackets are < and ],
then the reading goes to the right bracket which matches the opening one,
i.e. the 2nd ] here.

Brackets are case insensitive. If the left bracket is NULL, then reading starts
from the current position. Whitechars are skipped before the left bracket
or, if it is NULL, before the first non-delimiter.

Examples:

@code
Read(str, "\"", "\"", true);
@endcode

reads a quoted text, then discards the quotes.

@code
Read(str, NULL, "end", true)
@endcode

reads until "end"; the latter is discarded.
**/

/**
@enum InFile::StreamParseErr

Stream parsing error.

This is the error code; "SP_" stands for "Stream Parser".
The class InFile itself uses only some of them;
others are reserved for extensions and derived classes.
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor, initializes by a full path to the file.
/// @param[in] pathname - A pathname to the file to be used for initialization.
/// @param[in] user - An optional User object to be used to notify the end user
/// about errors. Set it to NULL to turn off the feature.
InFile::InFile(const PathStr &pathname, IUser *user) :
  m_file(pathname, user)
  {
  m_pos = 0;
  m_line_no = 0;
  m_separators = " \t";
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor, initializes by the given file.
/// @param[in] src_file - A file to be used for initialization.
InFile::InFile(const File &src_file) :
  m_file(src_file)
  {
  m_pos = 0;
  m_line_no = 0;
  m_separators = " \t";
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The file @a src_file must not be open. Debug version asserts this.
/// @param[in] src_file - A source file for copying.
InFile::InFile(const InFile &src_file) :
  m_file(src_file.m_file)
  {
  m_pos = 0;
  m_line_no = 0;
  m_separators = " \t";
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
/// @note The file will be closed if it was open.
InFile::~InFile()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Open the file.
///
/// The method opens the file for reading in the text mode.
/// @param[in] txt_mode Is file opened in text or binary mode.
///
/// @return SUCCESS / FAILURE (opening error).
///
/// @note Binary mode is necessary for usage of GetPos(), SetPos().
///
OKAY InFile::Open(bool txt_mode)
  {
  const char *md = txt_mode ? "r" : "rb";
  if (m_file.Open(md) != SUCCESS)
    return FAILURE;
  m_line = NULL;
  m_line_no = 0;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Close the file.
/// @note The file must be open. Debug version asserts it.
/// @return SUCCESS (if there were no problems when working with the file
/// and the file was closed successfully).
/// FAILURE (either an I/O error while working with the file or a closing
/// error).
OKAY InFile::Close()
  {
  m_line = NULL;
  return m_file.Close();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current line number.
/// @return A number of the current line.
int InFile::LineNo() const
  {
  return m_line_no;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a text specifying current position in the file.
/// @return A string describing where we are in the file, in the form
/// "line 23 col 55 of file f:\myfile.dat".
Str InFile::Where() const
  {
  Str str;
  str.Printf("line %1i col %1i in file \"%s\"", m_line_no, m_pos, PathName().Data());
  return str;
  }

//////////////////////////////////////////////////////////////////////////////
/// Make sure the current line is read.
///
/// The method checks if the current line is read in and not empty.
/// If so, returns SUCCESS immediately otherwise reads a line from
/// the stream and returns a result of the operation.
///
/// May be overridden in derived classes e.g. to truncate the line
/// past a comment after reading.
/// @note This is the only function that does input.
/// @return SUCCESS / FAILURE.
OKAY InFile::ReadCurLine()
  {
  if (!m_line.IsNull())
    return SUCCESS;
  if (m_file.ReadStr(m_line) != SUCCESS)
    return FAILURE;

  // Find Comments.
  if (!m_comm_sep.IsEmpty())
    {
    int out_pos;
    if (m_line.Find(m_comm_sep, &out_pos, 0))
      m_line.Resize(out_pos);
    }

  m_pos = 0;
  m_line_no++;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read (the rest of) the line.
/// @param[in] str - A string to read the line to.
/// @return SUCCESS / FAILURE.
OKAY InFile::ReadLine(Str &str)
  {
  if (ReadCurLine() != SUCCESS)
    return FAILURE;
  // Get the line
  str = m_line.SubStr(m_pos, m_line.Length() - m_pos);
  m_line = NULL;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for the end of line.
///
/// The method checks if there remains anything except for separators
/// in the current line after the cursor.
/// @return true if the current line after the cursor contains
/// nothing at all or only separators.
bool InFile::CurLineEnded()
  {
  if (m_line.IsNull() || m_pos >= m_line.Length())
    return true;

  int i = m_pos;
  while ((m_line[i] != '\0') && IsSeparator(m_line[i]))
    i++;
  // Whether a non-separator found?
  return m_line[i] == '\0';
  }

//////////////////////////////////////////////////////////////////////////////
/// Read the specified word.
///
/// The method checks if the next word (from the cursor) matches the given
/// text; if so, reads it in. If the current line ended (there is nothing
/// but delimiters), reads in the next line.
/// If not matched, the cursor is left before the first non-delimiter.
/// Otherwise it is moved past the keyword.
/// @param[in] keyword - An expected keyword.
/// @param[in] respect_case - Case sensitivity; true by default.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(const char *keyword, bool respect_case)
  {
  int ierr, oldpos = m_pos;
  if (respect_case)
    ierr = SkipText(keyword);
  else
    ierr = SkipTextIgnCase(keyword);

  if (ierr != SUCCESS)
    return FAILURE;

  // but maybe this is just a part of longer word? This does
  // not suit our purpose!
  if (m_line[m_pos] != '\0' && !IsSeparator(m_line[m_pos]))
    {
    m_pos = oldpos; // return cursor before the word
    return FAILURE;
    }
  return SUCCESS;
  }  // Read(const char *keyword, ...)

//////////////////////////////////////////////////////////////////////////////
/// Read a logical value.
///
/// The method reads from the current line a boolean value. Case is irrelevant.
/// - the words true-yes-on-enabled   are read in as "true".
/// - the words false-no-off-disabled are read in as "false".
///
/// If the word after the cursor is not one of the above or there is a line end,
/// then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] val - A read value.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(bool &val)
  {
  // Skip preceeding blanks
  SkipSeparators();
  if (m_line.IsNull())
    return FAILURE;
  // each call just COMPARES the next word with the given one
  // (ignoring case) AND if fails does not move cursor
  // so we try next variant... If succeeds moves cursor
  // after the word.
  if (Read("true",    false) == SUCCESS ||
      Read("yes",     false) == SUCCESS ||
      Read("on",      false) == SUCCESS ||
      Read("enabled", false) == SUCCESS)
    {
    val = true;
    return SUCCESS;
    }

  if (Read("false",    false) == SUCCESS ||
      Read("no",       false) == SUCCESS ||
      Read("off",      false) == SUCCESS ||
      Read("disabled", false) == SUCCESS)
    {
    val = false;
    return SUCCESS;
    }
  // alas, no match...
  return FAILURE;
  }  // Read(bool &val)

//////////////////////////////////////////////////////////////////////////////
/// Read an integer value.
///
/// The method reads an integer value from the current line.
///
/// If the word after the cursor is not recognized as an integer or there is
/// the line end, then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] val - A read value.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(int &val)
  {
  // Skip preceeding blanks
  SkipSeparators();
  if (m_line.IsNull())
    return FAILURE;
  // Scan the number
  int pos2 = m_pos;
  if ((m_line[pos2] == '-') || (m_line[pos2] == '+'))
    pos2++;
  if (!isdigit(m_line[pos2]))
    return FAILURE;
  while (isdigit(m_line[pos2]))
    pos2++;

  // Get the number
  val = atoi(&m_line[m_pos]);
  m_pos = pos2;
  return SUCCESS;
  }  // Read(int &val)

//////////////////////////////////////////////////////////////////////////////
/// Read a float value.
///
/// The method reads a float value from the current line.
///
/// If the word after the cursor is not recognized as a float or there is the
/// line end, then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] val - A read value.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(float &val)
  {
  SkipSeparators();
  Str buf;
  if (ReadReal(buf) != SUCCESS)
    return FAILURE;

  val = (float)atof(buf);
  if (_isnan(val) || !_finite(val))
    return FAILURE;

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a double value.
///
/// The method reads a double value from the current line.
///
/// If the word after the cursor is not recognized as a double or there is the
/// line end, then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] val - A read value.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(double &val)
  {
  SkipSeparators();
  Str buf;
  if (ReadReal(buf) != SUCCESS)
    return FAILURE;
  val = atof(buf);
  if (_isnan(val) || !_finite(val))
    return FAILURE;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a text.
///
/// The method reads a text string (up to a separator) from the current line.
///
/// If the word after the cursor is not recognized as text or there is the
/// line end, then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] val - A read value.
/// @return SUCCESS / FAILURE.
OKAY InFile::Read(Str &val)
  {
  SkipSeparators();
  if (m_line.IsNull())
    return FAILURE;

  int pos2 = m_pos;
  while ((m_line[pos2] != '\0') && !IsSeparator(m_line[pos2]))
    pos2++;
  val = m_line.SubStr(m_pos, pos2 - m_pos);
  m_pos = pos2;
  return SUCCESS;
  }


//////////////////////////////////////////////////////////////////////////////
/// Read a real number and convert it.
///
/// The method reads a real number from the current line and converts it
/// to a format acceptable to localized atof().
///
/// If the word after the cursor is not recognized as a real or there is the
/// line end, then returned is FAILURE and the cursor is not moved.
/// Otherwise the value is read and the cursor is moved to the first
/// char after the word end.
/// @param[out] buf - A read and converted number.
/// @return SUCCESS / FAILURE.
OKAY InFile::ReadReal(Str &buf)
  {
  // Check that the line is read
  if (m_line.IsNull())
    return FAILURE;
  // Validate the real number follows
  bool valid = false;
  int pos2 = m_pos;
  if ((m_line[pos2] == '-') || (m_line[pos2] == '+'))
    pos2++;
  // Scan integral part
  if (isdigit(m_line[pos2]))
    valid = true;
  while (isdigit(m_line[pos2]))
    pos2++;
  // Check for radix character
  if (m_line[pos2] == '.')
    {
    pos2++;
    // scan fractional part
    if (isdigit(m_line[pos2]))
      valid = true;
    while (isdigit(m_line[pos2]))
      pos2++;
    }
  if (!valid)
    return FAILURE;
  // Check for exponent part
  if ((m_line[pos2] == 'e') || (m_line[pos2] == 'E'))
    {
    int pos3 = pos2 + 1;
    if ((m_line[pos3] == '-') || (m_line[pos3] == '+'))
      pos3++;
    // scan exponent
    while (isdigit(m_line[pos3]))
      {
      pos3++;
      pos2 = pos3;
      }
    }
  // Get it
  buf = m_line.SubStr(m_pos, pos2 - m_pos);
  // Take care about regional settings
  int dot_pos;
  if (buf.FindFirst('.', &dot_pos))
    buf[dot_pos] = *localeconv()->decimal_point;
  // Skip the number
  m_pos = pos2;
  return SUCCESS;
  }  // ReadReal()

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed text.
///
/// The method reads a text (with whitechars) bracketed with @a left - @a right.
/// @param[out] val - A read value.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap - Set to @b true if an expression can occupy several lines.
/// @return
/// - @b SP_NOERROR   If successful.
/// - @b SP_ENDSTREAM No non-delimiter character up to the stream end.
/// - @b SP_NOOPENING No left bracket found
///   (the first non-delimiter symbols differs from it).
/// - @b SP_NOCLOSURE No matching @a right bracket.
InFile::StreamParseErr InFile::Read(Str &val, const char *left, const char *right, bool wrap)
  {
  Assert(right != NULL);;
  SkipSeparators();
  if (CurLineEnded()) // empty line!
    return SP_ENDSTREAM;
  // these are counters of left resp. right brackets
  int nleft = 0, nright = 0, oldpos = m_pos;
  if (left != NULL)
    {
    // if starting after current position is left bracket,
    // then skip it. Otherwise admit there is no such a block
    // and return leaving cursor at the first non-separator
    if (SkipTextIgnCase(left) != SUCCESS)
      return SP_NOOPENING;
    nleft = 1;
    }
  // Left bracket passed. Read text

  bool samebrackets = false;
  if (left && strcmp(left, right) == 0)
    samebrackets = true;

  Str str;
  TArray<char> val_tmp(500); // accumulation of read in characters
  for (int iline = 0; iline < MAX_LINES; iline++) // read file line by line
    {
    if (CurLineEnded())
      {
      // Alas this line remainder is empty.
      // Read next line if allowed by parameters or fail.
      if (!wrap)
        {
        // we can not read next line while current ended => reading of
        // the block failed. Restore position before left bracket
        m_pos = oldpos;
        return SP_NOCLOSURE;
        }
      else if (ReadCurLine() == FAILURE)
        return SP_NOCLOSURE;
      }

    // current line after cursor
    str = m_line.SubStr(m_pos, m_line.Length() - m_pos);

    for ( ; m_pos < m_line.Length(); m_pos++)
      {
      // if brackets are the same then we assume that the first
      // encountered after opening (which happpened ABOVE) will
      // be assumed to be RIGHT bracket
      if (!samebrackets && left && strnicmp(m_line + m_pos, left, strlen(left)) == 0)
        nleft++; // yet one left bracket encountered
      if (strnicmp(m_line + m_pos, right, strlen(right)) == 0)
        {
        // yet one right bracket encountered
        nright++;

        if (nright >= nleft) // ">" is for omitted left bracket
          {
          // end of expression!
          val_tmp.Add('\0'); // null-terminated string
          val = val_tmp.Data();
          m_pos += (int)strlen(right); // past the end of right bracket
          return SP_NOERROR;
          }
        }
      val_tmp.Add(m_line[m_pos]);
      }

    // expression was not closed on this line
    if (!wrap)
      {
      m_pos = oldpos; // restore position before left bracket
      return SP_NOCLOSURE;
      }
    } // end for ( ; ; ) (reading file line by line)
  return SP_NOCLOSURE;
  }  // Read(Str &val, ...)

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed integer value.
///
/// The method reads an integer which is bracketed with @a left - @a right.
/// @param[out] val - A read value.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap - Set to @b true if a value can occupy several lines.
/// @return
/// - @b SP_NOERROR   If successful.
/// - @b SP_ENDSTREAM No non-delimiter character up to stream end.
/// - @b SP_NOOPENING No left bracket found
///   (the first non-delimiter symbols differs from it).
/// - @b SP_NOCLOSURE No matching @a right bracket.
InFile::StreamParseErr InFile::Read(int &val, const char *left, const char *right,
                            bool wrap)
  {
  Str buf;
  InFile::StreamParseErr ret;
  ret = Read(buf, left, right, wrap);
  if (ret != SUCCESS)
    return ret;
  if (buf.CheckAndConvert2Real() != SUCCESS)
    return InFile::SP_BADVALUE;
  val = atoi(buf);
  return InFile::SP_NOERROR;
  }  // Read(int &val, ...)

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed float value.
///
/// The method reads a float bracketed with @a left - @a right.
/// @param[out] val - A read value.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap - Set to @b true if a value can occupy several lines.
/// @return
/// - @b SP_NOERROR   If successful.
/// - @b SP_ENDSTREAM No non-delimiter character up to stream end.
/// - @b SP_NOOPENING No left bracket found
///   (the first non-delimiter symbols differs from it).
/// - @b SP_NOCLOSURE No matching @a right bracket.
InFile::StreamParseErr InFile::Read(float &val, const char *left, const char *right,
                            bool wrap)
  {
  Str buf;
  InFile::StreamParseErr ret;
  ret = Read(buf, left, right, wrap);
  if (ret != SUCCESS)
    return ret;
  if (buf.CheckAndConvert2Real() != SUCCESS)
    return InFile::SP_BADVALUE;
  val = (float)atof(buf);
  return InFile::SP_NOERROR;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read a bracketed double value.
///
/// The method reads a double bracketed with @a left - @a right.
/// @param[out] val - A read value.
/// @param[in] left - An opening bracket; may be NULL.
/// @param[in] right - A closing bracket; may not be NULL.
/// @param[in] wrap - Set to @b true if a value can occupy several lines.
/// @return
/// - @b SP_NOERROR   If successful.
/// - @b SP_ENDSTREAM No non-delimiter character up to stream end.
/// - @b SP_NOOPENING No left bracket found
///   (the first non-delimiter symbols differs from it).
/// - @b SP_NOCLOSURE No matching @a right bracket.
InFile::StreamParseErr InFile::Read(double &val, const char *left, const char *right,
                            bool wrap)
  {
  Str buf;
  InFile::StreamParseErr ret;
  ret = Read(buf, left, right, wrap);
  if (ret != SUCCESS)
    return ret;
  if (buf.CheckAndConvert2Real() != SUCCESS)
    return InFile::SP_BADVALUE;
  val = atof(buf);
  return InFile::SP_NOERROR;
  }

//////////////////////////////////////////////////////////////////////////////
/// Discard tail.
///
/// The method truncates the current line from the given position.
/// @param[in] pos - Symbols pos, pos+1, ... are removed.
/// If exceeds the line length, nothing is done.
/// @return SUCCESS / FAILURE (if error during memory allocation).
/// @internal @note On exit the current position m_pos
/// may be changed to ensure it is not past line end.
OKAY InFile::TruncateCurLineFrom(int pos)
  {
  if (m_line.IsNull())
    return SUCCESS;

  if (pos > m_line.Length())
    return SUCCESS;
  if (pos > 0)
    {
    if (m_pos >= pos)
      m_pos = pos - 1; // cursor can not be beyond line end!
    return m_line.Resize(pos);
    }
  else
    {
    m_pos = 0; // cursor can not be beyond line end!
    m_line = NULL;
    return SUCCESS;
    }
  }  // TruncateCurLineFrom()

//////////////////////////////////////////////////////////////////////////////
/// Read one char
/// @return The read char.
int InFile::GetCh()
  {
  return m_file.GetCh();
  }

//////////////////////////////////////////////////////////////////////////////
/// Skip separators.
///
/// The method skips to the next non-separator character, reading the next
/// line if the current one ended.
void InFile::SkipSeparators()
  {
  while (ReadCurLine() == SUCCESS)
    {
    if (m_line.IsNull())
      return;
    // Skip in-line separators
    while ((m_line[m_pos] != '\0') && IsSeparator(m_line[m_pos]))
      m_pos++;
    // Whether a non-separator found?
    if (m_line[m_pos] != '\0')
      return;
    // Finish with the current line
    m_line = NULL;
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Skip a fixed text.
///
/// The method skips a fixed text field (must match literally).
/// @param[in] text - A text to be skipped.
/// @return SUCCESS / FAILURE.
OKAY InFile::SkipText(const char *text)
  {
  Assert(text != NULL);;
  Assert(!IsSeparator(text[0]));;

  SkipSeparators();
  if (m_line.IsNull())
    return FAILURE;

  int pos2;
  for (pos2 = m_pos; *text != '\0'; text++, pos2++)
    {
    if (*text != m_line[pos2])
      return FAILURE;
    }

  m_pos = pos2;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Skip a fixed text ignoring case.
///
/// The method skips a fixed text field ignoring case
/// (must match literally except case).
/// @param[in] text - A text to be skipped;
/// may not be NULL or separator, debug version asserts it.
/// @return SUCCESS / FAILURE.
OKAY InFile::SkipTextIgnCase(const char *text)
  {
  Assert(text != NULL);;
  Assert(!IsSeparator(text[0]));;

  SkipSeparators();
  if (m_line.IsNull())
    return FAILURE;

  int pos2;
  for (pos2 = m_pos; *text != '\0'; text++, pos2++)
    {
    if (tolower(*text) != tolower(m_line[pos2]))
      return FAILURE;
    }

  m_pos = pos2;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Skip up to the end of the current line.
///
/// @return SUCCESS / FAILURE.
OKAY InFile::SkipCurLine()
  {
  m_line = NULL;
  return ReadCurLine();
  }

//////////////////////////////////////////////////////////////////////////////
/// Delete the file.
/// Firstly, the file will be closed if it was open.
/// @note The file may not be open. Debug version asserts it.
/// @return SUCCESS or FAILURE.
OKAY InFile::Remove()
  {
  return m_file.Remove();
  }

//////////////////////////////////////////////////////////////////////////////
/// Rename the file.
///
/// The method renames the file to @a new_file_name; before that, the file
/// will be closed if it was open.
/// @note The file may not be open. Debug version asserts it.
/// @param[in] new_file_name - A new filename.
/// @return SUCCESS or FAILURE.
OKAY InFile::Rename(const PathStr &new_file_name)
  {
  return m_file.Rename(new_file_name);
  }

//////////////////////////////////////////////////////////////////////////////
/// Detect the error during a file operation.
/// @return A result of the detection.
bool InFile::IsError()
  {
  return m_file.IsError();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the full pathname to the file.
/// @return The full pathname to the file.
PathStr InFile::PathName() const
  {
  return m_file.PathName();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set separators.
///
/// The method sets a new set of token separators.
/// The default separators (if this method was not called) are ' ' and 'tab'.
/// @param[in] str - A string containing a new set of separators.
void InFile::SetSeparators(char *str)
  {
  m_separators = str;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set Comments separator.
///
/// End of string from this separator is ignored.
/// Default comments separators are absent.
/// @param[in] str String containing the comments separator.
void InFile::SetCommentsSeparator(char *str)
  {
  m_comm_sep = str;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current pointer position from file beginning.
///
/// @param[out] pos - The current pointer position.
/// @note Any modifications of returned pointer position are
/// not allowed because internal structure of it is different
/// on different platforms i.e. unknown. It can be only passed
/// to SetPos() method to restore position later.
/// @return SUCCESS / FAILURE.
OKAY InFile::GetPos(FPOS_T &pos)
  {
  return m_file.GetPos(pos);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the current pointer position from file beginning.
///
/// @param[in] pos - The current pointer position.
/// @note Any modifications of pointer position are not allowed
/// because internal structure of it is different on different
/// platforms i.e. unknown. The only way to obtain valid pointer
/// position is to call GetPos() method.
/// @return SUCCESS / FAILURE.
OKAY InFile::SetPos(const FPOS_T &pos)
  {
  return m_file.SetPos(pos);
  }

//////////////////////////////////////////////////////////////////////////////
/// Test for end of file.
/// @return True in case end of file.
bool InFile::IsEndOfFile()
  {
  return m_file.IsEndOfFile();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the whole current line.
/// @return The whole current line (not from the cursor position).
Str &InFile::CurLine()
  {
  return m_line;
  }

INTEGRA_NAMESPACE_END

