/// @file
///
/// @brief Definition of Guid class
///
/// @internal
/// @author Pbb - Boris Barladian, KC2A, '05.08.01.
/// @author Pls - Lev Shapiro, KC2A, '05.08.01.
///
/// Copyright &copy; INTEGRA, Inc., 2005.

#ifdef _WIN32
#include <rpc.h>
#endif

#include <integra.hpp>


#include "str.hpp"
#include "serializer.hpp"
#include "guid.hpp"


INTEGRA_NAMESPACE_START

/**
@class Guid base/guid.hpp

Class Guid (Globally Unique Identifier class) is used for unique
objects identification. For creation (default constructor)
it uses UuidCreate() method from PlatformSDK.

**/


//////////////////////////////////////////////////////////////////////////////
/// Constructor.
Guid::Guid()
  {
#ifdef _WIN32
  RPC_STATUS ret = RPC_S_OBJECT_NOT_FOUND;
  ret = UuidCreate((UUID *)guid);
  Assert(RPC_S_OK == ret);
#else
  Assert(false);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] src Source class.
Guid::Guid(const Guid &src)
  {
  memcpy(guid, src.guid, 16);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get guid value.
/// @param[out] val Output storage.
void Guid::Get(BYTE val[16])
  {
  memcpy(val, guid, 16);
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert to string.
/// @param[out] str Output sting.
void Guid::ConvertToStr(Str &str)
  {
#ifdef _WIN32
  unsigned char *str_tmp = NULL;
#if 0
  RPC_STATUS ret = UuidToString((UUID *)guid, &str_tmp);
  Assert(RPC_S_OK == ret);
#endif
  UuidToString((UUID *)guid, &str_tmp);
  str = (char *)str_tmp;
  RpcStringFree(&str_tmp);
#else
  Assert(false);
#endif
  }

//////////////////////////////////////////////////////////////////////////////
/// Equality operator.
/// @param[in] src Class to compare with.
/// @return true in equality case.
bool Guid::operator == (const Guid &src) const
  {
  return (memcmp(guid, src.guid, 16) == 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Inequality operator.
/// @param[in] src Class to compare with.
/// @return true in inequality case.
bool Guid::operator != (const Guid &src) const
  {
  return (memcmp(guid, src.guid, 16) != 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Less operator.
/// @param[in] src Class to compare with.
/// @return Result of the comparison.
bool Guid::operator < (const Guid &src) const
  {
  return (memcmp(guid, src.guid, 16) < 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Greater operator.
/// @param[in] src Class to compare with.
/// @return Result of the comparison.
bool Guid::operator > (const Guid &src) const
  {
  return (memcmp(guid, src.guid, 16) > 0);
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] src Source.
/// @return Reference to this class.
Guid & Guid::operator = (const Guid &src)
  {
  memcpy(guid, src.guid, 16);
  return *this;
  }


//////////////////////////////////////////////////////////////////////////////
/// Serialize Guid without default value.
/// @param[in, out] inout Serialization medium.
/// @param[in] tag Tag name.
void Guid::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  inout.Value((BYTE *)guid, 16 * (int)sizeof(BYTE));
  inout.EndChunk();
  }

INTEGRA_NAMESPACE_END
