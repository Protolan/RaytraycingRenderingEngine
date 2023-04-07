/// @file
///
/// @brief Declaration of AnnInFile class.
///
/// Copyright &copy; INTEGRA, Inc., 2001-2004.
///
/// @internal
/// @author Ose - Sergey Ershov '01.12.10

#ifndef _KLBS_ANNINFILE_HPP_
#define _KLBS_ANNINFILE_HPP_

#include "arrays.hpp"
#include "export.h"
#include "file.hpp"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Reads annotated text files, with comments and keywords.
class AnnInFile : public InFile
  {
  public:
    /// @name Enumerations
    //@{
    /// Stop criteria for array reading.
    enum StopToken
      {
      /// Stop at @b enddef.
      STOPAT_ENDDEF = 0x10000000,
      /// Stop at the end of the current line.
      STOPAT_ENDLINE = 0x01000000
      };
    /// Wrap mode.
    enum WrapModeEnum
      {
      /// An expression must start and end on the current line.
      WRAP_NONE = 0,
      /// An expression may begin on the next line or occupy several
      /// lines, but each "element" of an array must be on one line.
      WRAP_ENDLINE = 1,
      /// Elements may be split across lines (for arrays
      /// of bracketed expressions only).
      WRAP_ELEMENT = 2
      };
    //@}

  public:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    inline AnnInFile(const PathStr &pathname);
    /// Copy constructor.
    inline AnnInFile(const AnnInFile &src_file);
    /// Destructor.
    inline ~AnnInFile();
    //@}

  public:
    /// @name Reading modes, errors
    //@{
    /// Get the last error.
    inline const Str &LastError() const;
    /// Set the case sensitivity.
    inline void CaseSensitivity(bool mode);
    /// Set the wrapping mode.
    inline void WrapMode(bool wrap);
    //@}

  public:
    /// @name Reading of scalar values
    //@{
    /// Check the next keyword.
    INTAPI_BASE OKAY Read(const char *keyword);
    /// Read a text value.
    INTAPI_BASE StreamParseErr Read(Str &val, bool wrap = true);
    /// Read a bool value.
    INTAPI_BASE StreamParseErr Read(bool &val, bool wrap = true);
    /// Read an integer value.
    INTAPI_BASE StreamParseErr Read(int &val, bool wrap = true);
    /// Read a double value.
    INTAPI_BASE StreamParseErr Read(double &val, bool wrap = true);
    /// Read a bracketed text.
    INTAPI_BASE StreamParseErr Read(Str &val, const char *left,
                                   const char *right,
                                   WrapModeEnum wrap = WRAP_ENDLINE);
    /// Read a Str value preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            Str &val, bool wrap = true);
    /// Read a bool value preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            bool &val, bool wrap = true);
    /// Read an integer value preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            int &val, bool wrap = true);
    /// Read a double value preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            double &val, bool wrap = true);
    /// Read a bracketed text preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            Str &val, const char *left, const char *right,
                            WrapModeEnum wrap = WRAP_ENDLINE);
    //@}

  public:
    /// @name Reading of linear arrays
    //@{
    /// Read an array of text values.
    INTAPI_BASE StreamParseErr Read(TArray<Str> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of bool values.
    INTAPI_BASE StreamParseErr Read(TArray<bool> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of integer values.
    INTAPI_BASE StreamParseErr Read(TArray<int> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of double values.
    INTAPI_BASE StreamParseErr Read(TArray<double> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of float values.
    INTAPI_BASE StreamParseErr Read(TArray<float> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of bracketed text values.
    INTAPI_BASE StreamParseErr Read(TArray<Str> &array,
                            const char *left, const char *right,
                            int stopat = STOPAT_ENDDEF,
                            WrapModeEnum wrap = WRAP_ENDLINE);
    /// Read an array of text values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<Str> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of bool values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<bool> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of integer values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<int> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of double values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<double> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of float values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<float> &array,
                            int stopat = STOPAT_ENDDEF);
    /// Read an array of bracketed text values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword, TArray<Str> &array,
                            const char *left, const char *right,
                            int stopat = STOPAT_ENDDEF,
                            WrapModeEnum wrap = WRAP_ENDLINE);
    //@}

  public:
    /// @name Reading of tables - two-dimensional arrays
    //@{
    /// Read a table of text values.
    INTAPI_BASE StreamParseErr Read(TArray<TArray<Str> > &table);
    /// Read a table of bool values.
    INTAPI_BASE StreamParseErr Read(TArray<TArray<bool> > &table);
    /// Read a table of integer values.
    INTAPI_BASE StreamParseErr Read(TArray<TArray<int> > &table);
    /// Read a table of double values.
    INTAPI_BASE StreamParseErr Read(TArray<TArray<double> > &table);
    /// Read a table of bracketed text values.
    INTAPI_BASE StreamParseErr Read(TArray<TArray<Str> > &table,
                            const char *leftbrace, const char *rightbrace);
    /// Read a table of text values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            TArray<TArray<Str> > &table);
    /// Read a table of bool values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            TArray<TArray<bool> > &table);
    /// Read a table of integer values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            TArray<TArray<int> > &table);
    /// Read a table of double values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            TArray<TArray<double> > &table);
    /// Read a table of bracketed text values preceded with a keyword.
    INTAPI_BASE StreamParseErr Read(const char *keyword,
                            TArray<TArray<Str> > &table,
                            const char *leftbrace, const char *rightbrace);
    //@}

    /// Skip text in file to the specified keyword.
    INTAPI_BASE OKAY SkipToKeyword(const Str &keyword);

  protected:
    /// Read current line.
    INTAPI_BASE virtual OKAY ReadCurLine();
    /// Check for permission to read the next line.
    inline bool CanTakeNextLine() const;
    /// Set the last error.
    INTAPI_BASE StreamParseErr SetLastError(int code);

  private:
    /// Last error description.
    Str  lasterr;
    /// Flag allowing to take next line when current one ended.
    bool multiline;
    /// Case sensitivity mode for keywords.
    bool respect_case;
  }; // end class AnnInFile


//////////////////////////////////////////////////////////////////////////////
// Inline methods of AnnInFile class

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method only sets a full path to the file.
/// @param[in] pathname Name of the input file including path.
AnnInFile::AnnInFile(const PathStr &pathname)
  : InFile(pathname), respect_case(false)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The file may not be opened; debug version asserts it.
/// @param[in] src_file A source object.
AnnInFile::AnnInFile(const AnnInFile &src_file)
  : InFile(src_file), respect_case(src_file.respect_case)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// Closes the file if it was opened.
AnnInFile::~AnnInFile()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get the last error.
/// @return A textual description of the error which the reading stopped at,
/// in the form: <b>"Missed enddef at line 55 col 6 file f:\myfile.dat"</b>.
const Str &AnnInFile::LastError() const
  {
  return lasterr;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the case sensitivity.
///
/// The method determines whether the comparison of keywords with the file
/// text is case sensitive.
/// This only concerns keywords, i.e. "names of variables" and @b enddef
/// but not representations of bool values.
/// @param[in] mode @b true if the comparison must be case sensitive.
void AnnInFile::CaseSensitivity(bool mode)
  {
  respect_case = mode;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the wrapping mode.
/// @param[in] wrap @b true if multiline entities should be supported.
void AnnInFile::WrapMode(bool wrap)
  {
  multiline = wrap;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for permission to read a next line.
///
/// The method tells if the wrap mode allows reading of the next line (or
/// the reading is allowed within the current line only).
///
/// Reading of the next line is allowed when either even the first line has not
/// yet been read from the file (LineNo() == 0) or when wrap mode is true (this mode is set by WrapMode()
/// directly or by all reading methods indirectly according to their
/// parameter ("wrap") mode).
/// @return @b true if reading of next line is allowed.
bool AnnInFile::CanTakeNextLine() const
  {
  return multiline || LineNo() == 0;
  }


INTEGRA_NAMESPACE_END
#endif  // _KLBS_FILE_HPP_
