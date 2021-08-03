#pragma once

#include <NavigationPlugin/Navigation/NavigationMeshInputBuilder.h>
#include <NavigationPlugin/Navigation/NavigationMeshCachedObject.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

//\brief Data associated with static colliders for incremental building of navigation meshes
struct EZ_NAVIGATIONPLUGIN_DLL ezStaticColliderData
{
  // todo : StaticColliderComponent Component
  int ParameterHash = 0;
  bool Processed = false;
  ezNavigationMeshInputBuilder& InputBuilder;
  ezNavigationMeshCachedObject& Previous;

  // Planes are an exceptions to normal geometry since their size depends on the size of the bounding boxes in the scene,
  // however we don't want to rebuild the whole scene, unless the actual shape of the plane changes
  ezDynamicArray<ezPlane> Planes;
};
