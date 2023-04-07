/// @internal
/// @file
///
/// @brief Definition of PathStr class.
///
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01
/// @author Pnd - Nickolay Derjabin, '99.04.01
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <mbstring.h>
#include <process.h>
#include <shlobj.h>
#include <shlwapi.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>


#include <integra.hpp>

#include "arrays.hpp"
#include "base.hpp"
#include "guid.hpp"
#include "str.hpp"
#include "time.hpp"
#include "user.hpp"

INTEGRA_NAMESPACE_START

#ifdef _WIN32
const CHAR PathStr::PATH_SEPARATOR = '\\';
const CHAR *PathStr::PATH_SEPARATOR_STR = INTEGRA_T("\\");
#else
const CHAR PathStr::PATH_SEPARATOR = '/';
const CHAR *PathStr::PATH_SEPARATOR_STR = INTEGRA_T("/");
#endif

/// Path to application root folder.
static PathStr root_path;

/// Maximal number of arguments.
static const int MAX_ARG_IN_CMD = 50;

//////////////////////////////////////////////////////////////////////////////
// Static functions declarations

/// Form command line.
static OKAY FormCmdLine(TArray<CHAR> &cmd,
                        const CHAR *prg_name,
                        const CHAR *argv[],
                        const CHAR &div_ch,
                        bool is_bracket);
/// Convert command line to array of arguments.
static OKAY CmdLineToCmdArgv(const TArray<CHAR> &cmd, CHAR *argv[]);
/// Get length of command line.
static int LengthCmdLine(const CHAR *prg_name, const CHAR *argv[]);
/// Check for spaces in string.
static bool InternalSpacesInString(const CHAR *str, int &p_beg, int &p_end);
/// Compare string with mask.
static int RecMatch(unsigned char *p,  unsigned char *s);
/// Clean path.
static PathStr CleanPath(PathStr path, PathStr name);

/**
@class PathStr base/str.hpp

Processing of file path names.

This class is aimed to deal with file names and paths.
It inherits all the methods of the Str class
adding its own specific methods.

Definition of @b extension: if a file name (without a path) is
represented in the form <em>str1.str2</em>, where @em str2 is a non-empty
string that does not contain dots inside, then @em str2 is the extension,
otherwise the extension is "". The following convention is assumed:
<table>
<tr>
  <td>full name</td><td>basename</td><td>extension</td>
</tr>
<tr>
  <td>"xxx.yyy.zzz"</td><td>"xxx.yyy"</td><td>"zzz"</td>
</tr>
<tr>
  <td>"xxx..yyy"</td><td>"xxx."</td><td>"yyy"</td>
</tr>
<tr>
  <td>"xxx."</td><td>"xxx"</td><td>""</td>
</tr>
<tr>
  <td>"xxx.."</td><td>"xxx."</td><td>""</td>
</tr>
<tr>
  <td>"xxx"</td><td>"xxx"</td><td>""</td>
</tr>
</table>

Definition of @b path: if a full name is represented in the form
<em>str1/str2</em>,
where '/' is the (platform-dependent) path separator and
@em str2 does not contain path separators, then path
is @em str1 else path is "". In some operating systems,
a path may contain a preceding specification of a disk drive
or a network resource (in the UNC form)- they are treated as
a part of a path name.

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor from a path, a name and an extension.
///
/// The method initializes the object by the given path,
/// name and extension.
/// @param[in] path - A path.
/// @note The path separator will be added if it is absent
/// at the end of @a path.
/// @param[in] filename - A base name of the file.
/// @param[in] extension - An extension.
/// @note The dot will be added between @a filename
/// and @a extension if @a extension does not begin with the dot. If
/// @a extension is absent, then filename contains the full name of
/// the file.
PathStr::PathStr(const CHAR *path, const CHAR *filename, const CHAR *extension)
  {
  Assert(filename != NULL);   //  && filename[0] != T'\0');

  *this = INTEGRA_T("");

  if (path != NULL)
    *this += path;
  CorrectPathSeparator();

  // Add path separator at the end, if absent
  int pos;
  if (Length() > 0 &&
      (!FindLast(INTEGRA_T(PATH_SEPARATOR), &pos) || pos != Length() - 1))
    *this += PATH_SEPARATOR_STR;

  *this += filename;
  if (extension != NULL)
    {
    if (extension[0] != '.')
      *this += INTEGRA_T(".");
    *this += extension;
    }
  CorrectPathSeparator();
  }  // PathStr()

//////////////////////////////////////////////////////////////////////////////
/// Get the root.
/// @return The root folder for absolute paths; an empty string for relative paths.
/// @note For Windows the root is either logical drive (like "C:")
/// or machine name for UNC names (like "\\host").
/// For other platforms it is just "/".
PathStr PathStr::Root() const
  {
  if (!IsAbsPath())
    return INTEGRA_T("");

#ifdef _WIN32
  if (Length() > 1 && isalpha((*this)[0]) && (*this)[1] == ':')
    return SubStr(0, 2);
  if (Length() > 2 &&
      (*this)[0] == PATH_SEPARATOR &&
      (*this)[1] == PATH_SEPARATOR &&
      isalnum((*this)[2]))
    {
    int pos;
    if (Find(INTEGRA_T(PATH_SEPARATOR), 2, &pos))
      return SubStr(0, pos);
    return *this;
    }
  // We must not be here
  Assert(false);
  return INTEGRA_T("");
#else // Other platforms
  return PATH_SEPARATOR_STR;
#endif
  }  // Root()

//////////////////////////////////////////////////////////////////////////////
/// Get the path.
/// @return The extracted path from the full path name of the file.
/// If the path can not be extracted, an empty string is returned.
/// @note Logical drives (like "C:\") and
/// UNC names (like "\\host\resource\path\filename")
/// are supported, if supported by the platform.
PathStr PathStr::Path() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  int pos;
  if (!FindLast(INTEGRA_T(PATH_SEPARATOR), &pos))
    return INTEGRA_T("");

  // Separator found

#ifdef _WIN32

  // Check for UNC path
  if (IncorrectUNC())
    return *this;

  // Check for logical drive presence
  if (pos > 0)
    {
    unsigned char *p = _mbsdec((const unsigned char *)Data(), (const unsigned char *)Data() + pos);
    if (p != NULL && *p == ':')
      pos++;
    }

#endif

  // Check for root path
  if (pos == 0)
    pos++;

  return SubStr(0, pos);
  }  // Path()

//////////////////////////////////////////////////////////////////////////////
/// Get the top folder name.
///
/// Top folder name is a name before the first path separator.
/// If disk name is present - it will be returned.
/// @return The top folder name from the full path name of the file.
/// If the path can not be extracted, an empty string is returned.
/// @note Logical drives (like "C:\") and
/// UNC names (like "\\host\resource\path\filename")
/// are supported, if supported by the platform.
PathStr PathStr::GetTopFolder() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  int pos;
  if (!FindFirst(INTEGRA_T(PATH_SEPARATOR), &pos))
    return INTEGRA_T("");

  // Separator found
#ifdef _WIN32
  // Check for UNC path
  if (IncorrectUNC())
    return *this;

  // Check for logical drive presence
  if (pos > 0)
    {
    unsigned char *p = _mbsdec((const unsigned char *)Data(), (const unsigned char *)Data() + pos);
    if (p != NULL && *p == ':')
      pos++;
    }

#endif

  // Check for root path
  if (pos == 0)
    pos++;

  return SubStr(0, pos);
  }  // PathStr::GetTopFolder()

//////////////////////////////////////////////////////////////////////////////
/// Get the pathname relative to the top folder.
/// @return The pathname after the first path separator, or this pathname, if separator is absent.
/// If the name can not be extracted, an empty string is returned.
PathStr PathStr::GetTopChildPathName() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  int pos;
  if (!FindFirst(INTEGRA_T(PATH_SEPARATOR), &pos))
    return *this;

  // Separator found
#ifdef _WIN32
  // Check for UNC path
  if (IncorrectUNC())
    return INTEGRA_T("");
#endif

  pos++;
  return SubStr(pos, Length() - pos);
  }  // PathStr::GetTopChildPathName()

//////////////////////////////////////////////////////////////////////////////
/// Check if given path is inside of this path.
///
/// Method checks if this path contains (or equal to) the given path,
/// i.e. if the file or folder referenced by the given path is inside
/// the folder referenced by this path.
/// @param[in] path Path to check.
/// @return True if this path contains the given path.
bool PathStr::Contains(const CHAR *path) const
  {
  PathStr this_cl(*this), path_cl(path);
  this_cl.Clean();
  path_cl.Clean();

  if (path_cl.Length() < this_cl.Length())
    return false;

  PathStr sub_path_cl = path_cl.SubStr(0, this_cl.Length());
  if (this_cl != sub_path_cl)
    return false;

  if (path_cl.Length() > this_cl.Length() &&
      path_cl[this_cl.Length()] != PATH_SEPARATOR)
    return false;

  return true;  
  }  // PathStr::Contains()

//////////////////////////////////////////////////////////////////////////////
/// Get the filename.
/// @return The full name of the file (with the extension).
/// If the name can not be extracted, an empty string is returned.
PathStr PathStr::FileName() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  int pos;
  if (!FindLast(INTEGRA_T(PATH_SEPARATOR), &pos))
    return *this;

  // Separator found

#ifdef _WIN32

  // Check for UNC path
  if (IncorrectUNC())
    return INTEGRA_T("");

#endif

  pos++;
  return SubStr(pos, Length() - pos);
  }  // FileName()

//////////////////////////////////////////////////////////////////////////////
/// Get the basename.
/// @return The base name of the file (without the extension).
/// If the name can not be extracted, an empty string is returned.
PathStr PathStr::BaseName() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  PathStr fname = FileName();
  int pos;

  if (!fname.FindLast(INTEGRA_T('.'), &pos))
    return fname;

  return fname.SubStr(0, pos);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the pathname without extension.
/// @return The pathname of the file without the extension.
/// If the name can not be extracted, an empty string is returned.
PathStr PathStr::PathBaseName() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  int sep_pos = 0;
  if (!FindLast(INTEGRA_T(PATH_SEPARATOR), &sep_pos))
    sep_pos = 0;

  int pos;

  if (!FindLast(INTEGRA_T('.'), &pos))
    return *this;
  // point in some directory name
  if (sep_pos > pos)
    return *this;

  return SubStr(0, pos);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the extension.
/// @return The extension of the file (without the dot).
/// If the extension can not be extracted, an empty string is returned.
PathStr PathStr::Extension() const
  {
  if (IsEmpty())
    return INTEGRA_T("");

  PathStr fname = FileName();
  int pos;

  if (!fname.FindLast(INTEGRA_T('.'), &pos))
    return INTEGRA_T("");

  return fname.SubStr(pos + 1, fname.Length() - pos - 1);
  }

//////////////////////////////////////////////////////////////////////////////
/// Split into a path, a basename and an extension.
/// @param[in] path - A pointer to the string for the path.
/// @param[in] filename - A pointer to the string for file base name.
/// @param[in] extension - A pointer to the string for the extension.
/// @note If @a extension is NULL (or absent), then the file name with the
/// extension is placed to @a filename.
void PathStr::Split(UStr *path, UStr *filename, UStr *extension) const
  {
  if (path != NULL)
    *path = Path();

  if (filename != NULL)
    {
    if (extension != NULL)
      *filename  = BaseName();
    else
      *filename  = FileName();
    }

  if (extension != NULL)
    *extension = Extension();

  return;
  }

//////////////////////////////////////////////////////////////////////////////
/// Modify the pathname using default values.
///
/// A corresponding part of the full path is substituted by the given values,
/// if it is not present already, and the correspondent argument is not NULL.
/// @param[in] def_path - The default path. The path is added if this object
/// has no path part and @a def_path is not NULL.
/// @param[in] def_base_name - The default base name. Must be always specified.
/// The base name is substituted if this object has no basename part.
/// @param[in] def_ext - The default extension, added to this object if the
/// latter has no extension and @a def_ext is not NULL.
void PathStr::UseDefaults(const CHAR *def_path,
     const CHAR *def_base_name, const CHAR *def_ext)
  {
  UStr path      = Path();
  UStr base_name = BaseName();
  UStr ext       = Extension();
  if ((def_path != NULL) && (path.Length() == 0))
    path = def_path;
  CorrectPathSeparator();
  Assert(def_base_name != NULL);
  if (base_name.Length() == 0)
    base_name = def_base_name;
  if ((def_ext != NULL) && (ext.Length() == 0))
    ext = def_ext;
  (*this) = PathStr(path, base_name, ext);
  }

//////////////////////////////////////////////////////////////////////////////
/// Modify the pathname using given extension.
///
/// Extension of the full path is substituted by the given extension.
/// @param[in] ext - Given extension.
void PathStr::SetExtension(const CHAR *ext)
  {
  UStr path      = Path();
  UStr base_name = BaseName();

  (*this) = PathStr(path, base_name, ext);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set extension to the the pathname, if it is not already set.
///
/// Extension of the full path is kept, and if new extension is not equal to it - it is appended to the pathname.
/// @param[in] ext - Given extension.
void PathStr::SetExtensionIfNotSet(const CHAR *ext)
  {
  UStr curr_ext = Extension();
  if (curr_ext == ext)
    return;
  UStr path      = Path();
  UStr file_name = FileName();

  (*this) = PathStr(path, file_name, ext);
  }

//////////////////////////////////////////////////////////////////////////////
/// Append a path to this object.
/// @param[in] sub_path - A path to append; it may begin with the path
/// separator.
void PathStr::AddPath(const CHAR *sub_path)
  {
  // Process NULL case for sure
  if (sub_path == NULL)
    return;
  // Skip path separators if any
  const CHAR *p = sub_path;
  while (*p == PATH_SEPARATOR)
    p++;
  // Do nothing on empty path
  if (*p == '\0')
    return;

  // Add path separator at the end, if absent
  int pos;
  if (Length() > 0 &&
      (!FindLast(INTEGRA_T(PATH_SEPARATOR), &pos) || pos != Length() - 1))
    *this += PATH_SEPARATOR_STR;

  // Append the path
  *this += p;
  CorrectPathSeparator();
  }  // AddPath()

//////////////////////////////////////////////////////////////////////////////
/// Eliminate superfluous elements.
///
/// The method eliminates superfluous path separators,
/// "." and ".." directories from this object.
void PathStr::Clean()
  {
  *this = CleanPath(*this, INTEGRA_T(""));
  }

//////////////////////////////////////////////////////////////////////////////
/// Paths comparison (system-specific).
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A path to compare with.
/// @return A result of the comparison (result of str(i)cmp()).
/// @internal
/// @note This method is OS dependent. The comparison is case sensitive
/// For OS UNIX and case insensitive for MS Windows.
int PathStr::ComparePathStr(const PathStr& s) const
  {
  if (Data() == NULL)
    {
    if (s.Data() == NULL)
      return 0;
    return -1;
    }
  if (s.Data() == NULL)
    return 1;

#ifdef _WIN32
    return _mbsicmp((const unsigned char *)Data(), (const unsigned char *)s.Data());   // case insensitive
#else                  // UNIX
    return strcmp(Data(), s.Data());    // case sensitive
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Check whether this object is an existing directory.
/// @return @b true if this object is an existing directory.
bool PathStr::IsDir() const
  {
  if (IsEmpty())
    return false;
#ifdef _WIN32
  DWORD res = GetFileAttributes(Data());
  if (res == INVALID_FILE_ATTRIBUTES || !(res & FILE_ATTRIBUTE_DIRECTORY))
    return false;
  return true;
#else
  struct stat file_info;

  if (stat(Data(), &file_info) == -1)
    {
    // Remove the last '\' separator
    int pos;
    if (FindLast(INTEGRA_T(PATH_SEPARATOR), &pos))
      {
      if (pos < Length() - 1)
        return false;
      PathStr path_short = SubStr(0, pos);
      return path_short.IsDir();
      }
    }
  if (file_info.st_mode & S_IFDIR)
    return true;
  return false;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Check whether this object is an existing directory with write permission.
/// @return @b true if this object is an existing directory with write permission.
bool PathStr::IsDirWritePerm() const
  {
  if (IsEmpty())
    return false;

  if (!IsDir())
    return false;

#if 0
  struct stat file_info;

  if (stat(Data(), &file_info) == -1)
    return false;
  if (!(file_info.st_mode & S_IFDIR))
    return false;
  if (file_info.st_mode & S_IWRITE)
    return true;
#else
  // Generate unique name.
  Guid guid;
  Str str;
  guid.ConvertToStr(str);

  PathStr name(Data(), str);
  FILE *ff = fopen(name.Data(), "wD");
  if (ff != NULL)
    {
    fclose(ff);
    return true;
    }
#endif
  return false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check whether this object is an existing file with write permission.
///
/// @return @b true if this object is an existing file with write permission.
///
bool PathStr::WritableFile() const
  {
  if (IsEmpty())
    return false;

  if (!IsFile())
    return false;

  FILE *ff = fopen(Data(), "ab");
  if (ff != NULL)
    {
    fclose(ff);
    return true;
    }
  return false;
  }  // WritableFile()

//////////////////////////////////////////////////////////////////////////////
/// Check whether this object is an existing file.
/// @return @b true if this object is an existing file.
bool PathStr::IsFile() const
  {
  if (IsEmpty())
    return false;
#ifdef _WIN32
  DWORD res = GetFileAttributes(Data());
  if (res == INVALID_FILE_ATTRIBUTES || res & FILE_ATTRIBUTE_DIRECTORY)
    return false;
  return true;
#else
  struct stat file_info;

  if (stat(Data(), &file_info) == -1)
    return false;
  if (file_info.st_mode & S_IFREG)
    return true;
  return false;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Get file size.
/// @note The object must point to an existing file; debug version asserts it.
/// @return Size of the file, in bytes.
INT64 PathStr::FileSize() const
  {
  Assert(IsFile());
  if (IsEmpty())
    return (INT64)0;

#ifdef _WIN32

  WIN32_FILE_ATTRIBUTE_DATA file_info_ex;
  if (GetFileAttributesEx(Data(), GetFileExInfoStandard, &file_info_ex) != 0)
    return ((INT64)file_info_ex.nFileSizeHigh << 32) + file_info_ex.nFileSizeLow;

#endif

  struct stat file_info;

  if (stat(Data(), &file_info) == -1)
    return (INT64)0;

  return (INT64)(file_info.st_size);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get file time.
/// @note The object must point to an existing file; debug version asserts it.
/// @return The time of the last modification of the file.
Time PathStr::FileTime() const
  {
  Assert(IsFile());
  if (IsEmpty())
    return Time((unsigned)0);
#ifdef _WIN32
  WIN32_FILE_ATTRIBUTE_DATA file_info_ex;
  if (GetFileAttributesEx(Data(), GetFileExInfoStandard, &file_info_ex) != 0)
    {
    SYSTEMTIME st_utc, st_local;

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&file_info_ex.ftLastWriteTime, &st_utc);
    SystemTimeToTzSpecificLocalTime(NULL, &st_utc, &st_local);

    return Time(st_local.wYear, st_local.wMonth, st_local.wDay,
      st_local.wHour, st_local.wMinute, st_local.wSecond);
    }
#endif
  struct stat file_info;

  if (stat(Data(), &file_info) == -1)
    return Time((unsigned)0);
  return Time((unsigned)(file_info.st_mtime));
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the list of files and directories.
///
/// The method obtains the list of the names of the files and directories
/// located in the directory defined by this pathname.
/// @param[out] fnames - The resultant list of files and directories.
/// @param[in] mask - An optional search mask; if specified, only files
/// and directories matching the mask will be included in the list.
/// @return SUCCESS / FAILURE.
OKAY PathStr::ListFileNames(TArray<PathStr> &fnames, const PathStr *mask) const
  {
  Assert(IsDir());

  (void)fnames.Allocate(0);
  if (IsEmpty())
    return FAILURE;

  // ------------------------------------------------
  //             Open catalog
  // ------------------------------------------------

#ifdef _WIN32

  HANDLE handle = NULL;

#else                   // UNIX

  DIR *handle = opendir(Data());
  if (handle == NULL)
    return FAILURE;

#endif

  // ------------------------------------------------
  //             Read catalog
  // ------------------------------------------------

  int n_names = 0;

#ifdef _WIN32

  WIN32_FIND_DATA fdata;
  PathStr path(Data(), "*.*");  // All items of the catalog
                                // for the anologous with UNIX
  // Read first item from catalog list
  handle = FindFirstFile(path.Data(), &fdata);
  if (handle == INVALID_HANDLE_VALUE)
    return FAILURE;

  do
    {
    // Skip parent and current catalogs
    if ((fdata.cFileName[0] == '.' && fdata.cFileName[1] == '\0') ||
        (fdata.cFileName[0] == '.' && fdata.cFileName[1] == '.' &&
         fdata.cFileName[2] == '\0'))
      continue;

    PathStr filename = PathStr(fdata.cFileName);

#else                 // UNIX

  struct dirent *dr = NULL;
  while((dr = readdir(handle)) != NULL)
    {
    // Skip parent and current catalogs
    if ((dr->d_name[0] == '.' && dr->d_name[1] == '\0') ||
        (dr->d_name[0] == '.' && dr->d_name[1] == '.' && dr->d_name[2] == '\0'))
      continue;

    PathStr filename = PathStr(dr->d_name);

#endif

    if (mask != NULL && filename.CheckMask(*mask) != SUCCESS)
      continue;

    if (n_names == 0)
      (void)fnames.Allocate(0);

    if (fnames.Add(filename) != SUCCESS)
      return FAILURE;

    n_names++;
    }
#ifdef _WIN32
  while (FindNextFile(handle, &fdata));
#endif

  // ------------------------------------------------
  //             Close catalog
  // ------------------------------------------------

#ifdef _WIN32

  if (!FindClose(handle))
    return FAILURE;

#else                 // UNIX

  if (closedir(handle) != 0)
    return FAILURE;

#endif

  return SUCCESS;
  }  // ListFileNames()

//////////////////////////////////////////////////////////////////////////////
/// Get separate lists of files and folders.
///
/// The method obtains the separate lists of the names of the files
/// and folders located in the directory defined by this pathname.
/// Use this method where performance is crucial - there is no need
/// to call IsFile()/IsDir() after it.
/// @param[out] files - The resultant list of files.
/// @param[out] folders - The resultant list of folders.
/// @return SUCCESS / FAILURE.
OKAY PathStr::ListFilesAndFolders(TArray<PathStr> &files,
                                  TArray<PathStr> &folders) const
  {
  Assert(IsDir());
  Assert(!IsEmpty());

  files.Allocate(0);
  folders.Allocate(0);

#ifdef _WIN32
  HANDLE handle = NULL;
  WIN32_FIND_DATA fdata;
  PathStr path(Data(), "*.*");  // All items of the catalog
  // Read first item from catalog list
  handle = FindFirstFile(path.Data(), &fdata);
  if (handle == INVALID_HANDLE_VALUE)
    return User()->ErrorMessage(Tr("Can't list the contents of '%s' folder"), Data());

  bool ok = true;
  do
    {
    // Skip parent and current catalogs
    if ((fdata.cFileName[0] == '.' && fdata.cFileName[1] == '\0') ||
        (fdata.cFileName[0] == '.' && fdata.cFileName[1] == '.' &&
         fdata.cFileName[2] == '\0'))
      continue;
    bool is_dir = ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    if ((is_dir ? folders : files).Add(fdata.cFileName) != SUCCESS)
      {
      ok = false;
      break;
      }
    }
  while (FindNextFile(handle, &fdata));
  if (!FindClose(handle))
    Assert(false);
  if (!ok)
    return User()->ErrorNoMemory();
  return SUCCESS;

#else // Other platforms
  struct dirent **namelist;
  int i, n;

  n = scandir(Data(), &namelist, NULL, NULL);
  if (n < 0)
    return User()->ErrorMessage(Tr("Can't list the contents of '%s' folder"), Data());
  bool ok = true;
  for (i = 0; i < n; i++)
    {
    struct dirent *de = namelist[i];
    if (ok && (de->d_type == DT_REG || de->d_type == DT_DIR) &&
        (de->d_name[0] != '.' || de->d_name[1] != '\0') &&
        (de->d_name[0] != '.' || de->d_name[1] != '.' || de->d_name[2] != '\0'))
      if ((de->d_type == DT_DIR ? folders : files).Add(de->d_name) != SUCCESS)
        ok = false;
    free(namelist[i]);
    }
  free(namelist);
  if (!ok)
    return User()->ErrorNoMemory();
  return SUCCESS;
#endif
  }  // ListFilesAndFolders()

//////////////////////////////////////////////////////////////////////////////
/// Serialize the string.
/// @note Default is NULL. On import, any path separators are converted to
/// the path separator character used by the platform the software runs.
/// @param[in, out] inout - A Serializer.
/// @param[in] tag - A chunk name.
void PathStr::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  if (inout.Import())
    *this = NULL;
  if (inout.Import() || !IsNull())
    inout.Value(*this);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the full path to the executable module.
///
/// The method gets the full pathname for the executable file containing
/// the specified module.
/// @param[in] name - A pointer to a null-terminated string that contains
/// the name of the module (either a .DLL or .EXE file).
/// If the filename extension is omitted, the default library extension
/// .DLL is appended. The filename string can include a trailing point
/// character (.) to indicate that the module name has no extension.
/// The string need not specify a path.
/// The name is compared (case insensitively) to the names of modules
/// currently mapped into the address space of the calling process.
/// If this parameter is NULL, the method returns a full pathname to the
/// file used to create the calling process.
/// @return SUCCESS / FAILURE.
OKAY PathStr::GetExecutablePath(const CHAR *name)
  {
#ifdef _WIN32
  return GetExecutablePathByHandle(name == NULL ? NULL : GetModuleHandle(name));
#elif (OS_MARK == 'L')
  // In case of Linux full path to executable is taken from
  // system information about process ("/proc" directory).

  CHAR buffer[Str::MAX_LENGTH + 1];
  int n;
  PathStr tmp;

  tmp.Printf("/proc/%d/exe", getpid());
  n = readlink(tmp.Data(), buffer, Str::MAX_LENGTH);
  if (n == -1)
    return FAILURE;

  buffer[n] = '\0';
  *this = PathStr(buffer);
  return SUCCESS;

#elif (OS_MARK == 'U')
  // In case of Irix full path to executable is deduced from
  // argv[0] and search of PATH e.v. and current directory.

  CHAR buffer[Str::MAX_LENGTH + 1];
  extern int __Argc;
  extern char **__Argv;

  int n;
  char *p;
  bool found;
  PathStr tmp;
  const PathStr dot_sep = (Str)"." + PATH_SEPARATOR_STR;

  tmp = __Argv[0];
  if (tmp.Length() == 0)
    return FAILURE;

  found = tmp.FindFirst(PATH_SEPARATOR, &n);

  if (found && n == 0)
    {
    // If argv[0] begins with path separator,
    // it is absolute path - the most simple case.

    *this = tmp;
    return SUCCESS;
    }

  if (!found)
    {
    // If argv[0] does not contain path separator,
    // search it in the PATH e.v.
    PathStr path, dir;
    TArray<PathStr> list;

    p = getenv("PATH");
    if (p == NULL)
      return FAILURE;

    path = p;

    do
      {
      if (path.FindFirst(':', &n))
        {
        dir = path.SubStr(0, n);
        path.Exclude(0, n + 1);
        }
      else
        {
        dir = path;
        path = "";
        }

      list.Truncate();
      if (dir.ListFileNames(list, &tmp) == SUCCESS)
        if (list.Length() > 0)
          {
          // Module name was found
          found = true;
          tmp = PathStr(dir, tmp);
          break;
          }
      }
    while (!found && path.Length() > 0);

    if (!found)
      return FAILURE;

    // If found name begins with "./", it is necessary
    // to add current directory in the beginning -
    // in this case fall to next block which does this.
    if (tmp.SubStrCmp(dot_sep) != 0)
      {
      *this = tmp;
      return SUCCESS;
      }
    }

  // First, sequence "./" must be removed.
  if (tmp.Find(dot_sep, &n))
    tmp.Exclude(n, 2);

  // If argv[0] contains path separator, but not
  // as first symbol, it is relative path - add
  // current durectory in the beginning.
  if (getcwd(buffer, Str::MAX_LENGTH) == NULL)
    return FAILURE;

  *this = PathStr(buffer, tmp);
  return SUCCESS;

#else
  // Not implemented for rest platforms
  return FAILURE;
#endif
  }  // GetExecutablePath()

//////////////////////////////////////////////////////////////////////////////
/// Get the full path to the executable module.
///
/// The method gets the full pathname  for the executable file containing
/// the specified module.
/// @note The method is implemented for the WIN32 platform only and is a
/// direct front-end for GetModuleFileName() from the WIN32 API.
/// @param[in] handle - A handle of the module (either a .DLL or .EXE file).
/// @return SUCCESS / FAILURE.
OKAY PathStr::GetExecutablePathByHandle(const void *handle)
  {
#ifdef _WIN32
  CHAR buffer[Str::MAX_LENGTH + 1];

  if (GetModuleFileName((HMODULE)handle, buffer, Str::MAX_LENGTH) == 0)
    return FAILURE;
  *this = PathStr(buffer);
  return SUCCESS;
#else
  // Linux
  void *p = dlsym((void *)handle, "VersionId");
  if (p != NULL)
    {
    Dl_info info;
    if (dladdr(p, &info) != 0)
      {
      *this = PathStr(info.dli_fname);
      return SUCCESS;
      }
    }
  return FAILURE;
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Get path to the root folder of the application.
///
/// The root application folder is the folder where BASE.DLL is located in,
/// or parent folder, if it is located in a subfolder named BIN or BIND.
/// @return Root application folder.
const PathStr &PathStr::GetRootPath()
  {
  if (root_path.IsNull())
    {
    // Initialize the value
    root_path.GetExecutablePath("base.dll");
    root_path = root_path.Path();
    if (PathStr(root_path.FileName()) == PathStr("bin") ||
        PathStr(root_path.FileName()) == PathStr("bind") ||
        PathStr(root_path.FileName()) == PathStr("bin64") ||
        PathStr(root_path.FileName()) == PathStr("bind64"))
      root_path = root_path.Path();
    }
  return root_path;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get path of special folders.
///
/// Method provides a system-independent way to get path of special folders
/// used frequently by applications, but which may not have the same name or
/// location on any given system, such as "My documents", "Application data",
/// etc.
/// @note Method implemented for MSC_VER >= 1400 (VC++ 2005+).
/// @param[in] folder Identificator of folder to get path of.
/// @param[in] type Type of folder to be returned.
/// @return Path to folder in case of success; NULL otherwise.
/// NULL is returned if "current" type was requested but
/// appropriate folder was removed or renamed by user.
PathStr PathStr::GetSIFolder(SIFolder folder, SIType type)
  {
#if _MSC_VER >= 1400
  int csidl;
  DWORD tp;
  CHAR tmp_path[MAX_PATH + 1];

  switch(folder)
    {
    case SIFOLDER_APPDATA:
      csidl = CSIDL_APPDATA;
      break;
    case SIFOLDER_COMMON_APPDATA:
      csidl = CSIDL_COMMON_APPDATA;
      break;
    case SIFOLDER_COMMON_DOCUMENTS:
      csidl = CSIDL_COMMON_DOCUMENTS;
      break;
    case SIFOLDER_LOCAL_APPDATA:
      csidl = CSIDL_LOCAL_APPDATA;
      break;
    case SIFOLDER_PERSONAL:
      csidl = CSIDL_PERSONAL;
      break;
    default:
      return NULL;
    }

  switch (type)
    {
    case SITYPE_CURRENT:
      tp = SHGFP_TYPE_CURRENT;
      break;
    case SITYPE_DEFAULT:
      tp = SHGFP_TYPE_DEFAULT;
      break;
    default:
      return NULL;
    }

  if (SUCCEEDED(SHGetFolderPath(NULL, csidl, NULL, tp, tmp_path)))
    return tmp_path;
#endif
  return NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the current working directory.
///
/// The method gets the full pathname of the current working
/// directory to this pathname.
/// @return SUCCESS / FAILURE.
OKAY PathStr::CurDir()
  {
  CHAR buffer[Str::MAX_LENGTH + 1];

  // Receive CWD by getcwd()
  if (getcwd(buffer, Str::MAX_LENGTH) == NULL)
    return FAILURE;

  *this = PathStr(buffer);
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Change the current working directory.
///
/// The method changes the current directory to this pathname.
/// @return SUCCESS / FAILURE.
OKAY PathStr::ChDir() const
  {
  if (IsEmpty())
    return FAILURE;

  // Change Current Working Directory by chdir()
  if (chdir(Data()) != 0)
    return FAILURE;

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create a directory.
///
/// The method creates a directory with the name specified by this pathname.
/// @return SUCCESS / FAILURE.
OKAY PathStr::MkDir() const
  {
  if (IsEmpty())
    return FAILURE;

#ifdef _WIN32

  if (mkdir(Data()) == 0)

#else                 // UNIX:  for IRIX and Linux mode masks are same

  mode_t mode = (S_IRUSR | S_IWUSR | S_IXUSR |
                 S_IRGRP | S_IWGRP | S_IXGRP |
                 S_IROTH | S_IWOTH | S_IXOTH);

  if (mkdir(Data(), mode) == 0)

#endif

    return SUCCESS;

  return FAILURE;
  }  // MkDir()

//////////////////////////////////////////////////////////////////////////////
/// Create a subtree of directories.
///
/// The method creates a directory even if several layers of directories
/// should be created.
/// @return SUCCESS / FAILURE.
OKAY PathStr::MakeDir() const
  {
  // If the last character is '\'
  int pos;
  if (FindLast(INTEGRA_T(PATH_SEPARATOR), &pos))
    {
    if (pos == Length() - 1)
      {
      PathStr dir_without_sep = SubStr(0, Length() - 1);
      return dir_without_sep.MakeDir();
      }
    }

  // check and recursively create parent dirs
  PathStr par_dir = Path();
  // If par_dir == *this - this path is empty or it is disk name.
  if (par_dir != *this && !par_dir.IsEmpty() && !par_dir.IsDir())
    {
    if (par_dir.MakeDir() != SUCCESS)
       return FAILURE;
    }

  return MkDir();
  }

//////////////////////////////////////////////////////////////////////////////
/// Create a temporary directory.
///
/// The method creates a temporary directory and places its full
/// pathname to this object.
/// @param[in] pref - A desired prefix for the directory name.
/// of this string are used as prefix of the directory name.
/// @return SUCCESS / FAILURE.
OKAY PathStr::MkTempDir(CHAR *pref)
  {
#ifdef _WIN32

  // Get TEMP dir
  CHAR tmp_dir[MAX_PATH + 1];
  int len = GetTempPath(MAX_PATH, tmp_dir);
  if ((len <= 0) || (len > Str::MAX_LENGTH))
    return FAILURE;

  // Concatenate temp dir and prefix, if any
  CHAR tmp_fname[MAX_PATH + 1];
  strcpy(tmp_fname, tmp_dir);
  if (pref == NULL)
    len = 0;
  else
    {
    len = (int)strlen(pref);
    len = Min(3, len);
    strncat(tmp_fname, pref, len);
    }
  len = (int)strlen(tmp_fname);

  // Try to create temp dir, incrementing suffix if failed
  DWORD i, n = GetTickCount() % 0x100000;
  i = n;
  do
    {
    i = (i + 1) % 0x100000;
    if (i == n)
      return FAILURE;
    sprintf(tmp_fname + len, "%05x.tmp", i);
    *this = tmp_fname;
    }
  while (MkDir() != SUCCESS);
  return SUCCESS;

#else

  return FAILURE;

#endif
  }  // MkTempDir()

//////////////////////////////////////////////////////////////////////////////
/// Remove the directory with this pathname.
/// @note The directory must be empty.
/// @return SUCCESS / FAILURE.
OKAY PathStr::RmDir() const
  {
  if (IsEmpty())
    return FAILURE;

  if (rmdir(Data()) == 0)
    return SUCCESS;

  return FAILURE;
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove directory and all its subdirectories.
///
/// Method removes directory and all its subdirectories with all files.
/// Single file also is removed.
/// @return SUCCESS / FAILURE.
/// @internal Method is recursive.
OKAY PathStr::Remove() const
  {
  if (IsEmpty())
    return FAILURE;

  // If it is file, just remove it
  if (IsFile())
    {
#ifdef _WIN32
    if (!DeleteFile(Data()))
      return FAILURE;
#else
    if (unlink(Data()) != SUCCESS)
      return FAILURE;
#endif
    return SUCCESS;
    }

  // If it is dir, call this method for each inner item
  if (IsDir())
    {
    TArray<PathStr> fnames;
    if (ListFileNames(fnames) != SUCCESS)
      return FAILURE;
    for (int i = 0; i < fnames.Length(); i++)
      {
      PathStr p(Data(), fnames[i]);
      if (p.Remove() != SUCCESS)
        return FAILURE;
      }
    if (RmDir() != SUCCESS)
      return FAILURE;
    }

  return SUCCESS;
  }  // Remove()

//////////////////////////////////////////////////////////////////////////////
/// Rename a file or directory.
/// @note This method can be used to move files between directories,
/// but not between drives. Directories cannot be moved.
/// @param[in] new_name - A new name.
/// @return SUCCESS if the file or directory was successfully renamed;
/// FAILURE otherwise.
OKAY PathStr::Rename(const PathStr &new_name)
  {
  if (IsEmpty() || new_name.IsEmpty())
    return FAILURE;

  if (rename(Data(), new_name.Data()) != 0)
    return FAILURE;

  *this = new_name;
  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start an executable (obsolete version).
///
/// The method runs an external program with this pathname,
/// waits for the completion of the started process and reports a return code.
/// @param[in] argv - An array of the program parameters (without the name
/// of the program).
/// @param[out] rc - An error code returned by the program.
/// @return SUCCESS if the child process was created (@a rc is set to the
/// error code returned by the child process); FAILURE if the child process was
/// not created (@a rc is set to the code of the process creation error).
OKAY PathStr::Exec(const CHAR *argv[], int *rc) const
  {
  // Check name of child's process
  if (IsEmpty())
    return FAILURE;

  errno = 0;  // System variables

#ifdef _WIN32

  // Form command line
  TArray<CHAR> cmd_line;
  if (FormCmdLine(cmd_line, Data(), argv, INTEGRA_T('\0'), true) != SUCCESS)
    return FAILURE;

  CHAR *cmd_argv[MAX_ARG_IN_CMD];
  if (CmdLineToCmdArgv(cmd_line, cmd_argv) != SUCCESS)
    return FAILURE;

  // Workaround for bug in spawn()
  PathStr cmd_argv0(cmd_argv[0]);
  if (cmd_argv0.Length() >= 2 &&
      cmd_argv0[0] == INTEGRA_T('"')  &&
      cmd_argv0[cmd_argv0.Length() - 1] ==  INTEGRA_T('"'))
    cmd_argv0 = cmd_argv0.SubStr(1, cmd_argv0.Length() - 2);

  // Command line should not be empty
  Assert(!cmd_argv0.IsEmpty());

  // Execute Program
  int exit_status = (int)spawnvp(P_WAIT, cmd_argv0.Data(), cmd_argv);
  if (exit_status == -1) // program not started
    {
    if (rc != NULL) // Process was not created
      *rc = errno;
    return FAILURE;
    }

  if (rc != NULL)
    *rc = exit_status;

#else                 // UNIX

  // Prepare command string for function system()
  TArray<CHAR> cmd_line;
  if (FormCmdLine(cmd_line, Data(), argv, INTEGRA_T(' '), true) != SUCCESS)
    return FAILURE;

  // Run EXE module by system() function
  int exit_status = system(cmd_line.Data());
  if (rc != NULL)
    *rc = WEXITSTATUS(exit_status);

  if (errno != 0)
    return FAILURE;

#endif

  return SUCCESS;
  }  // Exec()

//////////////////////////////////////////////////////////////////////////////
/// Get free disk space available to user.
/// @return Free disk space available to user. 
/// @note If per-user quotas are being used, this value for the current path
/// may be less than the total number of free bytes on a disk.
INT64 PathStr::FreeSpace()
  {
  ULARGE_INTEGER free_to_caller, total_bytes, free_bytes;

  if (!GetDiskFreeSpaceEx(Data(), &free_to_caller, &total_bytes, &free_bytes))
    return 0;

  return free_to_caller.QuadPart;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start an executable.
/// @note This function may wait or not to wait for the completion of the
/// started executable. Its difference from the Exec() is that the console can be hidden.
/// @param[in] argv - An array of the program parameters (without the name
/// of the program).
/// @param wait If true, the function will wait till the executable completion.
/// @param mode Console mode flags.
/// @return SUCCESS / FAILURE.
OKAY PathStr::Run(const CHAR *argv[], bool wait, ConsoleMode mode) const
  {
  if (IsEmpty())
    return FAILURE;

#ifdef _WIN32

  // Form string of command line
  TArray<CHAR> cmd_line;
  if (FormCmdLine(cmd_line, Data(), argv, INTEGRA_T(' '), true) != SUCCESS)
    return FAILURE;

  STARTUPINFO startup_info;           // for CreateProcess() call
  PROCESS_INFORMATION process_info;   // filled by CreateProcess

  //  Fill STARTUPINFO structure
  startup_info.cb          = sizeof(startup_info);
  startup_info.lpReserved  = NULL;
  startup_info.lpDesktop   = NULL;
  startup_info.lpTitle     = NULL;
  startup_info.dwFlags     = 0;
  startup_info.cbReserved2 = 0;
  startup_info.lpReserved2 = NULL;

  // Dtermine console mode for new process.
  DWORD process_flags;
  switch (mode)
    {
    case OLD_CONSOLE:
      process_flags = 0;
      break;
    case NEW_CONSOLE:
      process_flags = CREATE_NEW_CONSOLE;
      break;
    default:
      // No console
      process_flags = DETACHED_PROCESS;
    }

  //  Create a process at the same computer
  if (CreateProcess(
      NULL,             // executative filename (NULL == use command line
      cmd_line.Data(),  // full command line for child
      NULL,             // process security attributes
      NULL,             // thread security attributes
      false,            // do not inherit handles
      process_flags,    // creation flags
      NULL,             // environment
      NULL,             // startup directory
      &startup_info,    // pointer to startup info (input)
      &process_info     // pointer to process info (output)
      ) == 0)
    {
    return FAILURE;
    }
  if (wait)
    WaitForSingleObject(process_info.hProcess, INFINITE);
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

#else                 // UNIX
  TArray<CHAR> cmd_line;
  if (FormCmdLine(cmd_line, Data(), argv, INTEGRA_T('\0'), false) != SUCCESS)
    return FAILURE;

  // Remove superfluous '"' symbols
  int n = 0;
  while (n < cmd_line.Length() - 1)
    {
    CHAR *p = cmd_line.Data() + n;
    int l = strlen(p) - 1;
    if (l > 0 && (p[0] == '"' && p[l] == '"'))
      {
      cmd_line.Exclude(n + l, 1);
      cmd_line.Exclude(n, 1);
      }
    n += strlen(cmd_line.Data() + n) + 1;
    }

  CHAR *cmd_argv[MAX_ARG_IN_CMD];
  if (CmdLineToCmdArgv(cmd_line, cmd_argv) != SUCCESS)
    return FAILURE;

  // Command line should not be empty
  Assert(cmd_argv[0] != NULL);

  // fork(): create child process
  pid_t child = fork();
  switch (child)
    {
    case  0:  // current process is child process
      execvp(cmd_argv[0], cmd_argv);
      // if this point is reached then exec failed

    case -1:   // error
      return FAILURE;

    default:  // current process is parent process
      if (wait)
        waitpid(child, NULL, 0);
      break;
    }

#endif

  return SUCCESS;
  }  // Run()

//////////////////////////////////////////////////////////////////////////////
/// Check for mask fitting.
///
/// The method checks the specified mask with this pathname.
/// @param[in] mask - A mask ('*' - any substring, '?' - any single character).
/// @return
/// - SUCCESS if the string matches the mask;
/// - FAILURE if the string does not match the mask.
OKAY PathStr::CheckMask(const PathStr &mask) const
  {
  if (IsNull() || mask.IsNull())
    return FAILURE;
  const CHAR *p1 = Data();
  const CHAR *p2 = mask.Data();

  if (RecMatch((unsigned char *) p2, (unsigned char *)p1) != 1)
    return FAILURE;

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for UNC incorrectness.
///
/// The method checks correctness of the path from the UNC point of view.
/// @return @b true if this is not a correct UNC (insufficient number of '\').
/// @b false if this is not an UNC or it has a proper number of '\'.
bool PathStr::IncorrectUNC() const
  {
  if (Length() < 3)
    return false;

  if ((*this)[0] == PATH_SEPARATOR && (*this)[1] == PATH_SEPARATOR)
    {
    int sep_count = 0;
    for (const unsigned char *p = (const unsigned char *)Data() + 2; *p != 0; p = _mbsinc(p))
      {
      if (*p != PATH_SEPARATOR)
        continue;
      sep_count++;
      if (sep_count == 2)
        return false;
      }
    return true;
    }

  return false;
  }  // IncorrectUNC()


//////////////////////////////////////////////////////////////////////////////
/// Replace all '/' and '\' by correct path separators (system-specific).
void PathStr::CorrectPathSeparator()
  {
  if (IsNull())
    return;
#ifdef _WIN32
  CHAR old_char = '/';
#else
  CHAR old_char = '\\';
#endif
  // Replace all occurances of old_char by PATH_SEPARATOR
  int pos = 0;
  while (FindFirst(old_char, &pos))
    (*this)[pos] = PATH_SEPARATOR;
  }

//////////////////////////////////////////////////////////////////////////////
/// Find a file in this tree of folders.
/// @param[in] file_name - A file name for search.
/// @param[out] path - A full path to the found file without the filename.
/// @return @b true if the file was found, @b false otherwise.
bool PathStr::FindFile(const PathStr &file_name, PathStr &path) const
  {
  // check - is this dir available
  if (!IsDir())
    return false;

  // check in root directory
  PathStr path_name(Data(), file_name);
  if (path_name.IsFile())
    {
    path = *this;
    return true;
    }

  TArray<PathStr> fnames;
  if (ListFileNames(fnames) != SUCCESS)
    return false;

  for (int i = 0; i < fnames.Length(); i++)
    {
    path_name = PathStr(*this, fnames[i]);
    if (path_name.IsDir())
      {
      if (path_name.FindFile(file_name, path))
        return true;
      }
    }

  return false;
  }  // FindFile()


///////////////////////////////////////////////////////////////////////////////
/// Find file among search path folders.
///
/// Method searches this file in the search path folders. If file was found,
/// this value is updated to full path. If file was not found, this value
/// is not changed.
/// @return @b true if the file was found, @b false otherwise.
bool PathStr::FindOnPath()
  {
  if (IsEmpty())
    return false;

#ifdef _WIN32
  CHAR buffer[Str::MAX_LENGTH + 1];
  strcpy(buffer, Data());
  if (PathFindOnPath(buffer, NULL))
    {
    *this = buffer;
    return true;
    }
#endif
  return false;
  }  // FindOnPath()


///////////////////////////////////////////////////////////////////////////////
/// Change attributes of the file.
///
/// This method may be used for the change attributes of the file. For example,
/// file may be hidden if set attribute FILE_ATTR_HIDDEN to parameter of this
/// method. This method may be used for the set to OFF ReadOnly mode of the
/// file - set parameter FILE_ATTR_NORMAL or FILE_ATTR_TEMPORARY.
/// @param mode file attribute. In this parameter the bits combination of
/// FileAttribute enum may be used.
/// @return SUCCESS or FAILURE.
/// @note This method uses Win32 function SetFileAttributes. 
OKAY PathStr::SetFileAttr(int mode) const
  {
#ifdef _WIN32
  
  DWORD attr = 0;

  if (mode & FILE_ATTR_ARCHIVE)
    attr |= FILE_ATTRIBUTE_ARCHIVE;
  if (mode & FILE_ATTR_HIDDEN)
    attr |= FILE_ATTRIBUTE_HIDDEN;
  if (mode & FILE_ATTR_NORMAL)
    attr |= FILE_ATTRIBUTE_NORMAL;
  if (mode & FILE_ATTR_NOT_INDEXED)
    attr |= FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
  if (mode & FILE_ATTR_OFFLINE)
    attr |= FILE_ATTRIBUTE_OFFLINE;
  if (mode & FILE_ATTR_READONLY)
    attr |= FILE_ATTRIBUTE_READONLY;
  if (mode & FILE_ATTR_SYSTEM)
    attr |= FILE_ATTRIBUTE_SYSTEM;
  if (mode & FILE_ATTR_TEMPORARY)
    attr |= FILE_ATTRIBUTE_TEMPORARY;

  if (!SetFileAttributes(Data(), attr))
    return FAILURE;

  return SUCCESS;

#else // Other platforms
  // The only usage of this function at the moment is to set
  // FILE_ATTR_TEMPORARY which is not supported under Linux.
  return SUCCESS;
#endif
  }


//////////////////////////////////////////////////////////////////////////////
/// Check for absolute path.
/// @return true if this path is absolute one, false otherwise. For empty
/// path false is returned.
INTAPI_BASE bool PathStr::IsAbsPath() const
  {
  if (IsEmpty())
    return false;

#ifdef _WIN32
  // PathIsRelative() gives wrong results; parse path ourselves.

  // Check for X:
  if (Length() > 2 &&
      toupper((*this)[0]) >= 'A' &&
      toupper((*this)[0]) <= 'Z' &&
      (*this)[1] == ':' &&
      (*this)[2] == PATH_SEPARATOR)
    return true;

  // Check for \\MACHINE\RESOURCE
  if (Length() < 5)
    return false;

  char c = (*this)[2];
  if ((*this)[0] != PATH_SEPARATOR ||
      (*this)[1] != PATH_SEPARATOR ||
      c <= 0x1F || c == '<' || c == '>'  || c == ':' ||
      c == '"'  || c == '/' || c == '\\' || c == '|')
    return false;

  int p;
  if (!Find(PATH_SEPARATOR, 2, &p))
    return false;

  c = (*this)[p + 1];
  if (c <= 0x1F || c == '<' || c == '>'  || c == ':' ||
      c == '"'  || c == '/' || c == '\\' || c == '|')
    return false;

  return true;
#else // Other platforms
  if ((*this)[0] == PATH_SEPARATOR)
    return true;
  return false;
#endif
  }  // IsAbsPath()

//////////////////////////////////////////////////////////////////////////////
/// Modify this pathname to get the full file path.
///
/// Method updates this path (if needed) to get the full file path.
/// For relative path (for example, "a/b") it prepends the current working directory.
/// For path in a form "X:file" it inserts the current path for particular drive X.
/// Also the path is converted to a comprehensible form:
/// fiolders "." and ".." are removed (resolved appropriately);
/// slashes are converted to back slashes;
/// Trailing dots in file names are eliminated; etc.
///
/// If extension argument is specified, the extension is appended to the file name,
/// or replaces existing extension if it coincides with required one except may be for the letter case.
/// Examples (for "ext" argument):
/// - a     - .../a.ext
/// - a.    - .../a.ext
/// - a.ext - .../a.ext
/// - a.EXt - .../a.ext
/// - a.b   - .../a.b.ext
///
/// @param[in] ext The files's extension to use (optional).
/// @return SUCCESS or FAILURE.
///
/// All errors are reported.
///
INTAPI_BASE OKAY PathStr::MakeFullFilePath(const char *ext)
  {
  Assert(Length() < MAX_PATH);
  Assert((ext == NULL) || ((ext[0] != '.') && (strlen(ext) < MAX_PATH)));
  if (IsEmpty())
    return User()->ErrorMessage(Tr("File path is empty"));
#ifdef _WIN32
  char buffer[MAX_PATH + MAX_PATH];
  char *p_file = NULL;
  int len = GetFullPathName(Data(), MAX_PATH, buffer, &p_file);
  if ((len <= 0) || (len >= MAX_PATH))
#else
  char buffer[PATH_MAX + PATH_MAX];
  char *p_file = NULL;
  int len = 0;
  if (realpath(Data(), buffer) != NULL)
    {
    len = strlen(buffer);
    p_file = strrchr(buffer, PATH_SEPARATOR);
    if (p_file != NULL)
      p_file++;
    }
  if ((len <= 0) || (len >= PATH_MAX))
#endif
    return User()->ErrorMessage(Tr("Can't derive full path name for '%s'"), Data());
  if (p_file == NULL)
    return User()->ErrorMessage(Tr("'%s' couldn't be a file name"), Data());
  // Force extension if given
  if (ext != NULL)
    {
    unsigned char *dot_pos = (unsigned char *)_mbsrchr((const unsigned char *)p_file, '.');
    // Trailing dots are not expected here
    Assert(dot_pos != (unsigned char *)buffer + len - 1);
    if ((dot_pos == NULL) || (_mbsicmp(dot_pos + 1, (const unsigned char *)ext) != 0))
      dot_pos = (unsigned char *)buffer + len;
    // Force (overwrite or append) the new extension
    *dot_pos++ = '.';
    strcpy((char *)dot_pos, ext);
    }
  *this = buffer;
  return SUCCESS;
  }  // MakeFullFilePath(const char *ext)

//////////////////////////////////////////////////////////////////////////////
/// Modify the pathname to get absolute path to existing file.
///
/// Method assumes that this path is a name of existing file. It converts
/// this path to an absolute path to file, searching it, if needed, in the
/// current working directory and then in given library paths, in order.
/// For an absolute path, method just checks that file with such path exists.
/// For a relative path, the method searches the file in the current working
/// directory and library paths, in order. For "X:file" case, the current path
/// of drive X is inserted; the current working directory and library paths
/// are not used.
/// @param[in] lib_paths The absolute library paths to be used for search
/// of file.
/// @return SUCCESS or FAILURE. In case of SUCCESS IsAbsPath() and IsFile()
/// are guaranteed to return true. In case of FAILURE error is reported
/// via User class.
INTAPI_BASE OKAY PathStr::MakeAbsFilePath(const TArray<PathStr> &lib_paths)
  {
  if (IsEmpty())
    {
    User()->ErrorMessage(Tr("Can not convert empty path to absolute path."));
    return FAILURE;
    }

  if (IsAbsPath())
    {
    if (IsFile())
      return SUCCESS;
    User()->ErrorMessage(Tr("File '%s' does not exist."), Data());
    return FAILURE;
    }
  PathStr path = *this;

#ifdef _WIN32
  // Check for "X:file" case
  bool check_libs = Length() < 3 ||
    toupper((*this)[0]) < 'A' ||
    toupper((*this)[0]) > 'Z' ||
    (*this)[1] != ':';
#else
  bool check_libs = true;
#endif

  if (path.MakeFullFilePath() != SUCCESS)
    return FAILURE;

  if (path.IsFile())
    {
    *this = path;
    return SUCCESS;
    }

  if (check_libs)
    {
    path = *this;
    for (int i = 0; i < lib_paths.Length(); i++)
      {
      PathStr lib_path(lib_paths[i]);
      if (!lib_path.IsAbsPath())
        continue;

      // Special processing of "/file" case
      if (path[0] == PATH_SEPARATOR)
        {
        if (lib_path[1] == ':')
          lib_path = lib_path.SubStr(0, 2);
        else if (lib_path[0] == PATH_SEPARATOR &&
                 lib_path[1] == PATH_SEPARATOR)
          {
          int p, q;
          if (lib_path.Find(PATH_SEPARATOR, 2, &p))
            if (lib_path.Find(PATH_SEPARATOR, p + 1, &q))
              lib_path = lib_path.SubStr(0, q);
          }
        }

      PathStr tmp(lib_path, path);
      if (tmp.IsFile())
        {
        *this = tmp;
        Clean();
        return SUCCESS;
        }
      }
    }

  User()->ErrorMessage(Tr("File '%s' could not be found."), Data());
  return FAILURE;
  }  // MakeAbsFilePath()

//////////////////////////////////////////////////////////////////////////////
/// Check path for correctness from syntax point of view.
/// @return True if path is valid (allowed), i.e. contains only allowed symbols,
/// does not contain forbidden names, etc.
INTAPI_BASE bool PathStr::IsAllowedPath() const
  {
  if (IsEmpty())
    return false;

#ifdef _WIN32
  const char *p;
  PathStr path = Path(), name = FileName();
  if (path.IsEmpty())
    {
    // Check for logical drive
    p = name.Data();
    if (name.Length() > 1 && isalpha(p[0]) && p[1] == ':')
      {
      name.Exclude(0, 2);
      if (name.IsEmpty())
        return true;
      }
    return name.IsAllowedFileName();
    }

  if (name.IsEmpty())
    {
    p = path.Data();
    // Check for path ending with path separator
    if (*this != path)
      return path.IsAllowedPath();
    // Check for absolute path
    if (path.Length() == 1 && p[0] == PATH_SEPARATOR)
      return true;
    // Check for logical drive
    if (path.Length() == 3 && isalpha(p[0]) && p[1] == ':' && p[2] == PATH_SEPARATOR)
      return true;
    // Check for UNC
    if (PathIsUNCServerShare(p))
      return true;
    return false;
    }
  return name.IsAllowedFileName() && path.IsAllowedPath();
#else
  return true;
#endif
  }  // IsAllowedPath()

//////////////////////////////////////////////////////////////////////////////
/// Check filename for correctness from syntax point of view.
/// @return True if filename is valid (allowed), i.e. contains only allowed symbols,
/// does not contain forbidden names, etc. Path separators are not allowed.
INTAPI_BASE bool PathStr::IsAllowedFileName() const
  {
#ifdef _WIN32
  if (IsEmpty())
    return false;

  const char *q;
  const unsigned char *p;
  
  // Check for special names
  q = Data();
  if (strcmp(q, ".") == 0 || strcmp(q, "..") == 0)
    return true;
  
  // Check for forbidden symbols
  for (p = (const unsigned char *)Data(); *p != 0; p = _mbsinc(p))
    {
    if (*p < 32 || *p == '<' || *p == '>' || *p == ':' || *p == '"' ||
        *p == '/' || *p == '\\' || *p == '|' || *p == '?' || *p == '*')
      return false;
    q = (const char *)p;
    }

  // Check for space or dot at the end
  if (*q == ' ' || *q == '.')
    return false;

  // Check for forbidden names
  PathStr basename = BaseName();
  q = basename.Data();
  if (stricmp(q, "CON")  == 0 || stricmp(q, "PRN") == 0 ||
      stricmp(q, "AUX")  == 0 || stricmp(q, "NUL") == 0 ||
      stricmp(q, "COM1") == 0 || stricmp(q, "COM2") == 0 ||
      stricmp(q, "COM3") == 0 || stricmp(q, "COM4") == 0 ||
      stricmp(q, "COM5") == 0 || stricmp(q, "COM6") == 0 ||
      stricmp(q, "COM7") == 0 || stricmp(q, "COM8") == 0 ||
      stricmp(q, "COM9") == 0 || stricmp(q, "LPT1") == 0 ||
      stricmp(q, "LPT2") == 0 || stricmp(q, "LPT3") == 0 ||
      stricmp(q, "LPT4") == 0 || stricmp(q, "LPT5") == 0 ||
      stricmp(q, "LPT6") == 0 || stricmp(q, "LPT7") == 0 ||
      stricmp(q, "LPT8") == 0 || stricmp(q, "LPT9") == 0)
    return false;
#endif
  return true;
  }  // IsAllowedFileName()

//////////////////////////////////////////////////////////////////////////////
/// Check base name of the file for correctness from syntax point of view.
/// @return True if basename is valid (allowed), i.e. contains only allowed symbols,
/// does not contain forbidden names, etc. Path separators are not allowed.
INTAPI_BASE bool PathStr::IsAllowedBaseName() const
  {
#ifdef _WIN32
  if (IsEmpty())
    return false;

  const char *q;
  const unsigned char *p;
  
  q = Data();
  
  // Check for forbidden symbols
  for (p = (const unsigned char *)Data(); *p != 0; p = _mbsinc(p))
    {
    if (*p < 32 || *p == '.' || *p == '<' || *p == '>' || *p == ':' || *p == '"' ||
        *p == '/' || *p == '\\' || *p == '|' || *p == '?' || *p == '*')
      return false;
    q = (const char *)p;
    }

  // Check for space or dot at the end
  if (*q == ' ' || *q == '.')
    return false;

  // Check for forbidden names
  q = Data();
  if (stricmp(q, "CON")  == 0 || stricmp(q, "PRN") == 0 ||
      stricmp(q, "AUX")  == 0 || stricmp(q, "NUL") == 0 ||
      stricmp(q, "COM1") == 0 || stricmp(q, "COM2") == 0 ||
      stricmp(q, "COM3") == 0 || stricmp(q, "COM4") == 0 ||
      stricmp(q, "COM5") == 0 || stricmp(q, "COM6") == 0 ||
      stricmp(q, "COM7") == 0 || stricmp(q, "COM8") == 0 ||
      stricmp(q, "COM9") == 0 || stricmp(q, "LPT1") == 0 ||
      stricmp(q, "LPT2") == 0 || stricmp(q, "LPT3") == 0 ||
      stricmp(q, "LPT4") == 0 || stricmp(q, "LPT5") == 0 ||
      stricmp(q, "LPT6") == 0 || stricmp(q, "LPT7") == 0 ||
      stricmp(q, "LPT8") == 0 || stricmp(q, "LPT9") == 0)
    return false;
#endif
  return true;
  }  // PathStr::IsAllowedBaseName()

//////////////////////////////////////////////////////////////////////////////
/// Correct base name by replacing of non-allowed characters by underscore.
/// @return Corrected base name - this name.
INTAPI_BASE PathStr &PathStr::CorrectBaseName() 
  {
#ifdef _WIN32
  char *q;
  unsigned char *p;
  
  // Check for special names
  q = (char *)Data();
  
  // Check for forbidden symbols
  for (p = (unsigned char *)Data(); *p != 0; p = _mbsinc(p))
    {
    if (*p < 32 || *p == '.' || *p == '<' || *p == '>' || *p == ':' || *p == '"' ||
        *p == '/' || *p == '\\' || *p == '|' || *p == '?' || *p == '*')
      *p = '_';
    q = (char *)p;
    }

  // Check for space or dot at the end
  if (*q == ' ' || *q == '.')
    *q = '_';

  // Check for forbidden names
  q = (char *)Data();
  if (stricmp(q, "CON")  == 0 || stricmp(q, "PRN") == 0 ||
      stricmp(q, "AUX")  == 0 || stricmp(q, "NUL") == 0 ||
      stricmp(q, "COM1") == 0 || stricmp(q, "COM2") == 0 ||
      stricmp(q, "COM3") == 0 || stricmp(q, "COM4") == 0 ||
      stricmp(q, "COM5") == 0 || stricmp(q, "COM6") == 0 ||
      stricmp(q, "COM7") == 0 || stricmp(q, "COM8") == 0 ||
      stricmp(q, "COM9") == 0 || stricmp(q, "LPT1") == 0 ||
      stricmp(q, "LPT2") == 0 || stricmp(q, "LPT3") == 0 ||
      stricmp(q, "LPT4") == 0 || stricmp(q, "LPT5") == 0 ||
      stricmp(q, "LPT6") == 0 || stricmp(q, "LPT7") == 0 ||
      stricmp(q, "LPT8") == 0 || stricmp(q, "LPT9") == 0)
    {
    for (p = (unsigned char *)q; *p != 0; p++)
      *p = '_';
    }
#endif
  return *this;
  }  // PathStr::CorrectBaseName()

//////////////////////////////////////////////////////////////////////////////
// Static functions

//////////////////////////////////////////////////////////////////////////////
/// @internal Form command line.
///
/// The method forms command line for the methods Run() and Exec().
/// @param[out] cmd_line Resulting full command line with program name.
/// @note parameters are separated:
/// - in Windows by ' ';
/// - in UNIX by 0.
/// @param[in] program_name Name of the program.
/// @param[in] argv Program arguments (without program name).
/// @param[in] ch Symbol between arguments.
/// @param[in] is_bracket @b true for Win32, @b false for UNIX.
/// @return SUCCESS / FAILURE.
static OKAY FormCmdLine(TArray<CHAR> &cmd_line, const CHAR *program_name,
                        const CHAR *argv[], const CHAR &ch, bool is_bracket)
  {
  // Calculate length of the command line
  int cmd_line_length = LengthCmdLine(program_name, argv);
  if (cmd_line.Allocate(cmd_line_length) != SUCCESS)
    return FAILURE;

  int pos_in_cmd = 0;            // current position inside cmd_line
  CHAR *p_argv = (CHAR *)program_name; // pointer on the current element of argv[]
  int offset = -1;           // index of the current element of argv[]
  while (p_argv != NULL)
    {
    int pos_beg, pos_end;
    bool spaces = InternalSpacesInString(p_argv, pos_beg, pos_end);

    if (p_argv[pos_beg] != '\0')
      {
      // Copy string argv[offset] to the cmd_line
      if (spaces && is_bracket)
        cmd_line[pos_in_cmd++] = '"';

      while (pos_beg < pos_end)
        cmd_line[pos_in_cmd++] = p_argv[pos_beg++];

      if (spaces && is_bracket)
        cmd_line[pos_in_cmd++] = '"';

      cmd_line[pos_in_cmd++] = ch;
      }   // if (*p_argv != '\0')

    // Go to the next item of the command line
    offset++;
    p_argv = (CHAR *)argv[offset];
    }   // while (p_argv != NULL)

  cmd_line.Allocate(pos_in_cmd + 1);
  cmd_line[pos_in_cmd] = '\0';
  return SUCCESS;
  }  // FormCmdLine()

//////////////////////////////////////////////////////////////////////////////
/// @internal Get length of command line.
/// @param[in] program_name Name of the program.
/// @param[in] argv Program arguments (without program name).
/// @return length of command line.
int LengthCmdLine(const CHAR *program_name, const CHAR *argv[])
  {
  SIZE_T cmd_line_length = 0;

  if (program_name != NULL)
    cmd_line_length = strlen(program_name) + 3;

  int i = 0;
  while (argv[i] != NULL)
    {
    cmd_line_length += strlen(argv[i++]) + 3;
    }

  return (int)cmd_line_length;
  }

//////////////////////////////////////////////////////////////////////////////
/// @internal Check for spaces in string.
///
/// The method checks internal spaces in the string,
/// gets first and last positions of non-spaces symbols
/// @param[in] line Initial line.
/// @param[out] pos_beg Position of the first non-space symbol.
/// @param[out] pos_end Position after last non-space symbol.
/// @return @b true if spaces were found; @b false otherwise.
bool InternalSpacesInString(const CHAR *line, int &pos_beg, int &pos_end)
  {
  bool spaces = false;
  const CHAR *p = line, *pp;
  pos_beg = 0;
  pos_end = 0;
  while (*p == ' ' || *p == '\t')
    {
    p++;
    pos_beg++;
    pos_end++;
    }

  while (*p != '\0')
    {
    if (*p == ' ' || *p == '\t')
      {
      pp = p;
      while (*pp == ' ' || *pp == '\t')
        pp++;
      if (*pp == '\0')
        break;

      spaces = true;
      while (*p == ' ' || *p == '\t')
        {
        p++;
        pos_end++;
        }
      }

    pos_end += (int)_mbclen((const unsigned char *)p);
    p += _mbclen((const unsigned char *)p);
    } // while (*p != '\0')

  // Analysis situation "   \"abc  def  \"   "
  if (spaces && pos_end - 1 > pos_beg)
    {
    p = line + pos_beg;
    pp = (const CHAR *)_mbsdec((const unsigned char *)line, (const unsigned char *)line + pos_end);
    if (*p == '"' && *pp == '"')
      spaces = false;
    }

  return spaces;
  }  // InternalSpacesInString()

//////////////////////////////////////////////////////////////////////////////
/// @internal Convert command line to array of arguments.
///
/// The method converts command line (0 between arguments) to array of arguments.
/// @param[in] cmd_line Source command line.
/// @param[out] cmd_argv Resulting array of arguments.
/// @return SUCCESS / FAILURE.
OKAY CmdLineToCmdArgv(const TArray<CHAR> &cmd_line, CHAR *cmd_argv[])
  {
  Assert(cmd_line[0] != '\0');

  // Calculate number of symbols '\0' in cmd_line
  int cmd_length = cmd_line.Length();
  int n_argv = 0;
  for (int i = 0; i < cmd_length; i++)
    if (cmd_line[i] == '\0')
      n_argv++;

  if (n_argv > MAX_ARG_IN_CMD || n_argv < 1)
    return FAILURE;

  CHAR *p_cmd = (CHAR *)cmd_line.Data();

  int pos = 0;
  cmd_argv[pos++] = p_cmd;

  if (pos < n_argv)
    {
    for (int i = 0; i < cmd_length; i++)
      {
      if (cmd_line[i] == '\0')
        {
        cmd_argv[pos++] = p_cmd + i + 1;
        if (pos == n_argv)
          break;
        }
      }
    }

  cmd_argv[pos] = NULL;
  return SUCCESS;
  }  // CmdLineToCmdArgv()

//////////////////////////////////////////////////////////////////////////////
/// @internal Compare string with mask.
/// @param[in] p Pattern to match.
/// @param[in] s String to match.
/// @return
/// - 1 - string matches to mask;
/// - 0, 2  - string does not match to mask.
/// @note Code taken from module "$(Integra)\tools\zip\util.c",
/// function recmatch().
int RecMatch(unsigned char *p, unsigned char *s)
  {
  unsigned char *c;  // pattern char or start of range in [-] loop

  // Get first character, the pattern for new recmatch calls follows
  c = p;
  p = _mbsinc(p);

  // If that was the end of the pattern, match if string empty too
  if (*c == 0)
    return *s == 0 ? 1 : 0;

  // '?' matches any character (but not an empty string)
  if (*c == '?')
    return *s ? RecMatch(p, _mbsinc(s)) : 0;

  // '*' matches any number of characters, including zero
  if (*c == '*')
    {
    if (*p == 0)
      return 1;
    int rc;
    for (; *s; s = _mbsinc(s))
      if ((rc = RecMatch(p, s)) != 0)
        return rc;
    return 2;           // 2 means give up--shmatch will return false
    }

  // If escape ('\'), just compare next character
  if (*c == '\\')
    {
    c = p;
    if (*c == 0)    // if \ at end, then syntax error
      return 0;
    p = _mbsinc(p);
    }

  // Just a character--compare it
#ifdef _WIN32
  if (_mbsnicmp(c, s, 1) == 0)
#else                 // UNIX
  if (c == (*s))
#endif
    return RecMatch(p, _mbsinc(s));

  return 0;
  }  // RecMatch()

//////////////////////////////////////////////////////////////////////////////
/// @internal Clean path.
///
/// The method eliminates superfluous path separators,
/// "." and ".." directories from given path.
/// @note Method is recursive.
/// @param[in] path Path part.
/// @param[in] name Name part. In first call should be empty.
/// @note Both arguments constitutes full (sub)path under processing.
/// @return Cleaned path.
PathStr CleanPath(PathStr path, PathStr name)
  {
  // Remove trailing path separators, if any
  int pos;
  for ( ; ; )
    {
    if (!path.FindLast(INTEGRA_T(PathStr::PATH_SEPARATOR), &pos))
      break;
    if (path.Length() < 2 ||
#ifdef WIN32
        (path.Length() == 3 && path[1] == ':') ||
#endif
        pos != path.Length() - 1)
      break;
    path.Exclude(path.Length() - 1, 1);
    }

  // Remove single '.' from path
  if (name == PathStr("."))
    name = "";
  if (path == PathStr("."))
    path = "";

  // Condition for end of recursion
  if (path.FileName().IsEmpty())
    {
    if (name.IsEmpty())
      return path;
    if (path.IsEmpty())
      return name;
    return PathStr(path, name);
    }

  // Separate path and file name and call recursively
  PathStr p(CleanPath(path.Path(), path.FileName()));

  // If any part is empty, return rest
  if (name.IsEmpty())
    return p;
  if (p.IsEmpty())
    return name;

  // Eliminate ".."
  if (name         == PathStr("..") &&
      p.FileName() != PathStr(".."))
    return p.Path();

  // Return processed path concatenated with name
  return PathStr(p, name);
  }  // CleanPath()

INTEGRA_NAMESPACE_END
