/// @internal
/// @file
///
/// @brief Non-abstract methods of Serializer class.
///
/// @author Pnd - Nickolay Derjabin, '00.12.25
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004

#include <integra.hpp>

#include "serializer.hpp"

INTEGRA_NAMESPACE_START

/**
@class Serializer base/serializer.hpp

Abstract interface for data serializers.

The purpose of serializers is to import or export data to some linear media.
Integra Object Framework use the Serializer abstract interface to this end.
Serializers allow for plugins to read / write, or load / save data
in an easy way.

Serializer is an abstract class. Particular Serializer implementations can
either import (read) or export (write) plugin data from or to some media.

@note The data parameter is always passed by reference and may be
as IN as OUT depending on particular serializer.

@sa @ref base_mainpage

@internal
The abstract class maintains only three flags for serialization direction
and error procession. It implements high level serialization calls  leaving
lower level serialization method implementation for the particular derived
classes.
**/
  
//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// The method constructs a Serializer object
/// (common abstract part of all serializers).
/// @param[in] imp - Serialization direction: true for import, false for export.
Serializer::Serializer(bool imp)
  {
  import = imp;
  format_error = false;
  alloc_error  = false;
  data_version = import ? 0 : REPOSITORY_VERSION;
  io_flags = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for an allocation error.
///
/// The method checks if a memory allocation error (lack of memory)
/// occurred while serialization.
/// @return YES if a memory allocation error occurred during
/// serialization, NO otherwise.
bool Serializer::AllocError()
  {
  return alloc_error;  // bad data
  }

//////////////////////////////////////////////////////////////////////////////
/// Clear an allocation error.
///
/// The method clears the memory allocation error flag.
void Serializer::ClearAllocError()
  {
  alloc_error = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Clear a format error.
///
/// The method clears the format error flag.
void Serializer::ClearFormatError()
  {
  format_error = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set a format error.
///
/// The method sets the error flag indicating that an error in serialized
/// data was found while serialization.
/// Useful to debug problems with bad format.
void Serializer::SetFormatError()
  {
  // It does not result in real error for Meshes in old IL files
  //Assert(false);
  format_error = true;  // bad data
  }

//////////////////////////////////////////////////////////////////////////////
/// Set an allocation error.
///
/// The method sets an error flag indicating a memory allocation error.
/// May be set by the user.
void Serializer::SetAllocError()
  {
  alloc_error = true;  // bad data
  }

//////////////////////////////////////////////////////////////////////////////
/// @brief High-level serialization of variables of basic types.
///
/// High-level serialization of variables of basic types.
///
/// Serialize scalar of particular type providing a chunk for it.
/// When reading, pre-sets default value for sure.
/// When writing, do not write the value if it coincides with the default one.
/// @param[in] TYPE - Type of scalar to serialize.
/// @param[in] DEF_TYPE - Type of default value for this type.
#define SERIALIZE_SCALAR(TYPE, DEF_TYPE)                                     \
void Serializer::Serialize(const char *tag, TYPE &var, DEF_TYPE dval)        \
  {                                                                          \
  BegChunk(tag);                                                             \
  if (Import())                                                              \
    var = (TYPE)dval;                                                        \
  if (Import() || (var != dval))                                             \
    Value(var);                                                              \
  EndChunk();                                                                \
  }

// Serialization methods for particular scalars
SERIALIZE_SCALAR(bool, bool)
SERIALIZE_SCALAR(char, int)
SERIALIZE_SCALAR(short, int)
SERIALIZE_SCALAR(int, int)
SERIALIZE_SCALAR(INT64, INT64)
SERIALIZE_SCALAR(BYTE, unsigned)
SERIALIZE_SCALAR(WORD, unsigned)
SERIALIZE_SCALAR(unsigned, unsigned)
SERIALIZE_SCALAR(float, double)
SERIALIZE_SCALAR(double, double)

#undef SERIALIZE_SCALAR

/**
@fn void Serializer::Serialize(const char *tag, bool &var, bool dval)

The method reads or writes a data chunk containing a value of a basic type.
Besides the value, the method arguments specify the value tag, and,
optionally, the default value.

Each chunk is logically identified by the chunk's tag.
The tag can appear or not in the serialization media dependent on
particular serializer.

Default values allow for serializers do not write to a serialization
media most often used values (thus saving media space).
If the value to export coincides with the default one, serializer simply
does not write the correspondent chunk to the serailization media.
On import, the chunk presence is checked, and, if the chunk is absent,
the variable is assigned the default value.

@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 

Note that the variable is always passed by reference and may be IN or OUT
depending on particular serialization direction. 

**/

/**
@overload void Serializer::Serialize(const char *tag, char &var, int dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, short &var, int dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, int &var, int dval = 0);
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, INT64 &var, INT64 dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, BYTE &var, unsigned dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, WORD &var, unsigned dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, unsigned &var, unsigned dval = 0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, float &var, double dval = 0.0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

/**
@overload void Serializer::Serialize(const char *tag, double &var, double dval = 0.0)
@param[in] tag The chunk's tag.
@param[in, out] var The variable whose value must be imported or exported.
@param[in] dval The default value for the variable. 
**/

//////////////////////////////////////////////////////////////////////////////
/// Serialize pointer to EntityType.
///
/// @param[in] tag - The chunk's tag.
/// @param[in, out] var - The variable whose value must be imported or exported.
void Serializer::Serialize(const char *tag, EntityType *&var)
  {
  BegChunk(tag);
  Value(var);
  EndChunk();
  }  // Serialize(const char *tag, EntityType *&var)

//////////////////////////////////////////////////////////////////////////////
/// Process obsolete chunks.
///
/// The order of Serialize() calls in the object's serialization method
/// is important for future backward compatibility.
/// Once written in a release version, the particular serialization call
/// may not be later removed, or moved to another place.
/// Neither a serialized variable type, nor the chunk's tag, nor the default
/// value may be changed. However, the chunk may be explicitly marked as
/// obsolete by replacing (at the same place!) the chunk's serialization
/// call (or the chunk's serialization sequence) by the call of Obsolete()
/// The method with the same chunk's tag.
///
/// This method informs the serializer that former versions of Integra
/// software would use here a chunk with given name that is no longer
/// supported and may be absent in the serialization media.
/// @param[in] tag - The obsolete chunk's tag.
void Serializer::Obsolete(const char *tag)
  {
  BegChunk(tag);
  IgnoreData();
  EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for an expected data type.
///
/// It is an escape feature to recover fatal bugs related with
/// incompatibility of IOF files.
///
/// This feature must not be used except for fatal bug recovery.
///
/// The method may be called whenever a lower level Value() call is expected.
/// It checks whether the input file contains data of the given type
/// for the current chunk.
/// 
/// @param[in] type - Type of the data chunk.
/// @return true, if on import next expected chunk exists and
/// contains data of specified type, false otherwise.
/// The default implementation returns false.
/// @internal
/// The parameter @a type uses internal coding defined by IOFSerializer 
/// (base/iof/iof.hxx) class enumerations for data type.
/// Examples: INT_TYPE int type, DOUBLE_TYPE for double type.
bool Serializer::CheckValueType(int type)
  {
  return false;
  }

//////////////////////////////////////////////////////////////////////////////
// Documentation for pure virtual methods

/**
@fn void Serializer::BegChunk(const char *tag)

Begin a complex sequence - a chunk.

The Method starts a new chunk serialization.
It is typically followed by Value() calls to serialize particular data,
and should be closed by the EndChunk() call.

@param[in] tag - The chunk's tag.
**/

/**
@fn void Serializer::EndChunk()

End a complex sequence - a chunk.

The method ends chunk serialization.

On writing, if there were no data written since BegChunk(),
the chunk is not actually written to the serialization media.
**/

/**
@fn int Serializer::NChunk()

Get number of chunk being serialized.

Serializer counts the chunks of the same level and may use the successive
number of the chunk for the chunk identification.
This method allows the user to check the chunk numbering.

Normally the programmer has no need in this method.

@return A successive number of the chunk (after BegChunk()) at the current level.
**/

/**
@fn void Serializer::Value(bool &var)

The method reads or writes raw values of a basic type.
It may be used between BegChunk() and EndChunk().

On reading, the data is read only if the chunk for it was found.

@param[in, out] var A variable whose value should be exported or imported.

Note that the variable is always passed by reference and may be
IN or OUT dependent on particular serialization direction.

**/

/**
@overload void Serializer::Value(char &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(short &var)
@param[in, out] var - A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(int &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(INT64 &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(BYTE &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(WORD &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(unsigned &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(float &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@overload void Serializer::Value(double &var)
@param[in, out] var A variable whose value should be exported or imported.
**/

/**
@fn void Serializer::Value(bool *arr, SIZE_T n)

The method reads or writes a number of successive values of a basic type.
It may be used between BegChunk() and EndChunk().

On reading, the data is read only if the chunk for it was found.

@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.

**/

/**
@overload void Serializer::Value(char     *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(short    *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(int      *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(INT64    *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(BYTE     *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(WORD     *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(unsigned *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(float    *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@overload void Serializer::Value(double   *arr, SIZE_T n)
@param[in, out] arr - A pointer to the first value to serialize (import or export).
@param[in]  n - The number of (successive) values to serialize.
**/

/**
@fn void Serializer::Value(EntityType *&var)

The method serializes Pointers to entity types.
It may be used between BegChunk() and EndChunk().

@param[in, out] var - A variable whose value should be imported or
exported.

Note that the variable is always passed by reference and may be
IN or OUT dependent on particular serialization direction. 

**/

/**
@fn void Serializer::Value(Str &var)

The method serializes the string data type.
It may be used between BegChunk() and EndChunk().

@param[in, out] var A variable whose value should be imported or exported.

Note that the variable is always passed by reference and may be
IN or OUT dependent on particular serialization direction. 

**/

/**
@overload void Serializer::Value(UStr &var)
@param[in, out] var A variable whose value should be imported or exported.
**/

/**
@overload void Serializer::Value(PathStr &var)
@param[in, out] var A variable whose value should be imported or exported.
**/

/**
@fn void Serializer::Value(PlugRef &var)

Serialize a reference.

The method serializes a plug reference. Default is no reference.

@param[in, out] var - A plug reference to import or export.
Note that the variable is always passed by (C++) reference and may be
IN or OUT dependent on particular serialization direction. 
**/

/**
@fn void Serializer::IgnoreData()

Ignore data for the current chunk.

This method is used between BegChunk() and EndChunk() calls to effectively
skip the chunk's body. It is necessary to implement the Obsolete() method.

Each particular serializer must implement this method.
**/

INTEGRA_NAMESPACE_END
