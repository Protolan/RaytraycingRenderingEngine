/// @file
///
/// @brief MapEnvelop - envelop of STL map class.
///
/// Copyright &copy; INTEGRA, Inc., 2014.
///
/// @internal
/// @author Pnd - Nick Deryabin, '14.04.07

#ifndef _ELxx_MAP_ENVELOP_H_
#define _ELxx_MAP_ENVELOP_H_

#include "export.h"

INTEGRA_NAMESPACE_START

/// Envelop of STL map (internal class).
class MapEnvelop
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE MapEnvelop();
    /// Destructor.
    INTAPI_BASE ~MapEnvelop();

  private:
    /// Copy constructor (forbidden).
    MapEnvelop(const MapEnvelop &src);

  public:
    /// Add element to the map.
    INTAPI_BASE OKAY AddObj(void *object, int index);
    /// Find element in the map.
    INTAPI_BASE bool FindObj(void *object, int *p_index = NULL);

  private:
    /// STL map pointer.
    void *m_pmap;

  };  // class MapEnvelop

INTEGRA_NAMESPACE_END

#endif

