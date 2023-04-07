/// @file
///
/// @brief MapEnvelop class definition.
///
/// Copyright &copy; INTEGRA, Inc., 2014.
///
/// @internal
/// @author Pnd - Nick Deryabin, '14.04.07

// Use release CRT
#undef _DEBUG

#include <map>
#include <integra.hpp>

#include "map_envelop.h"
#include "user.hpp"

using namespace Integra;

/**

@class MapEnvelop base/map_envelop.h

It is internal class to envelop STL map functionality.

**/

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
MapEnvelop::MapEnvelop()
  {
  try
    {
    m_pmap = new std::map<void *, int>;
    Assert(m_pmap != NULL);
    }
  catch(...)
    {
    m_pmap = NULL;
    }
  }  // MapEnvelop()

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
MapEnvelop::~MapEnvelop()
  {
  try
    {
    delete m_pmap;
    }
  catch(...)
    {
    }
  }  // ~MapEnvelop()

//////////////////////////////////////////////////////////////////////////////
/// Add element to the map.
///
/// @param[in] object Object to add (not NULL).
/// @param[in] index Index of the object.
/// @return SUCCESS or FAILURE.
///
OKAY MapEnvelop::AddObj(void *object, int index)
  {
  Assert(m_pmap != NULL);
  if (m_pmap == NULL)
    return User()->ErrorNoMemory();
  try
    {
    std::map<void *, int> &map = *static_cast<std::map<void *, int> *>(m_pmap);
    map[object] = index;
    return SUCCESS;
    }
  catch(...)
    {
    return User()->ErrorNoMemory();
    }
  }  // AddObj()

//////////////////////////////////////////////////////////////////////////////
/// Find element in the map.
///
/// @param[in] object Object to find (not NULL).
/// @param[out] p_index Index of the object found (optional; set when @c true is returned).
/// @return @c true, if the object was found, @c false otherwise.
///
bool MapEnvelop::FindObj(void *object, int *p_index)
  {
  Assert(m_pmap != NULL);
  try
    {
    std::map<void *, int> &map = *static_cast<std::map<void *, int> *>(m_pmap);
    const std::map<void *, int>::iterator it = map.find(object);
    if (it == map.end())
      return false;
    if (p_index != NULL)
      *p_index = it->second;
    return true;
    }
  catch(...)
    {
    User()->ErrorNoMemory();
    return false;
    }
  }  // FindObj()

