/// @file
///
/// @brief Declarations of the string classes: Str, UStr, PathStr.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01
/// @author Pnd - Nickolay Derjabin, '99.04.01

#ifndef _KLBC_STR_HPP_
#define _KLBC_STR_HPP_

#include <integra.hpp>

#include "export.h"

INTEGRA_NAMESPACE_START

#ifdef PATH_SEPARATOR
#undef PATH_SEPARATOR
#endif
#ifdef PATH_SEPARATOR_STR
#undef PATH_SEPARATOR_STR
#endif

class Serializer;
class Time;
template <class T> class TArray;

/// Processing of strings.
class Str
  {
  public:
    /// @name Constants
    //@{
    /// @brief Maximum length of string.
    ///
    /// Maximum length of string.
    ///
    /// This constant is a recommended maximal size for all the strings.
    /// In particular, it restricts the length of the buffer for the
    /// Printf() method. It is also OK to keep any file or path names.
    enum { MAX_LENGTH = 4096 };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline Str();
    /// Constructor from a string.
    INTAPI_BASE Str(const char *ch);
    /// Constructor from an array of characters.
    INTAPI_BASE Str(const char *ch, int n);
    /// Copy constructor.
    INTAPI_BASE Str(const Str &s);
    /// Destructor.
    INTAPI_BASE ~Str();
    //@}

    /// Allocate string with the given length of string.
    INTAPI_BASE OKAY Allocate(int n);

  public:
    /// @name Access to data
    //@{
    /// Get a reference to a character.
    inline char &operator [](int ind);
    /// Get a character.
    inline char operator [](int ind) const;
    /// Get a pointer to a string.
    inline operator const char *() const;
    /// Get a pointer to a string.
    inline const char *Data() const;
    /// Get a pointer to the string, if it is not NULL, or to empty string.
    INTAPI_BASE const char *PData() const;
    /// Get a pointer to a string - should not be used normally.
    inline char *XData() const;
    //@}

  public:
    /// @name Assignment and concatenation operators
    //@{
    /// Assignment operator.
    INTAPI_BASE void operator =(const char *s);
    /// Assignment operator.
    inline void operator =(const Str &s);
    /// Assignment operator.
    INTAPI_BASE void Set(const char *ch, int n);
    /// Concatenation and assignment operator.
    INTAPI_BASE void operator +=(const char *s);
    /// Concatenation operator.
    INTAPI_BASE Str operator +(const Str &s) const;
    //@}

  public:
    /// @name Comparison operators
    //@{
    /// Equal.
    inline bool operator ==(const char *s) const;
    /// Not equal.
    inline bool operator !=(const char *s) const;
    /// Less.
    inline bool operator <(const char *s) const;
    /// Less or equal.
    inline bool operator <=(const char *s) const;
    /// Greater.
    inline bool operator >(const char *s) const;
    /// Greater or equal.
    inline bool operator >=(const char *s) const;
    /// Case insensitive string comparison.
    inline int ICmp(const char *s) const;
    /// Case sensitive comparison of n chars.
    INTAPI_BASE int NCmp(const char *s, int n) const;
    /// Case insensitive comparison of n chars.
    INTAPI_BASE int NICmp(const char *s, int n) const;
    //@}

  public:
    /// @name String attributes
    //@{
    /// Check for a NULL string.
    inline bool IsNull() const;
    /// Check for an empty string.
    inline bool IsEmpty() const;
    /// Get the string length.
    inline int Length() const;
    //@}

  public:
    /// @name Strings processing
    //@{
    /// Remove blanks from the string.
    INTAPI_BASE void RemoveBlanks();
    /// Remove first and end blanks from string.
    INTAPI_BASE void Clip();
    /// Remove trailing zeros in fractional part of real number.
    INTAPI_BASE void RemoveTrailZeros();
    /// Convert the string to upper case.
    INTAPI_BASE void ToUpper();
    /// Convert the string to lower case.
    INTAPI_BASE void ToLower();
    /// Check the string for a real number and convert it.
    INTAPI_BASE OKAY CheckAndConvert2Real();
    /// Formatted output to the string.
    INTAPI_BASE void Printf(const char *format, ...);
    /// Formatted input from the string.
    INTAPI_BASE int Scanf(const char *format, ...) const;
    /// Append formatted output to the string.
    INTAPI_BASE void Catf(const char *format, ...);
    /// Get integer value from string.
    INTAPI_BASE int IntegerNum(bool &ok) const;
    /// Get double value from string.
    INTAPI_BASE double DoubleNum(bool &ok) const;
    /// Truncate the real numbers to minimize the quantity of digits after point.
    INTAPI_BASE void TruncateReal(double num, int prec, int digits);
    //@}

  public:
    /// @name Substrings and resize
    //@{
    /// Get a substring.
    INTAPI_BASE Str SubStr(int start, int nchars) const;
    /// Compare a substring.
    INTAPI_BASE int SubStrCmp(const char *str, int start = 0) const;
    /// Remove a substring.
    INTAPI_BASE void Exclude(int pos, int n);
    /// Resize the string.
    INTAPI_BASE OKAY Resize(int nchars);
    /// Clear the string.
    INTAPI_BASE void Clear();
    //@}

  public:
    /// @name Search in the string
    //@{
    /// Find the first occurrence of a character.
    INTAPI_BASE bool FindFirst(char c, int *out_pos = NULL) const;
    /// Find the last occurrence of a character.
    INTAPI_BASE bool FindLast(char c, int *out_pos = NULL) const;
    /// Find the first occurrence of a character from the given position.
    INTAPI_BASE bool Find(char c, int in_pos, int *out_pos = NULL) const;
    /// Find a string.
    INTAPI_BASE bool Find(const Str &s, int *out_pos = NULL, int start = 0) const;
    /// Find a string (case insensitive).
    INTAPI_BASE bool IFind(const Str &s, int *out_pos = NULL, int start = 0) const;
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Serialize the string.
    INTAPI_BASE void Serialize(Serializer &inout, const char *tag);
    //@}

  private:
    /// @name Private members
    //@{
    /// Buffer for the string storage.
    char *buf;
    //@}
  };  // Str class

INTEGRA_NAMESPACE_END

/// Representation of characters for UStr.
typedef char CHAR;
/// Macro converter for char literals.
/// What is the sense in such conversion?
#define INTEGRA_T(x) x

#if (_MSC_VER >= 1400) || defined(__linux)
/// Macro for translation of user strings.
#define Tr(str, ...) GlobTr(__FUNCTION__, str, ##__VA_ARGS__)
/// Dummy macro for translation of user strings.
#define FTr(str, ...) (UStr(str))
#else // Not supported under VC++ 6.0
/// Macro for translation of user strings.
#define Tr(str) (UStr(str))
/// Dummy macro for translation of user strings.
#define FTr(str) (UStr(str))
#endif

#ifndef QT_TR_NOOP
/// Macro for translation of user strings outside of a function.
#define QT_TR_NOOP(x) (x)
#endif
#ifndef QT_TRANSLATE_NOOP
/// Macro for translation of user strings (with context) outside of a function.
#define QT_TRANSLATE_NOOP(scope, x) (x)
#endif

INTEGRA_NAMESPACE_START

/// Deals with strings of CHARs.
class UStr : public Str
  {
  public:
    /// @name Enumerations
    //@{
    /// Code pages constants.
    enum CodePage
      {
      DEF_CP = 0,
      SHIFT_JIS = 932,
      WIN1251 = 1251,
      BIG5 = 10002,
      GB2312 = 20936
      };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline UStr();
    /// Constructor from a string.
    inline UStr(const CHAR *s);
    /// Constructor from an array of characters.
    inline UStr(const char *ch, int n);
    /// Constructor from Str.
    inline UStr(const Str &s);
    /// Copy constructor.
    inline UStr(const UStr &s);
    //@}

  public:
    /// @name Assignment and concatenation operators
    //@{
    /// Assignment operator.
    inline void operator =(const UStr &s);
    /// Concatenation and assignment operator.
    inline void operator +=(const UStr &s);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Serialize the string.
    INTAPI_BASE void Serialize(Serializer &inout, const char *tag);
    //@}

  public:
    /// @name Encodings
    //@{
    /// Convert a multibyte string to/from a Unicode UTF-8 string.
    INTAPI_BASE OKAY Encode(const Str &input, CodePage codepage = DEF_CP, bool to_utf8 = true);
    /// Convert a multibyte string to a Unicode UTF-16 string.
    INTAPI_BASE static OKAY ToUnicode(const CHAR *input, TArray<unsigned short> &unicode, CodePage codepage = DEF_CP);
    /// Convert a Unicode UTF-16 string to a multibyte string.
    INTAPI_BASE OKAY FromUnicode(const unsigned short *unicode, CodePage codepage = DEF_CP);
    /// Convert a multibyte string to a Unicode UTF-32 string.
    INTAPI_BASE static OKAY ToUnicode32(const CHAR *input, TArray<unsigned int> &unicode, CodePage codepage = DEF_CP);
    /// Convert a Unicode UTF-32 string to a multibyte string.
    INTAPI_BASE OKAY FromUnicode32(const unsigned int *unicode, CodePage codepage = DEF_CP);
    //@}

  public:
    /// Get unique name (prefix) for List of names.
    INTAPI_BASE static OKAY GetUniqueName(TArray<const UStr *> &list, UStr &uniq_name);

  public:
    /// @name Translation
    //@{
    /// Translate a user string.
    INTAPI_BASE friend UStr GlobTr(const char *context, const char *str, const char *comment);
    /// Set translation function.
    INTAPI_BASE static void SetTr(UStr (*tr_func)(const char *context, const char *str, const char *comment));
    //@}

  private:
    /// Pointer to translation function.
    static UStr (*tr_func)(const char *context, const char *str, const char *comment);
  };  // UStr class

/// Translate a user string.
INTAPI_BASE UStr GlobTr(const char *context, const char *str, const char *comment = NULL);

/// Processing of file path names.
class PathStr : public UStr
  {
  public:
    /// @name Constants
    //@{
    /// Platform-dependent path separator as a character.
    /// Direct usage of path separators in the programs violates
    /// portability - use this constant instead.
    INTAPI_BASE static const CHAR PATH_SEPARATOR;
    /// Platform-dependent path separator as a string.
    /// Direct usage of path separators in the programs violates
    /// portability - use this constant instead.
    INTAPI_BASE static const CHAR *PATH_SEPARATOR_STR;
    /// @brief Console mode for new processes.
    ///
    /// Console mode for new processes.
    enum ConsoleMode
      {
      /// Created process will have no console window.
      NO_CONSOLE,
      /// Created process will reuse console window of parent process.
      OLD_CONSOLE,
      /// Created process will have own new console window.
      NEW_CONSOLE
      };

    /// @brief File attributes.
    ///
    /// List of the file attributes for the SetFileAttr method.
    enum FileAttribute
      {
      /// @brief The file is an archive file. Applications use this attribute
      /// to mark files for backup or removal.
      FILE_ATTR_ARCHIVE = 0x00000020,
      /// @brief The file is hidden. It is not included in an ordinary 
      /// directory listing.
      FILE_ATTR_HIDDEN = 0x00000002,
      /// @brief The file has no other attributes set. This attribute 
      /// is valid only if used alone. 
      FILE_ATTR_NORMAL = 0x00000080, 
      /// @brief The file will not be indexed by the content indexing service.
      FILE_ATTR_NOT_INDEXED = 0x00002000,
      /// @brief The data of the file is not immediately available. This 
      /// attribute indicates that the file data has been physically moved 
      /// to offline storage. This attribute is used by Remote Storage, the 
      /// hierarchical storage management software. Applications should not 
      /// arbitrarily change this attribute.
      FILE_ATTR_OFFLINE = 0x00001000,
      /// @brief  The file is read-only. Applications can read the file
      /// but cannot write to it or delete it. 
      FILE_ATTR_READONLY = 0x00000001,
      /// @brief  The file is part of the operating system or is used 
      /// exclusively by it. 
      FILE_ATTR_SYSTEM = 0x00000004,      
      /// @brief The file is being used for temporary storage. File 
      /// systems avoid writing data back to mass storage if sufficient
      /// cache memory is available, because often the application deletes
      /// the temporary file shortly after the handle is closed. In that case, 
      /// the system can entirely avoid writing the data. Otherwise, the data 
      /// will be written after the handle is closed.
      FILE_ATTR_TEMPORARY = 0x00000100
      };

    /// @brief System-independent identificators of special folders.
    ///
    /// System-independent identificators of special folders.
    enum SIFolder
      {
      /// The file system directory that serves as a common repository
      /// for application-specific data. A typical path is
      /// C:\\Documents and Settings\\\<username\>\\Application Data.
      SIFOLDER_APPDATA,
      /// The file system directory containing application data for all users.
      /// A typical path is
      /// C:\\Documents and Settings\\All Users\\Application Data.
      SIFOLDER_COMMON_APPDATA,
      /// The file system directory that contains documents that are common
      /// to all users. A typical paths is
      /// C:\\Documents and Settings\\All Users\\Documents. 
      SIFOLDER_COMMON_DOCUMENTS,
      /// The file system directory that serves as a data repository for
      /// local (nonroaming) applications. A typical path is
      /// C:\\Documents and Settings\\\<username\>\\Local Settings\\Application Data.
      SIFOLDER_LOCAL_APPDATA,
      /// The virtual folder representing the My Documents desktop item.
      SIFOLDER_PERSONAL
      };

    /// @brief Flags to specify which system-independent path is to be returned.
    ///
    /// Flags to specify which system-independent path is to be returned.
    /// It is used for cases where the folder associated with a SIFolder
    /// may be moved or renamed by the user.
    enum SIType
      {
      /// Return the folder's current path.
      SITYPE_CURRENT,
      /// Return the folder's default path.
      SITYPE_DEFAULT
      };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    inline PathStr();
    /// Constructor from a string.
    inline PathStr(const CHAR *s);
    /// Constructor from Str.
    inline PathStr(const Str &s);
    /// Constructor from UStr.
    inline PathStr(const UStr &s);
    /// Constructor from a path, a basename and an extension.
    INTAPI_BASE PathStr(const CHAR *path, const CHAR *filename, const CHAR *extension = NULL);
    /// Copy constructor.
    inline PathStr(const PathStr &s);
    //@}

  public:
    /// @name Assignment operators
    //@{
    /// Assignment operator.
    inline PathStr &operator =(const PathStr &s);
    //@}

  public:
    /// @name Path splitting
    //@{
    /// Get the root.
    INTAPI_BASE PathStr Root() const;
    /// Get the path.
    INTAPI_BASE PathStr Path() const;
    /// Get the filename.
    INTAPI_BASE PathStr FileName() const;
    /// Get the basename.
    INTAPI_BASE PathStr BaseName() const;
    /// Get the extension.
    INTAPI_BASE PathStr Extension() const;
    /// Get the pathname without extension.
    INTAPI_BASE PathStr PathBaseName() const;
    /// Split into the path, the basename and the extension.
    INTAPI_BASE void Split(UStr *path, UStr *filename,
                           UStr *extension = NULL) const;
    /// Get the top folder name.
    INTAPI_BASE PathStr GetTopFolder() const;
    /// Get the pathname relative to the top folder.
    INTAPI_BASE PathStr GetTopChildPathName() const;
    /// Check if given path is inside of this path.
    INTAPI_BASE bool Contains(const CHAR *path) const;
    //@}

  public:
    /// @name Path modification
    //@{
    /// Modify the pathname using default values.
    INTAPI_BASE void UseDefaults(const CHAR *path, const CHAR *base_name,
                                 const CHAR *ext);
    /// Modify the pathname using given extension.
    INTAPI_BASE void SetExtension(const CHAR *ext);
    /// Set extension to the the pathname, if it is not already set.
    INTAPI_BASE void SetExtensionIfNotSet(const CHAR *ext);
    /// Append a path.
    INTAPI_BASE void AddPath(const CHAR *sub_path);
    /// Eliminate superfluous elements.
    INTAPI_BASE void Clean();
    /// Check for absolute path.
    INTAPI_BASE bool IsAbsPath() const;
    /// Modify the pathname to get absolute path.
    INTAPI_BASE OKAY MakeFullFilePath(const char *ext = NULL);
    /// Modify the pathname to get absolute path to existing file.
    INTAPI_BASE OKAY MakeAbsFilePath(const TArray<PathStr> &lib_paths);
    //@}

  public:
    /// @name Path correctness
    //@{
    /// Check path for correctness from syntax point of view.
    INTAPI_BASE bool IsAllowedPath() const;
    /// Check filename for correctness from syntax point of view.
    INTAPI_BASE bool IsAllowedFileName() const;
    /// Check base name of the file for correctness from syntax point of view.
    INTAPI_BASE bool IsAllowedBaseName() const;
    /// Correct base name by replacing of non-allowed characters by underscore.
    INTAPI_BASE PathStr &CorrectBaseName(); 
    //@}

  public:
    /// @name Comparison operators
    //@{
    /// Equal.
    inline bool operator ==(const PathStr &s) const;
    /// Not equal.
    inline bool operator !=(const PathStr &s) const;
    /// Less.
    inline bool operator <(const PathStr &s) const;
    /// Less or equal.
    inline bool operator <=(const PathStr &s) const;
    /// Greater.
    inline bool operator >(const PathStr &s) const;
    /// Greater or equal.
    inline bool operator >=(const PathStr &s) const;
    //@}

  public:
    /// @name Path attributes
    //@{
    /// Check for file existence.
    INTAPI_BASE bool IsFile() const;
    /// Check for directory existence.
    INTAPI_BASE bool IsDir() const;
    /// Check for directory with write permission.
    INTAPI_BASE bool IsDirWritePerm() const;
    /// Check the file for write permission.
    INTAPI_BASE bool WritableFile() const;
    /// Get file size.
    INTAPI_BASE INT64 FileSize() const;
    /// Get file time.
    INTAPI_BASE Time FileTime() const;
    /// Change attributes of the file.
    INTAPI_BASE OKAY SetFileAttr(int mode) const;
    //@}

  public:
    /// @name Listing and search
    //@{
    /// Get list of files and folders.
    INTAPI_BASE OKAY ListFileNames(TArray<PathStr> &fnames,
                                   const PathStr *mask = NULL) const;
    /// Get the separate lists of files and folders.
    INTAPI_BASE OKAY ListFilesAndFolders(TArray<PathStr> &files,
                                         TArray<PathStr> &folders) const;
    /// Find file in the tree of folders.
    INTAPI_BASE bool FindFile(const PathStr &file_name, PathStr &path) const;
    /// Find file among search path folders.
    INTAPI_BASE bool FindOnPath();
    //@}

  public:
    /// @name Operations with directories
    //@{
    /// Get the current working directory.
    INTAPI_BASE OKAY CurDir();
    /// Change the current working directory.
    INTAPI_BASE OKAY ChDir() const;
    /// Create a directory.
    INTAPI_BASE OKAY MkDir() const;
    /// Create a subtree of directories.
    INTAPI_BASE OKAY MakeDir() const;
    /// Create a temporary directory.
    INTAPI_BASE OKAY MkTempDir(CHAR *pref = "int");
    /// Remove a directory.
    INTAPI_BASE OKAY RmDir() const;
    /// Remove directory and all its subdirectories.
    INTAPI_BASE OKAY Remove() const;
    //@}

  public:
    /// @name Get paths and rename
    //@{
    /// Get the full path to the executable module.
    INTAPI_BASE OKAY GetExecutablePath(const CHAR *name = NULL);
    /// Get the full path to the executable module.
    INTAPI_BASE OKAY GetExecutablePathByHandle(const void *handle);
    /// Change the name of a file or directory.
    INTAPI_BASE OKAY Rename(const PathStr &new_name);
    //@}

  public:
    /// @name Get paths
    //@{
    /// Get path to the root folder of the application.
    INTAPI_BASE static const PathStr &GetRootPath();
    /// Get path of special folders.
    INTAPI_BASE static PathStr GetSIFolder(SIFolder folder, SIType type);
    //@}

  public:
    /// @name Execution
    //@{
    /// Start an executable.
    INTAPI_BASE OKAY Run(const CHAR *argv[],
                         bool wait = false, ConsoleMode mode = NO_CONSOLE) const;
    /// Start an executable (obsolete version).
    INTAPI_BASE OKAY Exec(const CHAR *argv[], int *rc = NULL) const;
    //@}

  public:
    /// @name Disk operations.
    //@{
    /// Get free disk space available to user.
    INTAPI_BASE INT64 FreeSpace();
    //@}
  public:
    /// @name Serialization methods
    //@{
    /// Serialize the string.
    INTAPI_BASE void Serialize(Serializer &inout, const char *tag);
    //@}

  private:
    /// @name Private methods
    //@{
    /// Paths comparison (system-specific).
    INTAPI_BASE int ComparePathStr(const PathStr &s) const;
    /// Check for mask fitting.
    INTAPI_BASE OKAY CheckMask(const PathStr &mask) const;
    /// Check for UNC incorrectness.
    INTAPI_BASE bool IncorrectUNC() const;
    /// Replace all '/' and '\' by correct path separators (system-specific).
    INTAPI_BASE void CorrectPathSeparator();
    //@}
  }; // Class PathStr


//////////////////////////////////////////////////////////////////////////////
// Inline methods of Str class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method sets the string to NULL.
Str::Str()
  {
  buf = NULL;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get reference to character.
/// @note String may not be NULL, debug version asserts it.
/// @param[in] ind Index of required character;
/// must be >= 0 and <= length, debug version asserts it.
/// @return Reference to character at specified position.
char &Str::operator [](int ind)
  {
  Assert(buf != NULL);
  Assert(0 <= ind && ind <= Length());
  return buf[ind];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a character.
/// @note The string may not be NULL, debug version asserts it.
/// @param[in] ind - An index of required character;
/// must be >= 0 and <= length, debug version asserts it.
/// @return A character at the specified position.
char Str::operator [](int ind) const
  {
  Assert(buf != NULL);
  Assert(0 <= ind && ind <= Length());
  return buf[ind];
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the string.
/// @return A pointer to the buffer of the string.
Str::operator const char *() const
  {
  return buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the string.
/// @note Have problems with the method?  See XData().
/// @return A pointer to the buffer of the string.
const char *Str::Data() const
  {
  return buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get a pointer to the string - should not be used normally.
/// @note This method is dangerous and should not be used normally.
/// It is provided for interfacing with old code only.
/// It replaces the former operator char * and char *Data().
/// New name was introduced to mark the dangerous place explicitly.
/// @return A pointer to the buffer of the string.
char *Str::XData() const
  {
  return buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] s - A source string.
/// @return A reference to this string.
void Str::operator =(const Str &s)
  {
  *this = s.buf;
  }

//////////////////////////////////////////////////////////////////////////////
/// Equal.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool Str::operator ==(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return buf == s;
  return strcmp(buf, s) == 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Not equal.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool Str::operator !=(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return buf != s;
  return strcmp(buf, s) != 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Less.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison. If the lengths of the strings are
/// not equal and the string with lesser length is identical to the beginning of
/// the other string, then the longer string is greater.
bool Str::operator <(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return (s != NULL);
  return strcmp(buf, s) < 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Less or equal.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison. If the lengths of the strings are
/// not equal and the string with lesser length is identical to the beginning of
/// the other string, then the longer string is greater.
bool Str::operator <=(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return (buf == NULL ? s == NULL : false);
  return strcmp(buf, s) <= 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Greater.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison. If the lengths of the strings are
/// not equal and the string with lesser length is identical to the beginning of
/// the other string, then the longer string is greater.
bool Str::operator >(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return buf != NULL;
  return strcmp(buf, s) > 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Greater or equal.
///
/// Comparison operator.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison. If the lengths of the strings are
/// not equal and the string with lesser length is identical to the beginning of
/// the other string, then the longer string is greater.
bool Str::operator >=(const char *s) const
  {
  if (buf == NULL || s == NULL)
    return (buf == NULL ? s == NULL : true);
  return strcmp(buf, s) >= 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Case insensitive string comparison.
/// @param[in] s - A string to compare with.
/// @return
/// - value < 0 if this string < than s;
/// - value = 0 if this string is identical to s;
/// - value > 0 if this string > than s.
int Str::ICmp(const char *s) const
  {
  if (buf == NULL)
    {
    if (s == NULL)
      return 0;
    return -1;
    }
  if (s == NULL)
    return 1;

  return stricmp(buf, s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for a NULL string.
/// @return A result of the check.
bool Str::IsNull() const
  {
  return (buf == NULL);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for an empty string.
/// @return A result of the check.
bool Str::IsEmpty() const
  {
  return (buf == NULL) || (buf[0] == '\0');
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the string length.
/// @return The length of the string, in chars.
int Str::Length() const
  {
  if (IsNull())
    return 0;
  return (int)strlen(buf);
  }

//////////////////////////////////////////////////////////////////////////////
// Inline methods of UStr class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method sets the string to NULL.
UStr::UStr()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from a string.
/// @param[in] s - A source string.
UStr::UStr(const CHAR *s) : Str((const char *)s)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from an array of characters.
/// @param[in] ch - An array of characters.
/// @param[in] n - The length of the array.
UStr::UStr(const char *ch, int n) : Str(ch, n)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from Str.
/// @param[in] s - A source string.
UStr::UStr(const Str &s) : Str(s)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] s - A source object.
UStr::UStr(const UStr &s) : Str(s)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] s - A source object.
/// @return A reference to the string.
void UStr::operator =(const UStr &s)
  {
  Str::operator = (s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Concatenation and assignment operator.
/// @param[in] s - A source object.
/// @return A reference to the string.
void UStr::operator +=(const UStr &s)
  {
  Str::operator += (s);
  }

//////////////////////////////////////////////////////////////////////////////
// Inline methods of PathStr class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// The method sets the path to NULL.
PathStr::PathStr()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from a string.
/// @param[in] s - A source string.
/// @note All occurrences of '\' or '/' in @a s, if any,
/// are replaced by the proper path separator used by the system.
PathStr::PathStr(const CHAR *s) : UStr(s)
  {
  CorrectPathSeparator();
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from Str.
/// @param[in] s - A source string.
/// @note All occurrences of '\' or '/' in @a s , if any,
/// are replaced by the proper path separator used by the system.
PathStr::PathStr(const Str &s) : UStr(s)
  {
  CorrectPathSeparator();
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from UStr.
/// @param[in] s - A source string.
/// @note All occurrences of '\' or '/' in @a s, if any,
/// are replaced by the proper path separator used by the system.
PathStr::PathStr(const UStr &s) : UStr(s)
  {
  CorrectPathSeparator();
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] s - A source object.
PathStr::PathStr(const PathStr &s) : UStr(s)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] s - A source object.
/// @return A reference to the object.
PathStr &PathStr::operator =(const PathStr &s)
  {
  UStr::operator = (s);
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Equal.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator ==(const PathStr &s) const
  {
  return ComparePathStr(s) == 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Not equal.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator !=(const PathStr &s) const
  {
  return ComparePathStr(s) != 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Less.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator <(const PathStr &s) const
  {
  return ComparePathStr(s) < 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Less or equal.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator <=(const PathStr &s) const
  {
  return ComparePathStr(s) <= 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Greater.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator >(const PathStr &s) const
  {
  return ComparePathStr(s) > 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Greater or equal.
///
/// Comparison operator.
/// @note This operator is OS dependent, i.e. it takes into account
/// case sensitivity of filenames.
/// @note Any strings, including NULL ones, are compared.
/// A NULL string is considered to be less than any other string.
/// @param[in] s - A string to compare with.
/// @return A result of the comparison.
bool PathStr::operator >=(const PathStr &s) const
  {
  return ComparePathStr(s) >= 0;
  }

INTEGRA_NAMESPACE_END
#endif
