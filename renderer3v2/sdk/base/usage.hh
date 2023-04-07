/// @internal
/// @file
///
/// @brief Base component - How To Use page.
///
/// @author Eed - Eugene Denisov, '04.11.16.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

//////////////////////////////////////////////////////////////////////////////
/**
@page base_how_to_use How To Use

<HR>
- @ref base_strings_usage 
- @ref base_time_usage 
- @ref base_stack_usage 
- @ref base_array_usage 
- @ref base_table_usage 
- @ref base_envi_usage 
- @ref base_plugin_usage 

<HR>

@section base_strings_usage How to Use Strings

To deal with the UStr class, an application developer must always convert 
string and character literals to the (CHAR *) and CHAR types, respectively. 
The INTEGRA_T() macro must be used for this, like in:

@code
INTEGRA_T("Hello, world!\n")
INTEGRA_T('A')
@endcode

This class automatically allocates the proper amount of space for the string.
This is very handy in the case of internationalization / localization. 

Methods allow access to the buffer of a string. Examples: 

@code
USrt str1 = UStr(INTEGRA_T("Hello, world!\n"));
CHAR c = INTEGRA_T('B');

str1[2] = c;
c = str1[12];
@endcode

The string str1 is initialized with the string "Hello, world!\n"; the third 
character in the string  str1 is replaced by 'B'; c is changed to '!'.

Methods and operators provide concatenation and substring operations,
character searching, case conversion, comparison, truncation and formatted
printing.

Examples: 

@code
USrt str1 = UStr(INTEGRA_T("Hello, world!\n"));
UStr s1, s2;

s1 = str1.SubStr(5, 8);
s1 = INTEGRA_T("Hi") + s1;

int pos;
if (s1.FindLast(INTEGRA_T('\n'), &pos))
  s1.Exclude(pos, 1);

s2.Printf(INTEGRA_T("%s\n", s1.Data());
@endcode

The class PathStr is aimed to deal with file names and paths.
It inherits the representation and all the methods of the Str class adding 
own specific methods.

Constructors of PathStr allow initialization of a pathname either by full string
presenting a full path or by specific parts of a full path
(a path, a filename, an extension).

Examples:

@code
Str full_path;
...
...

PathSrt path1(full_path);
PathSrt path2(INTEGRA_T("dir"), INTEGRA_T("file.ext"));
PathSrt path3(INTEGRA_T("dir1"), INTEGRA_T("file"), INTEGRA_T("ext"));
@endcode

Methods allow getting of a path, a filename with extension, a basename of a file
(without the extension) and an extension from a full pathname of a PathStr object;
the method PathStr::Split() splits a full pathname to the listed above parts.

Examples:

@code
PathSrt full_path;
...

Str path = full_path.Path(), basename = full_path.BaseName();
Str ext = full_path.Extension();

Str path1, filename, ext1;

// filename will contain a file
// name with an extension
full_path.Split(&path1, &filename);

// filename will contain a file
// name without an extension
full_path.Split(&path1, &filename, &ext1);
@endcode

The method Tr() performs translation of user strings to another language.
Translation function must be implemented by application and registered
using method UStr::SetTr().

Examples:

@code
static UStr TrFunc(const char *context, const char *str, const char *comment);

int main(int argc, char **argv)
  {
  ...
  UStr::SetTr(TrFunc);
  ShowMessage(Tr("Hello, world!"));
  ...
  }

UStr TrFunc(const char *context, const char *str, const char *comment)
  {
  // Translate string using Qt
  return (UStr)(qApp->translate(context, str, comment).local8Bit());
  }
@endcode

@section base_time_usage How to Use Time and Timers

The class Timer can be used for calculation in milliseconds of the length
of a period between two fixed points of the time. The class provides the
default constructor and the copy constructor. The other methods are:
Timer::Reset() (set current time),
Timer::Elapsed() (get time after last timer reset),
Timer::Wait() (wait for specified time).

The class Time can be used for registration of time in seconds
since 00:00:00 1970.01.01 and for presentation of local time in the
form: year, month, day, hours, minutes and seconds.
The method Time::Get() returns the current time in seconds
since 00:00:00 1970.01.01.
Constructors allow initialization by any time, by the current time
(returned by the method Time::Get()) and by separate components of the local time.

Example:

@code
unsigned t = Time::Get();
Time cur_time(t);
Time lt1(1990, 11, 23), lt2(1999, 5, 4, 15, 55);
@endcode

The methods Time::DateStamp() and Time::TimeStamp() return date/time in
a printable form, of the Str type.

Example:

@code
Time cur_time; // cur_time is initiated by
               // default (by Time::Get())

int cur_year = cur_time.year;
int cur_month = cur_time.month;
...
int cur_hour = cur_time.hour;
...

printf("The date is %s\n", (CHAR *)cur_time.DateStamp());
printf("The time is %s\n", (CHAR *)cur_time.TimeStamp());
@endcode

@section base_stack_usage How to Use Stacks

The template class TStack provides objects keeping in a LIFO stack of elements
of a generic type T. The number of elements in a TStack is variable.
The constructor and destructor of type T are called to create and destroy stack
elements.

Examples:

@code
typedef TStack<int> IntStack;
IntStack i1;

struct MyStruct
  {
  ....
  };

TStack<MyStruct> m1;
@endcode

The stack i1 with elements of the int type and the stack m1 with elements of 
the type MyStruct are created with default initialization.

The methods allow a programmer to push, pop, and exclude elements. 

Examples:

@code
TStack<MyStruct> m1;

...

MyStruct a1;

m1.Push(a1);   // push into the stack

...

while (!m1.IsEmpty())
  {
  MyStruct t1 = m1.Top();   // get top item
  m1.Pop(); // pop the stack 

  }
@endcode

@section base_array_usage How to use Arrays

The template class TArray provides ordered sets (vectors, lists) of elements 
of a generic type T.
The number of elements in TArray is variable.
The constructor and destructor of the type T are called to create and destroy
array elements.

Examples:

@code
typedef TArray<int> IntArray;
IntArray m1;

struct MyStruct
  {
  ....
  };

TArray<MyStruct> a1;
@endcode

The arrays m1 with elements of the int type and a1 with elements of the type 
MyStruct are created with default initialization.

Methods allow a programmer to add elements, put an element to a specified position,
exclude elements, find an element and copy arrays. 

Operator[] can be used to access an element in some position of array. 

Examples:

@code
TArray<MyStruct> a1;
MyStruct ms;
...
ms = a1[3]; //to get element from the 3rd position

a1.Remove(2); // remove element from the 2nd position
...
a1[2] = ms; // change element

// insert one element to 10 position
// even if the size of the array < 10
a1.Insert(&ms, 10, 1); 

// add the element ms if it is absent
if (!a1.Find(ms))
  a1.Add(ms);
@endcode

@section base_table_usage How to Use Tables

The template class TTable allows an easy implementation of voxelization.
Let there is a set of the triangles and a set of the voxels (rectangles in
the 2D case or rectangle parallelepipeds in the 3D case).
The following code provides a method for the getting lists of the triangles
which intersect specific voxels. 

Example:

@code
TTable<int> table(number_of_voxels);

for (int i = 0; i < number_of_triangles; i++)
  for (int j = 0; j < number_of_voxels; j++)
    if (i-th triangle intersected with j-th voxel)
     {
     (void)table.Insert(j, i);
     }
...
// The list of the triangles which intersect the n-th voxel
TArray<int> &list_trgs = table.GetList(n);

// Alternative access to the list of the triangles in a voxel
TArray<int> list_trgs;
table.GetList(n, list_trgs);

// Delete the k-th triangle in the n-th voxel
(void)Remove(n, k);
@endcode

@section base_envi_usage How to Use Environment Variables

The methods Envi::GetEnv() and Envi::PutEnv() provide
access to environment variables.

@code
// Example to set a default environment variable value

  {
   ...
  Str var_name = "WIDTH";
  Str default_width = "1024";

  if (Envi::GetEnv(var_name) == "")
    Envi::PutEnv(var_name, default_width);
   ...
  }
@endcode

@section base_plugin_usage How to Use Plugins

- @ref base_plugin_types
- @ref base_plugin_implicit
- @ref base_plugin_explicit
- @ref base_plugin_reqs
- @ref base_plugin_use

@subsection base_plugin_types Types of Linking
There are two types of DLL (plugin) loading (linking) - implicit and
explicit one. Implicit linking is sometimes referred to as static loading
or load-time dynamic linking. Explicit linking is sometimes referred to as
dynamic loading or run-time dynamic linking.

With implicit linking, the executable using the DLL links to an
import library (.lib file) provided by the maker of the DLL.
The operating system loads the DLL when the executable using it is loaded.
The client executable calls the DLL's exported functions just as if
the functions were contained within the executable. 

With explicit linking, the executable using the DLL must make function
calls to explicitly load the DLL, and to access the
DLL's exported functions. The client executable must call the
exported functions through a function pointer.

The same DLL can be used with either linking function. Furthermore,
these mechanisms are not mutually exclusive, as one executable can
implicitly link to a DLL and another can attach to it explicitly.

One DLL also can explicitly or implicitly link another one. In case of
implicit linking, operating system loads the second DLL before the first one.
In case of explicit linking, initialization routine of the first DLL has
to bother about loading of the second DLL.

@subsection base_plugin_implicit Implicit Linking

For implicit linking, a plugin (DLL) must provide the following:
  
- an external header(s) (.hpp files) containing the declarations
of the exported functions and/or C++ classes.
- an import library (a .lib file) to link with. The linker creates
the import library when the DLL is built.
- the actual DLL itself (the .dll file).

Modules using the DLL must include the header file containing
the exported functions (or C++ classes) in each source file that
contains calls to the exported functions. From a coding perspective,
the function calls to the exported functions are just like any other
function call.

To build the calling executable file, you must link with the import library.

The operating system must be able to locate the .dll file when
it loads the calling module. It searches DLL in the following
directories, in exactly this order:

-# The directory where the executable module for
   the current process is located.
-# The current directory.
-# The Windows system directory (usually it is C:\\Windows\\System32).
-# The Windows 16-bit system directory (usually it is C:\\Windows\\System).
-# The Windows directory (usually it is C:\\Windows).
-# The directories listed in the PATH environment variable.

@subsection base_plugin_explicit Explicit linking

With explicit linking, a module must make a function call to
explicitly load the DLL at run time. To explicitly link to a DLL,
the module must call LoadPlugin(). It is assumed, that
the plugin somehow registers its functions (e.g. in the System Registry)
during its InitClasses() execution in order a loader could later
use functions exported by this plugin.

For a directory where DLL is loaded from, see description of LoadPlugin().

@subsection base_plugin_reqs Requirements to plugins

For declaration of exported functions and/or classes in external
header(s) use the following approach:

 - in project settings of a DLL project define macro @b INTEXPORT_<component_name>.
   The name of the macro must be unique throughout the whole INTEGRA repository.
 - in the component folder create an internal header file <B>%export.h</B>
   with the following contents:
   @code
   #ifdef INTEXPORT_<component_name>
   #define INTAPI_<component_name> __declspec(dllexport)
   #else
   #define INTAPI_<component_name> __declspec(dllimport)
   #endif
   @endcode
 - Use macro @b INTAPI_<component_name> for declaration of exported functions:
   @code
   INTAPI_<component_name> int SomeFunction(...);
   @endcode
   Always place @b INTAPI_<component_name> as the very first type modifier -
   this allows an easy recognition of exported functions.

Each plugin, besides own exported functions must provide the exported
InitClasses() and TermClasses() global functions.
InitClasses() must do the full initialization of the
plugin, including creation of types supplied by this plugin, creation of
objects if needed, registering the types and objects in the system
registry, etc.
TermClasses() must do correct termination of the plugin -
deletion of objects, created by InitClasses() and during work of the plugin,
releasing of allocated resources, etc.

@code
#include <base/plugins.hpp>

DLLEXPORT OKAY InitClasses()
  {
  ...
  }

DLLEXPORT void TermClasses()
  {
  ...
  }
@endcode

@subsection base_plugin_use Usage of Plugins

In case of implicit linking, a module must include exported header(s)
of the plugin and use its functions as usually. An executable has to be
linked with the import library of the corresponding plugin.

In case of explicit linking, a plugin is loaded by call of LoadPlugin():
@code
  ...
  if (LoadPlugin("plugin1") != SUCCESS)
    User()->ErrorMessage("Unable to load plugin1");
  ...
@endcode

It is possible to call LoadPlugin() inside InitClasses()
(e.g. if one plugin depends on another one).

Further usage of an explicitly linked plugin depends on functionality
provided by the plugin.

In order to use plugin related functions, the BASE plugin (base.dll)
must be implicitly linked by the application. This is achieved by linking
the application with the corresponding import library (base.lib).

Each plugin must be linked with the BASE import library (base.lib) and with
plugin.obj (which contains code necessary for correct plugin initialization).

**/
