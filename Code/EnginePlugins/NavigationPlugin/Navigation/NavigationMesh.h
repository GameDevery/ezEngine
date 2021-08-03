#pragma once

#include <Foundation/Containers/Set.h>
#include <NavigationPlugin/Navigation/Navigation.h>
#include <NavigationPlugin/NavigationPluginDLL.h>
#include <Recast/DetourNavMesh.h>
#include <Recast/DetourNavMeshQuery.h>
#include <Recast/Recast.h>


class EZ_NAVIGATIONPLUGIN_DLL ezNavigationMesh
{
public:
  ezNavigationMesh();
  ~ezNavigationMesh();
  bool Init(float cellTileSize);
  bool LoadTile(ezUInt8* navData, int navDataLength);
  bool RemoveTile(ezNavigation::Point tileCoordinate);
  void FindPath(const ezNavigation::PathFindQuery& query, ezNavigation::PathFindResult* result);
  void Raycast(const ezNavigation::RaycastQuery& query, ezNavigation::RaycastResult* result);

private:
  dtNavMesh* m_navMesh = nullptr;
  dtNavMeshQuery* m_navQuery = nullptr;
  ezSet<dtTileRef> m_tileRefs;
};
