#pragma once

#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/Navigation/NavigationMeshCachedObject.h>
#include <NavigationPlugin/Navigation/NavigationMeshInputBuilder.h>
#include <NavigationPlugin/Navigation/StaticColliderData.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

//\brief Holds the cached result of building a scene into a navigation mesh, with input vertex data to allow incremental builds.
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshCache
{
  // State of static colliders by their component Id that was used for building
  ezDynamicArray<ezNavigationMeshCachedObject> Objects;

  // The bounding boxes used for build
  ezDynamicArray<ezBoundingBoxd> BoundingBoxes;

  // Hash for the building settings used
  ezInt32 SettingsHash = 0;

  void Add(const ezStaticColliderComponent& collider, const ezNavigationMeshInputBuilder& data, const ezDynamicArray<ezPlane>& planes, ezInt32 entityColliderHash);
};
