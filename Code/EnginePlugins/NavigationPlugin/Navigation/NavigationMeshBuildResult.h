#pragma once

#include <NavigationPlugin/Navigation/NavigationMesh.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Foundation/Types/Uuid.h>

//\brief Information about what tiles changes after building a navigation mesh
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshLayerUpdateInfo
{
  //\ brief The id of the group
  ezUuid GroupId;

  //\brief Coordinates of the tiles that were updated
  ezDynamicArray<ezNavigation::Point> UpdatedTiles;
};

//\brief The result of building a navigation mesh
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshBuildResult
{
  bool Success = false;
  ezNavigationMesh pNavigationMesh;
  ezDynamicArray<ezNavigationMeshLayerUpdateInfo> UpdatedLayers;
};
