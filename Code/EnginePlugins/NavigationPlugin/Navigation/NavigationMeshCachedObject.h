#pragma once

#include <NavigationPlugin/Navigation/NavigationMeshInputBuilder.h>
#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

//\brief Represents cached data for a static collider component on an entity
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshCachedObject
{
  //Unique id of the collider
  ezUInt32 Id;

  // Hash obtained with <see cref="NavigationMeshBuildUtils.HashEntityCollider"/>
  ezInt32 ParameterHash;

  //Cached vertex data
  const ezNavigationMeshInputBuilder& InputBuilder;

  // List of infinite planes contained on this object
  const ezDynamicArray<ezPlane>& Planes;
};
