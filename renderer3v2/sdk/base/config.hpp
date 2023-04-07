/// @file
///
/// @brief Declaration of Config class.
///
/// Copyright &copy; INTEGRA, Inc., 2015
///
/// @internal
/// @author Eed - Eugene Denisov, '15.07.08

#ifndef _KLBC_CONFIG_HPP_
#define _KLBC_CONFIG_HPP_

#include <base/arrays.hpp>
#include <base/dict.hpp>
#include <base/str.hpp>

INTEGRA_NAMESPACE_START

/// Class for reading/writing of INI file (configuration file).
class Config
  {
  public:
    /// Default constructor.
    INTAPI_BASE Config(const PathStr &fn);
    /// Destructor.
    INTAPI_BASE ~Config();

    /// Set the current group.
    INTAPI_BASE void SetGroup(const CHAR *gname);
    /// Remove all entries in the current group.
    INTAPI_BASE void ClearGroup();
    /// Remove entry under given key from the current group.
    INTAPI_BASE void RemoveEntry(const CHAR *key);

    /// Write string entry to the current group.
    INTAPI_BASE void WriteEntry(const CHAR *key, const CHAR *value);
    /// Write integer entry to the current group.
    INTAPI_BASE void WriteEntry(const CHAR *key, int num);
    /// Write double entry to the current group.
    INTAPI_BASE void WriteEntry(const CHAR *key, double num);
    /// Write boolean entry to the current group.
    INTAPI_BASE void WriteEntry(const CHAR *key, bool b);
    /// Write list entry to the current group.
    INTAPI_BASE void WriteEntry(const CHAR *key, const TArray<UStr> &lst, const CHAR &sep);

    /// Read string entry from the current group.
    INTAPI_BASE UStr ReadEntry(const CHAR *key, const UStr &deflt = NULL);
    /// Read integer entry from the current group.
    INTAPI_BASE int ReadNumEntry(const CHAR *key, int deflt = -1);
    /// Read double entry from the current group.
    INTAPI_BASE double ReadDoubleEntry(const CHAR *key, double deflt = 0);
    /// Read boolean entry from the current group.
    INTAPI_BASE bool ReadBoolEntry(const CHAR *key, bool deflt = false);
    /// Read list of strings entry from the current group.
    INTAPI_BASE TArray<UStr> ReadListEntry(const CHAR *key, const CHAR &sep);
    /// Read list of paths entry from the predefined group.
    INTAPI_BASE void GetLibrary(const CHAR *lib_name, TArray<PathStr> &out_libs);

    /// Write file.
    INTAPI_BASE void Write(const PathStr &fn = NULL);

  protected:
    /// Copy constructor (forbidden).
    INTAPI_BASE Config(const Config &src);

  private:
    /// Read config file.
    INTAPI_BASE void Read();
    /// Parse line.
    INTAPI_BASE void Parse(const UStr &line);

  private:
    /// Path of config file.
    PathStr filename;
    /// Flag indicating updated config (necessity to write file).
    bool updated;

    /// Type for group.
    typedef TDict<UStr, UStr> ConfigGroup;
    /// Set of groups.
    TDict<UStr, ConfigGroup> groups;
    /// Current group.
    UStr cur_group;
  };

INTEGRA_NAMESPACE_END

#endif
