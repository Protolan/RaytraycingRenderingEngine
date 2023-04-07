/// @file
///
/// @brief GUID class
///
/// @internal
/// @author Pbb - Boris Barladian, KC2A, '05.11.03.
/// @author Pls - Lev Shapiro, KC2A, '05.11.03.
///
/// Copyright &copy; INTEGRA, Inc., 2005.


#ifndef _KC2A_GUID_HPP_
#define _KC2A_GUID_HPP_

#include "export.h"

INTEGRA_NAMESPACE_START

class Serializer;
class Str;

/// Globally Unique Identifier class.
class Guid
  {
  public:
    /// Constructor.
    INTAPI_BASE Guid();
    /// Copy constructor.
    INTAPI_BASE Guid(const Guid &src);
    /// Get guid value.
    INTAPI_BASE void Get(BYTE val[16]);
    /// Convert to string.
    INTAPI_BASE void ConvertToStr(Str &str);

    /// Equality operator.
    INTAPI_BASE bool operator == (const Guid &g) const;
    /// Inequality operator.
    INTAPI_BASE bool operator != (const Guid &g) const;
    /// Less operator.
    INTAPI_BASE bool operator < (const Guid &g) const;
    /// Greater operator.
    INTAPI_BASE bool operator > (const Guid &g) const;
    /// Assignment operator.
    INTAPI_BASE Guid & operator = (const Guid &g);
    /// Serialize Guid without default value.
    INTAPI_BASE void Serialize(Serializer &inout, const char *tag);

  private:
    /// Guid storage.
    BYTE guid[16];
  };  // class Guid

INTEGRA_NAMESPACE_END
#endif
