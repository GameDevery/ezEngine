#pragma once

#include <Foundation/Strings/String.h>
#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Foundation/Algorithm/HashableStruct.h>
#include <NavigationPlugin/Navigation/NavigationMeshBuildSettings.h>
#include <NavigationPlugin/Navigation/NavigationMeshGroup.h>

//\brief Settings for the dynamic navigation mesh builder
struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationSettings : public ezHashableStruct<ezNavigationSettings>
{
  //\brief If set to <c>true</c>, navigation meshes will be built at runtime. This allows for scene streaming and dynamic obstacles
  /// Enable dynamic navigation on navigation components that have no assigned navigation mesh
  bool EnableDynamicNavigationMesh;

  // todo: collisionfiltergroupflags
  // ezCollisionFilterGroupFlags IncludedCollisionGroups; // ezCollisionFilterConfig

  /// Build settings used by Recast
  /// Advanced settings for dynamically-generated navigation meshes
  ezNavigationMeshBuildSettings BuildSettings;


  /// Settings for agents used with the dynamic navigation mesh
  /// The groups that use the dynamic navigation mesh
  ezDynamicArray<ezNavigationMeshGroup> Groups;
};
