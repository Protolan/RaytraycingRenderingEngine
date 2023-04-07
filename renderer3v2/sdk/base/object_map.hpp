/// @file
///
/// @brief Definition of ObjectMap (fast map of addresses to indices) template class.
///
/// Copyright &copy; INTEGRA, Inc., 2014.
///
/// @internal
/// @author Pnd - Nick Deryabin, '14.04.07

#ifndef _ELXX_OBJECT_MAP_HPP_
#define _ELXX_OBJECT_MAP_HPP_

#include "map_envelop.h"

INTEGRA_NAMESPACE_START

/// Fast map of objects to indices template class.
template <class OBJ_TYPE>
class ObjectMap
  {
  public:
    /// Constructor.
    ObjectMap();
    /// Destructor.
    ~ObjectMap();

  public:
    /// Add element to object map.
    OKAY AddObj(OBJ_TYPE *object, int index);
    /// Find element in object map.
    bool FindObj(OBJ_TYPE *object, int *p_index = NULL);

  private:
    /// Copy constructor (forbidden).
    ObjectMap(const ObjectMap<OBJ_TYPE> &src);

  private:
    /// STL map envelop.
    MapEnvelop m_map_env;

  };  // class ObjectMap

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
template <class OBJ_TYPE>
ObjectMap<OBJ_TYPE>::ObjectMap()
  {
  }  // ObjectMap()

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
template <class OBJ_TYPE>
ObjectMap<OBJ_TYPE>::~ObjectMap()
  {
  }  // ~ObjectMap()

//////////////////////////////////////////////////////////////////////////////
/// Add element to the map.
///
/// @param[in] object Object to add (not NULL).
/// @param[in] index Index of the object.
/// @return SUCCESS or FAILURE.
///
template <class OBJ_TYPE>
OKAY ObjectMap<OBJ_TYPE>::AddObj(OBJ_TYPE *object, int index)
  {
  return m_map_env.AddObj(object, index);
  }  // AddObj()

//////////////////////////////////////////////////////////////////////////////
/// Find element in the map.
///
/// @param[in] object Object to find (not NULL).
/// @param[out] p_index Index of the object found (optional; set when @c true is returned).
/// @return @c true, if the object was found, @c false otherwise.
///
template <class OBJ_TYPE>
bool ObjectMap<OBJ_TYPE>::FindObj(OBJ_TYPE *object, int *p_index)
  {
  return m_map_env.FindObj(object, p_index);
  }  // FindObj()

INTEGRA_NAMESPACE_END
#endif

