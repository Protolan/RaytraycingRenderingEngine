/// @internal
/// @file
///
/// @brief Base component - Main page and Functionality page.
///
/// @author Eed - Eugene Denisov, '04.11.16.
///
/// Copyright &copy; INTEGRA, Inc., 2004.

//////////////////////////////////////////////////////////////////////////////
/**
@page base_mainpage Base Documentation

The base classes provide platform-independent
low-level commonly used services to deal with strings, path names, files,
time, simple arrays and dictionaries, matrices.

<HR>

- @subpage base_functionality
- @subpage base_how_to_use
@internal
- @subpage base_design
- @subpage base_works
- @subpage base_test
**/

//////////////////////////////////////////////////////////////////////////////
/**
@page base_functionality Overview of Functionality

<HR>
- @ref base_overview_general 
- @ref base_overview_strings
- @ref base_overview_arrays
- @ref base_overview_matrices
- @ref base_overview_files
- @ref base_overview_time
- @ref base_overview_stacks
- @ref base_overview_tables
- @ref base_overview_streams
- @ref base_overview_user
- @ref base_overview_serializer
- @ref base_overview_plugins
- @ref base_overview_sync
- @ref base_overview_guid
<HR>

@section base_overview_general General System Support

The header file base.hpp provides the most general commonly used
features such as the Swap(), Min(), Min3(), Max(), Max3() functions.

The class Envi contains methods for obtaining size of various
types of memory available to an application: Envi::GetMemSize(),
Envi::AvailMemBlockSize(), Envi::Bytes2MBytes().
Also there are methods for access to environment variables:
Envi::GetEnv() and Envi::PutEnv().

See @ref base_envi_usage.

@section base_overview_strings Strings and Pathnames

Whenever text strings are necessary, the classes Str and UStr can be used.

The class Str is aimed to deal with text strings in a useful way if efficiency
is not very important. Elements of the text strings are of the 'char' type 
and contain text in plain English. The class may not be used for strings
participating in the user interface - use UStr class instead.

The class UStr is aimed to use for all the strings participating in the user 
interface. Currently the class is essentially identical to (inherited from) 
the Str class based on the char type but the underlying type may be changed 
in future to, say, wchar_t (16 bit UNICODE). Therefore the application
programmer must make no assumptions about particular representation of string 
elements, including memory size occupied by it.

The class PathStr deals with file names and paths.

See @ref base_strings_usage.

@section base_overview_arrays Arrays and Dictionaries

The simplest implementation of arrays (indexed sets of arbitrary
number of elements) and dictionaries (sets of key-value pairs) are
provided by the template classes TArray and TDict, respectively.
The application developer may instantiate own instances of the template 
classes with certain base types (the element type for arrays; the key 
and value types for dictionaries).

The GArray class provides additional functionality (the Find() method and checking
for array equality) if the base type supports "operator ==()".

The TBArray, BArray, SArray, Array template classes provide different types
of arrays. The class IArray (base for array classes) performs memory allocation
control using the MemoryClass functionality.

See @ref base_array_usage.

@section base_overview_matrices Matrices

The simplest implementation of matrices (2D arrays of elements) is provided
by the template class TMatrix. The application developer may instantiate own 
instances of the template class with given base types. The template class 
TMatrix3D provides three-dimensional matrices.

@section base_overview_files Files

The File objects are provided to deal with files, both text and binary ones.
You create a File object specifying a correspondent path name (of the PathStr type),
and then deal with it using methods for opening, closing, reading, writing etc.

The class InFile can be used for the token by token input. The class AnnInFile 
supports reading of annotated text file, with comments and keywords. The class 
OutFile outputs text files with ability to specify a left margin (an indent).

@section base_overview_time Time

Use the Time class to obtain the current time in seconds from 1970 and to
convert it into ordinary year, month, day, hours, minutes, seconds.

Use Timer objects to measure time intervals in milliseconds.

See @ref base_time_usage.

@section base_overview_stacks Stacks

The simplest implementation of a LIFO stack (Last Input - First Output)
is provided by the template class TStack. The application developer may 
instantiate own instances of the template class with given
base types (the types of stack elements).

See @ref base_stack_usage.

@section base_overview_tables Tables

The template class TTable provides methods for realization of an alternative
to arrays of the pointers to arrays with changeable lengths. 
The GTable class provides additional functionality (the Find() method and table
equality) if the base type supports "operator ==()".

See @ref base_table_usage.

@section base_overview_streams Streams

Here streams are per-byte input / output from / to some media.
The streams may be useful to process binary files with strict structure
organization. They are used by Integra Object Framework file loader / saver.

The streams are based on the abstract ByteStream class. Particular implementations
are provided for files (ReadFileStream and WriteFileStream) and memory
(MemStream, ReadMemStream, WriteMemStream).

@section base_overview_user Abstract IUser %Interface 

The IUser abstract class is used to provide UI-independent dialogue with the
end user. Each application program based on Integra SDK should provide a customized
implementation of this class, and call the global function SetUser()
which registers a new default User object of the application.
The User() function returns the current User object of the application.

For tests and simple batch mode applications, a simple batch mode -
oriented implementation of the IUser interface is available as the default
object of the BatchUser class.

@section base_overview_serializer Abstract Class Serializer

Serializer is an abstract class that is a base for all Integra serializers.
Serializers allow for Integra Object Framework objects and object hierarchies
to read / write their data (internal state) from or to any external media.
See System component for details.

@section base_overview_plugins Methods Dealing with Plugins

Any application based on Integra SDK, which works with plugins must correctly
initialize (at the beginning of the work) and terminate (after the work,
before exit) the <b> IOF - Integra Object Framework</b>.
The global functions InitFrameWork() and TermFrameWork() are used for this.

A plugin is a DLL having special interface. Each
plugin must implement the initialization and termination routines:
InitClasses() and TermClasses(). A plugin is loaded to an application
dynamically using the global function LoadPlugin().
This function loads the DLL, attaches it to the address space of the
current process and calls InitClasses() in it.

The call TermFrameWork() causes calls of TermClasses() for all the
initialized plugins and their unloading.

See @ref base_plugin_usage.

@section base_overview_sync Processes and Threads, Synchronization

The class MPSync provides synchronization of processes.

The classes TSync, TAutoSync, TEvent, TEventSet are used for synchronization
of threads.

There are several global functions for support of multithreading:

- GetNumberOfProcesors().
- IntCreateThread(), IntCloseHandle(), IntResumeThread(), IntSuspendThread(),
IntWaitForSingleThread(), IntWaitForMultipleThreads().
- IntInitializeCriticalSection(), IntDeleteCriticalSection(), 
IntEnterCriticalSection(), IntLeaveCriticalSection().
- IntCreateEvent(), IntResetEvent(), IntSetEvent(),
IntWaitForSingleEvent(), IntWaitForMultipleEvents().

@section base_overview_guid Guid

Guid object presents Globally Unique Identifier. It provides assignment and
comparing operations, serialization and conversion to the string.

**/
