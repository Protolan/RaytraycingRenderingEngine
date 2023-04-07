/// @internal
/// @file
///
/// @brief Definition of Config class.
///
/// @author Eed - Eugene Denisov, '15.07.08
///
/// Copyright &copy; INTEGRA, Inc., 2015

#include <integra.hpp>
#include <base/arrays.hpp>
#include <base/dict.hpp>
#include <base/file.hpp>
#include <base/marray.hpp>
#include <base/str.hpp>
#include "config.hpp"

INTEGRA_NAMESPACE_START

/**
@class Config base/config.hpp

Class for reading/writing of configuration file (INI file).
This file has standard Windows's format.

Class reads file at once (in constructor) and all further operations are
performed on data stored in memory. Actual update (writing) of file
is performed either explicitly by Write() method, or implicitly in destructor.
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Content of specified file, if it exists, is read immediately.
/// @param[in] fn Filename of config file.
Config::Config(const PathStr &fn)
  : filename(fn)
  {
  Read();
  cur_group = NULL;
  updated = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor (forbidden).
///
/// Issues unconditional assertion.
/// @param[in] src Source object.
Config::Config(const Config &src)
  {
  Assert(false);
  cur_group = NULL;
  updated = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// File will be written, if it was not yet written explicitly.
Config::~Config()
  {
  if (updated)
    Write();
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the current group.
///
/// All further reading/writing will be performed from/to this group.
/// @param[in] gname Group name.
void Config::SetGroup(const CHAR *gname)
  {
  cur_group = gname;
  if (!groups.Find(cur_group))
    {
    ConfigGroup cg;
    groups.Enter(gname, cg);
    updated = true;
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove all entries in the current group.
void Config::ClearGroup()
  {
  if (cur_group == NULL)
    {
    Debug("Config: no group is set");
    return;
    }
  groups.Remove(cur_group);
  updated = true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Remove entry under given key from the current group.
/// @param[in] key Name of entry to be removed.
void Config::RemoveEntry(const CHAR *key)
  {
  if (cur_group == NULL)
    {
    Debug("Config: no group is set");
    return;
    }
  groups[cur_group].Remove(key);
  updated = true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write string entry to the current group.
/// @param[in] key Name of entry to be written.
/// @param[in] value Value to be written.
void Config::WriteEntry(const CHAR *key, const CHAR *value)
  {
  if (cur_group == NULL)
    {
    Debug("Config: no group is set");
    return;
    }
  groups[cur_group].Enter(key, value);
  updated = true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Write integer entry to the current group.
/// @param[in] key Name of entry to be written.
/// @param[in] num Value to be written.
void Config::WriteEntry(const CHAR *key, int num)
  {
  UStr s;
  s.Printf("%ld", num);
  WriteEntry(key, s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Write double entry to the current group.
/// @param[in] key Name of entry to be written.
/// @param[in] num Value to be written.
void Config::WriteEntry(const CHAR *key, double num)
  {
  UStr s;
  s.Printf("%g", num);
  WriteEntry(key, s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Write boolean entry to the current group.
/// @param[in] key Name of entry to be written.
/// @param[in] b Value to be written.
void Config::WriteEntry(const CHAR *key, bool b)
  {
  UStr s(b ? "1" : "0");
  WriteEntry(key, s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Write list entry to the current group.
/// @param[in] key Name of entry to be written.
/// @param[in] lst List of values to be written.
/// @param[in] sep Character to be used for separation of values.
void Config::WriteEntry(const CHAR *key, const TArray<UStr> &lst, const CHAR &sep)
  {
  UStr s, sp;
  sp.Allocate(1);
  sp[0] = sep;
  for (int i = 0; i < lst.Length(); ++i)
    s += lst[i] + sp;
  WriteEntry(key, s);
  }

//////////////////////////////////////////////////////////////////////////////
/// Read string entry from the current group.
/// @param[in] key Name of entry to be read.
/// @param[in] deflt Default value to be returned if no such entry.
/// @return Value read from file, or default value, if file does not contain such entry.
UStr Config::ReadEntry(const CHAR *key, const UStr &deflt)
  {
  if (cur_group == NULL)
    return deflt;

  ConfigGroup *grp = groups.FindValue(cur_group);
  if (grp == NULL)
    return deflt;
  UStr *s = grp->FindValue(key);
  if (s == NULL)
    return deflt;
  return *s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read integer entry from the current group.
/// @param[in] key Name of entry to be read.
/// @param[in] deflt Default value to be returned if no such entry.
/// @return Value read from file, or default value, if file does not contain such entry.
int Config::ReadNumEntry(const CHAR *key, int deflt)
  {
  bool ok;
  int ret = ReadEntry(key).IntegerNum(ok);
  if (ok)
    return ret;
  return deflt;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read double entry from the current group.
/// @param[in] key Name of entry to be read.
/// @param[in] deflt Default value to be returned if no such entry.
/// @return Value read from file, or default value, if file does not contain such entry.
double Config::ReadDoubleEntry(const CHAR *key, double deflt)
  {
  bool ok;
  double ret = ReadEntry(key).DoubleNum(ok);
  if (ok)
    return ret;
  return deflt;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read boolean entry from the current group.
/// @param[in] key Name of entry to be read.
/// @param[in] deflt Default value to be returned if no such entry.
/// @return Value read from file, or default value, if file does not contain such entry.
bool Config::ReadBoolEntry(const CHAR *key, bool deflt)
  {
  return ReadNumEntry(key, deflt ? 1 : 0) != 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read list entry from the current group.
/// @param[in] key Name of entry to be read.
/// @param[in] sep Character to be used for separation of values.
/// @return List of read values.
TArray<UStr> Config::ReadListEntry(const CHAR *key, const CHAR &sep)
  {
  TArray<UStr> res;
  UStr s = ReadEntry(key);
  if (s.IsEmpty())
    return res;
  int pos;
  while (!s.IsEmpty() && s.FindFirst(sep, &pos))
    {
    UStr sub = s.SubStr(0, pos);
    s = s.SubStr(pos + 1, s.Length() - pos - 1);
    if (!sub.IsEmpty())
      res.Add(sub);
    }
  return res;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read list of paths entry from the predefined group.
///
/// Method reads specified entry from "Libraries" group; only existing
/// paths are presented among returned ones.
/// @param[in] lib_name Library name.
/// @param[out] out_libs Library paths.
void Config::GetLibrary(const CHAR *lib_name, TArray<PathStr> &out_libs)
  {
  out_libs.Truncate();
  UStr old_group = cur_group;
  cur_group = "Libraries";
  TArray<UStr> res = ReadListEntry(lib_name, '|');
  cur_group = old_group;
  for (int i = 0; i < res.Length(); i++)
    {
    PathStr path(res[i]);
    if (path.IsDir())
      out_libs.Add(path);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Write file.
/// @param[in] fn Name of file; will replace the one specified in constructor.
void Config::Write(const PathStr &fn)
  {
  if (!fn.IsEmpty())
    filename = fn;

  File f(filename, User());
  if (f.Open("w") != SUCCESS)
    return;

  // Sort groups alphabetically
  int i, j;
  UStr tmp;
  MArray<UStr> gr, entr;
  for (i = 0; i < groups.Length(); i++)
    gr.Add(groups.GetKey(i));
  gr.QSort();
  for (i = 0; i < gr.Length(); i++)
    {
    f.Printf("[%s]\n", gr[i].Data());
    // Sort entries alphabetically
    entr.Truncate();
    for (j = 0; j < groups[gr[i]].Length(); j++)
      entr.Add(groups[gr[i]].GetKey(j));
    entr.QSort();
    for (j = 0; j < entr.Length(); j++)
      {
      tmp = groups[gr[i]][entr[j]];
      f.Printf("%s = %s\n", entr[j].Data(), tmp.IsEmpty() ? "" : tmp.Data());
      }
    }
  f.Close();
  updated = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Read config file.
void Config::Read()
  {
  if (!filename.IsFile())
    return;

  File f(filename, User());
  if (f.Open("r") != SUCCESS)
    return;

  UStr line;
  while (f.ReadStr(line) == SUCCESS)
    Parse(line);

  f.Close();
  }

//////////////////////////////////////////////////////////////////////////////
/// Process and store one line.
/// @param[in] l Line to process.
void Config::Parse(const UStr &l)
  {
  UStr line = l;
  line.Clip();
  if (line[0] == '[')
    {
    UStr gname = line;
    gname.Exclude(0, 1);
    if (gname.Length() > 1 && gname[gname.Length() - 1] == ']')
      {
      gname.Exclude(gname.Length() - 1, 1);
      cur_group = gname;
      return;
      }
    Debug("Config: line `%s' - bad syntax of group name", line.Data());
    return;
    }

  if (cur_group.IsEmpty())
    {
    Debug("Config: line `%s' is out of group", line.Data());
    return;
    }

  int pos;
  UStr key, value;
  if (!line.FindFirst('=', &pos))
    {
    Debug("Config: corrupted line `%s' in group `%s'", line.Data(), cur_group.Data());
    return;
    }
  key = line.SubStr(0, pos);
  value = line.SubStr(pos + 1, line.Length() - pos - 1);
  key.Clip();
  value.Clip();
  groups[cur_group].Enter(key, value);
  }

INTEGRA_NAMESPACE_END
