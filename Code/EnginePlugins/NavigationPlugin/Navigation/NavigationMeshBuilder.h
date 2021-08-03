#pragma once

#include <NavigationPlugin/NavigationPluginDLL.h>
#include <NavigationPlugin/Navigation/StaticColliderData.h>
#include <NavigationPlugin/Navigation/NavigationMeshBuildResult.h>
#include <NavigationPlugin/Navigation/NavigationMeshTile.h>
#include <Foundation/Containers/HashSet.h>

class ezNavicationMesh;

/// Incremental navigation mesh builder.
/// Builds the navigation mesh in individual tiles
class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMeshBuilder
{
public:
  ezNavigationMeshBuilder(ezNavicationMesh* pOldNavigationMesh);

  void Add(const ezStaticColliderData& colliderData);
  void Remove(const ezStaticColliderData& colliderData);

  ezNavigationMeshBuildResult Build(const ezNavigationMeshBuildSettings& buildSettings,
    const ezDynamicArray<ezNavigationMeshGroup>& groups,
    ezUInt32 includedCollisionGroups,
    const ezDynamicArray<ezBoundingBox>& boundingBoxes);

  private:
  ezNavigationMeshTile* BuildTile(const ezNavigation::Point& tileCoordinate,
    const ezNavigationMeshBuildSettings& buildSettings,
    const ezNavigationAgentSettings& agentSettings,
    const ezDynamicArray<ezBoundingBox>& boundingBoxes, const ezDynamicArray<ezVec3>& inputVertices,
    const ezDynamicArray<ezInt32>& inputIndices,
    long buildTimeStamp);

  void BuildInput(const ezDynamicArray<ezStaticColliderData>& collidersLocal, ezUInt32 includedCollisionGroups);

  void MarkTiles(const ezNavigationMeshInputBuilder& inputBuilder,
    const ezNavigationMeshBuildSettings& buildSettings,
    const ezNavigationAgentSettings& agentSettings,
    const ezHashSet<ezNavigation::Point>& tilesToBuild);

  ezNavigationMeshInputBuilder BuildPlaneGeometry(const ezPlane& plane, const ezBoundingBox& boundingBox);

private:
  ezNavicationMesh* pOldNavigationMesh;
  ezDynamicArray<ezStaticColliderData> colliders;
  ezHashSet<ezUuid> registeredGuids;
};
